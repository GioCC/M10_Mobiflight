/*
*
* File     : ButtonAna.h
* Version  : 1.0
* Released : 29/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the ButtonAdv+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This library allows to conveniently define pushbutton actions with callbacks for several events.
* It is meant to work jointly with a ButtonGroupManager, which in turn receives (and passes along)
* input flags supplied by an underlaying I/O reader (digital buttons only).
*
* The ButtonAna object receives an analog value (unsigned 8-bit integer) describing the status
* of the associated input or 'pins'; if the value falls between the defined thresholds (with
* hysteresis), then the "equivalent" digital input is considered active, and callback functions are invoked
* accordingly.
* For current status, up/dn transitions etc see docs.
*
* This file declares the ButtonAna class.
*
* Usage:
* - Include ButtonAna.h and ButtonManager.h in your sketch
* - Add a call to ButtonMgr.checkButtons() in your main loop
* - Declare each button (corresponding to an input value range) and define the events using a ButtonAna constructor
* - Declare the required event functions ( void OnXXX(ButtonAna* but) )
* - See the comments in the code for more help
*/

#ifndef BUTTONANA_H
#define BUTTONANA_H

#include <Arduino.h>
#include "Button.h"
#include "ButtonManager.h"

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

    ButtonAna(  uint8_t     pin,
                char        *name,
                uint8_t     lthreshold =128,
                uint8_t     uthreshold =255,
                uint16_t    repeatDelay=0,
                uint16_t    repeatRate=0,
                uint8_t     *mirrorvar=NULL,
                uint8_t     mirrorbit=0
            );

    ButtonAna(  uint8_t     pin,
                uint16_t    codeh,
                uint16_t    codel,
                uint8_t     lthreshold =128,
                uint8_t     uthreshold =255,
                uint16_t    repeatDelay=0,
                uint16_t    repeatRate=0,
                uint8_t     *mirrorvar=NULL,
                uint8_t     mirrorbit=0
            );

    void
    CButtonAna(uint16_t repeatDelay, uint16_t repeatRate);

    // ======================================
    // === Operation methods
    // ======================================
    //TODO These (for Analog button) are NOT overrides of Button::check()/initState():
    // these require an analog value. The argument coincidentally has the same data type 
    // as ButtonStatus_t, but they're not the same functions!
    // Maybe add a second arg?

    // Checks the state of the button and triggers events accordingly;
    // Will be called from the ButtonGroupManager
    void check(ButtonStatus_t value) override;

    // initState is used to assign the initial value.
    // It differs from check() because it only triggers OnKeyPress/OnKeyRelease events.
    // These are usually associated to stable switches (as opposed to temporary pushbuttons),
    // which require to have their position recorded at startup
    void initState(ButtonStatus_t value) override;

    // ======================================
    // === Setup methods: common
    // ======================================
    // Must redefine methods with the derived type - in this case, static type matching is what is required.
    // These are all and the same (normally they also do the exact same thing) as defined in the Button class,
    // except for the return type.
    // For details, see comments in Button.h.

    ButtonAna& info(uint8_t npin, uint8_t isHW) { Button::info(npin, isHW); return *this; }

    ButtonAna& tag(const char *s)                   { Button::tag(s); return *this; }
    ButtonAna& tag(uint16_t hi, uint16_t lo)        { Button::tag(hi, lo); return *this; }

    ButtonAna& data(const char *s)                  { Button::data(s); return *this; }
    ButtonAna& data(byte *b)                        { Button::data(b); return *this; }
    ButtonAna& data(uint16_t hi, uint16_t lo)       { Button::data(hi, lo); return *this; }

    // Following two are only effective if corresponding compilation switches have been enabled in "Button.h" (compiling Button base class)
    ButtonAna& mirror(uint8_t *mvar, uint8_t mbit)  { Button::mirror(mvar, mbit); return *this; }
    ButtonAna& source(uint8_t *svar, uint8_t sbit)  { Button::source(svar, sbit); return *this; }

    // ======================================
    // === Setup methods: specialized
    // ======================================

    ButtonAna& callbacks(ABcallback OnPress, ABcallback OnRelease = nullptr)
    { 
        setOnPress(OnPress); 
        setOnRelease(OnRelease); 
        return *this; 
    }


    ButtonAna& params(uint16_t repeatDelay = 0, uint16_t repeatRate = 0)
    {
        setRepeatDelay(repeatDelay);
        setRepeatRate(repeatRate);
        return *this;
    }

    ButtonAna& parana(uint8_t lthreshold = 0, uint8_t uthreshold = 0, uint8_t hyst = 2)
    {
        lowerAnaThrs = lthreshold;
        upperAnaThrs = uthreshold;
        hysteresis   = hyst;
        flagChg(flags, F_Analog, (lthreshold == uthreshold));
        return *this;
    }

    // ======================================
    // === Setters (single params)
    // ======================================

    // Sets the debounce time (milliseconds)
    void    setDebounce(unsigned int delay) { debounceTime = delay; };
    // Sets the delay (milliseconds) before the keypress event is repeated
    // Rounded to nearest 100 ms; effective range 100ms..25.5s
    void    setRepeatDelay(uint16_t delay);
    // Sets the time (milliseconds) between each repeated keypress event
    // Rounded to next 10 ms; effective range 10ms..2.55s
    void    setRepeatRate(uint16_t repeat);

    void    setHysteresis(uint8_t hys)      {hysteresis = hys;}

    void    setOnPress(ABcallback f)        {_OnPress   = f;}
    void    setOnRelease(ABcallback f)      {_OnRelease = f;}

    // ======================================
    // === Getters
    // ======================================

    // - none -

private:

    static ABcallback   _OnPress;
    static ABcallback   _OnRelease;

    uint8_t         debounceTime;       // internally in ms
    uint8_t         repeatDelay;        // internally in ms*100; range 100ms..25.5s
    uint8_t         repeatRate;         // internally in ms*10; range 10ms..2.55s
    unsigned long   TlastChange;
    unsigned long   TstartPress;
    unsigned long   TlastPress;

    uint8_t         lowerAnaThrs;   // 0..255
    uint8_t         upperAnaThrs;   // 0..255
    uint8_t         hysteresis;     // 0..255

    uint8_t         _getInput(uint8_t ival);
};

#endif
