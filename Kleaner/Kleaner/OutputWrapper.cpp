
#include "OutputWrapper.h"
#include <Arduino.h>

// ****************************************************************************
// See header file for details
// ****************************************************************************
void OutputWrapper::setup()
{
  pinMode(mPin, OUTPUT);
  digitalWrite(mPin, mIdleState);     
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void OutputWrapper::loop()
{
  // Nothing to do here
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
bool OutputWrapper::set_state(int aState)
{
  digitalWrite(mPin, aState);
  return true;
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
int OutputWrapper::get_state()
{
  return digitalRead(mPin); 
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void OutputWrapper::reset()
{
  digitalWrite(mPin, mIdleState);
}