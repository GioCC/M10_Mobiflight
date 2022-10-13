#ifndef VARSTORE_H
#define VARSTORE_H
#include <Arduino.h>

template<typename Tadr, Tadr bytesize>
class varStore
{
//typedef typeof(bytesize) Tadr;

public:
        varStore() {}

        uint8_t     *base(void)                     { return _store; }
        uint8_t     *item(Tadr adr)                 { return &(_store[adr]); }

        uint8_t     setUB(Tadr adr, uint8_t val)    { return _store[adr] = val; }
        uint8_t     getUB(Tadr adr)                 { return _store[adr]; }

        uint16_t    setUW(Tadr adr, uint16_t val)   { return (*(uint16_t *)(_store[adr]) = val); }
        uint16_t    getUW(Tadr adr)                 { return *((uint16_t *)(_store[adr])); }

        uint32_t    setUL(Tadr adr, uint32_t val)   { return (*(uint32_t *)(_store[adr]) = val); }
        uint32_t    getUL(Tadr adr)                 { return *((uint32_t *)(_store[adr])); }

        int8_t      setB(Tadr adr, int8_t val)      { return _store[adr] = val; }
        int8_t      getB(Tadr adr)                  { return _store[adr]; }

        void        setB(Tadr adr, byte *src, Tadr maxlen);             // Write byte array
        void        getB(Tadr adr, byte *dst, Tadr maxlen);             // Read byte array
        void        setS(Tadr adr, char *src, Tadr maxlen=bytesize);    // Write String (NUL-terminated)
        void        getS(Tadr adr, char *dst, Tadr maxlen=bytesize);    // Read String (NUL-terminated)

        int16_t     setW(Tadr adr, int16_t val)     { return (*(int16_t *)(_store[adr]) = val); }
        int16_t     getW(Tadr adr)                  { return *((int16_t *)(_store[adr])); }

        int32_t     setL(Tadr adr, int32_t val)     { return (*(int32_t *)(_store[adr]) = val); }
        int32_t     getL(Tadr adr)                  { return *((int32_t *)(_store[adr])); }

protected:

        uint8_t _store[bytesize];
};

#endif // VARSTORE_H

