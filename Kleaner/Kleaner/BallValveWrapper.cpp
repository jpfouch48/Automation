
#include "BallValveWrapper.h"
#include <Arduino.h>

// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::setup()
{
  pinMode(mPin1, OUTPUT);
  pinMode(mPin2, OUTPUT);
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
  digitalWrite(mPin1, HIGH);
  digitalWrite(mPin2, LOW);
  mState = State::Open;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::set_close()
{
  digitalWrite(mPin1, LOW);
  digitalWrite(mPin2, HIGH);
  mState = State::Close;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::set_idle()
{
  digitalWrite(mPin1, LOW);
  digitalWrite(mPin2, LOW);
  mState = State::Idle;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void BallValveWrapper::reset()
{
  set_idle();
}