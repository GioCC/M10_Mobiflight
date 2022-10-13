/**
 *  LCDface.cpp
 *
*/

// TODO: Lead zero suppression

#include "LCDface.h"

LCDface::LCDface(void)
: _out((LiquidCrystal *)0),
  _src((char *)0), _sstart(0), _slen(0), _align(0),
  _dlen(0), _dstartr(0), _dstartc(0), _nolead0(0)
{
}

LCDface::LCDface(LiquidCrystal *lc)
: _src((char *)0), _sstart(0), _slen(0), _align(0),
  _dlen(0), _dstartr(0), _dstartc(0), _nolead0(0)
{
    _out = lc;
}

LCDface::LCDface(LiquidCrystal *lc, byte dstartr, byte dstartc, byte dlen)
    : _src((char *)0), _sstart(0), _slen(0), _align(0), _nolead0(0)
{
    _out = lc;
    _dstartr = dstartr;
    _dstartc = dstartc;
    _dlen = dlen;
}

// Refresh data from data source to internal buffer
void
LCDface::_fetch(char *src, byte slen, byte sstart, byte dstartr, byte dstartc)
{
    register byte c;
    if(slen>_dlen) slen = _dlen;    // prevent overflow
    if(slen==0) return;
    _out->setCursor(dstartr, dstartc);
    for(byte i=0; i<slen; i++) {
        c = src[sstart+i];
        if(_nolead0 && i<(slen-1) && (*src=='0')) {
            c = ' ';
        }
        _out->write(c);
    }
}

void
LCDface::FetchFmt(char *src, byte slen, byte sstart, byte align)
{
    byte offs = 0;
    if(slen==0 || slen>_dlen) slen = _dlen;
    if(slen==0) return;
    if(align==LD_ALIGN_R || align==LD_ALIGN_R_PAD)
        { offs = _dlen-slen; }
    if(align==LD_ALIGN_L_PAD || align==LD_ALIGN_R_PAD)
        { Clr(); }
    _fetch(src, slen, sstart, _dstartr, _dstartc+offs);
}

// displays all spaces (not a shutdown)
void
LCDface::Clr(char filler)
{
    _out->setCursor(_dstartr, _dstartc);
    for(byte i=0; i<_dlen; i++) {
        _out->write(filler);
    }
}


// END LEDdisplay.cpp
