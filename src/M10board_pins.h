// =======================================================================
// @file        M10board_pins.h
//
// @project     M10_Mobiflight
// 
// @details     Constants for pin assignment
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-11-22
// @modifiedby  GiorgioCC - 2023-09-29 18:21
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


#ifndef M10BOARDPINS_H
#define M10BOARDPINS_H


#include <Arduino.h>

// ******* Unused pins:
// Serial #0
// Serial #2
// Serial #3
// I2C
// AD0...AD6
// Pin #53 (unconnected!)


// ******* Common pins

constexpr int8_t    PX_MISO = 50;
constexpr int8_t    PX_MOSI = 51;
constexpr int8_t    PX_SCK  = 52;

constexpr int8_t    LD_DO   = 18;
constexpr int8_t    LD_CK   = 19;

constexpr int8_t    LCD_RW  =  7;
constexpr int8_t    LCD_RS  =  6;
constexpr int8_t    LCD_D4  = 33;
constexpr int8_t    LCD_D5  = 32;
constexpr int8_t    LCD_D6  = 31;
constexpr int8_t    LCD_D7  = 30;
constexpr int8_t    LCD_PWM =  3;

// ******* Individual control pins

constexpr int8_t    PX_SSn[12]  = { 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38 }; 
constexpr int8_t    PX_IRQn[12] = { A8, A9, A10, A11, A12, A13, A14, A15, 10, 11, 12, 13 }; 

constexpr int8_t    LD_CSAn[12] = { 22, 23, 24, 25, 26, 27, -1, -1, -1, -1, -1 }; 
constexpr int8_t    LD_CSBn[12] = { 28, 29, 37, 36, 35, 34, -1, -1, -1, -1, -1 }; 

constexpr int8_t    LCD_ENn[8]  = { -1, -1, -1, -1, -1, 5, 2, 3 };
        
#endif // M10BOARDPINS_H
