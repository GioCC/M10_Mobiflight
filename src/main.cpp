/*

 Test program

*/

// IMPORTANTE
// Per errori di linker tipo 'multiple definition of `__ltsf2' etc,
// vedi: http://www.avrfreaks.net/forum/multiple-definition-error-c-atof
// (aggiunta opzioni "-lc -lm -lc")

#include <Arduino.h>
#include "M10board.h"

#define DW(a)   Serial.write(a)
#define DSPC    Serial.write(' ')
#define DP(a)   Serial.print(a)
#define DPN(a)  Serial.println(a)

M10board board;

// Fn prototypes
void lights_on(ButtonGrp *but);
void lights_off(ButtonGrp *but);

// User vars
int lights;

// Management vars
unsigned long counter[4];
unsigned long ticker;
byte toggle1 = 1;
unsigned long ticker2;
byte toggle2 = 1;
char dispbuf[8];

M10board_cfg    cfg;

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
//#include "board_code.h"
//================================


//----------------------
void setup() {

// FOR EACH (installed) BOARD:
    //BoardNumber 1;
    board.setIOMode(0,0xFFFF);            // All inputs
    board.setPUMode(0,0xFFFF);            // Pull-ups on all inputs
    board.setIOMode(1,0xFFFF);            // All inputs
    board.setPUMode(1,0xFFFF);            // Pull-ups on all inputs

    // Board base configuration
    board.setBoardCfg((M10board_cfg *)&BOARD_CFG);
    // Custom app config
    //board.init();
    // Board configuration adjustments after custom cfg
    board.setBoardPostCfg();

    for(byte i=1; i<5; i++) {
        board.dispClear(i);
        board.dispWidth(i, 6);
        board.dispIntensity(i, 15);
        board.dispShutdown(i, false);
    }
    board.dispWrite(1, (byte *)"012345", 0x08);
    board.dispWrite(2, (byte *)"ABCDE ", 0x08);
    board.dispWrite(3, (byte *)"67890 ", 0x08);
    board.dispWrite(4, (byte *)"F0123 ", 0x08);
    board.dispTransmit(0);
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

//--------------------------
void loop() {

    board.ScanInOut();

    if(millis()>=ticker) {
        ticker = millis()+200;

        counter[1] = board.Din.val()[0]; //.valL() & 0xFFFF;
        counter2buf(0);
        board.dispWrite(1, (byte *)dispbuf, 0x04);
        counter2buf(1);
        board.dispWrite(2, (byte *)dispbuf, 0x04);

        board.dispTransmit(0);

    }
    if(millis()>=ticker2) {
        ticker2 = millis()+1;
        //f_FastdigitalWrite(8, (toggle2&0x01));
        toggle2++;
    }
}
//--- End -------------------

