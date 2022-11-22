//===================================================
//  config_board-line.h
//
// Board peripheral set compile-time configuration
//
// This file is used by config_board.c as a template to build initialization lines
// for the Boards[] array

// NO INCLUDE GUARDS - this file is meant to be included repeatedly in place!
{
    DIG_INPUTS,         // digInputs
    DIG_OUTPUTS,        // digOutputs
    DIG_INPUTS2,        // digInputs2
    DIG_OUTPUTS2,       // digOutputs2
    ANA_INPUTS,         // anaInputs
    N_ENCODERS,         // nEncoders        (physically connected Encoders)
    N_VIRT_ENCODERS,    // nVirtEncoders    (virtual Encoders provided by M10Board.cpp)
    // union of anon structs (led or lcd):
    {
#ifndef HAS_LCD
        N_DISPLAYS1,    // nDisplays1
        N_DISPLAYS2,    // nDisplays2
#else
        LCD_COLS,
        LCD_LINES,
#endif
    }
    // viewport[]:
    {
        VIEWPORT1,    // viewport #n (Disp#, start, len)
        VIEWPORT2,
        VIEWPORT3,
        VIEWPORT4,
        VIEWPORT5,
        VIEWPORT6,
        VIEWPORT7,
        VIEWPORT8,
        VIEWPORT9,
        VIEWPORT10,
        VIEWPORT11,
        VIEWPORT12,
    }
},

// end