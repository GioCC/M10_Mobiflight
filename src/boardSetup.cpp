// =======================================================================
// @file        boardSetup.cpp
// @file        main.cpp
//
// @project     M10_Mobiflight
//
// @details     Board setup related code
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-15
// @modifiedby  GiorgioCC - 2023-09-29 17:01
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


#include "main.h"

// =================================
//  Local vars
// =================================
uint16_t        ConfigBoardFlags;

// =================================
//  Functions
// =================================

bool isBoardAttached(uint8_t slot)
{
    return ((ConfigBoardFlags & (1<<slot))!=0);
}

uint16_t readBoardSelector(void)
{
    //TODO: Check CPOL!
    // We may setup the lines here as well
    pinMode(CFG_SR_CLK, OUTPUT); digitalWrite(CFG_SR_CLK, HIGH);
    pinMode(CFG_SR_LAT, OUTPUT); digitalWrite(CFG_SR_LAT, HIGH);
    pinMode(CFG_SR_DIN, INPUT);

    digitalWrite(CFG_SR_LAT, LOW);
    ConfigBoardFlags = (~shiftIn(CFG_SR_DIN, CFG_SR_CLK, MSBFIRST)) << 8;
    ConfigBoardFlags |= ~shiftIn(CFG_SR_DIN, CFG_SR_CLK, MSBFIRST);
    digitalWrite(CFG_SR_LAT, HIGH);
}

void boardSetup(void)
{
    // uint8_t  slot = 0;
    
    ConfigBoardFlags = readBoardSelector(); 

    //TODO For each board assigned to a slot: initialize board object (Board[n])
    // with proper parameters

    if(isBoardAttached(0)) {
        // Type 1: Radio 1
        //....
    }
    if(isBoardAttached(1)) {
        // Type 1: Radio 2
        //....
    }
    if(isBoardAttached(2)) {
        // Type 2: ADF/DME
        //....
    }
    if(isBoardAttached(3)) {
        // Type 3: XPDR/OBS/CLK
        //....
    }
    if(isBoardAttached(4)) {
        // Type 4: AP
        //....
    }
    if(isBoardAttached(5)) {
        // Type 9: EFIS
        //....
    }
    if(isBoardAttached(6)) {
        // Type 5: Radio LCD
        //....
    }
    if(isBoardAttached(7)) {
        // Type 6: Multi LCD
        //....
    }
    if(isBoardAttached(8)) {
        // Type 7: AP-LCD
        //....
    }
    if(isBoardAttached(9)) {
        // Type 8: Kbd (AP)
        //....
    }
    if(isBoardAttached(10)) {
        // Type 8: Kbd (Radio (Audio))
        //....
    }
    if(isBoardAttached(11)) {
        // Type 8: Kbd (Aux)
        //....
    }
}

// =================================
//  Test stuff
// =================================

unsigned long counter[4];
unsigned long ticker;
byte toggle1 = 1;
unsigned long ticker2;
byte toggle2 = 1;

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
    // board.setBoardCfg((M10BoardConfig *)&BOARD_CFG);
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

