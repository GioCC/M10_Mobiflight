//===================================================
//  config_board-04_AP.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.

#include <Arduino.h>
#include <binary.h>

#ifndef BOARDTYPE
#define BOARDTYPE   M10_04_AP
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

#define ENC_ALT         1
#define ENC_VS          2
#define ENC_SPD         3
#define ENC_HDG         4
#define ENC_CRS         5

#define PB_ENC_ALT      3
#define PB_ENC_VS       6
#define PB_ENC_SPD      9
#define PB_ENC_HDG      19
#define PB_ENC_CRS      22
#define PB_ALT          10
#define PB_VS           11
#define PB_SPD          12
#define PB_ATARM_ON     13
#define PB_ATARM_OFF    14
#define PB_AP_ON        15
#define PB_AP_OFF       16
#define SW_SPD_MACH     25
#define PB_HDG          26
#define PB_CRS          27
#define PB_BANK_SET     28
#define PB_LNAV         29
#define PB_VNAV         30
#define PB_APP          31
#define PB_REV          32

#define LD_ALT          SEG_A06
#define LD_VS           SEG_B06
#define LD_SPD          SEG_C06
#define LD_AT_ON        SEG_D06
#define LD_AP1_ON       SEG_E06
#define LD_AP2_ON       SEG_F06
#define LD_CRS          SEG_A27
#define LD_HDG          SEG_B27
#define LD_BANK1        SEG_C27
#define LD_BANK2        SEG_D27
#define LD_BANK3        SEG_E27
#define LD_BANK4        SEG_F27
#define LD_BANK5        SEG_G27
#define LD_LNAV         SEG_A28
#define LD_VNAV         SEG_B28
#define LD_APP          SEG_C28
#define LD_REV          SEG_D28

#define pat(a, b)   ((((uint16_t)(a))<<8)|(b))

#define BANK2
#undef HAS_LCD

// Define number of control elements and input datarefs:
// Buttons/switches/digital inputs
#define MAXBUTTONS 20
// Encoders (INCLUDING VIRTUAL ONES!)
#define MAXENCS 8
// Datarefs (FS 'offsets') used
#define MAXCMDEVENTS 30

// Neither encoder nor enc. switches are to be considered as part of digital inputs;
// they have their own setup.
// I/O numbering:           16......9  8......1
#define DIG_INPUTS      pat(B11111110,B00000000)
#define DIG_OUTPUTS     pat(B00000000,B00000000)
#define DIG_INPUTS2     pat(B11111111,B00000000)
#define DIG_OUTPUTS2    pat(B00000000,B00000000)
#define ANA_INPUTS      pat(B00000000,B00000000)
#define N_ENCODERS      5
#define N_VIRT_ENCODERS 0
#define N_DISPLAYS1     2
#define N_DISPLAYS2     1
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