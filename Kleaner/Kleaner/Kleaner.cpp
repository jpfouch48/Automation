#include "Kleaner.h"
#include <Arduino.h>

#include "KleanerConfig.h"

//#define F(s)(s)

// ****************************************************************************
// See header file for details
// ****************************************************************************
Kleaner::Kleaner() : 

    // Input wrapper init
    // Note: Timing values will be updated based on state configuration
    mCo2Wrapper      (DO_PIN_RELAY_CO2,             LOW),
    mPumpWrapper     (DO_PIN_PUMP,                  LOW),
    
    mInSaniWrapper   (DO_PIN_RELAY_INPUT_SANITIZER, LOW),
    mInCleanerWrapper(DO_PIN_RELAY_INPUT_CLEANER,   LOW),

    mInWaterWrapper  (DO_PIN_MOTOR_INPUT_WATER_1, DO_PIN_MOTOR_INPUT_WATER_2),


    mReWasteWrapper  (DO_PIN_RECIRC_WASTE,          LOW),
    mReSaniWrapper   (DO_PIN_RECIRC_SANITIZER,      LOW),
    mReCleanerWrapper(DO_PIN_RECIRC_CLEANER,        LOW),

    // States
    mSplashState          ("Splash"),
    mMenuState            ("Menu"),
    mTestState            ("Test"),
    mConfirmState         ("Confirm"),
    
    
    mProcessInitState     ("Init"),
    mProcessPurgeState    ("Purge"),
    mProcessRinseState    ("Rinse"),
    mProcessSaniState     ("Sani"),
    mProcessWashState     ("Wash"),
    mProcessPressState    ("Pressure"),
    mCompleteState        ("Complete"),

    // State Pointers - We are starting with the splash screen
    // once the splash is done, the menu state is initiated
    mCurrentState(&mSplashState),
    mReturnToState(NULL),    
    mCommandState(NULL),
    mStateComplete(false),
    mFirstTimeInState(true),
    mInProcessDelay(false),
    mInProcessWaitForInput(false),
    mProcessDelayInSec(0),
    mNextionWrapper(PAGE_ID_HOME)
{
    // Reset component state flags for displaying status
    mPrevPumpState = -1;
    mPrevCo2State = -1;
    mPrevInWaterState = BallValveWrapper::State::Unknown;
    mPrevInCleanerState = -1;
    mPrevInSanitizerState = -1;
    mPrevReWasteState = -1;
    mPrevReCleanerState = -1;
    mPrevReSanitizerState = -1;  
    mPrevStatePercentComplete = -1;     
    mPrevProcessPercentComplete = -1;  
}

// ****************************************************************************
// See header file for details
// ****************************************************************************    
void Kleaner::setup()
{
  TPRINTLN(F("Kleaner::setup - enter"));

  // Init Nextion
  mNextionWrapper.setup(this);  

  // Setup output wrappers
  mCo2Wrapper.setup();
  mPumpWrapper.setup();
  mInWaterWrapper.setup();
  mInSaniWrapper.setup();
  mInCleanerWrapper.setup();
  mReWasteWrapper.setup();
  mReSaniWrapper.setup();
  mReCleanerWrapper.setup();

  // Splash State
  mSplashState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_HOME));
  mSplashState.add_process_step(new ProcessStepDisplayText("t0",  KLEANER_VERSION));
  mSplashState.add_process_step(new ProcessStepResetOutputs());
  mSplashState.add_process_step(new ProcessStepDelay(5));

  // Menu State
  mMenuState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_MAIN));
  mMenuState.add_process_step(new ProcessStepWaitForInput());

  // Confirm State
  mConfirmState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_CONFIRM));
  mConfirmState.add_process_step(new ProcessStepWaitForInput());

  // Init State
  mProcessInitState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_PROCESS));
  mProcessInitState.add_process_step(new ProcessStepDisplayValue("j0", 0));
  mProcessInitState.add_process_step(new ProcessStepDisplayValue("j1", 0));
  mProcessInitState.add_process_step(new ProcessStepDisplayText("t0",  "Init"));
  mProcessInitState.add_process_step(new ProcessStepResetOutputs());
  mProcessInitState.add_process_step(new ProcessStepDelay(5));

  // Purge State
  mProcessPurgeState.add_process_step(new ProcessStepDisplayText("t0",  "Purge"));
  mProcessPurgeState.add_process_step(new ProcessStepControlOutputWrapper(&mReWasteWrapper, HIGH));
  mProcessPurgeState.add_process_step(new ProcessStepDelay(10));
  mProcessPurgeState.add_process_step(new ProcessStepControlOutputWrapper(&mCo2Wrapper, HIGH));
  mProcessPurgeState.add_process_step(new ProcessStepDelay(10));
  mProcessPurgeState.add_process_step(new ProcessStepControlOutputWrapper(&mCo2Wrapper, LOW));
  mProcessPurgeState.add_process_step(new ProcessStepControlOutputWrapper(&mReWasteWrapper, LOW));
  mProcessPurgeState.add_process_step(new ProcessStepDelay(5));
  
  // Rinse State
  mProcessRinseState.add_process_step(new ProcessStepDisplayText("t0",  "Rinse"));
  mProcessRinseState.add_process_step(new ProcessStepControlOutputWrapper(&mReWasteWrapper, HIGH));
  mProcessRinseState.add_process_step(new ProcessStepControlBallValveWrapper(&mInWaterWrapper, BallValveWrapper::State::Open));
  mProcessRinseState.add_process_step(new ProcessStepDelay(15));
  mProcessRinseState.add_process_step(new ProcessStepControlOutputWrapper(&mPumpWrapper, HIGH));
  mProcessRinseState.add_process_step(new ProcessStepDelay(10));
  mProcessRinseState.add_process_step(new ProcessStepControlOutputWrapper(&mPumpWrapper, LOW));
  mProcessRinseState.add_process_step(new ProcessStepControlBallValveWrapper(&mInWaterWrapper, BallValveWrapper::State::Close));
  mProcessRinseState.add_process_step(new ProcessStepDelay(5));
  mProcessRinseState.add_process_step(new ProcessStepControlOutputWrapper(&mCo2Wrapper, HIGH));
  mProcessRinseState.add_process_step(new ProcessStepDelay(10));
  mProcessRinseState.add_process_step(new ProcessStepControlOutputWrapper(&mCo2Wrapper, LOW ));
  mProcessRinseState.add_process_step(new ProcessStepControlOutputWrapper(&mReWasteWrapper, LOW));

  // Sani State
  mProcessSaniState.add_process_step(new ProcessStepDisplayText("t0",  "Sanitize"));
  mProcessSaniState.add_process_step(new ProcessStepControlOutputWrapper(&mReSaniWrapper, HIGH));
  mProcessSaniState.add_process_step(new ProcessStepControlOutputWrapper(&mInSaniWrapper, HIGH));
  mProcessSaniState.add_process_step(new ProcessStepDelay(15));
  mProcessSaniState.add_process_step(new ProcessStepControlOutputWrapper(&mPumpWrapper, HIGH));
  mProcessSaniState.add_process_step(new ProcessStepDelay(10));
  mProcessSaniState.add_process_step(new ProcessStepControlOutputWrapper(&mPumpWrapper, LOW ));
  mProcessSaniState.add_process_step(new ProcessStepControlOutputWrapper(&mInSaniWrapper, LOW ));
  mProcessSaniState.add_process_step(new ProcessStepControlOutputWrapper(&mCo2Wrapper, HIGH));
  mProcessSaniState.add_process_step(new ProcessStepDelay(10));
  mProcessSaniState.add_process_step(new ProcessStepControlOutputWrapper(&mCo2Wrapper, LOW ));
  mProcessSaniState.add_process_step(new ProcessStepDelay(10));
  mProcessSaniState.add_process_step(new ProcessStepResetOutputs());
  mProcessSaniState.add_process_step(new ProcessStepDelay(10));

  // Wash State
  mProcessWashState.add_process_step(new ProcessStepDisplayText("t0",  "Wash"));
  mProcessWashState.add_process_step(new ProcessStepControlOutputWrapper(&mReCleanerWrapper, HIGH));
  mProcessWashState.add_process_step(new ProcessStepControlOutputWrapper(&mInCleanerWrapper, HIGH));
  mProcessWashState.add_process_step(new ProcessStepDelay(15));
  mProcessWashState.add_process_step(new ProcessStepControlOutputWrapper(&mPumpWrapper, HIGH));
  mProcessWashState.add_process_step(new ProcessStepDelay(10));
  mProcessWashState.add_process_step(new ProcessStepControlOutputWrapper(&mPumpWrapper, LOW));
  mProcessWashState.add_process_step(new ProcessStepControlOutputWrapper(&mInCleanerWrapper, LOW));
  mProcessWashState.add_process_step(new ProcessStepControlOutputWrapper(&mCo2Wrapper, HIGH));
  mProcessWashState.add_process_step(new ProcessStepDelay(10));
  mProcessWashState.add_process_step(new ProcessStepControlOutputWrapper(&mCo2Wrapper, LOW ));
  mProcessWashState.add_process_step(new ProcessStepDelay(10));
  
  // Pressurize State
  mProcessPressState.add_process_step(new ProcessStepDisplayText("t0",  "Pressurize"));
  mProcessPressState.add_process_step(new ProcessStepResetOutputs());
  mProcessPressState.add_process_step(new ProcessStepControlOutputWrapper(&mCo2Wrapper, HIGH));
  mProcessPressState.add_process_step(new ProcessStepDelay(15));
  mProcessPressState.add_process_step(new ProcessStepControlOutputWrapper(&mCo2Wrapper, LOW));

  // Complete State
  mCompleteState.add_process_step(new ProcessStepDisplayText("t0",  "Complete"));
  mCompleteState.add_process_step(new ProcessStepResetOutputs());
  mCompleteState.add_process_step(new ProcessStepDelay(5));
  mCompleteState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_COMPLETE));
  mCompleteState.add_process_step(new ProcessStepWaitForInput());


  // Test state - will be removed once HMI is working
  mTestState.add_process_step(new ProcessStepResetOutputs());
  mTestState.add_process_step(new ProcessStepControlBallValveWrapper(&mInWaterWrapper, BallValveWrapper::State::Open));
  mTestState.add_process_step(new ProcessStepWaitForInput());
  mTestState.add_process_step(new ProcessStepControlBallValveWrapper(&mInWaterWrapper, BallValveWrapper::State::Close));
  mTestState.add_process_step(new ProcessStepWaitForInput());


  // Setup the process state list
  mProcessStates.push_back(&mProcessInitState);
  mProcessStates.push_back(&mProcessPurgeState);
  mProcessStates.push_back(&mProcessRinseState);
  mProcessStates.push_back(&mProcessPurgeState);
  mProcessStates.push_back(&mProcessWashState);
  mProcessStates.push_back(&mProcessPurgeState);
  mProcessStates.push_back(&mProcessSaniState);
  mProcessStates.push_back(&mProcessPurgeState);
  mProcessStates.push_back(&mProcessPressState);
  mProcessStates.push_back(&mCompleteState);

  // Default it to the end until we start the process from the start menu
  mProcessStateIter = mProcessStates.end();  

  TPRINTLN(F("Kleaner::setup - exit"));
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::loop()
{
  mNextionWrapper.loop();

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
  {
    mFirstTimeInState = false;
  }  

  // A new state has been commanded, transition to it
  if (mCommandState != NULL)
  {
    TPRINT(F("Commanding State: "));
    TPRINTLN(mCommandState->get_state_name());

    mCurrentState = mCommandState;
    mCommandState = NULL;
    mFirstTimeInState = true;
    mInProcessWaitForInput = false;
    mInProcessDelay = false;
    mProcessStateIter = mProcessStates.end();      
  }
  // Check to see if current state is completed.
  else if(true == mStateComplete)
  {
    TPRINT(F("Complete State: "));
    TPRINTLN(mCurrentState->get_state_name());

    // Current state complete, setup next state

    // Check if return to state is set - if so, set 
    // it to our current state
    if(mReturnToState != NULL)
    {
      mCurrentState = mReturnToState;
      mReturnToState = NULL;
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
    TPRINT(F("Init State: "));
    TPRINT(F(" ID: "));
    TPRINT(aState->get_id());
    TPRINT(F(" Duration: "));
    TPRINT(aState->get_total_process_time_in_sec());
    TPRINT(F(" Name: "));
    TPRINTLN(aState->get_state_name());

    // Reset the state timer
    mStateTimer.reset(); 

    // Initialize the state
    aState->init_state();

    mCurrentStateTimer.reset();
  }
  else
  {
    // TODO: Generic processing

    if(true == mInProcessDelay)
    {
      // Check to see if our delay has
      if(mProcessDelayTimer.delta_in_secs() > mProcessDelayInSec)
      {
        TPRINT(aState->get_state_name());
        TPRINTLN(F(" - Delay Complete"));

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
        case ProcessStep::Type::Reset_Outputs:
          TPRINTLN(F("All OFF"));
          set_all_off();
        break;

        case ProcessStep::Type::Wait_For_Input:
          TPRINTLN(F("Wait for input "));
          mInProcessWaitForInput = true;
        break;

        case ProcessStep::Type::Delay:
        {
          const ProcessStepDelay *lStep = (ProcessStepDelay *)lCurrentStep;
          TPRINT(F("Delay "));
          TPRINTLN(lStep->get_delay());

          mInProcessDelay = true;
          mProcessDelayInSec = lStep->get_delay();
          mProcessDelayTimer.reset();
        } break;

        case ProcessStep::Type::Display_Page:
        {
          const ProcessStepDisplayPage *lStep = (ProcessStepDisplayPage *)lCurrentStep;
          mNextionWrapper.set_page(lStep->get_page_id());
        } break;

        case ProcessStep::Type::Display_Text:
        {
          const ProcessStepDisplayText *lStep = (ProcessStepDisplayText *)lCurrentStep;
          mNextionWrapper.set_text(lStep->get_comp(), lStep->get_value());
        } break;

        case ProcessStep::Type::Display_Value:
        {
          const ProcessStepDisplayValue *lStep = (ProcessStepDisplayValue *)lCurrentStep;
          mNextionWrapper.set_value(lStep->get_comp(), lStep->get_value());
        } break;

        case ProcessStep::Type::Control_Output_Wrapper:
        {
          const ProcessStepControlOutputWrapper *lStep = (ProcessStepControlOutputWrapper *)lCurrentStep;
          lStep->get_wrapper()->set_state(lStep->get_state());
        } break;

        case ProcessStep::Type::Control_Ball_Valve_Wrapper:
        {
          const ProcessStepControlBallValveWrapper *lStep = (ProcessStepControlBallValveWrapper *)lCurrentStep;
          lStep->get_wrapper()->set_state(lStep->get_state());
        } break;
        
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

    // State Specific processing
    if(true == is_process_state(aState->get_id()))
    {
      if(0 != aState->get_total_process_time_in_sec())
      {
        int lPercentComplete = ((float)mCurrentStateTimer.delta_in_secs()/aState->get_total_process_time_in_sec())*100;

        if(lPercentComplete != mPrevStatePercentComplete)
        {
          mNextionWrapper.set_value("j0", lPercentComplete);
          mPrevStatePercentComplete = lPercentComplete;
        }
      }

      if(0 != mTotalProcessingTime)
      {
        int lPercentComplete = ((float)mProcessTimer.delta_in_secs()/mTotalProcessingTime)*100;

        if(lPercentComplete != mPrevProcessPercentComplete)
        {
          mNextionWrapper.set_value("j1", lPercentComplete);
          mPrevProcessPercentComplete = lPercentComplete;
        }
      }

      // TODO: Display current state of outputs
      update_output_display(mInWaterWrapper,   mPrevInWaterState,     "t1");
      update_output_display(mInCleanerWrapper, mPrevInCleanerState,   "t2");
      update_output_display(mInSaniWrapper,    mPrevInSanitizerState, "t3");

      update_output_display(mReWasteWrapper,   mPrevReWasteState,     "t4");
      update_output_display(mReCleanerWrapper, mPrevReCleanerState,   "t5");
      update_output_display(mReSaniWrapper,    mPrevReSanitizerState, "t6");

      update_output_display(mPumpWrapper,      mPrevPumpState,        "t7");
      update_output_display(mCo2Wrapper,       mPrevCo2State,         "t8");
    }
  }

  // Check to see if our processing time has completed 
  // Note if 0 value timer is disabled
  if(true == lProcessListComplete)
  {
    return true;
  }

  return false;
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
     aStateId == mProcessPressState.get_id() ||
     aStateId == mCompleteState.get_id())
  {
    return true;
  }

  return false;
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::nextion_touch_event(byte aPageId, byte aCompId, byte aEventType)
{
  TPRINT(F("nextion_touch_event: Page "));
  TPRINT(aPageId);
  TPRINT(" Comp ");
  TPRINT(aCompId);
  TPRINT(" Event ");
  TPRINTLN(aEventType);

  if(mCurrentState->get_id() == mMenuState.get_id())
  {
    // Clean Button
    if(aPageId == 1 && aCompId == 1 && aEventType == 0)
    {
      mCommandState = &mConfirmState;
      mInProcessWaitForInput = false;
    }
    // TODO: SETTINGS BUTTON
  }
  else if(mCurrentState->get_id() == mConfirmState.get_id())
  {
    // YES Button
    if(aPageId == 2 && aCompId == 3 && aEventType == 0)
    {
      // Compute total processing time
      mProcessStateIter = mProcessStates.begin(); 
      mTotalProcessingTime = 0;
      
      while(mProcessStateIter != mProcessStates.end())
      {
        KleanerState *lCurrentState = *mProcessStateIter;
        mTotalProcessingTime += lCurrentState->get_total_process_time_in_sec();
        mProcessStateIter++;
      }

      // Setup list for processing
      mProcessStateIter = mProcessStates.begin(); 
      mInProcessWaitForInput = false;
      mProcessTimer.reset();
    }
    // No Button
    else if(aPageId == 2 && aCompId == 4 && aEventType == 0)
    {      
      mProcessStateIter = mProcessStates.end();
      mInProcessWaitForInput = false;  
    }
  }
  else if(mCurrentState->get_id() == mCompleteState.get_id())
  {
    // Start over button
    if(aPageId == 4 && aCompId == 2 && aEventType == 0)
    {
      mInProcessWaitForInput = false;  
    }    
  }  
  else if(is_process_state(mCurrentState->get_id()))
  {
    // Stop Button
    if(aPageId == 3 && aCompId == 3 && aEventType == 0)
    {
      mCommandState = &mCompleteState;
    }
  }

}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::nextion_page_event(byte aPageId)
{
  TPRINT(F("nextion_page_event: Page "));
  TPRINTLN(aPageId);
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::update_output_display(const OutputWrapper &aOutputWrapper, int &aPrevState, char *aCompId)
{
  int lCurrentState = aOutputWrapper.get_state();

  if(lCurrentState != aPrevState)
  {
    if(lCurrentState == HIGH)
      mNextionWrapper.set_background_color(aCompId, 40487);
    else
      mNextionWrapper.set_background_color(aCompId, 10860);

    aPrevState = lCurrentState;
  }
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::update_output_display(const BallValveWrapper &aOutputWrapper, BallValveWrapper::State &aPrevState, char *aCompId)
{
  BallValveWrapper::State lCurrentState = (BallValveWrapper::State)aOutputWrapper.get_state();

  if(lCurrentState != aPrevState)
  {
    if(lCurrentState == BallValveWrapper::State::Open)
      mNextionWrapper.set_background_color(aCompId, 40487);
    else if(lCurrentState == BallValveWrapper::State::Close)
      mNextionWrapper.set_background_color(aCompId, 10860);
    else
      mNextionWrapper.set_background_color(aCompId, 63488);

    aPrevState = lCurrentState;
  }
}