#ifndef CONFIG_BUILD_H_INCLUDED
#define CONFIG_BUILD_H_INCLUDED

#include "bitmasks.h"
#include "FastArduino.h"

//==========================================
//  Board role compile-time configuration
//
// This file defines the communication role of the board, i.e.
// if the board is just a controller (no peripherals), a standalone board, or a slave board.
// The peripheral set (for standalone or slave) is configured in config_board.h

// One of these three switches (to reduce code size) MUST be defined in main file:
// (preferably leave these here commented and put #define in Makefile / IDE project)
#define M10_STANDALONE   //-> (M10 board standalone) comms with host (Eth? or USB), disables I2C code
//#define CONTROLLER       //-> (Uno master board* that handles M10 units) comms with host (Eth? or USB) and I2C slaves. Disables I2C slave code and peripheral code.
//#define M10_SLAVE        //-> (M10 slave board) comms with I2C master, disables I2C slave code and host (ETH/USB) code
//
// * any board (Uno with attached peripherals, or M10 board) can act as a controller without need of setting switches;
//   the CONTROLLER switch is only used to get rid of all code that's not strictly relevant if the board should need space
//   for other purposes.
//
// Makefile settings
// Beyond switches in common files, following source modules must also be enabled/disabled during build:
// CONTROLLER
// - M10board, LEDdisplay, LedControlMod, MCP23S17, WireSlave
// + Wire (+Wire/utility/twi, +SPI)
//
// M10_STANDALONE
// + M10board, LEDdisplay, LedControlMod, MCP23S17 (+SPI), WireSlave
// - WireSlave, Wire
//
// M10_SLAVE
// + M10board, LEDdisplay, LedControlMod, MCP23S17 (+SPI), WireSlave, Wire (+Wire/utility/twi, +SPI)

// Floating point support can also be dropped, if you can dispense of it, to improve code size.
// 'NO_FLOAT' is only meaningful if __M10_Board__ is also defined!
//#define NO_FLOAT

//================================

#if defined(M10_STANDALONE)
#include "M10board.h"
// ??? #include <SPI.h>
// ??? #include <Ethernet.h>         // -- comment or delete these two lines if USB is used
#endif // M10_STANDALONE

#if defined(CONTROLLER)
//#include <SPI.h>
//#include <Ethernet.h>         // -- comment or delete these two lines if USB is used
// For I2C Master, use either Wire.h or SoftWire.h [https://github.com/felias-fogg/SoftI2CMaster], smaller & faster
// For I2C Slave, use Wire.h
#include <Wire.h>
//TwoWire Wire = TwoWire();
#endif // CONTROLLER

#if defined(M10_SLAVE)
#include "M10board.h"
#include <Wire.h>
//TwoWire Wire = TwoWire();
#include "WireSlave.h"
//WireSlave WSlave = WireSlave();
#endif // M10_SLAVE

#if (defined (ethernet_h) || defined (UIPETHERNET_H))
#define ETH_LAN
#endif

// XPD_FUNCTIONS are only available with Ethernet, not USB.
#if defined(ETH_LAN)
#define XPD_FUNCTIONS
#endif


#endif // CONFIG_BUILD_H_INCLUDED
