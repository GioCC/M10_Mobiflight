/*
*
* File     : ManagedEnc.h
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the AdvButton+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This file defines the ManagedEnc class
*
* Usage:
* - Include ManagedEnc.h and EncManager.h in your sketch
* - Declare each encoder and define the events using a overload of ManagedEnc
* - Declare the required event functions ( void OnXXX(ManagedEnc* enc) )
* - See the comments below for more help
*/

/// The purpose of this class is:
/// - to check for encoder transitions/count changes and call the proper callback functions accordingly
/// - to keep some additional information for user convenience (a string ptr for a name or user string,
///   or int code values)
///
/// This class relies on the presence of another processor class that receives the status of input lines
/// (HW or SW) and handles transition detection and counters.
/// As a matter of fact, this could be sensibly defined as a derived class of the processor class used as base;
/// however, for this application, they have been kept separate (also because, for convenience, the
/// 'base' class handles an encoder set, not a single encoder).
/// The user code feeds inputs to the processor class, and triggers the checks of the processor class' results
/// by the ManagedEnc class (either directly ot through an EncoderManager).


#ifndef MANAGEDENC_H
#define MANAGEDENC_H

#include <Arduino.h>
#include "boardDefine.h"
#include "EncManager.h"

// Define this if callbacks are common to all ManagedEncoders
#define  ME_STATIC_CB

// Bitmasks for 'flags'
#define ME_CntToTrn     0x80
#define ME_TrnToCnt     0x40
#define ME_hasString    0x20

#define flagChg(value, bitmsk, cond) ((cond) ? (value |= bitmsk) : (value &= ~bitmsk) )

class ManagedEnc;

using     MEcallback = void (*)(ManagedEnc*);
using     CountType  = int;

class ManagedEnc
{
public:
    // Define type of injected function to automatically add a new button to a given collection
    using MEcollector = void (*)(ManagedEnc*);

private:

    // If non-null, this callback is invoked when a new Button object is created with a "make" factory function,
    // so the new Button is automatically added to a given collection/manager object.
    // BEWARE: it is NOT called if the object is created directly (must be called manually, if desired, through 
    // its public alias "Collect()")
    static MEcollector _collect;

#ifdef  ME_STATIC_CB
    static MEcallback _OnChange;
    static MEcallback _OnUp;
    static MEcallback _OnDn;
    static MEcallback _OnFastUp;
    static MEcallback _OnFastDn;
    static MEcallback _OnModeChg;
#else
    MEcallback _OnChange;
    MEcallback _OnUp;
    MEcallback _OnDn;
    MEcallback _OnFastUp;
    MEcallback _OnFastDn;
    MEcallback _OnModeChg;
#endif  //ME_STATIC_CB

   union {
        char        *text;
        uint32_t    tag;
    } name;

    uint8_t     idx;

    uint8_t     nMode;      // Current mode (used only for change detection)
    uint8_t     nModes;     // Number of modes (used only as storage, accessible for custom inits and operations)
    CountType   lastCount;
    int8_t      lastDiff;
    uint8_t     fastStep;

    uint8_t     flags;

public:

    // These constructors allow to define each individual encoder.
    // Encoders can be automatically added to the collection in an EncManager (if set through setCollector)
    // to allow centralized polling. From there they can also be retrieved for custom operations.
    //
    // In ordinary usage, two types of encoders may be defined:
    // - Encoders that maintain an own counter will only supply 'OnChange';
    // - Encoders that generate transitions will only supply 'OnUp' and 'OnDn' (and possibly 'OnFastUp'/'OnFastDn').
    // Of course this is not mandatory, and other settings, if sensible, can be chosen.
    // The name is optional.
    
    ManagedEnc( uint8_t     index,
                char*       name,
#ifndef  ME_STATIC_CB
                MEcallback  OnChange,
                MEcallback  OnUp        = NULL,
                MEcallback  OnDn        = NULL,
                MEcallback  OnFastUp    = NULL,
                MEcallback  OnFastDn    = NULL,
                MEcallback  OnModeChg   = NULL,
#endif
                uint8_t     nModes = 0
                );

    ManagedEnc( uint8_t     index,
                uint16_t    codeh,
                uint16_t    codel,
#ifndef  ME_STATIC_CB
                MEcallback  OnChange,
                MEcallback  OnUp        = NULL,
                MEcallback  OnDn        = NULL,
                MEcallback  OnFastUp    = NULL,
                MEcallback  OnFastDn    = NULL,
                MEcallback  OnModeChg   = NULL,
#endif
                uint8_t     nModes = 0
                );

    // =============
    // Setup methods

    // Unlike buttons, ManagedEncs currently require a few parameters in the constructors,
    // therefore "make()" methods are not yet implemented (they would be a little cumbersome).
    // Automatic addition to an EncoderManager could be incorporated in the constructors
    // (again unlike Buttons).
    // 
    // static ManagedEnc& make(void)       
    //     { ManagedEnc* pe = new ManagedEnc;    if(_collect != nullptr) _collect(pe); return *pe; }
    // static ManagedEnc& make(void *p)    
    //     { ManagedEnc* pe = new(p) ManagedEnc; if(_collect != nullptr) _collect(pe); return *pe; }
    static void setCollector(MEcollector cb) { _collect = cb; }

    ManagedEnc& Collect(void) 
        { if(_collect != nullptr) _collect(this); return *this;}

    void setOnChange(MEcallback f)  __attribute__((always_inline))  {_OnChange = (*f);}
    void setOnUp(MEcallback f)      __attribute__((always_inline))  {_OnUp = (*f);}
    void setOnDn(MEcallback f)      __attribute__((always_inline))  {_OnDn = (*f);}
    void setOnFastUp(MEcallback f)  __attribute__((always_inline))  {_OnFastUp = (*f);}
    void setOnFastDn(MEcallback f)  __attribute__((always_inline))  {_OnFastDn = (*f);}
    void setOnModeChg(MEcallback f) __attribute__((always_inline))  {_OnModeChg = (*f);}

    //MEcallback OnChange(void)       __attribute__((always_inline))  {return _OnChange;}
    //MEcallback OnUp(void)           __attribute__((always_inline))  {return _OnUp;}
    //MEcallback OnDn(void)           __attribute__((always_inline))  {return _OnDn;}
    //MEcallback OnFastUp(void)       __attribute__((always_inline))  {return _OnFastUp;}
    //MEcallback OnFastDn(void)       __attribute__((always_inline))  {return _OnFastDn;}
    //MEcallback OnModeChg(void)      __attribute__((always_inline))  {return _OnModeChg;}

    void setIndex(uint8_t i)        __attribute__((always_inline))  {idx = i;}
    void setName(char *n)           __attribute__((always_inline))  {name.text = n; flagChg(flags, ME_hasString, 1);}
    void setTag(uint32_t tag)       __attribute__((always_inline))  {name.tag = tag;  flagChg(flags, ME_hasString, 0);}
    void setTag(uint16_t tagh,
                uint16_t tagl)      __attribute__((always_inline))  {name.tag = (((uint32_t)tagh)<<16) + tagl; flagChg(flags, ME_hasString, 0);}
    void setFastStep(uint8_t s)     __attribute__((always_inline))  {fastStep = s;}
    void setCount(CountType cnt)    __attribute__((always_inline))  {lastCount = cnt;}

    // Following settings specify whether transition callbacks should be triggered also upon count change
    // and the other way around.
    // Only simple transitions are generated though, not fast ones.
    // May be useful if e.g. the encoder data is supplied in counts only, but transition events are required, or vice-versa.
    // Beware that these are not activated if the encoder supplies both types of information (and methods for both polling styles are called).
    // Also beware that count increments are not so high as to generate a callback flood.
    void    CntToTrn(uint8_t i) __attribute__((always_inline))  {(i ? (flags|=ME_CntToTrn):(flags&=~ME_CntToTrn));}
    void    TrnToCnt(uint8_t i) __attribute__((always_inline))  {(i ? (flags|=ME_TrnToCnt):(flags&=~ME_TrnToCnt));}

    // =============
    // Info methods

    uint8_t     getIndex(void)      __attribute__((always_inline))  {return idx;}
    char*       getName(void)       __attribute__((always_inline))  {return name.text;}
    uint32_t    getTag(void)        __attribute__((always_inline))  {return name.tag;}
    CountType   getCount(void)      __attribute__((always_inline))  {return lastCount;}
    int8_t      getDiff(void)       __attribute__((always_inline))  {return lastDiff;}
    int8_t      getMode(void)       __attribute__((always_inline))  {return nMode;}
    int8_t      getNModes(void)     __attribute__((always_inline))  {return nModes;}

    // =================
    // Operation methods

    // These are the poll functions used to detect encoder changes:
    // arguments passed (either the current up-to-date count, or the transition flags, or both)
    // should be supplied by an encoder reader.

    // 'checkCnt' is used when the encoder count is available
    void    checkCnt(CountType cnt, uint8_t mode=0);

    // 'checkTrn' is used if the encoder supplies up/down transition flags
    // (and transition callbacks are specified).
    // Usually called with d[Fast]Pulses = +1/-1; otherwise, the number of pulses
    // is used to correctly update the counter, but the callback for each
    // affected transition is invoked only once.
    void    checkTrn(int8_t dPulses, int8_t dFastPulses=0, uint8_t mode=0);

};

#endif
