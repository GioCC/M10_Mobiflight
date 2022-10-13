/********************************************************************
*
*    EncoderM10.cpp - A library for controlling a bank of max 3 encoders
*    on an M10-series card
*    Derived from EncBank class
*
*    Project:  FSPanels-M10
*    Hardware: Arduino Nano / Uno on M10 card
*    Author:   Giorgio CROCI CANDIANI
*    Date:     Feb-2017
*
********************************************************************/

#define DEBUG 0

#include "EncoderM10.h"
#include <Arduino.h>

// Flag values
#define F_LONGPRESS 0x01       // Long pressure for key detected
#define F_COUNTDIR  0x10       // Count direction (0=up 1=down)
#define F_REPEAT    0x20       // After count end, restart it
#define F_ENCTRGR   0x80       // Encoder trigger input detected & processed

// Create a new controller for <n> encoders (up to 8)
EncoderM10::EncoderM10(uint8_t n)
{
    init(n);
}

void
EncoderM10::init(uint8_t n)
{
    ctdbn = DEBOUNCE;
    ctlpr = LONGPRESS;
    swvec = 0;
    swdif = 0;
    sw_up = 0;
    sw_dn = 0;
    swold = 0;
    swlpr = 0;

    //kflags = 0;
    //enAvec = 0;
    //enBvec = 0;

    eflags_u = 0;
    eflags_d = 0;
    eflags_inv = 0;
    //flags = 0;

    nencs = ((n>8) ? 8 : n);

    //trans.changed = 0;
    clearTrans();
    clearBtns();

    //clearCnts();
    encs.changed = 0x00;
    for(byte i=0; i<nencs; i++) {
    encs.ecount[i] = 0;
        encs.emode[i] = 0;
        encs.enmodes[i] = 0;
    }
}

/// Encoder flags (corresponding to HW connection) assumed in the
/// input vector passed for evaluation:
/// Bit 0, 1, 2 = Enc1A, Enc1B, Enc1S
/// Bit 3, 4, 5 = Enc2A, Enc2B, Enc2S
/// Bit 6, 7, 8 = Enc3A, Enc3B, Enc3S

void
EncoderM10::invert(uint8_t n, byte inverted)
{
    uint8_t msk = (n==0 ? 0xFF : (0x01<<((n-1)&0x07)));
    if(inverted)
        { eflags_inv |= msk; }
    else
        { eflags_inv &= ~msk; }
}


/// Update when convenient: pass the time counter (in ms), the object computes when to update
//void
//EncoderM10::update(unsigned long ms_ticks, unsigned int vec) {
//}

/// Scheduled update: no internal checks, but it is meant be called every 1ms
void
EncoderM10::update(uint32_t vec)
{
    byte  _encA = 0;
    byte  _encB = 0;
    byte  _encS = 0;

    static byte     p_encA;           // Previous values of input vectors
    //static byte  p_encB;           // not used
    static byte     p_encS;
    static uint32_t old_vec;

    uint32_t    v;
    byte        msk;

    /// time in ms since last call (should be <255 ms!)
    delta_ms -= (byte)(millis()&0xFF); // truncation gracefully takes care of overflow

    if(delta_ms==0) {
        return;    /// No sense in repeated calling within less than 1ms
    }

    msk = 0;
    // Collect data from ports
    v = vec;
    if(v != old_vec) {
        msk = 1;
        old_vec = v;
//    _encS &= 0xF8;
//    _encA &= 0xF8;
//    _encB &= 0xFC;
        uint16_t m1 = 0x01;
        uint8_t  m2 = 0x01;
        for(byte e=0; e<nencs; e++) {
            if(v & m1) { _encA |= m2; }
            m1 <<= 1;
            if(v & m1) { _encB |= m2; }
            m1 <<= 1;
            if(v & m1) { _encS |= m2; }
            m1 <<= 1;
            m2 <<= 1;
        }
    } else {

        return;
    }

    /// No debounce for encoders. Beware that this may not suit some particular devices.

    ///
    /// Encoder rotation event processing
    ///

    // Temporarily disabled, for debug:
    //enAvec = _encA;   // No particular use, but make it available
    //enBvec = _encB;   // No particular use, but make it available

    // This particular encoder always returns Ch.A/B to 0/0 at rest positions,
    // so the status must be detected on a state change of one of the channels.
    // Channel A is chosen for the trigger transition: the (steady) status of channel B gives the direction
    if(_encA != p_encA) {           // Encoder Ch.A (trigger) input detected

        // Full-cycle encoders:
        // filter out bits in _encA which have risen (or fallen, if inverted):
        //v = _encA & (_encA ^ p_encA);
        v = (_encA ^ eflags_inv) & (_encA ^ p_encA);
        // determine direction
        eflags_u = v & (~_encB);      // Up: A=0->1,  B=0
        eflags_d = v & _encB;         // Dn: A=0->1,  B=1

#ifdef HALF_CYCLE
        // Half-cycle encoders:
        // also filter out bits in _encA which have fallen (or risen, if inverted):
        //v = (~_encA) & (_encA ^ p_encA);
        v = (~(_encA ^ eflags_inv) & (_encA ^ p_encA));
        // determine direction (reverse code than before)
        eflags_d |= v & (~_encB);      // Dn: A=0->1,  B=0
        eflags_u |= v & _encB;         // Up: A=0->1,  B=1
#endif

        p_encA = _encA;
        //p_encB = _encB;   // (currently unused)

        // for each bit <i> set in eflags_u, add 1 (or larger step, if delta-T < threshold) to encs.ecount[i];
        // for each bit <i> set in eflags_d, sub 1 (or larger step, if delta-T < threshold) to encs.ecount[i];
        // v = 1;
        v = ((delta_ms < THR_VERYFAST) ? STEP_VERYFAST : ((delta_ms < THR_FAST) ? STEP_FAST : 1));

        msk = 0x01;
        for(byte i = 0; i<nencs; i++, msk+=msk) {
            encs.ecount[i]  += ((eflags_u&msk) ? v : ((eflags_d&msk) ? -v : 0));
        }
        encs.changed |= (eflags_d|eflags_u);

        trans.enUp |= eflags_u;
        trans.enDn |= eflags_d;
        trans.changed |= (eflags_d|eflags_u);

        // TODO Check & test
        if(v > 1) {
            trans.enQUp |= eflags_u;
            trans.enQDn |= eflags_d;
            // trans.changed is set by enUp/enDn
        }

    }

    ///
    ///  Debounce Logic for PUSHBUTTON
    ///

    if(_encS == p_encS) {
        ctdbn = (ctdbn > delta_ms ? ctdbn-delta_ms : 0);
        //ctdbn--;    // this is simpler if a call is guaranteed to happen once every 1ms
    } else {
        // on any variation: restart count
        ctdbn = DEBOUNCE;
        p_encS = _encS;
    }

    ///
    /// Key event processing
    ///
    /// (for documentation, see comments in the source header)

    if(ctdbn == 0) {
        /// Line vector is validated; we can update the status of the switches now.
        ctdbn = DEBOUNCE;

        /// Switch State Logic:
        ///
        /// swvec         ___---___---______  sampled & debounced switches (active high)
        /// swold         ____---___---_____  previous state
        /// swdif         ___-__-__-__-_____  changes, press or release
        /// sw_up         ___-_____-________  filter 'press' bits only
        /// sw_dn         ______-_____-_____  filter 'release' bits only
        /// keys.toggled  ___------_________  toggle flag bits for main (toggle is on rising edges)
        ///

        swvec = _encS;
        /// following processing is done for all significant bits of swvec at the same time
        swdif = swvec^swold;          // changes, press or release
        sw_up = (swdif & swvec);
        sw_dn = (swdif & swold);

        btns.changed |= swdif;    // keys.changed must be reset by the caller
        btns.current = swvec;
        btns.activated = ((btns.activated | sw_up) & ~sw_dn);
        btns.deactivated = ((btns.deactivated | sw_dn) & ~sw_up);
        btns.toggled ^= sw_up;
        swold = swvec;
        /// manage mode change
        msk = 0x01;
        for(byte i = 0; i<8; i++, msk+=msk) {
            if(encs.emode[i] & 0x7F) {
                /// handle modechange + Push&Hold-On
                if(sw_up & msk) {
                    if((encs.emode[i] & 0x80) == 0) {
                        incMode(i, 0x00);
                    }
                }
                /// handle Push&Hold-Off
                if(sw_dn & msk) {
                    if(encs.emode[i] == 1) {
                        setMode(i, 1);
                    }
                }
            }
        }

        /// Update counter for long press

        /// for simplicity, long press is not computed for every single button;
        /// a l.p. interval is counted from the moment in which a fixed pattern of pressed buttons exists.
        if(swdif) {
            ctlpr = LONGPRESS;  /// any variation restarts long press counter
            /// Released keys are to be marked as Short Presses, UNLESS they were
            /// still pressed from the latest Long Press pattern
            btns.shortpress |= (sw_dn & ~swlpr);
            btns.shortpress &= ~sw_up;        // Keys just pressed are not SP until we know when they are released
            btns.longpress &= ~sw_dn;         // Released keys are not LP anymore
            swlpr &= ~sw_dn;                  // also, remove them from last stored LP pattern
        } else {
            if(ctlpr > 0) {
                if(--ctlpr == 0) {
                    swlpr = swvec;                // keep LP pattern stored for release check
                    btns.longpress |= swvec;      // All keys of current pattern are now LP pattern
                    btns.shortpress &= ~swvec;    // Therefore, none of them is SP
                    /// manage mode change
                    msk = 0x01;
                    for(byte i = 0; i<8; i++, msk+=msk) {
                        if(swlpr & msk) {
                            if(encs.emode[i] & 0x80) {
                                incMode(i, 0x00);
                            }
                        }
                    }
                }
            }
        }
    }
}

/// Encoder modes

/// Read counter value; resets counter and 'change' flag
int
EncoderM10::getEncCount(byte n, byte reset)
{
    int res;
    if((n<1) || (n>nencs)) { return 0x0000; }
    n--;
    res = encs.ecount[n];
    if(reset) encs.ecount[n] = 0;
    encs.changed &= (~(1<<n));
    return res;
};     // resets counter and 'change' flag


/// Define number of modes for an encoder.
/// n = encoder no. (1..nencs)
/// nmodes = number of modes to assign (0..127)
void
EncoderM10::setNModes(byte n, byte nmodes)
{
    // Value 128 (No modes + longpress) not allowed
    if((n<1)||(n>nencs)||(nmodes==0x80)) { return; }
    encs.enmodes[n-1]=nmodes;
}

/// Set current mode for an encoder.
/// n = encoder no. (1..nencs)
/// nmodes = number of modes to assign (0..127)
void
EncoderM10::setMode(byte n, byte nmode)
{
    byte allowed;
    if((n<1) || (n>nencs)) { return; }
    // If nmode is not consistent with the range set with setNMode, call has no effect
    if(nmode == 0) { return; }
    allowed = (encs.enmodes[n-1]) & 0x7F;
    if(allowed == 0) { return; }
    if((allowed > 1) ? (nmode <= allowed) : (nmode < nencs)) {
        encs.enmodes[n-1] = nmode;
    }
}

/// Get current mode for an encoder.
/// n = encoder no. (1..nencs)
byte
EncoderM10::getMode(byte n)
{
    if((n<1) || (n>nencs)) { return 0xFF; }
    return encs.emode[n-1];
}

/// Increment (or decrement) current mode for an encoder.
void
EncoderM10::incMode(byte n, byte flags)
{
    byte nmax;
    byte nn;
    if((n<1) || (n>nencs)) { return; }
    nmax = (encs.enmodes[n-1]) & 0x7F;
    nn = encs.emode[n-1];
    // flags&0x01: 0->inc, 1->decs
    // flags&0x02: 0->Value stops at edges, 1->Value wraps around
    if(flags&INCMODE_NOWRAP) {
        if(flags&INCMODE_DEC) {
            if(nn>1) {
                nn--;
            }
        } else {
            if(nn<nmax) {
                nn++;
            }
        }
    } else {
        if(flags&INCMODE_DEC) {
            nn = (nn>1 ? nn-1 : nmax);
        } else {
            nn = (nn<nmax ? nn+1 : 1);
        }
    }
    encs.emode[n-1] = nn;
}

// Global instance (singleton use, for embedded)
//EncoderM10 EB = EncoderM10();

/// end class EncoderM10

