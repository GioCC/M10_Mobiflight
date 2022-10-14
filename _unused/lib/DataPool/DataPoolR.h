/********************************************************************
*
* DataPoolR.h
* Class DataPool:
* Local repository of data to/from a PC host connector (SimConnect/FSUIPC etc)
* Currently as simple as a byte vector or little more
* Derived from "DataPool" class, slightly stripped (fixed static allocation, no boundary checks etc)
*
* Project:  FSPanels-Gen
* Hardware: -
* Author:   Giorgio CROCI CANDIANI
* Date:     May-2015
*
********************************************************************/

#ifndef DATAPOOLR_H
#define DATAPOOLR_H

#include <Arduino.h>
#include "bitmasks.h"

/// SIZETYPE is used for variables that carry the number of data objects
/// that can be stored (indexes), not for those used to manage internal storage space.
/// Using 'byte' means that the user can store up to 256 bytes plus up to 256 bools.
#define SIZETYPE uint8_t
//#define SIZETYPE uint16_t

/// Default storage space sizes for a DataPool:
// Number of bytes for the byte area
#define STD_DP_SIZE_8 ((uint16_t)128)
// Number of bits for the flag area
#define STD_DP_SIZE_1 ((uint16_t)128)

class DataPool
{
private:

    byte        dp_data8[(STD_DP_SIZE_8 +((STD_DP_SIZE_8+7)>>3) + 2*((STD_DP_SIZE_1+7)>>3))];
    byte        *dp_data1;    //  Bit data area

    SIZETYPE    dp_max8;       //  Size (in bytes)-1 of byte data area
    SIZETYPE    dp_max1;       //  Size (in bytes)-1 of bit data area

    byte        *dp_change8;   //  Change flags array for byte data
    byte        dp_chgmax8;    //  Size (in bytes)-1 of change flags array for byte data

    byte        *dp_change1;   //  Change flags array for bool data
    byte        dp_chgmax1;    //  Size (in bytes)-1 of change flags array for bool data

public:

    /// The <anychanged> flag signals that any of the data has changed since last read,
    /// i.e. at least one of the individual change flag is set.
    /// The global <anychanged> flag is only meant as a quick convenience warning and
    /// must be reset by the user.
    /// It can be refreshed with <chgcheck()>.
    /// Resetting all the individual data change flags by reading does NOT affect
    /// the global flag.

    bool      anychanged;

    DataPool();

    // Indexing operator (read only): idx = 0..dp_max8
    byte        operator[](SIZETYPE idx)   { return ((dp_data8 != NULL && idx <= dp_max8) ? dp_data8[idx] : 0); }

    /// Get size of storage areas
    SIZETYPE    getSizeB()      { return dp_max8+1; }
    SIZETYPE    getSizeF()      { return ((dp_max1+1) << 3); }

    /// adr  is 0...bytesize-1;
    /// badr is 0...bitsize-1

    /// Each byte in the byte area has 1 associated change flag.
    /// Each bit in the bit area has 1 associated change flag.
    ///
    /// 'put' and 'or'/'and'/'not' functions affect 'change' flags.
    /// 'get' functions only read the value, but do not affect 'change' flags;
    /// these must be reset by the user (if required).
    /// 'fetch' functions read the value and reset both the value and the 'change' flags.

    /// Writing methods have two calling modes:
    /// setChg=0 [default] - if the value is equal to the existing one, no change flag is set;
    /// setChg=1 - the change flag is set regardless of the value (to mark that an update has occurred).
    /// The first mode is used if the detection of new values is required; the second mode is used to
    /// signal that new data has been written, regardless of the value.
    ///
    /// Reading methods have also two calling modes:
    /// rstChg=1 - when reading, the change flag is reset.
    /// rstChg=0 - the change flag is unaffected by read.
    /// Usually the first mode is used, but the second mode may be used if a "peek" at the value
    /// is required before validating the read.

    /// Access to data
    /// =======================

    // by Byte  (in BYTE area)
    byte        getB(SIZETYPE adr, byte rstChg=1)    { if(rstChg) clrBchg(adr); return (adr <= dp_max8 ? dp_data8[adr] : 0); }
    byte        setB(SIZETYPE adr, byte v, byte setChg=0);

    // Byte array  (in BYTE area)
    void        getB(SIZETYPE adr, byte *dst, SIZETYPE len, byte rstChg=1);     // read a byte array into a buffer
    byte        setB(SIZETYPE adr, byte *src, SIZETYPE len, byte setChg=0);     // write a byte array string, record change (on single bytes)

    // by Word  (in BYTE area)
    uint16_t    getW(SIZETYPE adr, byte rstChg=1)    { if(rstChg) clrWchg(adr); return ((uint16_t)(dp_data8[adr])<<8)|dp_data8[adr+1]; }
    byte        setW(SIZETYPE adr, uint16_t v, byte setChg=0);         // write a 2-byte value, record change if any

    // by Long  (in BYTE area)
    uint32_t    getL(SIZETYPE adr, byte rstChg=1)    { if(rstChg) clrLchg(adr); return ((uint32_t)(getW(adr))<<16)|getW(adr+2); }
    byte        setL(SIZETYPE adr, uint32_t v, byte setChg=0);         // write a 4-byte value, record change if any

    // by String  (in BYTE area)
    // Same as functions for byte arrays, except:
    // - a NUL terminator is added at the end
    // - if any char is changed, a change mark is set also on the first char (not just on the changed chars)
    void        getS(SIZETYPE adr, byte *str, SIZETYPE maxlen, byte rstChg=1);     // read a null-terminated string into a buffer
    byte        setS(SIZETYPE adr, byte *str, SIZETYPE maxlen, byte setChg=0);     // write a null-terminated string, record change if any

    // by Flag (=bit) (in BIT area)
    bool        getF(SIZETYPE badr, byte rstChg=1)   { if(rstChg) clrFchg(badr); return ((dp_data1[badr>>3] & (bmasks(badr & 0x07)))!=0); }
    byte        set0F(SIZETYPE badr, byte setChg=0)  { return setF(badr, 0, setChg); }
    byte        set1F(SIZETYPE badr, byte setChg=0)  { return setF(badr, 1, setChg); }
    byte        setF(SIZETYPE badr, byte val, byte setChg=0);

    // Flag bank access (bytewise)
    // These functions address the BIT data area, but the address specified is the BYTE index
    // (e.g. index #2 returns bits #16..#23)
    // If the bit# is known, just >>3 to return the call argument

    byte        getFB(SIZETYPE adr, byte rstChg=0)   { if(rstChg) clrFBchg(adr); return (adr <= dp_max1 ? dp_data1[adr] : 0); }
    byte        setFB(SIZETYPE adr, byte v, byte setChg=0);
    byte        fetchFB(SIZETYPE adr, byte rstval = 0);     // reads data, then clears both data and change flag

    // Dirty and dangerous shortcut to access internal storage directly -
    // use to READ only, and only for debug purposes!
    byte  *getBadr(SIZETYPE adr)        { if(adr <= dp_max8) return &(dp_data8[adr]); }


    /// Access to 'change' flags
    /// ======================

    void        clrBch_all()            { memset(dp_change8, 0, (dp_max8>>3)); }

    // Byte change flags
    bool        getBchg(SIZETYPE adr)    { return ((dp_change8[(adr >> 3)] & bmasks(adr & 0x07)) != 0); }
    void        setBchg(SIZETYPE adr)    { dp_change8[(adr >> 3)] |= bmasks(adr & 0x07); }
    void        clrBchg(SIZETYPE adr)    { dp_change8[(adr >> 3)] &= ~bmasks(adr & 0x07); }

    bool        getBchg(SIZETYPE adr, SIZETYPE len);
    void        setBchg(SIZETYPE adr, SIZETYPE len);
    void        clrBchg(SIZETYPE adr, SIZETYPE len);

    // Word change flags
    bool        getWchg(SIZETYPE adr)    { return (getBchg(adr) || getBchg(adr+1)); }
    void        setWchg(SIZETYPE adr)    { setBchg(adr); setBchg(adr+1); }
    void        clrWchg(SIZETYPE adr)    { clrBchg(adr); clrBchg(adr+1); }
    // void chg_rst16_all(); (use chg_rst8_all())

    // LWord change flags
    bool        getLchg(SIZETYPE adr)    { return (getWchg(adr) || getWchg(adr+1)); }
    void        setLchg(SIZETYPE adr)    { setWchg(adr); setWchg(adr+2); }
    void        clrLchg(SIZETYPE adr)    { clrWchg(adr); clrWchg(adr+2); }

    // String change flags (IT'S ON THE FIRST BYTE)
    bool        getSchg(SIZETYPE adr, byte *str) { return getBchg(adr); }
    void        setSchg(SIZETYPE adr, byte *str) { setBchg(adr); }

    // Flag (=bit) change flags
    bool        getFchg(SIZETYPE badr)   { return ((dp_change1[badr>>3] & bmasks(badr & 0x07)) != 0); }
    void        setFchg(SIZETYPE badr)   { dp_change1[badr>>3] |= bmasks(badr & 0x07); }
    void        clrFchg(SIZETYPE badr)   { dp_change1[badr>>3] &= ~bmasks(badr & 0x07); }
    void        clrFch_all()            { memset(dp_change1, 0, (dp_max1>>3)); }

    // Flag (=bit) bank change flags
    bool        getFBchg(SIZETYPE adr)   { return dp_change1[adr]; }
    void        setFBchg(SIZETYPE adr)   { dp_change1[adr] = 0xFF; }
    void        clrFBchg(SIZETYPE adr)   { dp_change1[adr] = 0x00; }

    bool        chgcheck();

    // Dirty and dangerous shortcut to access internal storage directly
    // use to READ only, and only for debug purposes!
    byte *getFBadr(SIZETYPE adr)        { if(adr <= dp_max1) return &(dp_data1[adr]); }

};

#endif // DATAPOOL_H

