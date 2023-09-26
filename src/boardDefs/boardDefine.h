// =======================================================================
// @file        boardDefine.h
//
// @project     M10-MobiFlight
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-07-19
// @modifiedby  GiorgioCC - 2023-09-23 00:02
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __CONFIG_BOARD__H__
#define __CONFIG_BOARD__H__

#include <Arduino.h>

// This file declares the struct for objects containing 
// the configuration parameters for a board;
// therse are collected in an array defined in  
#include "M10board_cfg.h"

namespace Config {

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
} T_BoardType;

// ==========================================================
//  Define boards' own constants from the board cfg file
// ==========================================================

// This #define filters out the pin/IO assignment part from the include file
#define BUILDING_CONFIG_RUNTIME

#include "board_def_01_Radio.h"
#include "board_def_02_ADF_DME.h"
#include "board_def_03_XPDR_OBS_CLK.h"
#include "board_def_04_AP.h"
#include "board_def_05_Radio_LCD.h"
#include "board_def_06_Multi_LCD.h"
#include "board_def_07_AP_LCD.h"
#include "board_def_08_Kbd.h"
#include "board_def_09_EFIS.h"

#undef BUILDING_CONFIG_RUNTIME

//--------------------------------------------
// Costants
//--------------------------------------------

constexpr uint8_t MAX_BOARDS = 12;

//--------------------------------------------
// Exported vars
//--------------------------------------------

// Board parameter definitions
extern const M10board_cfg   BoardCfg[] PROGMEM;

// The BoardCfgs[] array is defined (as array of constants) in "config_board.cpp".
// Individual elements are defined through a template of constants (in 'board-def-board.inc'),
// whose values are redefined (through macros) between elements, through #defines contained
// in the 'board-def-<nn>*.inc' files (one per each actual board).

//--------------------------------------------
// Exported functions
//--------------------------------------------
// -none-


}

#endif  //!__CONFIG_BOARD__H__