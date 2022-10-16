//
// config.cpp
//
// (C) MobiFlight Project 2022
//

#include "mobiflight.h"

const char *MFserial  = "SN-000-M10";    //TODO is "M" allowed?
const char *MFname    = "M10CONTROLLER";
const char *MFtype    = "M10-CUSTOM";
const char *MFVERSION = "1.0.0";

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
}
 

void OnGetInfo()
{
    cmdMessenger.sendCmdStart(kInfo);
    cmdMessenger.sendCmdArg(type);
    cmdMessenger.sendCmdArg(name);
    cmdMessenger.sendCmdArg(serial);
    cmdMessenger.sendCmdArg(VERSION);
    cmdMessenger.sendCmdEnd();
}

// ************************************************************
// serial number handling
// ************************************************************
void OnGenNewSerial()
{
    cmdMessenger.sendCmd(kInfo, serial);
}

// ************************************************************
// Naming handling
// ************************************************************
void OnSetName()
{
    cmdMessenger.sendCmd(kStatus, name);
}

// config.cpp
