// =======================================================================
// @file        ButtonEnc.h
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-09 12:25
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


// NOTE: Currently, a "long press" triggers both the "Press" and the "LongPress" events;
// this may or may not be as intended.
// A possible improvement: if a "LongPress" callback is used, the "Press" event is handled differently
// so as to match a "ShortPress" (which is actually activated on release before the LongPress time).
// Better: a mode flag tells whether to use two available callbacks either as "Press/Release" or "Short/Long press"
// (Repeat would be feasible in both modes).

#ifndef BUTTONENC_H
#define BUTTONENC_H

#include <Arduino.h>
#include "Button.h"
#ifdef USE_BTN_MGR
#include "ButtonManager.h"
#endif

class ButtonEnc;

using EBcallback = void (*)(ButtonEnc*);

class ButtonEnc
: public Button
{

public:

    // ======================================
    // === Constructors
    // ======================================

    // This constructor allows to define each individual encoder button.
    // Buttons are automatically added to the collection in the ButtonManager named 'BtnMgr'
    // to allow centralized polling. From there they can also be retrieved for custom operations.
    //
    // The name is optional, as are the mirror variable and flag (bit# in var).
    // If the mirror var is not NULL, the button state is automatically mirrored into it.

    ButtonEnc() {}     // for objects that will be completely filled in later

    ButtonEnc(  
        uint8_t     index,
        char*       name,
        uint8_t*    mirrorvar   =NULL,
        uint8_t     mirrorbit   =0
    );

    ButtonEnc(  
        uint8_t     index,
        uint16_t    code,
        uint8_t*    mirrorvar   =NULL,
        uint8_t     mirrorbit   =0
    );


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

    DEFINE_BASIC_METHODS(ButtonEnc)

    // ======================================
    // === Setup methods: specialized
    // ======================================

    ButtonEnc& callbacks(EBcallback OnPress, EBcallback OnRelease = nullptr, EBcallback OnLong = nullptr)
    {
        setOnPress(OnPress);
        setOnRelease(OnRelease);
        setOnLong(OnLong);
        return *this;
    }

    // ======================================
    // === Setters (single params)
    // ======================================

    // Sets the delay (milliseconds) before the long-keypress event is triggered
    // Rounded to nearest 100 ms; effective range 100ms..25.5s
    void setLongPDelay(uint16_t delay);

    void setOnPress(EBcallback f)   { _OnPress   = f; }
    void setOnRelease(EBcallback f) { _OnRelease = f; }
    void setOnLong(EBcallback f)    { _OnLong    = f; }

    // ======================================
    // === Operation methods
    // ======================================

    // Version where the value is supplied from an external button manager.
    // 'status' is a bit pattern with following meaning (see also base class):
    //      Button::curr    Current input status
    //      Button::dn      Input just became active
    //      Button::up      Input was just released
    //      Button::rpt     (ignored)
    //      Button::long    The long press interval just expired
    // All flags except Button::curr are expected to be only set for the call right after the event occurs.
    void    process(uint8_t status) override;

    // Checks the state of the button (retrieving it internally)
    // and triggers events accordingly
    void    check(bool force = false) override;
    
    // A variant of 'check()' for single digital inputs
    // The current status is passed; status flags are computed internally,
    // then process() is called.
    void    checkVal(uint8_t val, bool force = false) override;

    // A variant of 'checkVal()' for digital input vectors (specific to derived class)
    // Gets its input source from the passed byte array according to pin#:
    // bytevec[0] contains values of pins 1..8 (bits 0..7), bytevec[1] contains pins 9..16 etc
    // It is responsibilty of the caller to assure that bytevec[] has a size compatible with the button's pin no.
    void    checkVec(uint8_t *bytevec, bool force = false) override;

private:

    static EBcallback _OnPress;
    static EBcallback _OnRelease;
    static EBcallback _OnLong;

    unsigned long   TlastChange;
    unsigned long   TlastPress;
    uint8_t         debounceTime;   // internally in ms
    uint8_t         longPDelay;     // internally in ms*100; range 100ms..25.5s

    // uint8_t         _getInput(void);
};

#endif
