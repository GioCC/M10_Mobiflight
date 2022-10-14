/*

 Test program

*/

// IMPORTANTE
// Per errori di linker tipo 'multiple definition of `__ltsf2' etc,
// vedi: http://www.avrfreaks.net/forum/multiple-definition-error-c-atof
// (aggiunta opzioni "-lc -lm -lc")

#include <Arduino.h>
//================================
//   Compile-time configuration
#include "config_build.h"
//================================
//#include <ArdSim_Interface.h>          //  -- ArdSim library

#define DW(a)   Serial.write(a)
#define DSPC    Serial.write(' ')
#define DP(a)   Serial.print(a)
#define DPN(a)  Serial.println(a)

#if !defined(CONTROLLER)    // Controller requires no HW interface resources
M10board board;
#endif

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

//MCP         MCPIO(0,10);
//uint16_t    digIO;
//uint16_t    digIO2;

/*
//  Control elements
#define CABIN_PIN 10
ManagedEnc  testEnc  = ManagedEnc(1, 1234, 5678, NULL, NULL, NULL, NULL, NULL, NULL);
ButtonEnc   testBEnc = ButtonEnc(1, 4321, 8765, NULL, NULL, NULL);
ButtonGrp   testGrp  = ButtonGrp(CABIN_PIN,"cabin", NULL, lights_on, lights_off, NULL, 0, &lights, 9);
Button      testBtn  = Button(5, 0, "pippo", 0);

ButtonGrp cabin_lights = ButtonGrp();
*/
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
#include "board_code.h"
//================================


//----------------------
void setup() {

    //BoardNumber 1;
    board.ModeIO_L(0xFFFF);            // All inputs
    board.ModePU_L(0xFFFF);            // Pull-ups on all inputs
#ifdef BANK2
    board.ModeIO_H(0xFFFF);            // All inputs
    board.ModePU_H(0xFFFF);            // Pull-ups on all inputs
#endif

    // Board base configuration
    board.setBoardCfg((M10board_cfg *)&BOARD_CFG);
    // Custom app config
    board_init();
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

