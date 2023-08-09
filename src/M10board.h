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
#include "conversions.h"
#include "bank.h"
#include "FastArduino.h"
#include "MCP23S17.h"
#include "Button_all.h"
#include "EncoderSet.h"
#include "ButtonManager.h"
#include "EncManager.h"

#include "LedControlMod.h"
#include "LiquidCrystal.h"

#define   ENCSLOTS    6     // TODO REDUCE AS POSSIBLE

constexpr uint8_t LCDsize = sizeof(LiquidCrystal);
constexpr uint8_t LCsize  = sizeof(LedControl);
constexpr uint8_t DispSize = ((LCDsize > 2*LCsize) ? LCDsize : 2*LCsize);

#define UNUSED(x)       ((void)(x))
#define BYTESIZE(x)     ((x+7)>>3)

constexpr uint8_t DIN_COUNT  = 64;
// DOUT_COUNT should be 49, counting the highest numbers of LEDs on a MAX; however,
// we have no interest in caching them, so we cam spare some memory
constexpr uint8_t DOUT_COUNT = 32;  

class M10board
{
    private:

        // ******* Configuration

        M10board_cfg*    cfg;

        // ******* Internal data storage:

        Bank<DIN_COUNT>     Din;            // Buffer for I/O vector - Inputs
        Bank<DOUT_COUNT>    Dout;           // Buffer for I/O vector - Outputs

        uint16_t        IOcfg[BYTESIZE(DIN_COUNT)];     // In (1) or Out (0)
        uint16_t        IOpullup[BYTESIZE(DIN_COUNT)];  // On (1) or Off (0)

        uint8_t*        AINS = nullptr;     // Array of used analog inputs
        uint8_t         nAINS = 0;          // Number of used analog inputs

        // ******* Buttons / Switches
    
        ButtonManager   ButtonMgr;

        // ******* Encoders
    
        //TODO: if the AP module can be split, the max number of encoder reserved (common to all boards) can be decreased from 5 to 3
        EncoderSet      Encs;
        int             EncCount[ENCSLOTS];
        uint8_t         EncModes[ENCSLOTS];
        byte            EncMap[3] = {0xFF, 0xFF, 0xFF};  // Encoder mappings; handles only up to 3 source encoders to spare memory
        uint32_t        encInputs;     // Input vector directly read for encoders
        //uint16_t      encSwitches;       // Switches are read directly from I/O lines, not through M10Encoder

        // ******* I/O expanders

        MCP*            MCPIO1;
        MCP*            MCPIO2;
        
        // ******* LED/LCD Display drivers
        union {
            LedControl*      LEDCTRL;
            LiquidCrystal*   LCDCTRL;
        };

        // ******* LED Display driver handles

        uint8_t          _DISP[DispSize];

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
        /// Digital I/O management
        /// ====================================================

        // R/W functions operate on buffers 'Din'/'Dout', therefore require the invocation of ScanInOut(),
        // either before (for inputs) or after (for outputs).

        void        ScanInOut(byte mode=0);   // Mode: 0=R+W, 1=R, 2=W

        // In the functions below, pin = 1..32
        
        // Pin = 1..32
        // Mode = INPUT, INPUT_PULLUP, OUTPUT
        void        setIOPinMode(uint8_t pin, uint8_t mode);  
        // Bank = 1, 2
        // dir (bits):     0 = Out, 1 = In
        // pullups (bits): 1 = on
        void        setIOMode(uint8_t bank, uint16_t dir, uint16_t pullups);
        
        // Wrappers for cache IO bit access
        // Pin = 1..32
        void        cacheWrite(uint8_t pin, uint8_t val);
        int         cacheRead(uint8_t pin);

        // Effective immediate bit write.
        // Discerns between I/O expander or upper pins (used for LEDs on MAX segments).
        // (Obviously also updates cache)
        void        outWrite(uint8_t pin, uint8_t val);

        // FOR DEBUG ONLY (no boundary checks)
        uint16_t    getIns(byte bank = 0)               { return (bank==0 ? MCPIO1->IORead() : MCPIO2->IORead()); }
        void        setOuts(uint16_t ov, byte bank = 0) { (bank==0 ? MCPIO1->IOWrite(ov) : MCPIO2->IOWrite(ov)); }

        /// ====================================================
        /// Analog I/O management
        /// ====================================================

        void        setupAnaIns(uint16_t AIvector);
        // Read n-th configured analog input (scaled to 0..255)
        uint8_t     readAI(uint8_t n)   { return ( n < nAINS ? ((analogRead(AINS[n])+2) >> 2) : 0);  }

        /// ====================================================
        /// Switch/button management
        /// ====================================================

        // After a fresh input scan, process switches and buttons
        void    ProcessSwitches(void);

        /// ====================================================
        /// Encoder management
        /// ====================================================

        /// "Virtual' (mirrored) encoders
        ///
        /// One or more input sets corresponding to physical encoders (EncA, EncB, Sw lines), stored in the lower
        /// places of an I/O vector, can be "mirrored" to higher "slots" in the same vector, which are associated
        /// to "virtual" encoders.
        /// (By default, the inputs are actually moved, not copied; "mirrored" is a bit misleading.)
        /// 
        /// This mirroring happens at the level right after reading inputs; therefore, virtual encoders
        /// are effectively identical to physical ones, and are managed by the same classes with all the same features.
        /// A physical encoder can be associated to a single virtual encoder at any given time.
        /// The matching between physical and virtual encoders can be changed on the fly at any moment.
        ///
        /// The typical use case is a single encoder with a shared function (according to a physical selector
        /// or a software setting): e.g. an encoder can be assigned to the frequency change of different radios
        /// (and/or to int/frac portion of these frequencies) according to the position of a rotary selector.
        /// 
        /// In order to efficiently use the memory space:
        /// - the first 3 "slots" (bits from 0 to 8) correspond to physical encoders;
        /// - upper "slots" (bits from 9 upwards) correspond to virtual encoders.
        /// If there are fewer than 3 physical encoders, the slots corresponding to the missing ones can be used as
        /// virtual; the remap is not constrained to upper position, therefore it is up to the user to care for the 
        /// correct mapping.
        /// 
        /// Using options, the same physical encoder can also be mirrored to more than one virtual encoder,
        /// if required (and if it makes sense).
        ///
        /// How to use:
        ///
        /// - Read the I/O vectors (physical I/O lines) of the physical encoders
        /// - For each encoder to be mirrored, call 'mirrorEncoder(...)', with the proper matching parameters active at the time
        /// - just go on transparently with normal encoder processing.
        /// Warning: mirrorEncoder(..) can handle 10 units, however, class M10Encoder (which is used to manage it) might 
        /// handle less (default is usually 8)!
        // 
        // fromPos = 1..3
        // toPos   = 1..10
        // move    = if TRUE (default), all inputs of encoders different from the one written are set to 0
        // (requires that encoders have A=B=0 at detents).
        
        // TODO: do not use implicit I/O vector variable?

        void    mirrorEncoder(byte fromPos, byte toPos, bool move = true);

        // Version using the supplied remap table
        // TODO change overlapping signature
        void    mirrorEncoder(byte fromPos, bool move = true) { mirrorEncoder(fromPos, EncMap[fromPos-1], move); }

        // Define encoder mappings
        // fromPos = 1..3
        // toPos   = 1..10
        void    remapEncoder(byte fromPos, byte toPos);
        
        // After a fresh input scan, process encoder inputs
        void    ProcessEncoders(void);

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
