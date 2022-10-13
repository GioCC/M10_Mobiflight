/// conversions.cpp

#include "conversions.h"
/// =====================
///  COM/NAV

void BCDtoASC(uint16_t src, char *dst, uint8_t src_frac)
{
    for(byte i=4; i>0; i--) {
        dst[i-1] = '0'+(src&0x0F); src >>=4;
    }
    if(src_frac != 0xFF) {
        dst[4] = '0'+(src_frac&0x0F);
        dst[5] = 0;
    } else {
        dst[4] = 0;
    }
}

uint16_t BCDtouint(uint16_t src, uint8_t src_frac)
{
    uint16_t dst = 0;
    uint16_t dd;
    //uint16_t mul = 10;
    for(byte i=0; i<4; i++) {
        dd = (src&0x0F);
        for(byte j=0; j<i+1; j++) {dd = times10(dd);}
        dst += dd;
        //dst += (src&0x0F)*mul;
        src >>=4;
        //mul = times10(mul);
    }
    if(src_frac != 0xFF) {
        dst += (src_frac&0x0F);
    }
    return dst;
}

uint16_t ASCtoBCD(char *src, uint8_t *dst_frac)
{
    uint16_t dst = 0;
    for(byte i=0; i<4; i++) {
        dst |= (src[i]-'0'); dst <<=4;
    }
    if(dst_frac) {
        *dst_frac = (src[4]-'0') & 0x0F;
    }
    return dst;
}

uint16_t uinttoBCD(uint16_t src, uint8_t *dst_frac)
{
    // argument value is in (MHz*100)-100000, e.g. 123.475 MHz -> 23475 (int) -> 0x2347/0x05 (BCD)
    uint16_t dst = 0;
    uint16_t mul = 10000;
    //uint16_t r;
    uint16_t a;
    //r = src%mul;
    for(byte i=0; i<3; i++) {
        dst <<= 4;
        a = src/mul;
        dst |= (a&0x0F);
        src -= (a*mul);
        mul /= 10;
    }
    if(dst_frac) {
        *dst_frac = (src&0x0F);
    }
    return dst;
}

void uinttoHex(uint32_t src, char *dst, uint8_t len)
{
    if(len>8) len=8;
    for(byte i=len; i>0; i--) {
        dst[i-1] = '0'+(src&0x0F); src >>=4;
    }
    dst[len] = 0;
}

uint16_t Hextouint16(char *src, uint8_t len)
{
    uint8_t     dgt = 0;
    uint16_t    dst = 0;
    if(len>4) len=4;
    for(byte i=0; i<len; i++) {
        dst <<= 4;
        dgt = src[i]-'0';
        if(dgt > 48) dgt-=38;       // 'a'...'f'
        else if(dgt > 26) dgt-=16;  // 'A'...'F'
        dst |= (dgt&0x0F);
    }
    return dst;
}

/// =====================
///  ADF

void BCD_ADFtoASC5(uint16_t srcmain, uint16_t srcext, char *dst)
{
    dst[4] = '0'+(srcext&0x0F); srcext >>=8;
    dst[0] = '0'+(srcext&0x0F);
    for(byte i=4; i>1; i--) {
        dst[i-1] = '0'+(srcmain&0x0F); srcmain >>=4;
    }
}

uint16_t BCD_ADFtouint(uint16_t srcmain, uint16_t srcext)
{
    // returned value is in kHz*10, e.g. 1234.5 kHz -> 12345
    uint16_t dst = 0;
    uint16_t dd;
    dst += (srcext&0x0F);
    srcext >>=8;
    for(byte i=0; i<3; i++) {
        dd = (srcmain&0x0F);
        for(byte j=0; j<i+1; j++) {dd = times10(dd);}
        dst += dd;
        srcmain >>=4;
    }
    if((srcext&0x0F)!=0) dst += 5;  // Frac can only be in 0.5 kHz steps
    return dst;
}

void uinttoBCD_ADF(uint16_t src, uint16_t *dstmain, uint16_t *dstext)
{
    uint16_t mul = 10000;
    //uint16_t r;
    uint16_t a;
    *dstmain = *dstext = 0;
    //r = src%mul;
    a = src/mul;
    (*dstext) |= (a&0x0F);
    (*dstext)<<=8;
    src -= (a*mul);
    mul /= 10;
    for(byte i=0; i<3; i++) {
        a = src/mul;
        (*dstmain) |= (a&0x0F);
        (*dstmain) <<= 4;
        src -= (a*mul);
        mul /= 10;
    }
    (*dstext) |= (src&0x0F);
}

