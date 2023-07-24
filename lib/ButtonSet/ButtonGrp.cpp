/*
*
* File     : ButtonGrp.cpp
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the AdvButton+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This library allows to conveniently define pushbutton actions with callbacks for several events.
* It is meant to work jointly with a ButtonGroupManager, which in turn receives (and passes along)
* input flags supplied by an underlaying I/O reader (digital buttons only).
*
* The ButtonGrp receives a set of I/O flags describing the status of its associated input or 'pin'
* (for current status, up/dn transitions etc - see doc) and invokes callback functions accordingly.
*
* This file defines the ButtonGrp class.
*
* Usage:
* - Include ButtonGrp.h and ButtonManager.h in your sketch
* - Add a call to ButtonMgr.checkButtons() in your main loop
* - Declare each button and define the events using a ButtonGrp constructor
* - Declare the required event functions ( void OnKeyXXX(ButtonGrp* but) )
* - See the comments in the code for more help
*
*/

#include "ButtonGrp.h"
GBcallback ButtonGrp::_OnPress = nullptr;
GBcallback ButtonGrp::_OnRelease = nullptr;
GBcallback ButtonGrp::_OnLong = nullptr;


ButtonGrp::ButtonGrp(
    uint8_t     npin,
    char*       name,
    uint8_t     hasRepeat,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(npin, 0, name, mirrorvar, mirrorbit)
{
    setRepeat(hasRepeat);
}

ButtonGrp::ButtonGrp(
    uint8_t     npin,
    uint16_t    code,
    uint8_t     hasRepeat,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(npin, 0, code, mirrorvar, mirrorbit)
{
    setRepeat(hasRepeat);
}

void ButtonGrp::check(Button::ButtonStatus_t ival)
{
    flagChg(_flags, Button::lastState, ((ival & Button::Curr) != 0));

    if ((ival & Button::Dn) /*&& (cur == HIGH)*/) {
        if (_OnPress)   _OnPress(this);
        setBit();
    }
    if (ival & Button::Up) {
        if (_OnRelease) _OnRelease(this);
        clearBit();
    }
    if ((ival & Button::Rpt) && (_flags & Button::hasRepeat) /*&& (cur == HIGH)*/) {
        if (_OnPress)   _OnPress(this);
    }
    if ((ival & Button::Long) /*&& (cur == HIGH)*/) {
        if (_OnLong)    _OnLong(this);
    }
}

void ButtonGrp::initState(Button::ButtonStatus_t ival)
{
    if ((ival & Button::Curr) != 0) {
        if (_OnPress)   _OnPress(this);
        setBit();
    } else {
        if (_OnRelease) _OnRelease(this);
        clearBit();
    }
}

// #ifdef USE_BTN_MGR
// ButtonGrp& ButtonGrp::addTo(ButtonManager& mgr)
// { 
//     mgr.add(this); return *this;
// }

// ButtonGrp& ButtonGrp::make(ButtonManager& mgr)
// {
//     ButtonGrp* b = new ButtonGrp(); 
//     b->addTo(mgr); 
//     return *b; 
// }
// #endif

// end ButtonGrp.cpp