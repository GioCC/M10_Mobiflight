//
// config.cpp
//
// (C) MobiFlight Project 2022
//

#include "mobiflight.h"

//TODO: Set sensible values
//TODO: Place constants in flash
const char *MFserial  = "SN-000-M10";    //TODO is "M" allowed?
const char *MFname    = "M10CONTROLLER";
const char *MFtype    = "M10-CUSTOM";
const char *MFVERSION = "1.0.0";
const char *MFCOREVERSION = "1.0.0";

// ************************************************************
// configBuffer handling
// ************************************************************

void OnResetConfig()
{
    cmdMessenger.sendCmd(kStatus, F("OK"));
}

void OnSetConfig()
{
#ifdef DEBUG2CMDMESSENGER
    cmdMessenger.sendCmd(kDebug, F("Setting config NOT SUPPORTED"));
#endif
}

void OnSaveConfig()
{
    cmdMessenger.sendCmd(kConfigSaved, F("OK"));
}

void OnActivateConfig()
{
    cmdMessenger.sendCmd(kConfigActivated, F("OK"));
}

void OnGetConfig()
{
    //TODO: Build config string in MF format according to actual config (external function)
    
    // Temp defines:
    const int configLength = 100;
    char getConfigChar(int i);

    cmdMessenger.sendCmdStart(kInfo);
    // if (configLength > 0) {
        // cmdMessenger.sendCmdArg((char)MFeeprom.read_byte(MEM_OFFSET_CONFIG));
        cmdMessenger.sendCmdArg(getConfigChar(0));
        for (uint16_t i = 1; i < configLength; i++) {
            // cmdMessenger.sendArg((char)MFeeprom.read_byte(MEM_OFFSET_CONFIG + i));
            cmdMessenger.sendArg(getConfigChar(i));
        }
    // }
    cmdMessenger.sendCmdEnd();
}
 

void OnGetInfo()
{
    cmdMessenger.sendCmdStart(kInfo);
    cmdMessenger.sendCmdArg(MFtype);
    cmdMessenger.sendCmdArg(MFname);
    cmdMessenger.sendCmdArg(MFserial);
    cmdMessenger.sendCmdArg(MFVERSION);
    cmdMessenger.sendCmdArg(MFCOREVERSION);
    cmdMessenger.sendCmdEnd();
}

// ************************************************************
// serial number handling
// ************************************************************
void OnGenNewSerial()
{
    cmdMessenger.sendCmd(kInfo, MFserial);
}

// ************************************************************
// Naming handling
// ************************************************************
void OnSetName()
{
    cmdMessenger.sendCmd(kStatus, MFname);
}

// config.cpp
