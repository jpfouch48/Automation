#ifndef KLEANER_CONFIG_h
#define KLEANER_CONFIG_h

//#define KLEANER_TEST_MENU
#define KLEANER_TEST_STATE_MENU


// ****************************************************************************
//
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
//
// ****************************************************************************
#define LCD_ROW_COUNT                   2
#define LCD_COL_COUNT                  16

// ****************************************************************************
//
// ****************************************************************************
#define LOOP_TIME_IN_MS                100

// ****************************************************************************
// Splash Display
// ****************************************************************************
#define SPLASH_LINE_1    "Kleaner"
#define SPLASH_LINE_2    "V 0.3"


// ****************************************************************************
// Dump Config
//   Timeline - 0123456789
//   PUMP     -           
//   CO2      - XXXXXXXXXX
// ****************************************************************************
#define DUMP_DURATION      10
#define DUMP_INPUT         InputSource::None
#define DUMP_RECIRC        RecircDest::Waste 
#define DUMP_PUMP          OutputWrapper::Config()
#define DUMP_CO2           OutputWrapper::Config(LOW, HIGH, 10000, 10000)

// ****************************************************************************
// Purge Config
//   Timeline - 0123456789
//   PUMP     -           
//   CO2      - XXXXXXXXXX
// ****************************************************************************
#define PURGE_DURATION      10
#define PURGE_INPUT         InputSource::None
#define PURGE_RECIRC        RecircDest::Waste 
#define PURGE_PUMP          OutputWrapper::Config()
#define PURGE_CO2           OutputWrapper::Config(LOW, HIGH, 10000, 10000)

// ****************************************************************************
// Pre Rinse Config
//   Timeline - 0123456789
//   PUMP     -  XXXXXXX           
//   CO2      -         XX
// ****************************************************************************
#define PRE_RINSE_DURATION     20
#define PRE_RINSE_INPUT        InputSource::Water
#define PRE_RINSE_RECIRC       RecircDest::Waste 
#define PRE_RINSE_PUMP         OutputWrapper::Config(LOW, HIGH, 10000, 7000, 1000)
#define PRE_RINSE_CO2          OutputWrapper::Config(LOW, LOW,  10000, 8000)

// ****************************************************************************
// Wash Config
//   Timeline - 0123456789
//   PUMP     -  XXXXXXX           
//   CO2      -         XX
// ****************************************************************************
#define WASH_DURATION     30
#define WASH_INPUT        InputSource::Cleaner
#define WASH_RECIRC       RecircDest::Cleaner 
#define WASH_PUMP         OutputWrapper::Config(LOW, HIGH, 10000, 7000, 1000)
#define WASH_CO2          OutputWrapper::Config(LOW, LOW,  10000, 8000)

// ****************************************************************************
// Post Rinse Config
//   Timeline - 0123456789
//   PUMP     -  XXXXXXX           
//   CO2      -         XX
// ****************************************************************************
#define POST_RINSE_DURATION     20
#define POST_RINSE_INPUT        InputSource::Water
#define POST_RINSE_RECIRC       RecircDest::Waste 
#define POST_RINSE_PUMP         OutputWrapper::Config(LOW, HIGH, 10000, 7000, 1000)
#define POST_RINSE_CO2          OutputWrapper::Config(LOW, LOW,  10000, 8000)

// ****************************************************************************
// Sanitize Config
//   Timeline - 0123456789
//   PUMP     -  XXXXXXX           
//   CO2      -         XX
// ****************************************************************************
#define SANI_DURATION     30
#define SANI_INPUT        InputSource::Sanitizer
#define SANI_RECIRC       RecircDest::Sanitizer 
#define SANI_PUMP         OutputWrapper::Config(LOW, HIGH, 10000, 7000, 1000)
#define SANI_CO2          OutputWrapper::Config(LOW, LOW,  10000, 8000)

// ****************************************************************************
// Pressurize Config
//   Timeline - 0123456789
//   PUMP     -              
//   CO2      - XX        
// ****************************************************************************
#define PRESS_DURATION     10
#define PRESS_INPUT        InputSource::None
#define PRESS_RECIRC       RecircDest::None 
#define PRESS_PUMP         OutputWrapper::Config()
#define PRESS_CO2          OutputWrapper::Config(LOW, HIGH,  10000, 10000)

#endif