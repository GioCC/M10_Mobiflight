// =======================================================================
// @file        ButtonAna.h
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-07 18:09
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


#ifndef BUTTONANA_H
#define BUTTONANA_H

#include <Arduino.h>
#include "Button.h"
#ifdef USE_BTN_MGR
#include "ButtonManager.h"
#endif

class ButtonAna;

using ABcallback = void (*)(ButtonAna*);

class ButtonAna
: public Button
{

public:

    // ======================================
    // === Constructors
    // ======================================

    // This constructor allows to define each individual 'button' associated to a subrange of the
    // whole analog input range.
    // Buttons are automatically added to the collection in the ButtonManager named 'ButtonMgr'
    // to allow centralized polling. From there they can also be retrieved for custom operations.
    //
    // In ordinary usage, the definition for each subrange will either supply 'OnKeyPress' and 'hasRepeat'
    // (if required), or 'OnKeyDown' and 'OnKeyRelease' (if required).
    // The name is optional, as are the mirror variable and flag (bit# in var).
    // If the mirror var is not NULL, the button state is automatically mirrored into it.

    ButtonAna() {}     // for objects that will be completely filled in later

    ButtonAna(  
        uint8_t     pin,
        char*       name,
        uint8_t     lthreshold =128,
        uint8_t     uthreshold =255,
        uint8_t*    mirrorvar  =NULL,
        uint8_t     mirrorbit  =0
    );

    ButtonAna(  
        uint8_t     pin,
        uint16_t    code,
        uint8_t     lthreshold =128,
        uint8_t     uthreshold =255,
        uint8_t*    mirrorvar  =NULL,
        uint8_t     mirrorbit  =0
    );

    void  CButtonAna(void);

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

    DEFINE_BASIC_METHODS(ButtonAna)

    // ======================================
    // === Setup methods: specialized
    // ======================================

    ButtonAna& callbacks(ABcallback OnPress, ABcallback OnRelease = nullptr)
    { 
        setOnPress(OnPress); 
        setOnRelease(OnRelease); 
        return *this; 
    }

    ButtonAna& params(uint8_t lthreshold, uint8_t uthreshold, uint8_t hyst = 2)
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

    void    setHysteresis(uint8_t hys)      {hysteresis = hys;}

    void    setOnPress(ABcallback f)        {_OnPress   = f;}
    void    setOnRelease(ABcallback f)      {_OnRelease = f;}

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

    // Variant of 'check()' with input value (analog or digital, as supported)
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

    static ABcallback   _OnPress;
    static ABcallback   _OnRelease;

    unsigned long   TlastChange;
    unsigned long   TstartPress;
    unsigned long   TlastPress;

    uint8_t         lowerAnaThrs;   // 0..255
    uint8_t         upperAnaThrs;   // 0..255
    uint8_t         hysteresis;     // 0..255

    uint8_t         _getInput(void);
};

#endif
