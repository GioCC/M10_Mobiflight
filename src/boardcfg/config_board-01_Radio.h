//===================================================
//  config_board-01_Radio.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.

#include <Arduino.h>
#include <binary.h>

#ifndef BOARDTYPE
#define BOARDTYPE   M10_01_Radio
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

#define ENC_SIDE_A      1
#define ENC_SIDE_B      2

#define PB_ENC_A        3
#define PB_ENC_B        6
#define PB_PROGRAM      10
#define SW_COM_NAV_A    11
#define SW_1_2_A        12
#define SW_COM_NAV_B    13
#define SW_1_2_B        14
#define PB_SWAP_A       15
#define PB_SWAP_B       16

#define pat(a, b)   ((((uint16_t)(a))<<8)|(b))

#undef BANK2
#undef HAS_LCD

// Define number of control elements and input datarefs:
// Buttons/switches/digital inputs
#define MAXBUTTONS 10
// Encoders (INCLUDING VIRTUAL ONES!)
#define MAXENCS 8
// Datarefs (FS 'offsets') used
#define MAXCMDEVENTS 20

// Neither encoder nor enc. switches are to be considered as part of digital inputs;
// they have their own setup.
// I/O numbering:           16......9  8......1
#define DIG_INPUTS      pat(B11111110,B00000000)
#define DIG_OUTPUTS     pat(B00000000,B00000000)
#define DIG_INPUTS2     pat(B00000000,B00000000)
#define DIG_OUTPUTS2    pat(B00000000,B00000000)
#define ANA_INPUTS      pat(B00000000,B00000000)
#define N_ENCODERS      2
#define N_VIRT_ENCODERS 4
#define N_DISPLAYS1     2
#define N_DISPLAYS2     2
//#define LCD_COLS
//#define LCD_LINES

// LED Viewport definitions:
// Channel, start digit, len
// (all index-1 based)
// "Channel" is each sequence of MAX drivers; in each channel, digit numbering starts with 1
// (first MAX, leftmost digit) through 8 (first MAX, rightmost digit), 9 (2nd MAX, leftmost digit),
// 16 (2nd MAX, rightmost digit) and so on
#define VIEWPORT1       {1,3, 5} //6}
#define VIEWPORT2       {1,11,5} //6}
#define VIEWPORT3       {2,3, 5} //6}
#define VIEWPORT4       {2,11,5} //6}
#define VIEWPORT5       {0,0,0}
#define VIEWPORT6       {0,0,0}
//#define VIEWPORT7
//#define VIEWPORT8
//#define VIEWPORT9
//#define VIEWPORT10
//#define VIEWPORT11
//#define VIEWPORT12

// Uses Virtual Encoders provided by M10Board.cpp
/*
ManagedEnc COM1Enc(1, "COM1", NULL, );
ManagedEnc COM2Enc(1, "COM2", NULL, );
ManagedEnc NAV1Enc(1, "NAV1", NULL, );
ManagedEnc NAV2Enc(1, "NAV2", NULL, );
*/

// end