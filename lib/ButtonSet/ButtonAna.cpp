// =======================================================================
// @file        ButtonAna.cpp
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-09 12:47
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================

#include "ButtonAna.h"

ABcallback   ButtonAna::_OnPress = nullptr;
ABcallback   ButtonAna::_OnRelease = nullptr;

ButtonAna::ButtonAna(
    uint8_t     npin,
    char*       name,
    uint8_t     lthreshold,
    uint8_t     uthreshold,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(npin, 0, name, mirrorvar, mirrorbit),
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonAna();
}

ButtonAna::ButtonAna(
    uint8_t     npin,
    uint16_t    code,
    uint8_t     lthreshold,
    uint8_t     uthreshold,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(npin, 0, code, mirrorvar, mirrorbit),
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonAna();
}

void
ButtonAna::CButtonAna(void)
{
    hysteresis = 2;
    TlastChange = millis();
    valBit(0);
    if(lowerAnaThrs==upperAnaThrs) {
        // Force thresholds: they can't be the same, otherwise it isn't an Analog button
        lowerAnaThrs = 128;
        upperAnaThrs = 255;
    }
    modeAnalog(1);
}

bool
ButtonAna::ana2dig(uint8_t val)
{
    int8_t  hyst = ((_flags & Button::lastState) ? hysteresis : -hysteresis);
    bool lowt  = (val >= lowerAnaThrs - hyst);
    bool hight = (val <  upperAnaThrs + hyst);
    return (lowerAnaThrs < upperAnaThrs) ? (lowt && hight) : (lowt || hight);
}

void
ButtonAna::process(uint8_t sts)
{
    valBit(((sts & Button::Curr) != 0));

    if ((sts & Button::Dn) && _OnPress) _OnPress(this);
    if ((sts & Button::Up) && _OnRelease) _OnRelease(this);
}

void
ButtonAna::check(bool force)
{
    checkVal(getInput(), force);
}

void
ButtonAna::checkVec(uint8_t *bytevec, bool force)
{
    // useless here
}

void
ButtonAna::checkVal(uint8_t ival, bool force)
{
    uint8_t newi = ana2dig(ival);
    uint8_t curi = ((_flags & Button::lastState) ? HIGH : LOW);

    if (force || newi != curi) {
        // button state changed
        newi = (newi == HIGH ? (Button::Dn|Button::High) : Button::Up);
        process(newi);
    }
}

// end ButtonAna.cpp
