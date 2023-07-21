/*
* File     : ButtonAdv.h
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
* - an argument passed to the polling routine, digital or analog
* and, after processing, invokes callback functions accordingly.
* Analog values are used for multi-position switches (thus are converted to a digital value
* for a specified range); for each position a different ButtonAdv object must be defined (each
* with its own range).
* One of the four modes above can be chosen by using the corresponding constructor.
*
* Timings (for debounce, repeat etc) are handled internally.
*
* There are some peculiarities with respect to other classes in the family:
* - this class can deal with physical inputs directly
* - this class can handle independent timings when using data from a supplied input value;
* - this class can take an analog value supplied as input.
* In the first case, independent timings are mandatory (because no global vector-level timings are available);
* in the second case, the internal implementation of timings may be exploited if several inputs require their
* own different timing parameters.
*
* For uniformity, the polling method always takes a ButtonStatus_t argument, which however is ignored if hardware
* inputs are used.
*
* This file declares the ButtonAdv class.
*
* Usage:
* - Include ButtonAdv.h and ButtonGroupManager.h in your sketch
* - Add a call to ButtonGrpManager.checkButtons() in your main loop
* - Declare each button and define the events using a ButtonAdv constructor
* - Declare the required event functions ( void OnKeyXXX(ButtonAdv* but) )
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

#ifndef BUTTONADV_H
#define BUTTONADV_H

#include <Arduino.h>
#include "Button.h"

#ifdef USE_BTN_MGR
#include "ButtonManager.h"
#endif

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

    ButtonAdv(  uint8_t     in,
                uint8_t     hardware,
                char*       name,
                uint16_t    repeatDelay =0,
                uint16_t    repeatRate  =0,
                uint16_t    longPress   =0,
                uint8_t     lthreshold  =0,  // Threshold values specified in 1/256th (0..255)
                uint8_t     uthreshold  =0,  // Threshold values specified in 1/256th (0..255)
                uint8_t*    mirrorvar   =NULL,
                uint8_t     mirrorbit   =0
            );

    ButtonAdv(  uint8_t     in,
                uint8_t     hardware,
                uint16_t    code,
                uint16_t    repeatDelay =0,
                uint16_t    repeatRate  =0,
                uint16_t    longPress   =0,
                uint8_t     lthreshold  =0,  // Threshold values specified in 1/256th (0..255)
                uint8_t     uthreshold  =0,  // Threshold values specified in 1/256th (0..255)
                uint8_t*    mirrorvar  =NULL,
                uint8_t     mirrorbit   =0
            );

    void
    CButtonAdv( uint16_t    repeatDelay,
                uint16_t    repeatRate,
                uint16_t    longPress,
                uint8_t     lthreshold,
                uint8_t     uthreshold
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

    ButtonAdv& pin(uint8_t npin, uint8_t isHW)      { Button::pin(npin, isHW); return *this; }

    ButtonAdv& tag(const char *s)                   { Button::tag(s);    return *this; }
    ButtonAdv& tag(byte *b)                         { Button::tag(b);    return *this; }
    ButtonAdv& tag(uint16_t code)                   { Button::tag(code); return *this; }

    // ButtonAdv& data(const char *s)               { Button::data(s);    return *this; }
    // ButtonAdv& data(byte *b)                     { Button::data(b);    return *this; }
    // ButtonAdv& data(uint16_t code)               { Button::data(code); return *this; }

    // Following two are only effective if corresponding compilation switches have been enabled in "Button.h"
    ButtonAdv& mirror(uint8_t *mvar, uint8_t mbit)  { Button::mirror(mvar, mbit); return *this; }
    ButtonAdv& source(uint8_t *svar, uint8_t sbit)  { Button::source(svar, sbit); return *this; }

    static 
    ButtonAdv& make(void)                           { ButtonAdv* b = new ButtonAdv(); return *b; }

    #ifdef USE_BTN_MGR
    // Add the button to the collection in the specified ButtonManager,
    // to allow centralized polling. From there they can also be retrieved for custom operations.
    ButtonAdv& addTo(ButtonManager& mgr);

    // Create a Button and add it to the collection in the specified ButtonManager.
    static 
    ButtonAdv& make(ButtonManager& mgr);
    #endif

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

    ButtonAdv& parana(uint8_t lthreshold = 0, uint8_t uthreshold = 0)
    {
        lowerAnaThrs=lthreshold;
        upperAnaThrs=uthreshold;
        flagChg(_flags, Button::Analog, (lthreshold==uthreshold));
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

    void    setHysteresis(uint8_t hys)  {hysteresis = hys;}

    void    setOnPress(BAcallback f)    {_OnPress   = f;}
    void    setOnRelease(BAcallback f)  {_OnRelease = f;}
    void    setOnLong(BAcallback f)     {_OnLong    = f;}

    // ======================================
    // === Getters
    // ======================================

    //Retrieve the amount of milliseconds since the input vector is validated
    uint16_t getPressTime(void) {return (uint16_t)(millis() - TstartPress);}

    // ======================================
    // === Operation methods
    // ======================================

    // Checks the state of the button and triggers events accordingly;
    // Will be called from the ButtonGroupManager
    // 'value' is either:
    // - an analog value (0..255)
    // - for digital values, a bit pattern of type Button::ButtonStatus_t,
    //   with following meaning (see also base class):
    //      Button::curr    Current input status
    //      Button::dn      Input just became active
    //      Button::up      Input was just released
    //      Button::rpt     A repeat interval just expired
    //      Button::long    The long press interval just expired
    //   All flags except Button::curr are expected to be only set for the call right after the event occurs.
    // If the button is configured for direct HW pin reading, this value is ignored and HW value fetch is performed.
    void    check(ButtonStatus_t value) override;

    // initState is used to assign the initial value.
    // It differs from check() because it only triggers OnKeyPress/OnKeyRelease events.
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

    static BAcallback   _OnPress;
    static BAcallback   _OnRelease;
    static BAcallback   _OnLong;

    uint8_t         debounceTime;       // internally in ms
    uint8_t         repeatDelay;        // internally in ms*100; range 100ms..25.5s
    uint8_t         repeatRate;         // internally in ms*10; range 10ms..2.55s
    uint8_t         longPDelay;         // internally in ms*100; range 100ms..25.5s
    unsigned long   TlastChange;
    unsigned long   TstartPress;
    unsigned long   TlastPress;
    uint8_t         longPFlag;

    uint8_t         lowerAnaThrs;   // internally in 1/256th
    uint8_t         upperAnaThrs;   // internally in 1/256th
    uint8_t         hysteresis;     // 0..255

    void            _check(uint8_t value);      // Helper for check() methods
    void            _initState(uint8_t value);  // Helper for initState() methods
    uint8_t         _getInput(Button::ButtonStatus_t ival);
};

#endif