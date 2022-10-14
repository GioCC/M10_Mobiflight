/*
*
* This file implements the CmdEventManager class
*
*/

#include <stdlib.h>
#include "CmdEventManager.h"

/// This constructor call for the default instance could be removed from here and moved in the user's source,
/// before the CmdEvent constructors.
/// The constructor version 'CmdEventManager(CMCompare f)' should preferably be used there, otherwise - lacking a
/// comparator, which would be assigned AFTER the the CmdEvent constructors - these latter would NOT be added to the list!

CmdEventManager CmdEventMgr(stdCompare);

/// Constructor version to be normally used
CmdEventManager::CmdEventManager(CMCompare f)
: _compare(f), numCmdEvents(0), currCmd(0), lastPos(0)
{
}

/// Use this constructor only if you can make sure that the comparation function is assigned before starting to add
/// elements (ie. to instantiate CmdEvent objects)!
CmdEventManager::CmdEventManager(void)
: numCmdEvents(0), currCmd(0), lastPos(0)
{
}

void
CmdEventManager::setCompare(CMCompare f)
{
    _compare = f;
    /// TODO - MANDATORY:
    /// if the comparation function is changed when the list is already populated,
    /// the list MUST then be sorted according to the new comparation criteria!!!
}

uint8_t
CmdEventManager::add(CmdEvent* ce)
{
    if(_compare == NULL) return 0xFF;    // Can't add elements until we can sort them!
    if (numCmdEvents+1 < MAXCMDEVENTS) {
        CmdEvent* cp;
        lastPos = numCmdEvents;
        numCmdEvents++;
        cmdevents[lastPos] = ce;
        // Bring element back until sorting is restored
        while(lastPos >= 1 && (_compare(cmdevents[lastPos-1], ce)>0)) {
            cp = cmdevents[lastPos-1]; cmdevents[lastPos-1] = cmdevents[lastPos]; cmdevents[lastPos] = cp;
            lastPos--;
        }
        return numCmdEvents-1;
    } else {
        return 0xFF;
    }
}

CmdEvent *
CmdEventManager::getCmdEvent(uint8_t index) {
    return ((index >= numCmdEvents) ? ((index == 0xFF) ? cmdevents[currCmd] : NULL) : cmdevents[index]);
}

CmdEvent *
CmdEventManager::nextCmdEvent(uint8_t index) {
    if(index != 0xFF) currCmd = index;
    if(currCmd >= numCmdEvents) currCmd = 0;
    return cmdevents[currCmd++];
}

CmdEvent*
CmdEventManager::seekFirst(CmdEvent* ce, uint8_t &pos)
{
/// TODO: check that, on non-match, the returned position is exactly the position where
/// the sought element should be inserted if so required.
/// This would allow e.g. to use seekFirst also for seeking the insertion position of a new element
/// (not just for matching).
    if(_compare) return 0;
    // Seeks in list, returns index or 0xFF if not found
    uint8_t p;
    uint8_t p1 = 1;
    uint8_t p2 = numCmdEvents;
    int8_t  c;

    pos = 0xFF;
    if(numCmdEvents && _compare) {
        do {
            p = (p1+p2)/2;
            c = _compare(cmdevents[p-1], ce);
            if(c<0) {
                p1 = p+1;
            } else
            if(c>0) {
                p2 = p-1;
            } else /*if(c==0)*/ {
                // backtrack to first matching element in a group
                while(p >= 2 && (_compare(cmdevents[p-2], ce))) --p;
                pos = p-1;     // record match position
                return cmdevents[pos];
            }
        pos = p-1;     // record last position
        }
        while(p1<=p2);
    }
    return NULL;
}

CmdEvent*
CmdEventManager::seekNext(CmdEvent* ce, uint8_t &pos)
{
    // almost trivial
    if((pos < (numCmdEvents-1) && _compare)) {
        if(_compare(cmdevents[++pos], ce) == 0){
            return cmdevents[pos];
        }
    }
    return NULL;
}

uint8_t
CmdEventManager::check(char *tag, char *payload, uint8_t multiple)
{
    matcher.info(tag);
    return _check(&matcher, payload, multiple);
}

uint8_t
CmdEventManager::check(uint16_t tagh, uint16_t tagl, char *payload, uint8_t multiple)
{
    matcher.info(tagh, tagl);
    return _check(&matcher, payload, multiple);
}

uint8_t
CmdEventManager::_check(CmdEvent *toFind, char *payload, uint8_t multiple)
{
    CmdEvent    *found;
    uint8_t     firstMatch = 0xFF;
    if((found = seekFirst(toFind, lastPos)) != NULL) {
        firstMatch = lastPos;
        do {
            if(found->_callback) { found->_callback(toFind, payload); }
            else if(_fallbackCallback && !(found->flags & CEF_NoDefltCallback)) { _fallbackCallback(toFind, payload); }
        } while(multiple && (found = seekNext(toFind, lastPos)));
    }
    return firstMatch;
}
