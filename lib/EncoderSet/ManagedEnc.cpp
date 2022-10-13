/*
*
* File     : ManagedEnc.cpp
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

#include "EncManager.h"
#include "ManagedEnc.h"

ManagedEnc::ManagedEnc( uint8_t     index,
                        char*       nm,
                        MEcallback  OnChange,
                        MEcallback  OnUp,
                        MEcallback  OnDn,
                        MEcallback  OnFastUp,
                        MEcallback  OnFastDn,
                        MEcallback  OnModeChg,
                        uint8_t     numModes
                        )
: idx(index),
  _OnChange(OnChange), _OnUp(OnUp), _OnDn(OnDn), _OnFastUp(OnFastUp), _OnFastDn(OnFastDn), _OnModeChg(OnModeChg),
  lastCount(0), lastDiff(0), fastStep(10), flags(0), nModes(numModes)
{
    setName(nm);
    //if(EncMgr) EncMgr->addEnc(this);
    EncMgr.addEnc(this);
}

ManagedEnc::ManagedEnc( uint8_t     index,
                        uint16_t    codeh,
                        uint16_t    codel,
                        MEcallback  OnChange,
                        MEcallback  OnUp,
                        MEcallback  OnDn,
                        MEcallback  OnFastUp,
                        MEcallback  OnFastDn,
                        MEcallback  OnModeChg,
                        uint8_t     numModes
                        )
: idx(index),
  _OnChange(OnChange), _OnUp(OnUp), _OnDn(OnDn), _OnFastUp(OnFastUp), _OnFastDn(OnFastDn), _OnModeChg(OnModeChg),
  lastCount(0), lastDiff(0), fastStep(10), flags(0), nModes(numModes)
{
    setTag(codeh, codel);
    //if(EncMgr) EncMgr->addEnc(this);
    EncMgr.addEnc(this);
}

// Encoder count arguments:
// for checkCnt(.):
// - currCount  the current total count of the encoder
// - mode       the current encoder mode (if used)
// for checkTrn(..):
// - dPulses       number of single pulses (signed)
// - dFastPulses   number of fast pulses (signed)
// - mode          the current encoder mode (if used)
// with the transition values, the total count is updated
// (also according to the fast step size defined with FastStepSize()).
// If dFastPulses!=0 but dPulses==0, the callback for slow pulses is NOT called!

void
ManagedEnc::checkCnt(CountType cnt, uint8_t mode)
{
    if(cnt != lastCount) {
        lastDiff = cnt-lastCount;
        lastCount = cnt;
        if(_OnChange) _OnChange(this);
        if(flags & ME_CntToTrn) {
            uint8_t np = (lastDiff > 0 ? lastDiff : -lastDiff);
            for (uint8_t i=0; i<np; i++) {
                if(_OnUp && lastDiff>0) {
                    _OnUp(this);
                } else
                if(_OnDn && lastDiff<0) {
                    _OnDn(this);
                }
            }
        }
    }
    if(mode != nMode) {
        nMode = mode;
        if(_OnModeChg) _OnModeChg(this);
    }
}

void
ManagedEnc::checkTrn(int8_t dPulses, int8_t dFastPulses, uint8_t mode)
{
    lastDiff = dPulses + dFastPulses*fastStep;
    lastCount += lastDiff;
    if(_OnUp && dPulses>0) {
        _OnUp(this);
    } else
    if(_OnDn && dPulses<0) {
        _OnDn(this);
    }
    if(_OnFastUp && dPulses>0) {
        _OnFastUp(this);
    } else
    if(_OnFastDn && dPulses<0) {
        _OnFastDn(this);
    }
    if(mode !=nMode) {
        nMode = mode;
        if(_OnModeChg) _OnModeChg(this);
    }
    if(flags & ME_TrnToCnt) {
        _OnChange(this);
    }
}

// END ManagedEnc.cpp
