/*
* File     : ButtonBas.h
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the ButtonAdv+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This library allows to conveniently define pushbutton actions with callbacks for several events.
* It is meant to work jointly with a ButtonManager, which in turn receives (and passes along)
* input values supplied by an underlying I/O reader.
*
* The ButtonAdv reads a value either:
* - directly from a digital I/O pin
* - directly from an analog I/O pin
* - an argument passed to the polling routine,  digital or analog
* and, after processing, invokes callback functions accordingly.
* Analog values are used for multi-position switches (thus are converted to a digital value
* for a specified range); for each position a different ButtonAdv object must be defined (each
* with its own range).
* One of the four modes above can be chosen by using the corresponding constructor.
*
* Timings (for debounce, repeat etc) are handled internally.
*
* This class is a simplified version of ButtonAdv, stripped of repeat/longpress functions
* to substantially reduce memory footprint.
*
* For uniformity, the polling method always takes a ButtonStatus_t argument, which however is ignored if hardware
* inputs are used.
*
* This file declares the ButtonBas class.
*
* Usage:
* - Include ButtonAdv.h and ButtonGroupManager.h in your sketch
* - Add a call to ButtonGrpManager.checkButtons() in your main loop
* - Declare each button and define the events using a ButtonAdv constructor
* - Declare the required event functions ( void OnKeyXXX(ButtonAdv* but) )
* - See the comments in the code for more help
*/

#ifndef BUTTONBAS_H
#define BUTTONBAS_H

#include <Arduino.h>
#include "Button.h"
#include "ButtonManager.h"

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

    ButtonBas(  uint8_t     pin,
                uint8_t     useHWinput,
                char        *name,
                uint8_t     lthreshold =0,
                uint8_t     uthreshold =0,
                uint8_t     *mirrorvar=NULL,
                uint8_t     mirrorbit=0
            );

    ButtonBas(  uint8_t     pin,
                uint8_t     useHWinput,
                uint16_t    codeh,
                uint16_t    codel,
                uint8_t     lthreshold =0,
                uint8_t     uthreshold =0,
                uint8_t     *mirrorvar=NULL,
                uint8_t     mirrorbit=0
            );

    void
    CButtonBas(uint8_t lthreshold, uint8_t uthreshold);

    // ======================================
    // === Setup methods: common
    // ======================================
    // Must redefine methods with the derived type - in this case, static type matching is what is required.
    // These are all and the same (normally they also do the exact same thing) as defined in the Button class,
    // except for the return type.
    // For details, see comments in Button.h.

    ButtonBas& info(uint8_t npin, uint8_t isHW){ Button::info(npin, isHW); return *this; }

    ButtonBas& tag(const char *s)              { Button::tag(s); return *this; }
    ButtonBas& tag(uint16_t hi, uint16_t lo)   { Button::tag(hi, lo); return *this; }

    ButtonBas& data(const char *s)             { Button::data(s); return *this; }
    ButtonBas& data(byte *b)                   { Button::data(b); return *this; }
    ButtonBas& data(uint16_t hi, uint16_t lo)  { Button::data(hi, lo); return *this; }

    // Following two are only effective if corresponding compilation switches have been enabled in "Button.h" (compiling Button base class)
    ButtonBas& mirror(uint8_t *mvar, uint8_t mbit)  { Button::mirror(mvar, mbit); return *this; }
    ButtonBas& source(uint8_t *svar, uint8_t sbit)  { Button::source(svar, sbit); return *this; }

    // ======================================
    // === Setup methods: specialized
    // ======================================

    ButtonBas& callbacks(BBcallback OnPress, BBcallback OnRelease)
    {
        setOnPress((*OnPress)); 
        setOnRelease((*OnRelease)); 
        return *this; 
    }

    //ButtonBas& parana(uint16_t lthreshold, uint16_t uthreshold)
    ButtonBas& parana(uint8_t lthreshold = 0, uint8_t uthreshold=0)
    {
        lowerAnaThrs=lthreshold;    // (uint8_t)((lthreshold+2)>>2);
        upperAnaThrs=uthreshold;    // (uint8_t)((uthreshold+2)>>2);
        flagChg(flags, F_Analog, (lthreshold==uthreshold));
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

    // Checks the state of the button and triggers events accordingly;
    // Will be called from the ButtonGroupManager
    // 'value' is either:
    // - an analog value (0..255)
    // - for digital values, a bit pattern of type Button::ButtonStatus_t,
    //   with following meaning (see also base class):
    //      S_curr    Current input status
    //      S_dn      Input just became active
    //      S_up      Input was just released
    //      S_rpt     A repeat interval just expired
    //      S_long    The long press interval just expired
    //   All flags except S_curr are expected to be only set for the call right after the event occurs.
    // If the button is configured for direct HW pin reading, this value is ignored and HW value fetch is performed.
    // WARNING (for HW reading only): digital inputs are considered ACTIVE (yielding HIGH) if closed to GND.
    void    check(ButtonStatus_t value) override;

    // initState is used to assign the initial value.
    // It differs from check() because it only triggers OnPress/OnRelease events.
    // These are usually associated to stable switches (rather than temporary pushbuttons),
    // which require to have their position recorded at startup.
    // Argument: same as check(..).
    void    initState(ButtonStatus_t value) override;

    // A variant of 'check()' for digital inputs only (specific to derived class)
    // Gets its input source from the passed byte array according to pin#:
    // bytevec[0] contains values of pins 1..8 (bits 0..7), bytevec[1] contains pins 9..16 etc
    // It is responsibilty of the caller to assure that bytevec[] has a size compatible with the button's pin no.
    //
    // If the button is configured for direct HW pin reading or Analog source, a call to this method has NO EFFECT.
    void    check(uint8_t *bytevec);

    // Variant of initState
    void    initState(uint8_t *bytevec);

private:

    static BBcallback  _OnPress;
    static BBcallback  _OnRelease;

    uint8_t         debounceTime;       // internally in ms
    unsigned long   TlastChange;
    uint8_t         pressFlag;

    uint8_t         lowerAnaThrs;   // internally in 1/256th
    uint8_t         upperAnaThrs;   // internally in 1/256th

    void            _check(uint8_t value);      // Helper for check() methods
    void            _initState(uint8_t value);  // Helper for initState() methods
    uint8_t         _getInput(Button::ButtonStatus_t ival);
};

#endif
