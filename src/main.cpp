// =======================================================================
// @file        main.cpp
//
// @project     M10_Mobiflight
//
// @details     Main application body
//              (Currently just a test rig)
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-15
// @modifiedby  GiorgioCC - 2023-09-26 18:11
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


#include "main.h"

void crashHandler(void);

// =================================
//  Global vars
// =================================

memPool<MEM_POOL_SIZE>  pool(crashHandler);
M10board                Board[Config::MAX_BOARDS];

// =================================
//  Local vars
// =================================

// =================================
//  Functions
// =================================

// Landing spot for irrecoverable system errors
void crashHandler(void) 
{
    // No exit.
    // If available, flash a LED or something.
}

// =================================
//  Test stuff
// =================================

unsigned long counter[4];
unsigned long ticker;
byte toggle1 = 1;
unsigned long ticker2;
byte toggle2 = 1;

M10board_cfg    cfg;
M10board        board;

char dispbuf[8];
// Converts a counter in a displayable string for LED displays
char *counter2buf(byte cntno) {
    unsigned long cc = counter[cntno&0x03];
    register byte h;
    for(byte i=0; i<8; i++) {
        h = cc&0x0F;
        if(h>9) h+=7;
        dispbuf[7-i]='0'+h;
        cc>>=4;
    }
    return dispbuf;
}

void SANDBOX(void) {

    void* bp;
    bp = pool.reserve(sizeof(ButtonBas));
    ButtonBas::make(bp).pin(1, true); //.addTo(mgr);
}

//===========================================================================

void setup() {

// FOR EACH (installed) BOARD:
    //BoardNumber 1;
    board.setIOMode(0,0xFFFF);            // All inputs
    board.setPUMode(0,0xFFFF);            // Pull-ups on all inputs
    board.setIOMode(1,0xFFFF);            // All inputs
    board.setPUMode(1,0xFFFF);            // Pull-ups on all inputs

    // Board base configuration
    // board.setBoardCfg((M10board_cfg *)&BOARD_CFG);
    // Custom app config
    //board.init();
    // Board configuration adjustments after custom cfg
    board.setBoardPostCfg();

    for(byte i=1; i<5; i++) {
        // board.dispClear(i);
        // board.dispWidth(i, 6);
        // board.dispIntensity(i, 15);
        // board.dispShutdown(i, false);
    }
    // board.dispWrite(1, (byte *)"012345", 0x08);
    // board.dispWrite(2, (byte *)"ABCDE ", 0x08);
    // board.dispWrite(3, (byte *)"67890 ", 0x08);
    // board.dispWrite(4, (byte *)"F0123 ", 0x08);
    // board.dispTransmit(0);
// END FOR

    counter[0] = 0x00000000;
    counter[1] = 0x00000000;
    counter[2] = 0x00000000;
    counter[3] = 0x00000000;

    ticker = millis()+1000;
    ticker2 = millis()+1;

    //Serial.begin(115200);
    Serial.begin(19200);
}

//===========================================================================

void loop() {

    board.ScanInOut();

    if(millis()>=ticker) {
        ticker = millis()+200;

        counter[1] = board.getIns(0);
        counter2buf(0);
        // board.dispWrite(1, (byte *)dispbuf, 0x04);
        counter2buf(1);
        // board.dispWrite(2, (byte *)dispbuf, 0x04);
        // board.dispTransmit(0);
    }
    if(millis()>=ticker2) {
        ticker2 = millis()+1;
        //f_FastdigitalWrite(8, (toggle2&0x01));
        toggle2++;
    }
}

//--- End -------------------

