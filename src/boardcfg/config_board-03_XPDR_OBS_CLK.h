//===================================================
//  config_board-03_XPDR_OBS_CLK.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.

#include <Arduino.h>
#include <binary.h>

#ifndef BOARDTYPE
#define BOARDTYPE   M10_03_XPDR_OBS_CLK
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

#define ENC_XDPR        1
#define ENC_OBS         2
#define ENC_CLK         3

#define PB_ENC_XPDR     3
#define PB_ENC_OBS      6
#define PB_ENC_CLK      9
#define SW_CLK_TMR      11
#define SW_XPDR_ON      12
#define PB_OBS_RVS      13
#define PB_CLK_START    14
#define PB_CLK_PAUSE    15
#define PB_CLK_ZERO     16

#define LD_XPDR_D1      DP1
#define LD_XPDR_D2      DP2
#define LD_XPDR_D3      DP3
#define LD_XPDR_D4      DP4
#define LD_OBS_RVS      DP8
#define LD_OBS_VOR1     SEG_A8
#define LD_OBS_VOR2     SEG_D8
#define LD_OBS_ADF      SEG_F8

#define pat(a, b)   ((((uint16_t)(a))<<8)|(b))

#undef BANK2
#undef HAS_LCD

// Define number of control elements and input datarefs:
// Buttons/switches/digital inputs
#define MAXBUTTONS 10
// Encoders (INCLUDING VIRTUAL ONES!)
#define MAXENCS 5
// Datarefs (FS 'offsets') used
#define MAXCMDEVENTS 20

// Neither encoder nor enc. switches are to be considered as part of digital inputs;
// they have their own setup.
// I/O numbering:           16......9  8......1
#define DIG_INPUTS      pat(B11111100,B00000000)
#define DIG_OUTPUTS     pat(B00000000,B00000000)
#define DIG_INPUTS2     pat(B00000000,B00000000)
#define DIG_OUTPUTS2    pat(B00000000,B00000000)
#define ANA_INPUTS      pat(B00000000,B00000000)
#define N_ENCODERS      3
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