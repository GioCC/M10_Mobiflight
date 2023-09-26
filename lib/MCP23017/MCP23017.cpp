/*
  MCP23017.cpp
  Microchip MCP23017 I2C I/O Expander Class for Arduino

  Class MCP0: Class for MCP23017 built on base class MCP23x17
              Implicitly uses Arduino's default I2C (Wire) object (and library)

  Based by MCP23S17 class by Cort Buffington & Keith Neufeld
*/

#include "MCP23017.h"            // Header files for this class

// Constructor to instantiate an instance of MCP to a specific chip (address)

MCP0::MCP0(uint8_t hwaddress)
: MCP(), _ibuf{0}
{
    config(hwaddress);
    //init();
}

void
MCP0::config(uint8_t hwaddress)
{
    _hwaddress = 0x40+((hwaddress<<1)&0x0E);
}

void
MCP0::init(void)
{
    begin();
}

void
MCP0::begin()
{
    MCP::init();
}

char
MCP0::_read(char regaddr)
{
	Wire.beginTransmission(_hwaddress);
	Wire.write(regaddr);
	Wire.endTransmission(); // false); // for repeated start - required?
	Wire.requestFrom(_hwaddress, (byte)1);
	return Wire.read();
}

unsigned int
MCP0::_readW(char regaddr)
{
    uint16_t res;
	Wire.beginTransmission(_hwaddress);
	Wire.write(regaddr);
	Wire.endTransmission(); // false); // for repeated start - required?
	Wire.requestFrom(_hwaddress, (byte)2);
	res = Wire.read();
	res <<= 8;
    res |= (uint8_t)Wire.read();
    return res;
}

void
MCP0::_write(char regaddr, char data)
{
	Wire.beginTransmission(_hwaddress);
	Wire.write(regaddr);
	Wire.write(data);
	Wire.endTransmission();
}

void
MCP0::_writeW(char regaddr, unsigned int data)
{
  	Wire.beginTransmission(_hwaddress);
	Wire.write(regaddr);
	Wire.write((char)(data&0xFF));
	Wire.write((char)((data>>8)&0xFF));
    Wire.endTransmission();
}

