//===================================================
//  config_board-01_Radio.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.

#include <binary.h>

// Clean-slate the #define namespace
#include "config_board-undef.inc"

#define BOARDTYPE   M10_01_Radio1

#define MAXBUTTONS 10
#define MAXENCS 8
#define MAXCMDEVENTS 20

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

#define VIEWPORT1       {1,3, 5} //6}
#define VIEWPORT2       {1,11,5} //6}
#define VIEWPORT3       {2,3, 5} //6}
#define VIEWPORT4       {2,11,5} //6}
#define VIEWPORT5       {0,0,0}
#define VIEWPORT6       {0,0,0}

// ==================================================
//  Constants specific to the board
// ==================================================

// TODO Add board-specific prefix to avoid name clashes!
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

// end