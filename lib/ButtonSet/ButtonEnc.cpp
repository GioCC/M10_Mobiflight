/*
*
* File     : ButtonEnc.cpp
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the AdvButton+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This file defines the ButtonEnc class
*
* Usage:
* - Include ButtonEnc.h and ButtonManager.h in your sketch
* - Declare each encoder and define the events using a overload of ButtonEnc
* - Declare the required event functions ( void OnXXX(ButtonEnc* enc) )
* - See the comments below for more help
*/

#include "ButtonEnc.h"

ButtonEnc::ButtonEnc(uint8_t    index,
                    char        *name,
                    EBcallback  OnKeyPress,
                    EBcallback  OnKeyRelease,
                    EBcallback  OnKeyLong,
                    uint8_t     *mirrorvar,
                    uint8_t     mirrorbit
                    )
: Button(index, 0, name, mirrorvar, mirrorbit),
_OnPress(OnKeyPress), _OnRelease(OnKeyRelease), _OnLong(OnKeyLong)
{
}


ButtonEnc::ButtonEnc(uint8_t    index,
                    uint16_t    codeh, uint16_t codel,
                    EBcallback  OnKeyPress,
                    EBcallback  OnKeyRelease,
                    EBcallback  OnKeyLong,
                    uint8_t     *mirrorvar,
                    uint8_t     mirrorbit
                    )
: Button(index, 0, codeh, codel, mirrorvar, mirrorbit),
_OnPress(OnKeyPress), _OnRelease(OnKeyRelease), _OnLong(OnKeyLong)
{
}

void
ButtonEnc::check(Button::ButtonStatus_t ival)
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
    if ((ival & S_Long) /*&& (cur == HIGH)*/) {
        if (_OnLong)    _OnLong(this);
    }
}

void ButtonEnc::initState(Button::ButtonStatus_t ival)
{
    if ((ival & S_Curr) != 0) {
        if (_OnPress)   _OnPress(this);
        setBit();
    } else {
        if (_OnRelease) _OnRelease(this);
        clearBit();
    }
}

