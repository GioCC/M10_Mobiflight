/*
* File     : debouncer.hpp
* Version  : 1.0
* Released : 20/08/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
*
* Vector-level debounce filter
*
*/

// Bogus include to satisfy IDE syntax parser
#ifndef DEBOUNCER_H
#include "debouncer.h"
#endif

template <class T>
T debouncer<T>::holder(T newi)
{
    unsigned long now = millis();
    if (newi != lastValid) {    // button state changed
        if(TlastChange == 0) {
            TlastChange = now;  // only transitions during stable states are regarded
        } else if (now - TlastChange >= debounceTime) {
            TlastChange = 0;    // this condition means "stable"
            lastValid = newi;
        }
    }
    return lastValid;
}

template <class T>
T debouncer<T>::delayer(T newi)
{
    unsigned long now = millis();
    if (newi != lastValid) {    // button state changed
        TlastChange = now;
    } else if (now - TlastChange >= debounceTime) {
        lastValid = newi;
    }
    return lastValid;
}

template <class T>
T debouncer<T>::blanker(T newi)
{
    unsigned long now = millis();
    if (now - TlastChange >= debounceTime) {
        if (newi != lastValid) {    // button state changed
            lastValid = newi;
            TlastChange = now;
        }
    }
    return lastValid;
}

// make the linker aware of some possible types
template class debouncer<uint8_t>;
//template class debouncer<int8_t>;
template class debouncer<uint16_t>;
//template class debouncer<int16_t>;
template class debouncer<uint32_t>;
//template class debouncer<int32_t>;
//template class debouncer<uint64_t>;
//template class debouncer<int64_t>;
//template class debouncer<char>;

