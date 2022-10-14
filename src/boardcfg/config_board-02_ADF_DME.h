//===================================================
//  config_board-02_ADF_DME.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.

#include <Arduino.h>
#include <binary.h>

#ifndef BOARDTYPE
#define BOARDTYPE   M10_02_ADF_DME
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

#define ENC_DME_FREQ    1
#define ENC_ADF_FREQ    2

#define PB_ENC_DME      3
#define PB_ENC_ADF      6
#define SW_DME_RMT      13
#define SW_DME_GS_T     14
#define PB_ADF_FLT_ET   15
#define PB_ADF_SET_RST  16

#define LD_DME_KTS_MHZ  11
#define LD_ADF_FLT_ET   12

#define pat(a, b)   ((((uint16_t)(a))<<8)|(b))

#undef BANK2
#undef HAS_LCD

// Define number of control elements and input datarefs:
// Buttons/switches/digital inputs
#define MAXBUTTONS 10
// Encoders (INCLUDING VIRTUAL ONES!)
#define MAXENCS 4
// Datarefs (FS 'offsets') used
#define MAXCMDEVENTS 20

// Neither encoder nor enc. switches are to be considered as part of digital inputs;
// they have their own setup.
// I/O numbering:           16......9  8......1
#define DIG_INPUTS      pat(B11110000,B00000000)
#define DIG_OUTPUTS     pat(B00001100,B00000000)
#define DIG_INPUTS2     pat(B00000000,B00000000)
#define DIG_OUTPUTS2    pat(B00000000,B00000000)
#define ANA_INPUTS      pat(B00000000,B00000000)
#define N_ENCODERS      2
#define N_VIRT_ENCODERS 0
#define N_DISPLAYS1     2
#define N_DISPLAYS2     0
//#define LCD_COLS
//#define LCD_LINES

// LED Viewport definitions:
// Channel, start digit, len
// (all index-1 based)
// "Channel" is each sequence of MAX drivers; in each channel, digit numbering starts with 1
// (first MAX, leftmost digit) through 8 (first MAX, rightmost digit), 9 (2nd MAX, leftmost digit),
// 16 (2nd MAX, rightmost digit) and so on
#define VIEWPORT1       {0,0,0}
#define VIEWPORT2       {0,0,0}
#define VIEWPORT3       {0,0,0}
#define VIEWPORT4       {0,0,0}
#define VIEWPORT5       {0,0,0}
#define VIEWPORT6       {0,0,0}
//#define VIEWPORT7
//#define VIEWPORT8
//#define VIEWPORT9
//#define VIEWPORT10
//#define VIEWPORT11
//#define VIEWPORT12

// end