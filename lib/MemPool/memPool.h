// =======================================================================
// @file        memPool.h
//
// @project     
// 
// @details     
// A simple library to manage dynamic RAM allocation from a statically
// allocated area.
// Targeted mainly for embedded systems, where chunks of memory are usually
// allocated once but never released.
// This gives the flexibility of dynamic allocation without the burden of 
// memory management, garbage collection etc. with the additional advantage
// of a known pre-defined total memory occupation.
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-07-21
// @modifiedby  GiorgioCC - 2023-09-26 18:10
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __MEMPOOL__H__
#define __MEMPOOL__H__

#include <Arduino.h>
#include <stdint.h>

template <uint16_t SIZE_BYTES>
class memPool
{
public:
    using T_crash_handler = void (*)(void);

private:

    static T_crash_handler  crash_handler = nullptr;
    static uint8_t          mem[SIZE_BYTES];
    uint16_t                ptr;

public:

    explicit 
    memPool(T_crash_handler chf = nullptr): ptr(0), crash_handler(chf) {}

    static const uint16_t getCapacity(void)   { return (SIZE_BYTES);}
    const uint16_t        getAvailable(void)  { return (SIZE_BYTES-ptr);}
    const bool  isAvailable(uint16_t size)    { return size <= getAvailable();}
    void        wipe(void)                    { ptr = 0; };
    
    void* reserve(uint16_t size) 
        {
            if(!isAvailable(size)) {
                if(crash_handler != nullptr) crash_handler() else return nullptr;
            }

            void* res = (void*)&mem[ptr];
            ptr += size;
            return res; 
        }

};

#endif  //!__MEMPOOL__H__
