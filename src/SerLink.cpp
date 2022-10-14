/********************************************************************
*  SerLink.cpp
*
*    Manages a stub for a serial link channel
*
*  Project:  FSPanels-M10
*  Hardware: Arduino M10-series panels
*  Author:   Giorgio CROCI CANDIANI
*  Date:     Apr-2017
*
********************************************************************/
#include "SerLink.h"

cbuf::cbuf(byte bsize)
{
    // buf size argument unused, use static allocation for embedded
    // hdr = term = '\0';
}

/// Allocate global buffers to avoid dynamic allocation
cbuf bbUSB = cbuf(0);   // USBONLY
/*
cbuf bbEth = cbuf(0);
#ifdef MULTIBUF
cbuf bbUSB = cbuf(0);
#endif
cbuf bbI2CIn = cbuf(0);
//cbuf bbI2COut = cbuf(0);
*/
cbuf bbOut = cbuf(0);

/// Space allocation for static member vars

/*
const byte SerLink::Ard_MAC[6] = {ARD_MAC};
const byte SerLink::Ard_IP[4]  = {ARD_IP};
const byte SerLink::Host_IP[4]   = {HOST_IP};

cbuf* SerLink::bI2CIn;
*/
char SerLink::hdrChar;
char SerLink::termChar;


SerLink::SerLink(byte streamCfg, byte unitAdr, byte hdr, byte term)
{
    baseInit();
    init(streamCfg, unitAdr, hdr, term);
}

SerLink::SerLink()
{
    baseInit();
}

SerLink::~SerLink()
{
    close();
}

void
SerLink::baseInit()
{
    hdrChar = '>';
    termChar = 0x0D;
    ownUnitAdr = 0x01;
    Serial.begin(115200);   // used for USB and/or for debug
    bUSB = &bbUSB;
    bOut = &bbOut;
}

void
SerLink::init(byte streamCfg, byte unitAdr, byte hdr, byte term)
{
    hdrChar = hdr;
    termChar = term;
    ownUnitAdr = unitAdr;
    bOut->buf[0] = hdr;
    open(); // USBONLY
}

// Open USB serial channel upstream
void
SerLink::open()
{
}

void
SerLink::close()
{
}

void
SerLink::strRead(Stream *str, cbuf *buf)
{
    buf->len = 0;
    int sz;
    // Strip non-hdr chars + header at the beginning
    if(hdrChar) {
        while(str->available() && str->read() != hdrChar){};
    }
    if(str->available()) {
        if(termChar) {
            sz = str->readBytesUntil(termChar, buf->buf, MSG_BUFSIZE);  // terminator is _not_ read
        } else {
            sz = str->readBytes(buf->buf, MSG_BUFSIZE);
        }
        str->flush();
        if(sz<MSG_BUFSIZE) {
            buf->buf[sz]='\0';
        } else {
            // no term found (or found beyond last buf pos, which we have to assume is too late)
            sz = 0;
            buf->buf[MSG_BUFSIZE-1] = '\0'; // make sure buffer is terminated anyway
        }
        buf->len = sz;
    }
}

/// Poll upstream channels for incoming requests
/// Message is stored in the internal buffers;
/// header and terminator are stripped.
byte
SerLink::poll(void)
{
    // Inquire USB serial channel (only if buffer not occupied)
    if(bUSB->len == 0) strRead(&Serial, bUSB);
    return bUSB->len;
}

/// Check if a message from the up/downstream channel is available
/// Returns the length of the next available message.
byte
SerLink::incomingMsg(void)
{
    return bUSB->len;
}

/// Fetch next available message;
/// returns a pointer to the message buffer (0-terminated).
/// The message is considered as consumed by setting its length to 0;
/// subsequent calls to getMsgLen() will return either 0 or the length
/// of the next available message.
char *
SerLink::fetchMsg()
{

    if(bUSB->len != 0) {
        bUSB->len = 0;
        return bUSB->buf;
    }
    return (char *)0;
}

/// Sends message
/// Message contains neither header nor terminator;
/// it must be terminated with '\0'
byte
SerLink::sendMsg(char *msg)
{
    register byte ll;
    register char *bb;

    // if buf adr=0, assume msg was placed in own buffer
    bb = ((msg != (char *)0) ? msg : (bOut->buf + 1));

    bOut->len = ll = strlen(bb) + 2;
    if(ll > MSG_BUFSIZE) {
        return 0xFF;
    }

    if(bb != (bOut->buf + 1)) {
        strcpy((bOut->buf + 1), bb);
    }

    bOut->buf[ll-1] = termChar;

    Serial.write((uint8_t *)bOut->buf, ll);     // USBONLY
    bOut->len = 0;
    return 0;
}

/// Sends raw message to serial (upstream)
/// Message must be terminated with '\0'
/// FOR DEBUG PURPOSES ONLY
void
SerLink::sendRaw(char *msg, byte crlf)
{
    Serial.write((uint8_t *)msg, strlen(msg));
    if(crlf) {
        Serial.write(0x0D);
        Serial.write(0x0A);
    }
}

// End SerLink.cpp
