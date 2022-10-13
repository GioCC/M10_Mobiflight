#include <Arduino.h>


void BCD4toASC4(uint16_t src, char *dst) {
    for(byte i=4; i>0; i--) {
        dst[i-1] = '0'+(src&0x0F); src >>=4;
    }
}

uint16_t ASC4toBCD4(char *src) {
    uint16_t dst = 0;
    for(byte i=0; i<4; i++) {
        dst |= (src[i]-'0'); dst <<=4;
    }
    return dst;
}

uint16_t BCD4touint(uint16_t src) {
    uint16_t dst = 0;
    uint16_t mul = 1;
    for(byte i=4; i>0; i--) {
        dst += (src&0x0F)*mul;
        src >>=4;
        mul <<= 3; mul++; mul++;    // mul *= 10;
    }
    return dst;
}

uint16_t uinttoBCD4(uint16_t src) {
    uint16_t dst = 0;
    uint16_t mul = 1000;
    uint16_t r;
    uint16_t a;
    // ....
    //r = src%mul;

    a = src/mul;
    dst |= (a&0x0F);
    dst<<=4;
    src-=(a*mul);
    mul /= 10;
    return dst;
}

/// Following is taken (very slightly adapted) from:
/// http://www.dragonwins.com/domains/getteched/de248/bin2bcd.htm
/// YET TO BE TESTED

uint16_t uinttoBCD4_alt(uint16_t src) {
    const uint16_t w[4] = { 1, 10, 100, 1000 };
    uint16_t    dst = 0;
    uint16_t    v;
    uint8_t     b, k;
    for (uint8_t i = 3; i>0; i--) {
        for ( b=0, v=w[i]<<(k=3); k>=0; k--,v>>=1) {
            if (src >= v){
                src -= v;
                b += 1<<k;
            }
        }
        dst |= (b&0x0F);
        dst <<= 4;
    }
    dst |= (src&0x0F);
    return dst;
}
