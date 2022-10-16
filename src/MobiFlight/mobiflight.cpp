//
// mobiflight.cpp
//
// (C) MobiFlight Project 2022
//

#include <Arduino.h>
#include "mobiflight.h"

bool                powerSavingMode   = false;
const unsigned long POWER_SAVING_TIME = 60 * 15; // in seconds

void timedUpdate(void (*fun)(), uint32_t *last, uint8_t intv)
{
    if (millis() - *last >= intv) {
        fun();
        *last = millis();
    }
}

// ************************************************************
// Power saving
// ************************************************************
void SetPowerSavingMode(bool state)
{
#ifdef DEBUG2CMDMESSENGER
        cmdMessenger.sendCmd(kDebug, F("Off"));
#endif
}

// ************************************************************
// Setup
// ************************************************************
void MF_setup()
{
    attachCommandCallbacks();
    cmdMessenger.printLfCr();

    //TODO: (MADE IN THE MAIN APPLICATION) read config switches and setup config accordingly
}

// ************************************************************
// Loop function
// ************************************************************
void MF_loop()
{
    // Process incoming serial data, and perform callbacks
    // cmdMessenger.feedinSerialData();
}

// mobiflight.cpp
