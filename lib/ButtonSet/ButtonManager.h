// =======================================================================
// @file        ButtonManager.h
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-08-09 18:09
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================

#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <Arduino.h>
#include "Button.h"

// If the following "BM_STRAIGHT" token is defined, the manager only reports the current "static",
// i.e. not timing-related values (current value, change flags) to buttons.
// Button objects will then manage every timing-related aspect such as debounce, long press, repeat etc.
// Otherwise, the manager processes these events on an input vector level and presents 
// a complete set of flags to the button object.
// This latter solution allows much simpler button objects (basically just callback invokers),
// but has a less accurate implementation: timings of all buttons interfere, therefore it is
// reasonably suitable only if a single button is "active" (i.e. changes state, waits for debounce, is on repeat...)
// at any given time.

#define BM_STRAIGHT

// define the max number of buttons
// #ifndef MAXBUTTONS
// #define MAXBUTTONS 24
// #endif

// #define NBANKS  ((MAXBUTTONS+7)/8)

template<uint8_t MAXSIZE>
class ButtonManager
{
    uint8_t         numButtons;
    uint8_t         currBut;
    Button          *buttons[MAXSIZE] = {nullptr};
    constexpr uint8_t NBanks = ((MAXSIZE&0x07)+1)>>3;

    // Event flags for digital I/O banks
    // Each status marker for an element is 1 byte wide, because it contains the corresponding flags
    // for all 8 bits of the (element=)bank.
    using _flags = struct {
        uint8_t    LastIO;
        uint8_t    Change;
        uint8_t    Down;
        uint8_t    Up;
        uint8_t    Repeat;
        uint8_t    LongP;
        uint8_t    LPflag;
    };
    
    _flags vec[NBanks] = {0};

    // Pointers to analog input values (externally supplied)
    uint8_t         *analogVals;
    uint8_t         nAnaVals;

    // Vars for timing
    uint8_t         debounceTime;   // internally in ms
    unsigned long   lastPress;
    unsigned long   lastChange;
    unsigned long   lastRepeat;
    uint8_t         longPDelay;     // internally in ms*100; range 100ms..25.5s
    uint8_t         repeatDelay;    // internally in ms*100; range 100ms..25.5s
    uint8_t         repeatInterval; // internally in ms*10; range 10ms..2.55s

    // Helper for checkButtons()/initButtons()
    void _checkInit(uint8_t *vecIO, uint8_t doinit=0);

public:

    //static ButtonGroupManager *instance();
    explicit 
    ButtonManager(uint16_t lpDelay=600, uint16_t rptDelay=400, uint16_t rptRate=200);

    // Parameter management for digital I/Os:

    // Sets the debounce time (milliseconds)
    void setDebounce(unsigned int delay)    { debounceTime = delay; };
    // Sets the delay (milliseconds) before the long-keypress event is triggered
    // Rounded to nearest 100 ms; effective range 100ms..25.5s
    void setLongPDelay(uint16_t delay);
    // Sets the delay (milliseconds) before the keypress event is repeated
    // Rounded to nearest 100 ms; effective range 100ms..25.5s
    void setRepeatDelay(uint16_t delay);
    // Sets the time (milliseconds) between each repeated keypress event
    // Rounded to next 10 ms; effective range 10ms..2.55s
    void setRepeatRate(uint16_t repeat);
    // Return the pressure duration (milliseconds since the input vector was validated)
    uint16_t getPressTime(void) {return (uint16_t)(millis() - lastChange);}

    // Supply of references to externally managed Analog inputs:
    uint8_t setAnalogSource(uint8_t *aVals, uint8_t nVals);

    // Polling:

    // WARNING: in vecIO, '1' bits are considered ACTIVE,
    // therefore any input inversion (ie switches/pushbuttons closing to GND when active)
    // must have been handled in advance.
    void checkButtons(uint8_t *vecIO);

    // initButtons is used to assign the initial value.
    // It differs from checkButtons() because it signals a transition on all inputs.
    // This is usually required for toggle/rocker switches (as opposed to temporary pushbuttons),
    // which require to have their position recorded at startup
    void initButtons(uint8_t *vecIO);

    // Collection management:
    Button *add(Button* but);
    Button *get(uint8_t nBut = 0xFF);
    Button *next(uint8_t nBut = 0xFF);

};

//===================================================================================
//   Implementation of templated method bodies
//===================================================================================

#include "ButtonManager.hpp"

#endif
