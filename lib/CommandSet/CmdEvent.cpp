/*
* File     : CmdEvent.cpp
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
*/

#include "CmdEvent.h"
#include "CmdEventManager.h"


/// =========================================================
/// Example of callback:
void  callbackExample(CmdEvent *matching, byte *payload)
{
    Serial.println(matching->getTagName());
    Serial.println((payload[0]<<8) + payload[1], 16);
}
/// =========================================================
/// Example of compare functions to be passed to CmdEventManager:
///
/// Following sample functions compare elements by the value of one field only.
/// Depending on the protocol, more complex matching rules can be established.

/// Sample compare function if only name tags are used
int8_t compareNameTag(CmdEvent *to, CmdEvent *src)
{
    // Let's assume that we only use string tags - and trust that won't find surprises
    // Consider (e.g.) first 4 chars (we assume that the rest is payload)
    return strncmp(to->getTagName(), src->getTagName(), 4);
    // Or the whole string
    // return strcmp(to->getTagName(), src->getTagName());
}

/// Sample compare functions if only int code tags (either high or low) are used
int8_t compareCodeTagH(CmdEvent *to, CmdEvent *src)
{
    // Let's assume that we only use int (high) tags - and trust that won't find surprises
    uint16_t t1; to->getTagCode(&t1);
    uint16_t t2; src->getTagCode(&t2);
    return (t1 == t2) ? 0 : ((t1<t2) ? -1 : +1);
}

int8_t compareCodeTagL(CmdEvent *to, CmdEvent *src)
{
    // Let's assume that we only use int (high) tags - and trust that won't find surprises
    uint16_t t1; to->getTagCode(NULL, &t1);
    uint16_t t2; src->getTagCode(NULL, &t2);
    return (t1 == t2) ? 0 : ((t1<t2) ? -1 : +1);
}

/// Sample compare function considering that tags may be names or codes (mixed)
int8_t compareTag(CmdEvent *to, CmdEvent *src)
{
    if(to->getTagType() > src->getTagType()) return -1;
    if(to->getTagType() < src->getTagType()) return +1;

    if(to->getTagType() != 0) {
        if(src->getTagType() == 0) return +1;
        // both src and to are strings
        // Consider (e.g.) first 3 chars
        return strncmp(to->getTagName(), src->getTagName(), 3);
    } else {
        if(src->getTagType() != 0) return -1;
        // both src and to are pairs of uint16
        // Let's consider just the first one
        uint16_t t1; to->getTagCode(&t1);
        uint16_t t2; src->getTagCode(&t2);
        return (t1 == t2) ? 0 : ((t1<t2) ? -1 : +1);
    }
    //return 0;   // dummy, never gets here
}
/// =========================================================

int8_t stdCompare(CmdEvent *to, CmdEvent *src)
{
    ///
    /// CHOOSE CORRECT REQUIRED FUNCTION HERE!
    /// (or define another elsewhere for all that matters...)
    ///

    //return compareNameTag(to, src);
    //return compareCodeTagH(to, src);
    return compareCodeTagL(to, src);
    //return compareTag(to, src);
}


CmdEvent::CmdEvent(const char *s, uint8_t dataSize, uint8_t bitNo, CECallback f)
{
    info(s, dataSize, bitNo);
    _callback = f;
    CmdEventMgr.add(this);
}

CmdEvent::CmdEvent(uint16_t codeh, uint16_t codel, uint8_t dataSize, uint8_t bitNo, CECallback f)
{
    uint8_t index;
    info(codeh, codel, dataSize, bitNo);
    _callback = f;
    index = CmdEventMgr.add(this);
    // "joker" code for autonumbering of events
    if(codeh == NEXT_EVENT) {
        _tag.code.hi = index;
    }
}

CmdEvent *
CmdEvent::info(const char *s, uint8_t dataSize, uint8_t bitNo)
{
    // this reserved data-changing method can only be used on objects not in the mgr list,
    // to prevent altering its sorting
    _tag.text = s;
    flagChg(flags, CEF_String, 1);
    _size = dataSize;
    setBitNum(bitNo);
    return this;
}

CmdEvent *
CmdEvent::info(uint16_t codeh, uint16_t codel, uint8_t dataSize, uint8_t bitNo)
{
    // this reserved data-changing method can only be used on objects not in the mgr list,
    // to prevent altering its sorting
    _tag.code.hi = codeh;
    _tag.code.lo = codel;
    flagChg(flags, CEF_String, 0);
    _size = dataSize;
    setBitNum(bitNo);
    return this;
}

// end
