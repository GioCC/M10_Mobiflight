/*
  MCP23S17.h
  Microchip MCP23S17 SPI I/O Expander Class for Arduino

  Class MCPS: Class for MCP23S17 built on base class MCP23x17
              Implicitly uses Arduino's default SPI object (and library)

  Based by MCP23S17 class by Cort Buffington & Keith Neufeld
*/

#ifndef MCP23S17_h
#define MCP23S17_h

//#include "mbed.h"
#include "MCP23x17.h"
#include <SPI.h>                 // Arduino IDE SPI library - uses AVR hardware SPI features

class MCPS :
public MCP
{
  public:
    MCPS(uint8_t hwaddress, uint8_t nCs_pin, uint8_t nReset_pin=0);
    virtual void init(void);
    virtual void begin();

  private:

    uint8_t     _address;   // Address of the MCP23S17 in use
	uint8_t     _ss;        // Slave-select pin
	uint8_t     _rst;       // Reset pin
    SPISettings _SPIset;

    // HW address info
    uint8_t      _writeopcode;
    uint8_t      _readopcode;
    void         _make_opcode(uint8_t _hwaddress);

    // These members are no longer virtual (they were pure virtual in the base class),
    // but add virtual specifier to avoid "xxxx inherits implicit virtual" warning.
    virtual char         _read(char regaddr);
    virtual unsigned int _readW(char regaddr);
    virtual void         _write(char regaddr, char data);
    virtual void         _writeW(char regaddr, unsigned int data);
};

#endif //MCP23S17
