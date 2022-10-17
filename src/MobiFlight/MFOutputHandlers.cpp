//
// MFOutputHandlers.cpp
//
#include "mobiflight.h"


namespace Output {

    void OnSet()
    {
        // Read led state argument, interpret string as boolean
        int pin = cmdMessenger.readInt16Arg();
        int state = cmdMessenger.readInt16Arg();
        //TODO: insert operation function (attach as callback?)
        // analogWrite(pin, state);
    }

}

namespace LedSegment
{
    void OnInitModule()
    {
        int module     = cmdMessenger.readInt16Arg();
        int subModule  = cmdMessenger.readInt16Arg();
        int brightness = cmdMessenger.readInt16Arg();
        //TODO: insert operation function (attach as callback?)
        //ledSegments[module]->setBrightness(subModule, brightness);
    }

    void OnSetModule()
    {
        int     module    = cmdMessenger.readInt16Arg();
        int     subModule = cmdMessenger.readInt16Arg();
        char   *value     = cmdMessenger.readStringArg();
        uint8_t points    = (uint8_t)cmdMessenger.readInt16Arg();
        uint8_t mask      = (uint8_t)cmdMessenger.readInt16Arg();
        //TODO: insert operation function (attach as callback?)
        //ledSegments[module]->display(subModule, value, points, mask);
        //Prototype:    
        //void display(byte module, char *string, byte points, byte mask, bool convertPoints = false);
    }

    void OnSetModuleBrightness()
    {
        int module     = cmdMessenger.readInt16Arg();
        int subModule  = cmdMessenger.readInt16Arg();
        int brightness = cmdMessenger.readInt16Arg();
        //TODO: insert operation function (attach as callback?)
        //ledSegments[module]->setBrightness(subModule, brightness);
    }
}

