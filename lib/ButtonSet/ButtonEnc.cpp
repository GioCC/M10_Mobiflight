// =======================================================================
// @file        ButtonEnc.cpp
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-09 12:47
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


#include "ButtonEnc.h"

EBcallback ButtonEnc::_OnPress = nullptr;
EBcallback ButtonEnc::_OnRelease = nullptr;
EBcallback ButtonEnc::_OnLong = nullptr;

ButtonEnc::ButtonEnc(
    uint8_t     index,
    char*       name,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(index, 0, name, mirrorvar, mirrorbit), 
TlastChange(0), TlastPress(0), debounceTime(0), longPDelay(0)
{}


ButtonEnc::ButtonEnc(
    uint8_t     index,
    uint16_t    code,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(index, 0, code, mirrorvar, mirrorbit),
TlastChange(0), TlastPress(0), debounceTime(0), longPDelay(0)
{}

// Set long pressure delay (in ms; rounded to nearest 100 ms; effective range 100ms..25.5s)
void
ButtonEnc::setLongPDelay(uint16_t delay)
{
    if(delay > 25450) delay = 25450;
    longPDelay = (uint8_t)((delay+50)/100);    // rounded to the nearest 100ms
}

void
ButtonEnc::process(uint8_t sts)
{
    valBit(((sts & Button::Curr) != 0));
    if((sts & ~Button::Curr) == 0) return; 
    
    if ((sts & Button::Dn) && _OnPress)   _OnPress(this);
    if ((sts & Button::Up) && _OnRelease) _OnRelease(this);
    if ((sts & Button::Long) && _OnLong)  _OnLong(this);
}

void
ButtonEnc::check(bool force)
{
    checkVal(getInput(), force);
}

void
ButtonEnc::checkVal(uint8_t val, bool force)
{
    uint8_t curi = ((_flags & Button::lastState) ? HIGH : LOW);
    uint8_t newi = (val ? HIGH : LOW);
    
    unsigned long now = millis();
    uint8_t res = Button::None;

    // Debounce strategy: Steady state
    // Input is validated only if it's been stable for a debouncing time interval
    // Safe but slightly delayed (and further delayed, up to insensitive, if input is very noisy)

    if (newi != curi) {
        // button state changed
        TlastChange = now;
    } else
    if (curi == HIGH && longPDelay != 0 && TlastPress != 0) {
        // Long press interval expired
        if (now >= TlastPress + times100(longPDelay)) {
            TlastPress = 0;     // lock further activations
            res |= Button::Long;
        }
    }

    if (force || (TlastChange != 0 && ((now - TlastChange) >= debounceTime))) {
        // Debounce interval expired: button state confirmed stable
        res |= (newi == HIGH ? (Button::Dn|Button::High) : (Button::Up|Button::Low));
        if (newi == HIGH) TlastPress = now;
    }
    process(res);
}

void
ButtonEnc::checkVec(uint8_t *bytevec, bool force)
{
    uint8_t val = bytevec[((_pin-1)>>3)];
    val = ((val & (1<<((_pin-1)&0x07))) ? HIGH : LOW);
    checkVal(val, force);
}

// end ButtonEnc.cpp