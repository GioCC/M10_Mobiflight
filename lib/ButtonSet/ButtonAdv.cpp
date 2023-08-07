// =======================================================================
// @file        ButtonAdv.cpp
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-08 15:41
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


#include "ButtonAdv.h"

BAcallback      ButtonAdv::_OnPress = nullptr;
BAcallback      ButtonAdv::_OnRelease = nullptr;
BAcallback      ButtonAdv::_OnLong = nullptr;

ButtonAdv::ButtonAdv(
    uint8_t    _pin,
    uint8_t     useHWinput,
    char        *name,
    uint16_t    rptDelay,
    uint16_t    rptRate,
    uint16_t    longPress,
    uint8_t     lthreshold,
    uint8_t     uthreshold,
    uint8_t     *mirrorvar,
    uint8_t     mirrorbit
)
: Button(_pin, useHWinput, name, mirrorvar, mirrorbit),
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonAdv(rptDelay, rptRate, longPress, lthreshold, uthreshold);
}

ButtonAdv::ButtonAdv(
    uint8_t    _pin,
    uint8_t     useHWinput,
    uint16_t    code,
    uint16_t    rptDelay,
    uint16_t    rptRate,
    uint16_t    longPress,
    uint8_t     lthreshold,
    uint8_t     uthreshold,
    uint8_t     *mirrorvar,
    uint8_t     mirrorbit
)
: Button(_pin, useHWinput, code, mirrorvar, mirrorbit),
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonAdv(rptDelay, rptRate, longPress, lthreshold, uthreshold);
}

void
ButtonAdv::CButtonAdv( uint16_t rptDelay, uint16_t rptRate, uint16_t longPress, uint8_t lthreshold, uint8_t uthreshold)
{
    debounceTime = 100;
    setRepeatDelay(rptDelay);
    setRepeatRate(rptRate);
    setLongPDelay(longPress);
    modeAnalog(lthreshold != uthreshold);
    TlastChange = millis();
    valBit(0);
}

// Set repeat time (in ms; rounded to next 10 ms; effective range 10ms..2.55s)
void
ButtonAdv::setRepeatRate(uint16_t rtime)
{
    if(rtime > 2541) rtime = 2541;
    repeatRate = (uint8_t)((rtime+9)/10);       // rounded to the next 10ms
    // repeat = rtime;
}

// Set repeat start delay (in ms; rounded to nearest 100 ms; effective range 100ms..25.5s)
void
ButtonAdv::setRepeatDelay(uint16_t delay)
{
    if(delay > 25450) delay = 25450;
    repeatDelay = (uint8_t)((delay+50)/100);    // rounded to the nearest 10ms
    //repeatDelay = delay;
}

// Set long pressure delay (in ms; rounded to nearest 100 ms; effective range 100ms..25.5s)
void
ButtonAdv::setLongPDelay(uint16_t delay)
{
    if(delay > 25450) delay = 25450;
    longPDelay = (uint8_t)((delay+50)/100);    // rounded to the nearest 100ms
}

uint8_t
ButtonAdv::_getInput()
{
    uint8_t res;

    if(isHW()) {
        if(isAna()) {
            res = ana2dig((analogRead(_pin)+2)>>2); // Scale ADC value from 10 to 8 bits
        } else {
            res = !digitalRead(_pin);
        }
    } else if(hasSrcVar()) {
        res = getSrcVal();
    } else {
        res = false;
    }
    return (res ? HIGH : LOW);
}

bool
ButtonAdv::ana2dig(uint8_t val)
{
    int8_t  hyst = ((_flags & Button::lastState) ? hysteresis : -hysteresis);
    bool lowt  = (val >= lowerAnaThrs - hyst);
    bool hight = (val <  upperAnaThrs + hyst);
    return (lowerAnaThrs < upperAnaThrs) ? (lowt && hight) : (lowt || hight);
}

void    
ButtonAdv::process(uint8_t sts)
{
    valBit((sts & Button::Curr) != 0);
    if((sts & ~Button::Curr) == 0) return; 
    
    if ((sts & Button::Dn) && _OnPress)   _OnPress(this);
    if ((sts & Button::Up) && _OnRelease) _OnRelease(this);
    if ((sts & Button::Long) && _OnLong)  _OnLong(this);
}

void
ButtonAdv::check(bool force)
{
    checkVal(_getInput(), force);
}

void
ButtonAdv::checkVec(uint8_t *bytevec, bool force)
{
    if(isHW() || isAna() || hasSrcVar()) return;
    register uint8_t res = bytevec[((_pin-1)>>3)];
    res = ((res&(1<<((_pin-1)&0x07))) ? HIGH : LOW);
    checkVal(res, force);
}

void
ButtonAdv::checkVal(uint8_t newi, bool force)
{
    unsigned long now;
    uint8_t res = Button::None;

    uint8_t curi = ((_flags & Button::lastState) ? HIGH : LOW);
    newi = (newi ? HIGH : LOW);

    // Debounce strategy: Blanking time
    // First edge immediately triggers status change; after this, the input
    // is ignored for a blanking time interval (to skip detection of possible
    // bounces).
    // Quick reaction but sensitive to glitches/spikes (if sampling is very frequent)
    
    now = millis();
    if (now - TlastChange >= debounceTime) {
        TlastChange = 0; // We're past blanking time: rearm detection
    }

    if (newi != curi && (TlastChange == 0)) {
        // Transition (first edge)
            TlastChange = now;      
            if (newi == HIGH) {
                // Press:
                TstartPress = now;
            res |= Button::Dn;
            } else {
                // Release:
                TstartPress = 0;
                TlastPress = 0;
                longPFlag = 0;      // release LP lock
            res |= Button::Up;
            }
        curi = newi;
    }

    if (curi == HIGH) {
        res |= Button::High;
        if (TstartPress == 0) {
            // After a valid edge trigger.
            
            // Is repeating enabled? We check _OnPress here because 'repeat' does not set
            // any flag, therefore if there's no callback there's no reason for processing.
            if ((repeatRate > 0) && (_OnPress)) {
                // is the startdelay passed?
                // 'TlastPress != 0' (ie at least one repetition already happened)
                // is only used to spare computing time, skipping the check of the whole condition.
                if ((TlastPress != 0) || now >= TstartPress + times100(repeatDelay)) {
                    // is it time for a repeat keypress call?
                    if ((now - TlastPress) >= (unsigned long)times10(repeatRate)) {
                        TlastPress = now;
                        res |= Button::Dn;
                    }
                }
            }
            // Is long press enabled (and LP not yet triggered)?
            if (longPFlag == 0 && longPDelay > 0) {
                if (now >= TstartPress + times100(longPDelay)) {
                    res |= Button::Long;
                    longPFlag = 1;      // lock further activations
                }
            }
        
        }
    } else {
        res |= Button::Low;
    }
    
    process(res);
}

// end ButtonAdv.cpp
