//
// commandmessenger.cpp
//
// (C) MobiFlight Project 2022
//

#include "mobiflight.h"

CmdMessenger  cmdMessenger = CmdMessenger(Serial);

void OnTrigger();
void OnUnknownCommand();

// Callbacks define on which received commands we take action
void attachCommandCallbacks()
{
    // Attach callback methods
    cmdMessenger.attach(OnUnknownCommand);
    cmdMessenger.attach(kGetInfo, OnGetInfo);
    cmdMessenger.attach(kGetConfig, OnGetConfig);
    cmdMessenger.attach(kSetConfig, OnSetConfig);
    cmdMessenger.attach(kResetConfig, OnResetConfig);
    cmdMessenger.attach(kSaveConfig, OnSaveConfig);
    cmdMessenger.attach(kActivateConfig, OnActivateConfig);
    cmdMessenger.attach(kSetName, OnSetName);
    cmdMessenger.attach(kGenNewSerial, OnGenNewSerial);
    cmdMessenger.attach(kTrigger, OnTrigger);

    //TODO: build OUTPUT device interface functions:

    // Output (LED)
    // cmdMessenger.attach(kSetPin, Output::OnSet);
    
    // Block Outputs (Shift reg - IOexp.)
    // cmdMessenger.attach(kSetShiftRegisterPins, OutputShifter::OnSet);
    
    // Display
    // cmdMessenger.attach(kInitModule, LedSegment::OnInit);
    // cmdMessenger.attach(kSetModule, LedSegment::OnSet);
    // cmdMessenger.attach(kSetModuleBrightness, LedSegment::OnSetBrightness);

    // LCD
    // cmdMessenger.attach(kSetLcdDisplayI2C, LCDDisplay::OnSet);
        
    cmdMessenger.sendCmd(kDebug, F("Attached callbacks"));
}

// Called when a received command has no attached function
void OnUnknownCommand()
{
    cmdMessenger.sendCmd(kStatus, F("n/a"));
}

void OnTrigger()
{
    //TODO: build input "trigger" function (initial setting) (EXTERNAL) 
    // Button::OnTrigger();
    // InputShifter::OnTrigger();
}

// commandmessenger.cpp
