//===================================================
//  config_board-09_EFIS.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.

#include <binary.h>

// Clean-slate the #define namespace
#include "config_board-undef.inc"

#define BOARDTYPE   M10_09_EFIS

// M10_09_EFIS has an additional "Ana-to-Dig" 8-pos digital rotary switch based on analog input A1;
// it is not defined anyhow in the 'M10board_cfg' struct, therefore it must be managed separately

#define BANK2           1

#define MAXBUTTONS 32
#define MAXENCS     1
//#define MAXCMDEVENTS 20

// I/O numbering:           16......9  8......1
#define DIG_INPUTS      pat(B11111111,B11111111)
#define DIG_OUTPUTS     pat(B00000000,B00000000)
#define DIG_INPUTS2     pat(B11111111,B11111111)
#define DIG_OUTPUTS2    pat(B00000000,B00000000)
#define ANA_INPUTS      pat(B00000000,B00000010)
#define N_ENCODERS      2
#define N_VIRT_ENCODERS 4

#define N_DISPLAYS1     0
#define N_DISPLAYS2     0

#define N_LCD           0

// ==================================================
//  Constants specific to the board
// ==================================================

// TODO Add board-specific prefix to avoid name clashes!

// end