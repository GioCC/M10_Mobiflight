//===================================================
//  config_board-09_EFIS.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set for a specific board type.
// It is divided in two parts (selectively activated through the 
// BUILDING_CONFIG_DATA and BUILDING_CONFIG_RUNTIME symbols):
// the first is used to declare the vector of configuration records 
// BoardCfg[] (in config_board.cpp),
// whereas the second is used in the initialization phase.

#include <binary.h>

#ifdef BUILDING_CONFIG_DATA

//===================================================================================
// CONSTANTS USED TO BUILD THE CONFIGURATION DATA STRUCT
//===================================================================================

#define BOARDTYPE   T_09_EFIS

// T_09_EFIS has an additional "Ana-to-Dig" 8-pos digital rotary switch based on analog input A1;
// it is not defined anyhow in the 'M10BoardConfig' struct, therefore it must be managed separately

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

#define N_IOEXP         2
#define N_DISPLAYS1     0
#define N_DISPLAYS2     0
#define N_LCD           0

#endif  // BUILDING_CONFIG_DATA

#ifdef BUILDING_CONFIG_RUNTIME

//===================================================================================
// RUNTIME OBJECT CONSTRUCTION FOR BOARD CONTROLS/DEVICES
//===================================================================================

// TODO Add board-specific prefix to avoid name clashes!

#endif  // BUILDING_CONFIG_RUNTIME

// end