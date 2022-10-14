/********************************************************************
*
* DataPoolR.cpp
* Local repository of data to/from a PC host connector (SimConnect/FSUIPC etc)
* Derived from "DataPool" class, slightly stripped (fixed static allocation, no boundary checks etc)
*
* Project:  FSPanels-Gen
* Hardware: -
* Author:   Giorgio CROCI CANDIANI
* Date:     May-2015
*
********************************************************************/

#include "DataPoolR.h"

#define round8(s) ((((uint16_t)(s)+1)+7)>>3)-1

DataPool::DataPool()
{
    dp_max8     = STD_DP_SIZE_8-1;
    dp_max1     = round8(STD_DP_SIZE_1-1);
    dp_chgmax8  = round8(STD_DP_SIZE_8-1);
    dp_chgmax1  = dp_max1;

    //dp_data8 already assigned
    dp_data1    = dp_data8 + dp_max8 + 1;
    dp_change8  = dp_data1 + dp_max1 + 1;
    dp_change1  = dp_change8 + dp_chgmax8 + 1;
}

/// Write 1-byte data into byte area
byte
DataPool::setB(SIZETYPE adr, byte v, byte setChg)
{
    if(setChg || (v != dp_data8[adr])) {
        dp_data8[adr] = v;
        setBchg(adr);
        return anychanged = true;
    }
    return false;
}

/// Read n-byte data array into byte area
void
DataPool::getB(SIZETYPE adr, byte *dst, SIZETYPE maxlen, byte rstChg)
{
    SIZETYPE ll = 0;
    do {
        dst[ll++] = dp_data8[adr++];
    } while ((ll < maxlen) && (adr < dp_max8) && (adr != 0));

}
/// Write n-byte data array into byte area
byte
DataPool::setB(SIZETYPE adr, byte *src, SIZETYPE maxlen, byte setChg)
{
    // WARNING: the array is just written into storage space.
    // It is up to the user to monitor the length of the array:
    // possible overflow into space used by other variables is not
    // detected (nor detectable!)
    // Overflow out of the storage area is prevented (array is truncated).
    SIZETYPE cc;
    byte chg = 0;
    cc = 0;
    do {
        chg |= setB(adr++, *(src++), setChg);
        cc++;
    } while ((*src != 0) && (cc < maxlen) && (adr < dp_max8) && (adr != 0));

    // If any character has changed, the first character is also marked as changed.
    if(setChg || chg) {
        setBchg(cc);
        dp_change8[(adr >> 3)] |= bmasks(cc & 0x07); //(1 << (adr & 0x07));
        return anychanged = true;
    }
    return false;
}

/// Write 2-byte data into byte area
byte
DataPool::setW(SIZETYPE adr, uint16_t vv, byte setChg)
{
    uint8_t vl;

    byte chg = 0;
    vl = (uint8_t)((vv >> 8)&0xFF);
    chg |= setB(adr, vl, setChg);
    vl = (uint8_t)(vv&0xFF);
    chg |= setB(adr+1, vl, setChg);
    return chg;
}

/// Write 4-byte data into byte area
byte
DataPool::setL(SIZETYPE adr, uint32_t vv, byte setChg)
{
    byte chg = 0;
    uint8_t vl;
    vl = (uint8_t)(vv&0xFF);
    chg |= setB(adr+3, vl, setChg);
    vl = (uint8_t)((vv>>=8)&0xFF);
    chg |= setB(adr+2, vl, setChg);
    vl = (uint8_t)((vv>>=8)&0xFF);
    chg |= setB(adr+1, vl, setChg);
    vl = (uint8_t)((vv>>=8)&0xFF);
    chg |= setB(adr, vl, setChg);
    return chg;
}

/// Write null-terminated string (including final NULL)
/// If any character has changed, also the first character
/// is marked as changed.
byte
DataPool::setS(SIZETYPE adr, byte *str, SIZETYPE maxlen, byte setChg)
{
    // WARNING: string is just written into storage space.
    // It is up to the user to monitor the length of the string:
    // possible overflow into space used by other variables is not
    // detected (nor detectable!)
    // Overflow out of the storage area is prevented (string is truncated).
    byte chg;
    chg = setB(adr, str, maxlen, setChg);
    if(adr+maxlen < (dp_max8-1)){
        // if NUL wasn't in the same position, length has changed -> set chg flag
        chg |= (dp_data8[adr+maxlen] == 0);
        dp_data8[adr+maxlen] = 0;   // Terminate string
    }
    // for strings, also mark first byte
    if(chg) setBchg(adr);
    return (chg ? (anychanged = true) : false);
}

/// Read null-terminated string (including final NULL)
/// into supplied buffer. Read up to <maxlen> chars ('0' included).
/// Warning: it is up to the user to supply a large enough buffer
void
DataPool::getS(SIZETYPE adr, byte *str, SIZETYPE maxlen, byte rstChg)
{
    SIZETYPE ll = 0;
    do {
        *str = dp_data8[adr++];
        ll++;
    } while ((*(str++)!=0) && (ll < maxlen) && (adr < dp_max8) && (adr != 0));
}


/// Set bit in bit area
byte
DataPool::setF(SIZETYPE badr, byte val, byte setChg)
{
    register byte badrB = badr>>3;
    register byte msk = bmasks(badr & 0x07); //(1<<(badr&0x07));
    register byte newv;
    register byte oldv;
    oldv = dp_data1[badrB];
    newv = (val ? (oldv | msk) : (oldv & ~msk));
    if(setChg || (newv != oldv)) {
        dp_data1[badrB] = newv;
        dp_change1[badrB] |= msk;
        return anychanged = true;
    }
    return false;
}

byte
DataPool::fetchFB(SIZETYPE adr, byte rstval)
{
    byte v = dp_data1[adr];
    dp_data1[adr] = rstval;
    dp_change1[adr] = 0;
    return v;
}

byte
DataPool::setFB(SIZETYPE adr, byte v, byte setChg)
{
    if(setChg || (v != dp_data1[adr])) {
        dp_change1[adr] |= (dp_data1[adr]^v);
        dp_data1[adr] = v;
        return anychanged = true;
    }
    return false;
}

bool
DataPool::getBchg(SIZETYPE adr, SIZETYPE len)
{
    for(SIZETYPE i = adr; (i<adr+len) && (i<dp_max8); i++) {
        if(getBchg(i)) return true;
    }
    return false;
}

void
DataPool::setBchg(SIZETYPE adr, SIZETYPE len)
{
    for(SIZETYPE i = adr; (i<adr+len) && (i<dp_max8); i++) {
        setBchg(i);
    }
}

void
DataPool::clrBchg(SIZETYPE adr, SIZETYPE len)
{
    for(SIZETYPE i = adr; (i<adr+len) && (i<dp_max8); i++) {
        setBchg(i);
    }
}

bool
DataPool::chgcheck()
{
    byte i;
    byte c;
    for(i = 0, c = 0; i < (dp_chgmax8+dp_chgmax1+2); i++) {
        if((c |= dp_change8[i]) != 0) {
            break;
        }
    }
    return (anychanged = (c != 0));
}

// END DataPoolR.cpp
