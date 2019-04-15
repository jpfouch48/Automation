#ifndef KLEANER_CONFIG_h
#define KLEANER_CONFIG_h

// ****************************************************************************
// Test macros to enable/disable specific test menus
// ****************************************************************************
#define KLEANER_TEST_MENU
//#define KLEANER_TEST_STATE_MENU

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
#define SPLASH_LINE_2    "V 0.5"

// ****************************************************************************
// Prime Config
//   Timeline - 0123456789
//   PUMP     -           
//   CO2      - 
//   Input    - XXXXXXXXXX
// ****************************************************************************
#define DURATION_PRIME      10
#define INPUT_PRIME         InputSource::Water
#define RECIRC_PRIME        RecircDest::Waste 
#define PUMP_CFG_PRIME      OutputWrapper::Config(LOW)
#define CO2_CFG_PRIME       OutputWrapper::Config(LOW)
#define INPUT_CFG_PRIME     OutputWrapper::Config(LOW, HIGH, 10000, 10000)

// ****************************************************************************
// Dump Config
//   Timeline - 0123456789
//   PUMP     -           
//   CO2      - XXXXXXXXXX
//   Input    - 
// ****************************************************************************
#define DURATION_DUMP      10
#define INPUT_DUMP         InputSource::None
#define RECIRC_DUMP        RecircDest::Waste 
#define PUMP_CFG_DUMP      OutputWrapper::Config(LOW)
#define CO2_CFG_DUMP       OutputWrapper::Config(LOW, HIGH, 10000, 10000)
#define INPUT_CFG_DUMP     OutputWrapper::Config(LOW)

// ****************************************************************************
// Purge Config
//   Timeline - 0123456789
//   PUMP     -           
//   CO2      - XXXXXXXXXX
//   Input    - 
// ****************************************************************************
#define DURATION_PURGE      10
#define INPUT_PURGE         InputSource::None
#define RECIRC_PURGE        RecircDest::Waste 
#define PUMP_CFG_PURGE      OutputWrapper::Config(LOW)
#define CO2_CFG_PURGE       OutputWrapper::Config(LOW, HIGH, 10000, 10000)
#define INPUT_CFG_PURGE     OutputWrapper::Config(LOW)

// ****************************************************************************
// Pre Rinse Config
//   Timeline - 01234567890123456789
//   PUMP     -     XXXXXXXXXX           
//   CO2      -                XXXXX
//   Input (W)- XXXXXXXXXXXXXX
// ****************************************************************************
#define DURATION_PRE_RINSE     20
#define INPUT_PRE_RINSE        InputSource::Water
#define RECIRC_PRE_RINSE       RecircDest::Waste 
#define PUMP_CFG_PRE_RINSE     OutputWrapper::Config(LOW, HIGH, 20000, 10000, 4000)
#define CO2_CFG_PRE_RINSE      OutputWrapper::Config(LOW, LOW,  20000, 15000)
#define INPUT_CFG_PRE_RINSE    OutputWrapper::Config(LOW, HIGH, 20000, 14000)

// ****************************************************************************
// Wash Config
//   Timeline - 0123456789
//   PUMP     -  XXXXXXX           
//   CO2      -         XX
//   Input (C)- XXXXXXXX
// ****************************************************************************
#define DURATION_WASH     30
#define INPUT_WASH        InputSource::Cleaner
#define RECIRC_WASH       RecircDest::Cleaner 
#define PUMP_CFG_WASH     OutputWrapper::Config(LOW, HIGH, 10000, 7000, 1000)
#define CO2_CFG_WASH      OutputWrapper::Config(LOW, LOW,  10000, 8000)
#define INPUT_CFG_WASH    OutputWrapper::Config(LOW, HIGH, 10000, 8000)

// ****************************************************************************
// Post Rinse Config
//   Timeline - 0123456789
//   PUMP     -  XXXXXXX           
//   CO2      -         XX
//   Input (W)- XXXXXXXX
// ****************************************************************************
#define DURATION_POST_RINSE     30
#define INPUT_POST_RINSE        InputSource::Water
#define RECIRC_POST_RINSE       RecircDest::Waste 
#define PUMP_CFG_POST_RINSE     OutputWrapper::Config(LOW, HIGH, 10000, 7000, 1000)
#define CO2_CFG_POST_RINSE      OutputWrapper::Config(LOW, LOW,  10000, 8000)
#define INPUT_CFG_POST_RINSE    OutputWrapper::Config(LOW, HIGH, 10000, 8000)

// ****************************************************************************
// Sanitize Config
//   Timeline - 0123456789
//   PUMP     -  XXXXXXX           
//   CO2      -         XX
//   Input (S)- XXXXXXXX
// ****************************************************************************
#define DURATION_SANI     30
#define INPUT_SANI        InputSource::Sanitizer
#define RECIRC_SANI       RecircDest::Sanitizer 
#define PUMP_CFG_SANI     OutputWrapper::Config(LOW, HIGH, 10000, 7000, 1000)
#define CO2_CFG_SANI      OutputWrapper::Config(LOW, LOW,  10000, 8000)
#define INPUT_CFG_SANI    OutputWrapper::Config(LOW, HIGH, 10000, 8000)

// ****************************************************************************
// Pressurize Config
//   Timeline - 0123456789
//   PUMP     -              
//   CO2      - XXXXXXXXXX        
//   Input    -
// ****************************************************************************
#define DURATION_PRESS     10
#define INPUT_PRESS        InputSource::None
#define RECIRC_PRESS       RecircDest::None 
#define PUMP_CFG_PRESS     OutputWrapper::Config(LOW)
#define CO2_CFG_PRESS      OutputWrapper::Config(LOW, HIGH,  10000, 10000)
#define INPUT_CFG_PRESS    OutputWrapper::Config(LOW)

#endif