//===================================================
//  config_board-08_Kbd.h
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

#define BOARDTYPE   T_08_Kbd

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

#define N_IOEXP         1
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