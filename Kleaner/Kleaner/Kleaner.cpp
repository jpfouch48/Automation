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
    mCo2Wrapper      (DO_PIN_RELAY_CO2,             LOW),
    mPumpWrapper     (DO_PIN_PUMP,                  LOW),
    mInWaterWrapper  (DO_PIN_RELAY_INPUT_WATER,     LOW),
    mInSaniWrapper   (DO_PIN_RELAY_INPUT_SANITIZER, LOW),
    mInCleanerWrapper(DO_PIN_RELAY_INPUT_CLEANER,   LOW),
    mReWasteWrapper  (DO_PIN_RECIRC_WASTE,          LOW),
    mReSaniWrapper   (DO_PIN_RECIRC_SANITIZER,      LOW),
    mReCleanerWrapper(DO_PIN_RECIRC_CLEANER,        LOW),

    // State init
    // States             Name            State Duration
    mSplashState          (F("Splash"),   2),
    mProcessInitState     (F("Init"),     0),
    mProcessPurgeState    (F("Purge"),    0),
    mProcessRinseState    (F("Rinse"),    0),
    mProcessSaniState     (F("Sani"),     0),
    mProcessWashState     (F("Wash"),     0),
    mProcessPressState    (F("Pressure"), 0),
    mProcessCompleteState (F("Complete"), 0),
    mMenuState            (F("Menu"),     0),

    // State Pointers
    mCurrentState(&mSplashState),
    mCommandState(NULL),
    mReturnToState(NULL),    
    mStateComplete(false),
    mFirstTimeInState(true),
    mInProcessDelay(false),
    mInProcessWaitForInput(false),
    mProcessDelayInSec(0),

    // Menu init
    mStartMenuItem(F("Start"), NULL, NULL),

    mCurrentMenuItem(&mStartMenuItem)
{
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

  // Init State
  mProcessInitState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mProcessInitState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,           0,  new String(F("Load Keg"))));
  mProcessInitState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,           1,  new String(F("Enter To Start"))));
  mProcessInitState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));

  // Purge State
  mProcessPurgeState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mProcessPurgeState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Recirc_Waste, HIGH));
  mProcessPurgeState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            10  ));
  mProcessPurgeState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,          HIGH));
  mProcessPurgeState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            10  ));
  mProcessPurgeState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,          LOW ));
  mProcessPurgeState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            10  ));
  mProcessPurgeState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off               ));
  
  // Rinse State
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Recirc_Waste, HIGH));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Water,  HIGH));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            15  ));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Pump,         HIGH));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            10  ));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Pump,         LOW ));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Water,  LOW ));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,          HIGH));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            10  ));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,          LOW ));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            10  ));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off               ));

  // Sani State
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Recirc_Sani,   HIGH));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Sani,    HIGH));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,             15  ));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Pump,          HIGH));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,             10  ));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Pump,          LOW ));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Sani,    LOW ));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,           HIGH));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,             10  ));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,           LOW ));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,             10  ));
  mProcessSaniState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off                ));

  // Wash State
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Recirc_Cleaner,HIGH));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Cleaner, HIGH));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,             15  ));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Pump,          HIGH));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,             10  ));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Pump,          LOW ));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Cleaner, LOW ));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,           HIGH));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,             10  ));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,           LOW ));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,             10  ));
  mProcessWashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off                ));

  // Pressurize State
  mProcessPressState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mProcessPressState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,          HIGH));
  mProcessPressState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            15  ));
  mProcessPressState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,          LOW ));

  // Complete State
  mProcessCompleteState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mProcessCompleteState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       0,  new String(F("Complete"))));
  mProcessCompleteState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       1,  new String(F("Enter To Restart"))));
  mProcessCompleteState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));

  mProcessStates.push_back(&mProcessInitState);
  mProcessStates.push_back(&mProcessPurgeState);
  mProcessStates.push_back(&mProcessRinseState);
  mProcessStates.push_back(&mProcessPurgeState);
  mProcessStates.push_back(&mProcessWashState);
  mProcessStates.push_back(&mProcessPurgeState);
  mProcessStates.push_back(&mProcessSaniState);
  mProcessStates.push_back(&mProcessPurgeState);
  mProcessStates.push_back(&mProcessPressState);
  mProcessStates.push_back(&mProcessCompleteState);

  // Default it to the end until we start the process from the start menu
  mProcessStateIter = mProcessStates.end();  
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
  mStateComplete = process_state(mCurrentState, mFirstTimeInState);

  // Reset our first time in state flag after we've porcessed the 
  // current state once
  if(true == mFirstTimeInState)
    mFirstTimeInState = false;  

  // Check to see if current state is completed.
  if(true == mStateComplete)
  {
    // Current state complete, setup next state

    // Check if return to state is set - if so, set 
    // it to our current state
    if(mReturnToState != NULL)
    {
      mCurrentState = mReturnToState;
      mReturnToState = NULL;
    }
    // Check if the command state is set - if so, set 
    // it to our current state
    else if (mCommandState != NULL)
    {
      mCurrentState = mCommandState;
      mCommandState = NULL;
    }
    // Check if we are processing the process state list
    // if so, set it current state and advance the iterator to
    // the next state    
    else if(mProcessStateIter != mProcessStates.end())
    {
      mCurrentState = *mProcessStateIter;
      mProcessStateIter++;
    } 
    else
    {
      // Reached the end of our prcoessing list, proceed back to 
      // menu state
      mCurrentState = &mMenuState;
    }
    
    // Set first time flag so new state can initialize
    mFirstTimeInState = true;
  }
  // A new state has been commanded, transition to it
  else if (mCommandState != NULL)
  {
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

  bool lProcessListComplete = false;

  // Check to see if this is our first time in this state
  if(aInitState)
  {
    // Reset the state timer
    mStateTimer.reset();

    // Reset component state flags for displaying status
    mPrevPumpState = -1;
    mPrevCo2State = -1;
    mPrevInWaterState = -1;
    mPrevInCleanerState = -1;
    mPrevInSanitizerState = -1;
    mPrevReWasteState = -1;
    mPrevReCleanerState = -1;
    mPrevReSanitizerState = -1;    

    // Initialize the state
    aState->init_state();

    // Reset the display and update with state name
    mDisplayWrapper.clear();
    mDisplayWrapper.display(0, aState->get_state_name());

    // State Specific initialization
    if(aState->get_id() == mSplashState.get_id())
    {
      mDisplayWrapper.display(0, F(SPLASH_LINE_1));
      mDisplayWrapper.display(1, F(SPLASH_LINE_2));
    }
    else if(true == is_process_state(aState->get_id()))
    {
      mDisplayWrapper.display(1, 0, F("I    R"), true);    
    }
    else if(aState->get_id() == mMenuState.get_id())
    {
      // Set the menu to the first option
      mCurrentMenuItem = &mStartMenuItem;
      mDisplayWrapper.display(1, 0, mCurrentMenuItem->get_title());
    }
  }
  else
  {
    // TODO: Generic processing

    if(true == mInProcessDelay)
    {
      // Check to see if our delay has elapsed
      if(mProcessDelayTimer.delta_in_secs() > mProcessDelayInSec)
      {
        mInProcessDelay = false;
        mProcessDelayInSec = 0;
      }
    }
    else if(true == mInProcessWaitForInput)
    {
      // Nothing to do here. Once enter button is pressed, this 
      // flag will be reset and processing will continue
    }
    else if(aState->process_list_iter() != aState->process_list().end())
    {
      ProcessStep *lCurrentStep = *aState->process_list_iter();

      switch(lCurrentStep->get_type())
      {
        case ProcessStep::ProcessType::All_Off:
          set_all_off();
        break;

        case ProcessStep::ProcessType::Set_Input_Water:
          mInWaterWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Input_Cleaner:
          mInCleanerWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Input_Sani:
          mInSaniWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Recirc_Waste:
          mReWasteWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Recirc_Cleaner:
          mReCleanerWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Recirc_Sani:
          mReSaniWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Pump:
          mPumpWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Co2:
          mCo2Wrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Wait_For_Input:
          mInProcessWaitForInput = true;
        break;

        case ProcessStep::ProcessType::Delay:
          mInProcessDelay = true;
          mProcessDelayInSec = lCurrentStep->get_value();
          mProcessDelayTimer.reset();
        break;

        case ProcessStep::ProcessType::Display:
          mDisplayWrapper.display(lCurrentStep->get_value(), *lCurrentStep->get_sz_value(), true);
        break;
        
        default:
          // TODO: Should not get here but lets flag an error
        break;
      }

      // Move to next step;
      aState->process_list_iter()++;
    }
    else
    {
      lProcessListComplete = true;
    }

    // TODO : State Specific processing
    if(true == is_process_state(aState->get_id()))
    {
      // 0123456789012345
      // ICSW RCSW P C
      update_output_display(mInCleanerWrapper, mPrevInCleanerState,   F("C"), 1);
      update_output_display(mInSaniWrapper,    mPrevInSanitizerState, F("S"), 2);
      update_output_display(mInWaterWrapper,   mPrevInWaterState,     F("W"), 3);

      update_output_display(mReCleanerWrapper, mPrevReCleanerState,   F("C"), 6);
      update_output_display(mReSaniWrapper,    mPrevReSanitizerState, F("S"), 7);
      update_output_display(mReWasteWrapper,   mPrevReWasteState,     F("W"), 8);

      update_output_display(mPumpWrapper,      mPrevPumpState,        F("P"), 10);
      update_output_display(mCo2Wrapper,       mPrevCo2State,         F("C"), 12);
    }
  }

  // Check to see if our processing time has completed 
  // Note if 0 value timer is disabled
  if(aState->get_state_time_in_sec() == 0 &&
     true == lProcessListComplete)
  {
    return true;
  }
  else if(aState->get_state_time_in_sec() != 0 &&
          aState->get_state_time_in_sec() < mStateTimer.delta_in_secs())
  {

    // Once splash state is complete, goto menu state
    if(aState->get_id() == mSplashState.get_id())
    {
      mCommandState = &mMenuState;
    }

    // TODO: State specific cleanup
    return true;
  }

  return false;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************   
void Kleaner::on_up_button(int aState)
{
  if(aState == LOW)
  {
    // Process menu menu option for menu and test menu state
    if(mCurrentState->get_id() == mMenuState.get_id())
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
    if(mCurrentState->get_id() == mMenuState.get_id())
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
    // Process Main Menu Options
    // ************************************************************************
    if(mCurrentState->get_id() == mMenuState.get_id())
    {
      // Start cleaning option
      if(mCurrentMenuItem->get_id() == mStartMenuItem.get_id())
      {
        mProcessStateIter = mProcessStates.begin();
      }
    }
    // Process Complete and Splash State Options
    // ************************************************************************
    else if(mCurrentState->get_id() == mSplashState.get_id())
    {
      mCommandState = &mMenuState;
    }

    if(true == mInProcessWaitForInput)
      mInProcessWaitForInput = false;
  }
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::update_output_display(const OutputWrapper &aOutputWrapper, int &aPrevState, String aDisplayVal, int aDisplayCol)
{
  int lCurrentState = aOutputWrapper.get_state();

  if(lCurrentState != aPrevState)
  {
    if(lCurrentState == HIGH)
      mDisplayWrapper.display(1, aDisplayCol, aDisplayVal, false);
    else
      mDisplayWrapper.display(1, aDisplayCol, F(" "), false);
    aPrevState = lCurrentState;
  }
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::set_all_off()          
{ 
  mCo2Wrapper.reset(); 
  mPumpWrapper.reset();
  mInWaterWrapper.reset();
  mInSaniWrapper.reset();
  mInCleanerWrapper.reset();
  mReWasteWrapper.reset();
  mReSaniWrapper.reset();
  mReCleanerWrapper.reset();
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
bool Kleaner::is_process_state(unsigned char aStateId)
{
  if(aStateId == mProcessInitState.get_id()  || 
     aStateId == mProcessPurgeState.get_id() ||
     aStateId == mProcessRinseState.get_id() ||
     aStateId == mProcessSaniState.get_id()  ||
     aStateId == mProcessWashState.get_id()  ||
     aStateId == mProcessPressState.get_id())
  {
    return true;
  }

  return false;
}
