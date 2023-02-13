#ifndef BANK_H
#define BANK_H

#include "bitmasks.h"

/// Class to handle a n-bit bank
///
/// The class Bank<N> only consists of the "bare" flag vector;
/// the class BankC<N> also manages flags to record bit change.

/// THIS IS THE CLASS VERSION WITHOUT INHERITANCE.
/// The versions of these classes with inheritance would make extensive use of virtual functions.
/// However, in order to spare memory on embedded systems (e.g. on gcc for AVR8 a class with 'n'
/// virtual functions seems to take up 4+(2*n) bytes for the virtual table),
/// this non-inheriting version has been made, where Bank and BankC are independent classes
/// (though they retain the same name).
/// Of course, in this case polymorphysm can no longer be exploited.

#define NBYTES  ((NBITS+7)/8)
#define PREDEF  uint8_t b=((pin-1)>>3); uint8_t p=((pin-1)&0x07); if(b>=NBYTES) return 0;


template<byte NBITS>
class Bank
{
    private:
        uint8_t     _data[NBYTES];

    public:
        Bank(void) {}

        uint8_t     *val(void) { return _data; }
        uint8_t     size(void)  { return NBYTES; }

        // pin=1..N
        uint8_t     val(byte pin)               { PREDEF; return ((_data[b]&(1<<p))!=0); }
        uint8_t     write(byte pin, byte val)   { return (val ? set(pin) : clr(pin)); }
        uint8_t     set(byte pin)               { PREDEF; _data[b]|=(1<<p); return 1; }
        uint8_t     clr(byte pin)               { PREDEF; _data[b]&=(~(1<<p)); return 1; }

        uint16_t    valW(byte pos)              { return (((uint16_t)(_data[pos+1])<<8)+_data[pos]); }
        uint8_t     writeB(byte pos, byte val)  { if(pos>=NBYTES) return 0; _data[pos] = val; return 1; }
        uint8_t     writeW(byte pos, uint16_t wval)  { if(pos>=NBYTES) return 0; _data[pos]=(wval>>8); _data[pos+1]=(wval&0xFF); return 1; }

        uint8_t     clr(void)                   { for(byte i=0; i<NBYTES; i++) _data[i]=0; return 1; }
};

template<byte NBITS>
class BankC
{
    private:
        uint8_t     _data[NBYTES];
        uint8_t     _chg[NBYTES];

    public:
        BankC(void) {}

        uint8_t     *val(void) { return _data; }
        uint8_t     *chg(void)  { return _chg; }
        uint8_t     size(void)  { return NBYTES; }

        // pin=1..N
        // pin=1..N
        uint8_t     val(byte pin)               { PREDEF; return ((_data[b]&(1<<p))!=0); }
        void        write(byte pin, byte val)   { (val ? set(pin) : clr(pin)); }
        uint8_t     set(byte pin)               { PREDEF; _chg[b] |= (~_data[b] & (1<<p)); _data[b]|=(1<<p);    return 1; }
        uint8_t     clr(byte pin)               { PREDEF; _chg[b] |= (_data[b] & (1<<p));  _data[b]&=(~(1<<p)); return 1; }

        //uint16_t    valW(byte pos)              { ... }
        //uint8_t     writeB(byte pos, byte val)  { ... }
        //uint8_t     writeW(byte pos, uint16_t wval)  { ... }

        uint8_t     chg(byte pin)               { PREDEF; return ((_chg[b]&(1<<p))!=0); }
        uint8_t     chgClr(byte pin)            { PREDEF; _chg[b]&=(~(1<<p)); return 1; }

        uint8_t     clr(void)                   { for(byte i=0; i<NBYTES; i++) {_data[i]=0; _chg[i]=0;} return 1; }
        uint8_t     chgClr(void)                { for(byte i=0; i<NBYTES; i++) _chg[i]=0; return 1; }
};

#endif // BANK_H
