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

uint8_t ButtonBas::_ana2dig(uint8_t val)
{
    bool lowt  = (val >= lowerAnaThrs);
    bool hight = (val <  upperAnaThrs);
    return (lowerAnaThrs < upperAnaThrs) ? (lowt && hight) : (lowt || hight);
}

uint8_t
ButtonBas::_getInput(void)
{
    uint8_t res;
    uint8_t ana = isAna();
    uint8_t hw  = isHW();

    if(ana && hw) {
        res = ((analogRead(_pin)+2)>>2);    // Scale ADC value from 10 to 8 bits
    } else if(hw) {
        res = !digitalRead(_pin);
    } else if(hasSrcVar()) {
        res = getSrcVal();
    }
    if (ana) res = _ana2dig(res);
    return (res ? HIGH : LOW);
}

void 
ButtonBas::_check(uint8_t newi, uint8_t force)
{
    unsigned long now;
    uint8_t curi = ((_flags & Button::lastState) ? HIGH : LOW);
    newi = (newi ? HIGH : LOW);

    if (force || newi != curi) {
        // button state changed
        now = millis();
        if (force || (now - TlastChange) >= debounceTime) {
            TlastChange = now;
            valBit(newi == HIGH);   // Register new status
            curi = newi;
            if (curi == HIGH) {
                if (_OnPress) _OnPress(this);
            } else {
                if (_OnRelease) _OnRelease(this);
            }
        }
    }

}

void
ButtonBas::check(uint8_t force)
{
    _check(_getInput(), force);
}

void
ButtonBas::checkVal(Button::ButtonStatus_t val, uint8_t force)
{
    uint8_t newi;
    if (_flags & Button::Analog) {
        newi = _ana2dig(val.aVal);
    } else {
        newi = ((val.dVal & Button::lastState) ? HIGH : LOW);
    }
    _check(newi, force);
}

void
ButtonBas::check(uint8_t *bytevec, uint8_t force)
{
    if(isHW() || isAna() || hasSrcVar()) return;
    uint8_t res = bytevec[((_pin-1)>>3)];
    res = ((res & (1<<((_pin-1)&0x07))) ? HIGH : LOW);
    _check(res, force);
}

// end ButtonBas.cpp
