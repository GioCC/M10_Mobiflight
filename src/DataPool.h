/********************************************************************
*
* DataPool.h
* Class DataPool:
* Local repository of data to/from a PC host connector (SimConnect/FSUIPC etc)
* Currently as simple as a byte vector or little more
*
* Project:  FSPanels-Gen
* Hardware: -
* Author:   Giorgio CROCI CANDIANI
* Date:     May-2015
*
********************************************************************/

#ifndef DATAPOOL_H
#define DATAPOOL_H

#include <Arduino.h>

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

/// USE THIS DEFINE TO AVOID malloc() USAGE
/// Allocates fixed default-sized, non-dynamic array
#define DP_FIXEDSIZE

class DataPool
{
private:
    // sizes here are all actual storage sizes, not user-requested sizes
#ifndef DP_FIXEDSIZE
    byte      *dp_data8;     //  Byte data area
#else
    byte      dp_data8[(STD_DP_SIZE_8 +((STD_DP_SIZE_8+7)>>3) + 2*((STD_DP_SIZE_1+7)>>3))];
#endif
    byte      *dp_data1;    //  Bit data area

    SIZETYPE  dp_max8;       //  Size (in bytes)-1 of byte data area
    SIZETYPE  dp_max1;       //  Size (in bytes)-1 of bit data area

    byte      *dp_change8;   //  Change flags array for byte data
    byte      dp_chgmax8;    //  Size (in bytes)-1 of change flags array for byte data

    byte      *dp_change1;   //  Change flags array for bool data
    byte      dp_chgmax1;    //  Size (in bytes)-1 of change flags array for bool data

public:

    /// The <anychanged> flag signals that any of the data has changed since last read,
    /// i.e. at least one of the individual change flag is set.
    /// The global <anychanged> flag is only meant as a quick convenience warning and
    /// must be reset by the user.
    /// It can be refreshed with <chgcheck()>.
    /// Resetting all the individual data change flags by reading does NOT affect
    /// the global flag.

    bool      anychanged;

    /// Sizes passed to the constructor are all storage area sizes,
    /// i.e. the number of data objects of the corresponding type that can be stored.
    /// WARNING: in order to fit in used var types, the size to be specified
    /// is (actual desired size)-1!
    DataPool();     // uses default sizes
    DataPool(SIZETYPE bytesize, SIZETYPE bitsize);      // args are fake if DP_FIXEDSIZE is defined

    ~DataPool();

    /*
          byte& operator[](SIZETYPE idx);   // Read+Write
          const byte& operator[](const SIZETYPE idx);
    */
    byte operator[](SIZETYPE idx)     // Read only; idx = 0..dp_max8
    {
        return ((dp_data8 != NULL && idx <= dp_max8) ? dp_data8[idx] : 0);
    }

    /// Get size of storage areas
    SIZETYPE getSpace8() { return dp_max8+1; }
    SIZETYPE getSpace1()
    {
        return ((dp_max1+1) << 3);
    }

    /// adr  is 0...bytesize-1;
    /// badr is 0...bitsize-1

    /// Each byte in the byte area has 1 associated change flag.
    /// Each bit in the bit area has 1 associated change flag.
    ///
    /// 'put' and 'or'/'and'/'not' functions affect 'change' flags.
    /// 'get' functions only read the value, but do not affect 'change' flags;
    /// these must be reset by the user (if required).
    /// 'fetch' functions read the value and reset both the value and the 'change' flags.

    /// Functions for BYTE data
    /// =======================

    // Actual data - 1 byte
    byte get8(SIZETYPE adr);
    void put8(SIZETYPE adr, byte v);                        // write a byte, record change if any
    void put8x(SIZETYPE adr, byte *str, SIZETYPE maxlen);   // write a null-terminated string, record change if any
    void get8x(SIZETYPE adr, byte *str, SIZETYPE maxlen);   // read a null-terminated string into a buffer

    // Actual data - 2 bytes
    uint16_t    get16(SIZETYPE adr);
    void        put16(SIZETYPE adr, uint16_t v);            // write a 2-byte value, record change if any

    // Change flags
    bool chg_get8(SIZETYPE adr);
    void chg_set8(SIZETYPE adr);
    void chg_rst8(SIZETYPE adr);
    void chg_rst8_all();
    // Change flags for 2-byte wide data - affect chg flags of both bytes
    bool chg_get16(SIZETYPE adr);
    void chg_set16(SIZETYPE adr);
    void chg_rst16(SIZETYPE adr);
    // void chg_rst16_all(); (use chg_rst8_all())


    // Dirty and dangerous shortcut to access internal storage directly -
    // use for READ only, and only for debug purposes!
    byte *get8adr(SIZETYPE adr)
    {
        if(adr <= dp_max8) {
            return &(dp_data8[adr]);
        }
    }

    /// Functions for BIT data
    /// ======================

    // Actual data
    bool get1(SIZETYPE badr);
    void set1(SIZETYPE badr);  // write a '1' bit, record change if any
    void clr1(SIZETYPE badr);  // write a '0' bit, record change if any
    void put1(SIZETYPE badr, byte val)
    {
        if(val) {
            set1(badr);
        } else {
            clr1(badr);
        }
    };

    // Change flags
    bool chg_get1(SIZETYPE badr);
    void chg_set1(SIZETYPE badr);
    void chg_rst1(SIZETYPE badr);
    void chg_rst1_all();

    /// Functions for BIT data, byte- (bank-)wise

    /// These functions address the BIT data area, but the address
    /// specified is the BYTE index (e.g. index #2 returns bits #16..#23)
    /// If the bit# is known, just >>3 to return the call argument

    // Actual data
    byte getbank1(SIZETYPE adr);            // just reads data
    byte fetchbank1(SIZETYPE adr);          // reads data, then clears both data and change flag
    void putbank1(SIZETYPE adr, byte v);
    void orbank1(SIZETYPE adr, byte v);     // ORs supplied value with data
    void andbank1(SIZETYPE adr, byte v);    // ANDs supplied value with data
    void notbank1(SIZETYPE adr);            // negates data

    // Change flags
    bool chg_getbank1(SIZETYPE adr);
    void chg_setbank1(SIZETYPE adr);
    void chg_rstbank1(SIZETYPE adr);

    bool chgcheck();

    // Dirty and dangerous shortcut to access internal storage directly
    // use for READ only, and only for debug purposes!
    byte *getbank1adr(SIZETYPE adr)
    {
        if(adr <= dp_max1) {
            return &(dp_data1[adr]);
        }
    }

};

#endif // DATAPOOL_H

