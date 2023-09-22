//===================================================
//  config_board-02_ADF_DME.h
//
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set for a specific board type.
// It is divided in two parts (selectively activated through the 
// BUILDING_CONFIG_DATA and BUILDING_CONFIG_RUNTIME symbols):
// the first is used to declare the vector of configuration records 
// BoardCfgs[] (in config_board.cpp),
// whereas the second is used in the initialization phase.

#include <binary.h>

#ifdef BUILDING_CONFIG_DATA

//===================================================================================
// CONSTANTS USED TO BUILD THE CONFIGURATION DATA STRUCT
//===================================================================================

#define BOARDTYPE   M10_02_ADF_DME

#define MAXBUTTONS 10
#define MAXENCS 4
// #define MAXCMDEVENTS 20

// I/O numbering:           16......9  8......1
#define DIG_INPUTS      pat(B11110000,B00000000)
#define DIG_OUTPUTS     pat(B00001100,B00000000)
#define DIG_INPUTS2     pat(B00000000,B00000000)
#define DIG_OUTPUTS2    pat(B00000000,B00000000)
#define ANA_INPUTS      pat(B00000000,B00000000)
#define N_ENCODERS      2
#define N_VIRT_ENCODERS 0
#define N_DISPLAYS1     2
#define N_DISPLAYS2     0
#define N_LCD           0

#endif  // BUILDING_CONFIG_DATA

#ifdef BUILDING_CONFIG_RUNTIME

//===================================================================================
// RUNTIME OBJECT CONSTRUCTION FOR BOARD CONTROLS/DEVICES
//===================================================================================

// TODO Add board-specific prefix to avoid name clashes!
#define ENC_DME_FREQ    1
#define ENC_ADF_FREQ    2

#define PB_ENC_DME      3
#define PB_ENC_ADF      6
#define SW_DME_RMT      13
#define SW_DME_GS_T     14
#define PB_ADF_FLT_ET   15
#define PB_ADF_SET_RST  16

#define LD_DME_KTS_MHZ  11
#define LD_ADF_FLT_ET   12

#endif  // BUILDING_CONFIG_RUNTIME

// end