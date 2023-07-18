//===================================================
//  config_board.c
//
// Board peripheral set compile-time configuration
//
// This file builds the definitions for:
// - the content of the const struct defining the configuration of all boards
// - the names of the individual peripherals of each board
// This is achieved by invoking the .h definition files for each board in sequence
// and using the contained macros to build the initial values for the config struct.

#include <Arduino.h>
#include "M10board.h"

// Currently available M10 board types:
enum {
    M10_01_Radio1,
    M10_01_Radio2,
    M10_02_ADF_DME,
    M10_03_XPDR_OBS_CLK,
    M10_04_AP,
    M10_05_Radio_LCD,
    M10_06_Multi_LCD,
    M10_07_AP_LCD,
    M10_08_Kbd,
    M10_09_EFIS,
};


constexpr uint8_t MaxBoards = 12;

#define HAS_DISPLAYS ((N_DISPLAYS1+N_DISPLAYS2)!=0)
#define HAS_LCD (N_LCD!=0)
#define HAS_BANK2 (defined(BANK2))

const
M10board_cfg Boards[MaxBoards] PROGMEM = {

#include "config_board-undef.inc"
#include "config_board-01_Radio.h"
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-01_Radio.h"
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-02_ADF_DME.h"
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-03_XPDR_OBS_CLK.h"
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-04_AP.h"
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-09_EFIS.h"
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-05_Radio_LCD.h"
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-06_Multi_LCD.h"
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-07_AP_LCD.h"
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-08_Kbd.h"    // AP
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-08_Kbd.h"    // Radio (Audio)
#include "config_board-board.inc"

#include "config_board-undef.inc"
#include "config_board-08_Kbd.h"    // Aux
#include "config_board-board.inc"

};

// end