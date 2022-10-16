//
// MFInputHandlers.h
//

// These files contain the event processing functions for interfacing 
// INPUT devices to Mobiflight.
//  
// These functions must be called upon value change or other events 
// (possibly by attaching them as static fields to the classes of corresponding devices).
//
// These files do NOT contain functions regarding:
// - Device init
// - Device value storage management
// - Device internal logic

#include <Arduino.h>

namespace Button
{
    enum {
        OnPress,
        OnRelease,
    };

    void OnEvent(uint8_t eventId, uint8_t pin, const char *name);
    //void OnResync(void);     // see comments in .cpp
}

namespace Encoder
{
    enum {
        encLeft,
        encLeftFast,
        encRight,
        encRightFast,
    };

    void OnEvent(uint8_t eventId, uint8_t pin, const char *name); // <pin> is unused
    //void OnResync(void);     // Encoders don't have a Resync() operation
}

namespace InputShifter
{
    void OnEvent(uint8_t eventId, uint8_t pin, const char *name);
    //void OnResync(void);     // see comments in .cpp
}

