// =======================================================================
// @file        ButtonEvt.h
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

#ifndef BUTTONGRP_H
#define BUTTONGRP_H

#include <Arduino.h>
#include "Button.h"

class ButtonEvt;

using EVBcallback = void (*)(ButtonEvt*);

class ButtonEvt
: public Button
{

public:

    // ======================================
    // === Constructors
    // ======================================

    // This constructor allows to define each individual button.
    //
    // In ordinary usage:
    // - pushbuttons will only supply 'OnKeyPress' (and possibly 'OnKeyLong' and/or 'rptEnabled', if required);
    // - stable position switches will only supply 'OnKeyPress' and 'OnKeyRelease'.
    // Of course this is not mandatory, and other settings, if sensible, can be chosen.
    // The name is optional, as are the mirror variable and flag (bit# in var).
    // If the mirror var is not NULL, the button state is automatically mirrored into it.

    ButtonEvt() {}     // for objects that will be completely filled in later

    ButtonEvt(  
        uint8_t     pin,
        char*       name,
        uint8_t     rptEnabled   = 0,
        uint8_t*    mirrorvar   = NULL,
        uint8_t     mirrorbit   = 0
    );

    ButtonEvt(  
        uint8_t     pin,
        uint16_t    code,
        uint8_t     rptEnabled   = 0,
        uint8_t*    mirrorvar   = NULL,
        uint8_t     mirrorbit   = 0
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

    DEFINE_BASIC_METHODS(ButtonEvt)

    // ======================================
    // === Setup methods: specialized
    // ======================================

    ButtonEvt& callbacks(EVBcallback OnPress, EVBcallback OnRelease = nullptr, EVBcallback OnLong = nullptr)
    {
        setOnPress(OnPress); 
        setOnRelease(OnRelease); 
        setOnLong(OnLong); 
        return *this; 
    }

    ButtonEvt& params(uint8_t rptEnabled)            { enableRepeat(rptEnabled); return *this; }

    // ======================================
    // === Setters (single params)
    // ======================================

    void    enableRepeat(uint8_t r)      { flagChg(_flags, Button::rptEnabled, r); }

    void    setOnPress(EVBcallback f)     {_OnPress   = f;}
    void    setOnRelease(EVBcallback f)   {_OnRelease = f;}
    void    setOnLong(EVBcallback f)      {_OnLong    = f;}

    // ======================================
    // === Getters
    // ======================================

    // - none -

    // ======================================
    // === Operation methods
    // ======================================

    // Triggers the appropriate events according to the passed state,
    // which is normally supplied by an external button manager.
    // 'Status' is a bit pattern with following meaning:
    //      Button::Curr    Current input status
    //      Button::Dn      Input just became active
    //      Button::Up      Input was just released
    //      Button::Rpt     A repeat interval just expired
    //      Button::Long    The long press interval just expired
    // All flags except Button::curr are expected to be only set for the call right after the event occurs.
    void    process(uint8_t status) override;

private:

    static EVBcallback _OnPress;
    static EVBcallback _OnRelease;
    static EVBcallback _OnLong;

};

#endif
