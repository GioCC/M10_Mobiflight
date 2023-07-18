/*
  MCP23S17.cpp
  Microchip MCP23S17 SPI I/O Expander Class for Arduino

  Class MCPS: Class for MCP23S17 built on base class MCP23x17
              Implicitly uses Arduino's default SPI object (and library)

  Based by MCP23S17 class by Cort Buffington & Keith Neufeld
*/

#include "MCP23S17.h"            // Header files for this class

// Here we have things for the SPI bus configuration
#define    CLOCK_DIVIDER (2)           // SPI bus speed to be 1/2 of the processor clock speed - 8MHz on most Arduinos
// Control byte and configuration register information - Control Byte: "0100 A2 A1 A0 R/W" -- W=0
#define    ADDR_ENABLE   (0b00001000)  // Configuration register for MCP23S17, the only thing we change is enabling hardware addressing



// Constructor to instantiate an instance of MCP to a specific chip (address)
// Requires init() (or begin()) to be called later
MCPS::MCPS(uint8_t hwaddress, uint8_t nCs_pin, uint8_t nReset_pin)
: MCP(hwaddress), _ss(nCs_pin), _rst(nReset_pin), _address(0)
{
    _make_opcode(hwaddress);
    //init();
}

// Constructor for delayed setup
// ONLY STORES VALUES, does not setup pins or activate peripherals yet
// Requires config(), then init() (or begin()) to be called later
MCPS::MCPS(uint8_t hwaddress) 
: MCP(hwaddress), _rst(0xFF), _ss(0xFF), _address(0)
{
    _make_opcode(hwaddress);
}

// Constructor to instantiate an instance of MCP to a specific chip (address)
// ONLY STORES VALUES, does not setup pins or activate peripherals yet
void MCPS::config(uint8_t nCs_pin, uint8_t nReset_pin)
{
    _ss  = nCs_pin; 
    _rst = nReset_pin;
}

void
MCPS::init(void)
{
    begin();
}

void
MCPS::begin() {
    ::pinMode(_ss, OUTPUT);               // Set SlaveSelect pin as an output
    ::digitalWrite(_ss, HIGH);           // Set SlaveSelect HIGH (chip de-selected)
    SPI.begin();                          // Start up the SPI bus
    //SPI.setClockDivider(CLOCK_DIVIDER); // Sets the SPI bus speed
    //SPI.setBitOrder(MSBFIRST);          // Sets SPI bus bit order (this is the default, setting it for good form!)
    //SPI.setDataMode(SPI_MODE0);         // Sets the SPI bus timing mode (this is the default, setting it for good form!)
    byteWrite(MCP_IOCON, ADDR_ENABLE);
    // Once the channel is set up, we can initialize che component:
    MCP::init();
}

void
MCPS::_make_opcode(uint8_t hwaddr)
{
    _writeopcode = 0x40+(hwaddr<<1);
    _readopcode  = _writeopcode+1;
}

char
MCPS::_read(char regaddr)
{
    uint8_t value = 0;                          // Initialize a variable to hold the read values to be returned
    SPI.beginTransaction(_SPIset);              // Start up the SPI bus
    ::digitalWrite(_ss, LOW);                   // Take slave-select low
    SPI.transfer(_readopcode);                  // Send the MCP23S17 opcode, chip address, and read bit
    SPI.transfer(regaddr);                      // Send the register we want to read
    value = SPI.transfer(0xFF);                 // Send any byte, the function will return the read value
    ::digitalWrite(_ss, HIGH);                  // Take slave-select high
    SPI.endTransaction();
    return value;                               // Return the constructed word, the format is 0x(register value)
}

unsigned int
MCPS::_readW(char regaddr)
{
    uint16_t value = 0;                     // Initialize a variable to hold the read values to be returned
    SPI.beginTransaction(_SPIset);          // Start up the SPI bus
    ::digitalWrite(_ss, LOW);               // Take slave-select low
    SPI.transfer(_readopcode);              // Send the MCP23S17 opcode, chip address, and read bit
    SPI.transfer(regaddr);                  // Send the register we want to read
    value = SPI.transfer(0xFF);             // Send any byte, the function will return the read value (register address pointer will auto-increment after write)
    value |= ((uint16_t)SPI.transfer(0xFF))<<8; // Read in the "high byte" (portB) and shift it up to the high location and merge with the "low byte"
    ::digitalWrite(_ss, HIGH);              // Take slave-select high
    SPI.endTransaction();
    return value;                           // Return the constructed word, the format is 0x(portB)(portA)
}

void
MCPS::_write(char regaddr, char data)
{
    SPI.beginTransaction(_SPIset);  // Start up the SPI bus
    ::digitalWrite(_ss, LOW);       // Take slave-select low
    SPI.transfer(_writeopcode);     // Send the MCP23S17 opcode, chip address, and write bit
    SPI.transfer(regaddr);          // Send the register we want to write
    SPI.transfer(data);             // Send the byte
    ::digitalWrite(_ss, HIGH);      // Take slave-select high
    SPI.endTransaction();
}

void
MCPS::_writeW(char regaddr, unsigned int data)
{
    SPI.beginTransaction(_SPIset);          // Start up the SPI bus
    ::digitalWrite(_ss, LOW);               // Take slave-select low
    SPI.transfer(_writeopcode);             // Send the MCP23S17 opcode, chip address, and write bit
    SPI.transfer(regaddr);                  // Send the register we want to write
    SPI.transfer((char)(data&0xFF));        // Send the low byte (register address pointer will auto-increment after write)
    SPI.transfer((char)((data>>8)&0xFF));   // Shift the high byte down to the low byte location and send
    ::digitalWrite(_ss, HIGH);              // Take slave-select high
    SPI.endTransaction();
}


