//===================================================
//  board_def_board.inc
//
// Board peripheral set compile-time configuration
//
// This file is used by config_board.c as a template to build initialization lines
// for the BoardCfg[] array.
// The values below must match the attributes of the <M10BoardConfig> struct.

// >>> NO INCLUDE GUARDS <<< - this file is not a .h, it is meant to be included repeatedly in place!

{
    BOARDTYPE,
    DIG_INPUTS,
    DIG_OUTPUTS,
    (N_IOEXP==2),       // was 'HAS_BANK2'
    DIG_INPUTS2,
    DIG_OUTPUTS2,
    ANA_INPUTS,
    N_ENCODERS,         // (physically connected Encoders)
    N_VIRT_ENCODERS,    // (virtual Encoders provided by M10Board.cpp)
    HAS_DISPLAYS,
    HAS_LCD,
#ifdef N_LEDS_ON_MAX
    N_LEDS_ON_MAX,
    LEDS_ON_MAX,
#else
    0,
    nullptr,
#endif
    // union of anon structs (led or lcd):
#if(HAS_DISPLAYS)
    { N_DISPLAYS1, N_DISPLAYS2 }
#elif(HAS_LCD)
    { LCD_COLS, LCD_LINES }
#else 
    {0,0}
#endif
},

// end