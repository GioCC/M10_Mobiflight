/*
* File     : Button.cpp
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
* This is the base class for specific ButtonXXX classes.
* ButtonXXX objects receive a set of I/O flags describing the status of its associated input or 'pin'
* (for current status, up/dn transitions etc - see doc) and invokes callback functions accordingly.
* Callbacks are all defined in the derived classes.
*
* This file defines the base class for all ButtonXXX classes.
*
* Usage:
* - Include ButtonXXX.h (for all relevant XXX types) and ButtonManager.h in your sketch
* - Add a call to ButtonManager.check(...) in your main loop
* - Declare each button and define the events using a ButtonXXX constructor
* - Declare the required event functions ( void OnKeyYYY(ButtonXXX *but) )
* - See the comments in the code for more help
*
*/

#include "Button.h"

#ifdef USE_BTN_MGR
#include "ButtonManager.h"
#endif // USE_BTN_MGR

Button::Button(uint8_t npin, uint8_t useHWinput, const char *name, uint8_t *mirrorvar, uint8_t mirrorbit)
: pin(npin)
{
    CButton(useHWinput, mirrorvar, mirrorbit);
    tag(name);
}

Button::Button(uint8_t npin, uint8_t useHWinput, uint16_t codeh, uint16_t codel, uint8_t *mirrorvar, uint8_t mirrorbit)
: pin(npin)
{
    CButton(useHWinput, mirrorvar, mirrorbit);
    tag(codeh, codel);
}

void
Button::CButton(uint8_t useHWinput, uint8_t *mirrorvar, uint8_t mirrorbit)
{
    flagChg(flags, F_lastState, 0);//lastState = LOW;
    flagChg(flags, F_HWinput, useHWinput);
    if (useHWinput) {
        pinMode(pin,INPUT);
        digitalWrite(pin, HIGH);
    }
    mirror(mirrorvar, mirrorbit);

#ifdef USE_BTN_MGR
    //ButtonGroupManager::instance()->addButton(this);
    ButtonMgr.add(this);
#endif // USE_BTN_MGR
}

Button *
Button::add(void)
{
#ifdef USE_BTN_MGR
    ButtonMgr.add(this);
#endif // USE_BTN_MGR
    return this;
}
