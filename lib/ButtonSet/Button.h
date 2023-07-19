/*
*
* File     : Button.h
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the ButtonAdv+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This library allows to conveniently define pushbutton actions with callbacks for several events.
* It is meant to work jointly with a ButtonGroupManager, which in turn receives (and passes along)
* input flags supplied by an underlaying I/O reader (digital buttons only).
*
* This is the base class for specific ButtonXXX classes.
* ButtonXXX objects receive a set of I/O flags describing the status of its associated input or 'pin'
* (for current status, up/dn transitions etc - see doc) and invokes callback functions accordingly.
* Callbacks are all defined in the derived classes.
*
* This file declares the base class for all ButtonXXX classes.
*
* 'Tag' and 'Data' attibutes can be of several types (they are unions); currently there is no provision
* to discriminate which of the possible types is used, the caller is responsible for their correct
* interpretation!
*
* Usage:
* - Include ButtonXXX.h (for all relevant XXX types) and ButtonManager.h in your sketch
* - Add a call to ButtonManager.check(...) in your main loop
* - Declare each button and define the events using a ButtonXXX constructor
* - Declare the required event functions ( void OnKeyYYY(ButtonXXX *but) )
* - See the comments in the code for more help
*
*/


#ifndef BUTTON_H
#define BUTTON_H

/// If the ButtonManager class is used, a singleton object called "ButtonMgr" is automatically instantiated;
/// activate following define so ALL ButtonXXX objects are automatically added to its list by their constructor.
/// If (at least some) ButtonXXX objects are to be used stand-alone, do _not_ compile with this option on!
/// If a button manager is desired anyway, ButtonXXX objects can still be added manually.
//
// A #define has been chosen over an additional constructor argument because, if the manager is not needed,
// this way we are not required to compile and instantiate it.
//
// ALTERNATIVE OPTION:
// If the implicit add is not desired (and an explicit add() operation is preferred):
// - remove the 'ButtonMgr.add(this);' calls from the constructors;
// - add a method for type redefinition in each derived class, like existing ones: "ButtonXXX *add(void) { Button::add(); return this; }",
//   so the "add" method can be chained in the definition statement.
// This option also allows managed and stand-alone objects to co-exist.
#define USE_BTN_MGR

/// Define if Source Var and Mirror Var featurs should be compiled
//#define SOURCEVAR
//#define MIRRORVAR

#include <Arduino.h>
#include "RawData.h"

#define flagChg(value, bitmsk, cond) ((cond) ? (value |= bitmsk) : (value &= ~bitmsk) )
#define times10(n)  ((n<<1)+(n<<3))
#define times100(n) ((n<<2)+(n<<5)+(n<<6))
#define UNUSED(x) ((void)(x))

// Bitmasks for 'flags'
// These flags define the specific features of the button
// (some may be applicable to just specific button types)
enum {
    F_lastState   = 0x80,   // Last recorded input status
    F_hasRepeat   = 0x40,   // Uses repeat
    F_Analog      = 0x20,   // Uses an analog value as input
    F_HWinput     = 0x10    // Reads HW inputs directly (as opposed to receiving data)
    //F_hasString   = 0x01,   // Name must be interpreted as string ptr (rather than uint32_t code)
    };

// Bitmasks for bit nos. used for src and mirror vars
enum {
    M_srcbits     = 0xF0,   // bit# in the source variable
    M_mirbits     = 0x0F    // bit# in the mirror variable
    }; // t_bitno;

// Status values in the bit pattern passed as argument to check()
enum {
    S_Curr = 0x01,
    S_Dn   = 0x02,
    S_Up   = 0x04,
    S_Rpt  = 0x08,
    S_Long = 0x10,
    }; // t_digstatus;

class Button
{

public:

    typedef uint8_t ButtonStatus_t;

    /// This empty constructor is used to pre-allocate an object that will be completely filled in later.
    /// This avoids the need for dynamic allocation support and, most importantly, allows to allocate all
    /// required objects (thereby knowing whether the required space fits memory size or not) already
    /// at compile time.
    /// Objects allocated in this way - typically through an array - are NOT added to the manager collection, thus
    /// they must be explicitly added later (when their values are filled).
    Button() {}

    /// These constructors allow each individual button to be defined in a concise way,
    /// if all required information is available at compile time.
    /// Buttons are automatically added to the collection in the ButtonGroupManager named 'ButtonGrpMgr'
    /// to allow centralized polling. From there they can also be retrieved for custom operations.
    /// The name is optional, as are the mirror variable and flag (bit# in var).
    /// If the mirror var is not NULL, the button state is automatically mirrored into it.
    /// pin = 1...n

    Button( uint8_t     pin,
            uint8_t     useHWinput,
            const char  *name,
            uint8_t     *mirrorvar=NULL,
            uint8_t     mirrorbit=0
            );

    Button( uint8_t     pin,
            uint8_t     useHWinput,
            uint16_t    codeh,
            uint16_t    codel,
            uint8_t     *mirrorvar=NULL,
            uint8_t     mirrorbit=0
            );

    void
    CButton(uint8_t     useHWinput,
            uint8_t     *mirrorvar,
            uint8_t     mirrorbit
            );

    // Checks the state of the button and triggers events accordingly;
    // This method must be (re)defined in the specific button type classes.
    // Will be called from the ButtonGroupManager.
    // 'value' is either a byte value (for analog inputs)
    // or a bit pattern with following meaning (where applicable):
    //   Scurr    Current input status
    //   Sdn      Input just became active
    //   Sup      Input was just released
    //   Srpt     A repeat interval just expired
    //   Slong    The long press interval just expired
    // All flags except Scurr are expected to be set for one call only,
    // right after the event occurs.
    // virtual 
    void check(ButtonStatus_t value) { UNUSED(value); } // = 0;

    // initState is used to assign the initial value.
    // It differs from check() because it only triggers _OnPress/_OnRelease events.
    // These are usually associated to stable switches (rather than temporary pushbuttons),
    // which require to have their position recorded at startutp
    // virtual 
    void initState(ButtonStatus_t value) { UNUSED(value); }

    // === Bulk setup methods

    // Register button in manager: use this ONLY for pre-allocated, still unregistered empty buttons.
    // The operation should conceptually belong to the manager class, however it is also defined here as a convenience wrapper.
    // Beware: it always returns a valid pointer, even if registering fails!
    Button *add(void);

    Button *info(uint8_t npin, uint8_t isHW)
                                            __attribute__((always_inline))  { pin = npin; flagChg(flags, F_HWinput, isHW); return this; }

    Button *tag(const char *s)              __attribute__((always_inline))  { _tag.text = s; return this; }
    Button *tag(uint16_t hi, uint16_t lo)   __attribute__((always_inline))  { _tag.code.hi = hi; _tag.code.lo = lo; return this; }

    Button *data(const char *s)             __attribute__((always_inline))  { _data.text = s; return this; }
    Button *data(byte *b)                   __attribute__((always_inline))  { _data.bytes = b; return this; }
    Button *data(uint16_t hi, uint16_t lo)  __attribute__((always_inline))  { _data.code.hi = hi; _data.code.lo = lo; return this; }

    // === Setters (single params)

    void    setPin(uint8_t p)       __attribute__((always_inline))  {pin = p;}

    // === Getters

    uint8_t getPin(void)            __attribute__((always_inline))  {return pin;}
    uint8_t isHW(void)              __attribute__((always_inline))  {return ((flags & F_HWinput)!=0);}
    uint8_t isAna(void)             __attribute__((always_inline))  {return ((flags & F_Analog)!=0);}

    //This is kept for temporary compatibility during debug:
    RawData *getTag(void)           __attribute__((always_inline))  {return &_tag;}
    char*   getName(void)           __attribute__((always_inline))  {return (char*)_tag.text;}
    void    getTag(uint16_t *lo,
                    uint16_t *hi)   __attribute__((always_inline))  {*lo=_tag.code.lo; *hi=_tag.code.hi;}
    RawData *getData(void)          __attribute__((always_inline))  {return &_data;}
    void    getData(uint16_t *lo,
                    uint16_t *hi)   __attribute__((always_inline))  {*lo=_data.code.lo; *hi=_data.code.hi;}

#ifdef SOURCEVAR
    Button *source(uint8_t *sourcevar, uint8_t sourcebit)
                                    __attribute__((always_inline))  {srcVar = sourcevar; bitno = (bitno&0x0F)|((sourcebit&0x07)<<4); return this;}
    uint8_t hasSrcVar(void)         __attribute__((always_inline))  {return (srcVar != NULL);}
    uint8_t *getSrcVar(void)        __attribute__((always_inline))  {return srcVar; }
    uint8_t getSrcVal(void)         __attribute__((always_inline))  {return ((flags&F_Analog) ? *srcVar : (((*mirrVar)&(1<<((bitno>>4)&0x0F))) ? true : false)); }
#else
    Button *source(uint8_t *sourcevar, uint8_t sourcebit)
                                    __attribute__((always_inline))  {UNUSED(sourcevar); UNUSED(sourcebit); return this;}
    uint8_t hasSrcVar(void)         __attribute__((always_inline))  {return false;}
    uint8_t *getSrcVar(void)        __attribute__((always_inline))  {return NULL;}
    uint8_t getSrcVal(void)         __attribute__((always_inline))  {return 0;}
#endif // SOURCEVAR

#ifdef MIRRORVAR
    Button *mirror(uint8_t *mirrorvar, uint8_t mirrorbit)
                                    __attribute__((always_inline))  {mirrVar = mirrorvar; bitno = (bitno&0xF0)|(sourcebit&0x07); return this;}
    uint8_t *getMVar(void)          __attribute__((always_inline))  {return mirrVar; }
    void    setBit(void)            __attribute__((always_inline))  {if(mirrVar) {*mirrVar |= (1 << (bitno&0x0F));}}
    void    clearBit(void)          __attribute__((always_inline))  {if(mirrVar) {*mirrVar &= ~(1 << (bitno&0x0F));}}
#else
    Button *mirror(uint8_t *mirrorvar, uint8_t mirrorbit)
                                    __attribute__((always_inline))  {UNUSED(mirrorvar); UNUSED(mirrorbit); return this;}
    uint8_t *getMVar(void)          __attribute__((always_inline))  {return NULL;}
    void    setBit(void)            __attribute__((always_inline))  {}
    void    clearBit(void)          __attribute__((always_inline))  {}
#endif // MIRRORVAR

protected:
    uint8_t         pin;
    uint8_t         flags;
    RawData         _tag;
    RawData         _data;
#if defined(SOURCEVAR)||defined(MIRRORVAR)
    uint8_t         bitno;
#endif
#ifdef SOURCEVAR
    uint8_t         *srcVar;
#endif // SOURCEVAR
#ifdef MIRRORVAR
    uint8_t         *mirrVar;
#endif // MIRRORVAR

};

constexpr uint8_t RDsize = sizeof(RawData);
constexpr uint8_t BBsize = sizeof(Button);

#endif
