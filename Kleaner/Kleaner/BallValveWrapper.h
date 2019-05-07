#ifndef BALL_VALVE_WRAPPER_H
#define BALL_VALVE_WRAPPER_H

#include <Arduino.h>

// ****************************************************************************
// BallValveWrapper class
// ****************************************************************************
// Description:
//  This class is a wrapper to control a motorized ball valve. We are using an
//  L298N motor driver to control our valves and each L298N can handle upto
//  two valves. This wrapper will contol 1 of the motors on the driver. 
// 
// Notes:
//  [1] The current designe does not utilize the en pin to control speed. It
//      is assumed that the L298N is configured with the en pin jummpers to
//      provide full power when opening or clsing the valve.
// ****************************************************************************
class BallValveWrapper
{
  public:
    // ************************************************************************
    // State enumeration
    //
    // Description:
    //  This enumeration defines the current state of the valve. The valve can
    //  be opened, closed, idle or unknown. 
    //    Open: The outputs are set to open the valve
    //    Close: The outputs are set to close the valve
    //    Idle: The outputs are set to idel state (both outputs off)
    //    Unknow: The state of the outputs are unknow. This is the inital state
    //      after construction until setup. It is not a valid command when
    //      telling the class to act.
    // ************************************************************************
    enum class State
    {
      Idle,
      Open,
      Close,

      Unknown,
    };

    // ************************************************************************
    // BallValveWrapper - Constructor 
    // ************************************************************************
    // Description:
    //
    // ************************************************************************
    // Parameters:
    //
    // ************************************************************************
    BallValveWrapper(int aIn1Pin, int aIn2Pin) :
      mInPin1(aIn1Pin),
      mInPin2(aIn2Pin),
      mState(State::Unknown)
    {
    }

    // ************************************************************************
    //
    // ************************************************************************
    void setup();

    // ************************************************************************
    //
    // ************************************************************************
    void loop();

    // ************************************************************************
    //
    // ************************************************************************
    void set_open();

    // ************************************************************************
    //
    // ************************************************************************
    void set_close();

    // ************************************************************************
    //
    // ************************************************************************
    void set_idle();

    // ************************************************************************
    //
    // ************************************************************************
    void reset();

    // ************************************************************************
    //
    // ************************************************************************
    State get_state()
    {
      return mState;
    }

  protected:

  private:
    int     mInPin1;
    int     mInPin2;
    State   mState;
};
#endif