#ifndef CONFIG_BUILD_H_INCLUDED
#define CONFIG_BUILD_H_INCLUDED

#include "bitmasks.h"
#include "FastArduino.h"

// Floating point support can also be dropped, if you can dispense of it, to improve code size.
// 'NO_FLOAT' is only meaningful if __M10_Board__ is also defined!
//#define NO_FLOAT

//================================

#include "M10board.h"
// ??? #include <SPI.h>
// ??? #include <Ethernet.h>         // -- comment or delete these two lines if USB is used

#if (defined (ethernet_h) || defined (UIPETHERNET_H))
#define ETH_LAN
#endif

// XPD_FUNCTIONS are only available with Ethernet, not USB.
#if defined(ETH_LAN)
#define XPD_FUNCTIONS
#endif


#endif // CONFIG_BUILD_H_INCLUDED
