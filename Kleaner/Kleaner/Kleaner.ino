
#include <arduino.h>
#include <HardwareSerial.h>
#include "KleanerConfig.h"
#include "Kleaner.h"

Kleaner gKleaner;

// Loop Timer
MilliTimer gLoopTimer;

// ****************************************************************************
//
// ****************************************************************************
void setup()
{
  // Setup serial for logging
  Serial.begin(115200);
  gKleaner.setup();
}
 
// ****************************************************************************
//
// ****************************************************************************
void loop()
{
  gLoopTimer.reset();
  gKleaner.loop();

  int lProcTime = gLoopTimer.delta();
  int lIdleTime = LOOP_TIME_IN_MS - lProcTime;

  if(lIdleTime <= 0)
  {
    //Serial.print(F("Overprocessing time: "));
    //Serial.println(lProcTime);
  }
  else
  {
    delay(lIdleTime);
  }
}

