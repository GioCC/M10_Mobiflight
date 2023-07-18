//===================================================
//  config_board-board.inc
//
// Board peripheral set compile-time configuration
//
// This file is used by config_board.c as a template to build initialization lines
// for the Boards[] array

// NO INCLUDE GUARDS - this file is meant to be included repeatedly in place!
{
    DIG_INPUTS,
    DIG_OUTPUTS,
    HAS_BANK2,
    DIG_INPUTS2,
    DIG_OUTPUTS2,
    ANA_INPUTS,
    N_ENCODERS,         // (physically connected Encoders)
    N_VIRT_ENCODERS,    // (virtual Encoders provided by M10Board.cpp)
    HAS_DISPLAYS,
    HAS_LCD,
    // union of anon structs (led or lcd):
#if(HAS_DISPLAYS)
    { N_DISPLAYS1, N_DISPLAYS2 }
#elif(HAS_LCD)
    { LCD_COLS, LCD_LINES }
#else 
    {0,0}
#endif
},

#undef HAS_DISPLAYS
// end