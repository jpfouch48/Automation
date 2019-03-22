#include "Kleaner.h"
#include <Arduino.h>

#include "KleanerConfig.h"

//#define F(s)(s)

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
    mSplashState      (F("Splash"),     &mMenuState,       2),

    mDumpState        (F("Dump"),       &mPreRinseState,   DUMP_DURATION,       DUMP_INPUT,       DUMP_RECIRC,       DUMP_PUMP,       DUMP_CO2,       true),
    mPreRinseState    (F("Pre"),        &mPurge1State,     PRE_RINSE_DURATION,  PRE_RINSE_INPUT,  PRE_RINSE_RECIRC,  PRE_RINSE_PUMP,  PRE_RINSE_CO2,  true),    
    mPurge1State      (F("Pre-Purge"),  &mWashState,       PURGE_DURATION,      PURGE_INPUT,      PURGE_RECIRC,      PURGE_PUMP,      PURGE_CO2,      true),
    mWashState        (F("Wash"),       &mPurge2State,     WASH_DURATION,       WASH_INPUT,       WASH_RECIRC,       WASH_PUMP,       WASH_CO2,       true),
    mPurge2State      (F("Wash-Purge"), &mPostRinseState,  PURGE_DURATION,      PURGE_INPUT,      PURGE_RECIRC,      PURGE_PUMP,      PURGE_CO2,      true),
    mPostRinseState   (F("Post"),       &mPurge3State,     POST_RINSE_DURATION, POST_RINSE_INPUT, POST_RINSE_RECIRC, POST_RINSE_PUMP, POST_RINSE_CO2, true),
    mPurge3State      (F("Post-Purge"), &mSanitizeState,   PURGE_DURATION,      PURGE_INPUT,      PURGE_RECIRC,      PURGE_PUMP,      PURGE_CO2,      true),
    mSanitizeState    (F("Sani"),       &mPurge4State,     SANI_DURATION,       SANI_INPUT,       SANI_RECIRC,       SANI_PUMP,       SANI_CO2,       true),
    mPurge4State      (F("Sani-Purge"), &mPressurizeState, PURGE_DURATION,      PURGE_INPUT,      PURGE_RECIRC,      PURGE_PUMP,      PURGE_CO2,      true),
    mPressurizeState  (F("Press"),      &mCompleteState,   PRESS_DURATION,      PRESS_INPUT,      PRESS_RECIRC,      PRESS_PUMP,      PRESS_CO2,      true),

    mCompleteState    (F("Complete"), NULL),

        
    // Menu States
    mMenuState          (F("Menu"), NULL),
#if defined KLEANER_TEST_MENU    
    mTestMenuState      (F("Test Menu"), NULL),
#endif
#if defined KLEANER_TEST_STATE_MENU    
    mTestStateMenuState (F("Test State"), NULL),
#endif

    mCurrentState(&mSplashState),
    mCommandState(NULL),
    mReturnToState(NULL),    
    mStateComplete(false),
    mFirstTimeInState(true),

    // Menu init
#if defined KLEANER_TEST_MENU  
    mStartMenuItem         (F("Start"),           NULL,                 &mTestMenuItem),    
#elif defined KLEANER_TEST_STATE_MENU
    mStartMenuItem         (F("Start"),           NULL,                 &mTestStateMenuItem),    
#else
    mStartMenuItem         (F("Start"),           NULL,                 NULL),    
#endif

#if defined KLEANER_TEST_MENU  
  #if defined KLEANER_TEST_STATE_MENU
    mTestMenuItem          (F("Test Menu"),       &mStartMenuItem,      &mTestStateMenuItem),
  #else
    mTestMenuItem          (F("Test Menu"),       &mStartMenuItem,      NULL),
  #endif
    mTestMenuCycleInput    (F(" Cycle Input"),  NULL,                  &mTestMenuCycleRecirc),
    mTestMenuCycleRecirc   (F(" Cycle Recirc"), &mTestMenuCycleInput,  &mTestMenuTogglePump),
    mTestMenuTogglePump    (F(" Toggle Pump"),  &mTestMenuCycleRecirc, &mTestMenuToggleCo2),
    mTestMenuToggleCo2     (F(" Toggle Co2"),   &mTestMenuTogglePump,  &mTestMenuExit),
    mTestMenuExit          (F(" <BACK>"),       &mTestMenuToggleCo2,   NULL),
#endif

#if defined KLEANER_TEST_STATE_MENU
  #if defined KLEANER_TEST_MENU
    mTestStateMenuItem       (F("Test State Menu"), &mTestMenuItem,       NULL),
  #else
    mTestStateMenuItem       (F("Test State Menu"), &mStartMenuItem,       NULL),
  #endif
    mTestStateMenuDump       (F(" Dump"),   NULL,                      &mTestStateMenuPreRinse),
    mTestStateMenuPreRinse   (F(" Pre"),    &mTestStateMenuDump,       &mTestStateMenuWash),
    mTestStateMenuWash       (F(" Wash"),   &mTestStateMenuPreRinse,   &mTestStateMenuPostRinse),
    mTestStateMenuPostRinse  (F(" Post"),   &mTestStateMenuWash,       &mTestStateMenuSanitize),
    mTestStateMenuSanitize   (F(" Sani"),   &mTestStateMenuPostRinse,  &mTestStateMenuPressurize),
    mTestStateMenuPressurize (F(" Press"),  &mTestStateMenuSanitize,   &mTestStateMenuExit),
    mTestStateMenuExit       (F(" <BACK>"), &mTestStateMenuPressurize, NULL),
#endif
    mCurrentMenuItem(&mStartMenuItem)
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
    if(mCurrentState->get_id() == mMenuState.get_id() 
#if defined KLEANER_TEST_MENU        
       || mCurrentState->get_id() == mTestMenuState.get_id()
#endif
#if defined KLEANER_TEST_STATE_MENU
       || mCurrentState->get_id() == mTestStateMenuState.get_id()
#endif
      )
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
    if(mCurrentState->get_id() == mMenuState.get_id() 
#if defined KLEANER_TEST_MENU        
       || mCurrentState->get_id() == mTestMenuState.get_id()
#endif
#if defined KLEANER_TEST_STATE_MENU
       || mCurrentState->get_id() == mTestStateMenuState.get_id()
#endif       
      )
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
        mCommandState = &mDumpState;
      }
#if defined KLEANER_TEST_MENU       
      // Goto Test Menu
      else if (mCurrentMenuItem->get_id() == mTestMenuItem.get_id())
      {
        mCommandState = &mTestMenuState;      
      }
#endif

#if defined KLEANER_TEST_STATE_MENU      
      else if (mCurrentMenuItem->get_id() == mTestStateMenuItem.get_id())
      {
        mCommandState = &mTestStateMenuState;          
      }
#endif 
    }
#if defined KLEANER_TEST_MENU    
    else if(mCurrentState->get_id() == mTestMenuState.get_id())
    {
      // Cycle Input
      if(mCurrentMenuItem->get_id() == mTestMenuCycleInput.get_id())
      {
        // Cycle Input
        mTestInputSource = cycle_input_source(mTestInputSource);
        set_input(mTestInputSource);

        switch(mTestInputSource)
        {
          case InputSource::Cleaner:   mDisplayWrapper.display(1, 15, F("C"), false); break;
          case InputSource::Sanitizer: mDisplayWrapper.display(1, 15, F("S"), false); break;
          case InputSource::Water:     mDisplayWrapper.display(1, 15, F("W"), false); break;
          case InputSource::None:      mDisplayWrapper.display(1, 15, F(" "), false); break;
          default:                     mDisplayWrapper.display(1, 15, F("?"), false); break;
        };        
      }
      // Cycle Recirc
      else if (mCurrentMenuItem->get_id() == mTestMenuCycleRecirc.get_id())
      {
        // Cycle Recirc   
        mTestRecircDest = cycle_recirc_desk(mTestRecircDest);
        set_recirc(mTestRecircDest);

        switch(mTestRecircDest)
        {
          case RecircDest::Cleaner:   mDisplayWrapper.display(1, 15, F("C"), false); break;
          case RecircDest::Sanitizer: mDisplayWrapper.display(1, 15, F("S"), false); break;
          case RecircDest::Waste:     mDisplayWrapper.display(1, 15, F("W"), false); break;
          case RecircDest::None:      mDisplayWrapper.display(1, 15, F(" "), false); break;
          default:                    mDisplayWrapper.display(1, 15, F("?"), false); break;
        };
      }
      // Toggle Pump
      else if (mCurrentMenuItem->get_id() == mTestMenuTogglePump.get_id())
      {
        // Toggle Pump
        if(mPumpWrapper.is_pulse_running())
        {
          mPumpWrapper.reset_pulse();
        }
        else
        {
          mPumpWrapper.start_pulse();
        }        
      }
      // Toggle Co2
      else if (mCurrentMenuItem->get_id() == mTestMenuToggleCo2.get_id())
      {
        // Toggle Pump
        if(mCo2Wrapper.is_pulse_running())
        {
          mCo2Wrapper.reset_pulse();
        }
        else
        {
          mCo2Wrapper.start_pulse();
        }
      }
      // Exit Test Menu
      else if (mCurrentMenuItem->get_id() == mTestMenuExit.get_id())
      {
        mCommandState = &mMenuState;
      }
    }
#endif

#if defined KLEANER_TEST_STATE_MENU
    else if(mCurrentState->get_id() == mTestStateMenuState.get_id())
    {    
      if (mCurrentMenuItem->get_id() == mTestStateMenuDump.get_id())
      {
        mCommandState = &mDumpState;
        mReturnToState = &mTestStateMenuState;
      }      
      else if (mCurrentMenuItem->get_id() == mTestStateMenuPreRinse.get_id())
      {
        mCommandState = &mPreRinseState;
        mReturnToState = &mTestStateMenuState;
      }      
      else if (mCurrentMenuItem->get_id() == mTestStateMenuWash.get_id())
      {
        mCommandState = &mWashState;
        mReturnToState = &mTestStateMenuState;
      }      
      else if (mCurrentMenuItem->get_id() == mTestStateMenuPostRinse.get_id())
      {
        mCommandState = &mPostRinseState;
        mReturnToState = &mTestStateMenuState;
      }       
      else if (mCurrentMenuItem->get_id() == mTestStateMenuSanitize.get_id())
      {
        mCommandState = &mSanitizeState;
        mReturnToState = &mTestStateMenuState;
      }          
      else if (mCurrentMenuItem->get_id() == mTestStateMenuPressurize.get_id())
      {
        mCommandState = &mPressurizeState;
        mReturnToState = &mTestStateMenuState;
      }       
      else if (mCurrentMenuItem->get_id() == mTestStateMenuExit.get_id())
      {
        mCommandState = &mMenuState;
      }      
    }    
#endif
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
    if(mReturnToState == NULL)
    {
      mCurrentState = mCurrentState->get_next_state();
    }
    else 
    {
      mCurrentState = mReturnToState;
      mReturnToState = NULL;
    }
    
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
#if defined KLEANER_TEST_MENU 
    else if(aState->get_id() == mTestMenuState.get_id())
    {
      // Set the menu to the first option
      mCurrentMenuItem = &mTestMenuCycleInput;
      mDisplayWrapper.display(1, 0, mCurrentMenuItem->get_title());

      mTestInputSource = InputSource::None;
      mTestRecircDest  = RecircDest::None;

      // Timeline - 0123456789
      // PUMP     -    XXXXX           
      // CO2      - XXXXX
      static OutputWrapper::Config mPumpConfig(LOW, HIGH, 10000, 5000, 3000);
      static OutputWrapper::Config mCo2Config(LOW,  HIGH, 10000, 5000, 0);

      mCo2Wrapper.update_config(mCo2Config);
      mPumpWrapper.update_config(mPumpConfig);
    }
#endif
#if defined KLEANER_TEST_STATE_MENU
    else if(aState->get_id() == mTestStateMenuState.get_id())
    {
      // Set the menu to the first option
      mCurrentMenuItem = &mTestStateMenuDump;
      mDisplayWrapper.display(1, 0, mCurrentMenuItem->get_title());
    }
#endif
  }
  else
  {
    // TODO: Generic processing
    // TODO : State Specific processing
    if(true == aState->get_is_process_state())
    {
      mDisplayWrapper.display(1, 0, mCurrentMenuItem->get_title());
    }
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
