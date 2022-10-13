/*
*
* File     : CmdEvent.h
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
*
*/

#ifndef CMDEVENT_H
#define CMDEVENT_H

#include <Arduino.h>
#include "RawData.h"

#define flagChg(value, bitmsk, cond) ((cond) ? (value |= bitmsk) : (value &= ~bitmsk) )
#define setBitNum(n)                 {flags &= ~CEF_bitNum; flags |= (n & CEF_bitNum);}

#define NEXT_EVENT  0xFFFF      // See constructor for info

class CmdEvent;

/// This is the compare function; it must be defined either in this unit or somewhere else, but it must be
/// known during compilation, in order to allow the manager to already have the reference to it in its constructor,
/// BEFORE the first CmdEvents are instantiated in the user's code.

extern int8_t stdCompare(CmdEvent *, CmdEvent *);


// Bitmasks for 'flags'
// (some may be applicable to just specific CmdEvent types)
enum {
    CEF_InList          = 0x20,   // if =1, this object has been inserter in the manager's list, thus its data should not be altered
    CEF_String          = 0x10,   // if =1, _tag data was built passing a string (and must be interpreted as such) rather than 2 uints
    CEF_NoDefltCallback = 0x08,   // If no callback specified, do not use default one even if available (neutralizes cmds)
    CEF_bitNum          = 0x07    // Mask of the bits used to store the bit# in the mirror variable
    };

typedef void(*CECallback)(CmdEvent *matching, char *payload);

class CmdEvent
{
friend class CmdEventManager;

public:

typedef uint8_t CmdEventStatus_t;


    /// ==========================================
    ///  Special members
    ///
    /// These members generate / operate on objects which are NOT inserted / present in the manager's list.
    /// They must be used carefully, otherwise they could be mixed up with the other methods for ordinary
    /// usage, and they would disrupt the operation of the CmdEventManager.
    /// (A safeguard is in place anyway through the CEF_InList flag, that marks the object as belonging to a list
    /// and prevents modification; however, if modification fails no alert is issued to the caller).
    /// These serve two different purposes:
    ///
    /// 1. PRE-ALLOCATION. Objects can be pre-allocated at compile time with this empty constructor, and
    /// will be completely filled in later.
    /// This avoids the need for dynamic allocation support and, most importantly, allows to detect during
    /// compilation whether the required space fits memory size or not.
    /// Since objects allocated in this way (typically through an array) are not added to the manager collection,
    /// they must be explicitly added later, and their values filled in with the other methods in this group.
    ///
    /// 2. SERVICE INSTANCES. Instances generated in this way can be used as service objects (eg. to define a sample
    /// object to be matched against) since they can be modified without disrupting the list sorting.
    /// If this purpose were the only required, the methods could be defined as private and reserved to friend
    /// class(es)  (basically CmdEventManager).

    CmdEvent(void)  {}
    CmdEvent        *info(const char *s, uint8_t dataSize=0, uint8_t bitNo=0);
    CmdEvent        *info(uint16_t codeh, uint16_t codel=0, uint8_t dataSize=0, uint8_t bitNo=0);

    ///
    /// ==========================================


    /// These constructors allow each individual CmdEvent to be defined in the most concise way,
    /// if all required information is available at compile time.
    /// CmdEvents are automatically added to the collection in the CmdEventGroupManager named 'CmdEventGrpMgr'
    /// to allow centralized polling. From there they can also be retrieved for custom operations.

    CmdEvent(const char *s, uint8_t dataSize, uint8_t bitNo=0, CECallback f=NULL);
    CmdEvent(uint16_t codeh, uint16_t codel, uint8_t dataSize, uint8_t bitNo=0, CECallback f=NULL);

    // === Bulk setup methods

    // === Setters (single params)
    void setCallback(CECallback f)      __attribute__((always_inline))  {_callback = f;}

    // === Getters

    uint8_t getTagType(void)    __attribute__((always_inline))  {return ((flags&CEF_String) != 0);}
    RawData *getTag(void)       __attribute__((always_inline))  {return &_tag;}
    char*   getTagName(void)    __attribute__((always_inline))  {return (char*)_tag.text;}
    void    getTagCode(uint16_t *hi, uint16_t *lo=NULL)
                                __attribute__((always_inline))  {if(lo){*lo=_tag.code.lo;} if(hi){*hi=_tag.code.hi;}}
    uint8_t getSize(void)       __attribute__((always_inline))  {return _size;}
    uint8_t getBit(void)        __attribute__((always_inline))  {return flags & CEF_bitNum;}

private:

    // Private data:

    RawData     _tag;
    uint8_t     _size;      // Size in bytes; 0 = bit var (see bit index in flags)
    uint8_t     flags;      // contains the bit index (3 right bits)

    CECallback  _callback;

};

#endif
