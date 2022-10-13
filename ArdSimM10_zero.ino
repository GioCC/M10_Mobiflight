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

unsigned long counter[4];
unsigned long ticker;
unsigned long ticker2;
byte toggle = 1;

char dispbuf[8];

char *counter2buf(byte cntno) {
    unsigned long cc = counter[cntno&0x03];
    for(byte i=0; i<8; i++) {
        dispbuf[7-i]='0'+(cc&0x0F);
        cc>>=4;
    }
    return dispbuf;
}

//================================

//----------------------
void setup() {

    counter[0] = 0x00000000;
    counter[1] = 0x00000000;
    counter[2] = 0x00000000;
    counter[3] = 0x00000000;

    ticker = millis()+1000;
    ticker2 = millis()+1;
    pinMode(8, OUTPUT);
    Serial.begin(115200);
}

//--------------------------
void loop() {

    //sim.SimScan(); //ArdSimScan;

    if(millis()>=ticker) {
        ticker = millis()+500;
        counter[0] += 1;
        counter[1] += 0x00010000;
        counter[2] -= 1;
        counter[3] -= 0x00010000;
        Serial.println(counter[0]);
    }
    if(millis()>=ticker2) {
        ticker2 = millis()+1;
        digitalWrite(8, (toggle&0x01));
        toggle++;
    }
}
//--- End -------------------

