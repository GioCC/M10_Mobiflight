/**
 *  board_code.h
 *
 *  - Not a standard include file! -
 *  This file is made to be included in the main .ino source for a specific M10 board
 *  It collects all peripheral definitions, related callbacks and possibly additional custom code.
*/

#ifndef BOARD_CODE_H
#define BOARD_CODE_H

#include <Arduino.h>

#include "bitmasks.h"
#include "FastArduino.h"
//#include "bank32_novirt.h"
#include "varStore.h"
#include "Frequency.h"
#include "conversions.h"

#include "ButtonManager.h"
#include "EncManager.h"
#include "CmdEventManager.h"

#ifndef HAS_LCD
#include "LedControlMod.h"
#include "LEDface.h"
#else
#include "LiquidCrystal.h"
#include "LCDface.h"
#endif
#define UNUSED(x) ((void)(x))

//================================
// Board I/O configuration
#include "config_board.h"
//================================

// -------------------------------
// OFFSET/DATAREF DEFINES

#define     OFS_COM1_A    0x034E
#define     OFS_COM2_A    0x3118
#define     OFS_NAV1_A    0x0350
#define     OFS_NAV2_A    0x0352
#define     OFS_COM1_S    0x311A
#define     OFS_COM2_S    0x311C
#define     OFS_NAV1_S    0x311E
#define     OFS_NAV2_S    0x3120
#define     OFS_COM1_SWP  0x3123
#define     OFS_COM2_SWP  0x3123
#define     OFS_NAV1_SWP  0x3123
#define     OFS_NAV2_SWP  0x3123

const uint16_t Radio_DOfs[] PROGMEM = {OFS_COM1_A, OFS_COM2_A, OFS_NAV1_A, OFS_NAV1_A, OFS_COM1_S, OFS_COM2_S, OFS_NAV1_S, OFS_NAV1_S};

#define     DRF_COM1_A    OFS_COM1_A,2,0
#define     DRF_COM2_A    OFS_COM2_A,2,0
#define     DRF_NAV1_A    OFS_NAV1_A,2,0
#define     DRF_NAV2_A    OFS_NAV2_A,2,0
#define     DRF_COM1_S    OFS_COM1_S,2,0
#define     DRF_COM2_S    OFS_COM2_S,2,0
#define     DRF_NAV1_S    OFS_NAV1_S,2,0
#define     DRF_NAV2_S    OFS_NAV2_S,2,0
#define     DRF_COM1_SWP  OFS_COM1_SWP,0,3
#define     DRF_COM2_SWP  OFS_COM2_SWP,0,2
#define     DRF_NAV1_SWP  OFS_NAV1_SWP,0,1
#define     DRF_NAV2_SWP  OFS_NAV2_SWP,0,0

// -------------------------------
// INTERNAL UTILITY

#define     nCOM1       (0x00)
#define     nCOM2       (0x01)
#define     nNAV1       (0x02)
#define     nNAV2       (0x03)
#define     L_isCOM     (!(comnav & 0x01))
#define     L_isNAV     (comnav & 0x01)
#define     L_is1       (!(comnav & 0x02))
#define     L_is2       (comnav & 0x02)
#define     R_isCOM     (!(comnav & 0x04))
#define     R_isNAV     (comnav & 0x04)
#define     R_is1       (!(comnav & 0x08))
#define     R_is2       (comnav & 0x08)

#define     isCOM(n)    (!(n & 0x01))
#define     isNAV(n)    ((n & 0x01)!=0)
#define     is1(n)      (!(n & 0x02))
#define     is2(n)      ((n & 0x02)!=0)

#define     L_RECV_TYPE (comnav & 0x03)
#define     R_RECV_TYPE ((comnav>>2) & 0x03)

uint8_t     comnav = 0;
char        buf[8];

/// ===============================
///  INCOMING VARIABLES

Frequency   ComNavFreq[8];      // C1A, C2A, N1A, N2A, C1S, C2S, N1S, N2S

// Enum for frq change events
enum {N_COM1_A=0, N_COM2_A, N_NAV1_A, N_NAV2_A, N_COM1_S, N_COM2_S, N_NAV1_S, N_NAV2_S};

// Enum for frq swap events
enum {N_COM1_SWP=N_NAV2_S+1, N_COM2_SWP, N_NAV1_SWP, N_NAV2_SWP};

/// ===============================
///  INTERNAL EVENT DEFINITION

// Switch I/O pins
#define PB_ENC_L        1
#define PB_ENC_R        4
#define PB_PROG         10
#define SW_COMNAV_L     11
#define SW_ONETWO_L     12
#define SW_COMNAV_R     13
#define SW_ONETWO_R     14
#define PB_SWAP_L       15
#define PB_SWAP_R       16
// Other defines
#define ENC_L           1
#define ENC_R           2

void On_PProgram(ButtonGrp*);
void OnL_PProgram(ButtonGrp*);
void On_ComNavL(ButtonGrp*);
void On_ComNavR(ButtonGrp*);
void On_PSwapL(ButtonGrp*);
void On_PSwapR(ButtonGrp*);
void On_ChgFreqL(ManagedEnc*);
void On_ChgFreqR(ManagedEnc*);
void On_ChgModeL(ManagedEnc*);
void On_ChgModeR(ManagedEnc*);
void OnEvt_FreqChg(CmdEvent*, char*);
void OnEvt_Swap(CmdEvent*, char*);


ButtonGrp   PProgram    = ButtonGrp(PB_PROG, 0, 0, On_PProgram, NULL, OnL_PProgram);

ButtonGrp   SComNavL    = ButtonGrp(SW_COMNAV_L, 1,1, On_ComNavL, On_ComNavL, NULL, 0, &comnav, 1);
ButtonGrp   SOneTwoL    = ButtonGrp(SW_ONETWO_L, 2,2, On_ComNavL, On_ComNavL, NULL, 0, &comnav, 0);
ButtonGrp   SComNavR    = ButtonGrp(SW_COMNAV_R, 3,3, On_ComNavR, On_ComNavR, NULL, 0, &comnav, 3);
ButtonGrp   SOneTwoR    = ButtonGrp(SW_ONETWO_R, 4,4, On_ComNavR, On_ComNavR, NULL, 0, &comnav, 2);

ButtonGrp   PSwapL      = ButtonGrp(PB_SWAP_L,   5,5, On_PSwapL, NULL, NULL);
ButtonGrp   PSwapR      = ButtonGrp(PB_SWAP_R,   6,6, On_PSwapR, NULL, NULL);

ManagedEnc  EncLeft     = ManagedEnc(ENC_L,      7,7, On_ChgFreqL, NULL, NULL, NULL, NULL, On_ChgModeL);
ManagedEnc  EncRight    = ManagedEnc(ENC_R,      8,8, On_ChgFreqR, NULL, NULL, NULL, NULL, On_ChgModeR);

ButtonGrp   PEncLeft    = ButtonGrp(PB_ENC_L,    9, 9, NULL, NULL, NULL);
ButtonGrp   PEncRight   = ButtonGrp(PB_ENC_R,   10,10, NULL, NULL, NULL);

// EVENTS
// Code tags usage: Hi is used as index, Lo is the Dataref offset (constant also includes size or bit#)
// Sorting and matching must be done on Int code Lo
CmdEvent    ChgCOM1a    = CmdEvent(N_COM1_A, DRF_COM1_A, OnEvt_FreqChg);
CmdEvent    ChgCOM2a    = CmdEvent(N_COM2_A, DRF_COM2_A, OnEvt_FreqChg);
CmdEvent    ChgNAV1a    = CmdEvent(N_NAV1_A, DRF_NAV1_A, OnEvt_FreqChg);
CmdEvent    ChgNAV2a    = CmdEvent(N_NAV2_A, DRF_NAV2_A, OnEvt_FreqChg);
CmdEvent    ChgCOM1s    = CmdEvent(N_COM1_S, DRF_COM1_S, OnEvt_FreqChg);
CmdEvent    ChgCOM2s    = CmdEvent(N_COM2_S, DRF_COM2_S, OnEvt_FreqChg);
CmdEvent    ChgNAV1s    = CmdEvent(N_NAV1_S, DRF_NAV1_S, OnEvt_FreqChg);
CmdEvent    ChgNAV2s    = CmdEvent(N_NAV2_S, DRF_NAV2_S, OnEvt_FreqChg);

CmdEvent    SwapCOM1    = CmdEvent(N_COM1_SWP, DRF_COM1_SWP, OnEvt_Swap);
CmdEvent    SwapCOM2    = CmdEvent(N_COM2_SWP, DRF_COM2_SWP, OnEvt_Swap);
CmdEvent    SwapNAV1    = CmdEvent(N_NAV1_SWP, DRF_NAV1_SWP, OnEvt_Swap);
CmdEvent    SwapNAV2    = CmdEvent(N_NAV2_SWP, DRF_NAV2_SWP, OnEvt_Swap);

/// ===============================
///  Init code
/// ===============================

void board_init(void)
{
    // Both encoders have 2 modes (int or frac frequency change)
    board.Encs->setNModes(1, 2);
    board.Encs->setNModes(2, 2);

    board.Encs->setMode(1, 1);
    board.Encs->setMode(2, 1);
}

/// ===============================
///  Utility functions
/// ===============================

// Refresh LEFT act and/or stby display, reading data from stored frequencies
// which: 1=act, 2=stby, 3=both
void refreshL(uint8_t which = 3)
{
    uint8_t  idx = L_RECV_TYPE;
    if(which != 2) {
        board.Viewport(1)->Write(ComNavFreq[idx].ASC(buf,1));
        board.Viewport(1)->Send();
    }
    if(which != 1) {
        board.Viewport(2)->Write(ComNavFreq[idx+4].ASC(buf,1));
        board.Viewport(2)->Send();
    }
}

// Refresh RIGHT act and/or stby display, reading data from stored frequencies
// which: 1=act, 2=stby, 3=both
void refreshR(uint8_t which = 3)
{
    uint8_t  idx = R_RECV_TYPE;
    if(which != 2) {
        board.Viewport(3)->Write(ComNavFreq[idx].ASC(buf,1));
        board.Viewport(3)->Send();
    }
    if(which != 1) {
        board.Viewport(4)->Write(ComNavFreq[idx+4].ASC(buf,1));
        board.Viewport(4)->Send();
    }
}

// Refresh LEFT act OR stby display with supplied string
// which: 1=act, 2=stby
void refreshL(uint8_t which, char *freq)
{
    if(which == 1) {
        board.Viewport(1)->Write(freq);
        board.Viewport(1)->Send();
    } else
    if(which == 2) {
        board.Viewport(2)->Write(freq);
        board.Viewport(2)->Send();
    }
}

// Refresh RIGHT act OR stby display with supplied string
// which: 1=act, 2=stby
void refreshR(uint8_t which, char *freq)
{
    if(which == 1) {
        board.Viewport(3)->Write(freq);
        board.Viewport(3)->Send();
    }
    if(which == 2) {
        board.Viewport(4)->Write(freq);
        board.Viewport(4)->Send();
    }
}

/// ===============================
///  INTERNAL EVENT CALLBACKS
/// ===============================

void On_PProgram(ButtonGrp *src)
{
    UNUSED(src);
    // nothing to do yet
}

void OnL_PProgram(ButtonGrp *src)
{
    UNUSED(src);
    // nothing to do yet
}

void On_ComNavL(ButtonGrp *src)
{
    UNUSED(src);
    refreshL();
}

void On_ComNavR(ButtonGrp *src)
{
    UNUSED(src);
    refreshR();
}

void On_PSwapL(ButtonGrp *src)
{
    UNUSED(src);
    uint8_t fl = L_RECV_TYPE;
    //uint8_t fr = R_RECV_TYPE;

    // Send dataref change
    (isCOM(fl) ? (is1(fl) ? board.send(DRF_COM1_SWP, 1) : board.send(DRF_COM2_SWP, 1))
               : (is1(fl) ? board.send(DRF_NAV1_SWP, 1) : board.send(DRF_NAV2_SWP, 1))
    );

    /*  Following should be pointless (if not harmful):
        sending the swap will already cause the updated frequency values to be
        received as events, so no need to do it explicitly here

    // Swap local frequencies too
    ComNavFreq[fl].swap(&ComNavFreq[fl+4]);
    // Refresh display
    refreshL();
    // If other display has the same setting, refresh that too
    if(fl == fr) refreshR();
    
    */
}

void On_PSwapR(ButtonGrp *src)
{
    UNUSED(src);
    //uint8_t fl = comnav & 0x03;
    uint8_t fr = (comnav>>2) & 0x03;
    (isCOM(fr) ? (is1(fr) ? board.send(DRF_COM1_SWP, 1) : board.send(DRF_COM2_SWP, 1))
               : (is1(fr) ? board.send(DRF_NAV1_SWP, 1) : board.send(DRF_NAV2_SWP, 1))
    );

    // Send dataref change
    // TODO...

    /*  Following should be pointless (if not harmful):
        sending the swap will already cause the updated frequency values to be
        received as events, so no need to do it explicitly here

    // Swap local frequencies too
    ComNavFreq[fr].swap(&ComNavFreq[fr+4]);
    // Refresh display
    refreshR();
    // If other display has the same setting, refresh that too
    if(fl == fr) refreshL();

    */
}

void On_ChgFreqL(ManagedEnc *src)
{
    UNUSED(src);
    uint8_t fl = L_RECV_TYPE;
    uint8_t fr = R_RECV_TYPE;

    if(src->getMode()==1) {     // Mode1: frac (25/50 kHz)
        ComNavFreq[fl+4].addkHz(src->getDiff(), isCOM(fl), buf, 0);
    } else {                    // Mode2: int  (1 MHz)
        ComNavFreq[fl+4].addMHz(src->getDiff(), isCOM(fl), buf, 0);
    }

    // Send freq change to dataref
    board.send(Radio_DOfs[fr], 2, 0, ComNavFreq[fl+4].BCD());
    // Alt (use string): board.send(Radio_DOfs[fl], 4, buf+1);

    // Refresh display (stby only)
    refreshL(2, buf);
    // If other display has the same setting, refresh that too
    if(fl == fr) refreshR(2, buf);
}

void On_ChgFreqR(ManagedEnc *src)
{
    UNUSED(src);
    uint8_t fl = L_RECV_TYPE;
    uint8_t fr = R_RECV_TYPE;

    if(src->getMode()==1) {     // Mode1: frac (25/50 kHz)
        ComNavFreq[fr+4].addkHz(src->getDiff(), isCOM(fr), buf, 0);
    } else {                    // Mode2: int  (1 MHz)
        ComNavFreq[fr+4].addMHz(src->getDiff(), isCOM(fr), buf, 0);
    }
    // Send freq change to dataref
    board.send(Radio_DOfs[fr], 2, 0, ComNavFreq[fr+4].BCD());
    // Alt (use string): board.send(Radio_DOfs[fr], 4, buf+1);

    // Refresh display
    refreshR(2, buf);
    // If other display has the same setting, refresh that too
    if(fl == fr) refreshL(2, buf);
}

void On_ChgModeL(ManagedEnc *src)
{
    UNUSED(src);
}

void On_ChgModeR(ManagedEnc *src)
{
    UNUSED(src);
}

/// ===============================
///  EXTERNAL EVENT CALLBACKS
/// ===============================

void OnEvt_FreqChg(CmdEvent *src, char *payload)
{
    uint8_t     nf  = (uint8_t)(src->getTag()->code.hi);
    uint16_t    fv; // = *((uint16_t *)payload);
    uint8_t     recv;

    fv = ASCtoBCD(payload);
    ComNavFreq[nf].setBCD(fv, NO_FRAC, buf, 1);

    // If freq #<nf> is displayed anywhere, send <cbuf>
    // Check on left display
    recv = L_RECV_TYPE;
    if(nf == recv)   refreshL(1, buf);
    else
    if(nf == recv+4) refreshL(2, buf);
    // Check on right display
    recv = R_RECV_TYPE;
    if(nf == recv)   refreshR(1, buf);
    else
    if(nf == recv+4) refreshR(2, buf);
}

void OnEvt_Swap(CmdEvent *src, char *payload)
{
    UNUSED(payload);

    uint8_t recv = (uint8_t)(src->getTag()->code.hi)-N_COM1_SWP;

    // Swap local frequencies
    ComNavFreq[recv].swap(&ComNavFreq[recv+4]);

    // Check if that frq was shown on left / right display
    if(L_RECV_TYPE == recv) refreshL();
    if(R_RECV_TYPE == recv) refreshR();
}

#endif // BOARD_CODE_H
