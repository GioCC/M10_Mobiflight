/*

 Test program

*/

// IMPORTANTE
// Per errori di linker tipo 'multiple definition of `__ltsf2' etc,
// vedi: http://www.avrfreaks.net/forum/multiple-definition-error-c-atof
// (aggiunta opzioni "-lc -lm -lc"


//================================
//   Compile-time configuration
#include "config_build.h"
//================================
#include <ArdSim_Interface.h>          //  -- ArdSim library
//#include <Arduino.h>
//#include "bitmasks.h"
//#include "FastArduino.h"
//#include "LedControlMod.h"
//#include "MCP23S17.h"
//#include "EncoderM10.h"
//#include "LEDdisplay.h"


#if !defined(CONTROLLER)
M10board board = M10board(2,2,2);   // 2 disp on line1, 2 disp on line 2, [2 IO banks (unused)]
#endif

unsigned long counter[4];
unsigned long ticker;
byte toggle1 = 1;
unsigned long ticker2;
byte toggle2 = 1;

char dispbuf[8];

//MCP         MCPIO(0,10);
//uint16_t    digIO;
//uint16_t    digIO2;

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

//================================

//----------------------
void setup() {

    //BoardNumber 1;

    board.digModeIO(0xFFFF);            // All inputs
    board.digModePU(0xFFFF);            // Pull-ups on all inputs

    for(byte i=1; i<5; i++) {
        board.DispClear(i);
        board.DispScanLimit(i, 7);
        board.DispIntensity(i, 15);
        board.DispShutdown(i, false);
    }
    board.DispWrite(1, (byte *)"01234567", 0x08);
    board.DispWrite(2, (byte *)"  ABCDE ", 0x08);
    board.DispWrite(3, (byte *)"  67890 ", 0x08);
    board.DispWrite(4, (byte *)"  F0123 ", 0x08);
    board.DispTransmit(0);

    counter[0] = 0x00000000;
    counter[1] = 0x00000000;
    counter[2] = 0x00000000;
    counter[3] = 0x00000000;

    ticker = millis()+1000;
    ticker2 = millis()+1;
    //pinMode(8, OUTPUT);

/*
    MCPIO.begin();
    MCPIO.pinMode(0xFFFF);
    MCPIO.pullupMode(0xFFFF);
    MCPIO.inputInvert(0xFFFF);
*/
    Serial.begin(115200);

//    digIO = MCPIO.digitalRead();
//    Serial.println(digIO, 2);
}

//--------------------------
void loop() {

    //sim.SimScan(); //ArdSimScan;

    board.ScanInOut();

    if(millis()>=ticker) {
        ticker = millis()+200;

        //counter[0] += 0x10;
        //counter[1] += 0x00010000;
        //counter[2] -= 0x10;
        //counter[3] -= 0x00010000;
        counter[1] = board.digReadAll() & 0xFFFF;
        counter2buf(0);
        board.DispWrite(1, (byte *)dispbuf, 0x04);
        counter2buf(1);
        board.DispWrite(2, (byte *)dispbuf, 0x04);

        /*
        if(toggle1&0x01) {
            //board.DispSetChar(1, 1, '4', true);
        } else {
            //board.DispSetChar(1, 1, '7', true);
        }
        toggle1++;
        */

        board.DispTransmit(0);

/*
        digIO = MCPIO.digitalRead();
        if(digIO != digIO2) {
            digIO2 = digIO;
            Serial.println(digIO, 2);
        }
*/
        //Serial.println(counter[0]);
    }
    if(millis()>=ticker2) {
        ticker2 = millis()+1;
        //f_FastdigitalWrite(8, (toggle2&0x01));
        toggle2++;
    }
}
//--- End -------------------

