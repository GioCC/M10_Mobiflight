#include "WireSlave.h"

WireSlave WSlave = WireSlave();
//extern WireSlave WSlave;

// Wrappers to allow passing members as callbacks (UGLY!)
// See: https://isocpp.org/wiki/faq/pointers-to-members#fnptr-vs-memfnptr-types

void cbRequest_wrap(void)   { WSlave.cbRequest(); }
void cbReceive_wrap(int n)  { WSlave.cbReceive(n); }

WireSlave::WireSlave()
{
    buf_s = buf_e = 0;
}

byte WireSlave::getFree(void)
{
    int len;
    len = buf_e; if(buf_e < buf_s) len += WIRE_BUF_LEN;
    len -= buf_s;
    return (byte)len;
}

void WireSlave::open(byte adr)
{
    Wire.begin(adr);
    Wire.onRequest(cbRequest_wrap);
    Wire.onReceive(cbReceive_wrap);
}

void WireSlave::close(void)
{
}

#define inc_wrap(n) { n++; n &= (WIRE_BUF_LEN-1); }
#define add_wrap(a, b) { a += b; a &= (WIRE_BUF_LEN-1); }

byte WireSlave::addSend(byte *data, byte len)
{
    int  new_e = 0;

    if(getFree() < (len+1)) return 0xff;

    add_wrap(new_e, (buf_e + len + 1)); // save new buf_e value to be assigned later
    inc_wrap(buf_e);            // Move to next free pos

    buf[buf_e] = len;           // 1st char is msg length
    inc_wrap(buf_e);            // Move to next free pos

    if(new_e > buf_e) {
        //strncpy(&(buf[buf_e]), data, len);
        strncpy((char *)(buf+buf_e), (char *)data, len);
    } else {
        byte len1;
        len1 = WIRE_BUF_LEN-buf_e;
        //strncpy(&(buf[buf_e]), data, len1);
        strncpy((char *)(buf+buf_e), (char *)data, len1);
        strncpy((char *)buf, (char *)data+len1, len-len1);
    }
    buf_e = new_e;

    return 0;
}

// callback for Wire onRequest() (respond to data request from master)
void WireSlave::cbRequest(void)
{
    if(txEmpty()) return;
    // Maybe: send 1st byte as # of following bytes
    // Acts as a check, and allows to always send something (nothing to reply: send back '\0')
    // to avoid "NACK"s
    // Actually, if the user does not Wire.write() anything, Wire sends back a single '\0' anyway!
    // See twi.c, lines 482...495

    // Note on difference between # of bytes requested by the master and length of reply by the slave:
    /* "The Arduino Slave can write up to 32 bytes with Wire.write() in the requestEvent().
        They will be written in a buffer and the Master can request one or more bytes. Everything is okay if the Master reads less.
        The Slave can also return less than the Master requests. Then the Wire.requestFrom() function returns the actual
        received bytes and Wire.available() also returns the actual received bytes that are waiting in a buffer
        (a receive buffer inside the Wire library).
        [http://forum.arduino.cc/index.php?topic=362256.0]
    */

    // Copy next message to single msg buffer
    byte len;
    byte new_s = 0;

    len = buf[buf_s];
    inc_wrap(buf_s);

    add_wrap(new_s, (buf_s + len + 1));

    if(new_s > buf_s) {
        //strncpy(&(buf[buf_s]), len);
        strncpy((char *)msg, (char *)(buf+buf_s), len);
    } else {
        byte len1;
        len1 = WIRE_BUF_LEN-buf_s;
        //strncpy(msg, &(buf[buf_s]), len1);
        //strncpy(&(msg[len1]), buf, len-len1);
        strncpy((char *)msg, (char *)(buf+buf_s), len1);
        strncpy((char *)msg+len1, (char *)buf, len-len1);
    }
    buf_s = new_s;

    Wire.write((byte *)msg, len);
}

// callback for Wire onReceive() (fetch data received from master)
void WireSlave::cbReceive(int len)
{
    //byte len = Wire.available();
    if (msginLen) {
        // Problem - previous buffer was not read yet and will be lost!
    }
    if(len > WIRE_MSG_LEN) len = WIRE_MSG_LEN;
    for (int n=0; n<len; n++) {
        msgin[n] = Wire.read();
    }
    msginLen = len;
    // TODO
    // ...parse content and do something with data...

}

/** Reference functions from ArdREF:

// MASTER
void ARDref::Slave_init(int slave, int pin, byte mode, char in)    // Used to forward initial values to slave
{
    Wire.beginTransmission(slave);
    Wire.write(255);     // mode (0xFF is init)
    Wire.write(in);      // Input type: <A>nalog, <B>inary, <E>ncoder, <X>-Reset
    Wire.write(pin);     // pin# (+1)
    Wire.write(mode);    // a,b,c (Enc type) for <E> or 0-255 (NSteps) for A
    Wire.write(1);       //
    Wire.endTransmission();

}

// MASTER
void SendToSlave(int slave, byte data_id)   // Send DataRef to slave
{
    if (OUT.isNew[data_id] == 1) {
        XP.ftob.fl_v =  OUT.Dget[data_id];
        Wire.beginTransmission(slave);  // transmit  control parameters to Arduino  #10
        Wire.write(data_id);          // data_out number in list
        Wire.write(XP.ftob.byte_v[0]);
        Wire.write(XP.ftob.byte_v[1]);
        Wire.write(XP.ftob.byte_v[2]);
        Wire.write(XP.ftob.byte_v[3]);
        Wire.endTransmission();
        OUT.isNew[data_id] == 0;
    }  // data sent
}

// MASTER
void ARDref::GetSlave(int id)   // Poll slave for info; if any is available, forward upstream to host
{
    Wire.requestFrom(id, 3);     // request #id Arduino Mega (5 bytes)
    if (Wire.available()) {
        char act = Wire.read();
        byte pin = Wire.read();
        byte val = Wire.read();
        if ( act == 'X') {
            ini_m = 0;    // slave init?
            while (ini_m == 0) {
                GetData(0);
            }
            return;
        }
        if ( act == 'A') {
            outa(id, pin-1,  itoa(val-1, trans2, 10));
        }
        if ( act == 'B') {
            outref(id, pin-1, val-1);
        }
    }
}

// SLAVE
void ARDrefSlave(int adr)   // Wire initialization as Slave
{
    Wire.begin(adr);
    Wire.onRequest(SendData);
    Wire.onReceive(ReadData);
}

// MASTER
void ARDrefScan()       // Scan function for Master
{
    XP.GetData(0 + !XP.ini_m);
    if (XP.board>1 && XP.ini_m==1) {
        for (int i=2; i<=XP.board; i++) {
            XP.GetSlave(i);
        }
    }
}

// SLAVE
void ReadData(int cnt)   // onReceive() callback for Wire (receive data from master)
{
    byte in[5];    	// incoming data from master
    for (int n=0; n<5; n++) {
        in[n] = Wire.read();
    }
    int par = in[0];
    if (in[0] == 255 ) {   // if mode init
        int pin = in[2]-1;
        if (in[1] == 'B' ) {
            IO.pins[pin] = 1;    // set as digital
            pinMode(pin, INPUT_PULLUP);
            if (pin==13) {
                pinMode(pin, INPUT);
            }
        }
        if (in[1] == 'A' ) {
            IO.dz[pin] = in[3];    // set analog d-zone, steps
        }
        if (in[1] == 'E' ) {
            IO.pins[pin] = in[3];     // set encoders, type
            pinMode(pin, INPUT_PULLUP);
            pinMode(pin+1, INPUT_PULLUP);
        }
        if (in[1] == 'X' ) {
            if (pin == 0) {
                XP.ini_m=1;    // init end
            } else  {
                for (int i=0; i<54; i++) {
                    IO.pins[i] = 0;
                    IO.D_pos[i] = 0;
                }
                for (int i=0; i<16; i++) {
                    IO.A_pos[i]=0;
                    IO.dz[i]=0;
                }
            }
        }
    } else {
        for (int i=0; i<4; i++) {
            OUT.ftob.byte_v[i] = in[i+1];    // data to out
        }
        OUT.Dget[par] = OUT.ftob.fl_v;     // save received data
        OUT.isNew[par] = 1;                // has data been changed?
    }
}

// SLAVE
void SendData()     // onRequest() callback for Wire (respond to data request from master)
{
    if (IO.D_ext[0] == 'W') {
        return;
    }
    Wire.write(IO.D_ext);
    IO.D_ext[0] = 'W';  //data success
}


*/
