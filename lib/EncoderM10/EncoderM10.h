/********************************************************************
*    EncoderM10.h - A library for controlling a bank of encoders
*    on an M10-series card
*    Derived from EncBank class
*
*    Project:  FSPanels-M10
*    Hardware: Arduino Nano / Uno on M10 card
*    Author:   Giorgio CROCI CANDIANI
*    Date:     Feb-2017
*
********************************************************************/

/// Encoder flags (corresponding to HW connection) assumed in the
/// input vector passed for evaluation:
/// Bit 0, 1, 2 = Enc1A, Enc1B, Enc1S
/// Bit 3, 4, 5 = Enc2A, Enc2B, Enc2S
/// Bit 6, 7, 8 = Enc3A, Enc3B, Enc3S

#ifndef EncoderM10_h
#define EncoderM10_h

#include <Arduino.h>
#include "bitmasks.h"

/// Max no of encoders managed with current implementation can be 8
/// If you're _really_ tight on memory, reducing following parameter might help save a few bytes
#define MAXENC 8

/// Type of encoder
/// Default is full-cycle (both A/B signals make a complete cycle from one detent to another)
/// Define following constant for half-cycle encoders
//#define HALF_CYCLE

/// Constants for Encoder acceleration
/// If the period (ms) between two variations is lower than THRESHOLDn, add STEPn counts instead of just 1
#define THR_VERYFAST    5       // very fast: <5ms
#define STEP_VERYFAST   5       // very fast: step *5
/// To disable second threshold, set it to the same value as first one
#define THR_FAST        5
//#define THR_FAST        10      // fast: <10ms
#define STEP_FAST       5       // fast: step *5

/// Constants for PUSHBUTTONS
#define DEBOUNCE  10           // Debounce validation interval (ms)
#define LONGPRESS 50           // Long press interval (*DEBOUNCE ms)

/// This class manages encoders (w/switch) basing on a "raw" digital I/O vector
/// (likely coming from a direct port read); it does NOT read hardware
/// inputs directly, therefore input data can also be pre-processed or simulated
/// or whatever required.
/// The class handles:
/// - Input debounce (in this version: just for buttons)
/// - Transition detection for rotation (normal and fast)
/// - Status and transition detection for button (short/long press, toggle, )
/// - Accumulation counters (see also below).
/// The input vector should contain, from bit 0 (rightmost) upwards:
/// Enc1A, Enc1B, Enc1S, Enc2A, Enc2B, Enc2S....
///
/// Encoder counters can be used in two ways:
/// - as temporary accumulators, to allow the caller to consume the values
///   less often without losing counts; at each read access, the read value is
///   reset. In this case getEncCount(..) is used;
/// - as absolute counters, to mark the encoder's absolute position.
///   In this case, changes in the count values are detected with getEncChange();
///   changed values can be read with readEncCount(..), which does not alter the count
///   (but resets the <change> flags for those encoders that have been read).
///
///   Initial reset or zeroing of the counters can be done in both modes with
///   a "dummy" call to getEncCount(..).

class EncoderM10
{

public:

    /// Encoder transitions
    typedef struct {
        byte changed;           // flags keys that have changed since last zeroing (which must be done by the user)
        byte enUp;              // Up
        byte enDn;              // Down
        byte enQUp;             // Quick Up
        byte enQDn;             // Quick Down
        //byte enAvec;            // Encoder A lines vector
        //byte enBvec;            // Encoder B lines vector
    } t_enctstat;

    /// Encoder counts and modes
    typedef struct {
        byte changed;            // Mark encoder whose counts have changed since last read
        long ecount[MAXENC];     // Net encoder counts accumulated since last read (can be neg)
        byte emode[MAXENC];      // Current mode value for each encoder
        byte enmodes[MAXENC];    // Number of modes for each encoders
    } t_enccstat;

    /// Encoder button transitions
    typedef struct {
        byte changed;       // flags keys that have changed since last zeroing (which must be done by the user)
        byte current;       // mirrors current (debounced) btn status
        byte activated;     // set on ACTIVATION, reset on deactivation (or by user)
        byte deactivated;   // set on DEACTIVATION, reset on activation (or by user)
        byte toggled;       // toggle: set on ACTIVATION, reset on next activation (or by user)
        byte shortpress;    // set on SHORT PRESS trigger; reset on deactivation (or by user)
        byte longpress;     // set on LONG PRESS (past threshold time); reset on deactivation (or by user)
    } t_btnstat;


    /// Encoder pulse transition flags
    ///  Up/Dn flags in following vectors are only SET by this code,
    ///  they must be reset by the user.
    ///  If QUp is set, so is also Up (same for QDn and Dn)
    // Following btn and enc vars should be made private
    // Currently, for the sake of efficiency, they are left public despite it being bad
    // from a programming point of view.
    t_enctstat  trans;          // Transition flags
    t_enccstat  encs;           // Counters
    t_btnstat   btns;           // Status of pushbuttons

private:

    byte delta_ms;

    byte nencs;                 // number of encoders managed

    /// Variables for keys (pushbuttons)
    byte ctdbn;                 // input debounce counter
    byte ctlpr;                 // input long press counter

    //uint16_t old_vec;                // Previous port values

    byte swvec;             // (internal) current switch state
    byte swdif;             // (internal) switch state change
    byte sw_up;             // (internal) switch state change, activation only
    byte sw_dn;             // (internal) switch state change, deactivation only
    byte swold;             // (internal) switch state previous state
    byte swlpr;             // (internal) switch long press mask stored

    //byte kflags;            // switch flags - mirrors current (debounced) btn status.

    /// Variables for encoders

    byte eflags_u;          // Encoder flags - up count
    byte eflags_d;          // Encoder flags - down count
    byte eflags_inv;        // Encoder flags - inversion

public:

    /// Create a new controller for <n> encoders (up to 8)
    EncoderM10(uint8_t n);

    void    init(uint8_t n);

    /// Invert rotation of specified encoder
    void    invert(uint8_t n, byte inverted=1);

    /// Update when convenient: pass the time counter (in ms), the object computes when to update
    //void    update(unsigned long ms_ticks);

    /// Scheduled update: no internal checks, but it is supposed be called about every 1ms
    void    update(uint32_t vec);

    // Following functions are meant for use in more correct OOP, if key and enc vars were made private
    // Currently, for the sake of efficiency, key and enc vars are kept public despite it being bad
    // from a programming point of view.
    t_btnstat   *getBtns(void)      __attribute__((always_inline)) { return &btns; }
    t_enctstat  *getEncStatus(void) __attribute__((always_inline)) { return &trans; }
    t_enccstat  *getEncCnt(void)    __attribute__((always_inline)) { return &encs; }

    ///
    /// TRANSITIONS
    ///
    /// Return mask of encoders which had a transition (of the corresponding type) since last read
    /// If an enc no. != 0 is specified, the returned value is relative to that encoder only (!=0 on change)
    /// Because of inlining, no range check is performed on the encoder index
    byte getEncChange(byte nenc=0)      __attribute__((always_inline))  { return trans.changed & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    byte getEncChangeUp(byte nenc=0)    __attribute__((always_inline))  { return trans.enUp & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    byte getEncChangeDn(byte nenc=0)    __attribute__((always_inline))  { return trans.enDn & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    byte getEncChangeQUp(byte nenc=0)   __attribute__((always_inline))  { return trans.enQUp & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    byte getEncChangeQDn(byte nenc=0)   __attribute__((always_inline))  { return trans.enQDn & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    void clearTrans(void)               __attribute__((always_inline))  { memset(&trans, 0, sizeof(t_enctstat)); }

    ///
    /// BUTTONS
    ///
    /// Return mask of encoders BUTTONS which had a transition (of the corresponding type) since last read
    /// If an enc no. != 0 is specified, the returned value is relative to that encoder only (!=0 on change)
    /// Because of inlining, no range check is performed on the encoder index
    byte getBtnChange(byte nenc=0)      __attribute__((always_inline))  { return btns.changed & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    byte getBtnPress(byte nenc=0)       __attribute__((always_inline))  { return btns.activated & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    byte getBtnRelease(byte nenc=0)     __attribute__((always_inline))  { return btns.deactivated & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    byte getBtnLongP(byte nenc=0)       __attribute__((always_inline))  { return btns.longpress & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    byte getBtnShortP(byte nenc=0)      __attribute__((always_inline))  { return btns.shortpress & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    byte getBtnToggle(byte nenc=0)      __attribute__((always_inline))  { return btns.toggled & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    byte getBtnCurrent(byte nenc=0)     __attribute__((always_inline))  { return btns.current & (nenc==0 ? 0xFF:bmasks(nenc-1)); }
    void clearBtns(void)                __attribute__((always_inline))  { memset(&btns, 0, sizeof(t_btnstat)); }


    ///
    /// COUNTERS / MODES
    ///

    /// Read counter value; resets 'change' flag and, if specified, also counter
    int  getEncCount(byte encNo, byte reset=0);
    /// Explicit version for diff count
    int  getEncDiffCount(byte encNo)    { return getEncCount(encNo, 1); };
    /// Clear encoder counts and "changed" flag; if 'all' also reset mode values
    void clearCnts(byte all=0)          __attribute__((always_inline))  { encs.changed = 0; for(byte i=0; i<MAXENC; i++) { encs.ecount[i] = 0; if(all) {encs.emode[i] = 1;} } }

    /// Return encoder BUTTONS which had a transition (of the corresponding type) since last read
    /// For change detection: if an enc no. != 0 is specified, the returned value is relative to that encoder only (!=0 on change)
    /// Because of inlining, no range check is performed on the encoder index
    byte getCntChange(byte encNo=0)      __attribute__((always_inline))  { return encs.changed & (encNo==0 ? 0xFF:(byte)(wmasks[encNo-1])); }

    /// Define number of modes for an encoder.
    /// 0 (default) means modes are not used
    /// 1 means Push&Hold mode (mode=1 normally, mode=2 during push)
    /// 2..127 means that mode cycles between 1..2 to 1..127 on short press
    /// If bit 7 is set (values 130..255), cycle happens on long press
    void setNModes(byte encNo, byte nmodes);

    /// Get current mode for an encoder (1..n).
    /// Meaningless if nModes for this encoder is == 0.
    byte getMode(byte encNo);

    /// Set current mode for an encoder.
    /// If nmode is not consistent with the range set with setNMode, call has no effect
    void setMode(byte encNo, byte nmode);

    /// Increment (or decrement) current mode for an encoder.
    /// flags&0x01: 0->inc, 1->decs
    /// flags&0x02: 0->Value wraps around, 1->Value tops at edges
#define INCMODE_DEC   0x01
#define INCMODE_NOWRAP  0x02
    void incMode(byte nenc, byte flags);
};

// Global instance (singleton use, for embedded)
//extern EncBank EB;

#endif

