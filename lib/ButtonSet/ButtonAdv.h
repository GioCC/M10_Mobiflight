// =======================================================================
// @file        ButtonAdv.h
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-10-16 22:08
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


// NOTE: Currently, a "long press" triggers both the "Press" and the "LongPress" events;
// this may or may not be as intended.
// A possible improvement: if a "LongPress" callback is used, the "Press" event is handled differently
// so as to match a "ShortPress" (which is actually activated on release before the LongPress time).
// Better: a mode flag tells whether to use two available callbacks either as "Press/Release" or "Short/Long press"
// (Repeat would be feasible in both modes).

#ifndef BUTTONADV_H
#define BUTTONADV_H

#include <Arduino.h>
#include "Button.h"

class ButtonAdv;

using BAcallback = void (*)(ButtonAdv*);

class ButtonAdv
: public Button
{

public:

    // ======================================
    // === Constructors
    // ======================================

    ButtonAdv() {}     // for objects that will be completely filled in later

    ButtonAdv(  
        uint8_t     pin,
        uint8_t     hardware,
        char*       name,
        uint16_t    repeatDelay = 0,
        uint16_t    repeatRate  = 0,
        uint16_t    longPress   = 0,
        uint8_t     lthreshold  = 0,  // Threshold values specified in 1/256th (0..255)
        uint8_t     uthreshold  = 0,  // Threshold values specified in 1/256th (0..255)
        uint8_t*    mirrorvar   = NULL,
        uint8_t     mirrorbit   = 0
    );

    ButtonAdv(  
        uint8_t     pin,
        uint8_t     hardware,
        uint16_t    code,
        uint16_t    repeatDelay = 0,
        uint16_t    repeatRate  = 0,
        uint16_t    longPress   = 0,
        uint8_t     lthreshold  = 0,  // Threshold values specified in 1/256th (0..255)
        uint8_t     uthreshold  = 0,  // Threshold values specified in 1/256th (0..255)
        uint8_t*    mirrorvar   = NULL,
        uint8_t     mirrorbit   = 0
    );

    void
    CButtonAdv( 
        uint16_t    repeatDelay,
        uint16_t    repeatRate,
        uint16_t    longPress,
        uint8_t     lthreshold,
        uint8_t     uthreshold
    );

    // ======================================
    // === Setup methods: common
    // ======================================
    // These methods, all returning the object itself, are meant to allow chained configuration calls like:
    //   make().tag("MyName").data(0x6E)
    // These methods can't be overrides of 'virtual' methods in the base class: in order to insure compatibility
    // with out specialized setup methods (if any), derived objects have to redefine these methods 
    // WITH THE SAME NAME AND SIGNATURE but RETURNING THEIR OWN TYPE. 
    // The redefined methods must ONLY call the corresponding base class methods and return the reference to
    // the object, and nothing more.
    // For details, see comments in Button.h.

    DEFINE_BASIC_METHODS(ButtonAdv);

    // ======================================
    // === Setup methods: specialized
    // ======================================

    ButtonAdv& callbacks(BAcallback OnPress, BAcallback OnRelease = nullptr, BAcallback OnLong = nullptr)
    { 
        setOnPress((*OnPress)); 
        setOnRelease((*OnRelease)); 
        setOnLong((*OnLong)); 
        return *this; 
    }

    ButtonAdv& params(uint16_t repeatDelay = 0, uint16_t repeatRate = 0, uint16_t longPress = 0)
    {
        setRepeatDelay(repeatDelay);
        setRepeatRate(repeatRate);
        setLongPDelay(longPress);
        return *this;
    }

    ButtonAdv& anaParm(uint8_t lthreshold, uint8_t uthreshold, uint8_t hyst = 2)
    {
        lowerAnaThrs = lthreshold;
        upperAnaThrs = uthreshold;
        hysteresis   = hyst;
        modeAnalog(lthreshold != uthreshold);
        return *this;
    }

    // ======================================
    // === Setters (single params)
    // ======================================

    // Sets the debounce time (milliseconds)
    void    setDebounce(unsigned int delay)    { debounceTime = delay; };
    // Sets the time (milliseconds) between each repeated keypress event
    // Rounded to next 10 ms; effective range 10ms..2.55s
    void    setRepeatRate(uint16_t repeatRate);
    // Sets the delay (milliseconds) before the keypress event is repeated
    // Rounded to nearest 100 ms; effective range 100ms..25.5s
    void    setRepeatDelay(uint16_t delay);
    // Sets the delay (milliseconds) before the long-keypress event is triggered
    // Rounded to nearest 100 ms; effective range 100ms..25.5s
    void    setLongPDelay(uint16_t delay);

    void    setHysteresis(uint8_t hys)  { hysteresis = hys; }

    void    setOnPress(BAcallback f)    { _OnPress   = f; }
    void    setOnRelease(BAcallback f)  { _OnRelease = f; }
    void    setOnLong(BAcallback f)     { _OnLong    = f; }

    // ======================================
    // === Getters
    // ======================================

    //Retrieve the amount of milliseconds since the input vector is validated
    uint16_t getPressTime(void) { return (uint16_t)(millis() - TstartPress); }

    // ======================================
    // === Operation methods
    // ======================================

    // Triggers the appropriate events according to the state of the button,
    // which is normally supplied by an external button manager.
    // 'Status' is a bit pattern with following meaning:
    //      Button::Curr    Current input status
    //      Button::Dn      Input just became active
    //      Button::Up      Input was just released
    //      Button::Rpt     A repeat interval just expired
    //      Button::Long    The long press interval just expired
    // All flags except Button::curr are expected to be only set for the call right after the event occurs.
    // If the button is configured for direct HW pin reading, this value is ignored and HW value fetch is performed.
    // WARNING (for HW reading only): digital inputs are considered ACTIVE (yielding HIGH) if closed to GND.
    void    process(uint8_t status) override;

    // Checks the state of the button: 
    // polls status value internally and triggers events accordingly.
    void    check(bool force = false) override;

    // Variant of 'check()' with input value (analog or digital, as supported)
    // passed from outside by the caller. Status flags are computed internally.
    void    checkVal(uint8_t value, bool force = false) override;

    // Variant of 'checkVal()' for digital input vectors (specific to derived class)
    // Gets its input source from the passed byte array according to pin#:
    // bytevec[0] contains values of pins 1..8 (bits 0..7), bytevec[1] contains pins 9..16 etc
    // It is responsibilty of the caller to assure that bytevec[] has a size compatible with the button's pin no.
    //
    // If the button is configured for direct HW pin reading or Analog source, a call to this method has NO EFFECT.
    void    checkVec(uint8_t *bytevec, bool force = false) override;

    // Translates the analog value to the corresponding digital state.
    bool    ana2dig(uint8_t val) override;

private:

    static BAcallback   _OnPress;
    static BAcallback   _OnRelease;
    static BAcallback   _OnLong;

    // Debounce:
    unsigned long   TlastChange;
    uint8_t         debounceTime;   // internally in ms
    
    // Repeat:
    unsigned long   TlastPress; 
    uint8_t         repeatDelay;    // internally in ms*100; range 100ms..25.5s
    uint8_t         repeatRate;     // internally in ms*10; range 10ms..2.55s
    
    // Long pressure:
    unsigned long   TstartPress;
    uint8_t         longPDelay;     // internally in ms*100; range 100ms..25.5s
    uint8_t         longPFlag;

    uint8_t         lowerAnaThrs;   // internally in 1/256th
    uint8_t         upperAnaThrs;   // internally in 1/256th
    uint8_t         hysteresis;     // 0..255

    //uint8_t         _getInput(void);
};

#endif
