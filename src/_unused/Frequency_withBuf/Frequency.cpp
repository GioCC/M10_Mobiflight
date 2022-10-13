/**
 *  Frequency.cpp
 *
 *  Classes defining a COM/NAV frequency and an ADF frequency
 *  Deals with FSUIPC-style BCD formats
 *  
 *  This version keeps an internal buffer for ASCII representation
*/

#include "Frequency.h"

Frequency::Frequency(void)
:_BCDfreq(0), _BCDfrac(0), _Intfreq(0)
{
    for(uint8_t i=0; i<7; i++) _ASCfreq[i] = 0;
}

// NAV FREQUENCY can be 108.000 to 117.950 MHz
// COM FREQUENCY can be 118.000 to 136.750 Mhz (137.000?)

// ILS and VOR share the same frequency range, 108 to 117.95 MHz, but they don't share the exact same frequencies.
// ILS frequencies are between 108 and 111.95, with numbers after the decimal point starting with odd numbers,
// and ending in 0 or 5, like 108.15, 108.30, etc.
// VOR stations have frequencies with numbers after the decimal point starting with odd numbers,
// and everything between 112 and 117.95.

// See also http://ww1.jeppesen.com/documents/aviation/notices-alerts/change-sets/Radio-Aids.pdf

uint8_t
Frequency::setBCD(uint16_t f, uint8_t frac)
{
    uint16_t fi;
    fi = BCDtouint(f, frac);
    // Limit check: just tests for valid COM+NAV range
    // does not catch invalid BCD
    // also, does NOT check that the value conforms to correct freq steps.
    if(!checkCOMNAV(fi)) return 0;
    if(fi==_Intfreq) return 0;
    _BCDfreq = f;
    _BCDfrac = frac;
    _Intfreq = fi;
    BCDtoASC(_BCDfreq, &_ASCfreq[1], _BCDfrac);
    _ASCfreq[0]='1';
    return 1;
}

uint8_t
Frequency::setInt(uint16_t f)
{
    // Limit check: just tests for valid COM+NAV range
    // also, does NOT check correct freq steps.
    if(f < 5000) {
        f = times10(f);
        _BCDfrac = 0xFF;   // _BCDfrac doubles as a flag for 5/6 digit mode
    }
    if(!checkCOMNAV(f)) return 0;
    if(f==_Intfreq) return 0;
    _Intfreq = f;
    _BCDfreq = uinttoBCD(f, (IS_5DIGITS ? NULL : &_BCDfrac));
    BCDtoASC(_BCDfreq, &_ASCfreq[1], _BCDfrac);
    _ASCfreq[0]='1';
    return 1;
}

uint8_t
Frequency::addInt(int16_t df, uint8_t isCOM)
{
    uint16_t newf = _Intfreq + df;
    if(isCOM) {
        if(newf<MINCOM6) newf=MINCOM6;
        else
        if(newf>MAXCOM6) newf=MAXCOM6;
    } else {
        if(newf<MINNAV6) newf=MINNAV6;
        else
        if(newf>MAXNAV6) newf=MAXNAV6;
    }
    _Intfreq = newf;
    _BCDfreq = uinttoBCD(_Intfreq, (IS_5DIGITS ? NULL : &_BCDfrac));
    BCDtoASC(_BCDfreq, &_ASCfreq[1], _BCDfrac);
    _ASCfreq[0]='1';
    return 1;
}

uint8_t
Frequency::addkHz(int8_t ticks, uint8_t isCOM)
{
    if(ticks==0) return 0;
    if (!isCOM) {
        return  addInt(ticks*50, 0);   // +50 => +0.050 MHz
    } else {
        return  addInt(ticks*25, 1);   // +25 => +0.025 MHz
    }
}

uint8_t
Frequency::addMHz(int8_t ticks, uint8_t isCOM)
{
    if(ticks==0) return 0;
    // same step for COM and NAV here
    return addInt(ticks*1000, isCOM);   // +1000 => +1 MHz
}

void
Frequency::swap(Frequency *f)
{
    uint16_t x;
    uint8_t  a;
    x = f->_Intfreq; f->_Intfreq=_Intfreq; _Intfreq=x;
    x = f->_BCDfreq; f->_BCDfreq=_BCDfreq; _BCDfreq=x;
    a = f->_BCDfrac; f->_BCDfrac=_BCDfrac; _BCDfrac=a;
    for(uint8_t i=0; i<5; i++) {
        a = f->_ASCfreq[i]; f->_ASCfreq[i]=_ASCfreq[i]; _ASCfreq[i]=a;
    }
}


// ADF FREQUENCY can be 190 kHz to 1750 kHz
// Normally 1-kHz steps are used, but some stations (and receivers) use 0.5-kHz steps.

// See also http://ww1.jeppesen.com/documents/aviation/notices-alerts/change-sets/Radio-Aids.pdf

ADFFrequency::ADFFrequency()
:_BCDfreqExt(0), _BCDfreqMain(0), _Intfreq(0)
{
    for(uint8_t i=0; i<6; i++) _ASCfreq[i] = 0;
}

uint8_t
ADFFrequency::setBCD(uint16_t fmain, uint16_t fext)
{
    // Limit check: just tests for valid freq range
    // does not catch invalid BCD
    // also, does NOT check correct 1/0.5 kHz freq steps.
    uint16_t fi;
    fi = BCD_ADFtouint(fmain, fext);

    if(!check(fi)) return 0;
    if(fi==_Intfreq) return 0;

    _BCDfreqMain = fmain;
    _BCDfreqExt  = fext;
    _Intfreq     = fi;
    BCD_ADFtoASC5(_BCDfreqMain, _BCDfreqExt, _ASCfreq);
    _ASCfreq[5]=0;
    return 1;
}

uint8_t
ADFFrequency::setInt(uint16_t f)
{
    // Limit check: just tests for valid freq range
    // also, does NOT check correct 1/0.5 kHz freq steps.
    if(!check(f)) return 0;
    if(f==_Intfreq) return 0;
    _Intfreq = f;
    uinttoBCD_ADF(f, &_BCDfreqMain, &_BCDfreqExt);
    BCD_ADFtoASC5(_BCDfreqMain, _BCDfreqExt, _ASCfreq);
    _ASCfreq[5]=0;
    return 1;
}

uint8_t
ADFFrequency::addInt(int16_t df)
{
    uint16_t newf = _Intfreq + df;
    if(newf<MINADF) newf=MINADF;
    else
    if(newf>MAXADF) newf=MAXADF;
    _Intfreq = newf;
    uinttoBCD_ADF(newf, &_BCDfreqMain, &_BCDfreqExt);
    BCD_ADFtoASC5(_BCDfreqMain, _BCDfreqExt, _ASCfreq);
    _ASCfreq[5]=0;
    return 1;
}


// END Frequency.cpp
