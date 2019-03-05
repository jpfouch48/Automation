
#include "OutputWrapper.h"
#include <Arduino.h>

// ****************************************************************************
// See header file for details
// ****************************************************************************
OutputWrapper::OutputWrapper(int aPin, 
                             int aIdleState) :
  mPin(aPin),
  mConfig(aIdleState),
  mPulseRunning(false)
{
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
OutputWrapper::OutputWrapper(int aPin, 
                             int aIdleState,
                             int aInitialState, 
                             int aCycleTimeInMs, 
                             int aInitialTimeInMs) :
  mPin(aPin),
  mConfig(aIdleState, 
          aInitialState, 
          aCycleTimeInMs, 
          aInitialTimeInMs),
  mPulseRunning(false)
{
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void OutputWrapper::setup()
{
  pinMode(mPin, OUTPUT);
  digitalWrite(mPin, mConfig.get_idle_state());     
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void OutputWrapper::loop()
{
  if(mConfig.get_type() == Config::Type::Pulsing)
  {
    if(true == mPulseRunning)
    {
      int lCurrentState = digitalRead(mPin);

      if(lCurrentState == mConfig.get_initial_state() && 
         mPulseTimer.delta() > mConfig.get_initial_time_in_ms())
      {
        digitalWrite(mPin, !lCurrentState);
      }
      else if(mPulseTimer.delta() > mConfig.get_cycle_time_in_ms())
      {
        digitalWrite(mPin, mConfig.get_initial_state());
        mPulseTimer.reset();
      }
    }
  }
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
bool OutputWrapper::set_state(int aState)
{
  if(mConfig.get_type() != Config::Type::Manual)
    return false;

  digitalWrite(mPin, aState);    

  return true;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
bool OutputWrapper::start_pulse()
{
  if(mConfig.get_type() != Config::Type::Pulsing)
    return false;

    reset_pulse();
    digitalWrite(mPin, mConfig.get_initial_state());
    mPulseRunning = true;

  return true;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
bool OutputWrapper::reset_pulse()
{
  if(mConfig.get_type() != Config::Type::Pulsing)
    return false;

  mPulseRunning = false;
  mPulseTimer.reset();

  digitalWrite(mPin, mConfig.get_idle_state());      

  return true;
}
