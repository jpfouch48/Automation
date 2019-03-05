
#include "OutputWrapper.h"
#include <Arduino.h>


// ****************************************************************************
// See header file for details
// ****************************************************************************
void OutputWrapper::update_config(Config aConfig)
{
  mConfig = aConfig;
  mPulseRunning = false;
  digitalWrite(mPin, mConfig.get_idle_state());
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
      if(true == mInDelay)
      {
        if(mPulseTimer.delta() > mConfig.get_initial_state_time_in_ms())
        {
            digitalWrite(mPin, mConfig.get_initial_state());
            mInDelay = false;
        }
      }
      else
      {
        int lCurrentState = digitalRead(mPin);

        if(lCurrentState == mConfig.get_initial_state() && 
          mPulseTimer.delta() > mConfig.get_initial_state_time_in_ms())
        {
          digitalWrite(mPin, !lCurrentState);
        }
        else if(mPulseTimer.delta() > mConfig.get_cycle_time_in_ms())
        {
          if(0 == mConfig.get_initial_delay_time_in_ms())
            digitalWrite(mPin, mConfig.get_initial_state());
          else
          { 
            digitalWrite(mPin, mConfig.get_idle_state());
            mInDelay = true;
          }

          mPulseTimer.reset();
        }
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

    // If no initial delay is set we will transition to the initial state
    // now, otherwise, the loop function will set the initial state
    // once the initial delay has elapsed
    if(0 == mConfig.get_initial_delay_time_in_ms())
      digitalWrite(mPin, mConfig.get_initial_state());
    else
      mInDelay = true;

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
  mInDelay = false;
  mPulseTimer.reset();

  digitalWrite(mPin, mConfig.get_idle_state());      

  return true;
}
