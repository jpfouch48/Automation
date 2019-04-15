#ifndef KLEANER_CONFIG_h
#define KLEANER_CONFIG_h

#include <Arduino.h>

// ****************************************************************************
//  Digital I/O Pin definitions
// ****************************************************************************
//#define DO_PIN_RELAY_AIR               12
#define DO_PIN_RELAY_CO2               11
#define DO_PIN_RELAY_INPUT_WATER       10
#define DO_PIN_RELAY_INPUT_SANITIZER    9
#define DO_PIN_RELAY_INPUT_CLEANER      8
#define DO_PIN_RECIRC_SANITIZER         7
#define DO_PIN_RECIRC_WASTE             6
#define DO_PIN_RECIRC_CLEANER           5
#define DO_PIN_PUMP                     4

#define DI_PIN_BUTTON_UP               A4
#define DI_PIN_BUTTON_DN               A2
#define DI_PIN_BUTTON_EN               A3
#define DO_PIN_DISPLAY                 A5

// ****************************************************************************
// LCD parameters
// ****************************************************************************
#define LCD_ROW_COUNT                   2
#define LCD_COL_COUNT                  16

// ****************************************************************************
// Loop timing
// ****************************************************************************
#define LOOP_TIME_IN_MS                100

// ****************************************************************************
// Splash Display
// ****************************************************************************
#define SPLASH_LINE_1    "Kleaner"
#define SPLASH_LINE_2    "V 0.1.0"

#endif