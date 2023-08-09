// =======================================================================
// @file        ButtonBas.h
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-09 12:24
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


#ifndef BUTTONBAS_H
#define BUTTONBAS_H

#include <Arduino.h>
#include "Button.h"
#ifdef USE_BTN_MGR
#include "ButtonManager.h"
#endif

class ButtonBas;

using BBcallback = void (*)(ButtonBas*);

class ButtonBas
: public Button
{

public:

    // ======================================
    // === Constructors
    // ======================================

    ButtonBas() {}     // for objects that will be completely filled in later

    ButtonBas(  
        uint8_t     pin,
        uint8_t     useHWinput,
        char*       name,
        uint8_t     lthreshold  =0,
        uint8_t     uthreshold  =0,
        uint8_t*    mirrorvar   =NULL,
        uint8_t     mirrorbit   =0
    );

    ButtonBas(  
        uint8_t     pin,
        uint8_t     useHWinput,
        uint16_t    code,
        uint8_t     lthreshold  =0,
        uint8_t     uthreshold  =0,
        uint8_t*    mirrorvar   =NULL,
        uint8_t     mirrorbit   =0
    );

    void
    CButtonBas(uint8_t lthreshold, uint8_t uthreshold);

    // ======================================
    // === Setup methods: common
    // ======================================
    // These methods, all returning the object itself, are meant to allow chained configuration calls like:
    //   add().tag("MyName").data(0x6E)
    // These methods can't be overrides of 'virtual' methods in the base class: in order to insure compatibility
    // with out specialized setup methods (if any), derived objects have to redefine these methods 
    // WITH THE SAME NAME AND SIGNATURE but RETURNING THEIR OWN TYPE. 
    // The redefined methods must ONLY call the corresponding base class methods and return the reference to
    // the object, and nothing more.
    // For details, see comments in Button.h.

    DEFINE_BASIC_METHODS(ButtonBas)

    // ======================================
    // === Setup methods: specialized
    // ======================================

    ButtonBas& callbacks(BBcallback OnPress, BBcallback OnRelease)
    {
        setOnPress((*OnPress)); 
        setOnRelease((*OnRelease)); 
        return *this; 
    }

    ButtonBas& anaParm(uint8_t lthreshold, uint8_t uthreshold)
    {
        lowerAnaThrs=lthreshold;    // (uint8_t)((lthreshold+2)>>2);
        upperAnaThrs=uthreshold;    // (uint8_t)((uthreshold+2)>>2);
        modeAnalog(lthreshold != uthreshold);
        return *this;
    }

    // ======================================
    // === Setters (single params)
    // ======================================

    // Sets the debounce time (milliseconds)
    void    setDebounce(unsigned int delay) { debounceTime = delay; };

    void    setOnPress(BBcallback f)        {_OnPress   = f;}
    void    setOnRelease(BBcallback f)      {_OnRelease = f;}

    // ======================================
    // === Getters
    // ======================================

    // - none -

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

    // Variant of 'check()' with input value (analog or digital, as selected)
    // passed from outside by the caller. Status flags are computed internally.
    void    checkVal(uint8_t val, bool force = false) override;

    // Variant of 'checkVal()' for digital input vectors (specific to derived class)
    // Gets its input source from the passed byte array according to pin#:
    // bytevec[0] contains values of pins 1..8 (bits 0..7), bytevec[1] contains pins 9..16 etc
    // It is responsibilty of the caller to assure that bytevec[] has a size compatible with the button's pin no.
    //
    // If the button is configured for direct HW pin reading or Analog source, a call to this method has NO EFFECT.
    void    checkVec(uint8_t *bytevec, bool force = false) override;

    // Translates the analog value to the corresponding digital state.
    bool ana2dig(uint8_t val) override;

private:

    static BBcallback  _OnPress;
    static BBcallback  _OnRelease;

    uint8_t         debounceTime;   // internally in ms
    unsigned long   TlastChange;

    uint8_t         lowerAnaThrs;   // internally in 1/256th
    uint8_t         upperAnaThrs;   // internally in 1/256th

    // uint8_t         _getInput(void);
};

#endif
