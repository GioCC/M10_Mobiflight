//===================================================
//  config_board-08_Kbd.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.

#include <binary.h>

// Clean-slate the #define namespace
#include "config_board-undef.inc"

#define BOARDTYPE   M10_08_Kbd

#define MAXBUTTONS 10
#define MAXENCS 0
//#define MAXCMDEVENTS 20

#define DIG_INPUTS      pat(B11111111,B00000000)
#define DIG_OUTPUTS     pat(B00000000,B11111111)
#define DIG_INPUTS2     pat(B00000000,B00000000)
#define DIG_OUTPUTS2    pat(B00000000,B00000000)
#define ANA_INPUTS      pat(B00000000,B00000000)
#define N_ENCODERS      0
#define N_VIRT_ENCODERS 0

#define N_DISPLAYS1     0
#define N_DISPLAYS2     0

#define N_LCD           0

// ==================================================
//  Constants specific to the board
// ==================================================

// TODO Add board-specific prefix to avoid name clashes!

// end