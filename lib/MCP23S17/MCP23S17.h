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
    explicit MCPS(uint8_t hwaddress);
             MCPS(uint8_t hwaddress, uint8_t nCs_pin, uint8_t nReset_pin=0);
    void     config(uint8_t nCs_pin, uint8_t nReset_pin=0);
    void     init(void)  override;
    void     begin(void) override;

  private:

    uint8_t     _address;   // Address of the MCP23S17 in use
	uint8_t     _ss;        // Slave-select pin
	uint8_t     _rst;       // Reset pin
    const SPISettings _SPIset = { 1000000, MSBFIRST, SPI_MODE0 }; // will be passed at each SPI transaction begin

    // HW address info
    uint8_t      _writeopcode;
    uint8_t      _readopcode;
    void         _make_opcode(uint8_t _hwaddress);

    char         _read(char regaddr) override;
    unsigned int _readW(char regaddr) override;
    void         _write(char regaddr, char data) override;
    void         _writeW(char regaddr, unsigned int data) override;
};

#endif //MCP23S17
