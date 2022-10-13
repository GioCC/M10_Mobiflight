/// Ss_FastArduino library
/// from McMajan Library pack
/// http://www.mcmajan.com/mcmajanwpr/?page_id=1885

#include "FastArduino.h"

void f_FastdigitalWrite(unsigned char pin, unsigned char val)
{
    unsigned char bit = digitalPinToBitMask(pin);
    volatile unsigned char *out;
    out = portOutputRegister(digitalPinToPort(pin));
    if (val != LOW) {
        *out |= (unsigned char)bit;
    } else {
        *out &= ~(unsigned char)bit;
    }
}

int f_FastdigitalRead(unsigned char pin)
{
    unsigned char  bit = digitalPinToBitMask(pin);
    unsigned char  port = digitalPinToPort(pin);
    if (*portInputRegister(port) & bit) {
        return HIGH;
    }
    return LOW;
}


void f_FastshiftOut(const unsigned char dataPin, const unsigned char clockPin, const unsigned char order, const unsigned char val)
{
    register unsigned char i;
    register unsigned char tmp=(order == MSBFIRST ? 0x80 : 0x01);
    unsigned char send;
    for (i = 8; i ; i--) {
        send=((unsigned char)val & tmp);
        f_FastdigitalWrite(dataPin,send);
        f_FastdigitalWrite(clockPin, HIGH);
        f_FastdigitalWrite(clockPin, LOW);
        tmp = (order == MSBFIRST ? tmp>>1 : tmp<<1);
    }

}

void HPulse(unsigned char pin)
{
    f_FastdigitalWrite(pin,HIGH);
    f_FastdigitalWrite(pin,LOW);
}


void f_FastdigitalPulse(unsigned char pin, unsigned char val)
{
    unsigned char bit = digitalPinToBitMask(pin);
    volatile unsigned char *out;
    out = portOutputRegister(digitalPinToPort(pin));
    if (val) {
        *out |= bit;
        *out &= ~bit;
    } else {
        *out &= ~bit;
        *out |= bit;
    }

}
//asm volatile("nop \n\t nop \n\t nop \n\t nop");
//asm volatile("jmp 0"); // restart

