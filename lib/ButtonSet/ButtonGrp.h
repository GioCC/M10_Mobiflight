/*
*
* File     : ButtonGrp.h
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
* This file defines the ButtonGrp class.
* The ButtonGrp receives a set of I/O flags describing the status of its associated input or 'pin'
* (for current status, up/dn transitions etc - see doc) and invokes callback functions accordingly.
*
* Usage:
* - Include ButtonGrp.h and ButtonManager.h in your sketch
* - Add a call to ButtonMgr.checkButtons() in your main loop
* - Declare each button and define the events using a ButtonGrp constructor
* - Declare the required event functions ( void OnKeyXXX(ButtonGrp* but) )
* - See the comments in the code for more help
*/

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
#ifdef USE_BTN_MGR
#include "ButtonManager.h"
#endif

#define UNUSED(x) ((void)(x))

class ButtonGrp;

using GBcallback = void (*)(ButtonGrp*);

class ButtonGrp
: public Button
{

public:

    // ======================================
    // === Constructors
    // ======================================

    // This constructor allows to define each individual button.
    // Buttons are automatically added to the collection in the ButtonManager named 'ButtonMgr'
    // to allow centralized polling. From there they can also be retrieved for custom operations.
    //
    // In ordinary usage:
    // - pushbuttons will only supply 'OnKeyPress' (and possibly 'OnKeyLong' and/or 'hasRepeat', if required);
    // - stable position switches will only supply 'OnKeyPress' and 'OnKeyRelease'.
    // Of course this is not mandatory, and other settings, if sensible, can be chosen.
    // The name is optional, as are the mirror variable and flag (bit# in var).
    // If the mirror var is not NULL, the button state is automatically mirrored into it.

    ButtonGrp() {}     // for objects that will be completely filled in later

    ButtonGrp(  uint8_t     pin,
                char*       name,
                uint8_t     hasRepeat   = 0,
                uint8_t*    mirrorvar   = NULL,
                uint8_t     mirrorbit   = 0
            );

    ButtonGrp(  uint8_t     pin,
                uint16_t    code,
                uint8_t     hasRepeat   = 0,
                uint8_t*    mirrorvar   = NULL,
                uint8_t     mirrorbit   = 0
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

    ButtonGrp& pin(uint8_t npin, uint8_t isHW)      { Button::pin(npin, isHW); return *this; }

    ButtonGrp& tag(const char *s)                   { Button::tag(s);    return *this; }
    ButtonGrp& tag(byte *b)                         { Button::tag(b);    return *this; }
    ButtonGrp& tag(uint16_t code)                   { Button::tag(code); return *this; }

    // ButtonGrp& data(const char *s)               { Button::data(s);    return *this; }
    // ButtonGrp& data(byte *b)                     { Button::data(b);    return *this; }
    // ButtonGrp& data(uint16_t code)               { Button::data(code); return *this; }

    // Following two are only effective if corresponding compilation switches have been enabled in "Button.h"
    ButtonGrp& mirror(uint8_t *mvar, uint8_t mbit)  { Button::mirror(mvar, mbit); return *this; }
    ButtonGrp& source(uint8_t *svar, uint8_t sbit)  { Button::source(svar, sbit); return *this; }

    static 
    ButtonGrp& make(void)                           { ButtonGrp* b = new ButtonGrp(); return *b; }

    #ifdef USE_BTN_MGR
    // Add the button to the collection in the specified ButtonManager,
    // to allow centralized polling. From there they can also be retrieved for custom operations.
    ButtonGrp& addTo(ButtonManager& mgr);

    // Create a Button and add it to the collection in the specified ButtonManager.
    static 
    ButtonGrp& make(ButtonManager& mgr);
    #endif

    // ======================================
    // === Setup methods: specialized
    // ======================================

    ButtonGrp& callbacks(GBcallback OnPress, GBcallback OnRelease = nullptr, GBcallback OnLong = nullptr)
    {
        setOnPress(OnPress); 
        setOnRelease(OnRelease); 
        setOnLong(OnLong); 
        return *this; 
    }

    ButtonGrp& params(uint8_t hasRepeat)            { setRepeat(hasRepeat); return *this; }

    // ======================================
    // === Setters (single params)
    // ======================================

    void    setRepeat(uint8_t r)         {if(r) {_flags |= Button::hasRepeat;} else {_flags &= ~Button::hasRepeat;} }

    void    setOnPress(GBcallback f)     {_OnPress   = f;}
    void    setOnRelease(GBcallback f)   {_OnRelease = f;}
    void    setOnLong(GBcallback f)      {_OnLong    = f;}

    // ======================================
    // === Getters
    // ======================================

    // - none -

    // ======================================
    // === Operation methods
    // ======================================

    // Checks the state of the button and triggers events accordingly;
    // Will be called from the ButtonGroupManager
    // 'value' is a bit pattern with following meaning (see base class):
    //   Button::curr    Current input status
    //   Button::dn      Input just became active
    //   Button::up      Input was just released
    //   Button::rpt     A repeat interval just expired
    //   Button::long    The long press interval just expired
    // All flags except Button::curr are expected to be set for one call only,
    // right after the event occurs.
    void    check(ButtonStatus_t value) override;

    // initState is used to assign the initial value.
    // It differs from check() because it only triggers OnKeyPress/OnKeyRelease events.
    // These are usually associated to stable switches (rather than temporary pushbuttons),
    // which require to have their position recorded at startup
    void    initState(ButtonStatus_t value) override;

private:

    static GBcallback _OnPress;
    static GBcallback _OnRelease;
    static GBcallback _OnLong;

};

#endif
