/*
*
* File     : EncManager.h
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the AdvButton+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This is a wrapper library for an underlying encoder reader; it allows to stick callbacks
* to encoder events. Encoder polling functions must also be passed as callbacks (as long as they have the proper interface),
* so the class is not tightly bound to a particular encoder reader library.
* - Event based implementation
* - requires only a single call in the main loop
*
* This file defines the EncManager class.
*
*/

/// The purpose of this class is:
/// - to keep a collection of ManagedEnc objects
/// - to scan the collected ManagedEncs (supplying them with processed encoder data, ie. transitions, counts etc.)
///   so they can work their stuff and call user callbacks on events.
///
/// General use:
/// The user code first feeds inputs to the processor class, and then calls the ManagedEnc objects 
/// (either directly or through the EncoderManager) in order to act on the processed results.
///
/// This class relies on the presence of a processor class that reads the status of input lines
/// (HW or SW) and handles transition detection and counters; data from this class (usually regarding
/// a whole encoder bank) is passed along to the single ManagedEncs one by one.
/// As a matter of fact, this could be sensibly defined as a derived class of the processor class used as base;
/// however, for this application, they have been kept separate.
/// 
/// The result of the pre-processing is a set of status flags for each encoders:
/// Up, Down, FastUp, FastDown, Mode. These can be grouped either by encoder or by flag type (depending on
/// the USE_CALLBACKS option, see below). 
///
/// Data from the data provider class can be transmitted in two ways (depending on how the class is compiled):
/// - WITHOUT CALLBACKS: The encoder flags are passed to the polling function at each invocation
///   as a set of bit vectors, one for each flag type - Up, Dn, Fast Up, etc.
///   The constructor requires no arguments.
/// - WITH CALLBACKS: the polling functions invokes callbacks to retrieve the flags for each encoder.
///   The callback functions are passed to the constructor (or set later on);
///   the polling function requires no arguments.
/// The class is compiled in the first variant unless USE_CALLBACKS is defined.
///
/// Please note that callbacks in this class are used to FETCH ENCODER DATA, not to be confused with
/// callbacks in the ManagedEnc's which PERFORM THE ACTIONS corresponding to encoder movements.

//#define USE_CALLBACKS

#ifndef ENCMANAGER_H
#define ENCMANAGER_H

// This include is just for usage with M10 boards (in order to tailor storage requirements); it is not for the general purpose library
//#include "config_board.h"

#include "config_board.h"
// define the number of encoders
#ifndef MAXENCS
#define MAXENCS 10
#endif // MAXENCS

#include <Arduino.h>
#include "ManagedEnc.h"

class  EncManager;
extern EncManager  EncMgr;


// Encoder polling callbacks:
// - getCount()     returns the current total count of the encoder
// - getMode()      returns the current mode for the encoder
// - getUp()        returns TRUE if there has been a single (or more) pulse in the UP direction
// - getDn()        returns TRUE if there has been a single (or more) pulse in the DOWN direction
// - getFastUp()    returns TRUE if there has been a single (or more) FAST pulse in the UP direction
// - getFastDn()    returns TRUE if there has been a single (or more) FAST pulse in the DOWN direction
// The latter four functions can also return a number of pulses >1, however their value is only regarded as boolean.

class EncManager
{

    uint8_t       numEncs;
    uint8_t       currEnc;

    ManagedEnc    *encs[MAXENCS];

#ifdef USE_CALLBACKS
    int  (*_getCount)(byte);        // ptr to function that returns encoder count for encoder #n
    byte (*_getUp)(byte);           // ptr to function that returns the number of up counts since last invocation for encoder #n
    byte (*_getDn)(byte);           // ditto, for down counts
    byte (*_getFastUp)(byte);       // ditto, for fast-up counts
    byte (*_getFastDn)(byte);       // ditto, for fast-down counts
    byte (*_getMode)(byte);         // ditto, for mode
    void (*_clean)(byte);           // ptr to function that (if required) cleans up/dn flags after reading
#endif

public:

#ifdef USE_CALLBACKS
    // This constructor should be the default one, since it allows to specify the required
    // callbacks for encoder data polling.
    // However, for the default instance, the other parameterless constructor must be used.
    EncManager(
                int  (*getCount)(byte),
                byte (*getUp)(byte),
                byte (*getDn)(byte),
                byte (*getFastUp)(byte),
                byte (*getFastDn)(byte),
                byte (*getMode)(byte),
                void (*clean)(byte)
                );
    // This parameterless constructor is used for the default instance,
    // where the data polling callbacks are not known and must be filled in later by user code.
    EncManager(void);
#else
    // This is the default constructor when not using callbacks
    EncManager(void);
#endif


#ifdef USE_CALLBACKS
    // These methods allow to set callbacks at a later time
    void setGetCount(int (*f)(byte))    { _getCount = (*f); }
    void setGetUp(byte (*f)(byte))      { _getUp = (*f); }
    void setGetDn(byte (*f)(byte))      { _getDn = (*f); }
    void setGetFastUp(byte (*f)(byte))  { _getFastUp = (*f); }
    void setGetFastDn(byte (*f)(byte))  { _getFastDn = (*f); }
    void setGetMode(byte (*f)(byte))    { _getMode = (*f); }
    void setClean (void (*f)(byte))     { _clean = (*f); }

    // Polling function - version with callbacks.
    // If the _getCount callback is defined, only encoder counts are used for checks;
    // otherwise transitions are used, bot not both.
    // After the check, for each encoder the _clean function is called (if provided),
    // in order to reset the transition flags (not used for counts).
    // If no _clean is provided, the caller must take care of it.
    // Loops over all encoders if encno=0xFF, otherwise only does #encno.
    void checkEncs(byte encno=0xFF);
#else

    // Polling functions
    // Loop over all encoders if encno=0xFF, otherwise only do #encno.
    //
    // The two versions of the polling function below trigger each the corresponding events only:
    // if both count and transition events are required (provided that the data source supplies them),
    // both functions should be called.

    // This version uses encoder counts.
    void checkEncs(int counts[],
                   uint8_t modes[] = NULL, byte encno=0xFF);

    // This version uses transition flags.
    // After the check, the caller must take care of clearing the transition flags
    // of the encoders affected before next invocation!
    void checkEncs(uint16_t flagsUp, uint16_t flagsDn, uint16_t flagsFastUp, uint16_t flagsFastDn,
                   uint8_t modes[] = NULL, byte encno=0xFF);

    // Other versions of the above functions can be defined, for different data suppliers or "cleaner" interfaces
    // (e.g. passing all arrays, with one element per encoder, instead of byte patterns)

#endif

    void        addEnc(ManagedEnc* but);
    ManagedEnc *getEnc(uint8_t nEnc = 0);    // nEnc = 1..254; 0 is current
    ManagedEnc *nextEnc(uint8_t nEnc = 0);   // nEnc = 1..254; 0 is next
};

#endif


