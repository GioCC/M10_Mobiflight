// =======================================================================
// @file        M10BoardConfig.h
//
// @project     M10_Mobiflight
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-07-19
// @modifiedby  GiorgioCC - 2023-09-29 14:32
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __M10BOARD_CFG__H__
#define __M10BOARD_CFG__H__

// This struct defined the position of a LED 
// driven as a MAX7219's individual segment 
using LEDonMAX = struct {
    byte unit     : 2;
    byte digit    : 3;
    byte segment  : 3;
};

// typedef struct { //M10BoardConfig
using M10BoardConfig = struct {
    
    //! TODO (M10) Add control pins on the Mega for the specific board:
    //! PX_SSn, (PX_IRQn), LD_CSAn/LD_CSBn, (LCD_ENn)
    
    // pins not defined as either input or output are configured as inputs anyway, however it is apparent from the definition that they are unused
    uint16_t    digInputs;      // Map of digital inputs (1=input) for MCP on main board
    uint16_t    digOutputs;     // Map of digital outputs (1=output) for MCP on main board
    
    bool        hasBank2;
    // following are used only for those panels having a second extension board (currently only Autopilot, LED or LCD)
    uint16_t    digInputs2;     // Map of digital inputs (1=input) for MCP on second board
    uint16_t    digOutputs2;    // Map of digital outputs (1=output) for MCP on second board

    uint16_t    anaInputs;      // Map of analog inputs used

    // Number of encoders (actually connected; max 6)
    // Encoders #1,#2,#3 are allocated on DIO pins 1..3, 4..6 and 7...9; pins used are defined as inputs overriding the definition as GPIO.
    // Encoders #4,#5,[#6] are allocated on DIO pins 1..3, 4..6 [and 7...9] on the extension board.
    uint8_t     nEncoders;
    // Number of virtual (logical) encoders
    uint8_t     nVirtEncoders;
    bool        hasDisplays;
    bool        hasLCD;

    uint8_t     nLEDsOnMAX = 0;
    LEDonMAX    *LEDsOnMAX = nullptr;

    union {
        struct {
            // LED display drivers (actually connected; max 4)
            uint8_t     nDisplays1;         // Number of displays on the first port (0,1,2) - These are numbered 1 / 2
            uint8_t     nDisplays2;         // Number of displays on the second port (0,1,2) - These are numbered 3 / 4
        }; // led;  // uncomment if compiler doesn't allow anon structs (gcc should)
        struct {
            // LCD display
            uint8_t     LCDCols;
            uint8_t     LCDLines;
        }; //lcd;  // uncomment if compiler doesn't allow anon structs (gcc should)
    };
// } M10BoardConfig;
};


#endif  //M10BOARD_CFG__H