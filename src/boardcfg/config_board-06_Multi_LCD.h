//===================================================
//  config_board-06_Multi_LCD.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.

#include <Arduino.h>
#include <binary.h>

#ifndef BOARDTYPE
#define BOARDTYPE   M10_06_Multi_LCD
#endif

// Currently available M10 board types:
// M10_01_Radio
// M10_02_ADF_DME
// M10_03_XPDR_OBS_CLK
// M10_04_AP
// M10_05_Radio_LCD
// M10_06_Multi_LCD
// M10_07_AP_LCD
// M10_08_Audio
// M10_09_EFIS

#define pat(a, b)   ((((uint16_t)(a))<<8)|(b))

#undef BANK2
#define HAS_LCD

// Define number of control elements and input datarefs:
// Buttons/switches/digital inputs
#define MAXBUTTONS 10
// Encoders (INCLUDING VIRTUAL ONES!)
#define MAXENCS 10
// Datarefs (FS 'offsets') used
#define MAXCMDEVENTS 40

// Neither encoder nor enc. switches are to be considered as part of digital inputs;
// they have their own setup.
// I/O numbering:           16......9  8......1
#define DIG_INPUTS      pat(B11111000,B00000000)
#define DIG_OUTPUTS     pat(B00000000,B11111000)
#define DIG_INPUTS2     pat(B00000000,B00000000)
#define DIG_OUTPUTS2    pat(B00000000,B00000000)
#define ANA_INPUTS      pat(B00000000,B00000000)
#define N_ENCODERS      1
#define N_VIRT_ENCODERS 8
//#define N_DISPLAYS1  
//#define N_DISPLAYS2
#define LCD_COLS        20  
#define LCD_LINES       4
#define VIEWPORT1       {0,0,0}
#define VIEWPORT2       {0,0,0}
#define VIEWPORT3       {0,0,0}
#define VIEWPORT4       {0,0,0}
#define VIEWPORT5       {0,0,0}
#define VIEWPORT6       {0,0,0}
#define VIEWPORT7       {0,0,0}
#define VIEWPORT8       {0,0,0}
#define VIEWPORT9       {0,0,0}
#define VIEWPORT10      {0,0,0}
#define VIEWPORT11      {0,0,0}
#define VIEWPORT12      {0,0,0}

// end