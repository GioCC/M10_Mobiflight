#ifndef RAWDATA_H
#define RAWDATA_H

class RawData
{
public:
    union {
        const char      *text;
        const byte      *bytes;
        struct {
            uint16_t hi;
            uint16_t lo;
        } code;
    };

    RawData(void) {};
    RawData(uint16_t h, uint16_t l) {code.hi = h; code.lo = l;}
    explicit RawData(const char *s)          {text = s;}
    explicit RawData(const byte b[])         {bytes = b;}
};

#endif // RAWDATA_H
