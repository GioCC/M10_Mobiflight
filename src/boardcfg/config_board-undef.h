//===================================================
//  config_board-undef.h
//
// Board peripheral set compile-time configuration
//
// This file UNdefines the symbols used for the common parameters of board configurations.
// It is meant to be invoked by each board configuration header so as to 
// "recycle" the same parameter names and replace them with their own.
// It can also be used as a "cheat sheet" reminder for the set of parameter names.

// NO INCLUDE GUARDS - this file is meant to be included repeatedly in place!

// Convenience helper macro for Digital I/O banks:
#undef  pat(a, b)
#define pat(a, b)   ((((uint16_t)(a))<<8)|(b))
// Usage example:
//          I/O numbering:    16......9   8......1
//#define DIG_INPUTS      pat(B11111110, B00000000)

// ------------------------------------------------------------------------
//  ALL DEFINITIONS BELOW ARE MANDATORY except where otherwise specified
// ------------------------------------------------------------------------

// Currently available M10 board types:
// M10_01_Radio1
// M10_01_Radio2
// M10_02_ADF_DME
// M10_03_XPDR_OBS_CLK
// M10_04_AP
// M10_05_Radio_LCD
// M10_06_Multi_LCD
// M10_07_AP_LCD
// M10_08_Audio
// M10_09_EFIS

#undef BOARDTYPE

#undef BANK2        // <<< Only required if the board has 2nd bank
#undef HAS_LCD      // <<< Only required if the board has an LCD

// Number of control elements and input datarefs:
// Buttons/switches/digital inputs
#undef MAXBUTTONS
// Encoders (INCLUDING VIRTUAL ONES!)
#undef MAXENCS
// Datarefs (FS 'offsets') used
//#undef MAXCMDEVENTS

// Digital I/O banks
// Note: neither encoder nor enc. switches are to be considered
// as part of digital inputs; they have their own setup.

#undef DIG_INPUTS
#undef DIG_OUTPUTS
#undef DIG_INPUTS2
#undef DIG_OUTPUTS2

// Other peripherals:

#undef ANA_INPUTS
#undef N_ENCODERS
#undef N_VIRT_ENCODERS
#undef N_DISPLAYS1
#undef N_DISPLAYS2

// LED Viewport definitions:
// Channel, start_digit, len
// "Channel" is each sequence of MAX drivers (0..1).
// Within each channel, digit numbering goes from left to right:
// 1 (first MAX, leftmost digit) through 8 (first MAX, rightmost digit), and
// 9 (2nd MAX, leftmost digit) to 16 (2nd MAX, rightmost digit) (and so on)
// 
// LCD Viewport definitions:
// Row, Column, len  (R/C index 0-based)
// 
// Example:
// #define VIEWPORT1  {1, 3, 5}

#undef VIEWPORT1
#undef VIEWPORT2
#undef VIEWPORT3
#undef VIEWPORT4
#undef VIEWPORT5
#undef VIEWPORT6

// >>> All #defines below are only required if the board has an LCD:

#undef LCD_COLS
#undef LCD_LINES

#undef VIEWPORT7
#undef VIEWPORT8
#undef VIEWPORT9
#undef VIEWPORT10
#undef VIEWPORT11
#undef VIEWPORT12

// end