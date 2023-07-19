// =======================================================================
// @file        config_board.h
//
// @project     M10-MobiFlight
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-07-19
// @modifiedby  GiorgioCC - 2023-07-19 10:53
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __CONFIG_BOARD__H__
#define __CONFIG_BOARD__H__

#include <Arduino.h>
#include "M10board_cfg.h"

extern const uint8_t        MaxBoards;
extern const M10board_cfg   BoardCfgs[] PROGMEM;

#endif  //!__CONFIG_BOARD__H__