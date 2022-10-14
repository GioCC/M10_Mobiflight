#ifndef HEADER_4CA953C7A0B90209
#define HEADER_4CA953C7A0B90209

/**
 *  LEDface.cpp
 *
 *  Wrapper for (modified) LedControl MAX7219 display driver
 *  Manages the display for a piece of data (1 to 8 digits), also allowing
 *  to overlay more than one of them on a single LedControl.
 *  Handles chained MAX7219s; a LEDface can span different physical displays.
 *  >>> Max length for a LEDface is 16 digits
 *
*/


// TODO: Lead zero suppression

#ifndef LEDFACE_H
#define LEDFACE_H

#include <Arduino.h>
#include "FastArduino.h"
#include "LedControlMod.h"
#include "bitmasks.h"

#define dwmask(i) (wmasks[15-i])
#define UNUSED(x) ((void)(x))

#define _UNIT(a) ((a)>>3)
#define _DGT(a)  ((a) & 0x07)

enum {
    LD_ALIGN_L = 0,
    LD_ALIGN_R = 1,
    LD_ALIGN_L_PAD = 2,
    LD_ALIGN_R_PAD = 3,
    };

class LEDface
{
    private:
        // Output connection
        LedControl *_out;

        // Data link
        char *_src;
        byte _sstart;
        byte _slen;
        byte _dstart;
        byte _dlen;
        byte _align;

        // generic
        uint16_t    _dpmask;        // Leftmost bit (15) is the leftmost digit
        byte        _nolead0;

        void _fetch(char *src, byte slen, byte dstart, uint16_t dpmsk);     // helper

    public:

        // WARNING 'LedControl's used should be set with width (and scanlimit) = 8
        // (see comments below)
        LEDface(void);
        LEDface(LedControl *lc);
        LEDface(LedControl *lc, byte dstart, byte dlen);

        void    setController(LedControl *lc) { _out = lc; }

        // MapXxx() defines where the source string is displayed in the specified LedControl range.
        //
        // <len> chars, starting from pos <sstart> within <*src>, are bound to be copied to
        // pos <dstart>...(<dstart>+<len>-1) in the display buffer (dstart=1 is the leftmost digit).
        // <len> is either <slen> or <dlen>, whichever is shorter (usually <slen> will be set to 0xFF,
        // unless source length is shorter than the viewport).
        // <sstart> is 0..n, <dstart> is 1..n,
        // Unused digits in the dest buffer are left untouched, allowing to overlay different LEDdisplay's
        // on a physical display.
        // If the LedControl is a chain of MAX72xx units, the LEDdisplay can span over different units;
        // in that case, dstart = 1..(8*nUnits)-1
        //
        // IMPORTANT: All display units are supposed to be 8-digit wide.
        // Beware of following:
        // - scanlimit settings. These would not disrupt the digit count, although incorrect settings might make some significant digits blank.
        // - LedControl settings that limit the effective display width (internally paired with 'scanlimit' limits). These WILL disrupt the digit count
        // (at least for intermediate units in a chain), therefore all LedControls should be set with the maximum (=default) width.

        void    MapSrc(char *src, byte slen = 0xFF, byte sstart=0, byte align=0){ _src = src+sstart; _slen = slen; _align = align; }
        void    MapDst(byte dstart, byte dlen)                             { _dstart = dstart-1; _dlen = dlen; }

        // Refresh data from source
        //
        // Text is written into LedControl buffers, but actual transmission must be triggered separately
        // by calling Send()
        // (this way, several LEDfaces on the same controller can be written before transmission).

        // This version uses all preset parameters, and does not regard alignment or padding
        void    Fetch(void)         { _fetch(_src, _dlen, _dstart, _dpmask); }
        // This version uses preset parameters just for dest
        void    Write(char *src, byte sstart=0, byte slen=0, byte align=0);
        // This version uses all preset parameters, and includes alignment/padding
        void    FetchFmt(void)      { Write(_src, 0, _slen, _align); }

        void    Send(void)          { _out->transmit(); }

        // Set Decimal Point pattern.
        // Calls Fetch to update digits displayed (if they are unchanged, they remain unaffected)
        // 'mask.0' is <rightmostdigit>.dp
        // DP can also be set by writing the corresponding char with bit 7 set.
        void    DPmask(uint16_t mask, byte sendnow=0)   { _dpmask = (mask<<(16-_dlen)); if(sendnow) Fetch(); }

        void    BlankLeadZeros(byte on)         { _nolead0 = on; }  // only up to the first comma


        // Direct digit manipulation
        // THESE FUNCTIONS REFER TO THE DIGIT RANGE DEFINED THROUGH PARMS <dstart>, <slen> SET WITH Map()

        void    Clr(char filler = ' ');

        // dispDigit():
        //   displays only values 0..15 (as '0'..'9','-','E','H','L','P',' '
        //   and values '0'...'9'  (as themselves)
        //   nrdigit: 0..7
        void    Digit(byte nrdigit, byte val, byte sendnow=1)
            { _out->setDigit(_UNIT(nrdigit), 7-_DGT(nrdigit), val, ((_dpmask & dwmask(nrdigit))!=0), !sendnow); }

        // dispChar():
        //   displays representable ASCII values from a char lookup table
        void    Char(byte nrdigit, byte val, byte sendnow=1)
            { _out->setChar(_UNIT(nrdigit), 7-_DGT(nrdigit), val, ((_dpmask & dwmask(nrdigit))!=0), !sendnow); }


};

#endif // LEDFACE_H
#endif // header guard 

