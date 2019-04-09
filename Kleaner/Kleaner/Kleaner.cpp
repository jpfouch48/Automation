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
    mCo2Wrapper(DO_PIN_RELAY_CO2, LOW),
    mPumpWrapper(DO_PIN_PUMP, LOW),
    mInWaterWrapper(DO_PIN_RELAY_INPUT_WATER, LOW),
    mInSaniWrapper(DO_PIN_RELAY_INPUT_SANITIZER, LOW),
    mInCleanerWrapper(DO_PIN_RELAY_INPUT_CLEANER, LOW),
    mReWasteWrapper(DO_PIN_RECIRC_WASTE, LOW),
    mReSaniWrapper(DO_PIN_RECIRC_SANITIZER, LOW),
    mReCleanerWrapper(DO_PIN_RECIRC_CLEANER, LOW),

    // State init
    mSplashState      (F("Splash"),     &mMenuState,       2),

    mDumpState        (F("Dump"),       &mPreRinseState,   DURATION_DUMP,       INPUT_DUMP,       RECIRC_DUMP,       PUMP_CFG_DUMP,       CO2_CFG_DUMP,       INPUT_CFG_DUMP,       true),
    mPreRinseState    (F("Pre"),        &mPurge1State,     DURATION_PRE_RINSE,  INPUT_PRE_RINSE,  RECIRC_PRE_RINSE,  PUMP_CFG_PRE_RINSE,  CO2_CFG_PRE_RINSE,  INPUT_CFG_PRE_RINSE,  true),    
    mPurge1State      (F("Pre-Purge"),  &mWashState,       DURATION_PURGE,      INPUT_PURGE,      RECIRC_PURGE,      PUMP_CFG_PURGE,      CO2_CFG_PURGE,      INPUT_CFG_PURGE,      true),
    mWashState        (F("Wash"),       &mPurge2State,     DURATION_WASH,       INPUT_WASH,       RECIRC_WASH,       PUMP_CFG_WASH,       CO2_CFG_WASH,       INPUT_CFG_WASH,       true),
    mPurge2State      (F("Wash-Purge"), &mPostRinseState,  DURATION_PURGE,      INPUT_PURGE,      RECIRC_PURGE,      PUMP_CFG_PURGE,      CO2_CFG_PURGE,      INPUT_CFG_PURGE,      true),
    mPostRinseState   (F("Post"),       &mPurge3State,     DURATION_POST_RINSE, INPUT_POST_RINSE, RECIRC_POST_RINSE, PUMP_CFG_POST_RINSE, CO2_CFG_POST_RINSE, INPUT_CFG_POST_RINSE, true),
    mPurge3State      (F("Post-Purge"), &mSanitizeState,   DURATION_PURGE,      INPUT_PURGE,      RECIRC_PURGE,      PUMP_CFG_PURGE,      CO2_CFG_PURGE,      INPUT_CFG_PURGE,      true),
    mSanitizeState    (F("Sani"),       &mPurge4State,     DURATION_SANI,       INPUT_SANI,       RECIRC_SANI,       PUMP_CFG_SANI,       CO2_CFG_SANI,       INPUT_CFG_SANI,       true),
    mPurge4State      (F("Sani-Purge"), &mPressurizeState, DURATION_PURGE,      INPUT_PURGE,      RECIRC_PURGE,      PUMP_CFG_PURGE,      CO2_CFG_PURGE,      INPUT_CFG_PURGE,      true),
    mPressurizeState  (F("Press"),      &mCompleteState,   DURATION_PRESS,      INPUT_PRESS,      RECIRC_PRESS,      PUMP_CFG_PRESS,      CO2_CFG_PRESS,      INPUT_CFG_PRESS,      true),

    mCompleteState    (F("Complete"),   NULL),

    mPrimePumpState   (F("Prime"),      NULL,              DURATION_PRIME,      INPUT_PRIME,      RECIRC_PRIME,      PUMP_CFG_PRIME,      CO2_CFG_PRIME,      INPUT_CFG_PRIME,      false),

        
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
    mStartMenuItem         (F("Start"),           NULL,                 &mPrimePumpMenuItem),
    
#if defined KLEANER_TEST_MENU  
    mPrimePumpMenuItem     (F("Prime"),           &mStartMenuItem,      &mTestMenuItem),
#elif defined KLEANER_TEST_STATE_MENU
    mPrimePumpMenuItem     (F("Prime"),           &mStartMenuItem,      &mTestStateMenuItem),
#else
    mPrimePumpMenuItem     (F("Prime"),           &mStartMenuItem,      NULL),
#endif

#if defined KLEANER_TEST_MENU  
  #if defined KLEANER_TEST_STATE_MENU
    mTestMenuItem          (F("Test Menu"),       &mPrimePumpMenuItem,      &mTestStateMenuItem),
  #else
    mTestMenuItem          (F("Test Menu"),       &mPrimePumpMenuItem,      NULL),
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
    mTestStateMenuItem       (F("Test State Menu"), &mPrimePumpMenuItem,       NULL),
  #endif
    mTestStateMenuDump       (F(" Dump"),   NULL,                      &mTestStateMenuPreRinse),
    mTestStateMenuPreRinse   (F(" Pre"),    &mTestStateMenuDump,       &mTestStateMenuWash),
    mTestStateMenuWash       (F(" Wash"),   &mTestStateMenuPreRinse,   &mTestStateMenuPostRinse),
    mTestStateMenuPostRinse  (F(" Post"),   &mTestStateMenuWash,       &mTestStateMenuSanitize),
    mTestStateMenuSanitize   (F(" Sani"),   &mTestStateMenuPostRinse,  &mTestStateMenuPressurize),
    mTestStateMenuPressurize (F(" Press"),  &mTestStateMenuSanitize,   &mTestStateMenuPurge),
    mTestStateMenuPurge      (F(" Purge"),  &mTestStateMenuPressurize, &mTestStateMenuExit),
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
      else if(mCurrentMenuItem->get_id() == mPrimePumpMenuItem.get_id())
      {
        mCommandState = &mPrimePumpState;
        mReturnToState = &mMenuState;
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
      else if (mCurrentMenuItem->get_id() == mTestStateMenuPurge.get_id())
      {
        mCommandState = &mPurge1State;
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
  // Setup Display Wrapper and turn on back light
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

  // Process the current state
  process_state();
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::process_state()
{
  mStateComplete = false;

  mStateComplete = process_state(mCurrentState, mFirstTimeInState);

  // Reset our first time in state flag after we've porcessed the 
  // current state once
  if(true == mFirstTimeInState)
    mFirstTimeInState = false;  

  // Check to see if current state is completed. If so, setup the next state
  // and set our first time in state flag.
  if(true == mStateComplete)
  {
    set_all_off();

    // If return to state pointer is not set, get the next state from the 
    // current state. Otherwise, set the current state to the return to
    // state pointer.
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
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
bool Kleaner::process_state(const KleanerState *aState, bool aInitState)
{
  // TODO: ERROR CHECK FOR NULL STATE

  if(aInitState)
  {
    mPrevPumpState = -1;
    mPrevCo2State = -1;
    mPrevInputState = -1;
    mPrevRecircState = -1;    

    mStateTimer.reset();

    // Generic initialization
    mDisplayWrapper.clear();
    mDisplayWrapper.display(0, aState->get_state_name());
        
    // Set the input source
    switch(aState->get_input_source())
    {
      case InputSource::Cleaner:
        mInCleanerWrapper.update_config(*aState->get_input_config());
        if(aState->get_input_config()->get_type() == OutputWrapper::Config::Type::Pulsing)
          mInCleanerWrapper.start_pulse();
      break;

      case InputSource::Sanitizer:
        mInSaniWrapper.update_config(*aState->get_input_config());
        if(aState->get_input_config()->get_type() == OutputWrapper::Config::Type::Pulsing)
          mInSaniWrapper.start_pulse();
      break;

      case InputSource::Water:
        mInWaterWrapper.update_config(*aState->get_input_config());
        if(aState->get_input_config()->get_type() == OutputWrapper::Config::Type::Pulsing)
          mInWaterWrapper.start_pulse();
      break;

      case InputSource::None:
      default:
        set_input(InputSource::None);
      break;
    }

    // Setup recirc dest
    set_recirc(aState->get_recirc_dest());

    // Setup Co2 processing
    mCo2Wrapper.update_config(*aState->get_co2_config());
    if(mCo2Wrapper.get_config()->get_type() == OutputWrapper::Config::Type::Pulsing)
      mCo2Wrapper.start_pulse();

    // Setup pump processing
    mPumpWrapper.update_config(*aState->get_pump_config());
    if(mPumpWrapper.get_config()->get_type() == OutputWrapper::Config::Type::Pulsing)
      mPumpWrapper.start_pulse();

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
    static int gTempState;

    // TODO: Generic processing
    // TODO : State Specific processing
    
    if(true == aState->get_is_process_state())
    {
      // 0123456789012345
      // IX RX P C 

      // Update input state display if it has changed
      switch(aState->get_input_source())
      {
        case InputSource::Cleaner:
          gTempState = mInCleanerWrapper.get_state();
          if(gTempState != mPrevInputState)
          {
            if(gTempState == HIGH)
              mDisplayWrapper.display(1, 0, F("IC"), false);
            else
              mDisplayWrapper.display(1, 0, F("I "), false);
            mPrevInputState = gTempState;
          }
        break;

        case InputSource::Sanitizer:
          gTempState = mInSaniWrapper.get_state();
          if(gTempState != mPrevInputState)
          {
            if(gTempState == HIGH)
              mDisplayWrapper.display(1, 0, F("IS"), false);
            else
              mDisplayWrapper.display(1, 0, F("I "), false);
            mPrevInputState = gTempState;
          }
        break;

        case InputSource::Water:
          gTempState = mInWaterWrapper.get_state();
          if(gTempState != mPrevInputState)
          {
            if(gTempState == HIGH)
              mDisplayWrapper.display(1, 0, F("IW"), false);
            else
              mDisplayWrapper.display(1, 0, F("I "), false);
            mPrevInputState = gTempState;
          }
        break;

        case InputSource::None:
          if(-1 == mPrevInputState)
            mDisplayWrapper.display(1, 0, F("I "), false);
          mPrevInputState = 1;
        break;
        
        default:
          if(-1 == mPrevInputState)
            mDisplayWrapper.display(1, 0, F("I?"), false);
          mPrevInputState = 1;
        break;
      }

      // Update recirc display if it has changed
      switch(aState->get_recirc_dest())
      {
        case RecircDest::Cleaner:
          gTempState = mReCleanerWrapper.get_state();
          if(gTempState != mPrevRecircState)
          {
            if(gTempState == HIGH)
              mDisplayWrapper.display(1, 3, F("RC"), false);
            else
              mDisplayWrapper.display(1, 3, F("R "), false);
            mPrevRecircState = gTempState;
          }
        break;

        case RecircDest::Sanitizer:
          gTempState = mReSaniWrapper.get_state();
          if(gTempState != mPrevRecircState)
          {
            if(gTempState == HIGH)
              mDisplayWrapper.display(1, 3, F("RS"), false);
            else
              mDisplayWrapper.display(1, 3, F("R "), false);
            mPrevRecircState = gTempState;
          }        
        break;

        case RecircDest::Waste:
          gTempState = mReWasteWrapper.get_state();
          if(gTempState != mPrevRecircState)
          {
            if(gTempState == HIGH)
              mDisplayWrapper.display(1, 3, F("RW"), false);
            else
              mDisplayWrapper.display(1, 3, F("R "), false);
            mPrevRecircState = gTempState;
          }            
        break;

        case RecircDest::None:
          if(-1 == mPrevRecircState)
            mDisplayWrapper.display(1, 3, F("R "), false);     
          mPrevRecircState = 1;  
        break;
        
        default:
          if(-1 == mPrevRecircState)
            mDisplayWrapper.display(1, 3, F("R?"), false);         
          mPrevRecircState = 1;  
        break;
      }

      // Update pump state display if it has changed
      gTempState = mPumpWrapper.get_state();
      if(gTempState != mPrevPumpState)
      {
        if(gTempState == HIGH)
          mDisplayWrapper.display(1, 6, F("P"), false);
        else  
          mDisplayWrapper.display(1, 6, F(" "), false);

        mPrevPumpState = gTempState;
      }

      // Update C02 state display if it has changed
      gTempState = mCo2Wrapper.get_state();
      if(gTempState != mPrevCo2State)
      {
        if(gTempState == HIGH)
          mDisplayWrapper.display(1, 8, F("C"), false);
        else  
          mDisplayWrapper.display(1, 8, F(" "), false);

        mPrevCo2State = gTempState;
      }      
    }
  }

  // Check to see if our processing time has completed 
  // Note if 0 value timer is disabled
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
  // Revert back to manual ouptut wrapper and for all as this function
  // does not work with pulsing outputs
  mInCleanerWrapper.update_config(OutputWrapper::Config(LOW));
  mInSaniWrapper.update_config(OutputWrapper::Config(LOW));
  mInWaterWrapper.update_config(OutputWrapper::Config(LOW));

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

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::set_all_off()          
{ 
  mCo2Wrapper.reset_pulse(); 
  mPumpWrapper.reset_pulse();

  set_input(InputSource::None);
  set_recirc(RecircDest::None); 
}
