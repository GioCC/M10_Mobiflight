//===================================================
//  config_board-06_Multi_LCD.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.

#include <binary.h>

// Clean-slate the #define namespace
#include "config_board-undef.h"

#define BOARDTYPE   M10_06_Multi_LCD

#define HAS_LCD

#define MAXBUTTONS 10
#define MAXENCS 10
// #define MAXCMDEVENTS 40

// I/O numbering:           16......9  8......1
#define DIG_INPUTS      pat(B11111000,B00000000)
#define DIG_OUTPUTS     pat(B00000000,B11111000)
#define DIG_INPUTS2     pat(B00000000,B00000000)
#define DIG_OUTPUTS2    pat(B00000000,B00000000)
#define ANA_INPUTS      pat(B00000000,B00000000)
#define N_ENCODERS      1
#define N_VIRT_ENCODERS 8
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

// ==================================================
//  Constants specific to the board
// ==================================================

// TODO Add board-specific prefix to avoid name clashes!

// end