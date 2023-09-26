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
    explicit    MCP0(uint8_t hwaddress = 0);    // Beware: HWaddress is 7-bit (will be <<1 by the class)
    void        config(uint8_t hwaddress);
    void        init(void)  override;
    void        begin()     override;       // Get hold of the I2C Bus

  private:

    // HW address info
    uint8_t         _hwaddress;
    char            _ibuf[6];

    char            _read(char regaddr) override;
    unsigned int    _readW(char regaddr) override;
    void            _write(char regaddr, char data) override;
    void            _writeW(char regaddr, unsigned int data) override;
};

#endif //MCP23017
