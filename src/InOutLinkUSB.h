/********************************************************************
*  InOutLinkUSB.h
*
*    Manages a stub for a serial link channel on USB
*
*    Stripped-down version of FSPanels-InOutLink
*    Eth- and I2C-related code has been disabled but left in place
*
*  Project:  FSPanels-M10
*  Hardware: Arduino M10-series panels
*  Author:   Giorgio CROCI CANDIANI
*  Date:     Apr-2017
*
********************************************************************/
#ifndef INOUTLINKUSB_H
#define INOUTLINKUSB_H

#include <Arduino.h>
#include <FastArduino.h>

///#include <Ethernet.h>
//#include <Ethernet.h>         // for W5x00-based cards
//#include <UIPEthernet.h>      // for ENC28J60-based cards
//#include <Wire.h>

#define UNUSED(x) ((void)(x))

#define UPSTREAM    1
#define DNSTREAM    2

//#define OPEN_ETH  0x04
#define OPEN_USB  0x02
//#define OPEN_I2C  0x01

#define MSG_BUFSIZE 32
#define MULTIBUF

// ADJUST IPS HERE BELOW
//#define ARD_MAC     0xDE,0xAD,0xBE,0xEF,0xFE,0xED
//#define ARD_IP      0,0,0,0
//#define ARD_PORT    5080
//#define HOST_IP     0,0,0,0       // Normally unused, will be transmitted by the host through first UDP packet

//#define LINKUP_ETH  0x04
//#define LINKUP_USB  0x02
//#define LINKUP_I2C  0x01
//#define LINKDN_I2C  0x10

//#define IAmProxy    (channelCfg & (LINKUP_ETH|LINKUP_USB))
//#define IAmPod      (channelCfg & LINKUP_I2C)
//#define IHaveI2C    (channelCfg & (LINKUP_I2C|LINKDN_I2C))

/// This class handles received messages as follows:
///
/// On serial channels:
/// - all first chars up to (and excluding) a header are discarded;
/// - chars are collected;
/// - when a terminator is recognized, the message is made available to the client
///   (and the buffer is marked as occupied).
/// If the serial buffer capacity is exceeded during reception, the message content is discarded up to the first
/// hdr (or after the first terminator, if hdr=='\0').
///
/// On packet-oriented channels (UDP):
/// - if a packet is not starting with hdr (if !=0) AND ending with the terminator, it is discarded altogether;
/// - otherwise, upon arrival it is made available.
///
/// Terminators are replaced by a '\0' upon reception.
/// If '\0' is defined as hdr, the header is NOT checked.

/// Message polling:
///
/// > queued send requests upstream are sent immediately;
/// > incoming requests are polled and then detected through incomingMsgUp/Dn(..).
///
/// Function pollDownstream(..) sends queued requests downstream (?);
///
/// Typical usage pattern:
///
///   pollUpstream();
///   while(l = incomingUpstream()) {
///       m = fetchMsg();
///       // ...process m
///   }
///   (similarly for Downstream)

class cbuf
{
public:
    cbuf(byte bufsize);       // Fake argument
    ~cbuf() {};
    char buf[MSG_BUFSIZE];     // Use static allocation for embedded.
    byte len;
    // char hdr;
    // char term;
}; // class cbuf


class InOutLink
{
private:

/*
    byte channelCfg;               // marks open channels
*/
    static char hdrChar;           // Character to be considered as header for messages
    static char termChar;          // Character to be considered as terminator for messages

    byte ownUnitAdr;

    cbuf *bUSB;
    cbuf *bOut;
/*
    cbuf *bEth;
    static cbuf *bI2CIn;
    //static cbuf *bI2COut;

    // ETH vars
    EthernetUDP     _UDPlink;
    byte            _MACadr[6];     // MAC address for Arduino
    IPAddress       _OwnIP;		    // IP addr for Arduino
    unsigned int    _OwnPort;       // UDP port for Arduino
    IPAddress       _HostIP; 	    // IP addr for PC Host
    unsigned int    _HostPort;      // UDP port for PC Host

    byte _I2Cadr;

    // static void I2CSendData();       // I2C request is not used
    static void I2CReadData(int cnt);
*/
    static void strRead(Stream *s, cbuf *buf);     // aux function for pollUpstream()

    void baseInit();      // helper for constructors

protected:
    // none

public:
/*
    static const byte Ard_MAC[6];
    static const byte Ard_IP[4];
    static const byte Host_IP[4];
*/
    InOutLink();
    InOutLink(byte streamCfg, byte unitAdr, byte hdr, byte term);
    ~InOutLink();

    void init(byte streamCfg, byte unitAdr, byte hdr, byte term);

    byte getUnitAdr()
    {
        return ownUnitAdr;
/*
        if(IAmPod) {
            return ownUnitAdr;
        }
        if(IAmProxy) {
            return 1;
        }
        return 0x80;
*/
    }

    /// Set header and terminator char (if those passed with the constructor are not OK anymore).
    void setHdr(byte hdr)
    {
        hdrChar = hdr;
    }
    void setTerm(byte term)
    {
        termChar = term;
    }

    /// These functions open (and activate) or close up- and downstream channels
    /// The configuration of the channels opened determines the role of the node.
    /// If any host channel is opened (Eth or USB), an I2C channel either already open or opened
    /// later will be used for downstream and the node is a proxy;
    /// If an I2C channel is opened and no host channels are open (or as soon as all are closed),
    /// the I2C channel is used for upstream and the node is a pod.

    //void openUpEth(char *mac, byte *ipadr, byte *ipx, unsigned int UDPport);   // Open ETH channel upstream
    void openUSB();     // Open USB channel upstream
    void closeUSB();
/*
    void openEth();     // Open ETH channel upstream
    void closeEth();

    void openI2C();     // Open I2C channel
    void closeI2C();
    void resetI2C();   // if the channel config changes, reopen I2C to adjust to possible address change
*/

    /// Poll up/downstream channels for incoming requests
    /// src is the source channel (UPSTREAM or DOWNSTREAM).
    /// WARNING: these fns overwrite buffer contents, so the client had better
    /// make sure that he has already detected and consumed all previous available
    /// messages (currently, BOTH up and downstream, since buffer is shared).
    /// Returns incoming message length (excluding terminator), or 0 if none.
    /// The terminator in the incoming message is replaced by '\0'.
    /// The origin channel for the incoming message is not currently available
    /// (nor should it be important).

    byte poll(byte src=UPSTREAM+DNSTREAM);    // Poll up/downstream channels for incoming messages

    /// Check if there are available messages awaiting processing.
    /// src is the source channel (UPSTREAM or DOWNSTREAM).
    /// These fn just checks EXISTING messages, they do not update nor overwrite anything.
    /// Returns: length of next available message, or 0 if none.

    byte incomingMsg(byte src=UPSTREAM+DNSTREAM);

    /// If incomingMsg() is positive, following function can be used to fetch the corresponding message.
    /// src is the source channel (UPSTREAM or DOWNSTREAM).
    /// It returns a pointer to the message buffer and marks the message as consumed (there by marking the buffer
    /// as free). Even if the buffer is marked as free, it won't be overwritten until poll()
    /// is called again; however, subsequent calls to incoming..() and fetchMsg() will detect/fetch
    /// a different message (if available).

    char *fetchMsg(byte src=UPSTREAM+DNSTREAM);

    /// Outbound Transmission
    /// If the unit is a pod, the transmission can only be upstream (and the specified dest address
    /// is ignored, since it is necessarily 0x01);
    /// If the unit is the proxy, an address 0xFF means that the transmission is upstream,
    /// otherwise a (pod) destination address must be specified.

    byte sendMsg(char *msg=0, byte adr=0xFF);      // Sends message up- or downstream


    /// Sends raw message to serial (upstream); message must be terminated with '\0'.
    /// FOR DEBUG PURPOSES ONLY
    void sendRaw(char *msg, byte crlf=0);
// shortcuts for debug printing
#define dp(b) sendRaw(b, 1)
#define dpc(b) sendRaw(b, 0)

    char *outBuf()  { return (bOut->buf + 1); }
    byte outMaxSize() { return (MSG_BUFSIZE-2); }

#define sendMsgUp(b) sendMsg(b, 0xFF)
#define sendMsgDn(b, a) sendMsg(b, a)

};

#endif // INOUTLINKUSB_H
