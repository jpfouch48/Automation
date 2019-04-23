
#include "MotorWrapper.h"
#include <Arduino.h>

// ****************************************************************************
// See header file for details
// ****************************************************************************
void MotorWrapper::setup()
{
  pinMode(mPin1, OUTPUT);
  pinMode(mPin2, OUTPUT);
  reset();
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void MotorWrapper::loop()
{
  // Nothing to do here
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void MotorWrapper::set_state_pin1(int aState)
{
  digitalWrite(mPin1, aState);
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void MotorWrapper::set_state_pin2(int aState)
{
  digitalWrite(mPin2, aState);
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
int MotorWrapper::get_state_pin1()
{
  return digitalRead(mPin1); 
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
int MotorWrapper::get_state_pin2()
{
  return digitalRead(mPin2); 
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void MotorWrapper::reset()
{
  digitalWrite(mPin1, mIdleStatePin1);
  digitalWrite(mPin2, mIdleStatePin2);
}