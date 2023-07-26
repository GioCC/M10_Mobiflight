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

BBcallback  ButtonBas::_OnRelease = nullptr;
BBcallback  ButtonBas::_OnPress = nullptr;


ButtonBas::ButtonBas(
    uint8_t     pin,
    uint8_t     useHWinput,
    char*       name,
    uint8_t     lthreshold,
    uint8_t     uthreshold,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(pin, useHWinput, name, mirrorvar, mirrorbit),
pressFlag(0), lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonBas(lthreshold, uthreshold);
}

ButtonBas::ButtonBas(
    uint8_t     pin,
    uint8_t     useHWinput,
    uint16_t    code,
    uint8_t     lthreshold,
    uint8_t     uthreshold,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(pin, useHWinput, code, mirrorvar, mirrorbit),
pressFlag(0), lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonBas(lthreshold, uthreshold);
}

void
ButtonBas::CButtonBas(uint8_t lthreshold, uint8_t uthreshold)
{
    debounceTime = 100;
    modeAnalog(lthreshold != uthreshold);
    TlastChange = millis();
    valBit(0);
}

uint8_t
ButtonBas::_getInput(Button::ButtonStatus_t ival)
{
    uint8_t newi;
    uint8_t ana = (_flags & Button::Analog);

    if(_flags & Button::HWinput) {
        newi = (ana ?
                 ((analogRead(_pin)+2)>>2)    // Scale ADC value from 10 to 8 bits
               : !digitalRead(_pin) );
    } else if(hasSrcVar()) {
        newi = getSrcVal();
    } else {
        newi = (ana ? (uint8_t)ival : (ival & Button::Curr));
    }
    
    if (ana) {
        bool lowt  = (ival >= lowerAnaThrs);
        bool hight = (ival <  upperAnaThrs);
        newi = (lowerAnaThrs < upperAnaThrs) ? (lowt && hight) : (lowt || hight);
    }

    newi = (newi ? HIGH : LOW);
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
    register uint8_t ival = bytevec[((_pin-1)>>3)];
    ival = ((ival&(1<<((_pin-1)&0x07))) ? HIGH : LOW);
    _check(ival);
}

void
ButtonBas::_check(uint8_t newi)
{
    unsigned long now;
    uint8_t curi = ((_flags & Button::lastState) ? HIGH : LOW);

    now = millis();
    if (newi != curi) {
        // button state changed
        if(TlastChange == 0) TlastChange = now;
        if (now - TlastChange >= debounceTime) {
            TlastChange = 0;    // this condition means "stable"
            // Register new status
            valBit(newi == HIGH);
            curi = newi;
        }
    }

    if (curi == HIGH) {
        if (!pressFlag) {
            pressFlag = 1;
            if (_OnPress) {
                _OnPress(this);
                // callback may have taken some time: update <now> for <if>s below
                now = millis();
            }
        }
    } else {
        if (pressFlag) {
            if (_OnRelease) _OnRelease(this);
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
    register uint8_t ival = bytevec[((_pin-1)>>3)];
    ival = ((ival&(1<<((_pin-1)&0x07))) ? HIGH : LOW);
    _initState(ival);
}

void
ButtonBas::_initState(uint8_t newi)
{
    valBit(newi == HIGH);
    if (newi == HIGH) {
        if (_OnPress) _OnPress(this);
    } else {
        if (_OnRelease) _OnRelease(this);
    }
}

// end ButtonBas.cpp
