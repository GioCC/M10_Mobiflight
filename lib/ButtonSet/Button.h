// =======================================================================
// @file        Button.h
//
// @project     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-18
// @modifiedby  GiorgioCC - 2023-10-16 22:52
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================

#ifndef BUTTON_H
#define BUTTON_H

//======================================
//=== Compilation options
//======================================

/// #define SOURCEVAR / MIRRORVAR if Source Var / Mirror Var features should be compiled
#define SOURCEVAR
#define MIRRORVAR

/// #define FETCH_CB if "Input Fetcher Callback" feature should be compiled
#define FETCH_CB

// #define MAKE_NEW to use alternative methods of allocation (defaults to standard "new")
//#define MAKE_NEW(obj)   new obj()

#include <Arduino.h>
#include <new>

#define flagChg(value, bitmsk, cond) ((cond) ? (value |= bitmsk) : (value &= ~bitmsk) )
#define times10(n)  ((n<<1)+(n<<3))
#define times100(n) ((n<<2)+(n<<5)+(n<<6))
#define UNUSED(x) ((void)(x))


// Following macro is used in the derived classes to automatically define aliases for all
// basic setup methods.

#define DEFINE_BASIC_METHODS(derived) \
derived& pin(uint8_t npin, uint8_t isHW)      { Button::pin(npin, isHW); return *this; } \
derived& tag(const char *s)                   { Button::tag(s);     return *this; } \
derived& tag(byte *b)                         { Button::tag(b);     return *this; } \
derived& tag(uint16_t code)                   { Button::tag(code);  return *this; } \
derived& data(const char *s)                  { Button::data(s);    return *this; } \
derived& data(byte *b)                        { Button::data(b);    return *this; } \
derived& data(uint16_t code)                  { Button::data(code); return *this; } \
derived& mirror(uint8_t *mvar, uint8_t mbit)  { Button::mirror(mvar, mbit); return *this; } \
derived& source(uint8_t *svar, uint8_t sbit)  { Button::source(svar, sbit); return *this; } \
static derived& make(void)                    { derived* pb = new derived;    if(_collect != nullptr) _collect(pb); return *pb; } \
static derived& make(void* p)                 { derived* pb = new(p) derived; if(_collect != nullptr) _collect(pb); return *pb; }

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
    
    using ButtonValue_t = union {
        uint8_t aVal;
        uint8_t dVal;
    };

    // Status values in the bit pattern passed as argument to check()
    using ButtonStatus_t = enum {
        High = 0x01,    // Constant for Current status = 1
        Low  = 0x00,    // Constant for Current status = 0
        None = 0x00,    // Constant for no trigger
        Curr = 0x01,    // Current status
        Dn   = 0x02,    // Transitioned High->Low
        Up   = 0x04,    // Transitioned Low->High
        Rpt  = 0x08,    // Repeat activation triggered
        Long = 0x10,    // Long press activation triggered
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


    // Bitmasks for bit nos. used for src and mirror vars
    enum {
        srcBits     = 0xF0,   // bit# in the source variable
        mirBits     = 0x0F    // bit# in the mirror variable
    } t_bitno;


    // Define type of injected function to automatically add a new button to a given collection
    using BTNcollector = void (*)(Button*);

protected:

    // If non-null, this callback is invoked when a new Button object is created with a "make" factory function,
    // so the new Button is automatically added to a given collection/manager object.
    // BEWARE: it is NOT called if the object is created directly (must be called manually, if desired, through 
    // its public alias "Collect()")
    static BTNcollector _collect;

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
#ifdef FETCH_CB
    uint8_t         (*fetchCB)(uint8_t);
#endif

    void modeAnalog(uint8_t v) {
        flagChg(_flags, Button::Analog, v);
    }

    void valBit(uint8_t v) {
        // Record current input value
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
        uint8_t     *mirrorvar=nullptr,
        uint8_t     mirrorbit=0
        );

    Button( 
        uint8_t     pin,
        uint8_t     useHWinput,
        uint16_t    tag,
        uint8_t     *mirrorvar=nullptr,
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

    static Button& make(void)       
        { Button* pb = new Button;    if(_collect != nullptr) _collect(pb); return *pb; }
    static Button& make(void *p)    
        { Button* pb = new(p) Button; if(_collect != nullptr) _collect(pb); return *pb; }

    static void setCollector(BTNcollector cb) { _collect = cb; }

    Button& Collect(void) 
        { if(_collect != nullptr) _collect(this); return *this;}

    // ======================================
    // === Setup methods
    // ======================================
    // These methods, all returning the object itself, are meant to allow chained configuration calls like:
    //   make().tag("MyName").data(0x6E)
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
    
    // Set source var reference
    Button& source(uint8_t* sourcevar, uint8_t sourcebit)
    #ifdef SOURCEVAR
        { srcVar = sourcevar; srcBit = sourcebit; return *this; }
    #else
        { UNUSED(sourcevar); UNUSED(sourcebit); return *this; }
    #endif

    // Set mirror var reference
    Button& mirror(uint8_t *mirrorvar, uint8_t mirrorbit)
    #ifdef MIRRORVAR
        { mirrVar = mirrorvar; mirrBit = mirrorbit; return *this; }
    #else
        { UNUSED(mirrorvar); UNUSED(mirrorbit); return *this; }
    #endif

    Button& setFetchCallback(uint8_t (*callback)(uint8_t))
    #ifdef FETCH_CB
        { fetchCB = callback; return *this; }
    #else
        { UNUSED(callback); return *this; }
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

    // Define 'getInput' as 'virtual' if required
    uint8_t     getInput(void);

    // ======================================
    // === Operation methods
    // ======================================

    // Triggers the appropriate events according to the state of the button.
    // 'Status' is a bit pattern with following meaning:
    //      Button::Curr    Current input status
    //      Button::Dn      Input just became active
    //      Button::Up      Input was just released
    //      Button::Rpt     A repeat interval just expired
    //      Button::Long    The long press interval just expired
    // All flags except Button::curr are expected to be only set for the call right after the event occurs.
    virtual void process(uint8_t status) { UNUSED(status); }

    // Checks the state of the button: 
    // polls status value internally and triggers events accordingly.
    // This method must be (re)defined in the specific button type classes.
    virtual void check(bool force = false) { UNUSED(force); }

    // Variant of 'check()' with input value (analog or digital, as supported)
    // passed from outside by the caller. Status flags are computed internally.
    virtual void checkVal(uint8_t val, bool force = false) { UNUSED(val); UNUSED(force); };

    // Variant of 'checkVal()' for digital input vectors (specific to derived class)
    // Gets its input source from the passed byte array according to pin#:
    // bytevec[0] contains values of pins 1..8 (bits 0..7), bytevec[1] contains pins 9..16 etc
    // It is responsibilty of the caller to assure that bytevec[] has a size compatible with the button's pin no.
    //
    // If the button is configured for direct HW pin reading or Analog source, a call to this method has NO EFFECT.
    virtual void checkVec(uint8_t *bytevec, bool force = false) { UNUSED(bytevec); UNUSED(force); };

    // Supplied only by classes with analog inputs: translates the analog value to the corresponding digital state.
    virtual bool ana2dig(uint8_t val) { return false; }

    // ======================================
    // === Initial syncing methods
    // ======================================

    // initState is used to assign the initial value.
    // It differs from check() because it only triggers _OnPress/_OnRelease events.
    // These are usually associated to stable switches (rather than temporary pushbuttons),
    // which require to have their position synced at startutp
    void initState(void) { check(1); }
    
    // Version where the value is supplied from an external button manager
    void initState(uint8_t status) { process(status); }

    // Version where the value is a direct input value
    void initStateVal(uint8_t val) { checkVal(val, 1); };
    
    // Version where the value is supplied from an external button manager as digital vector
    void initStateVec(uint8_t *bytevec) { checkVec(bytevec, 1); };
    
    // ======================================
    // === Conditional definitions
    // ======================================

#ifdef SOURCEVAR
    uint8_t     hasSrcVar(void)         {return (srcVar != nullptr);}
    uint8_t*    getSrcVar(void)         {return srcVar; }
    uint8_t     getSrcVal(void)         {return (isAna() ? *srcVar : (((*(srcVar + (srcBit>>3))&(1<<(srcBit&0x07))) ? HIGH : LOW))); }
#else
    uint8_t     hasSrcVar(void)         {return false;}
    uint8_t*    getSrcVar(void)         {return nullptr;}
    uint8_t     getSrcVal(void)         {return 0;}
#endif

#ifdef MIRRORVAR
    uint8_t     hasMVar(void)           {return (srcVar != nullptr);}
    uint8_t*    getMVar(void)           {return mirrVar; }
    void        setMirror(void)         {if(!isAna() && (mirrVar!=nullptr)) {*(mirrVar + (mirrBit>>3)) |=  (1 << (mirrBit&0x07));}}
    void        clrMirror(void)         {if(!isAna() && (mirrVar!=nullptr)) {*(mirrVar + (mirrBit>>3)) &= ~(1 << (mirrBit&0x07));}}
    void        mirrorBit(uint8_t st)   {st ? setMirror() : clrMirror();}
#else
    uint8_t     hasMVar(void)           {return false;}
    uint8_t*    getMVar(void)           {return nullptr;}
    void        setMirror(void)         {}
    void        clrMirror(void)         {}
    void        mirrorBit(uint8_t st)   {UNUSED(st);}
#endif

#ifdef FETCH_CB
    uint8_t     hasFetch(void)          {return (fetchCB != nullptr);}
    uint8_t     fetchVal(void)          {return (hasFetch() ? fetchCB(_pin) : LOW); }
#else
    uint8_t     hasFetch(void)          {return false;}
    uint8_t     fetchVal(void)          {return LOW; }
#endif

};

// DEBUG
constexpr uint8_t RDsize = sizeof(TagData);
constexpr uint8_t BBsize = sizeof(Button);

#endif
