/**
 *  LEDface.cpp
 *
*/

// TODO: Lead zero suppression

#include "LEDface.h"

LEDface::LEDface(void)
: _out((LedControl *)0),
  _src((char *)0), _slen(0), _dstart(0),  _dlen(0),
  _align(0), _dpmask(0), _nolead0(0)
{
}

LEDface::LEDface(LedControl *lc)
: _src((char *)0), _slen(0), _dstart(0),  _dlen(0),
  _align(0), _dpmask(0), _nolead0(0)

{
    _out = lc;
}

LEDface::LEDface(LedControl *lc, byte dstart, byte dlen)
    : _src((char *)0), _slen(0), _align(0),
    _dpmask(0), _nolead0(0)
{
    _out = lc;
    _dstart = dstart;
    _dlen = dlen;
}

// Refresh data from data source to internal buffer
void
LEDface::_fetch(char *src, byte slen, byte dstart, uint16_t dpmsk)
{
    register byte c;
    if(slen>_dlen) slen = _dlen;    // prevent overflow
    if(slen==0) return;
    for(byte i=0; i<slen; i++) {
        c = src[i];
        if(_nolead0 && i<(slen-1) && (*src=='0')) {
            c = ' ';
        }
        _out->setChar(_UNIT(dstart+i), 7-_DGT(dstart+i), c, ((dpmsk & dwmask(i))!=0), 1);
    }
}

void
LEDface::Write(char *src, byte slen, byte sstart, byte align)
{
    byte offs = 0;
    if(slen==0 || slen>_dlen) slen = _dlen;
    if(slen==0) return;
    if(align==LD_ALIGN_R || align==LD_ALIGN_R_PAD)
        { offs = _dlen-slen; }
    if(align==LD_ALIGN_L_PAD || align==LD_ALIGN_R_PAD)
        { Clr(); }
    _fetch(src, slen, _dstart+offs, _dpmask);
}

// displays all spaces (not a shutdown)
void
LEDface::Clr(char filler)
{
    for(byte i=_dstart; i<_dstart+_dlen; i++) {
        _out->setChar(_UNIT(i), _DGT(i), filler, ((_dpmask & dwmask(i))!=0), 1);
    }
}


// END LEDdisplay.cpp
