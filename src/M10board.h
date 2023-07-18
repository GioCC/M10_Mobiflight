/**
 *  M10board.h
 *
 *  Defines a unit with the hardware resources of a generic M10 board
*/


#ifndef M10BOARD_H
#define M10BOARD_H

//#define __M10_board__     // Unused (now implicit in customised code)

#include <Arduino.h>

//================================
// Board I/O configuration
#include "config_board.h"
//================================

#include "bitmasks.h"
#include "bank.h"
//#include "varStore.h"
#include "FastArduino.h"
#include "MCP23S17.h"
#include "EncoderM10.h"
#include "conversions.h"
#include "ButtonManager.h"
#include "EncManager.h"

#include "LedControlMod.h"
#include "LiquidCrystal.h"

#define UNUSED(x) ((void)(x))

// Struct used for HW config of the board
// This struct is used to define an actual config instances as constants in config_board.h

typedef struct //M10board_cfg
{
    //TODO (M10) Add: IRQ and CS pins, specific addresses
    // pins not defined as either input or output are configured as inputs anyway, however it is apparent from the definition that they are unused
    uint16_t    digInputs;      // Map of digital inputs (1=input) for MCP on main board
    uint16_t    digOutputs;     // Map of digital outputs (1=output) for MCP on main board
    
    bool        hasBank2;
    // following are used only for those panels having a second extension board (currently only Autopilot, LED or LCD)
    uint16_t    digInputs2;     // Map of digital inputs (1=input) for MCP on second board
    uint16_t    digOutputs2;    // Map of digital outputs (1=output) for MCP on second board

    uint16_t    anaInputs;      // Map of analog inputs used; remember that A0,A3,A4,A5 are preallocated (IORQ, DRDY, SDA, SCL). See comments.

    // Number of encoders (actually connected; max 6)
    // Encoders #1,#2,#3 are allocated on DIO pins 1..3, 4..6 and 7...9; pins used are defined as inputs overriding the definition as GPIO.
    // Encoders #4,#5,[#6] are allocated on DIO pins 1..3, 4..6 [and 7...9] on the extension board.
    uint8_t     nEncoders;
    // Number of virtual (logical) encoders
    uint8_t     nVirtEncoders;
    bool        hasDisplays;
    bool        hasLCD;

    union {
        struct {
            // LED display drivers (actually connected; max 4)
            uint8_t     nDisplays1;         // Number of displays on the first port (0,1,2) - These are numbered 1 / 2
            uint8_t     nDisplays2;         // Number of displays on the second port (0,1,2) - These are numbered 3 / 4
        }; // led;  // uncomment if compiler doesn't allow anon structs (gcc should)
        struct {
            // LCD display
            uint8_t     LCDCols;
            uint8_t     LCDLines;
        }; //lcd;  // uncomment if compiler doesn't allow anon structs (gcc should)
    };
} M10board_cfg;

extern const uint8_t        MaxBoards;
extern const M10board_cfg   Boards[] PROGMEM;

class M10board
{
    private:

        M10board_cfg*    cfg;

        MCP*             MCPIO1;
        MCP*             MCPIO2;

        EncoderM10*      ENCS;
        EncoderM10*      VENCS;

        LedControl*      LEDCTRL[2];
        LiquidCrystal*   LCDCTRL;

        // Internal data storage:

        // These are public:
        //Bank<48>      Din;       (defined below) Buffer for I/O vector - Inputs
        //Bank<32>      Dout;      (defined below) Buffer for I/O vector - Outputs
        uint16_t        IOcfg[2];           // In (1) or Out (0)
        uint16_t        IOpullup[2];        // On (1) or Off (0)

        uint8_t         *AINS;              // Array of used analog inputs
        uint8_t         nAINS;              // Number of used analog inputs

    public:
        //varStore<32>    vars;             // ENABLE IF REQUIRED

        M10board(void);

        /// ====================================================
        /// Main polling entry points
        /// ====================================================

        void        ScanInOut(byte mode=0);   // Mode: 0=R+W, 1=R, 2=W

        /// ====================================================
        /// Config functions
        /// ====================================================

        void        setBoardCfg(M10board_cfg *cfg);
        void        setBoardPostCfg(void);

        // In these functions (whether bank1 or bank2), pin = 1..16
        // void        ModeL(byte pin, byte mode);   // Mode: INPUT, INPUT_PULLUP, OUTPUT
        // void        ModeIO_L(uint16_t IOmode)      { MCPIO1->pinMode(IOcfgL = IOmode); }            // Mode 0 = Out, 1 = In
        // void        ModePU_L(uint16_t PUmode)      { MCPIO1->pullupMode(IOpullupL = PUmode); }      // PullUp: 1 = On
        
        // void        ModeH(byte pin, byte mode);  // Mode: INPUT, INPUT_PULLUP, OUTPUT
        // void        ModeIO_H(uint16_t IOmode)     { if(board.cfg->hasBank2) MCPIO2->pinMode(IOcfgH = IOmode); }           // Mode 0 = Out, 1 = In
        // void        ModePU_H(uint16_t PUmode)     { if(board.cfg->hasBank2) MCPIO2->pullupMode(IOpullupH = PUmode); }     // PullUp: 1 = On

        void        setIOMode(uint8_t bank, uint16_t IOmode);   // Mode 0 = Out, 1 = In
        void        setPUMode(uint8_t bank, uint16_t PUmode);   // PullUp: 1 = On

        /// ====================================================
        /// Digital I/O management
        /// ====================================================

        // R/W functions operate on buffers 'Din'/'Dout', therefore require the invocation of ScanInOut(),
        // either before (for inputs) or after (for outputs).

        Bank<64>      Din;              // Buffer for I/O vector - Inputs
        Bank<32>      Dout;             // Buffer for I/O vector - Outputs

        // Below, pin=1..32
        void        setPinMode(uint8_t pin, uint8_t mode);  // Mode: INPUT, INPUT_PULLUP, OUTPUT
        void        digitalWrite(uint8_t pin, uint8_t val);
        int         digitalRead(uint8_t pin);

        // FOR DEBUG ONLY (no boundary checks)
        uint16_t    getIns(byte bank=0) {return (bank==0 ? MCPIO1->digitalRead() : MCPIO2->digitalRead()); }
        void        setOuts(uint16_t ov, byte bank=0) { (bank==0 ? MCPIO1->digitalWrite(ov) : MCPIO2->digitalWrite(ov)); }

        /// ====================================================
        /// Analog I/O management
        /// ====================================================

        void        setupAnaIns(uint16_t AIvector);
        // Read n-th configured analog input (scaled to 0..255)
        uint8_t     readAI(uint8_t n)   { return ( n < nAINS ? ((analogRead(AINS[n])+2) >> 2) : 0);  }

        /// ====================================================
        /// Encoder management
        /// ====================================================

        /// "Virtual' (mirrored) encoders
        ///
        /// One or more input sets corresponding to physical encoders can be mirrored to an I/O vector
        /// (integrated in the standard I/O space) associated to several "virtual" encoders.
        /// This mirroring happens at the level right after reading inputs, therefore it effectively mimics
        /// the presence of the virtual encoders.
        /// Since switching happens at input level, Virtual encoders are managed by the same classes as physical
        /// encoders, thus they have all of their features.
        /// The matching between physical and virtual encoders can be changed on the fly at any moment.
        /// The typical use case is a single encoder with a shared function (according to a physical selector
        /// or a software setting): e.g. an encoder can be assigned to the frequency change of different radios
        /// (and/or to int/frac portion of these frequencies) according to the position of a rotary selector.
        /// The same phys encoder can also be mirrored to more than one virtual encoder, if required (and if it makes sense).
        ///
        /// How to use:
        ///
        /// - read the I/O vectors (physical I/O lines) of the physical encoders
        /// - call the 'mirrorEncoder(...)', with the proper matching parameters active at the time
        /// - just go on transparently with normal encoder processing. The virtual encoder lines are found
        ///   either in vector 'virtualEncs', or as normal read from inputs 33..56 (where 'virtualEncs' is
        ///   automatically mirrored to by mirrorEncoder if 'toHighInputs' == true) just as they were normal physical inputs.
        /// If required, the original encoders' physical inputs are still accessible as they were before, without any alteration.
        /// Warning: mirrorEncoder(..) can handle 10 units, however, class M10Encoder (which is used to manage it) only
        /// handles up to 8!

        // Use encoder mirror to "split" a single encoder to different virtual encoders mapped to virtual inputs 33..62(+)
        // FromPos = 1..6 (in base vector)
        // ToPos = 1..10 (in virtual vector)
        // clean = if TRUE, all inputs of encoders different from the one written are set to 0
        // (makes sense only for encoders that have A=B=0 at detents).

        uint32_t    realEncs;       // start
        uint32_t    virtualEncs;
        void        mirrorEncoder(byte fromPos, byte toPos, byte clean = 1, byte toHighInputs = 1);

        //uint16_t    encSwitches;      // Switches are read directly from I/O lines, not through M10Encoder

        // Convenience utility function - define actual encoder mappings
        // Handles only up to 2 source encoders to spare memory
        byte        encMap[2];
        void        remapEncoder(byte fromPos, byte toPos);

        // For custom processing, an encoder object made available (based on the digital input vector),
        EncoderM10  *Encs;      // (syntactic sugar - defined for convenience, set = to ENCS)
        EncoderM10  *Encoders(void)             { return Encs = ENCS; }

#ifndef HAS_LCD
        /// ====================================================
        /// LED Display management
        /// ====================================================
        ///

        // n is 0..1
        LedControl  *getDisplay(byte n)    { return LEDCTRL[n&0x01]; }

// ALL FOLLOWING DEFINITIONS ARE WRAPPERS:
// few of them are actually used, therefore we better use direct calls to LedControl objects

//         // Wrappers for LedControl functions
//         // Display index is 1..4:
//         // 1,2 are the first and second chained display (controllers) on the first port
//         // 3,4 same on the second port
//         // Channel is 1..2
// #define D_LINE  (LEDCTRL[((didx-1)>>1)&0x01])
// #define D_UNIT  ((didx-1)&0x01)
// 
//         void setDeviceCount(byte chan, byte nUnits, byte init=1)        { LEDCTRL[((chan-1)&0x01)]->setDeviceCount(nUnits, init); }
//         byte getDeviceCount(byte chan)                                  { return LEDCTRL[((chan-1)&0x01)]->getDeviceCount(); }
// 
//         // Setup functions
//         // Display index (didx) is 1..4
// 
//         // An ordinary setup sequence would be:
//         // setDeviceCount(...)      // Define how phys displays are allocated on channels
//         // dispWidth(...)           // Define the width of each display (if required - normally not, if viewports are used)
//         // dispInit(...)            // Init each display (or all)
//         void dispWidth(byte didx, byte width)                           {D_LINE->setScanLimit(D_UNIT,width);}
//         void dispInit(byte didx)                                        {D_LINE->init(D_UNIT);}
// 
//         // Following are provided if required (for decouplement from underlying LedControl)
//         void dispShutdown(byte didx, bool status)                       {D_LINE->shutdown(D_UNIT,status);}
//         void dispIntensity(byte didx, byte intensity)                   {D_LINE->setIntensity(D_UNIT,intensity);}
//         void dispClear(byte didx)                                       {D_LINE->clearDisplay(D_UNIT);}
// 
//         void dispSetLed(byte didx, byte row, byte col, boolean state)   {D_LINE->setLed(D_UNIT, row, col, state);}
//         void dispSetRow(byte didx, byte row, byte value)                {D_LINE->setRow(D_UNIT, row, value);}
//         void dispSetColumn(byte didx, byte col, byte value)             {D_LINE->setColumn(D_UNIT, col, value);}
//         void dispSetDigit(byte didx, byte digit, byte value, boolean dp, boolean no_tx=1)   {D_LINE->setDigit(D_UNIT, digit, value, dp, no_tx);}
//         void dispSetChar(byte didx, byte digit, char value, boolean dp, boolean no_tx=1)    {D_LINE->setChar(D_UNIT, digit, value, dp, no_tx);}
//         void dispWriteDigit(byte didx, byte *vals, byte dpmask, boolean no_tx=1)            {D_LINE->setAllDigits(D_UNIT, vals, dpmask, no_tx);}
//         void dispWrite(byte didx, byte *vals, byte dpmask, boolean no_tx=1)                 {D_LINE->setAllChars(D_UNIT, vals, dpmask, no_tx);}
//         void dispTransmit(boolean chgdonly=1)                           {LEDCTRL[0]->transmit(chgdonly); if(LEDCTRL[1]) LEDCTRL[1]->transmit(chgdonly); }

// TODO: LCD management

};

extern M10board board;

// overlay functions to hijack the standard direct pin access functions
// Here, pin = 1..54(+)
// void xpinMode(uint8_t pin, uint8_t mode);
// void xdigitalWrite(uint8_t pin, uint8_t val);
// int  xdigitalRead(uint8_t pin);

#endif // M10BOARD_H
