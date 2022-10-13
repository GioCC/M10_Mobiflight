/**
 *  Frequency.h
 *
 *  Classes defining a COM/NAV frequency and an ADF frequency
 *  Deals with FSUIPC-style BCD formats
 *  
 *  This version keeps an internal buffer for ASCII representation
*/

#ifndef FREQUENCY_H
#define FREQUENCY_H

#include <Arduino.h>
#include "conversions.h"

/// This class holds a 5- or 6-digit COM/NAV frequency (108.00[0] to 136.75[0] MHz).
///
/// This range can be actually represented with one less digit (the initial '1' is assumed),
/// which allows to use:
/// - a uint16_t for the BCD representation (supplemented by an additional byte for 6-digit mode)
/// - 6- (7-) element array for ASCII.
/// THE INTEGER REPRESENTATION ALWAYS LEAVES OUT THE 100MHz DIGIT (always '1'), both internally and
/// in passed values; internally it is ALWAYS in the 6-digit mode
/// (e.g. 123.475 MHz -> 23475 (int); 123.47 MHz -> 23470 (int)).
///
/// The 5- or 6- digit mode is implicitly chosen by passing a value (Int or BCD) to the setter methods
/// in the corresponding style. The mode can be changed at any time, and it has effect on the values
/// returned (Int, BCD, ASCII) (not on the internal Int storage format).
///
/// NAV radios use 50 kHz spacing;
/// COM radios, however, use 25 kHz spacing (not considering the newer regulations for 8.33(=25/3)kHz spacing).
/// On the fractional part, this gives the progression:
/// .000 .025 .050 .075
/// In 5-digit mode, since we have to give up the 1 kHz digit, these values are rounded to:
/// .00  .02  .05  .07
///

#define IS_5DIGITS  (_BCDfrac == 0xFF)

#define MINNAV5     800
#define MAXNAV5     1795
#define MINCOM5     1800
#define MAXCOM5     3675
#define MINNAV6     8000
#define MAXNAV6     17950
#define MINCOM6     18000
#define MAXCOM6     36750

#define MINADF      1900
#define MAXADF      17500

class Frequency
{
    public:
        Frequency();

        // Setters return 1 if freq was set, 0 otherwise
        // (because invalid, or no change)
        uint8_t     setBCD(uint16_t f, uint8_t frac=0xFF);
        uint8_t     setInt(uint16_t f);

        uint16_t    BCD(uint8_t *frac=NULL) { if(frac) *frac=_BCDfrac; return _BCDfreq; }
        uint16_t    Int(void)               { return (IS_5DIGITS ? _Intfreq/10 : _Intfreq); }
        char        *ASC(uint8_t whole=1)   { return (whole ? _ASCfreq : _ASCfreq+1); }

        uint8_t     checkNAV(uint16_t f)    { return ((f>=MINNAV6 && f<=MAXNAV6)||(f>=MINNAV5 && f<=MAXNAV5)); }
        uint8_t     checkCOM(uint16_t f)    { return ((f>=MINCOM6 && f<=MAXCOM6)||(f>=MINCOM5 && f<=MAXCOM5)); }
        uint8_t     checkCOMNAV(uint16_t f) { return ((f>=MINNAV6 && f<=MAXCOM6)||(f>=MINNAV5 && f<=MAXCOM5)); }

        uint8_t     addkHz(int8_t ticks, uint8_t isCOM);
        uint8_t     addMHz(int8_t ticks, uint8_t isCOM);
        void        swap(Frequency *f);

    protected:
        uint16_t    _BCDfreq;
        uint8_t     _BCDfrac;
        uint16_t    _Intfreq;
        char        _ASCfreq[7];

        uint8_t     addInt(int16_t df, uint8_t isCOM); //      { return setInt(_Intfreq + df); }
};

// ADF Frequency in FSUIPC:
// split in main part (central 3 digits, BCD) + extended part (1000's and frac).
// A frequency of 1234.5 will have 0x0234 as main (@offset 0x034C) and 0x0105 as ext (@offset 0x0356).
//
// Returned (and stored) integer value is in kHz*10, e.g. 1234.5 kHz -> 12345
class ADFFrequency
{
    public:
        ADFFrequency();

        // Setters return 1 if freq was set, 0 otherwise
        // (because invalid or same value as existing)
        uint8_t     setBCD(uint16_t fmain, uint16_t fext);
        uint8_t     setInt(uint16_t f);

        uint16_t    BCDmain(void)       { return _BCDfreqMain; }
        uint16_t    BCDext(void)        { return _BCDfreqExt;  }
        uint16_t    Int(void)           { return _Intfreq; }
        char        *ASC(void)          { return _ASCfreq; }

        uint8_t     check(int16_t f)    { return (f>=MINADF && f<=MAXADF);}
        uint8_t     addStep(int8_t ticks, uint8_t halfkHz=0)
                                        { if(ticks==0) return 0; return addInt(ticks*(halfkHz ? 5 : 10)); }

    private:

        uint16_t    _BCDfreqMain;
        uint16_t    _BCDfreqExt;
        uint16_t    _Intfreq;
        char        _ASCfreq[6];

        uint8_t     addInt(int16_t df);  //{ return setInt(_Intfreq + df); }
};

#endif // FREQUENCY_H
