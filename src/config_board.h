#ifndef CONFIG_BOARD_H_INCLUDED
#define CONFIG_BOARD_H_INCLUDED

//===================================================
//  Board peripheral set compile-time configuration
//
// This file defines the peripheral set of the board, i.e. for which actual model of board
// we are building the firmware.
// The communication role (controller, standalone or slave) is configured in config_build.h

#include <Arduino.h>
#include <binary.h>

#ifndef BOARDTYPE
#define BOARDTYPE   M10_01_Radio
#endif

// Currently available M10 board types:
// M10_01_Radio
// M10_02_ADF_DME
// M10_03_XPDR_OBS_CLK
// M10_04_AP
// M10_05_Radio_LCD
// M10_06_Multi_LCD
// M10_07_AP_LCD
// M10_08_Audio
// M10_09_EFIS

#if(BOARDTYPE == M10_01_Radio)
#include "boardcfg\config_board-01_Radio.h"
#elif(BOARDTYPE == M10_02_ADF_DME)
#include "boardcfg\config_board-03_XPDR_OBS_CLK.h"
#elif(BOARDTYPE == M10_03_XPDR_OBS_CLK)
#include "boardcfg\config_board-04_AP.h"
#elif(BOARDTYPE == M10_04_AP)
#include "boardcfg\config_board-05_Radio_LCD.h"
#elif(BOARDTYPE == M10_05_Radio_LCD)
#include "boardcfg\config_board-02_ADF_DME.h"
#elif(BOARDTYPE == M10_06_Multi_LCD)
#include "boardcfg\config_board-06_Multi_LCD.h"
#elif(BOARDTYPE == M10_07_AP_LCD)
#include "boardcfg\config_board-07_AP_LCD.h"
#elif(BOARDTYPE == M10_08_Audio)
#include "boardcfg\config_board-08_Audio.h"
#elif(BOARDTYPE == M10_09_EFIS)
#include "boardcfg\config_board-09_EFIS.h"
#endif

#endif // CONFIG_BOARD_H_INCLUDED
