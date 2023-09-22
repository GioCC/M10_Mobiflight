// =======================================================================
// @file        main.h
//
// @project     M10_Mobiflight
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-09-22
// @modifiedby  GiorgioCC - 2023-09-22 23:07
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __MAIN__H__
#define __MAIN__H__

#include <Arduino.h>
#include <stdint.h>
#include "memPool.h"
#include "M10board.h"

//--------------------------------------------
// Costants
//--------------------------------------------

constexpr uint16_t MEM_POOL_SIZE  (50*sizeof(ButtonBas)); // TODO

//--------------------------------------------
// Macros
//--------------------------------------------

// Pins for board setup shift registers
#define CFG_SR_LAT  7
#define CFG_SR_CLK  8
#define CFG_SR_DIN  9

// Utility macros
#define DW(a)   Serial.write(a)
#define DSPC    Serial.write(' ')
#define DP(a)   Serial.print(a)
#define DPN(a)  Serial.println(a)

//--------------------------------------------
// Global vars
//--------------------------------------------

extern memPool<MEM_POOL_SIZE>   pool;
extern M10board                 Board[Config::MAX_BOARDS];

// extern M10board_cfg    cfg;
// extern M10board        board;

//--------------------------------------------
// User vars
//--------------------------------------------

// extern int lights;

//--------------------------------------------
// Fn prototypes
//--------------------------------------------

// from boardSetup.cpp:
void boardSetup(void); 
bool isBoardAttached(uint8_t slot);    

//--------------------------------------------
// Management vars
//--------------------------------------------


#endif  //!__MAIN__H__
