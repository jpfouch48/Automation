#include "Kleaner.h"
#include <Arduino.h>

#include "KleanerConfig.h"

// ****************************************************************************
// See header file for details
// ****************************************************************************
Kleaner::Kleaner() : 
    // Input wrapper init
    mCo2Wrapper(DO_PIN_RELAY_CO2, LOW, HIGH, 5, 3),
    mPumpWrapper(DO_PIN_PUMP, LOW),
    
    mInWaterWrapper(DO_PIN_RELAY_INPUT_WATER, LOW),
    mInSaniWrapper(DO_PIN_RELAY_INPUT_SANITIZER, LOW),
    mInCleanerWrapper(DO_PIN_RELAY_INPUT_CLEANER, LOW),
    mReWasteWrapper(DO_PIN_RECIRC_WASTE, LOW),
    mReSaniWrapper(DO_PIN_RECIRC_SANITIZER, LOW),
    mReCleanerWrapper(DO_PIN_RECIRC_CLEANER, LOW),

    // State init
    mCurrentState(&mMenuState),
    mCommandState(NULL),
    mMenuState        (0, "Menu      ", NULL),
    mPurgeState       (1, "Purge     ", &mPreRinseState,   20, InputSource::None,      RecircDest::Waste),
    mPreRinseState    (2, "Pre Rinse ", &mWashState,       20, InputSource::Water,     RecircDest::Waste),
    mWashState        (3, "Wash      ", &mPostRinseState,  30, InputSource::Cleaner,   RecircDest::Cleaner),
    mPostRinseState   (4, "Post Rinse", &mSanitizeState,   20, InputSource::Water,     RecircDest::Waste),
    mSanitizeState    (5, "Sanitize  ", &mPressurizeState, 30, InputSource::Sanitizer, RecircDest::Sanitizer),
    mPressurizeState  (6, "Pressurize", &mCompleteState,   10, InputSource::None,      RecircDest::None),
    mCompleteState    (7, "Complete  ", NULL),
    mTestMenuState    (8, "Test Menu ", NULL),
    mStateComplete(false),
    mFirstTimeInState(true),

    // Display init
    mLCD(DO_PIN_DISPLAY, LCD_ROW_COUNT, LCD_COL_COUNT),

    // Menu init
    mCurrentMenuItem(&mStartMenuItem),
    mStartMenuItem         (0, " Start          ",  NULL,                 &mTestMenuItem),
    mTestMenuItem          (1, " Test Menu ...  ",  &mStartMenuItem,      NULL),

    mTestMenuCycleInput    (0, " Cycle Input    ", NULL,                  &mTestMenuCycleRecirc),
    mTestMenuCycleRecirc   (1, " Cycle Recirc   ", &mTestMenuCycleInput,  &mTestMenuTogglePump),
    mTestMenuTogglePump    (2, " Toggle Pump    ", &mTestMenuCycleRecirc, &mTestMenuToggleCo2),
    mTestMenuToggleCo2     (3, " Toggle Co2     ", &mTestMenuTogglePump,  &mTestMenuExit),
    mTestMenuExit          (4, " <Exit Menu>    ", &mTestMenuToggleCo2,   NULL)
{

}

// ****************************************************************************
// See header file for details
// ****************************************************************************   
void Kleaner::on_up_button(int aState)
{
//  Serial.print("Kleaner::on_up_button - ");
//  Serial.println(aState);  

  if(aState == LOW)
  {
    // Process menu menu option for menu and test menu state
    if(mCurrentState->get_id() == mMenuState.get_id() ||
       mCurrentState->get_id() == mTestMenuState.get_id())
    {
      if(mCurrentMenuItem->get_prev_item() != NULL)
      {
        mCurrentMenuItem = mCurrentMenuItem->get_prev_item();
        mLCD.at(1, 0, mCurrentMenuItem->get_title());
      }
    } 
  }
}

// ****************************************************************************
// See header file for details
// ****************************************************************************   
void Kleaner::on_dn_button(int aState)
{
//  Serial.print("Kleaner::on_dn_button - ");
//  Serial.println(aState);

  if(aState == LOW)
  {
    // Process menu menu option for menu and test menu state
    if(mCurrentState->get_id() == mMenuState.get_id() ||
       mCurrentState->get_id() == mTestMenuState.get_id())
    {
      if(mCurrentMenuItem->get_next_item() != NULL)
      {
        mCurrentMenuItem = mCurrentMenuItem->get_next_item();
        mLCD.at(1, 0, mCurrentMenuItem->get_title());
      }
    } 
  }
}

// ****************************************************************************
// See header file for details
// ****************************************************************************   
void Kleaner::on_en_button(int aState)
{
//  Serial.print("Kleaner::on_en_button - ");
//  Serial.println(aState);

  if(aState == LOW)
  {
    // Process menu option for menu state
    if(mCurrentState->get_id() == mMenuState.get_id())
    {
      if(mCurrentMenuItem->get_id() == mStartMenuItem.get_id())
      {
        mCommandState = &mPurgeState;
      }
      else if (mCurrentMenuItem->get_id() == mTestMenuItem.get_id())
      {
        mCommandState = &mTestMenuState;      
      }
    } 
    else if(mCurrentState->get_id() == mTestMenuItem.get_id())
    {
      if(mCurrentMenuItem->get_id() == mTestMenuCycleInput.get_id())
      {
        // TODO - Cycle Input
//        if(mTestInputSource == InputSource::None)
//          mTestInputSource = (InputSource)0;
//        else
//          mTestInputSource = (InputSource)((int)mTestInputSource)++;

      }
      else if (mCurrentMenuItem->get_id() == mTestMenuCycleRecirc.get_id())
      {
        // TODO - Cycle Recirc   
      }
      else if (mCurrentMenuItem->get_id() == mTestMenuTogglePump.get_id())
      {
        // TODO - Toggle Pump
      }
      else if (mCurrentMenuItem->get_id() == mTestMenuToggleCo2.get_id())
      {
        // TODO - Toggle Co2
      }
      else if (mCurrentMenuItem->get_id() == mTestMenuExit.get_id())
      {
        mCommandState = &mMenuState;
      }
    }
    else if(mCurrentState->get_id() == mCompleteState.get_id())
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
//  Serial.println("Kleaner::setup() called");
  mLCD.setup();

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
  //Serial.println("Klenaer::loop() called");

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
    mLCD.empty();
    mLCD.at(0,0, aState->get_state_name());

    InputSource lInput = aState->get_input_source();
    RecircDest  lRecirc = aState->get_recirc_dest();

    switch(lInput)
    {
      case InputSource::Cleaner:   set_input_cleaner();   break;
      case InputSource::Sanitizer: set_input_sanitizer(); break;
      case InputSource::Water:     set_input_water();     break;
      default:                     set_input_off();       break;
    };

    switch(lRecirc)
    {
      case RecircDest::Cleaner:   set_recirc_cleaner();   break;
      case RecircDest::Sanitizer: set_recirc_sanitizer(); break;
      case RecircDest::Waste:     set_recirc_waste();     break;
      default:                    set_recirc_off();       break;
    };

    // TODO : State Specific initialization
    if(aState->get_id() == mMenuState.get_id())
    {
      // Set the menu to the first option
      mCurrentMenuItem = &mStartMenuItem;
      mLCD.at(1, 0, mCurrentMenuItem->get_title());
    }
    else if(aState->get_id() == mTestMenuState.get_id())
    {
      // Set the menu to the first option
      mCurrentMenuItem = &mTestMenuCycleInput;
      mLCD.at(1, 0, mCurrentMenuItem->get_title());

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
//    Serial.print("Complete - ");
//    Serial.print(aState->get_state_name());
//    Serial.print(" ");
//    Serial.print(aState->get_state_time_in_sec());
//    Serial.print(" - ");
//    Serial.println(mStateTimer.delta_in_secs());

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

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::set_pump_off()         { mPumpWrapper.set_state(LOW); }
void Kleaner::set_pump_on()          { mPumpWrapper.set_state(HIGH); }

void Kleaner::set_all_off()          { mCo2Wrapper.reset_pulse(); set_pump_off(); set_input_off(); set_recirc_off(); }
