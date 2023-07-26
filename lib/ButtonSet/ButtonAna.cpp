/*
*
* File     : ButtonAna.h
* Version  : 1.0
* Released : 29/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the ButtonAdv+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This library allows to conveniently define pushbutton actions with callbacks for several events.
* It is meant to work jointly with a ButtonGroupManager, which in turn receives (and passes along)
* input flags supplied by an underlaying I/O reader (digital buttons only).
*
* The ButtonAna object receives a vector of analog values (unsigned 8-bit integers) describing the status
* of their associated inputs or 'pins'; if the value falls between the defined thresholds (with
* hysteresis), then the "equivalent" digital input is considered active, and callback functions are invoked
* accordingly.
* For current status, up/dn transitions etc see docs.
*
* This file defines the ButtonAna class.
*
* Usage:
* - Include ButtonAna.h and ButtonManager.h in your sketch
* - Add a call to ButtonMgr.checkButtons() in your main loop
* - Declare each button (corresponding to an input value range) and define the events using a ButtonAna constructor
* - Declare the required event functions ( void OnXXX(ButtonAna* but) )
* - See the comments in the code for more help
*/

#include "ButtonAna.h"

ABcallback   ButtonAna::_OnPress = nullptr;
ABcallback   ButtonAna::_OnRelease = nullptr;

ButtonAna::ButtonAna(
    uint8_t     npin,
    char*       name,
    uint8_t     lthreshold,
    uint8_t     uthreshold,
    uint16_t    rptDelay,
    uint16_t    rptRate,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(npin, 0, name, mirrorvar, mirrorbit),
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonAna(repeatDelay, repeatRate);
}

ButtonAna::ButtonAna(
    uint8_t     npin,
    uint16_t    code,
    uint8_t     lthreshold,
    uint8_t     uthreshold,
    uint16_t    rptDelay,
    uint16_t    rptRate,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(npin, 0, code, mirrorvar, mirrorbit),
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonAna(repeatDelay, repeatRate);
}

void
ButtonAna::CButtonAna( uint16_t rptDelay, uint16_t rptRate)
{
    debounceTime = 100;
    hysteresis = 2;
    setRepeatDelay(rptDelay);
    setRepeatRate(rptRate);
    TlastChange = millis();
    flagChg(_flags, Button::lastState, 0);
    if(lowerAnaThrs==upperAnaThrs) {
        // Force thresholds: they can't be the same, otherwise it isn't an Analog button
        lowerAnaThrs = 128;
        upperAnaThrs = 255;
    }
    flagChg(_flags, Button::Analog, 1);
}

// Set repeat time (in ms; rounded to next 10 ms; effective range 10ms..2.55s)
void
ButtonAna::setRepeatRate(uint16_t rtime)
{
    if(rtime > 2541) rtime = 2541;
    repeatRate = (uint8_t)((rtime+9)/10);       // rounded to the next 10ms
    // repeat = rtime;
}

// Set repeat start delay (in ms; rounded to nearest 100 ms; effective range 100ms..25.5s)
void
ButtonAna::setRepeatDelay(uint16_t delay)
{
    if(delay > 25450) delay = 25450;
    repeatDelay = (uint8_t)((delay+50)/100);    // rounded to the nearest 10ms
    //repeatDelay = delay;
}

uint8_t
ButtonAna::_getInput(uint8_t ival)
{
    uint8_t newi;
    if(_flags & Button::HWinput) {
        newi = ((analogRead(_pin)+2)>>2);    // Scale ADC value from 10 to 8 bits
    } else if(hasSrcVar()) {
        newi = getSrcVal();
    } else {
        newi = ival;
    }
    return newi;
}

void
ButtonAna::check(ButtonStatus_t bval)
{
    unsigned long now;
    uint8_t  ival = bval;   // UGLY CONVERSION!!!
    int8_t  h;
    uint8_t newi;
    uint8_t curi = ((_flags & Button::lastState) ? HIGH : LOW);

    if(hasSrcVar()) {
        ival = getSrcVal();
    }
  
    h = (curi == HIGH ? hysteresis : -hysteresis);

    bool lowt  = (ival >= lowerAnaThrs-h);
    bool hight = (ival <  upperAnaThrs+h);
    lowt = (lowerAnaThrs < upperAnaThrs) ? (lowt && hight) : (lowt || hight);   // recycle var
    newi = (lowt ? HIGH : LOW);

    now = millis();
    if (newi != curi) {
        // button state changed
        if(TlastChange == 0) TlastChange = now;
        if (now - TlastChange >= debounceTime) {
            TlastChange = 0;    // this condition means "stable"
            // Register new status
            flagChg(_flags, Button::lastState, (newi == HIGH));
            curi = newi;
        }
    }

    if (curi == HIGH) {
        if (TstartPress == 0) {
            // transition L->H: mark the start time and notify others
            TstartPress = TlastChange; //now;
            if (_OnPress) {
                _OnPress(this);
                // callback may have taken some time: update <now> for <if>s below
                now = millis();
            }
            setBit();
        }

        // is repeating enabled?
        // We check _OnPress here because 'repeat' does not set any flag, therefore
        // if there's no callback there's no reason for processing.
        if ((repeatRate > 0 ) && (_OnPress != nullptr)) {
            // is the startdelay passed?
            // 'TlastPress != 0' (ie at least one repetition already happened)
            // is only used to spare computing time, skipping the check of the whole condition.
            if ((TlastPress != 0) || now >= TstartPress + times100(repeatDelay)) {
                // is it time for a repeat keypress call?
                if ((now - TlastPress) >= (unsigned long)times10(repeatRate)) {
                    TlastPress = now;
                    _OnPress(this);
                }
            }
        }
    } else {
        // the button is released, but was it previously pressed
        if (TstartPress != 0) {
            // Input LOW: reset all counters
            TstartPress = 0;
            TlastPress = 0;
            if (_OnRelease) _OnRelease(this);
            clearBit();
        }
    }
}

void
ButtonAna::initState(ButtonStatus_t bval)
{
    uint8_t  ival = bval;   // UGLY CONVERSION!!!
    uint8_t newi;
    int8_t  h;

    h = ((_flags & Button::lastState) ? hysteresis : -hysteresis);
    newi = ((ival >= lowerAnaThrs+h && ival < upperAnaThrs-h) ? HIGH : LOW);

    // Register new status
    flagChg(_flags, Button::lastState, (newi == HIGH));
    if (newi == HIGH) {
        if (_OnPress) _OnPress(this);
        setBit();
    } else {
        if (_OnRelease) _OnRelease(this);
        clearBit();
    }
}

// #ifdef USE_BTN_MGR
// ButtonAna& ButtonAna::addTo(ButtonManager& mgr)
// { 
//     mgr.add(this); return *this;
// }

// ButtonAna& ButtonAna::make(ButtonManager& mgr)
// {
//     ButtonAna* b = new ButtonAna(); 
//     b->addTo(mgr); 
//     return *b; 
// }
// #endif

// end ButtonAna.cpp
