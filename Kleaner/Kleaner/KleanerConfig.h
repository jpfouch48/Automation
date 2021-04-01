#ifndef KLEANER_CONFIG_h
#define KLEANER_CONFIG_h

#include <Arduino.h>

// ****************************************************************************
// Debug output controls
// ****************************************************************************
//#define TRACE_OUTPUT
//#define AUTO_START

// ****************************************************************************
// Version Define
// ****************************************************************************
#define KLEANER_VERSION                   "0.5.0"

// ****************************************************************************
//  Digital I/O Pin definitions
// ****************************************************************************

// Normal Relay outputs
#define DO_PIN_PUMP                       11
#define DO_PIN_RELAY_CO2                  10

// Motor Controller outputs (ball valves)
#define DO_PIN_MOTOR_IN_WATER_1           6
#define DO_PIN_MOTOR_IN_WATER_2           5
#define DO_PIN_MOTOR_IN_CLEANER_1         4
#define DO_PIN_MOTOR_IN_CLEANER_2         3
#define DO_PIN_MOTOR_IN_SANITIZER_1       2
#define DO_PIN_MOTOR_IN_SANITIZER_2       14
#define DO_PIN_RECIRC_WASTE_1             8
#define DO_PIN_RECIRC_WASTE_2             15

#define DO_PIN_RECIRC_SANITIZER_1           9
#define DO_PIN_RECIRC_SANITIZER_2          12

#define DO_PIN_RECIRC_CLEANER_1             7
#define DO_PIN_RECIRC_CLEANER_2            13


// ****************************************************************************
// Loop timing
// ****************************************************************************
#define LOOP_TIME_IN_MS                   100

#define BALL_VALVE_DELAY                    5
// ****************************************************************************
// HMI Defines
// ****************************************************************************

// Page ID Defines
#define PAGE_ID_HOME                        0
#define PAGE_ID_MAIN                        1
#define PAGE_ID_CONFIRM                     2
#define PAGE_ID_PROCESS                     3
#define PAGE_ID_COMPLETE                    4
#define PAGE_ID_TEST_OUTPUT                 5
#define PAGE_ID_TEST_PHASE                  6
#define PAGE_ID_CONFIG                      7
#define PAGE_ID_CONFIG_EDIT                 8

// Input Event Defines (touch events)
#define MAIN_BUTTON_ID_CLEAN                1
#define MAIN_BUTTON_ID_TEST_OUTPUT          3
#define MAIN_BUTTON_ID_TEST_PHASE           4
#define MAIN_BUTTON_ID_CONFIG               5

#define CONFIRM_BUTTON_ID_YES               3
#define CONFIRM_BUTTON_ID_NO                4
#define CONFIRM_RADIO_ID_SIXTEL             5
#define CONFIRM_RADIO_ID_HALF               6

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

#define CONFIG_BUTTON_ID_SAVE               2
#define CONFIG_BUTTON_ID_BACK               1
#define CONFIG_BUTTON_ID_EDIT               6
#define CONFIG_BUTTON_ID_RESET_FACTORY      7

#define CONFIG_BUTTON_ID_TYPE_PREV          4
#define CONFIG_BUTTON_ID_TYPE_NEXT          5

#define CONFIG_EDIT_BUTTON_ID_OK            13
#define CONFIG_EDIT_BUTTON_ID_CYCLE_PREV    14
#define CONFIG_EDIT_BUTTON_ID_CYCLE_NEXT    16

#define CONFIG_EDIT_BUTTON_ID_RINSE_CNT_INC 17
#define CONFIG_EDIT_BUTTON_ID_RINSE_CNT_DEC 18
#define CONFIG_EDIT_BUTTON_ID_CO2_CNT_INC   20
#define CONFIG_EDIT_BUTTON_ID_CO2_CNT_DEC   19
#define CONFIG_EDIT_BUTTON_ID_PMP_DUR_INC   22
#define CONFIG_EDIT_BUTTON_ID_PMP_DUR_DEC   21
#define CONFIG_EDIT_BUTTON_ID_CO2ON_DUR_INC 24
#define CONFIG_EDIT_BUTTON_ID_CO2ON_DUR_DEC 23
#define CONFIG_EDIT_BUTTON_ID_CO2OF_DUR_INC 26
#define CONFIG_EDIT_BUTTON_ID_CO2OF_DUR_DEC 25
#define CONFIG_EDIT_BUTTON_ID_PURGE_DUR_INC 28
#define CONFIG_EDIT_BUTTON_ID_PURGE_DUR_DEC 27

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

// Config Text
#define CONFIG_COMP_ID_TYPE               "t0"

// Config-Edit Text
#define CONFIG_EDIT_COMP_ID_CYCLE         "t0"
#define CONFIG_EDIT_COMP_ID_RINSE_CNT     "t8"
#define CONFIG_EDIT_COMP_ID_CO2_CNT       "t9"
#define CONFIG_EDIT_COMP_ID_PUMP_DUR      "t10"
#define CONFIG_EDIT_COMP_ID_CO2_ON_DUR    "t11"
#define CONFIG_EDIT_COMP_ID_CO2_OFF_DUR   "t12"
#define CONFIG_EDIT_COMP_ID_PURGE_DUR     "t13"

// Radio compoenets
#define CONFIRM_COMP_ID_SIXTLE            "r0"
#define CONFIRM_COMP_ID_HALF              "r1"

// ****************************************************************************
// Debug output defines
// ****************************************************************************
#if defined TRACE_OUTPUT
  #define TPRINT(...)   Serial.print(__VA_ARGS__)
  #define TPRINTLN(...) Serial.println(__VA_ARGS__)

  #define TS() { char gzBuffer00001[10]; sprintf(gzBuffer00001, "%09d", millis()/1000); Serial.print(gzBuffer00001); Serial.print(F(" - ")); }

  #define TSPRINT(...)   { TS(); Serial.print(__VA_ARGS__); }
  #define TSPRINTLN(...) { TS(); Serial.println(__VA_ARGS__); }
#else
  #define TPRINT(...)
  #define TPRINTLN(...)
  #define TSPRINT(...)
  #define TSPRINTLN(...)
#endif

#endif