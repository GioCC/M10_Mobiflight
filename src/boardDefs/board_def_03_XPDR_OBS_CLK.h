//===================================================
//  config_board-03_XPDR_OBS_CLK.h
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

#define BOARDTYPE   M10_03_XPDR_OBS_CLK

#define MAXBUTTONS 10
#define MAXENCS 5
// #define MAXCMDEVENTS 20

// I/O numbering:           16......9  8......1
#define DIG_INPUTS      pat(B11111100,B00000000)
#define DIG_OUTPUTS     pat(B00000000,B00000000)
#define DIG_INPUTS2     pat(B00000000,B00000000)
#define DIG_OUTPUTS2    pat(B00000000,B00000000)
#define ANA_INPUTS      pat(B00000000,B00000000)
#define N_ENCODERS      3
#define N_VIRT_ENCODERS 0

#define N_IOEXP         1
#define N_DISPLAYS1     2
#define N_DISPLAYS2     0
#define N_LEDS_ON_MAX   10
#define LEDS_ON_MAX     { \
    {0, 3, 7}, \    
    {0, 2, 7}, \
    {0, 1, 7}, \
    {0, 0, 7}, \
    {0, 7, 0}, \
    {0, 7, 6}, \
    {0, 7, 3}, \
    {0, 7, 7}, \
    {1, 1, 7}, \
    {1, 3, 7}, \
}
#define N_LCD           0

#endif  // BUILDING_CONFIG_DATA

#ifdef BUILDING_CONFIG_RUNTIME

//===================================================================================
// RUNTIME OBJECT CONSTRUCTION FOR BOARD CONTROLS/DEVICES
//===================================================================================

// TODO Add board-specific prefix to avoid name clashes!
#define ENC_XDPR        1
#define ENC_OBS         2
#define ENC_CLK         3

#define PB_ENC_XPDR     3
#define PB_ENC_OBS      6
#define PB_ENC_CLK      9
#define SW_CLK_TMR      11
#define SW_XPDR_ON      12
#define PB_OBS_RVS      13
#define PB_CLK_START    14
#define PB_CLK_PAUSE    15
#define PB_CLK_ZERO     16

#define LD_XPDR_D1      DP1
#define LD_XPDR_D2      DP2
#define LD_XPDR_D3      DP3
#define LD_XPDR_D4      DP4
#define LD_OBS_RVS      DP8
#define LD_OBS_VOR1     SEG_A8
#define LD_OBS_VOR2     SEG_G8
#define LD_OBS_ADF      SEG_D8

#endif  // BUILDING_CONFIG_RUNTIME
// end