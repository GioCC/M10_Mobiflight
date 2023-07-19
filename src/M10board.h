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
//================================
#include "config_board.h"

#include "bitmasks.h"
#include "bank.h"
#include "FastArduino.h"
#include "MCP23S17.h"
#include "EncoderSet.h"
#include "conversions.h"
#include "ButtonManager.h"
#include "EncManager.h"

#include "LedControlMod.h"
#include "LiquidCrystal.h"

#define   ENCSLOTS    6     // TODO REDUCE AS POSSIBLE

constexpr uint8_t LCDsize = sizeof(LiquidCrystal);
constexpr uint8_t LCsize  = sizeof(LedControl);
constexpr uint8_t DispSize = ((LCDsize > 2*LCsize) ? LCDsize : 2*LCsize);

#define UNUSED(x) ((void)(x))

class M10board
{
    private:

        M10board_cfg*    cfg;

        MCP*             MCPIO1;
        MCP*             MCPIO2;

        // The actual number of physical encoders is defined in config_board.h.
        // Reserved space is #defined in EncoderSet.h and in #define ENCSLOTS above.
        //TODO: if the AP module can be split, the max number of encoder reserved (common to all boards) can be decreased from 5 to 3
        EncoderSet       Encs;
        int              EncCount[ENCSLOTS];
        uint8_t          EncModes[ENCSLOTS];
        byte             EncMap[3];         // Actual encoder mappings; handles only up to 3 source encoders to spare memory

        uint32_t         realEncInputs;     // Input vector directly read for encoders
        uint32_t         virtEncInputs;     // Input vector formed for virtual encoders
        //uint16_t       encSwitches;       // Switches are read directly from I/O lines, not through M10Encoder

        uint8_t          _DISP[DispSize];

        union {
            LedControl*      LEDCTRL;
            LiquidCrystal*   LCDCTRL;
        };

        // Internal data storage:

        // These are public:
        //Bank<48>      Din;       (defined below) Buffer for I/O vector - Inputs
        //Bank<32>      Dout;      (defined below) Buffer for I/O vector - Outputs
        uint16_t        IOcfg[2];           // In (1) or Out (0)
        uint16_t        IOpullup[2];        // On (1) or Off (0)

        uint8_t         *AINS;              // Array of used analog inputs
        uint8_t         nAINS;              // Number of used analog inputs

    public:

        M10board(void);

        /// ====================================================
        /// Config functions
        /// ====================================================

        void    init(void) {};

        void    setBoardCfg(M10board_cfg *cfg);
        void    setBoardPostCfg(void);

        void    setIOMode(uint8_t bank, uint16_t IOmode);   // Mode 0 = Out, 1 = In
        void    setPUMode(uint8_t bank, uint16_t PUmode);   // PullUp: 1 = On

        /// ====================================================
        /// Main polling entry points
        /// ====================================================

        void    ScanInOut(byte mode=0);   // Mode: 0=R+W, 1=R, 2=W

        /// ====================================================
        /// Digital I/O management
        /// ====================================================

        // R/W functions operate on buffers 'Din'/'Dout', therefore require the invocation of ScanInOut(),
        // either before (for inputs) or after (for outputs).

        //TODO These should be private:
        Bank<64>    Din;              // Buffer for I/O vector - Inputs
        Bank<32>    Dout;             // Buffer for I/O vector - Outputs

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
        ///   either in vector 'virtEncInputs', or as normal read from inputs 33..56 (where 'virtEncInputs' is
        ///   automatically mirrored to by mirrorEncoder if 'toHighInputs' == true) just as they were normal physical inputs.
        /// If required, the original encoders' physical inputs are still accessible as they were before, without any alteration.
        /// Warning: mirrorEncoder(..) can handle 10 units, however, class M10Encoder (which is used to manage it) might 
        /// handle less (default is usually 8)!
        // 
        // Use encoder mirror to "split" a single encoder to different virtual encoders mapped to virtual inputs 33..62(+)
        // FromPos = 1..6 (in base vector)
        // ToPos = 1..10 (in virtual vector)
        // clean = if TRUE, all inputs of encoders different from the one written are set to 0
        // (makes sense only for encoders that have A=B=0 at detents).

        void        mirrorEncoder(byte fromPos, byte toPos, byte clean = 1, byte toHighInputs = 1);

        // Convenience utility function - define actual encoder mappings
        void        remapEncoder(byte fromPos, byte toPos);

        // For custom processing, an encoder object made available (based on the digital input vector),
        EncoderSet  *Encoders(void)       { return &Encs; }

        /// ====================================================
        /// LED Display management
        /// ====================================================
        ///

        // n is 0..1
        LedControl  *getDisplay(byte n)    { return &LEDCTRL[n&0x01]; }

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

//extern M10board board;

// overlay functions to hijack the standard direct pin access functions
// Here, pin = 1..54(+)
// void xpinMode(uint8_t pin, uint8_t mode);
// void xdigitalWrite(uint8_t pin, uint8_t val);
// int  xdigitalRead(uint8_t pin);

#endif // M10BOARD_H
