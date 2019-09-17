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
    mCo2Wrapper                 (DO_PIN_RELAY_CO2,              LOW),
    mPumpWrapper                (DO_PIN_PUMP,                   LOW),
    mInWaterWrapper             (DO_PIN_MOTOR_IN_WATER_1,       DO_PIN_MOTOR_IN_WATER_2),
    mInCleanerWrapper           (DO_PIN_MOTOR_IN_CLEANER_1,     DO_PIN_MOTOR_IN_CLEANER_2),
    mInSaniWrapper              (DO_PIN_MOTOR_IN_SANITIZER_1,   DO_PIN_MOTOR_IN_SANITIZER_2),
    mReWasteWrapper             (DO_PIN_RECIRC_WASTE,           LOW),
    mReSaniWrapper              (DO_PIN_RECIRC_SANITIZER,       LOW),
    mReCleanerWrapper           (DO_PIN_RECIRC_CLEANER,         LOW),

    // States
    mSplashState                ("Splash"),
    mMenuState                  ("Menu"),
    mTestOutputState            ("Test Output"),
    mTestPhaseState             ("Test Phase"),
    mConfirmState               ("Confirm"),
    mCompleteState              ("Complete"),
    
    mProcessInitState           ("Init"),
    mProcessPurgeState          ("Purge"),
    mProcessRinseState          ("Rinse"),
    mProcessSaniState           ("Sani"),
    mProcessWashState           ("Wash"),
    mProcessPressState          ("Pressure"),
    mProcessShutdownState       ("Shutdown"),

    // State Pointers - We are starting with the splash screen
    // once the splash is done, the menu state is initiated
    mCurrentState               (&mSplashState),
    mReturnToState              (NULL),    
    mCommandState               (NULL),
    mStateComplete              (false),
    mFirstTimeInState           (true),
    mInProcessDelay             (false),
    mInProcessWaitForInput      (false),
    mProcessDelayInSec          (0),
    mNextionWrapper             (PAGE_ID_HOME),

    // Reset component state flags for displaying status
    mPrevPumpState              (-1),
    mPrevCo2State               (-1),
    mPrevInWaterState           (BallValveWrapper::State::Unknown),
    mPrevInCleanerState         (BallValveWrapper::State::Unknown),
    mPrevInSanitizerState       (BallValveWrapper::State::Unknown),
    mPrevReWasteState           (-1),
    mPrevReCleanerState         (-1),
    mPrevReSanitizerState       (-1),  
    mPrevStatePercentComplete   (-1),     
    mPrevProcessPercentComplete (-1)      
{
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
  // --------------------------------------------------------------------------
  mSplashState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_HOME));
  mSplashState.add_process_step(new ProcessStepDisplayText(HOME_COMP_ID_VERSION,  KLEANER_VERSION));
  mSplashState.add_process_step(new ProcessStepResetOutputs(5));

  // Menu State
  // --------------------------------------------------------------------------
  mMenuState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_MAIN));
  mMenuState.add_process_step(new ProcessStepWaitForInput());

  // Test Output State
  mTestOutputState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_TEST_OUTPUT));
  mTestOutputState.add_process_step(new ProcessStepWaitForInput());
  mTestOutputState.add_process_step(new ProcessStepResetOutputs(5));

  // Test Phase State
  mTestPhaseState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_TEST_PHASE));
  mTestPhaseState.add_process_step(new ProcessStepWaitForInput());
  mTestPhaseState.add_process_step(new ProcessStepResetOutputs(5));

  // Confirm State
  // --------------------------------------------------------------------------
  mConfirmState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_CONFIRM));
  mConfirmState.add_process_step(new ProcessStepWaitForInput());

  // Init State
  // --------------------------------------------------------------------------
  mProcessInitState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_PROCESS));
  mProcessInitState.add_process_step(new ProcessStepDisplayValue(PROCESS_COMP_ID_PROGRESS_STATE, 0));
  mProcessInitState.add_process_step(new ProcessStepDisplayValue(PROCESS_COMP_ID_PROGRESS_PROCESS, 0));
  mProcessInitState.add_process_step(new ProcessStepDisplayText(PROCESS_COMP_ID_TITLE,  "Init"));
  mProcessInitState.add_process_step(new ProcessStepResetOutputs(5));

  // Purge State
  // --------------------------------------------------------------------------
  mProcessPurgeState.add_process_step(new ProcessStepOutputWaste(5));
  mProcessPurgeState.add_process_step(new ProcessStepCo2On(10));
  mProcessPurgeState.add_process_step(new ProcessStepCo2Off(10));

  mProcessPurgeState.add_process_step(new ProcessStepOutputOff());
  
  // Rinse State
  // --------------------------------------------------------------------------
  mProcessRinseState.add_process_step(new ProcessStepOutputWaste());

  mProcessRinseState.add_process_step(new ProcessStepInputWater(5));  
  mProcessRinseState.add_process_step(new ProcessStepPumpOn(15));
  mProcessRinseState.add_process_step(new ProcessStepPumpOff());
  mProcessRinseState.add_process_step(new ProcessStepInputOff(5));
  mProcessRinseState.add_process_step(new ProcessStepCo2On(5));
  mProcessRinseState.add_process_step(new ProcessStepCo2Off(10));

  mProcessRinseState.add_process_step(new ProcessStepInputWater(5));
  mProcessRinseState.add_process_step(new ProcessStepPumpOn(15));
  mProcessRinseState.add_process_step(new ProcessStepPumpOff());
  mProcessRinseState.add_process_step(new ProcessStepInputOff(5));
  mProcessRinseState.add_process_step(new ProcessStepCo2On(5));
  mProcessRinseState.add_process_step(new ProcessStepCo2Off());

  mProcessRinseState.add_process_step(new ProcessStepOutputOff(10));

  // Sani State
  // --------------------------------------------------------------------------
  mProcessSaniState.add_process_step(new ProcessStepOutputSanitizer());
  mProcessSaniState.add_process_step(new ProcessStepInputSanitizer(5));

  mProcessSaniState.add_process_step(new ProcessStepPumpOn(10));
  mProcessSaniState.add_process_step(new ProcessStepPumpOff());
  mProcessSaniState.add_process_step(new ProcessStepInputOff(5));
  mProcessSaniState.add_process_step(new ProcessStepCo2On(5));
  mProcessSaniState.add_process_step(new ProcessStepCo2Off());

  mProcessSaniState.add_process_step(new ProcessStepOutputOff(10));

  // Wash State
  // --------------------------------------------------------------------------
  mProcessWashState.add_process_step(new ProcessStepOutputCleaner());
  
  mProcessWashState.add_process_step(new ProcessStepInputCleaner(5));
  mProcessWashState.add_process_step(new ProcessStepPumpOn(30));
  mProcessWashState.add_process_step(new ProcessStepPumpOff());
  mProcessWashState.add_process_step(new ProcessStepInputOff(5));
  mProcessWashState.add_process_step(new ProcessStepCo2On(5));
  mProcessWashState.add_process_step(new ProcessStepCo2Off(35));
  
  mProcessWashState.add_process_step(new ProcessStepInputCleaner(5));
  mProcessWashState.add_process_step(new ProcessStepPumpOn(30));
  mProcessWashState.add_process_step(new ProcessStepPumpOff());
  mProcessWashState.add_process_step(new ProcessStepInputOff(5));
  mProcessWashState.add_process_step(new ProcessStepCo2On(5));
  mProcessWashState.add_process_step(new ProcessStepCo2Off(35));
  
  mProcessWashState.add_process_step(new ProcessStepInputCleaner(5));
  mProcessWashState.add_process_step(new ProcessStepPumpOn(30));
  mProcessWashState.add_process_step(new ProcessStepPumpOff());
  mProcessWashState.add_process_step(new ProcessStepInputOff(5));
  mProcessWashState.add_process_step(new ProcessStepCo2On(5));
  mProcessWashState.add_process_step(new ProcessStepCo2Off(35));
  
  mProcessWashState.add_process_step(new ProcessStepInputCleaner(5));
  mProcessWashState.add_process_step(new ProcessStepPumpOn(30));
  mProcessWashState.add_process_step(new ProcessStepPumpOff());
  mProcessWashState.add_process_step(new ProcessStepInputOff(5));
  mProcessWashState.add_process_step(new ProcessStepCo2On(5));
  mProcessWashState.add_process_step(new ProcessStepCo2Off(35));

  mProcessWashState.add_process_step(new ProcessStepInputCleaner(5));
  mProcessWashState.add_process_step(new ProcessStepPumpOn(30));
  mProcessWashState.add_process_step(new ProcessStepPumpOff());
  mProcessWashState.add_process_step(new ProcessStepInputOff(5));
  mProcessWashState.add_process_step(new ProcessStepCo2On(5));
  mProcessWashState.add_process_step(new ProcessStepCo2Off(35));

  mProcessWashState.add_process_step(new ProcessStepOutputOff());
  
  // Pressurize State
  // --------------------------------------------------------------------------
  mProcessPressState.add_process_step(new ProcessStepCo2On(10));
  mProcessPressState.add_process_step(new ProcessStepCo2Off());

  // Shutdown
  // --------------------------------------------------------------------------
  mProcessShutdownState.add_process_step(new ProcessStepResetOutputs(5));

  // Complete State
  // --------------------------------------------------------------------------
  mCompleteState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_COMPLETE));
  mCompleteState.add_process_step(new ProcessStepWaitForInput());


  // Setup the process state list
  // --------------------------------------------------------------------------
  mProcessStates.push_back(&mProcessInitState);
  mProcessStates.push_back(&mProcessPurgeState);
  mProcessStates.push_back(&mProcessRinseState);
  mProcessStates.push_back(&mProcessWashState);
  mProcessStates.push_back(&mProcessRinseState);
  mProcessStates.push_back(&mProcessSaniState);
  mProcessStates.push_back(&mProcessPressState);
  mProcessStates.push_back(&mProcessShutdownState);
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
      // Reached the end of our prcoessing list (or test state completed), proceed back to 
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

    if(true == is_process_state(aState->get_id()))
    {
      mNextionWrapper.set_text(PROCESS_COMP_ID_TITLE, aState->get_state_name());
    }
  }
  else
  {
    // TODO: Generic processing

    if(true == mInProcessDelay)
    {
      // Check to see if our delay has
      if(mProcessDelayTimer.delta_in_secs() >= mProcessDelayInSec)
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
          // Nothing to do here - delay will be handled below
        } break;

        case ProcessStep::Type::Display_Page:
        {
          const ProcessStepDisplayPage *lStep = (ProcessStepDisplayPage *)lCurrentStep;
          mNextionWrapper.set_page(lStep->get_page_id());

          if(PAGE_ID_COMPLETE == lStep->get_page_id())
          {
            int lDelta = mProcessTimer.delta_in_secs();
            int lMin = lDelta / 60;
            int lSec = lDelta - lMin * 60;

            char *lBuffer = malloc(12);
            sprintf(lBuffer, "%02d.%02d", lMin, lSec);
            mNextionWrapper.set_text(COMPLETE_COMP_ID_DURATION, lBuffer);
            free(lBuffer);
          }

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

        case ProcessStep::Type::Input_Off:
        {
          mInWaterWrapper.set_state(BallValveWrapper::State::Close);
          mInSaniWrapper.set_state(BallValveWrapper::State::Close);
          mInCleanerWrapper.set_state(BallValveWrapper::State::Close);
        } break;

        case ProcessStep::Type::Input_Water:
        {
          mInWaterWrapper.set_state(BallValveWrapper::State::Open);
          mInSaniWrapper.set_state(BallValveWrapper::State::Close);
          mInCleanerWrapper.set_state(BallValveWrapper::State::Close);
        } break;

        case ProcessStep::Type::Input_Cleaner:
        {
          mInWaterWrapper.set_state(BallValveWrapper::State::Close);
          mInSaniWrapper.set_state(BallValveWrapper::State::Close);
          mInCleanerWrapper.set_state(BallValveWrapper::State::Open);
        } break;

        case ProcessStep::Type::Input_Sanitizer:
        {
          mInWaterWrapper.set_state(BallValveWrapper::State::Close);
          mInSaniWrapper.set_state(BallValveWrapper::State::Open);
          mInCleanerWrapper.set_state(BallValveWrapper::State::Close);
        } break;

        case ProcessStep::Type::Output_Off:
        {
          mReWasteWrapper.set_state(LOW);
          mReSaniWrapper.set_state(LOW);
          mReCleanerWrapper.set_state(LOW); 
        } break;

        case ProcessStep::Type::Output_Waste:
        {
          mReWasteWrapper.set_state(HIGH);
          mReSaniWrapper.set_state(LOW);
          mReCleanerWrapper.set_state(LOW); 
        } break;

        case ProcessStep::Type::Output_Cleaner:
        {
          mReWasteWrapper.set_state(LOW);
          mReSaniWrapper.set_state(LOW);
          mReCleanerWrapper.set_state(HIGH); 
        } break;

        case ProcessStep::Type::Output_Sanitizer:
        {
          mReWasteWrapper.set_state(LOW);
          mReSaniWrapper.set_state(HIGH);
          mReCleanerWrapper.set_state(LOW); 
        } break;     

        case ProcessStep::Type::Co2_Off:
        {
          mCo2Wrapper.set_state(LOW);
        } break;     

        case ProcessStep::Type::Co2_On:
        {
          mCo2Wrapper.set_state(HIGH);
        } break;    

        case ProcessStep::Type::Pump_Off:
        {
          mPumpWrapper.set_state(LOW);
        } break;     

        case ProcessStep::Type::Pump_On:
        {
          mPumpWrapper.set_state(HIGH);
        } break;                      
        
        default:
          // TODO: Should not get here but lets flag an error
        break;
      }

      // If this step is delayed, set the delay timer
      if(0 != lCurrentStep->get_delay())
      {
        TPRINT(F("Delay "));
        TPRINTLN(lCurrentStep->get_delay());
        mInProcessDelay = true;
        mProcessDelayInSec = lCurrentStep->get_delay();
        mProcessDelayTimer.reset();
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
          mNextionWrapper.set_value(PROCESS_COMP_ID_PROGRESS_STATE, lPercentComplete);
          mPrevStatePercentComplete = lPercentComplete;
        }
      }

      if(0 != mTotalProcessingTime)
      {
        int lPercentComplete = ((float)mProcessTimer.delta_in_secs()/mTotalProcessingTime)*100;

        if(lPercentComplete != mPrevProcessPercentComplete)
        {
          mNextionWrapper.set_value(PROCESS_COMP_ID_PROGRESS_PROCESS, lPercentComplete);
          mPrevProcessPercentComplete = lPercentComplete;
        }
      }

      // Display current state of outputs
      update_output_display(mInWaterWrapper,   mPrevInWaterState,     PROCESS_COMP_ID_IN_WATER);
      update_output_display(mInCleanerWrapper, mPrevInCleanerState,   PROCESS_COMP_ID_IN_CLEANER);
      update_output_display(mInSaniWrapper,    mPrevInSanitizerState, PROCESS_COMP_ID_IN_SANITIZER);

      update_output_display(mReWasteWrapper,   mPrevReWasteState,     PROCESS_COMP_ID_RE_WASTE);
      update_output_display(mReCleanerWrapper, mPrevReCleanerState,   PROCESS_COMP_ID_RE_CLEANER);
      update_output_display(mReSaniWrapper,    mPrevReSanitizerState, PROCESS_COMP_ID_RE_SANITIZER);

      update_output_display(mPumpWrapper,      mPrevPumpState,        PROCESS_COMP_ID_PUMP);
      update_output_display(mCo2Wrapper,       mPrevCo2State,         PROCESS_COMP_ID_C02);
    }
    else if(mCurrentState->get_id() == mTestOutputState.get_id())
    {
      // Display current state of outputs
      update_output_display(mInWaterWrapper,   mPrevInWaterState,     PROCESS_COMP_ID_IN_WATER);
      update_output_display(mInCleanerWrapper, mPrevInCleanerState,   PROCESS_COMP_ID_IN_CLEANER);
      update_output_display(mInSaniWrapper,    mPrevInSanitizerState, PROCESS_COMP_ID_IN_SANITIZER);

      update_output_display(mReWasteWrapper,   mPrevReWasteState,     PROCESS_COMP_ID_RE_WASTE);
      update_output_display(mReCleanerWrapper, mPrevReCleanerState,   PROCESS_COMP_ID_RE_CLEANER);
      update_output_display(mReSaniWrapper,    mPrevReSanitizerState, PROCESS_COMP_ID_RE_SANITIZER);

      update_output_display(mPumpWrapper,      mPrevPumpState,        PROCESS_COMP_ID_PUMP);
      update_output_display(mCo2Wrapper,       mPrevCo2State,         PROCESS_COMP_ID_C02);
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
     aStateId == mProcessShutdownState.get_id())
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

  // We only want to handle the release event type so we will
  // ignore the pressed event
  if(aEventType == NextionWrapper::TOUCH_EVENT_PRESS)
    return;

  if(mCurrentState->get_id() == mMenuState.get_id())
  {
    if(aPageId == PAGE_ID_MAIN)
    {
      // Clean Button
      if(aCompId == MAIN_BUTTON_ID_CLEAN)
      {
        mCommandState = &mConfirmState;
        mInProcessWaitForInput = false;
      }
      // Test Output Button
      else if(aCompId == MAIN_BUTTON_ID_TEST_OUTPUT)
      {
        mCommandState = &mTestOutputState;
        mInProcessWaitForInput = false;
      }
      // Test Phase Button
      else if(aCompId == MAIN_BUTTON_ID_TEST_PHASE)
      {
        mCommandState = &mTestPhaseState;
        mInProcessWaitForInput = false;
      }
    }
  }
  else if(mCurrentState->get_id() == mConfirmState.get_id())
  {
    if(aPageId == PAGE_ID_CONFIRM)
    {
      // YES Button
      if(aCompId == CONFIRM_BUTTON_ID_YES)
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
      else if(aCompId == CONFIRM_BUTTON_ID_NO)
      {      
        mProcessStateIter = mProcessStates.end();
        mInProcessWaitForInput = false;  
      }
    }
  }
  else if(mCurrentState->get_id() == mCompleteState.get_id())
  {
    if(aPageId == PAGE_ID_COMPLETE)
    {
      // Start over button
      if(aCompId == COMPLETE_BUTTON_ID_START_OVER)
      {
        mInProcessWaitForInput = false;  
      }
    }    
  }
  else if(mCurrentState->get_id() == mTestOutputState.get_id())
  {
    if(aPageId == PAGE_ID_TEST_OUTPUT)
    {
      switch(aCompId)
      {
        case TEST_OUTPUT_BUTTON_ID_BACK:
        {
          mInProcessWaitForInput = false;
        } break;

        case TEST_OUTPUT_BUTTON_ID_IN_WATER:
        {
          if(mInWaterWrapper.get_state() == BallValveWrapper::State::Close)
            mInWaterWrapper.set_open();
          else
            mInWaterWrapper.set_close();
        } break;

        case TEST_OUTPUT_BUTTON_ID_IN_CLEANER:
        {
          if(mInCleanerWrapper.get_state() == BallValveWrapper::State::Close)
            mInCleanerWrapper.set_open();
          else
            mInCleanerWrapper.set_close();
        } break;

        case TEST_OUTPUT_BUTTON_ID_IN_SANITIZER:
        {
          if(mInSaniWrapper.get_state() == BallValveWrapper::State::Close)
            mInSaniWrapper.set_open();
          else
            mInSaniWrapper.set_close();
        } break;

        case TEST_OUTPUT_BUTTON_ID_RE_WASTE:
        {
          if(mReWasteWrapper.get_state() == HIGH)
            mReWasteWrapper.set_state(LOW);
          else
            mReWasteWrapper.set_state(HIGH);            
        } break;

        case TEST_OUTPUT_BUTTON_ID_RE_CLEANER:
        {
          if(mReCleanerWrapper.get_state() == HIGH)
            mReCleanerWrapper.set_state(LOW);
          else
            mReCleanerWrapper.set_state(HIGH);          
        } break;

        case TEST_OUTPUT_BUTTON_ID_RE_SANITIZER:
        {
          if(mReSaniWrapper.get_state() == HIGH)
            mReSaniWrapper.set_state(LOW);
          else
            mReSaniWrapper.set_state(HIGH);
        } break;

        case TEST_OUTPUT_BUTTON_ID_PUMP:
        {
          if(mPumpWrapper.get_state() == HIGH)
            mPumpWrapper.set_state(LOW);
          else
            mPumpWrapper.set_state(HIGH);
        } break;

        case TEST_OUTPUT_BUTTON_ID_C02:
        {
          if(mCo2Wrapper.get_state() == HIGH)
            mCo2Wrapper.set_state(LOW);
          else
            mCo2Wrapper.set_state(HIGH);          
        } break;
      }
    }
  }
  else if(mCurrentState->get_id() == mTestPhaseState.get_id())
  {
    if(aPageId == PAGE_ID_TEST_PHASE)
    {
      switch(aCompId)
      {
        case TEST_PHASE_BUTTON_ID_BACK:
        {
          mInProcessWaitForInput = false;
        } break;

        case TEST_PHASE_BUTTON_ID_PURGE:
        {
          mCommandState = &mProcessPurgeState;
          mReturnToState = &mTestPhaseState;
          mNextionWrapper.set_page(PAGE_ID_PROCESS);
        } break;

        case TEST_PHASE_BUTTON_ID_WASH:
        {
          mCommandState = &mProcessWashState;
          mReturnToState = &mTestPhaseState;
          mNextionWrapper.set_page(PAGE_ID_PROCESS);
        } break;

        case TEST_PHASE_BUTTON_ID_RINSE:
        {
          mCommandState = &mProcessRinseState;
          mReturnToState = &mTestPhaseState;
          mNextionWrapper.set_page(PAGE_ID_PROCESS);
        } break;

        case TEST_PHASE_BUTTON_ID_SANI:
        {
          mCommandState = &mProcessSaniState;
          mReturnToState = &mTestPhaseState;
          mNextionWrapper.set_page(PAGE_ID_PROCESS);
        } break;

        case TEST_PHASE_BUTTON_ID_PRESSURE:
        {
          mCommandState = &mProcessPressState;
          mReturnToState = &mTestPhaseState;
          mNextionWrapper.set_page(PAGE_ID_PROCESS);
        } break;
      }
    }
  }  
  else if(is_process_state(mCurrentState->get_id()))
  {
    if(aPageId == PAGE_ID_PROCESS)
    {
      // Stop Button
      if(aCompId == PROCESS_BUTTON_ID_STOP)
      {
        mCommandState = &mProcessShutdownState;
        mReturnToState = &mCompleteState;
      }
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