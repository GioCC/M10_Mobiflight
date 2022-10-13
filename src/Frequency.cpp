/**
 *  Frequency.cpp
 *
 *  Classes defining a COM/NAV frequency and an ADF frequency
 *  Deals with FSUIPC-style BCD formats
 *  This version does NOT keep an internal buffer for ASCII representation;
 *  an external buffer must be supplied if required.
*/

#include "Frequency.h"

Frequency::Frequency(void)
:_BCDfreq(0), _BCDfrac(0), _Intfreq(0)
{
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
Frequency::setBCD(uint16_t f, uint8_t frac, char *ASCdst, uint8_t trimmed)
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
    ASC(ASCdst, trimmed);
    return 1;
}

uint8_t
Frequency::setInt(uint16_t f, char *ASCdst, uint8_t trimmed)
{
    // Limit check: just tests for valid COM+NAV range
    // also, does NOT check correct freq steps.
    if(f < 5000) {
        f = times10(f);
        _BCDfrac = NO_FRAC;   // _BCDfrac doubles as a flag for 5/6 digit mode
    }
    if(!checkCOMNAV(f)) return 0;
    setInt(f);
    ASC(ASCdst, trimmed);
    return 1;
}

// private helper
void
Frequency::setInt(uint16_t f)
{
    if(f==_Intfreq) return;
    _Intfreq = f;
    _BCDfreq = uinttoBCD(f, (IS_5DIGITS ? NULL : &_BCDfrac));
}

char *
Frequency::ASC(char *ASCdst, uint8_t trimmed)
{
    if(ASCdst) {
        BCDtoASC(_BCDfreq, (trimmed ? ASCdst : &ASCdst[1]), _BCDfrac);
        if(!trimmed) ASCdst[0]='1';
    }
}

void
Frequency::addInt(int16_t df, uint8_t isCOM)
{
    _Intfreq = (isCOM ? trimCOM(_Intfreq + df) : trimNAV(_Intfreq + df));
    _BCDfreq = uinttoBCD(_Intfreq, (IS_5DIGITS ? NULL : &_BCDfrac));
}

void
Frequency::addTicks(int8_t ticks, uint8_t isCOM, char *ASCdst, uint8_t trimmed)
{
    if(ticks==0) return;
    // +25 => +0.025 MHz
    // +50 => +0.050 MHz
    addInt(ticks*(isCOM ? 25 : 50), isCOM);
    if(ASCdst) {
        ASC(ASCdst, trimmed);
    }
}

void
Frequency::addkHz(int8_t ticks, uint8_t isCOM, char *ASCdst, uint8_t trimmed)
{
    if(ticks==0) return;
    int16_t newf;
    if (isCOM) {
        newf = (_Intfreq/1000)*1000+((_Intfreq+(ticks*25))%1000);   // +25 => +0.025 MHz
        setInt(trimCOM(newf));
    } else {
        newf = (_Intfreq/1000)*1000+((_Intfreq+(ticks*50))%1000);   // +50 => +0.050 MHz
        setInt(trimNAV(newf));
    }
    if(ASCdst) {
        ASC(ASCdst, trimmed);
    }
}

void
Frequency::addMHz(int8_t ticks, uint8_t isCOM, char *ASCdst, uint8_t trimmed)
{
    if(ticks==0) return;
    int16_t newf;
    // same step for COM and NAV here
    newf = (_Intfreq/1000+ticks)*1000+(_Intfreq%1000);   // +1000 => +1 MHz
    setInt(isCOM ? trimCOM(newf) : trimNAV(newf));
    if(ASCdst) {
        ASC(ASCdst, trimmed);
    }
}

void
Frequency::swap(Frequency *f)
{
    uint16_t x;
    uint8_t  a;
    x = f->_Intfreq; f->_Intfreq=_Intfreq; _Intfreq=x;
    x = f->_BCDfreq; f->_BCDfreq=_BCDfreq; _BCDfreq=x;
    a = f->_BCDfrac; f->_BCDfrac=_BCDfrac; _BCDfrac=a;
}


// ADF FREQUENCY can be 190 kHz to 1750 kHz
// Normally 1-kHz steps are used, but some stations (and receivers) use 0.5-kHz steps.

// See also http://ww1.jeppesen.com/documents/aviation/notices-alerts/change-sets/Radio-Aids.pdf

ADFFrequency::ADFFrequency()
:_BCDfreqExt(0), _BCDfreqMain(0), _Intfreq(0)
{
}

uint8_t
ADFFrequency::setBCD(uint16_t fmain, uint16_t fext, char *ASCdst)
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
    ASC(ASCdst);
    return 1;
}

uint8_t
ADFFrequency::setInt(uint16_t f, char *ASCdst)
{
    // Limit check: just tests for valid freq range
    // also, does NOT check correct 1/0.5 kHz freq steps.
    if(!check(f)) return 0;
    if(f==_Intfreq) return 0;
    _Intfreq = f;
    uinttoBCD_ADF(f, &_BCDfreqMain, &_BCDfreqExt);
    ASC(ASCdst);
    return 1;
}

char *
ADFFrequency::ASC(char *ASCdst)
{
    if(ASCdst) {
        BCD_ADFtoASC5(_BCDfreqMain, _BCDfreqExt, ASCdst);
        ASCdst[5]=0;
    }
    return ASCdst;
}

uint8_t
ADFFrequency::addStep(int8_t ticks, uint8_t halfkHz, char *ASCdst)
{
    if(ticks==0) return 0;
    addInt(ticks*(halfkHz ? 5 : 10));
    ASC(ASCdst);
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
    return 1;
}


// END Frequency.cpp
