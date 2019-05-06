
#include <arduino.h>

#include <HardwareSerial.h>

#include "KleanerConfig.h"
#include "Kleaner.h"


#include "InputWrapper.h"

Kleaner gKleaner;

// ****************************************************************************
// Forward declared functions
// ****************************************************************************
// Button declarations
//void on_button_up(int aCurrentState);
//void on_button_dn(int aCurrentState);
//void on_button_en(int aCurrentState);

// Button Wrappers
//InputWrapper gUpButton(DI_PIN_BUTTON_UP, INPUT_PULLUP, 50, on_button_up);
//InputWrapper gDnButton(DI_PIN_BUTTON_DN, INPUT_PULLUP, 50, on_button_dn);
//InputWrapper gEnButton(DI_PIN_BUTTON_EN, INPUT_PULLUP, 50, on_button_en);

// Loop Timer
MilliTimer gLoopTimer;

// ****************************************************************************
//
// ****************************************************************************
//void on_button_up(int aCurrentState)
//{
//  gKleaner.on_up_button(aCurrentState);
//}

// ****************************************************************************
//
// ****************************************************************************
//void on_button_dn(int aCurrentState)
//{
//  gKleaner.on_dn_button(aCurrentState);
//}

// ****************************************************************************
//
// ****************************************************************************
//void on_button_en(int aCurrentState)
//{
//  gKleaner.on_en_button(aCurrentState);
//}

// ****************************************************************************
//
// ****************************************************************************
void setup()
{
  // Setup serial for logging
  Serial.begin(115200);

  gKleaner.setup();

  // Setup input wrappers
//  gUpButton.setup();
//  gDnButton.setup();
//  gEnButton.setup();
}
 
// ****************************************************************************
//
// ****************************************************************************
void loop()
{
  gLoopTimer.reset();

  // Input wrapper processing
//  gUpButton.loop();
//  gDnButton.loop();
//  gEnButton.loop();  

  gKleaner.loop();

  // process_state();

  int lProcTime = gLoopTimer.delta();
  int lIdleTime = LOOP_TIME_IN_MS - lProcTime;

  if(lIdleTime <= 0)
  {
    //Serial.print("Overprocessing time: ");
    //Serial.println(lProcTime);
  }
  else
  {
    delay(lIdleTime);
  }
}

