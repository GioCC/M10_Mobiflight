//
// MFInputHandlers.cpp
//
#include "mobiflight.h"

namespace Button {
    
    enum {
        OnPress,
        OnRelease,
    };

    void OnEvent(uint8_t eventId, uint8_t pin, const char *name)
    {
        cmdMessenger.sendCmdStart(kButtonChange);
        cmdMessenger.sendCmdArg(name);
        cmdMessenger.sendCmdArg(eventId);
        cmdMessenger.sendCmdEnd();
    };

    //TODO: OnResync must cycle on ALL release events first, then on ALL press events:
    // the OnResync() function is probably better implemented at higher level by directly calling OnEvent().
    // Code is left below just for reference for the final implementation.
    void OnResync(void)     // was: OnTrigger
    {
        // Trigger all button release events first...
        // for (uint8_t i = 0; i < buttonsRegistered; i++) {
        //     OnEvent(btnOnRelease, _pin, _name);
        // }

        // ... then trigger all the press events
        // for (uint8_t i = 0; i < buttonsRegistered; i++) {
        //     OnEvent(btnOnPress, _pin, _name);
        // }

    }
}

namespace Encoder
{
    void OnEvent(uint8_t eventId, uint8_t pin, const char *name)
    {
        (void) pin; // Unused
        cmdMessenger.sendCmdStart(kEncoderChange);
        cmdMessenger.sendCmdArg(name);
        cmdMessenger.sendCmdArg(eventId);
        cmdMessenger.sendCmdEnd();
    };


}

namespace InputShifter
{
    enum {
        OnPress,
        OnRelease,
    };

    void OnEvent(uint8_t eventId, uint8_t pin, const char *name)
    {
        cmdMessenger.sendCmdStart(kInputShifterChange);
        cmdMessenger.sendCmdArg(name);
        cmdMessenger.sendCmdArg(pin);
        cmdMessenger.sendCmdArg(eventId);
        cmdMessenger.sendCmdEnd();
    };

    //TODO: OnResync must cycle on ALL release events first, then on ALL press events:
    // the OnResync() function is probably better implemented at higher level by directly calling OnEvent().
    // Code is left below just for reference for the final implementation.
    void OnResync()
    {
        // Retrigger all the input shifters. This automatically sends
        // the release events first followed by press events.
        // for (uint8_t i = 0; i < inputShiftersRegistered; i++) {
        //     inputShifters[i]->retrigger();
        // }

        // inputShifters[i]->retrigger() is: 
        // (_lastState[m] and _name are attributes of the i-th ShReg)

        //  void MFInputShifter::retrigger()
        //  {
        //      // Reads the current state for all connected modules then fires
        //      // release events for every released button followed by
        //      // press events for every pressed button.
        //      uint8_t state;
        //  
        //      poll(DONT_TRIGGER);
        //  
        //      // Trigger all the released buttons
        //      for (int module = 0; module < _moduleCount; module++) {
        //          state = _lastState[module];
        //          for (uint8_t i = 0; i < 8; i++) {
        //              // Only trigger if the button is in the off position
        //              if (state & 1) {
        //                  OnEvent(inputShifterOnRelease, (i + (module * 8)), _name);
        //              }
        //              state = state >> 1;
        //          }
        //      }
        //  
        //      // Trigger all the pressed buttons
        //      for (int module = 0; module < _moduleCount; module++) {
        //          state = _lastState[module];
        //          for (uint8_t i = 0; i < 8; i++) {
        //              // Only trigger if the button is in the on position
        //              if (!(state & 1)) {
        //                  OnEvent(inputShifterOnPress, (i + (module * 8)), _name);
        //              }
        //              state = state >> 1;
        //          }
        //      }
        //  }

    }

}
