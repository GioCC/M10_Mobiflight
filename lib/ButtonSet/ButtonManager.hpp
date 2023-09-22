// =======================================================================
// @file        ButtonManager.hpp
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-09 18:08
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================

#include <stdlib.h>
#include "ButtonManager.h"

#define FORALL_b    for(byte b=0; b<NBANKS; b++)
#define vv          vec[b]

template<uint8_t MAXSIZE>
ButtonManager<MAXSIZE>::
ButtonManager(uint16_t lpDelay, uint16_t rptDelay, uint16_t rptRate)
: lastPress(0), lastChange(0), lastRepeat(0),
longPDelay(lpDelay), repeatDelay(rptDelay), repeatInterval(rptRate)
{
    debounceTime = 20;

    FORALL_b {
        vec[b]     = {0};
        buttons[b] = nullptr;
    }
    numButtons  = 0;
    currBut     = 0;
    analogVals  = NULL;
    nAnaVals    = 0;
}

template<uint8_t MAXSIZE>
uint8_t
ButtonManager<MAXSIZE>::
setAnalogSource(uint8_t *aVals, uint8_t nVals)
{
    analogVals  = NULL;
    nAnaVals    = 0;
    if(aVals == NULL || nVals==0) return false;
    analogVals = aVals;
    nAnaVals = nVals;
    return true;
}

// Set long pressure delay (in ms; rounded to nearest 100 ms; effective range 100ms..25.5s)
template<uint8_t MAXSIZE>
void
ButtonManager<MAXSIZE>::
setLongPDelay(uint16_t delay)
{
    if(delay > 25450) delay = 25450;
    longPDelay = (uint8_t)((delay+50)/100);    // rounded to the nearest 100ms
}

// Set start delay (in ms; rounded to nearest 100 ms; effective range 100ms..25.5s)
template<uint8_t MAXSIZE>
void
ButtonManager<MAXSIZE>::
setRepeatDelay(uint16_t delay)
{
    if(delay > 25450) delay = 25450;
    repeatDelay = (uint8_t)((delay+50)/100);    // rounded to the nearest 100ms
}

// Set repeat time (in ms; rounded to next 10 ms; effective range 10ms..2.55s)
template<uint8_t MAXSIZE>
void
ButtonManager<MAXSIZE>::
setRepeatRate(uint16_t rate)
{
    if(rate > 2541) rate = 2541;
    repeatInterval = (uint8_t)((rate+9)/10);       // rounded to the next 10ms
}

template<uint8_t MAXSIZE>
Button *
ButtonManager<MAXSIZE>::
add(Button* but)
{
    if (numButtons+1 < MAXSIZE) {
        numButtons++;
        buttons[numButtons-1]= but;
        return but;
    }
    return NULL;
}

template<uint8_t MAXSIZE>
Button *
ButtonManager<MAXSIZE>::
get(uint8_t nBut) {
    return ((nBut >= numButtons) ? ((nBut == 0xFF) ? buttons[currBut] : NULL) : buttons[nBut]);
}

template<uint8_t MAXSIZE>
Button *
ButtonManager<MAXSIZE>::
next(uint8_t nBut) {
    if(nBut != 0xFF) currBut = nBut;
    if(currBut >= numButtons) currBut = 0;
    return buttons[currBut++];
}

template<uint8_t MAXSIZE>
void
ButtonManager<MAXSIZE>::
initButtons(uint8_t *vecIO)
{
    lastChange = millis() + debounceTime + 1;
    FORALL_b {
        vv.LastIO = ~(vecIO[b]);    // mark last values as opposite of current in order to trigger change flag
    }
    _checkInit(vecIO, 1);
}

template<uint8_t MAXSIZE>
void
ButtonManager<MAXSIZE>::
checkButtons(uint8_t *vecIO)
{
    _checkInit(vecIO, 0);
}

template<uint8_t MAXSIZE>
void
ButtonManager<MAXSIZE>::
_checkInit(uint8_t *vecIO, uint8_t doinit)
{
    unsigned long now;
    uint8_t chg = 0;

    now = millis();

    FORALL_b {
        vv.Down     = 0;
        vv.Up       = 0;
        vv.Change   = 0;
        vv.Repeat   = 0;
        vv.LongP    = 0;
    }
    
    // Check if anything changed (shortcut for speed in most passes)
    FORALL_b {
        if (vecIO[b] != vv.LastIO) {
            chg = 1; break;
        }
    }

    if (chg) {
        if(lastChange == 0) lastChange = now;
        if (now - lastChange >= debounceTime) {
            lastPress = lastChange; // this also includes debounce time
            // lastPress = now;       // this doesn't
            lastRepeat = 0;         // Changes do reset the repeat sequence
            lastChange = 0;
            FORALL_b {
                uint8_t vIO = vecIO[b];
                vv.Change   = (vIO^vv.LastIO);
                vv.Up       = vv.Change&(vIO);
                vv.Down     = vv.Change&(~vIO);
                vv.LastIO   = vIO;
                vv.LPflag  &= (~vIO);    // Reset LP flag for inputs gone low
            }
        }
    } else {
        lastChange = 0;
    }

#ifdef BM_STRAIGHT
    // is the startdelay passed?
    if ((repeatInterval) && (now >= lastPress + times100(repeatDelay))) {
        if ((now - lastRepeat) >= (unsigned long)times10(repeatInterval)) {
            // Mark active inputs for repeat
            // This could be a single flag for all; however, marking all individual repeats
            // could allow every single button to handle its repeat flag at its own time
            // (requires clearing access to vecRepeat though).
            // As a side perk, repeat is already set for active buttons only.
            FORALL_b {
                vv.Repeat = vecIO[b];
            }
            lastRepeat = now;
        }
    }

    if ((longPDelay) && (now >= lastPress + times100(longPDelay))) {
        // Mark active inputs for long press
        FORALL_b {
            vv.LongP = vecIO[b] & (~vv.LPflag);    // Activate LP event for active inputs which haven't had LP yet
            vv.LPflag |= vecIO[b];                 // Mask them out from next LP activations
        }
    }
#endif
    {
        uint8_t sts;
        byte pin;
        for (int i=0; i< numButtons; i++) {
            // Setup values (where not done implicitly) for each button
            pin = (buttons[i]->getPin())-1;
            if(buttons[i]->isHW()||buttons[i]->hasSrcVar()) {
                // Button bound to HW pin or memory-(var-)based
                // Nothing to do: the object fetches its value by itself
                // The value for sts is dummy.
            } else if(buttons[i]->isAna()) {
                sts = 0;
                if(pin < nAnaVals) {
                    sts = buttons[i]->ana2dig(analogVals[pin]);
                }
            } else {
                sts = 0;
                byte bnk = pin>>3;
                byte msk = (0x01 << (pin&0x07));
                if(vecIO[bnk]       & msk) sts |= Button::Curr;
                if(vec[bnk].Down    & msk) sts |= Button::Dn;
                if(vec[bnk].Up      & msk) sts |= Button::Up;
                if(vec[bnk].Repeat  & msk) sts |= Button::Rpt;
                if(vec[bnk].LongP   & msk) sts |= Button::Long;
            }

            // Let each button check its state and trigger its own action
            (doinit ? buttons[i]->initState(sts) : buttons[i]->process(sts));
        }
    }
    FORALL_b {
        vv.Repeat = 0;  // repeat presses should have been handled just above.
        vv.LongP = 0;   // same for long presses
    }
}

// end ButtonManager.hpp