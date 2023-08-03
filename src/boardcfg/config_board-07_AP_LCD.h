//===================================================
//  config_board-07_AP_LCD.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.

#include <binary.h>

// Clean-slate the #define namespace
#include "config_board-undef.inc"

#define BOARDTYPE   M10_07_AP_LCD

#define BANK2           1
#define HAS_LCD

#define MAXBUTTONS 20
#define MAXENCS 8
// #define MAXCMDEVENTS 30

// I/O numbering:           16......9  8......1
#define DIG_INPUTS      pat(B11111100,B00000000)
#define DIG_OUTPUTS     pat(B00000000,B11000000)
#define DIG_INPUTS2     pat(B11111111,B00000000)
#define DIG_OUTPUTS2    pat(B00000000,B11111111)
#define ANA_INPUTS      pat(B00000000,B00000000)
#define N_ENCODERS      1
#define N_VIRT_ENCODERS 5

#define N_DISPLAYS1     0
#define N_DISPLAYS2     0
#define N_LCD           1
#define LCD_COLS        20  
#define LCD_LINES       4

// ==================================================
//  Constants specific to the board
// ==================================================

// TODO Add board-specific prefix to avoid name clashes!

// end