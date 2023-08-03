//===================================================
//  config_board-undef.inc
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

#undef LEDS_ON_MAX
#undef ANA_INPUTS
#undef N_ENCODERS
#undef N_VIRT_ENCODERS

// Only required if the board has displays:
#undef N_DISPLAYS1
#undef N_DISPLAYS2

// Only required if the board has an LCD:
#undef LCD_COLS
#undef LCD_LINES

// end