// =======================================================================
// @file        M10board_cfg.h
//
// @project     M10_Mobiflight
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-07-19
// @modifiedby  GiorgioCC - 2023-07-19 10:52
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __M10BOARD_CFG__H__
#define __M10BOARD_CFG__H__

// using  M10board_cfg = struct {
typedef struct { //M10board_cfg
    
    //TODO (M10) Add control pins
    
    // pins not defined as either input or output are configured as inputs anyway, however it is apparent from the definition that they are unused
    uint16_t    digInputs;      // Map of digital inputs (1=input) for MCP on main board
    uint16_t    digOutputs;     // Map of digital outputs (1=output) for MCP on main board
    
    bool        hasBank2;
    // following are used only for those panels having a second extension board (currently only Autopilot, LED or LCD)
    uint16_t    digInputs2;     // Map of digital inputs (1=input) for MCP on second board
    uint16_t    digOutputs2;    // Map of digital outputs (1=output) for MCP on second board

    uint16_t    anaInputs;      // Map of analog inputs used; remember that A0,A3,A4,A5 are preallocated (IORQ, DRDY, SDA, SCL). See comments.

    // Number of encoders (actually connected; max 6)
    // Encoders #1,#2,#3 are allocated on DIO pins 1..3, 4..6 and 7...9; pins used are defined as inputs overriding the definition as GPIO.
    // Encoders #4,#5,[#6] are allocated on DIO pins 1..3, 4..6 [and 7...9] on the extension board.
    uint8_t     nEncoders;
    // Number of virtual (logical) encoders
    uint8_t     nVirtEncoders;
    bool        hasDisplays;
    bool        hasLCD;

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
} M10board_cfg;
// };


#endif  //!__M10BOARD_CFG__H__