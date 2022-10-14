#ifndef BANK32NV_H
#define BANK32NV_H

#include "bitmasks.h"

/// Class to handle a 32-bit bank
/// (mostly pure convenience functions, for lighter syntax and with some optimization)
///
/// The class Bank32 only consists of the "bare" flag vector;
/// the class Bank32c also manages flags to record bit change.

/// THIS IS THE CLASS VERSION WITHOUT INHERITANCE.
/// The original versions of these classes make extensive use of virtual functions.
/// However, in order to spare memory on embedded systems (e.g. on gcc for AVR8 a class with 'n'
/// virtual functions seems to take up 4+(2*n) bytes for the virtual table),
/// this non-inheriting version has been made, where Bank32 and Bank32c are independent classes
/// (though they retain the same name).
/// Of course, in this case polymorphysm can no longer be exploited.

class Bank32
{
    private:
        union {
          uint32_t b32;
          struct {
            uint16_t low;   // it appears that GCC[-AVR] is low-endian!
            uint16_t high;
          } b16;
        } data32;

    public:
        Bank32(void) {}

        // Whole data: pin=1..32
        uint32_t    val(byte pin)               { return lwbit(data32.b32,pin-1); }
        uint32_t    val(void)                   { return data32.b32; }
        uint32_t    write(byte pin, byte val)   { return (val ? lwset(data32.b32,pin-1) : lwclr(data32.b32,pin-1)); }
        uint32_t    write(uint32_t vec)         { return (data32.b32 = vec); }
        uint32_t    set(byte pin)               { return lwset(data32.b32,pin-1); }
        uint32_t    set(void)                   { return (data32.b32 = 0xFFFFFFFF); }
        uint32_t    clr(byte pin)               { return lwclr(data32.b32,pin-1); }
        uint32_t    clr(void)                   { return (data32.b32 = 0x00000000); }

        // Lower word: pin=1..16
        uint16_t    valL(byte pin)              { return wbit(data32.b16.low,pin-1); }
        uint16_t    valL(void)                  { return data32.b16.low; }
        uint16_t    writeL(byte pin, byte val)  { return (val ? wset(data32.b16.low,pin-1) : wclr(data32.b16.low,pin-1)); }
        uint16_t    writeL(uint16_t vec)        { return (data32.b16.low = vec); }
        uint16_t    setL(byte pin)              { return wset(data32.b16.low,pin-1); }
        uint16_t    setL(void)                  { return (data32.b16.low = 0xFFFF); }
        uint16_t    clrL(byte pin)              { return wclr(data32.b16.low,pin-1); }
        uint16_t    clrL(void)                  { return (data32.b16.low = 0x0000); }

        // Upper word: pin=1..16 (maps to 17..32 of LongWord)
        uint16_t    valH(byte pin)              { return wbit(data32.b16.high,pin-1); }
        uint16_t    valH(void)                  { return data32.b16.high; }
        uint16_t    writeH(byte pin, byte val)  { return (val ? wset(data32.b16.high,pin-1) : wclr(data32.b16.high,pin-1)); }
        uint16_t    writeH(uint16_t vec)        { return (data32.b16.high = vec); }
        uint16_t    setH(byte pin)              { return wset(data32.b16.high,pin-1); }
        uint16_t    setH(void)                  { return (data32.b16.high = 0xFFFF); }
        uint16_t    clrH(byte pin)              { return wclr(data32.b16.high,pin-1); }
        uint16_t    clrH(void)                  { return (data32.b16.high = 0x0000); }
};

class Bank32c
{
    private:

        union {
          uint32_t b32;
          struct {
            uint16_t low;   // it appears that GCC[-AVR] is low-endian!
            uint16_t high;
          } b16;
        } data32;

        union {
          uint32_t b32;
          struct {
            uint16_t low;   // it appears that GCC[-AVR] is low-endian!
            uint16_t high;
          } b16;
        } chg32;

    public:

        Bank32c(void) {}

        uint32_t    val(byte pin)                { return lwbit(data32.b32,pin-1); }
        uint32_t    val(void)                    { return data32.b32; }
        uint32_t    write(byte pin, byte val)    { return (val ? set(pin) : clr(pin)); }
        uint32_t    write(uint32_t vec)          { chg32.b32 |= (data32.b32^vec); return (data32.b32 = vec); }
        uint32_t    set(byte pin)                { chg32.b32 |= (~data32.b32 & lwmasks[pin-1]); return lwset(data32.b32,pin-1); }
        uint32_t    set(void)                    { chg32.b32 |= (~data32.b32);    return (data32.b32 = 0xFFFFFFFF); }
        uint32_t    clr(byte pin)                { chg32.b32 |= (data32.b32 & lwmasks[pin-1]);  return lwclr(data32.b32,pin-1); }
        uint32_t    clr(void)                    { chg32.b32 |= (data32.b32);     return (data32.b32 = 0x00000000); }

        byte        chg(byte pin)                { return (lwbit(chg32.b32,pin-1)!=0); }
        uint32_t    chg(void)                    { return chg32.b32; }
        void        chgClr(byte pin=0)           { chg32.b32 &= (pin==0 ? 0x0000 : ~lwmasks[pin-1]); }

        uint16_t    valL(byte pin)               { return wbit(data32.b16.low,pin-1); }
        uint16_t    valL(void)                   { return data32.b16.low; }
        uint16_t    writeL(byte pin, byte val)   { return (val ? setL(pin) : clrL(pin)); }
        uint16_t    writeL(uint16_t vec)         { chg32.b16.low |= (data32.b16.low^vec); return (data32.b16.low = vec); }
        uint16_t    setL(byte pin)               { chg32.b16.low |= (~data32.b16.low & wmasks[pin-1]); return wset(data32.b16.low,pin-1); }
        uint16_t    setL(void)                   { chg32.b16.low |= (~data32.b16.low);    return (data32.b16.low = 0xFFFF); }
        uint16_t    clrL(byte pin)               { chg32.b16.low |= (data32.b16.low & wmasks[pin-1]);  return wclr(data32.b16.low,pin-1); }
        uint16_t    clrL(void)                   { chg32.b16.low |= (data32.b16.low);     return (data32.b16.low = 0x0000); }

        byte        chgL(byte pin)               { return (wbit(chg32.b16.low,pin-1)!=0); }
        uint16_t    chgL(void)                   { return chg32.b16.low; }
        void        chgClrL(byte pin=0)          { chg32.b16.low &= (pin==0 ? 0x0000 : ~wmasks[pin-1]); }

        uint16_t    valH(byte pin)              { return wbit(data32.b16.high,pin-1); }
        uint16_t    valH(void)                  { return data32.b16.high; }
        uint16_t    writeH(byte pin, byte val)  { return (val ? setH(pin) : clrH(pin)); }
        uint16_t    writeH(uint16_t vec)        { chg32.b16.high |= (data32.b16.high^vec); return (data32.b16.high = vec); }
        uint16_t    setH(byte pin)              { chg32.b16.high |= (~data32.b16.high & wmasks[pin-1]); return wset(data32.b16.high,pin-1); }
        uint16_t    setH(void)                  { chg32.b16.high |= (~data32.b16.high);    return (data32.b16.high = 0xFFFF); }
        uint16_t    clrH(byte pin)              { chg32.b16.high |= (data32.b16.high & wmasks[pin-1]);  return wclr(data32.b16.high,pin-1); }
        uint16_t    clrH(void)                  { chg32.b16.high |= (data32.b16.high);     return (data32.b16.high = 0x0000); }

        byte        chgH(byte pin)              { return (wbit(chg32.b16.high,pin-1)!=0); }
        uint16_t    chgH(void)                  { return chg32.b16.high; }
        void        chgClrH(byte pin=0)         { chg32.b16.high &= (pin==0 ? 0x0000 : ~wmasks[pin-1]); }
};

#endif // BANK32NV_H
