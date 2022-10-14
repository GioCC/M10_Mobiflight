/*
 *    LedControlOneMod.h - A library for controling Leds with a MAX7219/MAX7221
 *    Modified from LedControl.cpp Copyright (c) 2007 Eberhard Fahle
 *
 *    Same as LedControlMod but stripped down for no display chaining
 */

#ifndef LedControlOneMod_h
#define LedControlOneMod_h

#include <avr/pgmspace.h>

#include <Arduino.h>
#include "FastArduino.h"
#include "bitmasks.h"

#define dmask(i) (masks[7-i])
#define UNUSED(x) ((void)(x))

/*
 * Segments to be switched on for characters and digits on
 * 7-Segment Displays
 */
const static byte charTable [] PROGMEM  = {
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    B01111111,B01111011,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B10000000,B00000001,B10000000,B00000000,
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    B01111111,B01111011,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,B00000000,
    B00110111,B00000000,B00000000,B00000000,B00001110,B00000000,B00000000,B00000000,
    B01100111,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001000,
    B00000000,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,B00000000,
    B00110111,B00000000,B00000000,B00000000,B00001110,B00000000,B00010101,B00011101,
    B01100111,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
};

class LedControl {
    private:
        void _spiTransfer(byte opcode, byte data);    // Send out a single command to the device

        byte digits[8];     // content of all digits on eack block - digits[0] is the leftmost one (and the last shifted out)
        byte digitchg;      // Flags of which digits have changed since last transmission - Leftmost bit (7) is the leftmost digit
        byte spidata[2];    // The array for shifting the data to the devices
        byte SPI_MOSI;      // Data is shifted out of this pin
        byte SPI_CLK;       // Pin # for clock
        byte SPI_CS;        // driven LOW for chip selection
        byte nUnits;        // Actual number of chained devices

        // helper for setDigit(.) and setChar(.)
        void setDigChr(byte type, byte digit, byte value, boolean dp, boolean no_tx=0);

    public:
        /*
         * Most functions have a dummy 'addr' argument for signature compatibility
         * with the standard LedControlMod library.
        */

        /*
         * Create a new controller
         * Params :
         * dataPin		pin on the Arduino where data gets shifted out
         * clockPin		pin for the clock
         * csPin		pin for selecting the device
         * numDevices	maximum number of devices that can be controled
         */
        LedControl(byte dataPin, byte clkPin, byte csPin);

        /*
         * Gets the number of devices attached to this LedControl.
         * Returns :
         * byte	the number of devices on this LedControl
         */
        byte getDeviceCount(void) { return 1; }

        /*
         * Set the shutdown (power saving) mode for the device
         * Params :
         * status	If true the device goes into power-down mode. Set to false
         *		    for normal operation.
         */
        void shutdown(byte addr, bool status);

        /*
         * Set the number of digits (or rows) to be displayed.
         * See datasheet for sideeffects of the scanlimit on the brightness
         * of the display.
         * Params :
         * limit	number of digits to be displayed (1..8)
         */
        void setScanLimit(byte addr, byte limit);

        /*
         * Set the brightness of the display.
         * Params:
         * intensity	the brightness of the display. (0..15)
         */
        void setIntensity(byte addr, byte intensity);

        /*
         * Switch all Leds on the display off.
         */
        void clearDisplay(byte addr);

        /*
         * Set the status of a single Led.
         * Params :
         * row	the row of the Led (0..7)
         * col	the column of the Led (0..7)
         * state	If true the led is switched on,
         *		if false it is switched off
         */
        void setLed(byte addr, byte row, byte col, boolean state);

        /*
         * Set all 8 Led's in a row to a new state
         * Params:
         * row	row which is to be set (0..7)
         * value	each bit set to 1 will light up the
         *		corresponding Led.
         */
        void setRow(byte addr, byte row, byte value);

        /*
         * Set all 8 Led's in a column to a new state
         * Params:
         * col	column which is to be set (0..7)
         * value	each bit set to 1 will light up the
         *		corresponding Led.
         */
        void setColumn(byte addr, byte col, byte value);

        /*
         * Display a hexadecimal digit on a 7-Segment Display
         * Params:
         * digit	the position of the digit on the display (0..7)
         * value	the value to be displayed. (0x00..0x0F)
         * dp	    sets the decimal point.
         * tx	    transmits the digit immediately
         */
        void setDigit(byte addr, byte digit, byte value, boolean dp, boolean no_tx=0)
            { UNUSED(addr); setDigChr(0, digit, value, dp, no_tx); }

        /*
         * Display a character on a 7-Segment display.
         * There are only a few characters that make sense here :
         *	'0','1','2','3','4','5','6','7','8','9','0',
         *  'A','b','c','d','E','F','H','L','P',
         *  '.','-','_',' '
         * Params:
         * digit	the position of the character on the display (0..7)
         * value	the character to be displayed.
         * dp	sets the decimal point.
         */
        void setChar(byte addr, byte digit, char value, boolean dp, boolean no_tx=0)
            { UNUSED(addr); setDigChr(1, digit, value, dp, no_tx); }

        /*
         * Sends stored digits
         * Params:
         * none
         */
        void transmit(boolean chgdonly=1);

};

#endif	//LedControlOneMod.h
