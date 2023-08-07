// =======================================================================
// @file        ButtonEvt.cpp
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-08 13:35
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


#include "ButtonEvt.h"
EVBcallback ButtonEvt::_OnPress = nullptr;
EVBcallback ButtonEvt::_OnRelease = nullptr;
EVBcallback ButtonEvt::_OnLong = nullptr;


ButtonEvt::ButtonEvt(
    uint8_t     npin,
    char*       name,
    uint8_t     rptEnabled,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(npin, 0, name, mirrorvar, mirrorbit)
{
    enableRepeat(rptEnabled);
}

ButtonEvt::ButtonEvt(
    uint8_t     npin,
    uint16_t    code,
    uint8_t     rptEnabled,
    uint8_t*    mirrorvar,
    uint8_t     mirrorbit
)
: Button(npin, 0, code, mirrorvar, mirrorbit)
{
    enableRepeat(rptEnabled);
}

void
ButtonEvt::process(uint8_t sts)
{
    valBit(((sts & Button::Curr) != 0));
    if((sts & ~Button::Curr) == 0) return; 
    
    if ((sts & Button::Dn) && _OnPress)   _OnPress(this);
    if ((sts & Button::Up) && _OnRelease) _OnRelease(this);
    if ((sts & Button::Long) && _OnLong)  _OnLong(this);
    if ((sts & Button::Rpt) && (_flags & Button::rptEnabled) &&_OnPress) _OnPress(this);
}

// end ButtonEvt.cpp