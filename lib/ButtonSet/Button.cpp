// =======================================================================
// @file        Button.cpp
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-07 17:20
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


#include "Button.h"

#ifdef USE_BTN_MGR
#include "ButtonManager.h"
#endif // USE_BTN_MGR

Button::Button(uint8_t npin, uint8_t useHWinput, const char *name, uint8_t *mirrorvar, uint8_t mirrorbit)
: _pin(npin)
{
    _tag.text = name;
    CButton(useHWinput, mirrorvar, mirrorbit);
}

Button::Button(uint8_t npin, uint8_t useHWinput, uint16_t tag, uint8_t *mirrorvar, uint8_t mirrorbit)
: _pin(npin)
{
    _tag.code = tag;
    CButton(useHWinput, mirrorvar, mirrorbit);
}

void
Button::CButton(uint8_t useHWinput, uint8_t *mirrorvar, uint8_t mirrorbit)
{
    valBit(0);  //lastState = LOW;
    flagChg(_flags, Button::rptEnabled, 1);  // Repeat enabled by default if present
    flagChg(_flags, Button::HWinput, useHWinput);
    setHW();
    mirror(mirrorvar, mirrorbit);
}

#ifdef USE_BTN_MGR
Button& 
Button::addTo(ButtonManager& mgr)
{ 
    mgr.add(this); 
    return *this; 
}
 
Button& 
Button::make(ButtonManager& mgr) 
{ 
    Button* pb = new Button; 
    pb->addTo(mgr); 
    return *pb; 
}
#include "new.h"
Button& 
Button::make(void* p, ButtonManager& mgr) 
{ 
    Button* pb = new(p) Button; 
    pb->addTo(mgr); 
    return *pb; 
}
#endif


// Button.cpp