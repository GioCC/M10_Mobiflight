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

EBcallback ButtonEnc::_OnPress = nullptr;
EBcallback ButtonEnc::_OnRelease = nullptr;
EBcallback ButtonEnc::_OnLong = nullptr;

ButtonEnc::ButtonEnc(
    uint8_t     index,
    char*       name,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(index, 0, name, mirrorvar, mirrorbit)
{}


ButtonEnc::ButtonEnc(
    uint8_t     index,
    uint16_t    code,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(index, 0, code, mirrorvar, mirrorbit)
{}

void
ButtonEnc::check(Button::ButtonStatus_t ival)
{
    flagChg(_flags, Button::lastState, ((ival & Button::Curr) != 0));

    if ((ival & Button::Dn) /*&& (cur == HIGH)*/) {
        setBit();
        if (_OnPress) _OnPress(this);
    }
    if (ival & Button::Up) {
        clearBit();
        if (_OnRelease) _OnRelease(this);
    }
    if ((ival & Button::Long) /*&& (cur == HIGH)*/) {
        if (_OnLong) _OnLong(this);
    }
}

void ButtonEnc::initState(Button::ButtonStatus_t ival)
{
    if ((ival & Button::Curr) != 0) {
        setBit();
        if (_OnPress) _OnPress(this);
    } else {
        clearBit();
        if (_OnRelease) _OnRelease(this);
    }
}

// end ButtonEnc.cpp