/*
 *    LedControlMod.h - A library for controling Leds with a MAX7219/MAX7221
 *    Modified from LedControl.cpp Copyright (c) 2007 Eberhard Fahle
 *
      Modifications from original LedControl: (GCC 2017/01)
      - changed all "digitalRead"/"digitalWrite"/"shiftOut" to "FdigitalRead"/
        "FdigitalWrite"/"FshiftOut" for use with FastArduino library
      - Added SendDigits() and "no tx" arg for SetDigit and SetChar
      The class name remained "LedControl" for compatibility.

 */

#ifndef LedControlMod_h
#define LedControlMod_h

#include <avr/pgmspace.h>

#include <Arduino.h>
#include "FastArduino.h"
#include "bitmasks.h"

#define dmask(i) (wmasks[7-i])
#define UNUSED(x) ((void)(x))

#define MAX_CHAINED     2 //4

/*
 * Segments to be switched on for characters and digits on
 * 7-Segment Displays
 */
const static byte charTable [] PROGMEM  = {
    // Segments: .abcdefg (according to MAX72xx datasheet)
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    B01111111,B01111011,B01110111,B00011111,B01001110,B00111101,B01001111,B01000111,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B10000000,B00000001,B10000000,B00000000,
    B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,
    B01111111,B01111011,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B01110111,B00011111,B01001110,B00111101,B01001111,B01000111,B00000000,
    B00110111,B00000000,B00000000,B00000000,B00001110,B00000000,B00000000,B00000000,
    B01100111,B00000000,B00000000,B00000000,B01000110,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00001000,
    B00000000,B01110111,B00011111,B00001101,B00111101,B01001111,B01000111,B00000000,
    B00110111,B00000000,B00000000,B00000000,B00001110,B00000000,B00000000,B00000000,
    B01100111,B00000000,B00000000,B00000000,B00001111,B00000000,B00000000,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
};

class LedControl
{
private:
    byte SPI_MOSI;                  // Data is shifted out of this pin
    byte SPI_CLK;                   // Pin # for clock
    byte SPI_CS;                    // driven LOW for chip selection
    void spiTransfer(byte addr, byte opcode, byte data);    // Send out a single command to the device
    void spiTransferAll(byte opcode, byte data);            // Send out a single command to all devices

    byte nUnits;                    // Actual number of chained devices

    static byte spidata[MAX_CHAINED*2];    // The array for shifting the data to the devices
    byte digits[MAX_CHAINED*8];     // content of all digits on eack block - digits[0] is the leftmost one (and the last shifted out)
    byte digitchg[MAX_CHAINED];     // Flags of which digits have changed since last transmission - Leftmost bit (7) is the leftmost digit

    byte fixedDPmask[MAX_CHAINED];  // the mask of fixed DPs to display
    byte width[MAX_CHAINED];        // Defines display width (1..8) by accounting for scanlimit settings



    // helper for setDigit(.) and setChar(.)
    void setDigChr(byte type, byte addr, byte digit, byte value, boolean no_tx=0);
    // helper for spiTransfer(.) and spiTransferAll(.)
    void shiftBuf(void);

public:
    /*
     * Create a new controller.
     * Passed data is only provisionally stored - no other operation (e.g. pin setup)
     * is done at this stage, since they might have to be deferred to a later phase.
     * Actual setup is effected later by calling hw_init().
     * The constructor does also NOT send init messages.
     * Params :
     * dataPin		pin on the Arduino where data gets shifted out
     * clockPin		pin for the clock
     * csPin		pin for selecting the device
     * numDevices	number of devices attached
     */
    LedControl(byte dataPin, byte clkPin, byte csPin, byte numDevices=1);

    // HW resource initialization
    void hw_init(void);

    // Functional initialization
    void init(byte addr = 0xFF, byte lum=15);

    void switchOn(byte addr = 0xFF, byte lum=0xFF);
    void switchOff(byte addr = 0xFF);

    /*
     * Sets the number of devices attached to this LedControl.
     * Used for run-time modifications when the constructor parameters are fixed during compilation
     * If count had a larger previous value, unused displays are shut down.
     * Params :
     * numDevices	number of devices attached
     */
    void setDeviceCount(byte nUnits, byte doinit=0);

    /*
     * Gets the number of devices attached to this LedControl.
     * Returns :
     * byte	the number of devices on this LedControl
     */
    byte getDeviceCount() { return nUnits; };

    /*
     * Set the shutdown (power saving) mode for the device
     * Params :
     * addr	    The address of the display to control (or 0xFF for all)
     * status	If true the device goes into power-down mode. Set to false
     *		    for normal operation.
     */
    void shutdown(byte addr, bool status);

    /*
     * Set the number of digits to be displayed.
     * WARNING: This is the scanlimit value that is sent straight to the IC.
     * For "shorter" displays, the user is normally supposed to make use of
     * the setWidth(...) method, which assigns index 0 to the actual
     * leftmost start digit.
     * Important: See datasheet for sideeffects of the scanlimit value (<=3)
     * on the brightness of the display.
     * Params :
     * addr	    address of the display to control (or 0xFF for all)
     * limit	number of digits to be displayed (1..8)
     */
    void setScanLimit(byte addr, byte limit);

    /*
     * Set the brightness of the display.
     * Params:
     * addr		    the address of the display to control (or 0xFF for all)
     * intensity	the brightness of the display. (0..15)
     */
    void setIntensity(byte addr, byte intensity);

    /*
     * Switch all Leds on the display off.
     * Params:
     * addr	address of the display to control (or 0xFF for all)
     */
    void clearDisplay(byte addr);

    /*
     * Set the status of a single Led.
     * Does NOT account for scanlimit/width value.
     * Params :
     * addr	address of the display
     * row	the row of the Led (0..7)
     * col	the column of the Led (0..7)
     * state	If true the led is switched on,
     *		if false it is switched off
     */
    void setLed(byte addr, byte row, byte col, boolean state);

    /*
     * Set all 8 Led's in a row to a new state
     * Does NOT account for scanlimit/width value.
     * Params:
     * addr	address of the display
     * row	row which is to be set (0..7)
     * value	each bit set to 1 will light up the
     *		corresponding Led.
     */
    void setRow(byte addr, byte row, byte value);

    /*
     * Set all 8 Led's in a column to a new state
	 * Does NOT account for scanlimit/width value.
     * Params:
     * addr	address of the display
     * col	column which is to be set (0..7)
     * value	each bit set to 1 will light up the
     *		corresponding Led.
     */
    void setColumn(byte addr, byte col, byte value);

    /*
     * Set the display width.
     * This method, beside setting the scanlimit value for the IC,
     * also maps digit index 0 to the actual leftmost start digit.
     * Important: See datasheet for sideeffects of the scanlimit value (<=3)
     * on the brightness of the display.
     * Params :
     * addr	    address of the display to control (or 0xFF for all)
     * width	number of digits to be displayed (1..8)
     */
    void setWidth(byte addr, byte width);

    // Set the mask of fixed DPs.
    // These DPs are displayed regardless of the DP value transmitted for the corresponding digit.
    // Bit 0 in the pattern is always the rightmost digit.
    // Params :
    // unit	    address of the display to control
    // dpp	    DP pattern for digits (bit 0 is always the rightmost digit)
    //
    void setDPmask(byte unit, byte dpp)
	{
		fixedDPmask[unit] = dpp;
	}

    /*
     * Display a hexadecimal digit.
     * DOES account for width value.
     * Params:
     * addr	    address of the display in the daisy-chain
     * digit	the position of the digit on the display (0..<width>-1); 0 is leftmost
     * value    the value to be displayed. (0x00..0x0F)
     * dp	    sets the decimal point.
     * tx	    transmits the digit immediately
     */
    void setDigit(byte addr, byte digit, byte value, boolean dp=0, boolean no_tx=1)
    {
        setDigChr(0, addr, digit, (value|(dp?0x80:0)), no_tx);
    }

    /*
     * Display a character.
     * DOES account for width value.
     * There are only a few characters that make sense here :
     *	'0','1','2','3','4','5','6','7','8','9','0',
     *  'A','b','c','d','E','F','H','L','P',
     *  '.','-','_',' '
     * Params:
     * addr	    address of the display in the daisy-chain
     * digit	the position of the character on the display (0..<width>-1); 0 is leftmost
     * value    the character to be displayed.
     * dp	sets the decimal point.
     */
    void setChar(byte addr, byte digit, char value, boolean dp=0, boolean no_tx=1)
    {
        setDigChr(1, addr, digit, (value|(dp?0x80:0)), no_tx);
    }

    /*
     * Display a set of 8 hexadecimal digits
     * Params:
     * addr	    address of the display in the daisy-chain
     * vals	    the array of values to be displayed. (0x00..0x0F)
     * dpmask   pattern of decimal points, Bit 0 is always the rightmost digit!
     * tx	    transmits the digit immediately
     */
    void setAllDigits(byte addr, byte *vals, byte dpmask=0x00, boolean no_tx=0);

    /*
     * Display a a set of 8 characters.
     * Params:
     * addr	    address of the display in the daisy-chain
     * vals	    the array of values to be displayed
     * dpmask   pattern of decimal points. Bit 0 is always the rightmost digit!
     * tx	    transmits the digit immediately
     */
    void setAllChars(byte addr, byte *vals, byte dpmask=0x00, boolean no_tx=0);

    /*
     * Return the width set for the display
     * Params:
     * addr	    address of the display in the daisy-chain
    */
    byte getWidth(byte addr) { return (addr<MAX_CHAINED ? width[addr] : 0); }

    /*
     * Sends stored digits
     * Params:
     * none
     */
    void transmit(boolean chgdonly=1);

    void DUMMYtransmit(boolean chgdonly=1);
};

#endif	//LedControlMod.h
