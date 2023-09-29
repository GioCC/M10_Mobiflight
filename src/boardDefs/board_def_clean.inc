//===================================================
//  board_def_clean.inc
//
// Board peripheral set compile-time configuration
//
// This file UNdefines the symbols used for the common parameters of board configurations.
// It is meant to be invoked before including each board configuration header 
// (or at the beginning of the headers) so as to "recycle" the same parameter names
// and replace them with their own.
// It can also be used as a "cheat sheet" reminder for the set of parameter names.

// >>> NO INCLUDE GUARDS <<< - this file is not a .h, it is meant to be included repeatedly in place!

// Convenience helper macro for Digital I/O banks:
#ifdef  pat(a, b)
#undef  pat(a, b)
#define pat(a, b)   ((((uint16_t)(a))<<8)|(b))
#endif  //

// Usage example:
//          I/O numbering:    16......9   8......1
//#define DIG_INPUTS      pat(B11111110, B00000000)

// ------------------------------------------------------------------------
//  ALL DEFINITIONS BELOW ARE MANDATORY except where otherwise specified
// ------------------------------------------------------------------------

// Currently available M10 board types (enum T_BoardType):
// T_01_Radio2
// T_02_ADF_DME
// T_03_XPDR_OBS_CLK
// T_04_AP
// T_05_Radio_LCD
// T_06_Multi_LCD
// T_07_AP_LCD
// T_08_Audio
// T_09_EFIS

#undef BOARDTYPE

// Number of control elements and input datarefs:

// Buttons/switches/digital inputs
#undef MAXBUTTONS
// Encoders (INCLUDING VIRTUAL ONES!)
#undef MAXENCS

// Digital I/O banks
// Note: neither encoder nor enc. switches are to be considered
// as part of digital inputs; they have their own setup.

#undef DIG_INPUTS
#undef DIG_OUTPUTS
#undef DIG_INPUTS2
#undef DIG_OUTPUTS2

// Other peripherals:

#undef LEDS_ON_MAX
#undef ANA_INPUTS
#undef N_ENCODERS
#undef N_VIRT_ENCODERS

#undef N_IOEXP
#undef N_DISPLAYS1
#undef N_DISPLAYS2
#undef LCD_COLS
#undef LCD_LINES

// end