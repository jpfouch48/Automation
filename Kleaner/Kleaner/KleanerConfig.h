#ifndef KLEANER_CONFIG_h
#define KLEANER_CONFIG_h

#include <Arduino.h>

#define TRACE_OUTPUT

// ****************************************************************************
//  Digital I/O Pin definitions
// ****************************************************************************

//#define DO_PIN_RELAY_AIR                  12
//#define DO_PIN_ON_BOARD_LED               12
#define DO_PIN_RELAY_CO2                  11

#define DO_PIN_RECIRC_SANITIZER           10
#define DO_PIN_RECIRC_WASTE               9
#define DO_PIN_RECIRC_CLEANER             8
#define DO_PIN_PUMP                       7

#define DO_PIN_MOTOR_IN_WATER_1           6
#define DO_PIN_MOTOR_IN_WATER_2           5
#define DO_PIN_MOTOR_IN_CLEANER_1         4
#define DO_PIN_MOTOR_IN_CLEANER_2         3
#define DO_PIN_MOTOR_IN_SANITIZER_1       2
#define DO_PIN_MOTOR_IN_SANITIZER_2       1

#define NEXT_SOFT_SERIAL_TX_PIN           A0
#define NEXT_SOFT_SERIAL_RX_PIN           A1

// ****************************************************************************
// Loop timing
// ****************************************************************************
#define LOOP_TIME_IN_MS                   100

// ****************************************************************************
// Version Define
// ****************************************************************************
#define KLEANER_VERSION                   "0.3.0"


// ****************************************************************************
// HMI Defines
// ****************************************************************************

// Page Defines
#define PAGE_ID_HOME                        0
#define PAGE_ID_MAIN                        1
#define PAGE_ID_CONFIRM                     2
#define PAGE_ID_PROCESS                     3
#define PAGE_ID_COMPLETE                    4
#define PAGE_ID_TEST_OUTPUT                 5
#define PAGE_ID_TEST_PHASE                  6

// Input Event Defines (touch events)
#define MAIN_BUTTON_ID_CLEAN                1
#define MAIN_BUTTON_ID_TEST_OUTPUT          3
#define MAIN_BUTTON_ID_TEST_PHASE           4

#define CONFIRM_BUTTON_ID_YES               3
#define CONFIRM_BUTTON_ID_NO                4

#define COMPLETE_BUTTON_ID_START_OVER       2

#define PROCESS_BUTTON_ID_STOP              3

#define TEST_OUTPUT_BUTTON_ID_BACK          2
#define TEST_OUTPUT_BUTTON_ID_IN_WATER      3
#define TEST_OUTPUT_BUTTON_ID_IN_CLEANER    4
#define TEST_OUTPUT_BUTTON_ID_IN_SANITIZER  5
#define TEST_OUTPUT_BUTTON_ID_RE_WASTE      6
#define TEST_OUTPUT_BUTTON_ID_RE_CLEANER    7
#define TEST_OUTPUT_BUTTON_ID_RE_SANITIZER  8
#define TEST_OUTPUT_BUTTON_ID_PUMP          9
#define TEST_OUTPUT_BUTTON_ID_C02           10

#define TEST_PHASE_BUTTON_ID_BACK           2
#define TEST_PHASE_BUTTON_ID_PURGE          4
#define TEST_PHASE_BUTTON_ID_RINSE          5
#define TEST_PHASE_BUTTON_ID_WASH           6
#define TEST_PHASE_BUTTON_ID_SANI           7
#define TEST_PHASE_BUTTON_ID_PRESSURE       8

// Text Component Defines
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