#include "Kleaner.h"
#include <Arduino.h>

#include "KleanerConfig.h"

// ****************************************************************************
// See header file for details
// ****************************************************************************
Kleaner::Kleaner() : 
    // Display wrapper
    mDisplayWrapper(DO_PIN_DISPLAY, LCD_ROW_COUNT, LCD_COL_COUNT),

    // Input wrapper init
    // Note: Timing values will be updated based on state configuration
    mCo2Wrapper(DO_PIN_RELAY_CO2, LOW, HIGH, 5, 3),
    mPumpWrapper(DO_PIN_PUMP, LOW, HIGH, 5, 3),

    mInWaterWrapper(DO_PIN_RELAY_INPUT_WATER, LOW),
    mInSaniWrapper(DO_PIN_RELAY_INPUT_SANITIZER, LOW),
    mInCleanerWrapper(DO_PIN_RELAY_INPUT_CLEANER, LOW),
    mReWasteWrapper(DO_PIN_RECIRC_WASTE, LOW),
    mReSaniWrapper(DO_PIN_RECIRC_SANITIZER, LOW),
    mReCleanerWrapper(DO_PIN_RECIRC_CLEANER, LOW),

    // State init
    mSplashState      ("Splash,   ", &mMenuState,       2),
    mPurgeState       ("Purge     ", &mPreRinseState,   
                       20,       
                       InputSource::None,
                       RecircDest::Waste,     
              /*PUMP*/ NULL,
               /*CO2*/ new OutputWrapper::Config(LOW, HIGH, 10000, 5000)),
    // Timeline - 0123456789
    // PUMP     -           
    // CO2      - XXXXX

    mPreRinseState    ("Pre Rinse ", &mWashState,       
                       20,       
                       InputSource::Water,     RecircDest::Waste,     
              /*PUMP*/ new OutputWrapper::Config(LOW, HIGH,  10000, 5000, 2000), 
               /*CO2*/ new OutputWrapper::Config(LOW, LOW,   10000, 8000)),
    // Timeline - 0123456789
    // PUMP     -   XXXXX           
    // CO2      -         XX
    
    mWashState        ("Wash      ", &mPostRinseState,  
                       30,       
                       InputSource::Cleaner,
                       RecircDest::Cleaner,   
              /*PUMP*/ new OutputWrapper::Config(LOW, HIGH,  10000, 5000, 2000), 
               /*CO2*/ new OutputWrapper::Config(LOW, LOW,   10000, 8000)),
    // Timeline - 0123456789
    // PUMP     -   XXXXX           
    // CO2      -         XX

    mPostRinseState   ("Post Rinse", &mSanitizeState,   
                       20,       
                       InputSource::Water,
                       RecircDest::Waste,     
              /*PUMP*/ new OutputWrapper::Config(LOW, HIGH,  10000, 5000, 2000), 
               /*CO2*/ new OutputWrapper::Config(LOW, LOW,   10000, 8000)),
    // Timeline - 0123456789
    // PUMP     -   XXXXX           
    // CO2      -         XX
               
    mSanitizeState    ("Sanitize  ", &mPressurizeState, 
                       30,       
                       InputSource::Sanitizer, 
                       RecircDest::Sanitizer, 
              /*PUMP*/ new OutputWrapper::Config(LOW, HIGH,  10000, 5000, 2000), 
               /*CO2*/ new OutputWrapper::Config(LOW, LOW,   10000, 8000)),
    // Timeline - 0123456789
    // PUMP     -   XXXXX           
    // CO2      -         XX

    mPressurizeState  ("Pressurize", &mCompleteState,   
                       10,       
                       InputSource::None,
                       RecircDest::None,      
              /*PUMP*/ NULL,                                              
               /*CO2*/ new OutputWrapper::Config(LOW, HIGH, 10000, 2000)),
    mCompleteState    ("Complete  ", NULL),
    // Timeline - 0123456789
    // PUMP     -           
    // CO2      - XX
        
    // Menu States
    mMenuState        ("Menu      ", NULL),
    mTestMenuState    ("Test Menu ", NULL),

    mCurrentState(&mSplashState),
    mCommandState(NULL),
    mStateComplete(false),
    mFirstTimeInState(true),

    // Menu init
    mCurrentMenuItem(&mStartMenuItem),
    mStartMenuItem         (" Start          ",  NULL,                 &mTestMenuItem),
    mTestMenuItem          (" Test Menu ...  ",  &mStartMenuItem,      NULL),

    mTestMenuCycleInput    (" Cycle Input    ", NULL,                  &mTestMenuCycleRecirc),
    mTestMenuCycleRecirc   (" Cycle Recirc   ", &mTestMenuCycleInput,  &mTestMenuTogglePump),
    mTestMenuTogglePump    (" Toggle Pump    ", &mTestMenuCycleRecirc, &mTestMenuToggleCo2),
    mTestMenuToggleCo2     (" Toggle Co2     ", &mTestMenuTogglePump,  &mTestMenuExit),
    mTestMenuExit          (" <Exit Menu>    ", &mTestMenuToggleCo2,   NULL)
{

}

// ****************************************************************************
// See header file for details
// ****************************************************************************   
void Kleaner::on_up_button(int aState)
{
  if(aState == LOW)
  {
    // Process menu menu option for menu and test menu state
    if(mCurrentState->get_id() == mMenuState.get_id() ||
       mCurrentState->get_id() == mTestMenuState.get_id())
    {
      if(mCurrentMenuItem->get_prev_item() != NULL)
      {
        mCurrentMenuItem = mCurrentMenuItem->get_prev_item();
        mDisplayWrapper.display(1, 0, mCurrentMenuItem->get_title());
      }
    } 
  }
}

// ****************************************************************************
// See header file for details
// ****************************************************************************   
void Kleaner::on_dn_button(int aState)
{
  if(aState == LOW)
  {
    // Process menu menu option for menu and test menu state
    if(mCurrentState->get_id() == mMenuState.get_id() ||
       mCurrentState->get_id() == mTestMenuState.get_id())
    {
      if(mCurrentMenuItem->get_next_item() != NULL)
      {
        mCurrentMenuItem = mCurrentMenuItem->get_next_item();
        mDisplayWrapper.display(1, 0, mCurrentMenuItem->get_title());
      }
    } 
  }
}

// ****************************************************************************
// See header file for details
// ****************************************************************************   
void Kleaner::on_en_button(int aState)
{
  if(aState == LOW)
  {
    // Process menu option for menu state
    if(mCurrentState->get_id() == mMenuState.get_id())
    {
      // Start Cleaning
      if(mCurrentMenuItem->get_id() == mStartMenuItem.get_id())
      {
        mCommandState = &mPurgeState;
      }
      // Goto Test Menu
      else if (mCurrentMenuItem->get_id() == mTestMenuItem.get_id())
      {
        mCommandState = &mTestMenuState;      
      }
    } 
    else if(mCurrentState->get_id() == mTestMenuState.get_id())
    {
      // Cycle Input
      if(mCurrentMenuItem->get_id() == mTestMenuCycleInput.get_id())
      {
        // Cycle Input
        mTestInputSource = cycle_input_source(mTestInputSource);
        set_input(mTestInputSource);
      }
      // Cycle Recirc
      else if (mCurrentMenuItem->get_id() == mTestMenuCycleRecirc.get_id())
      {
        // Cycle Recirc   
        mTestRecircDest = cycle_recirc_desk(mTestRecircDest);
        set_recirc(mTestRecircDest);
      }
      // Toggle Pump
      else if (mCurrentMenuItem->get_id() == mTestMenuTogglePump.get_id())
      {
        // Toggle Pump
        if(mPumpWrapper.is_pulse_running())
        {
          mPumpWrapper.start_pulse();
        }
        else
        {
          mPumpWrapper.reset_pulse();
        }        
      }
      // Toggle Co2
      else if (mCurrentMenuItem->get_id() == mTestMenuToggleCo2.get_id())
      {
        // Toggle Pump
        if(mCo2Wrapper.is_pulse_running())
        {
          mCo2Wrapper.start_pulse();
        }
        else
        {
          mCo2Wrapper.reset_pulse();
        }
      }
      // Exit Test Menu
      else if (mCurrentMenuItem->get_id() == mTestMenuExit.get_id())
      {
        mCommandState = &mMenuState;
      }
    }
    else if(mCurrentState->get_id() == mCompleteState.get_id() ||
            mCurrentState->get_id() == mSplashState.get_id())
    {
      mCommandState = &mMenuState;
    }
  }
}

// ****************************************************************************
// See header file for details
// ****************************************************************************    
void Kleaner::setup()
{
  // Display Wrapper
  mDisplayWrapper.setup();

  mDisplayWrapper.backlight_on(true);

  // Setup output wrappers
  mCo2Wrapper.setup();
  mPumpWrapper.setup();
  mInWaterWrapper.setup();
  mInSaniWrapper.setup();
  mInCleanerWrapper.setup();
  mReWasteWrapper.setup();
  mReSaniWrapper.setup();
  mReCleanerWrapper.setup();
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::loop()
{
  // Display Wrapper processing
  mDisplayWrapper.loop();

  // Output wrapper processing
  mCo2Wrapper.loop();
  mPumpWrapper.loop();
  mInWaterWrapper.loop();
  mInSaniWrapper.loop();
  mInCleanerWrapper.loop();
  mReWasteWrapper.loop();
  mReSaniWrapper.loop();
  mReCleanerWrapper.loop();

  process_state();
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::process_state()
{
  mStateComplete = false;

  if(true == mFirstTimeInState)
  {
    mStateTimer.reset();
  }

  mStateComplete = process_state(mCurrentState, mFirstTimeInState);

  if(true == mStateComplete)
  {
    mCurrentState = mCurrentState->get_next_state();
    mFirstTimeInState = true;
  }
  // A new state has been commanded, transition to it
  else if (mCommandState != NULL)
  {
    set_all_off();
    mCurrentState = mCommandState;
    mCommandState = NULL;
    mFirstTimeInState = true;
  }
  // Still processing current state
  else
  {
    if(true == mFirstTimeInState)
      mFirstTimeInState = false;
  }
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
bool Kleaner::process_state(const KleanerState *aState, bool aInitState)
{
  // TODO: ERROR CHECK FOR NULL STATE

  if(aInitState)
  {
    Serial.print("Init - ");
    Serial.println(aState->get_state_name());
  
    // Generic initialization
    mDisplayWrapper.clear();
    mDisplayWrapper.display(0, aState->get_state_name());

    set_input(aState->get_input_source());
    set_recirc(aState->get_recirc_dest());

    if(NULL != aState->get_co2_config())
    {
      mCo2Wrapper.update_config(*aState->get_co2_config());
      if(mCo2Wrapper.get_config()->get_type() == OutputWrapper::Config::Type::Pulsing)
        mCo2Wrapper.start_pulse();
    }

    if(NULL != aState->get_pump_config())
    {
      mPumpWrapper.update_config(*aState->get_pump_config());

      if(mPumpWrapper.get_config()->get_type() == OutputWrapper::Config::Type::Pulsing)
        mPumpWrapper.start_pulse();
    }


    // State Specific initialization
    if(aState->get_id() == mSplashState.get_id())
    {
      mDisplayWrapper.display(0, String(SPLASH_LINE_1));
      mDisplayWrapper.display(1, String(SPLASH_LINE_2));
    }
    else if(aState->get_id() == mMenuState.get_id())
    {
      // Set the menu to the first option
      mCurrentMenuItem = &mStartMenuItem;
      mDisplayWrapper.display(1, 0, mCurrentMenuItem->get_title());
    }
    else if(aState->get_id() == mTestMenuState.get_id())
    {
      // Set the menu to the first option
      mCurrentMenuItem = &mTestMenuCycleInput;
      mDisplayWrapper.display(1, 0, mCurrentMenuItem->get_title());

      mTestInputSource = InputSource::None;
      mTestRecircDest = RecircDest::None;
    }
  }
  else
  {
    // TODO: Generic processing

    // TODO : State Specific processing

  }

  // Check to see if our processing time has completed 
  // Note if -1 value timer is disabled
  if(aState->get_state_time_in_sec() != 0 &&
     aState->get_state_time_in_sec() < mStateTimer.delta_in_secs())
  {
    // Generic cleanup
    set_all_off();

    // TODO: State specific cleanup

    return true;
  }

  return false;
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::set_input(InputSource aInputSource)
{
  switch(aInputSource)
  {
    case InputSource::Water:     set_input_water();     break;
    case InputSource::Sanitizer: set_input_sanitizer(); break;
    case InputSource::Cleaner:   set_input_cleaner();   break;
    case InputSource::None:      set_input_off();       break;
    default:                     set_input_off();       break;
  };
}
void Kleaner::set_input_off()        { mInWaterWrapper.set_state(LOW); mInSaniWrapper.set_state(LOW); mInCleanerWrapper.set_state(LOW); }
void Kleaner::set_input_water()      { set_input_off(); mInWaterWrapper.set_state(HIGH); }
void Kleaner::set_input_cleaner()    { set_input_off(); mInCleanerWrapper.set_state(HIGH); }
void Kleaner::set_input_sanitizer()  { set_input_off(); mInSaniWrapper.set_state(HIGH); }

// ****************************************************************************
// See header file for details
// ****************************************************************************
// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::set_recirc(RecircDest aRecircDest)
{
  switch(aRecircDest)
  {
    case RecircDest::Waste:     set_recirc_waste();     break;
    case RecircDest::Sanitizer: set_recirc_sanitizer(); break;
    case RecircDest::Cleaner:   set_recirc_cleaner();   break;
    case RecircDest::None:      set_recirc_off();       break;
    default:                    set_recirc_off();       break;
  };
}
void Kleaner::set_recirc_off()       { mReWasteWrapper.set_state(LOW); mReSaniWrapper.set_state(LOW); mReCleanerWrapper.set_state(LOW); }
void Kleaner::set_recirc_waste()     { set_recirc_off(); mReWasteWrapper.set_state(HIGH); }
void Kleaner::set_recirc_sanitizer() { set_recirc_off(); mReSaniWrapper.set_state(HIGH); }
void Kleaner::set_recirc_cleaner()   { set_recirc_off(); mReCleanerWrapper.set_state(HIGH); }

void Kleaner::set_all_off()          { mCo2Wrapper.reset_pulse(); mPumpWrapper.reset_pulse(); set_input_off(); set_recirc_off(); }
