/*
 *    LedControlMod.cpp - A library for controling Leds with a MAX7219/MAX7221
 *    Modified from LedControl.cpp Copyright (c) 2007 Eberhard Fahle
 *
      Modifications from original LedControl: (GCC 2017/08)
      - changed all "digitalRead"/"digitalWrite"/"shiftOut" to "FdigitalRead"/
        "FdigitalWrite"/"FshiftOut" for use with FastArduino library
      - Added SendDigits() and "no tx" arg for SetDigit and SetChar
	  - Added display width management
	  - Added SwitchOn & SwitchOff
	  - probably other minor tweaks
      The class name remained "LedControl" for compatibility.

 */


#include "LedControlMod.h"

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

LedControl::LedControl(byte dataPin, byte clkPin, byte csPin, byte numDevices)
: SPI_MOSI(dataPin), SPI_CLK(clkPin), SPI_CS(csPin)
{

    for(byte i=0; i<MAX_CHAINED*8; i++) {
        digits[i]=0x00;
    }
    setDeviceCount(numDevices, 0);	// No init yet. Also sets width[]
}

void LedControl::hw_init(void)
{
    pinMode(SPI_MOSI,OUTPUT);
    pinMode(SPI_CLK,OUTPUT);
    pinMode(SPI_CS,OUTPUT);
    FdigitalWrite(SPI_CS,HIGH);
}

void
LedControl::init(byte addr, byte lum)
{
    if(addr>=nUnits && addr != 0xFF) return;
    // Perform inits for specified unit(s)
    for(byte i=0; i<nUnits; i++) {
        if(addr==0xFF || addr==i) {
		    spiTransfer(i,OP_DISPLAYTEST,0);
		    // ...pause some ms?
            setScanLimit(i,width[i]);
            spiTransfer(i,OP_DECODEMODE,0);		//decode is done in source
            clearDisplay(i);
            setIntensity(i, lum);
            //shutdown(i,true);  				    //we go into shutdown-mode on startup
            shutdown(i,false);
        }
    }
}

void
LedControl::switchOn(byte addr, byte lum)
{
    if(addr>=nUnits && addr != 0xFF) return;

    for(byte i=0; i<nUnits; i++) {
        if(addr==0xFF || addr==i) {
        //clearDisplay(i);
        if(lum != 0xFF) setIntensity(i, lum);
        shutdown(i,false);
        }
    }
}

void
LedControl::switchOff(byte addr)
{
    if(addr>=nUnits && addr != 0xFF) return;

    for(byte i=0; i<nUnits; i++) {
        if(addr==0xFF || addr==i) {
        //clearDisplay(i);
        shutdown(i,true);
        }
    }
}

void
LedControl::setDeviceCount(byte numDevices, byte doinit)
{
    if(numDevices==0 || numDevices>8 ) return;

    // Set scanlimit value for added units
    if(numDevices>nUnits) {
        for(byte i=0; i<numDevices-nUnits; i++) {
            width[i]=8;     // Scanlimit at startup is 8(-1)
        }
    }
    nUnits=numDevices;
    if(doinit) {
        init(0xFF);
    }
}

void
LedControl::shutdown(byte addr, bool b)
{
    if(addr>=nUnits && addr != 0xFF) return;
    if(addr==0xff) {
        spiTransferAll(OP_SHUTDOWN, (b?0:1));
    } else {
        spiTransfer(addr, OP_SHUTDOWN, (b?0:1));
    }
}

void
LedControl::setScanLimit(byte addr, byte limit)
{
    if((addr>=nUnits && addr != 0xFF) || limit>8 || limit==0) return;

    limit--;
    if(addr==0xFF) {
        spiTransferAll(OP_SCANLIMIT, limit);
    } else {
        spiTransfer(addr, OP_SCANLIMIT, limit);
    }
}

void
LedControl::setIntensity(byte addr, byte intensity)
{
    if((addr>=nUnits && addr != 0xFF)|| intensity>=16) return;
    if(addr==0xff) {
        spiTransferAll(OP_INTENSITY, intensity);
    } else {
        spiTransfer(addr, OP_INTENSITY, intensity);
    }
}

void
LedControl::clearDisplay(byte addr)
{
    byte offset;
    if(addr>=nUnits && addr!=0xff) return;
    if(addr!=0xff) {
        offset=addr<<3;
        for(byte i=0; i<8; i++) {
            spiTransfer(addr, i+1,(digits[offset+i]=0));
        }
    } else {
        for(byte i=0; i<(MAX_CHAINED*8); i++) {
            digits[i]=0;
        }
        for(byte i=0; i<8; i++) {
            spiTransferAll(i+1, 0);
        }
    }

}

void
LedControl::setLed(byte addr, byte row, byte column, boolean state)
{
    byte offset;
    byte val=0x00;

    if(addr>=nUnits || row>7 || column>7) return;
    offset=addr*8;
    val=B10000000 >> column;
    if(state) {
        digits[offset+row]=digits[offset+row]|val;
    } else {
        val=~val;
        digits[offset+row]=digits[offset+row]&val;
    }
    spiTransfer(addr, row+1,digits[offset+row]);
}

void
LedControl::setRow(byte addr, byte row, byte value)
{
    byte offset;
    if(addr>=nUnits || row>7) return;
    offset=addr*8;
    digits[offset+row]=value;
    spiTransfer(addr, row+1,digits[offset+row]);
}

void
LedControl::setColumn(byte addr, byte col, byte value)
{
    if(addr>=nUnits || col>7) return;
    for(byte row=0; row<8; row++) {
        setLed(addr,row,col,(value>>(7-row))&0x01);
    }
}

void
LedControl::setWidth(byte addr, byte wdth)
{
    if((addr>=nUnits && addr != 0xFF) || wdth>8 || wdth==0) return;

    for(byte i=0; i<MAX_CHAINED; i++) {
        if(addr==0xFF || addr==i) {
            width[i] = wdth;
            spiTransfer(addr, OP_SCANLIMIT, wdth-1);
        }
    }
}

void
LedControl::setDigChr(byte type, byte addr, byte digit, byte value, boolean no_tx)
{
    byte offset;
    byte v;
    boolean dp;

    if(type==0 && value>15)     return;

    if(addr>=nUnits) return;
    if(digit >= width[addr]) return;

	digit += (8-width[addr]);
	dp = (value>127);	// bit7 is the marker for decimal point
	dp |= (fixedDPmask[addr]&dmask(digit));
    value &= 0x7F;
    offset=addr*8;
    v=pgm_read_byte_near(charTable + value);
    if(dp) {
        v|=B10000000;
    }
    if(digits[offset+digit]!=v) {
        digits[offset+digit]=v;
        if(no_tx) {
            digitchg[offset] &= (~dmask(digit));
        } else {
            spiTransfer(addr, digit+1,v);
        }
    }
}

void
LedControl::shiftBuf(void)
{
    FdigitalWrite(SPI_CS,LOW);      // enable the CS line
    for(byte i=(nUnits*2); i>0; i--) {
        FshiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
	}
    FdigitalWrite(SPI_CS,HIGH);     // latch the data onto the display
}

void
LedControl::spiTransfer(byte addr, volatile byte opcode, volatile byte data)
{
    register byte offset=addr*2;

    // All data NULL except the one we are interested in
    for(int i=0; i<nUnits*2; i++) {
        spidata[i]=(byte)0;
    }
    //put our device data into the array
    spidata[offset+1]=opcode;
    spidata[offset]=data;

    shiftBuf();
}


void
LedControl::spiTransferAll(volatile byte opcode, volatile byte data)
{
    // Same data for all units
    for(int i=0; i<nUnits*2; i++, i++) {
        spidata[i+1]=opcode;
        spidata[i]=data;
    }
    shiftBuf();
}

void
LedControl::transmit(boolean chgdonly)
{
    register byte digit;
    byte anysent = 1;
    byte ndig[MAX_CHAINED];         // Last digit changed for this unit
    for(byte u=0; u<MAX_CHAINED; u++) {
        ndig[u] = 0;
    }

    while(anysent) {
        anysent = 0;
        for(byte i=0; i<nUnits*2; i++) {
            spidata[i]=(byte)0;         // clean outbound array to be prepared
        }
        // for each unit, see if there are unsent digits
        for(byte u=0; u<nUnits; u++) {
            // Seek next digit to send for this unit
            digit = ndig[u];
            while(chgdonly && (digit<8) && (digitchg[u] & dmask(digit)) == 0) {
				digit++;
			}
            if(digit<8) {
                // digit found for that unit: fill corresponding range in tx buffer
                anysent = 1;
                spidata[2*u+1]  = digit+1;              // opcode (digit #);
                spidata[2*u]    = digits[(u<<3)+digit];   // value
                // Data in spidata[0]..[n]:
                // <U1val> <U1opc> <U2val> <U2opc>....
                // Data is shifted out:
                // - last unit in chain goes first
                // - for each unit, MSB first (=> high byte (opc) first; MSBit first)
                digitchg[u] &= (~dmask(digit));
                ndig[u] = ++digit;
            }
            // if no digit was found for this unit, a 0x00 filler remains in buffer:
            // it acts as a "no-op" command on that unit
        }
        if(anysent) {
            shiftBuf();
        }
    }
}

void
LedControl::setAllDigits(byte addr, byte *vals, byte dpmask, boolean no_tx)
{
    if(addr>=nUnits) return;
    byte msk = 0x01;
    byte dp;
    byte bv;
    byte w = width[addr];
    dpmask |= fixedDPmask[addr];
    for(byte i=0; i<w; i++, msk<<=1) {
        dp = ((dpmask&msk) != 0);
        bv = vals[w-1-i]|(dp?0x80:0);
        setDigChr(0, addr, i, bv, no_tx);
    }
}

void
LedControl::setAllChars(byte addr, byte *vals, byte dpmask, boolean no_tx)
{
    if(addr>=nUnits) return;
    byte msk = 0x01;
    byte dp;
    byte bv;
    byte w = width[addr];
    dpmask |= fixedDPmask[addr];
    for(byte i=0; i<w; i++, msk<<=1) {
        dp = ((dpmask&msk) != 0);
        bv = vals[w-1-i]|(dp?0x80:0);
        setDigChr(1, addr, i, bv, no_tx);
    }
}

void
LedControl::DUMMYtransmit(boolean chgdonly)
{
    UNUSED(chgdonly);
}

// END LedControlMod.cpp
