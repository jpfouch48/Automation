#ifndef KLEANER_CONFIG_h
#define KLEANER_CONFIG_h

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
#define SPLASH_LINE_2    "V 0.1"






// ****************************************************************************
// Purge Config
//   Timeline - 0123456789
//   PUMP     -           
//   CO2      - XXXXX
// ****************************************************************************
#define PURGE_DURATION      20
#define PURGE_INPUT         InputSource::None
#define PURGE_RECIRC        RecircDest::Waste 
#define PURGE_PUMP          NULL
#define PURGE_CO2           new OutputWrapper::Config(LOW, HIGH, 10000, 5000)


// ****************************************************************************
// Pre Rinse Config
//   Timeline - 0123456789
//   PUMP     -   XXXXX           
//   CO2      -         XX
// ****************************************************************************
#define PRE_RINSE_DURATION     20
#define PRE_RINSE_INPUT        InputSource::Water
#define PRE_RINSE_RECIRC       RecircDest::Waste 
#define PRE_RINSE_PUMP         new OutputWrapper::Config(LOW, HIGH, 10000, 5000, 2000)
#define PRE_RINSE_CO2          new OutputWrapper::Config(LOW, LOW,  10000, 8000)

// ****************************************************************************
// Wash Config
//   Timeline - 0123456789
//   PUMP     -   XXXXX           
//   CO2      -         XX
// ****************************************************************************
#define WASH_DURATION     30
#define WASH_INPUT        InputSource::Cleaner
#define WASH_RECIRC       RecircDest::Cleaner 
#define WASH_PUMP         new OutputWrapper::Config(LOW, HIGH, 10000, 5000, 2000)
#define WASH_CO2          new OutputWrapper::Config(LOW, LOW,  10000, 8000)

// ****************************************************************************
// Post Rinse Config
//   Timeline - 0123456789
//   PUMP     -   XXXXX           
//   CO2      -         XX
// ****************************************************************************
#define POST_RINSE_DURATION     20
#define POST_RINSE_INPUT        InputSource::Water
#define POST_RINSE_RECIRC       RecircDest::Waste 
#define POST_RINSE_PUMP         new OutputWrapper::Config(LOW, HIGH, 10000, 5000, 2000)
#define POST_RINSE_CO2          new OutputWrapper::Config(LOW, LOW,  10000, 8000)

// ****************************************************************************
// Sanitize Config
//   Timeline - 0123456789
//   PUMP     -   XXXXX           
//   CO2      -         XX
// ****************************************************************************
#define SANI_DURATION     30
#define SANI_INPUT        InputSource::Sanitizer
#define SANI_RECIRC       RecircDest::Sanitizer 
#define SANI_PUMP         new OutputWrapper::Config(LOW, HIGH, 10000, 5000, 2000)
#define SANI_CO2          new OutputWrapper::Config(LOW, LOW,  10000, 8000)

// ****************************************************************************
// Pressurize Config
//   Timeline - 0123456789
//   PUMP     -              
//   CO2      - XX        
// ****************************************************************************
#define PRESS_DURATION     10
#define PRESS_INPUT        InputSource::None
#define PRESS_RECIRC       RecircDest::None 
#define PRESS_PUMP         NULL
#define PRESS_CO2          new OutputWrapper::Config(LOW, HIGH,  10000, 2000)

#endif