/*
*
* File     : EncManager.cpp
* Version  : 1.0
* Released : 12/03/2017
* Author   : Giorgio CROCI CANDIANI (g.crocic@gmail.com)
*
* Inspired by the AdvButton+ButtonManager library by Bart Meijer (bart@sbo-dewindroos.nl)
*
* This is a wrapper library for an underlaying encoder reader; it allows to stick callbacks
* to encoder events. Encoder polling functions can also be defined (as long as they have the proper interface),
* so the class is not tightly bound to a particular encoder reader library.
* - Event based implementation
* - requires only a single call in the main loop
*
* This file implements the EncManager class
*
*/

#include "EncManager.h"
#include <stdlib.h>
#include "bitmasks.h"

EncManager  EncMgr;

#ifdef USE_CALLBACKS
EncManager::EncManager(
    int  (*getCount)(byte),
    byte (*getUp)(byte),
    byte (*getDn)(byte),
    byte (*getFastUp)(byte),
    byte (*getFastDn)(byte),
    byte (*getMode)(byte),
    void (*clean)(byte)
    )
{
    numEncs = 0;
    currEnc = 0;
    _getCount = getCount;
    _getUp = getUp;
    _getDn = getDn;
    _getFastUp = getFastUp;
    _getFastDn = getFastDn;
    _getMode = getMode;
    _clean = clean;
}
#endif // USE_CALLBACKS

EncManager::EncManager(void)
{
    numEncs = 0;
    currEnc = 0;
#ifdef USE_CALLBACKS
    _getCount = NULL;
    _getUp = NULL;
    _getDn = NULL;
    _getFastUp = NULL;
    _getFastDn = NULL;
    _clean = NULL;
#endif // USE_CALLBACKS
}

void
EncManager::addEnc(ManagedEnc* enc)
{
    if (numEncs+1 < MAXENCS) {
        numEncs++;
        encs[numEncs-1]= enc;
    }
}

ManagedEnc *
EncManager::getEnc(uint8_t nEnc) {
    return (((nEnc-1) >= numEncs) ? NULL : encs[nEnc-1]);
}

ManagedEnc *
EncManager::nextEnc(uint8_t nEnc) {
    if(nEnc != 0) currEnc = nEnc-1;
    if(currEnc >= numEncs) currEnc = 0;
    return encs[currEnc++];
}

#ifdef USE_CALLBACKS
void
EncManager::checkEncs(byte encno)
{
    ManagedEnc *ee;
    for(int i=((encno==0xFF) ? 0 : encno);
        i < ((encno==0xFF) ? numEncs : encno+1);
        i++) {
        ee = encs[i];
        if(_getCount) {
            encs[i]->checkCnt(_getCount(i), (_getMode ? _getMode(i):0));
        } else {
            int8_t c = 0;
            int8_t f = 0;
            if(_getUp) c += _getUp(i);
            if(_getDn) c -= _getDn(i);
            if(_getFastUp) f += _getFastUp(i);
            if(_getFastDn) f -= _getFastDn(i);
            encs[i]->checkTrn(c, f, (_getMode ? _getMode(i):0));
        }
        if(_clean) _clean(i);
    }
}

#else

void
EncManager::checkEncs(int counts[], uint8_t modes[], byte encno)
{
    uint8_t m = 0;
    // scan all encoders or just the one if so specified
    for(int i=((encno==0xFF) ? 0 : encno);
        i < ((encno==0xFF) ? numEncs : encno+1);
        i++) {
        if(modes) m = modes[i];
        encs[i]->checkCnt((CountType)counts[i], m);
    }
}

void
EncManager::checkEncs(uint16_t flgUp, uint16_t flgDn, uint16_t flgFastUp, uint16_t flgFastDn, uint8_t modes[], byte encno)
{
    uint8_t m = 0;
    // scan all encoders or just the one if so specified
    for(int i=((encno==0xFF) ? 0 : encno);
        i < ((encno==0xFF) ? numEncs : encno+1);
        i++) {
        int8_t t = 0;
        int8_t ft = 0;
        if(flgUp & wmasks[i]) t++;
        if(flgDn & wmasks[i]) t--;
        if(flgFastUp & wmasks[i]) ft++;
        if(flgFastDn & wmasks[i]) ft--;
        if(modes) m = modes[i];
        encs[i]->checkTrn(t, ft, m);
    }
}
#endif

