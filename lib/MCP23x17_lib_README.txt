MCP23x17 Library

Files MCP23x17.*, MCP23017.*, MCP23S17.* belong to the same library (MCP23x17) and should be placed in the same folder.

Unfortunately, with PlatformIO, an issue occurs.

When scanning the library dependencies, the Wire library is not detected as required (correctly? Because MCP23017 is not used in this project).
However, the MCP23017.cpp file is compiled (because it is considered as a part of a used library?), and when the "#include <Wire.h>" is encountered, the missing dependency triggers an error.

The current workaround consists in keeping the three file groups MCP23x17.*, MCP23017.*, MCP23S17.* each in his own directory, as if they were three separate libraries; this way, the library processing is completed with no error.
