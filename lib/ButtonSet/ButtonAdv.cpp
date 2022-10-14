/*
* File     : ButtonAdv.cpp
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the ButtonAdv+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This library allows to conveniently define pushbutton actions with callbacks for several events.
* It is meant to work jointly with a ButtonGroupManager, which in turn receives (and passes along)
* input flags supplied by an underlaying I/O reader (digital buttons only).
*
* The ButtonAdv receives a set of I/O flags describing the status of its associated input or 'pin'
* (for current status, up/dn transitions etc - see doc) and invokes callback functions accordingly.
*
* This file defines the ButtonAdv class.
*
* Usage:
* - Include ButtonAdv.h and ButtonGroupManager.h in your sketch
* - Add a call to ButtonGrpManager.checkButtons() in your main loop
* - Declare each button and define the events using a ButtonAdv constructor
* - Declare the required event functions ( void OnKeyXXX(ButtonAdv* but) )
* - See the comments in the code for more help
*/

#include "ButtonAdv.h"

ButtonAdv::ButtonAdv(uint8_t    pin,
                    uint8_t     useHWinput,
                    char        *name,
                    BAcallback  OnPress,
                    BAcallback  OnRelease,
                    BAcallback  OnLong,
                    uint16_t    rptDelay,
                    uint16_t    rptRate,
                    uint16_t    longPress,
                    uint8_t     lthreshold,
                    uint8_t     uthreshold,
                    uint8_t     *mirrorvar,
                    uint8_t     mirrorbit
                    )
: Button(pin, useHWinput, name, mirrorvar, mirrorbit),
_OnPress(OnPress), _OnRelease(OnRelease), _OnLong(OnLong),
lowerAnaThrs(lthreshold), upperAnaThrs(uthreshold)
{
    CButtonAdv(rptDelay, rptRate, longPress, lthreshold, uthreshold);
}

ButtonAdv::ButtonAdv(uint8_t    pin,
                    uint8_t     useHWinput,
                    uint16_t    codeh,
                    uint16_t    codel,
                    BAcallback  OnPress,
                    BAcallback  OnRelease,
                    BAcallback  OnLong,
                    uint16_t    rptDelay,
                    uint16_t    rptRate,
                    uint16_t    longPress,
                    uint8_t     lthreshold,
                    uint8_t     uthreshold,
                    uint8_t     *mirrorvar,
                    uint8_t     mirrorbit
                    )
: Button(pin, useHWinput, codeh, codel, mirrorvar, mirrorbit),
_OnPress(OnPress), _OnRelease(OnRelease), _OnLong(OnLong),
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
    flagChg(flags, F_Analog, (lthreshold==uthreshold));
    TlastChange = millis();
    flagChg(flags, F_lastState, 0);
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
ButtonAdv::_getInput(Button::ButtonStatus_t ival) //Button::ButtonStatus_t ival)
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
        // Input digital values are considered ON if bit S_Curr (#0 - LSb) is == 1,
        // NOT simply if (value != 0) !!
        newi = (ana ? (uint8_t)ival : (ival & S_Curr));
    }
    if (flags & F_Analog) {
        int8_t  h = ((flags & F_lastState) ? hysteresis : -hysteresis);
        newi = ((newi >= lowerAnaThrs+h && newi < upperAnaThrs-h) ? HIGH : LOW);
    } else {
        newi = (newi ? HIGH : LOW);
    }
    return newi;
}


void
ButtonAdv::check(Button::ButtonStatus_t ival)
{
    _check(_getInput(ival));
}

void
ButtonAdv::check(uint8_t *bytevec)
{
    register uint8_t ival = bytevec[((pin-1)>>3)];
    ival = ((ival&(1<<((pin-1)&0x07))) ? HIGH : LOW);
    _check(ival);
}

void
ButtonAdv::_check(uint8_t newi)
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
            if(newi == HIGH) { flags |= F_lastState; } else { flags &= ~F_lastState; }
            curi = newi;
        }
    }

    if (curi == HIGH) {
        if (TstartPress == 0) {
            // transition L->H: mark the start time and notify others
            TstartPress = TlastChange; //now;
            if (_OnPress != NULL) {
                _OnPress(this);
                now = millis();     // callback may have taken some time
            }
        }

        // is repeating enabled?
        if ((repeatRate > 0 ) && (_OnPress != NULL)) {
            // is the startdelay passed?
            // 'TlastPress != 0' (ie at least one repetition already happened)
            // is only used to spare computing time, skipping the check of the whole condition.
            if ((TlastPress != 0) || now >= TstartPress + times100(repeatDelay)) {
                // is it time for a repeat keypress call?
                if ((now - TlastPress) >= (unsigned long)times10(repeatRate)) {
                    TlastPress = now;
                    _OnPress(this);
                    //now = millis();     // callback may have taken some time
                }
            }
        }
        // is long press enabled?
        if ((longPDelay > 0) && (_OnLong != NULL)) {
            if (longPFlag==0 && (now >= TstartPress + times100(longPDelay))) {
                longPFlag = 1;      // lock further activations
                _OnLong(this);
                //now = millis();     // callback may have taken some time
            }
        }
    } else {
        // the button is released, but was it previously pressed
        if (TstartPress != 0) {
            // Input LOW: reset all counters
            TstartPress = 0;
            TlastPress = 0;
            longPFlag = 0;      // release LP lock
            if (_OnRelease != NULL) {
                _OnRelease(this);
            }
        }
    }
}

void
ButtonAdv::initState(Button::ButtonStatus_t ival)
{
    _initState(_getInput(ival));
}

void
ButtonAdv::initState(uint8_t *bytevec)
{
    register uint8_t ival = bytevec[((pin-1)>>3)];
    ival = ((ival&(1<<((pin-1)&0x07))) ? HIGH : LOW);
    _initState(ival);
}

void
ButtonAdv::_initState(uint8_t newi)
{
    // Register new status
    if (newi == HIGH) {
        flags |= F_lastState;
        if (_OnPress != NULL) {
            _OnPress(this);
        }
    } else {
        flags &= ~F_lastState;
        if (_OnRelease != NULL) {
            _OnRelease(this);
        }
    }
}

