/*
* File     : ButtonBas.cpp
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the ButtonBas+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This library allows to conveniently define pushbutton actions with callbacks for several events.
* It is meant to work jointly with a ButtonGroupManager, which in turn receives (and passes along)
* input flags supplied by an underlaying I/O reader (>>> digital buttons only!!! <<<).
*
* The ButtonBas receives a set of I/O flags describing the status of its associated input or 'pin'
* (for current status, up/dn transitions etc - see doc) and invokes callback functions accordingly.
*
* This file defines the ButtonBas class.
*
* Usage:
* - Include ButtonBas.h and ButtonGroupManager.h in your sketch
* - Add a call to ButtonGrpManager.checkButtons() in your main loop
* - Declare each button and define the events using a ButtonBas constructor
* - Declare the required event functions ( void OnKeyXXX(ButtonBas* but) )
* - See the comments in the code for more help
*/

#include "ButtonBas.h"

ButtonBas::ButtonBas(uint8_t    pin,
                    uint8_t     useHWinput,
                    char        *name,
                    BBcallback  OnPress,
                    BBcallback  OnRelease,
                    uint8_t     lthreshold,
                    uint8_t     uthreshold,
                    uint8_t     *mirrorvar,
                    uint8_t     mirrorbit
                    )
: Button(pin, useHWinput, name, mirrorvar, mirrorbit),
_OnPress(OnPress), _OnRelease(OnRelease),
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonBas(lthreshold, uthreshold);
}

ButtonBas::ButtonBas(uint8_t    pin,
                    uint8_t     useHWinput,
                    uint16_t    codeh,
                    uint16_t    codel,
                    BBcallback  OnPress,
                    BBcallback  OnRelease,
                    uint8_t     lthreshold,
                    uint8_t     uthreshold,
                    uint8_t     *mirrorvar,
                    uint8_t     mirrorbit
                    )
: Button(pin, useHWinput, codeh, codel, mirrorvar, mirrorbit),
_OnPress(OnPress), _OnRelease(OnRelease),
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonBas(lthreshold, uthreshold);
}

void
ButtonBas::CButtonBas(uint8_t lthreshold, uint8_t uthreshold)
{
    debounceTime = 100;
    flagChg(flags, F_Analog, (lthreshold==uthreshold));
    TlastChange = millis();
    flagChg(flags, F_lastState, 0);
}

uint8_t
ButtonBas::_getInput(Button::ButtonStatus_t ival)
{
    uint8_t newi;
    uint8_t ana = (flags & F_Analog);

    if(flags & F_HWinput) {
        newi = (ana
              ? ((analogRead(pin)+2)>>2)    // Scale ADC value from 10 to 8 bits
              : !digitalRead(pin) );
    } else if(hasSrcVar()) {
        newi = getSrcVal();
    } else {
        newi = (ana ? (uint8_t)ival : (ival & S_Curr));
    }
    if (ana) {
        newi = ((newi >= lowerAnaThrs && newi < upperAnaThrs) ? HIGH : LOW);
    } else {
        newi = (newi ? HIGH : LOW);
    }
    return newi;
}


void
ButtonBas::check(Button::ButtonStatus_t ival)
{
    _check(_getInput(ival));
}

void
ButtonBas::check(uint8_t *bytevec)
{
    register uint8_t ival = bytevec[((pin-1)>>3)];
    ival = ((ival&(1<<((pin-1)&0x07))) ? HIGH : LOW);
    _check(ival);
}

void
ButtonBas::_check(uint8_t newi)
{
    unsigned long now;
    uint8_t curi = ((flags & F_lastState) ? HIGH : LOW);

    now = millis();
    if (newi != curi) {
        // button state changed
        if(TlastChange == 0) TlastChange = now;
        if (now - TlastChange >= debounceTime) {
            TlastChange = 0;    // this condition means "stable"
            // Register new status
            if (curi == HIGH) { flags |= F_lastState; } else { flags &= ~F_lastState; }
            curi = newi;
        }
    }

    if (curi == HIGH) {
        if (!pressFlag) {
            pressFlag = 1;
            if (_OnPress != NULL) {
                _OnPress(this);
                now = millis();     // callback may have taken some time, resync
            }
        }
    } else {
        if (pressFlag) {
            if (_OnRelease != NULL) {
                _OnRelease(this);
            }
            pressFlag = 0;
        }
    }
}

void
ButtonBas::initState(Button::ButtonStatus_t ival)
{
    _initState(_getInput(ival));
}

void
ButtonBas::initState(uint8_t *bytevec)
{
    register uint8_t ival = bytevec[((pin-1)>>3)];
    ival = ((ival&(1<<((pin-1)&0x07))) ? HIGH : LOW);
    _initState(ival);
}

void
ButtonBas::_initState(uint8_t newi)
{
    if (newi == HIGH) {
        if (_OnPress != NULL) {
            _OnPress(this);
        }
    } else {
        if (_OnRelease != NULL) {
            _OnRelease(this);
        }
    }
}

