
#include "OutputWrapper.h"
#include <Arduino.h>

// ****************************************************************************
// See header file for details
// ****************************************************************************
OutputWrapper::OutputWrapper(int aPin, 
              int aIdleState) :
  mPin(aPin),
  mIdleState(aIdleState),
  mInitialState(aIdleState),
  mType(OutputType::Manual),
  mCycleTimeInMs(0),
  mInitialTimeInMs(0),
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
  mIdleState(aIdleState),
  mInitialState(aInitialState),
  mType(OutputType::Pulsing),
  mCycleTimeInMs(aCycleTimeInMs),
  mInitialTimeInMs(aInitialTimeInMs),
  mPulseRunning(false)
{
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void OutputWrapper::set_mode_manual(int aIdleState)
{
  mIdleState = aIdleState;
  mInitialState = aIdleState;
  mType = OutputType::Manual;
  mCycleTimeInMs = 0;
  mInitialTimeInMs = 0;
  mPulseRunning = false;
  digitalWrite(mPin, mIdleState);  
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void OutputWrapper::set_mode_pulse(int aIdleState,
                         int aInitialState, 
                         int aCycleTimeInMs, 
                         int aInitialTimeInMs)
{
  mType = OutputType::Pulsing;
  mIdleState = aIdleState;
  mInitialState = aInitialState;
  mCycleTimeInMs = aCycleTimeInMs;
  mInitialTimeInMs = aInitialTimeInMs;
  reset_pulse();
}

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
  if(mType == OutputType::Pulsing)
  {
    if(true == mPulseRunning)
    {
      int lCurrentState = digitalRead(mPin);

      if(lCurrentState == mInitialState && mPulseTimer.delta() > mInitialTimeInMs)
      {
        digitalWrite(mPin, !lCurrentState);
      }
      else if(mPulseTimer.delta() > mCycleTimeInMs)
      {
        digitalWrite(mPin, mInitialState);
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
  if(mType != OutputType::Manual)
    return false;

  digitalWrite(mPin, aState);    

  return true;
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
bool OutputWrapper::start_pulse()
{
  if(mType != OutputType::Pulsing)
    return false;

    reset_pulse();
    digitalWrite(mPin, mInitialState);
    mPulseRunning = true;

  return true;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
bool OutputWrapper::reset_pulse()
{
  if(mType != OutputType::Pulsing)
    return false;

  mPulseRunning = false;
  mPulseTimer.reset();

  digitalWrite(mPin, mIdleState);      

  return true;
}
