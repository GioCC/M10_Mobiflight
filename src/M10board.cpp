/**
 *  M10board.cpp
 *
*/

#include "M10board.h"

// Define static objects, so they require no run-time allocation
//M10board    board    = M10board(...);    // Done in main file
SerLink       _IOL;   // Temporarily using the USB-only variant, so no parameters

MCPS            _MCPIO1(0,10);
#ifdef BANK2
MCPS            _MCPIO2(0,15);  // PCB v1.0
//MCP           _MCPIO2(1,10);  // PCB v1.1
#endif

// The number of physical encoders is actually defined in config_board.h;
// however, let's reserve 6 units fornow, since they do not take up any more space anyway.
#define         ENCSLOTS    6
EncoderM10      _Encoders(ENCSLOTS);
int             _EncCount[ENCSLOTS];
uint8_t         _EncModes[ENCSLOTS];

#ifndef HAS_LCD
LedControl      _LEDCTRL1(3,2,4, 2);   //pin #s (dta, clk, cs, cnt), #units
LedControl      _LEDCTRL2(5,2,6, 2);   //pin #s (dta, clk, cs, cnt), #units
//LEDface         _LEDD[6];
#else
LiquidCrystal   _LCDCTRL(8,2,3, 4,5,6,7);
//LCDface         _LCDD[12];
#endif

// -----------------------------------------------------

M10board::M10board(void)
{
    MCPIO1 = &_MCPIO1;
    _MCPIO1.begin();
    _MCPIO1.inputInvert(0xFFFF);
#ifdef BANK2
    MCPIO2 = &_MCPIO2;
    _MCPIO2.begin();
    _MCPIO2.inputInvert(0xFFFF);
#endif
    //vars.set(100, 0xff);
    nAINS = 0;

    IOL  = &_IOL;

    ENCS = &_Encoders;
    Encs = ENCS;
    // Provisionally map first 2 phys encoders to first 2 virtual encoders
    // -> Mapping will be changed at runtime anytime
    // -> if virtual encoders are used, no more than 2 phys encoders are realistically necessary
    encMap[0] = 1;
    encMap[1] = 2;
}


void
M10board::setupAnaIns(uint16_t ai)
{
    nAINS = 0;
    AINS = NULL;
    if(ai == 0) return;
    uint16_t    msk;

    msk = 0x0001;
    for(uint8_t i=0; i<16; i++, msk<<=1) { if(ai & msk) nAINS++; }
    //AINS = new uint8_t[nAINS];
    AINS = (uint8_t *)calloc(nAINS, sizeof(uint8_t));

    nAINS = 0;
    msk = 0x0001;
    for(uint8_t i=0; i<16; i++, msk<<=1) { if(ai & msk) AINS[nAINS++]=(A0+i);}
}

void
M10board::setBoardCfg(M10board_cfg *c)
{
    memcpy(&cfg, c, sizeof(M10board_cfg));

    ModeIO_L(~cfg.digOutputs);
    ModePU_L(~cfg.digOutputs);
#ifdef BANK2
    ModeIO_H(~cfg.digOutputs2);
    ModePU_H(~cfg.digOutputs2);
#endif

    setupAnaIns(cfg.anaInputs);

    ENCS->init(cfg.nEncoders > 8 ? 8 : cfg.nEncoders);      // Correct actual numbers of used encoders (max 8)

#ifndef HAS_LCD
    LEDCTRL[0] = &_LEDCTRL1;
    LEDCTRL[1] = &_LEDCTRL2;

    LEDCTRL[0]->setDeviceCount(cfg.led.nDisplays1, 1);     // also inits
    LEDCTRL[1]->setDeviceCount(cfg.led.nDisplays2, 1);     // also inits

    // for(byte v=0; v<6; v++) {
    //     LEDD[v] = &_LEDD[v];
    //     if(cfg.led.viewport[v].led.disp!=0) {
    //         LEDD[v]->setController(LEDCTRL[cfg.led.viewport[v].led.disp-1]);
    //         LEDD[v]->MapDst(cfg.led.viewport[v].led.start, cfg.led.viewport[v].led.len);
    //     }
    //}
#else
    //LCDCTRL->.....
    // ...other settings..?
    // for(byte v=0; v<12; v++) {
    //     LCDD[v] = &_LCDD[v];
    //     LCDD[v]->setController(&_LCDCTRL);
    //     LCDD[v]->MapDst(cfg.lcd.viewport[v].lcd.startr, cfg.lcd.viewport[v].lcd.startc, cfg.lcd.viewport[v].lcd.len);
    // }
#endif
}

void
M10board::setBoardPostCfg(void)
{
    uint8_t ne = (cfg.nVirtEncoders==0 ? cfg.nEncoders : cfg.nVirtEncoders);
    for(uint8_t i=0; i < ne; i++) {
        // Get number of configured modes from ManagedEnc into ENCS
        ENCS->setNModes(i+1, EncMgr.getEnc(i+1)->getNModes());
    }
		if(cfg.led.nDisplays1) LEDCTRL[0]->switchOn();
		if(cfg.led.nDisplays2) LEDCTRL[1]->switchOn();
}

void
M10board::ModeL(byte pin, byte mode)
{
    if(pin > 16) return;
    // Mode: INPUT, INPUT_PULLUP,  OUTPUT
    pin--;
    if(mode == INPUT || mode == INPUT_PULLUP) {
        // Internal I/O Mode: 0 = Out, 1 = In
        IOcfgL |= (1<<pin);
        if(mode == INPUT) {
            IOpullupL &= ~(1<<pin);
        } else {
            IOpullupL |= (1<<pin);
        }
    } else {
        IOcfgL &= ~(1<<pin);
    }
    MCPIO1->pinMode(IOcfgL);
    MCPIO1->pullupMode(IOpullupL);
}

#ifdef BANK2
void
M10board::ModeH(byte pin, byte mode)
{
    if(pin > 16) return;
    // Mode: INPUT, INPUT_PULLUP,  OUTPUT
    pin--;
    if(mode == INPUT || mode == INPUT_PULLUP) {
        // Internal I/O Mode: 0 = Out, 1 = In
        IOcfgH |= (1<<pin);
        if(mode == INPUT) {
            IOpullupH &= ~(1<<pin);
        } else {
            IOpullupH |= (1<<pin);
        }
    } else {
        IOcfgH &= ~(1<<pin);
    }
    MCPIO2->pinMode(IOcfgH); //IOcfg.b16.low);
    MCPIO2->pullupMode(IOpullupH)); //IOpullup.b16.low);
}
#endif

void
M10board::pinMode(uint8_t pin, uint8_t mode)
{
    if(pin<=16) { ModeL(pin, mode); }
#ifdef BANK2
    else        { ModeH(pin-16, mode); }
#endif
}

void
M10board::digitalWrite(uint8_t pin, uint8_t val)
{
    Dout.write(pin, val);

//     if(pin<=16) { Dout.writeL(pin, val); }
// #ifdef BANK2
//     else        { Dout.writeH(pin-16, val); }
// #endif
}

int
M10board::digitalRead(uint8_t pin)
{
    return Din.val(pin);

//     if(pin<=16)         { return Din.valL(pin); }
// #ifdef BANK2
//     else if(pin<=32)    { return Din.valH(pin-16); }
// #endif
//     // 'virtual' pins above 32 are reserved for encoder mirroring and are read-only
//     else if(pin>32)     { return (lwbit(virtualEncs, pin-33)!=0); }
//     else return 0;
}

void
M10board::mirrorEncoder(byte fromPos, byte toPos, byte clean, byte toHighInputs)
{
    // FromPos = 1..6 (in base vector);
    // ToPos = 1..10 (in virtual vector);
    if(fromPos>6 || toPos > 10) return;
    // Convert index pos to bit pos
    toPos   = toPos+toPos+toPos-3;          // 3*(toPos-1)
    fromPos = fromPos+fromPos+fromPos-3;    // 3*(fromPos-1)
    // Encs #1,#2,#3 are at the start of the lower word (bits 0...8)
    // Encs #4,#5,#6 are at the start of the upper word
    if(fromPos > 6) fromPos+=7;

    // Only affect bits from current encoder - others should remain "frozen"
    virtualEncs &= (clean ? 0UL : ~(7UL<<toPos));
    virtualEncs |= ((toPos>fromPos) ?
                    ((realEncs & (7UL<<fromPos))<<(toPos-fromPos)) :
                    ((realEncs & (7UL<<fromPos))>>(fromPos-toPos)) );

    // if requested, also copy virtual vector back to actual input vector
    // (from input 33 up)
    if(toHighInputs) {
        byte *p = (byte *)&virtualEncs;
        for(byte i=0; i<4; i++) {
            Din.writeB(4+i, p[i]);
        }
    }

}

void
M10board::remapEncoder(byte fromPos, byte toPos)
{
    if(((fromPos-1) < 2)&&(toPos <= 8)) encMap[fromPos-1] = toPos;
}

void
M10board::ScanInOut(byte mode)
{
    uint16_t    iovec;
    uint32_t    encvec;

    /// Write Outputs
    if(mode != 1) {
        iovec = Dout.valW(0);  //Dout.val()[0] + (Dout.val()[1] << 8);
        MCPIO1->digitalWrite(iovec);   // Pins configured as input are ignored on write
#ifdef BANK2
        iovec = Dout.valW(2);  //Dout.val()[2] + (Dout.val()[3] << 8);
        MCPIO2->digitalWrite(iovec);   // Pins configured as input are ignored on write
#endif
    }
    /// Read Inputs
    if(mode != 2) {
        iovec = MCPIO1->digitalRead() & IOcfgL;
        Din.writeW(0, iovec);

#ifdef BANK2
        iovec = MCPIO2->digitalRead() & IOcfgH;
        Din.writeW(2, iovec);
#endif

        /// Handle encoder input mirroring
        if(cfg.nEncoders + cfg.nVirtEncoders != 0) {
            // collect enc inputs
            encvec = (Din.valW(2) & 0x01FF);
            encvec <<= 9;
            encvec |= (Din.valW(0) & 0x01FF);

            realEncs = encvec;

            if(cfg.nVirtEncoders!=0) {
                // remap encoders if required (writes to virtualEncs and back to inputs)
                if(encMap[0]) { mirrorEncoder(1, encMap[0]); }
                if(encMap[1]) { mirrorEncoder(2, encMap[1]); }
            }
        }

        /// Handle button/switch processing
        ButtonMgr.checkButtons(Din.val());

        /// Handle encoder input processing
        if(cfg.nEncoders + cfg.nVirtEncoders != 0) {
            // Use EITHER physical OR virtual encoders, not both
            if(cfg.nVirtEncoders==0) {
                ENCS->update(realEncs);     // Feed inputs to encoder processors
            } else {
                ENCS->update(virtualEncs);  // Feed inputs to (virtual)encoder processors
            }
            // Detect encoders transitions/counts
            // Since the version of EncManager with no callbacks is used, copy relevant data to local vars and pass those along
            // (the version with callbacks would inquire the encoder data directly through indexed provider functions -
            // actually, the very same ones we are using here...)
            uint8_t ne = (cfg.nVirtEncoders==0 ? cfg.nEncoders : cfg.nVirtEncoders);
            for(uint8_t i=0; i < ne; i++) {
                _EncCount[i] = ENCS->getEncCount(i+1, 1);     // Get DIFF count
                //_EncCount[i] = ENCS->getEncCount(i, 0);     // Get ABSOLUTE count
                _EncModes[i] = ENCS->getMode(i+1);
            }

            /// Handle encoder processing
            // Register counts
            EncMgr.checkEncs(_EncCount, _EncModes);
            // If required, also register transitions
            EncMgr.checkEncs(ENCS->getEncChangeUp(), ENCS->getEncChangeDn(), ENCS->getEncChangeQUp(), ENCS->getEncChangeQDn(), _EncModes);

            // Manage encoder switch lines
            // (done as ordinary switches - nothing to do here)
        }
        // TODO
    }
}

/// =========================================
///
///  Comms functions (temporary protocol)
///
///  Msg format examples:
///
///  1234-020-030A      -> offset 0x1234; data len 02 bytes (bit #0 unused); high byte 0x03, low byte 0x0A
///  1234-005-1         -> offset 0x1234; data len 0 bytes => 1 bit; bit position #5; bit value 1
///  1234-05S-ABcde     -> offset 0x1234; data len 5 chars (bit #S => string); string = "ABcde"

void
M10board::ScanComms(void)
{
    char *m;

    if(!IOL->incomingMsg()) {
        IOL->poll();
    }

    if(IOL->incomingMsg()) {
        uint16_t lowcode;
        m = IOL->fetchMsg();
        //TODO: Process incoming message <m>
    }
}

char *
M10board::msghdr(uint16_t offset, uint8_t len, uint8_t bit) {
    char *d = IOL->outBuf();
    uinttoHex(offset, d, 4);
    d[4]='-';
    d[5]='0'+len/10;   // make sure len<99
    d[6]='0'+len%10;   // make sure len<99
    d[7]=(bit==0xFF ? 'S' : '0'+(bit&0x07));
    d[8]='-';
    return (d+9);
}

uint16_t *
M10board::request(uint16_t offset)
{
    // TEMPORARY:
    UNUSED(offset);
    return 0;
}

void
M10board::send(uint16_t offset, uint8_t lenChars, byte *value)
{
    char *d = msghdr(offset, lenChars, 0xFF);
    strcpy(d, (char *)value);
    IOL->sendMsg();
}

void
M10board::send(uint16_t offset, uint8_t lenBytes, uint8_t bit, uint16_t value)
{
    char *d = msghdr(offset, lenBytes, bit);
    uinttoHex(value, d, lenBytes*2);
    IOL->sendMsg();
}


//END M10board.cpp
