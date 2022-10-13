/*
Read any direct or matrix input state or detect input state change for use
in your custom code

Don't use this for sending data to X-Plane with the SimInput function ("C" pin name prefix) -
it is already done automatically using the "D"-prefix.

Download ArdSim Library on the SimVim Design website: http://svglobe.com
 */

// IMPORTANTE
// Per errori di linker tipo 'multiple definition of `__ltsf2' etc,
// vedi: http://www.avrfreaks.net/forum/multiple-definition-error-c-atof
// (aggiunta opzioni "-lc -lm -lc"


//================================
//   Compile-time configuration
#include "config_build.h"
//================================
#include <ArdSim_Interface.h>          //  -- ArdSim library

#if !defined(CONTROLLER)
M10board board = M10board(1,1,1);
#endif

//================================

//----------------------
void setup() {

    BoardNumber 1;

//InputMatrix(22, 23, 24, 25);   //connect button matrix 2x2
// servoGauge (1, 2, 0, 100, 3, 5);

}

//--------------------------
void loop() {

ArdSimScan;

#if !defined(CONTROLLER)
  // check if Pin #2 state is changed from OFF to ON:

if (ReadInput(2, ON ))  {   } // your code here

  // check if matrix butoon #2 state is changed from ON to OFF:

if (ReadInput(2, OFF, MX))  {   } // your code here

//-------------------------------------------
// Read the current Arduino Pin #3   state change (dynamics)

int act = ReadInput( 3, ON ); // check if Pin #3 state is changed from 0 to 1 and save

if (act==1)  { }// do something

//=============== STATIC ===================

// Read the current Arduino Pin #5 STATIC state for use in your custom code
// see  description on the site

if (ReadInput (5)==1) {   }  // your code here if pin state is "ON"
else     { } // your "else" code here  (pin state is static "OFF"

// Read static state of the matrix input #5:

volatile
int mx5 = ReadInput (5, MX);

// Read static state of the matrix input #67
// (MX can be omited if node number more than 53 for MEGA or 13 for UNU:

volatile
int mx67 = ReadInput (67);
#endif

}
//--- End -------------------

