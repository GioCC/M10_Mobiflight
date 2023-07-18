/*
* File     : debouncer.h
* Version  : 1.0
* Released : 20/08/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Vector-level debounce filter
*
* Three debounce mechanisms available:
* - holder: validates input if it has been stable for <DEBTIME_MS> ms.
*           Guarantees that validation occurs on a stable signal, but
*           delays detection (after the actual starting timepoint of the stable period)
*           by <DEBTIME_MS> + the time during which the signal is not stable.
*           Best suited when the glitch width is well known, and the delay is not an issue.
* - delayer: validates input by taking whatever value is present <DEBTIME_MS> ms
*           after the first transition that occurs while in a stable state.
*           Has a delayed detection like [holder], however not longer than <DEBTIME_MS> ms
*           (assuming the instability period is shorter than that, otherwise the returned
*           final state might not be correct).
*           Best suited when the max duration of the instability period is well known
*           and the delay is not an issue.
* - blanker: validates input right after first transition edge, ignoring any
*           further change in the input for the next <DEBTIME_MS> ms.
*           Has a very fast reaction time, however it is prone to being fooled
*           by glitches which are not the start of an actual transition.
*           Best suited when a fast reaction time is required, and no glitches are likely
*           to occur except as a consequence of the start of an actual transition.
*
*/

#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#define DEBTIME_MS  2

#include <Arduino.h>

template <typename T>
class debouncer
{

public:

    debouncer(): debounceTime(DEBTIME_MS), TlastChange(0), lastValid((T)0) {}

    explicit 
    debouncer(uint8_t dbtime): debounceTime(dbtime), TlastChange(0), lastValid((T)0) {}

    T update(T value)           { return (value);}
    T holder(T value);
    T delayer(T value);
    T blanker(T value);

    // === Setters (single params)

    void setDebounce(uint8_t dbtime)    {debounceTime = dbtime;}

    // === Getters

    T status(void)              {return lastValid;}

private:

    uint8_t         debounceTime;       // internally in ms
    unsigned long   TlastChange;
    T               lastValid;
    //uint8_t         pressFlag;

};

// Template implementation
#include "debouncer.hpp"

#endif // DEBOUNCER_H
