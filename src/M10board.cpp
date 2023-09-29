// =======================================================================
// @file        M10board.cpp
//
// @project     M10_Mobiflight
// 
// @details     Defines a generic individual M10 board 
//              with its the hardware resources
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2022-10-15
// @modifiedby  GiorgioCC - 2023-09-29 14:32
//
// Copyright (c) 2022 - 2023 GiorgioCC
// =======================================================================


#include "M10board.h"

// -----------------------------------------------------

M10board::M10board(void* memAllocator(uint16_t)) 
{
    memAlloc = memAllocator;
    nAINS = 0;
}

void
M10board::setupAnaIns(uint16_t ai)
{
    uint16_t    msk;
    uint16_t    AINSsize = 0;

    nAINS = 0;
    AINS = NULL;
    if(ai == 0) return;

    msk = 0x0001;
    for(uint8_t i=0; i<16; i++, msk<<=1) { 
        if((ai & msk)==0) continue;
        nAINS++; AINSsize += sizeof(uint8_t);
    }
    //AINS = new uint8_t[nAINS];
    //AINS = (uint8_t *)calloc(nAINS, sizeof(uint8_t));
    AINS = (uint8_t *)memAlloc(AINSsize);

    nAINS = 0;
    msk = 0x0001;
    for(uint8_t i=0; i<16; i++, msk<<=1) { 
        if((ai & msk)==0) continue;
        AINS[nAINS++]=(A0+i);
    }
}

void
M10board::setBoardCfg(M10BoardConfig *c)
{
    //! TODO - WARNING: <c> is in PROGMEM! Either:
    //  - change cfg from pointer to var (uses RAM)
    //  - move BoardCfgs[] out of PROGMEM (uses RAM)
    //  - assign "cfg = F(c)" rather than "cfg = c" (or whatever corrects to progmem reference)
    
    //memcpy(&cfg, c, sizeof(M10BoardConfig));
    cfg = c;

    // MCPIO1 = &_MCPIO1;  // TEST!!! TO BE REMOVED
    MCPIO1 = new (memAlloc(sizeof(MCPS))) MCPS(0,10);
    MCPIO1->begin();
    MCPIO1->inputInvert(0xFFFF);
    setIOMode(0,~cfg->digOutputs);
    setPUMode(0,~cfg->digOutputs);

    if(cfg->hasBank2) {
        // MCPIO2 = &_MCPIO2;  // TEST!!! TO BE REMOVED
        MCPIO2 = new (memAlloc(sizeof(MCPS))) MCPS(0,15);   // PCB v1.0
        //MCPIO2 = new (memAlloc(sizeof(MCPS))) MCPS(1,10);   // PCB v1.1
        MCPIO2->begin();
        MCPIO2->inputInvert(0xFFFF);
        setIOMode(1,~cfg->digOutputs2);
        setPUMode(1,~cfg->digOutputs2);
    }

    setupAnaIns(cfg->anaInputs);

    Encs.init(cfg->nEncoders > 8 ? 8 : cfg->nEncoders);      // Correct actual numbers of used encoders (max 8)

if(cfg->hasDisplays) {
    LedControl* base = (LedControl*)_DISP;

    LEDCTRL[0] = new (base)     LedControl(1,2,4, 2); //pin #s (dta, clk, cs, cnt), #units
    LEDCTRL[1] = new (&base[1]) LedControl(3,2,4, 2); //pin #s (dta, clk, cs, cnt), #units

    LEDCTRL[0].setDeviceCount(cfg->nDisplays1, 1);     // also inits
    LEDCTRL[1].setDeviceCount(cfg->nDisplays2, 1);     // also inits
}
else 
if(cfg->hasLCD)
{
    LiquidCrystal* base = (LiquidCrystal*)_DISP;
    LCDCTRL[0] = new(base) LiquidCrystal(8,2,3, 4,5,6,7);

    //TODO: LCD setup
}


}

void
M10board::setBoardPostCfg(void)
{
    uint8_t ne = (cfg->nVirtEncoders==0 ? cfg->nEncoders : cfg->nVirtEncoders);
    for(uint8_t i=0; i < ne; i++) {
        // Get number of configured modes from ManagedEnc into ENCS
        Encs.setNModes(i+1, EncMgr.getEnc(i+1)->getNModes());
    }
    if(cfg->hasDisplays) {
        if(cfg->nDisplays1) {
            LEDCTRL[0].hw_init();
            LEDCTRL[0].init();
            LEDCTRL[0].switchOn();
        }
        if(cfg->nDisplays2) {
            LEDCTRL[1].hw_init();
            LEDCTRL[1].init();
            LEDCTRL[1].switchOn();
        }
    }
    if(cfg->hasLCD) {
        //TODO
    }
}

void M10board::setIOMode(uint8_t bank, uint16_t IOmode)
{ 
    // Mode 0 = Out, 1 = In
    if(bank == 0) {
        MCPIO1->pinMode(IOcfg[0] = IOmode);
    } else if( cfg->hasBank2) {
        MCPIO2->pinMode(IOcfg[1] = IOmode); 
    } 
}            

void M10board::setPUMode(uint8_t bank, uint16_t PUmode)
{ 
    // PullUp: 1 = On
    if(bank == 0) {
        MCPIO1->pullupMode(IOpullup[0] = PUmode);
    } else if(cfg->hasBank2) {
        MCPIO2->pullupMode(IOpullup[1] = PUmode); 
    } 
}

void
M10board::setIOPinMode(uint8_t pin, uint8_t mode)
{
    // if(pin<=16) { ModeL(pin, mode); }
    // else        { ModeH(pin-16, mode); }
    pin--;
    uint8_t bank = (pin<16) ? 0 : 1;
    if(bank > 0 && !cfg->hasBank2) return;

    MCP *MCPIO = (pin<16) ? MCPIO1 : MCPIO2;
    
    if(mode == INPUT || mode == INPUT_PULLUP) {
        // Internal I/O Mode: 0 = Out, 1 = In
        IOcfg[bank] |= (1<<pin);
        if(mode == INPUT) {
            IOpullup[bank] &= ~(1<<pin);
        } else {
            IOpullup[bank] |= (1<<pin);
        }
    } else {
        IOcfg[bank] &= ~(1<<pin);
    }
    MCPIO->pinMode(IOcfg[bank]);
    MCPIO->pullupMode(IOpullup[bank]);
}

void
M10board::setIOMode(uint8_t bank, uint16_t dir, uint16_t pullups)
{
    bank--;
    if(bank > 1) return;
    if(bank == 1 && !cfg->hasBank2) return;
    MCP *MCPIO = (bank == 0) ? MCPIO1 : MCPIO2;
    IOcfg[bank] = dir;
    IOpullup[bank] = pullups;
    MCPIO->pinMode(IOcfg[bank]);
    MCPIO->pullupMode(IOpullup[bank]);
}

void
M10board::outWrite(uint8_t pin, uint8_t val)
{
    pin--;
    uint8_t bank = (pin<16) ? 0 : ((pin<32) ? 1 : 2);

    if(bank < 2) {
        if(bank == 1 && !cfg->hasBank2) return;
        cacheWrite(pin+1, val);
        MCP *MCPIO = (pin<16) ? MCPIO1 : MCPIO2;
        MCPIO->IOWrite((pin & 0x0F), val);
    } else {
        // These are not cached
        pin &= 0x1F; // pin -= 32;
        if((pin >= cfg->nLEDsOnMAX) || cfg->LEDsOnMAX == nullptr) return;
        LEDonMAX led = cfg->LEDsOnMAX[pin];
        // TODO ......
        // MAX.setSegment(led.unit, led.digit, led.segment, val);
    }
}

void
M10board::cacheWrite(uint8_t pin, uint8_t val)
{
    Dout.write(pin, val);
//     if(pin<=16) { Dout.writeL(pin, val); }
// #ifdef BANK2
//     else        { Dout.writeH(pin-16, val); }
// #endif
}

int
M10board::cacheRead(uint8_t pin)
{
    return Din.val(pin);

//     if(pin<=16)         { return Din.valL(pin); }
// #ifdef BANK2
//     else if(pin<=32)    { return Din.valH(pin-16); }
// #endif
//     // 'virtual' pins above 32 are reserved for encoder mirroring and are read-only
//     else if(pin>32)     { return (lwbit(virtEncInputs, pin-33)!=0); }
//     else return 0;
}

void
M10board::mirrorEncoder(byte fromPos, byte toPos, bool move)
{
    uint32_t msk;
    // FromPos = 1..3
    // ToPos = 1..10
    if(fromPos>3 || toPos > 10 || fromPos==toPos) return;
    // Convert index pos to bit pos: 3*(n-1)
    fromPos--; toPos--; 
    fromPos = fromPos+fromPos+fromPos;
    toPos   = toPos  +toPos  +toPos;

    // Only affect bits from current encoder - others should remain unchanged
    msk = (7UL<<toPos);
    encInputs &= ~msk;     
    msk = (7UL<<fromPos);
    encInputs |= ((toPos>fromPos) ?
                    ((encInputs & msk)<<(toPos-fromPos)) :
                    ((encInputs & msk)>>(fromPos-toPos)) );
    if(move) encInputs &= ~msk;

    // if requested, also copy virtual vector back to source input vector
    // (from input 33 up)
    // if(toSrcInputs) {
    //     byte *p = (byte *)&virtEncInputs;
    //     for(byte i=0; i<4; i++) {
    //         Din.writeB(4+i, p[i]);
    //     }
    // }
}

void
M10board::remapEncoder(byte fromPos, byte toPos)
{
    if(((fromPos-1) > 2)||((toPos-1) > 9)) return;
    EncMap[fromPos-1] = toPos;
}

void
M10board::ScanInOut(byte mode)
{
    uint16_t    iovec;
    // uint32_t    encvec;

    // ==============================
    //  Write Digital Outputs
    // ==============================
    if(mode != 1) {
        iovec = Dout.valW(0);  //Dout.val()[0] + (Dout.val()[1] << 8);
        MCPIO1->IOWrite(iovec);   // Pins configured as input are ignored on write
        if(cfg->hasBank2) {
            iovec = Dout.valW(2);  //Dout.val()[2] + (Dout.val()[3] << 8);
            MCPIO2->IOWrite(iovec);   // Pins configured as input are ignored on write
        }
    }
    // ==============================
    //  Read Digital Inputs
    // ==============================
    if(mode != 2) {
        iovec = MCPIO1->IORead() & IOcfg[0];
        Din.writeW(0, iovec);

        if(cfg->hasBank2) {
            iovec = MCPIO2->IORead() & IOcfg[1];
            Din.writeW(2, iovec);
        }

        // TODO
    }
}

void
M10board::ProcessSwitches(void)
{
    //  Handle button/switch processing
    // ===================================
    ButtonMgr.checkButtons(Din.val());
}

void
M10board::ProcessEncoders(void)
{
    if(cfg->nEncoders + cfg->nVirtEncoders == 0) return;
    
    //  Handle encoder input mirroring
    // ===================================
    // collect physical enc inputs
    if(cfg->nEncoders > 3) {
        // assert(cfg->nVirtEncoders == 0); // Virtual encoders only available if no 2nd bank is used!
        encInputs = (Din.valW(2) & 0x01FF);    // Encoders 4..6 (2nd bank)
        encInputs <<= 9;
    }
    encInputs |= (Din.valW(0) & 0x01FF);   // Encoders 1..3 (1st bank)

    if(cfg->nVirtEncoders!=0) {
        // remap encoders if required (ineffective otherwise)
        mirrorEncoder(1);
        mirrorEncoder(2);
        mirrorEncoder(3);
    }

    //  Handle encoder input processing
    // ===================================

    Encs.update(encInputs);     // Feed inputs to encoder processors
    
    // Detect encoders transitions/counts
    // Since the version of EncManager with no callbacks is used, copy relevant data to local vars and pass those along
    // (the version with callbacks would inquire the encoder data directly through indexed provider functions -
    // actually, the very same ones we are using here...)
    uint8_t ne = (cfg->nVirtEncoders==0 ? cfg->nEncoders : cfg->nVirtEncoders);
    for(uint8_t i=0; i < ne; i++) {
        EncCount[i] = Encs.getEncCount(i+1, 1);     // Get DIFF count
        //EncCount[i] = Encs.getEncCount(i, 0);     // Get ABSOLUTE count
        EncModes[i] = Encs.getMode(i+1);
    }

    //  Handle encoder event processing
    // ===================================
    
    // Register counts
    EncMgr.checkEncs(EncCount, EncModes);
    // If required, also register transitions
    EncMgr.checkEncs(Encs.getEncChangeUp(), Encs.getEncChangeDn(), Encs.getEncChangeQUp(), Encs.getEncChangeQDn(), EncModes);

    // Manage encoder switch lines
    // (done as ordinary switches - nothing to do here)
}

//END M10board.cpp
