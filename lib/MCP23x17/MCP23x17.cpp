/*
  MCP23x17.cpp
  Microchip MCP23S17 SPI I/O Expander Class for Arduino

  Class MCP: Base class for MCP23017 / MCP23S17

  Based by MCP23S17 class by Cort Buffington & Keith Neufeld
*/

#include "MCP23x17.h"

// Defines to keep logical information symbolic go here

#ifndef HIGH
#define HIGH          (1)
#define LOW           (0)
#endif
#ifndef ON
#define ON            (1)
#define OFF           (0)
#endif
#ifndef INPUT
#define INPUT
#endif

// Control byte and configuration register information - Control Byte: "0100 A2 A1 A0 R/W" -- W=0

#define    OPCODEW       (0b01000000)  // Opcode for MCP23S17 with LSB (bit0) set to write (0), address OR'd in later, bits 1-3
#define    OPCODER       (0b01000001)  // Opcode for MCP23S17 with LSB (bit0) set to read (1), address OR'd in later, bits 1-3
#define    ADDR_ENABLE   (0b00001000)  // Configuration register for MCP23S17, the only thing we change is enabling hardware addressing

// Constructor to instantiate an instance of MCP to a specific chip (address)

MCP::MCP(void)
{
    // init(); // Init must be performed by derived classes - tx must already be setup!
}

void
MCP::init(void)
{
    uint8_t  ctr;
    _modeCache   = 0xFFFF;      // Default I/O mode is all input, 0xFFFF
    _outputCache = 0x0000;      // Default output state is all off, 0x0000
    _pullupCache = 0x0000;      // Default pull-up state is all off, 0x0000
    _invertCache = 0xFFFF;      // Default input inversion state is inverted, 0xFFFF
    ctr = 0x0E;                 // setup of control register (BANK = 0, MIRROR = 0, SEQOP = 0, DISSLW = 0, HAEN = 1, ODR = 1, INTPOL = 1, NC = 0)
    _write(MCP_IOCON, ctr);
    _writeW(MCP_IODIRA, _modeCache);
    _writeW(MCP_GPPUA, _pullupCache);
    _writeW(MCP_IPOLA, _invertCache);
}

// GENERIC BYTE WRITE - will write a byte to a register, arguments are register address and the value to write

void
MCP::byteWrite(uint8_t reg, uint8_t value) {      // Accept the register and byte
    _write(reg, value);
}

// GENERIC WORD WRITE - will write a word to a register pair, LSB to first register, MSB to next higher value register

void
MCP::wordWrite(uint8_t reg, unsigned int word) {  // Accept the start register and word
    _writeW(reg, word);
}

// MODE SETTING FUNCTIONS - BY PIN AND BY WORD

void
MCP::pinMode(uint8_t pin, uint8_t mode) {  // Accept the pin # and I/O mode
  if ((pin < 1) || (pin > 16)) return;               // If the pin value is not valid (1-16) return, do nothing and return
  if (mode == INPUT ||
      mode == INPUT_PULLUP ) {                  // Determine the mode before changing the bit state in the mode cache
    _modeCache |= 1 << (pin - 1);               // Since input = "HIGH", OR in a 1 in the appropriate place
    pullupMode(pin, (mode == INPUT_PULLUP));
  } else {
    _modeCache &= ~(1 << (pin - 1));            // If not, the mode must be output, so and in a 0 in the appropriate place
  }
  _writeW(MCP_IODIRA, _modeCache);                // Call the generic word writer with start register and the mode cache
}

void
MCP::pinMode(unsigned int mode) {     // Accept the word…
  _writeW(MCP_IODIRA, mode);                // Call the the generic word writer with start register and the mode cache
  _modeCache = mode;
}

// THE FOLLOWING WRITE FUNCTIONS ARE NEARLY IDENTICAL TO THE FIRST AND ARE NOT INDIVIDUALLY COMMENTED

// WEAK PULL-UP SETTING FUNCTIONS - BY WORD AND BY PIN

void
MCP::pullupMode(uint8_t pin, uint8_t mode) {
  if ((pin < 1) || (pin > 16)) return;
  if (mode) {
    _pullupCache |= 1 << (pin - 1);
  } else {
    _pullupCache &= ~(1 << (pin -1));
  }
  _writeW(MCP_GPPUA, _pullupCache);
}


void
MCP::pullupMode(unsigned int mode) {
  _writeW(MCP_GPPUA, mode);
  _pullupCache = mode;
}


// INPUT INVERSION SETTING FUNCTIONS - BY WORD AND BY PIN
void
MCP::inputInvert(uint8_t pin, uint8_t mode) {
  if ((pin < 1) || (pin > 16)) return;
  if (mode == ON) {
    _invertCache |= 1 << (pin - 1);
  } else {
    _invertCache &= ~(1 << (pin - 1));
  }
  _writeW(MCP_IPOLA, _invertCache);
}

void
MCP::inputInvert(unsigned int mode) {
  _writeW(MCP_IPOLA, mode);
  _invertCache = mode;
}


// WRITE FUNCTIONS - BY WORD AND BY PIN
void
MCP::IOWrite(unsigned int value) {
  _writeW(MCP_GPIOA, value);
  _outputCache = value;
}

void
MCP::IOWrite(uint8_t pin, uint8_t value) {
  if ((pin < 1) || (pin > 16)) return;
  if (value) {
    _outputCache |= 1 << (pin - 1);
  } else {
    _outputCache &= ~(1 << (pin - 1));
  }
  _writeW(MCP_GPIOA, _outputCache);
}


// READ FUNCTIONS - BY WORD, BYTE AND BY PIN
uint8_t
MCP::byteRead(uint8_t reg) {        // This function will read a single register, and return it
  return _read(reg);
}

unsigned int
MCP::IORead(void) {       // This function will read all 16 bits of I/O, and return them as a word in the format 0x(portB)(portA)
  return _readW(MCP_GPIOA);          // Return the constructed word, the format is 0x(portB)(portA)
}

uint8_t
MCP::IORead(uint8_t pin) {                    // Return a single bit value, supply the necessary bit (1-16)
    if ((pin < 1) || (pin > 16)) return 0x0;                    // If the pin value is not valid (1-16) return, do nothing and return
    return (digitalRead() & (1 << (pin - 1))) ? HIGH : LOW;  // Call the word reading function, extract HIGH/LOW information from the requested pin
}

/// Following section is from Adafruit's MCP23017 library - 
/// serves as sample for future implementation of interrupt management, currently missing.

/*
// Configures the interrupt system. both port A and B are assigned the same configuration.
// Mirroring will OR both INTA and INTB pins.
// Opendrain will set the INT pin to value or open drain.
// polarity will set LOW or HIGH on interrupt.
// Default values after Power On Reset are: (false,flase, LOW)
// If you are connecting the INTA/B pin to arduino 2/3, you should configure the interupt handling as FALLING with
// the default configuration.

void Adafruit_MCP23017::setupInterrupts(uint8_t mirroring, uint8_t openDrain, uint8_t polarity){
	// configure the port A
	uint8_t ioconfValue=readRegister(MCP23017_IOCONA);
	bitWrite(ioconfValue,6,mirroring);
	bitWrite(ioconfValue,2,openDrain);
	bitWrite(ioconfValue,1,polarity);
	writeRegister(MCP23017_IOCONA,ioconfValue);

	// Configure the port B
	ioconfValue=readRegister(MCP23017_IOCONB);
	bitWrite(ioconfValue,6,mirroring);
	bitWrite(ioconfValue,2,openDrain);
	bitWrite(ioconfValue,1,polarity);
	writeRegister(MCP23017_IOCONB,ioconfValue);
}


// Set up a pin for interrupt. uses arduino MODEs: CHANGE, FALLING, RISING.
// Note that the interrupt condition finishes when you read the information about the port / value
// that caused the interrupt or you read the port itself. Check the datasheet can be confusing.

void Adafruit_MCP23017::setupInterruptPin(uint8_t pin, uint8_t mode) {

	// set the pin interrupt control (0 means change, 1 means compare against given value);
	updateRegisterBit(pin,(mode!=CHANGE),MCP23017_INTCONA,MCP23017_INTCONB);
	// if the mode is not CHANGE, we need to set up a default value, different value triggers interrupt

	// In a RISING interrupt the default value is 0, interrupt is triggered when the pin goes to 1.
	// In a FALLING interrupt the default value is 1, interrupt is triggered when pin goes to 0.
	updateRegisterBit(pin,(mode==FALLING),MCP23017_DEFVALA,MCP23017_DEFVALB);

	// enable the pin for interrupt
	updateRegisterBit(pin,HIGH,MCP23017_GPINTENA,MCP23017_GPINTENB);
}

uint8_t Adafruit_MCP23017::getLastInterruptPin(){
	uint8_t intf;

	// try port A
	intf=readRegister(MCP23017_INTFA);
	for(int i=0;i<8;i++) if (bitRead(intf,i)) return i;

	// try port B
	intf=readRegister(MCP23017_INTFB);
	for(int i=0;i<8;i++) if (bitRead(intf,i)) return i+8;

	return MCP23017_INT_ERR;

}
uint8_t Adafruit_MCP23017::getLastInterruptPinValue(){
	uint8_t intPin=getLastInterruptPin();
	if(intPin!=MCP23017_INT_ERR){
		uint8_t intcapreg=regForPin(intPin,MCP23017_INTCAPA,MCP23017_INTCAPB);
		uint8_t bit=bitForPin(intPin);
		return (readRegister(intcapreg)>>bit) & (0x01);
	}

	return MCP23017_INT_ERR;
}
*/
