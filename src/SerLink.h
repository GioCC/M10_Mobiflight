/********************************************************************
*  InOutLinkUSB.h
*
*    Manages a stub for a serial link channel on USB
*
*    Stripped-down version of FSPanels-SerLink
*    Eth- and I2C-related code has been disabled but left in place
*
*  Project:  FSPanels-M10
*  Hardware: Arduino M10-series panels
*  Author:   Giorgio CROCI CANDIANI
*  Date:     Apr-2017
*
********************************************************************/
#ifndef SERLINK_H
#define SERLINK_H

#include <Arduino.h>
#include <FastArduino.h>

#define UNUSED(x) ((void)(x))

#define MSG_BUFSIZE 32
#define MULTIBUF

/// This class handles received messages as follows:
///
/// - all first chars up to (and excluding) a header are discarded;
/// - chars are collected;
/// - when a terminator is recognized, the message is made available to the client
///   (and the buffer is marked as occupied).
/// If the serial buffer capacity is exceeded during reception, the message content is discarded up to the first
/// hdr (or after the first terminator, if hdr=='\0').
///

class cbuf
{
public:
    cbuf(byte bufsize);       // Fake argument
    ~cbuf() {};
    char buf[MSG_BUFSIZE];     // Use static allocation for embedded.
    byte len;
}; // class cbuf


class SerLink
{
private:

    static char hdrChar;           // Character to be considered as header for messages
    static char termChar;          // Character to be considered as terminator for messages

    byte ownUnitAdr;

    cbuf *bUSB;
    cbuf *bOut;
    static void strRead(Stream *s, cbuf *buf);     // aux function for pollUpstream()

    void baseInit();      // helper for constructors


public:
    SerLink();
    SerLink(byte streamCfg, byte unitAdr, byte hdr, byte term);
    ~SerLink();

    void init(byte streamCfg, byte unitAdr, byte hdr, byte term);

    /// Set header and terminator char (if those passed with the constructor are not OK anymore).
    void setHdr(byte hdr)   { hdrChar = hdr; }
    void setTerm(byte term) { termChar = term; }

    void open(void);
    void close(void);

    byte poll(void);    // Poll up/downstream channels for incoming messages

    /// Check if there are available messages awaiting processing.
    /// These fn just checks EXISTING messages, they do not update nor overwrite anything.
    /// Returns: length of next available message, or 0 if none.
    byte incomingMsg(void);

    /// If incomingMsg() is positive, following function can be used to fetch the corresponding message.
    /// It returns a pointer to the message buffer and marks the message as consumed (there by marking the buffer
    /// as free). Even if the buffer is marked as free, it won't be overwritten until poll()
    /// is called again; however, subsequent calls to incoming..() and fetchMsg() will detect/fetch
    /// a different message (if available).
    char *fetchMsg(void);

    /// Outbound Transmission
    byte sendMsg(char *msg=0);


    /// Sends raw message to serial; message must be terminated with '\0'.
    /// FOR DEBUG PURPOSES ONLY
    void sendRaw(char *msg, byte crlf=0);
    // shortcuts for debug printing
    #define dp(b) sendRaw(b, 1)
    #define dpc(b) sendRaw(b, 0)

    char *outBuf()  { return (bOut->buf + 1); }
    byte outMaxSize() { return (MSG_BUFSIZE-2); }
};

#endif // SERLINK_H
