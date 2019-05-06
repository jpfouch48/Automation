#ifndef KLEANER_CONFIG_h
#define KLEANER_CONFIG_h

#include <Arduino.h>

#define TRACE_OUTPUT

// ****************************************************************************
//  Digital I/O Pin definitions
// ****************************************************************************

#define DO_PIN_ON_BOARD_LED            13
//#define DO_PIN_RELAY_AIR               12
#define DO_PIN_RELAY_CO2               11

#define DO_PIN_RELAY_INPUT_SANITIZER    9
#define DO_PIN_RELAY_INPUT_CLEANER      8
#define DO_PIN_RECIRC_SANITIZER         7
#define DO_PIN_RECIRC_WASTE             6
#define DO_PIN_RECIRC_CLEANER           5
#define DO_PIN_PUMP                     4

#define DO_PIN_MOTOR_INPUT_WATER_1      3
#define DO_PIN_MOTOR_INPUT_WATER_2      2


// ****************************************************************************
// Loop timing
// ****************************************************************************
#define LOOP_TIME_IN_MS                100

// ****************************************************************************
// Splash Display
// ****************************************************************************
#define SPLASH_LINE_1    "Kleaner"
#define SPLASH_LINE_2    "V 0.1.1"




#if defined TRACE_OUTPUT
  #define TPRINT(...)   Serial.print(__VA_ARGS__)
  #define TPRINTLN(...) Serial.println(__VA_ARGS__)
#else
  #define TPRINT(...)
  #define TPRINTLN(...)
#endif

#endif