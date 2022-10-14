/*
*
*
* This file defines the CmdEventManager class.
*
*/

#ifndef CMDEVENTMANAGER_H
#define CMDEVENTMANAGER_H

#include <Arduino.h>

//typedef uint16_t Tvector;

//#define LAST  (CmdEventMgr.getCmdEvent())

// This include is just for usage with M10 boards (in order to tailor storage requirements); it is not for the general purpose library
#include "config_board.h"
// define the max number of event types handled
#ifndef MAXCMDEVENTS
#define MAXCMDEVENTS 20
#endif

#include "CmdEvent.h"

/// The compare function is required to return:
///  0 on match
/// -n if [to] '<' [src]
/// +n if [to] '>' [src]
/// (n can be any)
/// '<' and '>' are meant according to whichever criteria is used
typedef int8_t (*CMCompare)(CmdEvent *to, CmdEvent *src);

class CmdEventManager
{
friend class CmdEvent;

    CMCompare   _compare;
    CECallback  _fallbackCallback;  // just couldn't resist naming it this :)

    int         numCmdEvents;
    uint8_t     currCmd;
    uint8_t     lastPos;
    CmdEvent    *cmdevents[MAXCMDEVENTS];
    CmdEvent    matcher;            // internal instance for service purposes

    //static CmdEventManager *s_instance;
    //CmdEventManager();

    uint8_t     add(CmdEvent* ce);              // Adds to list, keeping list sorted

    /// Seeks match in storedlist
    /// Returns addr if found (NULL otherwise); in this case, the index of found element is written into argument 'pos'
    CmdEvent*   seekFirst(CmdEvent* ce, uint8_t &pos);

    /// Seeks next match starting from last position
    /// Returns addr if found (NULL otherwise); in this case, the index of found element is written into argument 'pos'
    /// Last position might be invalid (eg. previous not found, or never sought) - anyway this would either
    /// gracefully return "no match", or - by blind luck - even a match (in which case, all is well anyway).
    CmdEvent*   seekNext(CmdEvent* ce, uint8_t &pos);

    /// utility function
    uint8_t     _check(CmdEvent* ce, char *payload=NULL, uint8_t multiple = 0);

public:

    CmdEventManager(void);
    CmdEventManager(CMCompare f);

    void        setCompare(CMCompare f);

    void        setDefaultCallback(CECallback f)    {_fallbackCallback = f;}

    // Collection management:
    uint8_t     getNumCmdEvents(void) {return numCmdEvents;}
    CmdEvent    *getCmdEvent(uint8_t index = 0xFF);
    CmdEvent    *nextCmdEvent(uint8_t index = 0xFF);

    /// Main operation methods
    ///
    /// Tries to find an event in the stored list that matches the one built with passed data.
    /// If found, passes along the payload data (together with the cmdEvent built)
    /// to the stored callback (or, missing this, the default callback, unless barred).
    /// Notice that the match relies on a function given by the user: thus, any kind of comparison
    /// on any portion of the passed data can be used as a match rule (e.g. obtaining group matches,
    /// or multiple ones).
    /// The condition "multiple" tells the matching function to look for all matches (if more than one
    /// is available), and in case execute all their callbacks.
    /// Returned value is the index of the (first) matching element (or 0xFF if none found)

    uint8_t check(char *tag, char *payload=NULL, uint8_t multiple = 1);

    uint8_t check(uint16_t tagh, uint16_t tagl, char *payload=NULL, uint8_t multiple = 1);

};

extern  CmdEventManager CmdEventMgr;

#endif


