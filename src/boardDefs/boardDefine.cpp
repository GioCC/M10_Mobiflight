//===================================================
//  boardDefine.c
//
// Board peripheral set compile-time configuration
//
// This file builds the definitions for:
// - the content of the const struct defining the configuration of all boards
// - the names of the individual peripherals of each board
// This is achieved by invoking the .h definition files for each board in sequence
// and using the contained macros to build the initial values for the config struct.

#include "boardDefine.h"

namespace Config {

#define HAS_DISPLAYS ((N_DISPLAYS1+N_DISPLAYS2)!=0)
#define HAS_LCD (N_LCD!=0)
#define HAS_BANK2 (defined(BANK2))

#define BUILDING_CONFIG_DATA

const
M10BoardConfig BoardCfg[MAX_BOARDS] PROGMEM = {

#include "board_def_clean.inc"
#include "board_def_01_Radio.h"
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_01_Radio.h"
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_02_ADF_DME.h"
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_03_XPDR_OBS_CLK.h"
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_04_AP.h"
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_09_EFIS.h"
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_05_Radio_LCD.h"
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_06_Multi_LCD.h"
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_07_AP_LCD.h"
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_08_Kbd.h"    // AP
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_08_Kbd.h"    // Radio (Audio)
#include "board_def_board.inc"

#include "board_def_clean.inc"
#include "board_def_08_Kbd.h"    // Aux
#include "board_def_board.inc"

#undef BUILDING_CONFIG_DATA
};

}

// end