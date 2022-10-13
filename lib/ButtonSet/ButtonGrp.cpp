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

ButtonGrp::ButtonGrp(uint8_t    npin,
                    char        *name,
                    GBcallback  OnPress,
                    GBcallback  OnRelease,
                    GBcallback  OnLong,
                    uint8_t     hasRepeat,
                    uint8_t     *mirrorvar,
                    uint8_t     mirrorbit
                    )
: Button(npin, 0, name, mirrorvar, mirrorbit),
_OnPress(OnPress), _OnRelease(OnRelease), _OnLong(OnLong)
{
    setRepeat(hasRepeat);
}

ButtonGrp::ButtonGrp(uint8_t    npin,
                    uint16_t    codeh,
                    uint16_t    codel,
                    GBcallback  OnPress,
                    GBcallback  OnRelease,
                    GBcallback  OnLong,
                    uint8_t     hasRepeat,
                    uint8_t     *mirrorvar,
                    uint8_t     mirrorbit
                    )
: Button(npin, 0, codeh, codel, mirrorvar, mirrorbit),
_OnPress(OnPress), _OnRelease(OnRelease), _OnLong(OnLong)
{
    setRepeat(hasRepeat);
}

void ButtonGrp::check(Button::ButtonStatus_t ival)
{
    flagChg(flags, F_lastState, ((ival & S_Curr) != 0));

    if ((ival & S_Dn) /*&& (cur == HIGH)*/) {
        if (_OnPress)   _OnPress(this);
        setBit();
    }
    if (ival & S_Up) {
        if (_OnRelease) _OnRelease(this);
        clearBit();
    }
    if ((ival & S_Rpt) && (flags & F_hasRepeat) /*&& (cur == HIGH)*/) {
        if (_OnPress)   _OnPress(this);
    }
    if ((ival & S_Long) /*&& (cur == HIGH)*/) {
        if (_OnLong)    _OnLong(this);
    }
}

void ButtonGrp::initState(Button::ButtonStatus_t ival)
{
    if ((ival & S_Curr) != 0) {
        if (_OnPress)   _OnPress(this);
        setBit();
    } else {
        if (_OnRelease) _OnRelease(this);
        clearBit();
    }
}
