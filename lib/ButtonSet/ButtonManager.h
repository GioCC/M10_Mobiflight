/*
* This file defines the ButtonManager class.
*
*/

#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <Arduino.h>
#include "Button.h"
#include "ButtonGrp.h"
#include "ButtonEnc.h"
#include "ButtonBas.h"
#include "ButtonAdv.h"


#define LAST  (ButtonMgr.getButton())

// This include is just for usage with M10 boards (in order to tailor storage requirements); it is not for the general purpose library
// #include "config_board.h"   // This may define MAXBUTTONS

// define the max number of buttons
// This must be consistent with the length of the passed vector (currently an uint32).
// define MAXBUTTONS sizeof(Tvector)

// If other button types are used, or less buttons are needed, better specify the number explicitly
#ifndef MAXBUTTONS
#define MAXBUTTONS 64
#endif

#define NBANKS  ((MAXBUTTONS+7)/8)

//typedef uint8_t* TPvector;

class ButtonManager
{
    uint8_t         numButtons;
    uint8_t         currBut;
    Button          *buttons[MAXBUTTONS];

    // Event flags for digital I/O banks
    /*
    uint8_t    vecLastIO[NBANKS];
    uint8_t    vecDown[NBANKS];
    uint8_t    vecUp[NBANKS];
    uint8_t    vecRepeat[NBANKS];
    uint8_t    vecLongP[NBANKS];
    uint8_t    vecLPflag[NBANKS];
    */
    // Each status marker for an element is 1 byte wide, because it contains the corresponding flags
    // for all 8 bits of the (element=)bank.
    struct {
        uint8_t    LastIO;
        uint8_t    Down;
        uint8_t    Up;
        uint8_t    Repeat;
        uint8_t    LongP;
        uint8_t    LPflag;
    } vec[NBANKS];

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

    //static ButtonManager *s_instance;
    //ButtonManager();

    // Helper for checkButtons()/initButtons()
    void _checkInit(uint8_t *vecIO, uint8_t doinit=0);

public:

    //static ButtonGroupManager *instance();
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

// extern  ButtonManager ButtonMgr;

#endif
