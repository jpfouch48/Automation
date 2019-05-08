
#include "BallValveWrapper.h"
#include <Arduino.h>

// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::setup()
{
  pinMode(mInPin1, OUTPUT);
  pinMode(mInPin2, OUTPUT);
  reset();
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::loop()
{
  // Nothing to do here
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::set_open()
{
  digitalWrite(mInPin1, HIGH);
  digitalWrite(mInPin2, LOW);
  mState = State::Open;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::set_close()
{
  digitalWrite(mInPin1, LOW);
  digitalWrite(mInPin2, HIGH);
  mState = State::Close;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::set_idle()
{
  digitalWrite(mInPin1, LOW);
  digitalWrite(mInPin2, LOW);
  mState = State::Idle;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::set_state(State aState)
{
  switch(aState)
  {
    case State::Open:
      set_open();
    break;

    case State::Close:
      set_close();
    break;

    case State::Idle:
      set_idle();
    break;
  }
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::reset()
{
  set_close();
}