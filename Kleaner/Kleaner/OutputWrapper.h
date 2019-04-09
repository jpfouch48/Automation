#ifndef OUTPUT_WRAPPER_H
#define OUTPUT_WRAPPER_H

#include <Arduino.h>
#include "MilliTimer.h"

class OutputWrapper
{
  public:
    // ************************************************************************
    // 
    // ************************************************************************
    class Config
    {
      public:
        // ********************************************************************
        // OutputType - Enumeration
        // ********************************************************************
        // Manual: 
        //  A manual output type is used when the user of the class wants to 
        //  manually set the state of the output via the provided set_state 
        //  command.
        //
        // Pulsing:
        //  A pulsing output is used when the caller wants the state of the 
        //  output to be controlled via a timer. The output will pulse on and 
        //  off based on the provided parameters.
        // ********************************************************************
        enum class Type
        {
          Manual, 
          Pulsing,
          Disabled
        };

        // ********************************************************************
        // 
        // ********************************************************************
        Config() :
          mIdleState(LOW),
          mInitialState(LOW),
          mType(Type::Disabled),
          mCycleTimeInMs(0),
          mInitialStateTimeInMs(0),
          mInitialDelayInMs(0)
        {
        }

        // ********************************************************************
        // 
        // ********************************************************************
        Config(int aIdleState) :
          mIdleState(aIdleState),
          mInitialState(aIdleState),
          mType(Type::Manual),
          mCycleTimeInMs(0),
          mInitialStateTimeInMs(0),
          mInitialDelayInMs(0)
        {
        }

        // ************************************************************************
        // 
        // ************************************************************************
        Config(int aIdleState,
               int aInitialState, 
               int aCycleTimeInMs, 
               int aInitialStateTimeInMs,
               int aInitialDelayInMs=0) :
          mIdleState(aIdleState),
          mInitialState(aInitialState),
          mType(Type::Pulsing),
          mCycleTimeInMs(aCycleTimeInMs),
          mInitialStateTimeInMs(aInitialStateTimeInMs),
          mInitialDelayInMs(aInitialDelayInMs)               
        {
        }

        Type get_type()                     const { return mType; }
        int  get_idle_state()               const { return mIdleState; }
        int  get_initial_state()            const { return mInitialState; }
        int  get_cycle_time_in_ms()         const { return mCycleTimeInMs; }
        int  get_initial_state_time_in_ms() const { return mInitialStateTimeInMs; }
        int  get_initial_delay_time_in_ms() const { return mInitialDelayInMs; }

      protected:
      private:
        int  mPin;
        Type mType;
        int  mIdleState;
        int  mInitialState; 
        int  mCycleTimeInMs; 
        int  mInitialStateTimeInMs;
        int  mInitialDelayInMs;
    };

    // ************************************************************************
    // OuputWrapper - Constructor (Manual Output)
    // ************************************************************************
    // Description:
    //  Constructs a manual type Output wrapper. A manual type is intialized
    //  with the idel state during setup and then the state of the output pin
    //  is commanded by using the set_state() operation to set the pin to
    //  the desired state (e.g. HIGH or LOW)
    //
    // ************************************************************************
    // Parameters:
    //  aPin:
    //   output pin being used
    //  aIdleState:
    //   output pin is set to this value during setup processing
    // ************************************************************************
    OutputWrapper(int aPin, 
                  int aIdleState) :
      mPin(aPin),
      mConfig(aIdleState),
      mPulseRunning(false)
    {
    }

    // ************************************************************************
    // OuputWrapper - Constructor (Pulsing Output)
    // ************************************************************************
    // Description:
    //  Constructs a pulse type Output wrapper. A pulse type output will be
    //  controlled via a timer along with the pulse configuration to determine
    //  when the output is set to the desired states.
    // ************************************************************************
    // Parameters:
    //  aPin:
    //   output pin being used
    //  aIdleState:
    //   output pin is set to this value during the setup processing. It will
    //   also be set to this state when the pulse is reset.
    //  aInitialState:
    //   output pin is set to this initialily when the pulse is started. It is
    //   held at this position based on aInitialTimeInMS parameter. Once that
    //   time has elapsed, the value is toggled and will remain in the toggled
    //   state for the remainder of the cycle is elapsed (aCycleTimInMs). Once
    //   the cycle time has elapsed, the timer is reset and the cycle starts 
    //   again until the pulse is either reset or stopped.
    //  aCycleTimeInMs:
    //   duration of one single pulse cycle.
    //  aInitialTimeInMs:
    //   duration the output pin is held at the aInitialState before it is
    //   toggled.
    // ************************************************************************
    // Example:
    //  aPin: 0
    //  aIdleState: LOW
    //  aInitialState: HIGH
    //  aCycleTimeInMs: 5000 (5 seconds)
    //  aInitialTimeInMs: 2000 (2 secons)
    //
    //  1) Setup is call:     Pin 0 is set to LOW (Idle State)
    //  2) Pulse is started:  Pin 0 is set to HIGH (Initial State)
    //  3) After 2 seconds:   Pin 0 is set to LOW  (inital time has elapsed)
    //  4) After 3 seconds:   Cycle time has elapsed (5 second cycle time)
    //                        Timer will reset and we will return to step 2   
    // ************************************************************************
    OutputWrapper(int aPin, 
                  int aIdleState,
                  int aInitialState, 
                  int aCycleTimeInMs, 
                  int aInitialStateTimeInMs,
                  int aInitialDelayInMs=0) :
      mPin(aPin),
      mConfig(aIdleState, 
              aInitialState, 
              aCycleTimeInMs, 
              aInitialStateTimeInMs,
              aInitialDelayInMs),
      mPulseRunning(false),
      mInDelay(false)
    {
    }

    // ************************************************************************
    //
    // ************************************************************************
    OutputWrapper(Config aConfig) :
      mConfig(aConfig),
      mPulseRunning(false),
      mInDelay(false)
    {
    }

    // ************************************************************************
    //
    // ************************************************************************
    void update_config(Config aConfig);

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
    // - used for manual type only
    // ************************************************************************
    bool set_state(int aState);

    // ************************************************************************
    //
    // ************************************************************************
    int get_state() { digitalRead(mPin); }

    // ************************************************************************
    //
    // - used for pulse type only
    // ************************************************************************
    bool start_pulse();

    // ************************************************************************
    //
    // - used for pulse type only
    // ************************************************************************
    bool reset_pulse();

    // ************************************************************************
    //
    // - used for pulse type only
    // ************************************************************************
    bool is_pulse_running() { return mPulseRunning; }


    int get_pin() const { return mPin; }
    const Config* get_config() const { return &mConfig; }

  protected:

  private:
    int        mPin;
    Config     mConfig;
    bool       mInDelay;
    bool       mPulseRunning;
    MilliTimer mPulseTimer;
};
#endif