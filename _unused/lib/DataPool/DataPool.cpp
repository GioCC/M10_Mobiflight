/********************************************************************
*
* DataPool.cpp
* Local repository of data to/from a PC host connector (SimConnect/FSUIPC etc)
*
* Project:  FSPanels-Gen
* Hardware: -
* Author:   Giorgio CROCI CANDIANI
* Date:     May-2015
*
********************************************************************/

#include "DataPool.h"

#define round8(s) ((((uint16_t)(s)+1)+7)>>3)-1

#ifndef DP_FIXEDSIZE
DataPool::DataPool(SIZETYPE bytesize, SIZETYPE bitsize)
{
    dp_max8 = bytesize;
    dp_chgmax8 = round8(bytesize);  // rounds up to next boundary
    dp_max1 = round8(bitsize);      // rounds up to next boundary
    dp_chgmax1 = dp_max1;

    //dp_data8 = (byte *)malloc(dp_max8+dp_max1+dp_chgmax8+dp_chgmax1+4);
    dp_data8 = new byte[dp_max8+dp_max1+dp_chgmax8+dp_chgmax1+4];
    dp_data1 = dp_data8 + dp_max8 + 1;

    dp_change8 = dp_data1 + dp_max1 + 1;
    dp_change1 = dp_change8 + dp_chgmax8 + 1;
}

DataPool::DataPool()
{
    DataPool(STD_DP_SIZE_8-1, STD_DP_SIZE_1-1)
}
#else
DataPool::DataPool()
{
    dp_max8 = STD_DP_SIZE_8-1;
    dp_max1 = round8(STD_DP_SIZE_1-1);
    dp_chgmax8 = round8(STD_DP_SIZE_8-1);
    dp_chgmax1 = dp_max1;

    //dp_data8 already assigned
    dp_data1 = dp_data8 + dp_max8 + 1;
    dp_change8 = dp_data1 + dp_max1 + 1;
    dp_change1 = dp_change8 + dp_chgmax8 + 1;
}

DataPool::DataPool(SIZETYPE bytesize, SIZETYPE bitsize)
{
    DataPool();
}
#endif // DP_FIXEDSIZE

DataPool::~DataPool()
{
#ifndef DP_FIXEDSIZE
    delete[] dp_data8; //free(dp_data8);
#endif
}

/*
byte& DataPool::operator[](SIZETYPE idx)
  {
    //return dp_data[idx];
    return (idx <= dp_max ? dp_data[idx] : dp_dummy);
  }
const
byte& DataPool::operator[](SIZETYPE idx)
const
  {
    //return dp_data[idx];
    return (idx <= dp_max ? dp_data[idx] : dp_dummy);
  }
*/

/// Return 1-byte data from byte area
byte
DataPool::get8(SIZETYPE adr)
{
    return (adr <= dp_max8 ? dp_data8[adr] : 0);
}

/// Write 1-byte data into byte area
void
DataPool::put8(SIZETYPE adr, byte v)
{
#ifdef STRICTCHECKS
    if(adr > dp_max8) {
        return;
    }
#endif
    if(v != dp_data8[adr]) {
        dp_data8[adr] = v;
        dp_change8[(adr >> 3)] |= bmasks(adr & 0x07); //(1 << (adr & 0x07));
        anychanged = true;
    }
}

/// Return 2-byte data from byte area
uint16_t
DataPool::get16(SIZETYPE adr)
{
    uint16_t vv;
#ifdef STRICTCHECKS
    if(adr > dp_max8-1) {
        return;
    }
#endif
    vv = dp_data8[adr];
    vv = (vv<<8)|dp_data8[adr+1];
    return vv;
}

/// Write 2-byte data into byte area
/// If any bit has changed, both bytes are marked as changed.
void
DataPool::put16(SIZETYPE adr, uint16_t vv)
{
    uint8_t vh, vl;
#ifdef STRICTCHECKS
    if(adr > dp_max8-1) {
        return;
    }
#endif
    vh = (vv >> 8)&0xFF;
    vl = vv&0xFF;
    if(vh != dp_data8[adr] || vh != dp_data8[adr+1]) {
        dp_data8[adr] = vh;
        dp_data8[adr+1] = vl;
        // Mark both bytes as changed!
        dp_change8[adr >> 3] |= bmasks(adr & 0x07); //(1 << (adr & 0x07));
        adr++;
        dp_change8[adr >> 3] |= bmasks(adr & 0x07); //(1 << (adr & 0x07));
        anychanged = true;
    }
}

/// Write null-terminated string (including final NULL)
/// If any character has changed, also the first character
/// is marked as changed.
void
DataPool::put8x(SIZETYPE adr, byte *str, SIZETYPE maxlen)
{
    // WARNING: string is just written into storage space.
    // It is up to the user to monitor the length of the string:
    // possible overflow into space used by other variables is not
    // detected (nor detectable!)
    // Overflow out of the storage area is prevented (string is truncated).
    SIZETYPE cc;
#ifdef STRICTCHECKS
    if(adr > dp_max8 || maxlen == 0) {
        return;
    }
#endif
    cc = adr;
    do {
        if(*str != dp_data8[cc]) {
            dp_data8[cc] = *str;
            dp_change8[(cc >> 3)] |= bmasks(cc & 0x07); //(1 << (cc & 0x07));
            anychanged = true;
        }
        cc++;
    } while (*(str++) != 0 && cc < (adr+maxlen) && cc <= dp_max8 && cc != 0);

    // If any character has changed, the first character is also marked as changed.
    if(anychanged) {
        dp_change8[(adr >> 3)] |= bmasks(cc & 0x07); //(1 << (adr & 0x07));
    }
}

/// Read null-terminated string (including final NULL)
/// into supplied buffer. Read up to <maxlen> chars ('0' included).
/// Warning: it is up to the user to supply a large enough buffer
void
DataPool::get8x(SIZETYPE adr, byte *str, SIZETYPE maxlen)
{
    SIZETYPE ll = 0;
#ifdef STRICTCHECKS
    if(adr > dp_max8 || maxlen == 0) {
        return;
    }
#endif
    do {
        *str = dp_data8[adr++];
        ll++;
    } while (*(str++) != 0 && ll < maxlen && adr < dp_max8 && adr != 0);
}


/// Return 1-bit data from bit area
bool
DataPool::get1(SIZETYPE badr)
{
    register byte badrB = badr>>3;
    register byte msk = bmasks(adr & 0x07); //(1<<(badr&0x07));
#ifdef STRICTCHECKS
    if(badrB > dp_max1) {
        return 0;
    }
#endif
    return ((dp_data1[badrB] & msk)!=0);
}

/// Set bit in bit area
void
DataPool::set1(SIZETYPE badr)
{
    register byte badrB = badr>>3;
    register byte msk = bmasks(badr & 0x07); //(1<<(badr&0x07));
    register byte newv;
    register byte oldv;
#ifdef STRICTCHECKS
    if(badrB > dp_max1) {
        return;
    }
#endif
    oldv = dp_data1[badrB];
    newv = (oldv | msk);
    if(newv != oldv) {
        dp_data1[badrB] = newv;
        dp_change1[badrB] |= msk;
        anychanged = true;
    }
}

/// Clear bit in bit area
void
DataPool::clr1(SIZETYPE badr)
{
    register byte badrB = badr>>3;
    register byte msk = bmasks(badr & 0x07); //(1<<(badr&0x07));
    register byte newv;
    register byte oldv;
#ifdef STRICTCHECKS
    if(badrB > dp_max1) {
        return;
    }
#endif
    oldv = dp_data1[badrB];
    newv = (oldv & ~msk);
    if(newv != oldv) {
        dp_data1[badrB] = newv;
        dp_change1[badrB] |= msk;
        anychanged = true;
    }
}

bool
DataPool::chg_get8(SIZETYPE adr)
{
#ifdef STRICTCHECKS
    if(adr > dp_max8) {
        return;
    }
#endif
    return ((dp_change8[(adr >> 3)] & bmasks(adr & 0x07) /*(1 << (adr & 0x07))*/) != 0);
}

void
DataPool::chg_set8(SIZETYPE adr)
{
#ifdef STRICTCHECKS
    if(adr > dp_max8) {
        return;
    }
#endif
    dp_change8[(adr >> 3)] |= bmasks(adr & 0x07); //(1 << (adr & 0x07));
}

void
DataPool::chg_rst8(SIZETYPE adr)
{
#ifdef STRICTCHECKS
    if(adr > dp_max8) {
        return;
    }
#endif
    dp_change8[(adr >> 3)] &= ~bmasks(adr & 0x07); //(1 << (adr & 0x07));
}

void
DataPool::chg_rst8_all()
{
    memset(dp_change8, 0, (dp_max8>>3));
}

bool
DataPool::chg_get16(SIZETYPE adr)
{
#ifdef STRICTCHECKS
    if(adr > dp_max8-1) {
        return;
    }
#endif
    // check on first byte is enough
    return ((dp_change8[(adr >> 3)] & bmasks(adr & 0x07) /*(1 << (adr & 0x07))*/) != 0);
}

void
DataPool::chg_set16(SIZETYPE adr)
{
#ifdef STRICTCHECKS
    if(adr > dp_max8-1) {
        return;
    }
#endif
    dp_change8[(adr >> 3)] |= bmasks(adr & 0x07); //(1 << (adr & 0x07));
    adr++;
    dp_change8[(adr >> 3)] |= bmasks(adr & 0x07); //(1 << (adr & 0x07));
}

void
DataPool::chg_rst16(SIZETYPE adr)
{
#ifdef STRICTCHECKS
    if(adr > dp_max8-1) {
        return;
    }
#endif
    dp_change8[(adr >> 3)] &= ~bmasks(adr & 0x07); //(1 << (adr & 0x07));
    adr++;
    dp_change8[(adr >> 3)] &= ~bmasks(adr & 0x07); //(1 << (adr & 0x07));
}

bool
DataPool::chg_get1(SIZETYPE badr)
{
    register byte badr8 = badr>>3;
    register byte msk = bmasks(badr & 0x07); //(1<<(badr&0x07));
#ifdef STRICTCHECKS
    if(badr8 > dp_max1) {
        return false;
    }
#endif
    return ((dp_change1[badr8] & msk) != 0);
}

void
DataPool::chg_set1(SIZETYPE badr)
{
    register byte badr8 = badr>>3;
    register byte msk = bmasks(badr & 0x07); //(1<<(badr&0x07));
#ifdef STRICTCHECKS
    if(badr8 > dp_max1) {
        return;
    }
#endif
    dp_change1[badr8] |= msk;
}

void
DataPool::chg_rst1(SIZETYPE badr)
{
    register byte badr8 = badr>>3;
    register byte msk = (1<<(badr&0x07));
#ifdef STRICTCHECKS
    if(badr8 > dp_max1) {
        return;
    }
#endif
    dp_change1[badr8] &= ~msk;
}

void
DataPool::chg_rst1_all()
{
    memset(dp_change1, 0, (dp_max1>>3));
}

byte
DataPool::getbank1(SIZETYPE adr)
{
    return (adr <= dp_max1 ? dp_data1[adr] : 0);
}

byte
DataPool::fetchbank1(SIZETYPE adr)
{
    byte v;
#ifdef STRICTCHECKS
    if(adr > dp_max1) {
        return 0;
    }
#endif
    v = dp_data1[adr];
    dp_data1[adr] = 0;
    dp_change1[adr] = 0;
    return v;
}

void
DataPool::putbank1(SIZETYPE adr, byte v)
{
#ifdef STRICTCHECKS
    if(adr > dp_max1) {
        return;
    }
#endif
    if(v != dp_data1[adr]) {
        dp_change1[adr] |= (dp_data1[adr]^v);
        dp_data1[adr] = v;
        anychanged = true;
    }
}

void
DataPool::orbank1(SIZETYPE adr, byte v)
{
#ifdef STRICTCHECKS
    if(adr > dp_max1) {
        return;
    }
#endif
    if((v|dp_data1[adr]) != dp_data1[adr]) {
        dp_change1[adr] = (v^dp_data1[adr])&v;
        dp_data1[adr] |= v;
        anychanged = true;
    }
}

void
DataPool::andbank1(SIZETYPE adr, byte v)
{
#ifdef STRICTCHECKS
    if(adr > dp_max1) {
        return;
    }
#endif
    if((v&dp_data1[adr]) != dp_data1[adr]) {
        dp_change1[adr] = v^dp_data1[adr];
        dp_data1[adr] &= v;
        anychanged = true;
    }
}

void
DataPool::notbank1(SIZETYPE adr)
{
#ifdef STRICTCHECKS
    if(adr > dp_max1) {
        return;
    }
#endif
    dp_change1[adr] = 0xFF;
    dp_data1[adr] = ~dp_data1[adr];
    anychanged = true;
}

bool
DataPool::chg_getbank1(SIZETYPE adr)
{
#ifdef STRICTCHECKS
    if(adr > dp_max1) {
        return;
    }
#endif
    return (dp_change1[adr] != 0);
}

void
DataPool::chg_setbank1(SIZETYPE adr)
{
#ifdef STRICTCHECKS
    if(adr > dp_max1) {
        return;
    }
#endif
    dp_change1[adr] = 0xFF;
}

void
DataPool::chg_rstbank1(SIZETYPE adr)
{
#ifdef STRICTCHECKS
    if(adr > dp_max1) {
        return;
    }
#endif
    dp_change1[adr] = 0x00;
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

// END DataPool.cpp
