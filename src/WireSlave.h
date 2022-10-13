

#ifndef WIRESLAVE_H
#define WIRESLAVE_H

#include <Arduino.h>
#include <Wire.h>
//#include "FastArduino.h"

#define WIRE_MSG_LEN    8
#if (defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__))
#define WIRE_BUF_LEN    64
#elif (defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega328__))
#define WIRE_BUF_LEN    32
#elif (defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__))
#define WIRE_BUF_LEN    32
#endif

class WireSlave
{
    public:
        WireSlave(void);

        void open(byte adr);
        void close(void);

        byte addSend(byte *data, byte len);  // Add <len> chars from <data> to output queue (will be transmitted when requested by the master)

        byte txEmpty(void) { return (buf_s==buf_e); }
        byte getFree(void);

        byte rxCount(void) { return msginLen; }
        void rxClear(void) { msginLen = 0; }

        void cbRequest(void);           // Callback for Wire.onRequest
        void cbReceive(int nbytes);     // Callback for Wire.onReceive

    private:

        byte buf[WIRE_BUF_LEN];  // WIRE_BUF_LEN < 256, otherwise increase index type size
        volatile byte buf_s;     // index of first used char in buffer
        volatile byte buf_e;     // index of last used char in buffer

        volatile byte msginLen;

        volatile byte msg[WIRE_MSG_LEN];     // direct tx buffer
        volatile byte msgin[WIRE_MSG_LEN];   // rx buffer - could be shared with outgoing message as long as overlaps are prevented

};

#endif // WIRESLAVE_H
