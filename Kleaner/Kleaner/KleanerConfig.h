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

#define NEXT_SOFT_SERIAL_TX_PIN        10
#define NEXT_SOFT_SERIAL_RX_PIN        12

// ****************************************************************************
// Loop timing
// ****************************************************************************
#define LOOP_TIME_IN_MS                100

// ****************************************************************************
// Version Define
// ****************************************************************************
#define KLEANER_VERSION    "0.2.5"


// ****************************************************************************
// HMI Defines
// ****************************************************************************
#define PAGE_ID_HOME                      0
#define PAGE_ID_MAIN                      1
#define PAGE_ID_CONFIRM                   2
#define PAGE_ID_PROCESS                   3
#define PAGE_ID_COMPLETE                  4
#define PAGE_ID_MAINT                     5

#define HOME_COMP_ID_VERSION              "t0"


#define PROCESS_COMP_ID_TITLE             "t0"
#define PROCESS_COMP_ID_PROGRESS_STATE    "j0"
#define PROCESS_COMP_ID_PROGRESS_PROCESS  "j1"

#define PROCESS_COMP_ID_IN_WATER          "t1"
#define PROCESS_COMP_ID_IN_CLEANER        "t2"
#define PROCESS_COMP_ID_IN_SANITIZER      "t3"
#define PROCESS_COMP_ID_RE_WASTE          "t4"
#define PROCESS_COMP_ID_RE_CLEANER        "t5"
#define PROCESS_COMP_ID_RE_SANITIZER      "t6"
#define PROCESS_COMP_ID_PUMP              "t7"
#define PROCESS_COMP_ID_C02               "t8"

#define COMPLETE_COMP_ID_DURATION         "t1"


// ****************************************************************************
// Debug output defines
// ****************************************************************************
#if defined TRACE_OUTPUT
  #define TPRINT(...)   Serial.print(__VA_ARGS__)
  #define TPRINTLN(...) Serial.println(__VA_ARGS__)
#else
  #define TPRINT(...)
  #define TPRINTLN(...)
#endif

#endif