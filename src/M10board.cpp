/**
 *  M10board.cpp
 *
*/

#include "M10board.h"


MCPS            _MCPIO1(0,10);
MCPS            _MCPIO2(0,15);  // PCB v1.0
//MCP           _MCPIO2(1,10);  // PCB v1.1

// LedControl      _LEDCTRL1(3,2,4, 2);   //pin #s (dta, clk, cs, cnt), #units
// LedControl      _LEDCTRL2(5,2,6, 2);   //pin #s (dta, clk, cs, cnt), #units
//LiquidCrystal   _LCDCTRL(8,2,3, 4,5,6,7);

// -----------------------------------------------------

M10board::M10board(void)
{
    //vars.set(100, 0xff);
    nAINS = 0;

    // Provisionally map first 2 phys encoders to first 2 virtual encoders
    // -> Mapping will be changed at runtime anytime
    // -> if virtual encoders are used, no more than 2 phys encoders are realistically necessary
    EncMap[0] = 1;
    EncMap[1] = 2;
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
    //! TODO - WARNING: <c> is in PROGMEM! Either:
    //  - change cfg from pointer to var (uses RAM)
    //  - move BoardCfgs[] out of PROGMEM (uses RAM)
    //  - assign "cfg = F(c)" rather than "cfg = c" (or whatever corrects to progmem reference)
    
    //memcpy(&cfg, c, sizeof(M10board_cfg));
    cfg = c;

    MCPIO1 = &_MCPIO1;
    _MCPIO1.begin();
    _MCPIO1.inputInvert(0xFFFF);
    setIOMode(0,~cfg->digOutputs);
    setPUMode(0,~cfg->digOutputs);

    if(cfg->hasBank2) {
        MCPIO2 = &_MCPIO2;
        _MCPIO2.begin();
        _MCPIO2.inputInvert(0xFFFF);
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
M10board::setPinMode(uint8_t pin, uint8_t mode)
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
//     else if(pin>32)     { return (lwbit(virtEncInputs, pin-33)!=0); }
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
    virtEncInputs &= (clean ? 0UL : ~(7UL<<toPos));
    virtEncInputs |= ((toPos>fromPos) ?
                    ((realEncInputs & (7UL<<fromPos))<<(toPos-fromPos)) :
                    ((realEncInputs & (7UL<<fromPos))>>(fromPos-toPos)) );

    // if requested, also copy virtual vector back to actual input vector
    // (from input 33 up)
    if(toHighInputs) {
        byte *p = (byte *)&virtEncInputs;
        for(byte i=0; i<4; i++) {
            Din.writeB(4+i, p[i]);
        }
    }

}

void
M10board::remapEncoder(byte fromPos, byte toPos)
{
    if(((fromPos-1) < 2)&&(toPos <= 8)) EncMap[fromPos-1] = toPos;
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
        iovec = Dout.valW(2);  //Dout.val()[2] + (Dout.val()[3] << 8);
        MCPIO2->digitalWrite(iovec);   // Pins configured as input are ignored on write
    }
    /// Read Inputs
    if(mode != 2) {
        iovec = MCPIO1->digitalRead() & IOcfg[0];
        Din.writeW(0, iovec);

        if(cfg->hasBank2) {
            iovec = MCPIO2->digitalRead() & IOcfg[1];
            Din.writeW(2, iovec);
        }

        /// Handle encoder input mirroring
        if(cfg->nEncoders + cfg->nVirtEncoders != 0) {
            // collect enc inputs
            encvec = (Din.valW(2) & 0x01FF);
            encvec <<= 9;
            encvec |= (Din.valW(0) & 0x01FF);

            realEncInputs = encvec;

            if(cfg->nVirtEncoders!=0) {
                // remap encoders if required (writes to virtEncInputs and back to inputs)
                if(EncMap[0]) { mirrorEncoder(1, EncMap[0]); }
                if(EncMap[1]) { mirrorEncoder(2, EncMap[1]); }
            }
        }

        /// Handle button/switch processing
        ButtonMgr.checkButtons(Din.val());

        /// Handle encoder input processing
        if(cfg->nEncoders + cfg->nVirtEncoders != 0) {
            // Use EITHER physical OR virtual encoders, not both
            if(cfg->nVirtEncoders==0) {
                Encs.update(realEncInputs);     // Feed inputs to encoder processors
            } else {
                Encs.update(virtEncInputs);  // Feed inputs to (virtual)encoder processors
            }
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

            /// Handle encoder processing
            // Register counts
            EncMgr.checkEncs(EncCount, EncModes);
            // If required, also register transitions
            EncMgr.checkEncs(Encs.getEncChangeUp(), Encs.getEncChangeDn(), Encs.getEncChangeQUp(), Encs.getEncChangeQDn(), EncModes);

            // Manage encoder switch lines
            // (done as ordinary switches - nothing to do here)
        }
        // TODO
    }
}


//END M10board.cpp
