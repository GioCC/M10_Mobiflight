/**
 *  LCDface.cpp
 *
 *  Wrapper for LCD driver
 *  Manages the display for a piece of data on the LCD
 *
*/


// TODO: Lead zero suppression

#ifndef LCDface_H
#define LCDface_H

#include <Arduino.h>
#include "FastArduino.h"
#include "bitmasks.h"
#include <LiquidCrystal.h>

#define dwmask(i) (wmasks[15-i])
#define UNUSED(x) ((void)(x))

enum {
    LD_ALIGN_L = 0,
    LD_ALIGN_R = 1,
    LD_ALIGN_L_PAD = 2,
    LD_ALIGN_R_PAD = 3,
    };

class LCDface
{
    private:
        // Output connection
        LiquidCrystal *_out;

        // Data link
        char *_src;
        byte _sstart;
        byte _slen;
        byte _dstartr;
        byte _dstartc;
        byte _dlen;
        byte _align;

        // generic
        byte        _nolead0;

        void _fetch(char *src, byte len, byte sstart, byte dstartr, byte dstartc);     // helper

    public:
        LCDface(void);
        LCDface(LiquidCrystal *lc);
        LCDface(LiquidCrystal *lc, byte dstartr, byte dstartc, byte dlen);

        void    setController(LiquidCrystal *lc) { _out = lc; }

        // MapXxx() defines where the source string is displayed in the specified LedControl range.
        //
        // <slen> chars, starting from pos <sstart> within <*src>, are bound to be copied to
        // pos <dstart>...(<dstart>+<dlen>-1) in the display buffer (dstart=1 is the leftmost digit).
        // <sstart> is 0..n, <dstart> is 1..n,
        // Unused digits in the dest buffer are left untouched, allowing to overlay different LEDdisplay's
        // on a physical display.
        // If the LedControl is a chain of MAX72xx units, the LEDdisplay can span over different units;
        // in that case, dstart = 1..(8*nUnits)-1

        void    MapSrc(char *src, byte slen, byte sstart=0, byte align=0)  { _src = src; _sstart = sstart; _slen = slen; _align = align; }
        void    MapDst(byte dstartr, byte dstartc, byte dlen)              { _dstartr = dstartr; _dstartc = dstartc; _dlen = dlen; }

        // Refresh data from source
        //
        // This version uses all preset parameters, and does not regard alignment or padding
        void    Fetch(void)             { _fetch(_src, _sstart, _dlen, _dstartr, _dstartc); }
        // This version uses preset parameters just for dest
        void    FetchFmt(char *src, byte sstart=0, byte slen=0, byte align=0);
        // This version uses all preset parameters, and includes alignment/padding
        void    FetchFmt(void)          { FetchFmt(_src, _sstart, _slen, _align); }

        void    BlankLeadZeros(byte on) { _nolead0 = on; }  // only up to the first comma

        // Direct digit manipulation
        // THESE FUNCTIONS REFER TO THE DIGIT RANGE DEFINED THROUGH PARMS <dstart>, <slen> SET WITH Map()

        void    Clr(char filler = ' ');

        // Char():
        //   displays representable ASCII chars
        void    Char(byte row, byte col, byte val)
            { _out->setCursor(row, col); _out->write(val); }


};

#endif // LCDface_H
