/*
 *    LedControlOneMod.cpp - A library for controling Leds with a MAX7219/MAX7221
 *    Modified from LedControl.cpp Copyright (c) 2007 Eberhard Fahle
 *
 *    Same as LedControlMod but stripped down for no display chaining
 */


#include "LedControlOneMod.h"

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

LedControl::LedControl(byte dataPin, byte clkPin, byte csPin)
{
    SPI_MOSI=dataPin;
    SPI_CLK=clkPin;
    SPI_CS=csPin;
    pinMode(SPI_MOSI,OUTPUT);
    pinMode(SPI_CLK,OUTPUT);
    pinMode(SPI_CS,OUTPUT);
    FdigitalWrite(SPI_CS,HIGH);
    SPI_MOSI=dataPin;
    for(byte i=0;i<8;i++) {
        digits[i]=0x00;
        _spiTransfer(OP_DISPLAYTEST,0);
        //scanlimit is set to max on startup
        setScanLimit(0, 7);
        //decode is done in source
        _spiTransfer(OP_DECODEMODE,0);
        clearDisplay(0);
        //we go into shutdown-mode on startup
        shutdown(0, true);
    }
}

void
LedControl::shutdown(byte addr, bool b)
{
    UNUSED(addr);
    _spiTransfer(OP_SHUTDOWN, (b?0:1));
}

void
LedControl::setScanLimit(byte addr, byte limit)
{
    UNUSED(addr);
    if(limit>=8) return;
    _spiTransfer(OP_SCANLIMIT, limit);
}

void
LedControl::setIntensity(byte addr, byte intensity)
{
    UNUSED(addr);
    if(intensity>=16) return;
    _spiTransfer(OP_INTENSITY, intensity);
}

void
LedControl::clearDisplay(byte addr)
{
    UNUSED(addr);
    for(byte i=0;i<8;i++) {
        _spiTransfer(i+1,(digits[i]=0));
    }
}

void
LedControl::setLed(byte addr, byte row, byte column, boolean state)
{
    UNUSED(addr);
    byte val=0x00;

    if(row>7 || column>7) return;
    val=B10000000 >> column;
    if(state)
        digits[row]=digits[row]|val;
    else {
        digits[row]=digits[row]&(~val);
    }
    _spiTransfer(row+1,digits[row]);
}

void
LedControl::setRow(byte addr, byte row, byte value) {
    UNUSED(addr);
    if(row>7) return;
    digits[row]=value;
    _spiTransfer(row+1,digits[row]);
}

void
LedControl::setColumn(byte addr, byte col, byte value) {
    UNUSED(addr);
    if(col>7) return;
    for(byte row=0;row<8;row++) {
        setLed(0, row,col,(value>>(7-row))&0x01);
    }
}

void
LedControl::setDigChr(byte type, byte digit, byte value, boolean dp, boolean no_tx)
{
    byte v;

    if(digit>7) return;
    if(type==0 && value>15)
        return;
    if(value >127) {
        value=32;   //no printable chars defined beyond 127, so we use space
    }
    v=pgm_read_byte_near(charTable + value);
    if(dp) {
        v|=B10000000;
    }

    if(digits[digit]!=v) {
        digits[digit]=v;
        if(no_tx) {
            digitchg &= (~dmask(digit));
        } else {
            _spiTransfer(digit+1,v);
        }
    }
}

void
LedControl::_spiTransfer(volatile byte opcode, volatile byte data)
{
    spidata[1]=opcode;
    spidata[0]=data;
    FdigitalWrite(SPI_CS,LOW);
    FshiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[1]);
    FshiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[0]);
    FdigitalWrite(SPI_CS,HIGH);
}


void
LedControl::transmit(boolean chgdonly)
{
    for(byte dd=0;dd<8;dd++) {
        if(!chgdonly || ((digitchg & dmask(dd))!=0)) {
            _spiTransfer(dd+1, digits[dd]);
            digitchg &= (~dmask(dd));
        }
    }
}

// END LedControlOneMod.cpp
