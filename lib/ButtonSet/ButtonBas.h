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
* This file declares the ButtonBas class.
* This class is a simplified version of ButtonAdv, stripped of repeat/longpress functions
* to substantially reduce memory footprint.
*
*
* The ButtonBas reads a value either:
* - directly from a digital I/O pin
* - directly from an analog I/O pin
* - an argument passed to the polling routine,  digital or analog
* and, after processing, invokes callback functions accordingly.
* Analog values are used for multi-position switches (thus are converted to a digital value
* for a specified range); for each position a different ButtonBas object must be defined (each
* with its own range).
* One of the four modes above can be chosen by using the corresponding constructor.
*
* Timings (for debounce, repeat etc) are handled internally.
*
* For uniformity, the polling method always takes a ButtonStatus_t argument, which however is ignored if hardware
* inputs are used.
*
*
* Usage:
* - Include ButtonBas.h and ButtonGroupManager.h in your sketch
* - Add a call to ButtonGrpManager.checkButtons() in your main loop
* - Declare each button and define the events using a ButtonBas constructor
* - Declare the required event functions ( void OnKeyXXX(ButtonBas* but) )
* - See the comments in the code for more help
*/

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

    // Checks the state of the button and triggers events accordingly;
    void    check(uint8_t force = 0) override;
    
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
    // WARNING (for HW reading only): digital inputs are considered ACTIVE (yielding HIGH) if closed to GND.
    void    checkVal(ButtonStatus_t value, uint8_t force = 0) override;

    // A variant of 'check()' for digital input vectors (specific to derived class)
    // Gets its input source from the passed byte array according to pin#:
    // bytevec[0] contains values of pins 1..8 (bits 0..7), bytevec[1] contains pins 9..16 etc
    // It is responsibilty of the caller to assure that bytevec[] has a size compatible with the button's pin no.
    //
    // If the button is configured for direct HW pin reading or Analog source, a call to this method has NO EFFECT.
    void    check(uint8_t *bytevec, uint8_t force = 0);

private:

    static BBcallback  _OnPress;
    static BBcallback  _OnRelease;

    uint8_t         debounceTime;   // internally in ms
    unsigned long   TlastChange;

    uint8_t         lowerAnaThrs;   // internally in 1/256th
    uint8_t         upperAnaThrs;   // internally in 1/256th

    uint8_t         _getInput(void);
    uint8_t         _ana2dig(uint8_t val);
    void            _check(uint8_t newi, uint8_t force = 0);
};

#endif
