//
// MFOutputHandlers.h
//

// These files contain the command processing functions for interfacing 
// OUTPUT devices to Mobiflight. 
// 
// These functions must be called upon receiving a MF command message, 
// to parse it and call the relevant output driver functions.
//
// They do NOT contain functions regarding:
// - Device init
// - Device value storage management
// - Device internal logic

namespace Output
{
    void OnSet();
}

namespace LedSegment
{
    void OnInit();
    void OnSet();
    void OnSetBrightness();
}
