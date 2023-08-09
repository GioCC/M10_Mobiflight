// =======================================================================
// @file        ButtonBas.cpp
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-09 12:47
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


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
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
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
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
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

bool
ButtonBas::ana2dig(uint8_t val)
{
    bool lowt  = (val >= lowerAnaThrs);
    bool hight = (val <  upperAnaThrs);
    return (lowerAnaThrs < upperAnaThrs) ? (lowt && hight) : (lowt || hight);
}

void
ButtonBas::process(uint8_t sts)
{
    valBit(((sts & Button::Curr) != 0));

    if ((sts & Button::Dn) && _OnPress) _OnPress(this);
    if ((sts & Button::Up) && _OnRelease) _OnRelease(this);
}

void
ButtonBas::check(bool force)
{
    checkVal(getInput(), force);
}

void
ButtonBas::checkVec(uint8_t *bytevec, bool force)
{
    if(isHW() || isAna() || hasSrcVar()) return;
    uint8_t res = bytevec[((_pin-1)>>3)];
    res = ((res & (1<<((_pin-1)&0x07))) ? HIGH : LOW);
    checkVal(res, force);
}

void
ButtonBas::checkVal(uint8_t val, bool force)
{
    unsigned long now = millis();
    uint8_t res;

    uint8_t curi = ((_flags & Button::lastState) ? HIGH : LOW);
    uint8_t newi = (((_flags & Button::Analog) ? ana2dig(val) : val) ? HIGH : LOW);

    // Debounce strategy: Steady state
    // Input is validated only if it's been stable for a debouncing time interval
    // Safe but slightly delayed (and further delayed, up to insensitive, if input is very noisy)

    if (newi != curi) {
        // button state changed
        TlastChange = now;
    }

    if (force || (TlastChange != 0 && ((now - TlastChange) >= debounceTime))) {
        TlastChange = 0;    // Prevent further triggers until next change
        
        res = (newi == HIGH ? (Button::Dn|Button::High) : (Button::Up|Button::Low));
        process(res);
    }
}

// end ButtonBas.cpp
