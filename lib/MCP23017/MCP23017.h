/*
  MCP23017.h
  Microchip MCP23017 I2C I/O Expander Class for Arduino

  Class MCP0: Class for MCP23017 built on base class MCP23x17
              Implicitly uses Arduino's default I2C (Wire) object (and library)
  
  Based by MCP23S17 class by Cort Buffington & Keith Neufeld
*/

#ifndef MCP23017_h
#define MCP23017_h

//#include "mbed.h"
//#include <I2C.h>    // Mbed I2C Master library
#include "MCP23x17.h"
#include <Arduino.h>
#include <Wire.h>

class MCP0 :
public MCP
{
  public:
    MCP0(uint8_t hwaddress);      // Beware: HWaddress is 7-bit (will be <<1 by the class)
    virtual void init(void);
    virtual void begin();                            // Get hold of the I2C Bus

  private:

    // HW address info
    uint8_t         _hwaddress;
    char            _ibuf[6];

    // These members are no longer virtual (they were pure virtual in the base class), 
    // but add virtual specifier to avoid "xxxx inherits implicit virtual" warning.
    virtual char            _read(char regaddr);
    virtual unsigned int    _readW(char regaddr);
    virtual void            _write(char regaddr, char data);
    virtual void            _writeW(char regaddr, unsigned int data);
};

#endif //MCP23017
