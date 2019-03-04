#include "InputWrapper.h"


// ****************************************************************************
// See header file for details
// ****************************************************************************
InputWrapper::InputWrapper(int                  aPin, 
                           unsigned char        aInputMode, 
                           unsigned long        aDebounceDelay,
                           InputChangedCallback aInputChanedCallback) :
  mPin(aPin),
  mDebounceDelay(aDebounceDelay),
  mInputMode(aInputMode),
  mLastState(LOW),
  mDebounceTime(0),
  mInputChanedCallback(aInputChanedCallback),
  mSkipFirst(true)
{
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void InputWrapper::setup()
{
  pinMode(mPin, mInputMode);
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void InputWrapper::loop()
{
  // Get the current state
  int lReading = digitalRead(mPin);

  // If state has changed, reset our debounce time
  if(lReading != mLastState)
    mDebounceTime = millis();

  // Check to see if we have reached our debounce delay
  if((millis() - mDebounceTime) > mDebounceDelay)
  {
    // Delay has been reached, check to see if 
    // we are transitioning to a new state of the input
    if(lReading != mCurrentState)
    {
      // Set our current reading
      mCurrentState = lReading;

      // Trigger state change callback
      if(NULL != mInputChanedCallback && false == mSkipFirst)
        mInputChanedCallback(mCurrentState);

      if(true == mSkipFirst)
        mSkipFirst = false;
    }
  }

  mLastState = lReading;
}