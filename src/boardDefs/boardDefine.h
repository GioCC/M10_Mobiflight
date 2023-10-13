// =======================================================================
// @file        boardDefine.h
//
// @project     M10-MobiFlight
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-07-19
// @modifiedby  GiorgioCC - 2023-10-13 18:49
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __CONFIG_BOARD__H__
#define __CONFIG_BOARD__H__

#include <Arduino.h>

#define HAS_DISPLAY1 (N_DISPLAYS1!=0)
#define HAS_DISPLAY2 (N_DISPLAYS2!=0)
#define HAS_DISPLAYS ((N_DISPLAYS1+N_DISPLAYS2)!=0)
#define HAS_LCD (N_LCD!=0)

// This file declares the struct for objects containing 
// the configuration parameters for a board;
// therse are collected in an array defined in  
#include "M10BoardConfig.h"

namespace Config {

// Currently available M10 board types:
enum {
    T_01_Radio,
    T_02_ADF_DME,
    T_03_XPDR_OBS_CLK,
    T_04_AP,
    T_05_Radio_LCD,
    T_06_Multi_LCD,
    T_07_AP_LCD,
    T_08_Kbd,
    T_09_EFIS,
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

constexpr uint8_t MAX_BOARD_TYPES = 9;
constexpr uint8_t MAX_BOARDS      = 12;

// Following values should ideally be computed from definition files
// of individual boards, but let's not overthink it

// Max number of buttons across ALL boards
// constexpr uint8_t MAX_TOT_BUTTONS = (132 + 2);     // Leave 2 for margin
#define MAX_TOT_BUTTONS (132 + 2)     // Leave 2 for margin

// Max number of encoders across ALL boards
// constexpr uint8_t MAX_TOT_ENCS    = (52 + 2);     // Leave 2 for margin
#define MAX_TOT_ENCS    (52 + 2)     // Leave 2 for margin

//--------------------------------------------
// Exported vars
//--------------------------------------------

// Board parameter definitions
extern const M10BoardConfig   BoardCfg[] PROGMEM;

// The BoardCfg[] array is defined (as array of constants) in "config_board.cpp".
// Individual elements are defined through a template of constants (in 'board-def-board.inc'),
// whose values are redefined (through macros) between elements, through #defines contained
// in the 'board-def-<nn>*.inc' files (one per each actual board).

//--------------------------------------------
// Exported functions
//--------------------------------------------
// - none -

//--------------------------------------------
// Compute required allocation space
//--------------------------------------------

// Following computation must account for the (max) total number
// of actual connectable boards; the list sequence is therefore
// different from the one used for the initialization of BoardCfg[].

#define  ADD_BOARD \
    ( (sizeof(MCPS) * N_IOEXP) \
    + (sizeof(LedControl) * (HAS_DISPLAY1 ? 1 : 0)) \
    + (sizeof(LedControl) * (HAS_DISPLAY2 ? 1 : 0)) \
    + (sizeof(LiquidCrystal) * (HAS_LCD ? 1 : 0)) \
    )

#define BUILDING_CONFIG_DATA

constexpr uint16_t TotObjectMemSize(void) {
     return 0
#include "board_def_clean.inc"
#include "board_def_01_Radio.h"
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_01_Radio.h"
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_02_ADF_DME.h"
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_03_XPDR_OBS_CLK.h"
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_04_AP.h"
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_09_EFIS.h"
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_05_Radio_LCD.h"
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_06_Multi_LCD.h"
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_07_AP_LCD.h"
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_08_Kbd.h"    // AP
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_08_Kbd.h"    // Radio (Audio)
    + ADD_BOARD
#include "board_def_clean.inc"
#include "board_def_08_Kbd.h"    // Aux
    + ADD_BOARD
; }

#undef BUILDING_CONFIG_DATA

}

#endif  //!__CONFIG_BOARD__H__