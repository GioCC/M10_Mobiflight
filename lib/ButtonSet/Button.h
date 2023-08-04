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
* It is meant to work jointly with a ButtonManager, which in turn receives (and passes along)
* input flags supplied by an underlaying I/O reader (digital buttons only).
*
* This file declares the base class for all ButtonXXX classes.
* ButtonXXX objects receive a set of I/O flags describing the status of its associated input or 'pin'
* (for current status, up/dn transitions etc - see doc) and invokes callback functions accordingly.
* Callbacks are all defined in the derived classes.
*
* 'Tag' and 'Data' attibutes can be of several types (they are unions); currently there is no provision
* to discriminate which of the possible types is used, the caller is responsible for their correct
* interpretation!
*
* Usage:
* - Include ButtonXXX.h (for all relevant XXX types) and if required ButtonManager.h in your code
* - Add a call to ButtonManager->check(...) in your main loop
* - Declare each button and define the events using a ButtonXXX constructor
* - Declare the required event functions ( void OnKeyYYY(ButtonXXX *but) )
* - See the comments in the code for more help
*
*/

#ifndef BUTTON_H
#define BUTTON_H

//======================================
//=== Compilation options
//======================================

/// #define USE_BTN_MGR to include methods to interface with a ButtonManager
#define USE_BTN_MGR

/// #define SOURCEVAR/MIRRORVAR if Source Var and Mirror Var features should be compiled
#define SOURCEVAR
#define MIRRORVAR

// #define MAKE_NEW to use alternative methods of allocation (defaults to standard "new")
//#define MAKE_NEW(obj)   new obj()

#include <Arduino.h>

#ifdef USE_BTN_MGR
class ButtonManager;    // Fwd declaration
#endif

#define flagChg(value, bitmsk, cond) ((cond) ? (value |= bitmsk) : (value &= ~bitmsk) )
#define times10(n)  ((n<<1)+(n<<3))
#define times100(n) ((n<<2)+(n<<5)+(n<<6))
#define UNUSED(x) ((void)(x))


// Following macro is used in the derived classes to automatically define aliases for all
// basic setup methods.
#define _DEFINE_BASIC_METHODS_NONMGR(derived) \
derived& pin(uint8_t npin, uint8_t isHW)      { Button::pin(npin, isHW); return *this; } \
derived& tag(const char *s)                   { Button::tag(s);     return *this; } \
derived& tag(byte *b)                         { Button::tag(b);     return *this; } \
derived& tag(uint16_t code)                   { Button::tag(code);  return *this; } \
derived& data(const char *s)                  { Button::data(s);    return *this; } \
derived& data(byte *b)                        { Button::data(b);    return *this; } \
derived& data(uint16_t code)                  { Button::data(code); return *this; } \
derived& mirror(uint8_t *mvar, uint8_t mbit)  { Button::mirror(mvar, mbit); return *this; } \
derived& source(uint8_t *svar, uint8_t sbit)  { Button::source(svar, sbit); return *this; } \
static derived& make(void)                    { derived* pb = new derived;    return *pb; } \
static derived& make(void* p)                 { derived* pb = new(p) derived; return *pb; }

#ifdef USE_BTN_MGR
#define DEFINE_BASIC_METHODS(derived) \
_DEFINE_BASIC_METHODS_NONMGR(derived) \
derived& addTo(ButtonManager& mgr)                      { mgr.add(this); return *this; } \
static derived& make(ButtonManager& mgr)                { derived* pb = new derived;    mgr.add(pb); return *pb; } \
static derived& make(derived* p,  ButtonManager& mgr)   { derived* pb = new(p) derived; mgr.add(pb); return *pb; } 
#else
#define DEFINE_BASIC_METHODS(derived)         _DEFINE_BASIC_METHODS_NONMGR(derived)
#endif

// Aux class for tag/Data fields

class TagData
{
public:
    union {
        const char      *text;
        const byte      *bytes;
        uint16_t        code;
    };

    TagData(void) {};
    explicit TagData(uint16_t ncode)    { code = ncode; }
    explicit TagData(const char *s)     { text = s; }
    explicit TagData(const byte b[])    { bytes = b; }
};

class Button
{

public:

    // typedef uint8_t ButtonStatus_t;
    using ButtonStatus_t = union {
        uint8_t aVal;
        uint8_t dVal;
    };

    // Bitmasks for 'flags'
    // These flags define the specific features of the button
    // (some may be applicable to just specific button types)
    enum {
        lastState   = 0x80,   // Last recorded input status
        rptEnabled  = 0x40,   // Repeat is enabled (if supported)
        Analog      = 0x20,   // Uses an analog value as input
        HWinput     = 0x10    // Reads HW inputs directly (as opposed to receiving data)
        //hasString   = 0x01,   // Name must be interpreted as string ptr (rather than uint_t code)
    } t_flags;

    // Status values in the bit pattern passed as argument to check()
    enum {
        Curr = 0x01,
        Dn   = 0x02,
        Up   = 0x04,
        Rpt  = 0x08,
        Long = 0x10,
    } t_digstatus;

    // Bitmasks for bit nos. used for src and mirror vars
    enum {
        srcBits     = 0xF0,   // bit# in the source variable
        mirBits     = 0x0F    // bit# in the mirror variable
    } t_bitno;


protected:

    uint8_t         _pin;
    uint8_t         _flags;
    TagData         _tag;
    // TagData         _data;

#ifdef SOURCEVAR
    uint8_t         *srcVar;
    uint8_t         srcBit;
#endif
#ifdef MIRRORVAR
    uint8_t         *mirrVar;
    uint8_t         mirrBit;
#endif

    void modeAnalog(uint8_t v) {
        flagChg(_flags, Button::Analog, v);
    }

    void valBit(uint8_t v) {
        flagChg(_flags, Button::lastState, v);
        #ifdef MIRRORVAR
        mirrorBit(v);
        #endif
    }

    // ======================================
    // === Constructors
    // ======================================

    /// This empty constructor is used to pre-allocate an object that will be completely filled in later.
    /// This avoids the need for dynamic allocation support and, most importantly, allows to allocate all
    /// required objects (thereby knowing whether the required space fits memory size or not) already
    /// at compile time.
    Button() {}

    /// These constructors allow each individual button to be defined in a concise way,
    /// if all required information is available at compile time.
    /// The name is optional, as are the mirror variable and flag (bit# in var).
    /// If a mirror var is specified (not nullptr), the button state is automatically
    /// mirrored into it.
    /// _pin = 1...n


    Button( 
        uint8_t     pin,
        uint8_t     useHWinput,
        const char  *name,
        uint8_t     *mirrorvar=NULL,
        uint8_t     mirrorbit=0
        );

    Button( 
        uint8_t     pin,
        uint8_t     useHWinput,
        uint16_t    tag,
        uint8_t     *mirrorvar=NULL,
        uint8_t     mirrorbit=0
        );

    void
    CButton(uint8_t     useHWinput,
            uint8_t     *mirrorvar,
            uint8_t     mirrorbit
            );

    void setHW(void) {
        pinMode(_pin, INPUT_PULLUP);
        digitalWrite(_pin, HIGH);
    };

public:

    // ======================================
    // === Setup methods
    // ======================================
    // These methods, all returning the object itself, are meant to allow chained configuration calls like:
    //   add().tag("MyName").data(0x6E)
    // These methods can't be declared 'virtual': in order to insure compatibility with their specialized
    // setup methods, derived objects will have to redefine these methods WITH THE SAME NAME AND SIGNATURE
    // but RETURNING THEIR OWN TYPE. The redefined methods must ONLY call the corresponding base class methods
    // and return the reference to the object, and nothing more.
    //
    // For further details and an example, see Bruce Eckel, "Thinking in C++ (Vol.1)",
    // from section "Upcasting" (p.662) forward.

    Button& pin(uint8_t npin, uint8_t isHW)
        { _pin = npin; flagChg(_flags, Button::HWinput, isHW); if(isHW) setHW(); return *this; }

    Button& tag(const char *s)      { _tag.text = s;     return *this; }
    Button& tag(byte *b)            { _tag.bytes = b;    return *this; }
    Button& tag(uint16_t code)      { _tag.code = code;  return *this; }

    Button& data(const char *s)     { /*_data.text = s;    */ return *this; }
    Button& data(byte *b)           { /*_data.bytes = b;   */ return *this; }
    Button& data(uint16_t code)     { /*_data.code = code; */ return *this; }
    
    Button& source(uint8_t* sourcevar, uint8_t sourcebit)
    #ifdef SOURCEVAR
        { if(sourcevar!=nullptr) { srcVar = sourcevar; srcBit = sourcebit; } return *this; }
    #else
        { UNUSED(sourcevar); UNUSED(sourcebit); return *this; }
    #endif

    Button& mirror(uint8_t *mirrorvar, uint8_t mirrorbit)
    #ifdef MIRRORVAR
        { if(mirrVar==nullptr) { mirrVar = mirrorvar; mirrBit = mirrorbit; } return *this; }
    #else
        { UNUSED(mirrorvar); UNUSED(mirrorbit); return *this; }
    #endif

    static Button& make(void)       { Button* pb = new Button;    return *pb; };
    static Button& make(void *p)    { Button* pb = new(p) Button; return *pb; };

    #ifdef USE_BTN_MGR
    // Add the button to the collection in the specified ButtonManager,
    // to allow centralized polling. From there they can also be retrieved for custom operations.
    Button& addTo(ButtonManager& mgr);

    // Create a Button and add it to the collection in the specified ButtonManager.
    static Button& make(ButtonManager& mgr);
    // Same, but in pre-allocated space
    static Button& make(void* p, ButtonManager& mgr);
    #endif

    // ======================================
    // === Getters
    // ======================================

    uint8_t     getPin(void)            { return _pin; }
    uint8_t     isHW(void)              { return ((_flags & Button::HWinput)!=0); }
    uint8_t     isAna(void)             { return ((_flags & Button::Analog) !=0); }

    TagData*    getTag(void)            { return &_tag; }
    char*       getName(void)           { return (char*)_tag.text; }
    void        getTag(uint16_t *tag)   { *tag =_tag.code; }
    // TagData *getData(void)          { return &_data; }
    // void    getData(uint16_t *data) { *data=_data.code; }

    // ======================================
    // === Operation methods
    // ======================================

    // Checks the state of the button: Polls status value and triggers events accordingly.
    // This method must be (re)defined in the specific button type classes.
    virtual void check(uint8_t force = 0) { UNUSED(force); } // = 0;

    // Version where the value is supplied from an external button manager.
    // 'value' is either a byte value (for analog inputs)
    // or a bit pattern with following meaning (where applicable):
    //   Scurr    Current input status
    //   Sdn      Input just became active
    //   Sup      Input was just released
    //   Srpt     A repeat interval just expired
    //   Slong    The long press interval just expired
    // All flags except Scurr are expected to be set for one call only,
    // right after the event occurs.
    virtual void checkVal(ButtonStatus_t value, uint8_t force = 0) { UNUSED(value); UNUSED(force); }

    // A variant of 'check()' for digital input vectors (specific to derived class)
    // Gets its input source from the passed byte array according to pin#:
    // bytevec[0] contains values of pins 1..8 (bits 0..7), bytevec[1] contains pins 9..16 etc
    // It is responsibilty of the caller to assure that bytevec[] has a size compatible with the button's pin no.
    //
    // If the button is configured for direct HW pin reading or Analog source, a call to this method has NO EFFECT.
    virtual void check(uint8_t *bytevec, uint8_t force = 0) { UNUSED(bytevec); UNUSED(force); };

    // initState is used to assign the initial value.
    // It differs from check() because it only triggers _OnPress/_OnRelease events.
    // These are usually associated to stable switches (rather than temporary pushbuttons),
    // which require to have their position recorded at startutp
    void initState(void) { check(1); }
    
    // Version where the value is supplied from an external button manager
    void initStateVal(ButtonStatus_t value) { checkVal(value, 1); }

    // Version where the value is supplied from an external button manager as digital vector
    void initState(uint8_t *bytevec) { check(bytevec, 1); };
    
    // ======================================
    // === Conditional definitions
    // ======================================

#ifdef SOURCEVAR
    uint8_t hasSrcVar(void)         {return (srcVar != NULL);}
    uint8_t *getSrcVar(void)        {return srcVar; }
    uint8_t getSrcVal(void)         {return ((_flags&Button::Analog) ? *srcVar : (((*(srcVar + (srcBit>>3))&(1<<(srcBit&0x07))) ? HIGH : LOW))); }
#else
    uint8_t hasSrcVar(void)         {return false;}
    uint8_t *getSrcVar(void)        {return NULL;}
    uint8_t getSrcVal(void)         {return 0;}
#endif

#ifdef MIRRORVAR
    uint8_t *getMVar(void)          {return mirrVar; }
    void    setMirror(void)         {if(!isAna() && (mirrVar!=nullptr)) {*(mirrVar + (mirrBit>>3)) |=  (1 << (mirrBit&0x07));}}
    void    clrMirror(void)         {if(!isAna() && (mirrVar!=nullptr)) {*(mirrVar + (mirrBit>>3)) &= ~(1 << (mirrBit&0x07));}}
    void    mirrorBit(uint8_t st)   {st ? setMirror() : clrMirror();}
#else
    uint8_t *getMVar(void)          {return NULL;}
    void    setMirror(void)         {}
    void    clrMirror(void)         {}
    void    mirrorBit(uint8_t st)   {UNUSED(st);}
#endif

};

// DEBUG
constexpr uint8_t RDsize = sizeof(TagData);
constexpr uint8_t BBsize = sizeof(Button);

#endif
