/********************************************************************
*  InOutLinkUSB.cpp
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
#include "InOutLinkUSB.h"

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
const byte InOutLink::Ard_MAC[6] = {ARD_MAC};
const byte InOutLink::Ard_IP[4]  = {ARD_IP};
const byte InOutLink::Host_IP[4]   = {HOST_IP};

cbuf* InOutLink::bI2CIn;
*/
char InOutLink::hdrChar;
char InOutLink::termChar;


InOutLink::InOutLink(byte streamCfg, byte unitAdr, byte hdr, byte term)
{
    baseInit();
    init(streamCfg, unitAdr, hdr, term);
}

InOutLink::InOutLink()
{
    baseInit();
}

InOutLink::~InOutLink()
{
    closeUSB();
/*
    if(channelCfg & LINKUP_ETH) {
        closeEth();
    }
    if(channelCfg & LINKUP_USB) {
        closeUSB();
    }
    if(channelCfg & (LINKUP_I2C||LINKDN_I2C)) {
        closeI2C();
    }
*/
}

void
InOutLink::baseInit()
{
    hdrChar = '>';
    termChar = 0x0D;
    ownUnitAdr = 0x01;

    Serial.begin(115200);   // used for USB and/or for debug

    bUSB = &bbUSB;  // USBONLY:
/*
    bEth = &bbEth;  //= new cbuf(0);
#ifdef MULTIBUF
    bUSB = &bbUSB;  //= new cbuf(0);
#else
    bUSB = &bbEth;  //= bEth;
#endif
    bI2CIn = &bbI2CIn;      // if(bI2CIn == 0) bI2CIn = new cbuf(0);
    // bI2COut = &bbI2COut;    // if(bI2COut == 0) bI2COut = new cbuf(0);
*/
    bOut = &bbOut;
}

void
InOutLink::init(byte streamCfg, byte unitAdr, byte hdr, byte term)
{
    hdrChar = hdr;
    termChar = term;
    ownUnitAdr = unitAdr;

    bOut->buf[0] = hdr;

    openUSB();  // USBONLY

/*
    if(streamCfg & OPEN_ETH) {
        openEth();
    }
    if(streamCfg & OPEN_USB) {
        openUSB();
    }
    if(streamCfg & OPEN_I2C) {
        openI2C();
    }
*/
}

// Open USB serial channel upstream
void
InOutLink::openUSB()
{
/*
    if(channelCfg & LINKUP_I2C) {
        channelCfg |= LINKDN_I2C;
        channelCfg &= (~LINKUP_I2C);
        resetI2C();
    }
    // delay(500);
    // Serial.begin(115200);    // done in base_init, used for debug
    channelCfg |= LINKUP_USB;
    // host channel just opened: if I2C was opened upstream, now it becomes downstream
*/
}

void
InOutLink::closeUSB()
{
/*
    // Serial.End();
    channelCfg &= (~LINKUP_USB);
    if((channelCfg & (LINKUP_ETH|LINKDN_I2C))==LINKDN_I2C) {
        // no more host channels open, but I2C open: I2C goes from downstream to up
        channelCfg &= (~LINKDN_I2C);
        channelCfg |= LINKUP_I2C;
        resetI2C();
    }
*/
}

/*
// Open ETH channel upstream
void
//InOutLink::openEth(char *mac, byte *ip_ard, byte *ip_pc, unsigned int UDPport)
InOutLink::openEth()
{
    channelCfg |= LINKUP_ETH;
    // host channel just opened: if I2C was opened upstream, now it becomes downstream
    if(channelCfg & LINKUP_I2C) {
        channelCfg |= LINKDN_I2C;
        channelCfg &= (~LINKUP_I2C);
        resetI2C();
    }

    for(byte i=0; i<6; i++) {
        _MACadr[i] = Ard_MAC[i]; //mac[i];
    }
    for(byte i=0; i<4; i++) {
        _OwnIP[i] = Ard_IP[i]; //ip_ard[i];
        // _HostIP[i] = Host_IP[i]; // ip_pc[i];
        _HostIP = INADDR_NONE; // Will be read from first incoming message
    }
    // No address (or broadcast) supplied
    if (_OwnIP[3]==0) {
        // get from DHCP
        Ethernet.begin(_MACadr);
        // (should check return addr for DHCP success!)
        _OwnIP = Ethernet.localIP();
    } else {
        Ethernet.begin(_MACadr, _OwnIP);
    }

    _OwnPort = ARD_PORT; //UDPport;
    _UDPlink.begin(_OwnPort);


    if (_HostIP[4]!=0) {
        // ... if a host IP is already known, send init sequence to host
    }

}

void
InOutLink::closeEth()
{
    _UDPlink.stop();
    channelCfg &= (~LINKUP_ETH);
    if((channelCfg & (LINKUP_USB|LINKDN_I2C))==LINKDN_I2C) {
        // no more host channels open, but I2C open: I2C goes from downstream to up
        channelCfg &= (~LINKDN_I2C);
        channelCfg |= LINKUP_I2C;
        resetI2C();
    }
}

// Open I2C channel
void
InOutLink::openI2C()
{
    if(IHaveI2C) {
        closeI2C();
    }

    // any host channel open: I2C is downstream, node is the proxy
    // no host channels open: I2C is upstream, node is a pod
    channelCfg |= (IAmProxy ? LINKDN_I2C : LINKUP_I2C);

    _I2Cadr = getUnitAdr();

    Wire.begin(_I2Cadr);
    // Following line to enable broadcast reception (see http://www.gammon.com.au/i2c)
    TWAR = (_I2Cadr << 1) | 1;  // enable broadcasts to be received

    //Wire.onRequest(I2CSendData);  // I2C request is not used
    Wire.onReceive(I2CReadData);
}

void
InOutLink::closeI2C()
{
    // Apparently there is no way to disable I2C. Just stop responding.
    channelCfg &= (~(LINKUP_I2C|LINKDN_I2C));
    _I2Cadr = 0;
}

// Reopen I2C channel to adjust to address change
void
InOutLink::resetI2C()
{
    if(!IHaveI2C) {
        return;
    }
    _I2Cadr = getUnitAdr();

    Wire.begin(_I2Cadr);
    // Following line to enable broadcast reception (see http://www.gammon.com.au/i2c)
    TWAR = (_I2Cadr << 1) | 1;  // enable broadcasts to be received

    // Wire.onRequest(I2CSendData);  // I2C request is not used
    // Wire.onReceive(I2CReadData);  // Should not be needed here
}
*/

void
InOutLink::strRead(Stream *str, cbuf *buf)
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
InOutLink::poll(byte src)
{
    byte retlen = 0;

    // USBONLY:
    UNUSED(src);
    // Inquire USB serial channel (only if buffer not occupied)
    if(bUSB->len == 0) strRead(&Serial, bUSB);
    retlen = bUSB->len;

/*
    if(src & UPSTREAM) {
        if(!(channelCfg & (LINKUP_USB|LINKUP_ETH|LINKUP_I2C))) {
            return 0;
        }

        // Inquire Eth channel (only if buffer not occupied)
        if((channelCfg & LINKUP_ETH) && (bEth->len == 0)) {
            if(_UDPlink.parsePacket()) {
                _HostIP = _UDPlink.remoteIP();
                _HostPort = _UDPlink.remotePort();
                strRead(&_UDPlink, bEth);
            }
        }
        retlen = bEth->len;

        // Inquire USB serial channel (only if buffer not occupied)
        if((channelCfg & LINKUP_USB) && (bUSB->len == 0)) {
            strRead(&Serial, bUSB);
        }
        if(retlen == 0) {
            retlen = bUSB->len;
        }

        // Inquire I2C channel (only if buffer not occupied)

        // This is done in the callback function. Could be activated here too
        // if, for some reason, under some circumstances (e.g. buffer occupied?)
        // the callback fn might not be able to fetch the incoming message.

        // if((channelCfg & LINKUP_I2C) && (bI2CIn->len == 0)) strRead(&Wire, bI2CIn);
        // if(retlen == 0) retlen = bI2CIn->len;

    } else if(src & DNSTREAM) {
        // Inquire I2C channel (only if buffer not occupied)

        // This is done in the callback function. Could be activated here too
        // if, for some reason, under some circumstances (e.g. buffer occupied?)
        // the callback fn might not be able to fetch the incoming message.

        // if((channelCfg & LINKDN_I2C) && (bI2CIn->len == 0)) strRead(&Wire, bI2CIn);

        return bI2CIn->len;
    }
*/
    return retlen;
}

/// Check if a message from the up/downstream channel is available
/// Returns the length of the next available message.
byte
InOutLink::incomingMsg(byte src)
{
    // USBONLY:
    UNUSED(src);
    return bUSB->len;
/*
    if(src & UPSTREAM) {
        // Inquire Eth, USB serial, and I2C channels
        if((channelCfg & LINKUP_ETH) && (bEth->len != 0)) {
            return bEth->len;
        }
        if((channelCfg & LINKUP_USB) && (bUSB->len != 0)) {
            return bUSB->len;
        }
        if((channelCfg & LINKUP_I2C) && (bI2CIn->len != 0)) {
            return bI2CIn->len;
        }
    } else if(src & DNSTREAM) {
        if(channelCfg & LINKDN_I2C) {
            return bI2CIn->len;
        }
    }
    return 0;
*/
}

/// Fetch next available message from up or downstream;
/// returns a pointer to the message buffer (0-terminated).
/// The message is considered as consumed by setting its length to 0;
/// subsequent calls to getMsgLen() will return either 0 or the length
/// of the next available message.
char *
InOutLink::fetchMsg(byte src)
{

    // USBONLY:
    UNUSED(src);
    if(bUSB->len != 0) {
        bUSB->len = 0;
        return bUSB->buf;
    }
/*
    if(src & UPSTREAM) {
        // Inquire Eth, USB serial, and I2C channels
        if((channelCfg & LINKUP_ETH) && (bEth->len != 0)) {
            bEth->len = 0;
            return bEth->buf;
        }
        if((channelCfg & LINKUP_USB) && (bUSB->len != 0)) {
            bUSB->len = 0;
            return bUSB->buf;
        }
        if((channelCfg & LINKUP_I2C) && (bI2CIn->len != 0)) {
            bI2CIn->len = 0;
            return bI2CIn->buf;
        }
    } else if(src & DNSTREAM) {
        if((channelCfg & LINKDN_I2C) && (bI2CIn->len != 0)) {
            bI2CIn->len = 0;
            return bI2CIn->buf;
        }
    }
*/
    return (char *)0;
}

/// Sends message up- or downstream
/// Message contains neither header nor terminator;
/// it must be terminated with '\0'
byte
InOutLink::sendMsg(char *msg, byte dstadr)
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
/*
    if(dstadr == 0xFF) {
        if(channelCfg & LINKUP_ETH) {
            if(!(_HostIP == INADDR_NONE) ) {
                _UDPlink.beginPacket(_HostIP, _HostPort);
                _UDPlink.write((uint8_t *)bOut->buf, ll);
                _UDPlink.endPacket();
            }
            // No error issued if host IP/port is yet unknown
        }
        if(channelCfg & LINKUP_USB) {
            Serial.write((uint8_t *)bOut->buf, ll);
        }
        if(channelCfg & LINKUP_I2C) {
            Wire.beginTransmission(0x01);
            Wire.write((uint8_t *)bOut->buf, ll);
            Wire.endTransmission();
        }
    } else {
        if(channelCfg & LINKDN_I2C) {
            Wire.beginTransmission(dstadr);
            Wire.write((uint8_t *)bOut->buf, ll);
            Wire.endTransmission();
        }
    }
*/
    bOut->len = 0;
    return 0;
}

/// Sends raw message to serial (upstream)
/// Message must be terminated with '\0'
/// FOR DEBUG PURPOSES ONLY
void
InOutLink::sendRaw(char *msg, byte crlf)
{
    // if(!(channelCfg & LINKUP_USB)) return;

    Serial.write((uint8_t *)msg, strlen(msg));
    if(crlf) {
        Serial.write(0x0D);
        Serial.write(0x0A);
    }
}


//void InOutLink::I2CSendData() {}

/*
void
InOutLink::I2CReadData(int cnt)
{
    // Fetch incoming data
    if( //(channelCfg & LINKUP_I2C) &&
        (bI2CIn->len == 0)) {
        strRead(&Wire, bI2CIn);
    }
}
*/

// End InOutLink.cpp
