/*
*
* File     : ButtonEnc.h
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the AdvButton+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This library allows to conveniently define pushbutton actions with callbacks for several events.
* It is meant to work jointly with a ButtonManager, which in turn receives (and passes along)
* input flags supplied by an underlaying I/O reader (digital buttons only).
*
* The ButtonEnc receives a set of I/O flags describing the status of its associated input or 'pin'
* (for current status, up/dn transitions etc - see doc) and invokes callback functions accordingly.
*
* This file defines the ButtonEnc class.
*
* Usage:
* - Include ButtonEnc.h and ButtonManager.h in your sketch
* - Add a call to BtnMgr.checkButtons() in your main loop
* - Declare each button and define the events using a ButtonEnc constructor
* - Declare the required event functions ( void OnKeyXXX(ButtonEnc* but) )
* - See the comments in the code for more help
*/

/*
NOTE: Currently, a "long press" triggers both the "Press" and the "LongPress" events;
this may or may not be as intended.
A possible improvement: if a "LongPress" callback is used, the "Press" event is handled differently
so as to match a "ShortPress" (which is actually activated on release before the LongPress time).
Better: a mode flag tells whether to use two available callbacks either as "Press/Release" or "Short/Long press"
(Repeat would be feasible in both modes).
*/

#ifndef BUTTONENC_H
#define BUTTONENC_H

#include <Arduino.h>
#include "Button.h"
#include "ButtonManager.h"

class ButtonEnc;

typedef void (*EBcallback)(ButtonEnc*);

class ButtonEnc
: public Button
{

public:

    // This constructor allows to define each individual encoder button.
    // Buttons are automatically added to the collection in the ButtonManager named 'BtnMgr'
    // to allow centralized polling. From there they can also be retrieved for custom operations.
    //
    // The name is optional, as are the mirror variable and flag (bit# in var).
    // If the mirror var is not NULL, the button state is automatically mirrored into it.

    ButtonEnc() {}     // for objects that will be completely filled in later

    ButtonEnc(  uint8_t     index,
                char        *name,
                EBcallback  OnKeyPress,
                EBcallback  OnKeyRelease=NULL,
                EBcallback  OnKeyLong=NULL,
                uint8_t     *mirrorvar=NULL,
                uint8_t     mirrorbit=0
            );

    ButtonEnc(  uint8_t     index,
                uint16_t    codeh,
                uint16_t    codel,
                EBcallback  OnKeyPress,
                EBcallback  OnKeyRelease=NULL,
                EBcallback  OnKeyLong=NULL,
                uint8_t     *mirrorvar=NULL,
                uint8_t     mirrorbit=0
            );


    // Checks the state of the button and triggers events accordingly;
    // Will be called from the ButtonGroupManager
    // 'value' is a bit pattern with following meaning (see base class):
    //   S_curr    Current input status
    //   S_dn      Input just became active
    //   S_up      Input was just released
    //   S_rpt     A repeat interval just expired
    //   S_long    The long press interval just expired
    // All flags except S_curr are expected to be set for one call only,
    // right after the event occurs.
    // IMPORTANT: Input status is expected to be already debounced.
    virtual
    void    check(Button::ButtonStatus_t value);

    // initState is used to assign the initial value.
    // It differs from check() because it only triggers OnKeyPress/OnKeyRelease events.
    // These are usually associated to stable switches (rather than temporary pushbuttons),
    // which require to have their position recorded at startup
    virtual
    void    initState(Button::ButtonStatus_t value);

    // === Bulk setup methods

    // Must redefine methods with the derived type - in this case, static type matching is what is required!
    ButtonEnc *info(uint8_t npin, uint8_t isHW) { Button::info(npin, isHW); return this; }

    // Following two are only effective if corresponding compilation switches have been enabled in "Button.h" (compiling Button base class)
    ButtonEnc *mirror(uint8_t *mvar, uint8_t mbit)  { Button::mirror(mvar, mbit); return this; }
    ButtonEnc *source(uint8_t *svar, uint8_t sbit)  { Button::source(svar, sbit); return this; }

    ButtonEnc *tag(const char *s)               { Button::tag(s); return this; }
    ButtonEnc *tag(uint16_t hi, uint16_t lo)    { Button::tag(hi, lo); return this; }

    ButtonEnc *data(const char *s)              { Button::data(s); return this; }
    ButtonEnc *data(byte *b)                    { Button::data(b); return this; }
    ButtonEnc *data(uint16_t hi, uint16_t lo)   { Button::data(hi, lo); return this; }

    ButtonEnc  *callbacks(EBcallback OnPress, EBcallback OnRelease=NULL, EBcallback OnLong=NULL)
        { setOnPress(OnPress); setOnRelease(OnRelease); setOnLong(OnLong); return this; }


    // === Setters (single params)

    void setOnPress(EBcallback f)   __attribute__((always_inline))  {_OnPress = (*f);}
    void setOnRelease(EBcallback f) __attribute__((always_inline))  {_OnRelease = (*f);}
    void setOnLong(EBcallback f)    __attribute__((always_inline))  {_OnLong = (*f);}

    // === Getters

    // - none -

private:

    EBcallback _OnPress;
    EBcallback _OnRelease;
    EBcallback _OnLong;

};

#endif
