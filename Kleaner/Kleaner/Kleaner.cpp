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
    mReWasteWrapper             (DO_PIN_RECIRC_WASTE_1,         DO_PIN_RECIRC_WASTE_2),
    mReSaniWrapper              (DO_PIN_RECIRC_SANITIZER_1,     DO_PIN_RECIRC_SANITIZER_2),
    mReCleanerWrapper           (DO_PIN_RECIRC_CLEANER_1,       DO_PIN_RECIRC_CLEANER_2),

    // States
    mSplashState                ("Splash",       false),
    mMenuState                  ("Menu",         false),
    mTestOutputState            ("Test Output",  false),
    mTestPhaseState             ("Test Phase",   false),
    mConfirmState               ("Confirm",      false),
    mCompleteState              ("Complete",     false),
    
    mProcessInitState           ("Init",         true),

    mProcessPurgeStateSixtel    ("Purge",        true),
    mProcessRinseStateSixtel    ("Rinse",        true),
    mProcessSaniStateSixtel     ("Sani",         true),
    mProcessWashStateSixtel     ("Wash",         true),
    mProcessPressStateSixtel    ("Pressure",     true),

    mProcessPurgeStateHalf      ("Purge 1/2",    true),
    mProcessRinseStateHalf      ("Rinse 1/2",    true),
    mProcessSaniStateHalf       ("Sani 1/2",     true),
    mProcessWashStateHalf       ("Wash 1/2",     true),
    mProcessPressStateHalf      ("Pressure 1/2", true),

    mProcessShutdownState       ("Shutdown",     true),

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
    mPrevReWasteState           (BallValveWrapper::State::Unknown),
    mPrevReCleanerState         (BallValveWrapper::State::Unknown),
    mPrevReSanitizerState       (BallValveWrapper::State::Unknown),  
    mPrevStatePercentComplete   (-1),     
    mPrevProcessPercentComplete (-1),
    mKegType                    (KegType::KegType_Sixtel)      
{
}

// ****************************************************************************
// See header file for details
// ****************************************************************************    
void Kleaner::setup()
{
  TSPRINTLN(F("Kleaner::setup - enter"));

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
  mSplashState.add_process_step(new ProcessStepResetOutputs(BALL_VALVE_DELAY));

  // Menu State
  // --------------------------------------------------------------------------
  mMenuState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_MAIN));
  mMenuState.add_process_step(new ProcessStepWaitForInput());

  // Test Output State
  mTestOutputState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_TEST_OUTPUT));
  mTestOutputState.add_process_step(new ProcessStepWaitForInput());
  mTestOutputState.add_process_step(new ProcessStepResetOutputs(BALL_VALVE_DELAY));

  // Test Phase State
  mTestPhaseState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_TEST_PHASE));
  mTestPhaseState.add_process_step(new ProcessStepWaitForInput());
  mTestPhaseState.add_process_step(new ProcessStepResetOutputs(BALL_VALVE_DELAY));

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
  mProcessInitState.add_process_step(new ProcessStepResetOutputs(BALL_VALVE_DELAY));
  TSPRINT(mProcessInitState.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessInitState.get_total_process_time_in_sec());

  // Purge State
  // --------------------------------------------------------------------------
  mProcessPurgeStateSixtel.add_process_step(new ProcessStepOutputWaste(BALL_VALVE_DELAY));
  mProcessPurgeStateSixtel.add_process_step(new ProcessStepCo2On(10));
  mProcessPurgeStateSixtel.add_process_step(new ProcessStepCo2Off(5));
  mProcessPurgeStateSixtel.add_process_step(new ProcessStepOutputOff(BALL_VALVE_DELAY));
  TSPRINT(mProcessPurgeStateSixtel.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessPurgeStateSixtel.get_total_process_time_in_sec());

  // Purge State (Half)
  // --------------------------------------------------------------------------
  mProcessPurgeStateHalf.add_process_step(new ProcessStepOutputWaste(BALL_VALVE_DELAY));
  mProcessPurgeStateHalf.add_process_step(new ProcessStepCo2On(15));
  mProcessPurgeStateHalf.add_process_step(new ProcessStepCo2Off(5));
  mProcessPurgeStateHalf.add_process_step(new ProcessStepOutputOff(BALL_VALVE_DELAY));
  TSPRINT(mProcessPurgeStateHalf.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessPurgeStateHalf.get_total_process_time_in_sec());

  // Rinse State
  // --------------------------------------------------------------------------
  mProcessRinseStateSixtel.add_process_step(new ProcessStepOutputWaste(BALL_VALVE_DELAY));
  for(int lnRinseIndex = 0; lnRinseIndex < 3; lnRinseIndex++)
  {
    mProcessRinseStateSixtel.add_process_step(new ProcessStepInputWater(BALL_VALVE_DELAY));  
    mProcessRinseStateSixtel.add_process_step(new ProcessStepPumpOn(15));
    mProcessRinseStateSixtel.add_process_step(new ProcessStepPumpOff());
    mProcessRinseStateSixtel.add_process_step(new ProcessStepInputOff(BALL_VALVE_DELAY));

    for (int lnCo2Index = 0; lnCo2Index < 1; lnCo2Index++)
    {
      mProcessRinseStateSixtel.add_process_step(new ProcessStepCo2On(10));
      mProcessRinseStateSixtel.add_process_step(new ProcessStepCo2Off(0));
    }
    mProcessRinseStateSixtel.add_process_step(new ProcessStepDelay(10));
  }
  mProcessRinseStateSixtel.add_process_step(new ProcessStepOutputOff(BALL_VALVE_DELAY));
  TSPRINT(mProcessRinseStateSixtel.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessRinseStateSixtel.get_total_process_time_in_sec());

  // Rinse State (Half)
  // --------------------------------------------------------------------------
  mProcessRinseStateHalf.add_process_step(new ProcessStepOutputWaste(BALL_VALVE_DELAY));
  for(int lnRinseIndex = 0; lnRinseIndex < 3; lnRinseIndex++)
  {
    mProcessRinseStateHalf.add_process_step(new ProcessStepInputWater(BALL_VALVE_DELAY));  
    mProcessRinseStateHalf.add_process_step(new ProcessStepPumpOn(15));
    mProcessRinseStateHalf.add_process_step(new ProcessStepPumpOff());
    mProcessRinseStateHalf.add_process_step(new ProcessStepInputOff(BALL_VALVE_DELAY));

    for (int lnCo2Index = 0; lnCo2Index < 1; lnCo2Index++)
    {
      mProcessRinseStateHalf.add_process_step(new ProcessStepCo2On(10));
      mProcessRinseStateHalf.add_process_step(new ProcessStepCo2Off(0));
    }
    mProcessRinseStateHalf.add_process_step(new ProcessStepDelay(35));
  }
  mProcessRinseStateHalf.add_process_step(new ProcessStepOutputOff(BALL_VALVE_DELAY));
  TSPRINT(mProcessRinseStateHalf.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessRinseStateHalf.get_total_process_time_in_sec());

  // Sani State
  // --------------------------------------------------------------------------
  mProcessSaniStateSixtel.add_process_step(new ProcessStepOutputSanitizer(BALL_VALVE_DELAY));
  mProcessSaniStateSixtel.add_process_step(new ProcessStepInputSanitizer(BALL_VALVE_DELAY));

  mProcessSaniStateSixtel.add_process_step(new ProcessStepPumpOn(10));
  mProcessSaniStateSixtel.add_process_step(new ProcessStepPumpOff());
  mProcessSaniStateSixtel.add_process_step(new ProcessStepInputOff(BALL_VALVE_DELAY));

  for (int lnCo2Index = 0; lnCo2Index < 1; lnCo2Index++)
  {
    mProcessSaniStateSixtel.add_process_step(new ProcessStepCo2On(7));
    mProcessSaniStateSixtel.add_process_step(new ProcessStepCo2Off(0));
  }
  mProcessSaniStateSixtel.add_process_step(new ProcessStepDelay(10));

  mProcessSaniStateSixtel.add_process_step(new ProcessStepOutputOff(BALL_VALVE_DELAY));
  TSPRINT(mProcessSaniStateSixtel.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessSaniStateSixtel.get_total_process_time_in_sec());

  // Sani State (Half)
  // --------------------------------------------------------------------------
  mProcessSaniStateHalf.add_process_step(new ProcessStepOutputSanitizer(BALL_VALVE_DELAY));
  mProcessSaniStateHalf.add_process_step(new ProcessStepInputSanitizer(BALL_VALVE_DELAY));

  mProcessSaniStateHalf.add_process_step(new ProcessStepPumpOn(10));
  mProcessSaniStateHalf.add_process_step(new ProcessStepPumpOff());
  mProcessSaniStateHalf.add_process_step(new ProcessStepInputOff(BALL_VALVE_DELAY));

  for (int lnCo2Index = 0; lnCo2Index < 1; lnCo2Index++)
  {
    mProcessSaniStateHalf.add_process_step(new ProcessStepCo2On(10));
    mProcessSaniStateHalf.add_process_step(new ProcessStepCo2Off(0));
  }
  mProcessSaniStateHalf.add_process_step(new ProcessStepDelay(50));

  mProcessSaniStateHalf.add_process_step(new ProcessStepOutputOff(BALL_VALVE_DELAY));
  TSPRINT(mProcessSaniStateHalf.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessSaniStateHalf.get_total_process_time_in_sec());

  // Wash State
  // --------------------------------------------------------------------------
  mProcessWashStateSixtel.add_process_step(new ProcessStepOutputCleaner(BALL_VALVE_DELAY));
  for(int lWashIndex = 0; lWashIndex < 2; lWashIndex++)
  {
    mProcessWashStateSixtel.add_process_step(new ProcessStepInputCleaner(BALL_VALVE_DELAY));
    mProcessWashStateSixtel.add_process_step(new ProcessStepPumpOn(30));
    mProcessWashStateSixtel.add_process_step(new ProcessStepPumpOff());
    mProcessWashStateSixtel.add_process_step(new ProcessStepInputOff(BALL_VALVE_DELAY));

    for (int lnCo2Index = 0; lnCo2Index < 1; lnCo2Index++)
    {
      mProcessWashStateSixtel.add_process_step(new ProcessStepCo2On(10));
      mProcessWashStateSixtel.add_process_step(new ProcessStepCo2Off(0));
    }
    mProcessWashStateSixtel.add_process_step(new ProcessStepDelay(30));
  }
  mProcessWashStateSixtel.add_process_step(new ProcessStepOutputOff(BALL_VALVE_DELAY));
  TSPRINT(mProcessWashStateSixtel.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessWashStateSixtel.get_total_process_time_in_sec());

  // Wash State (Half)
  // --------------------------------------------------------------------------
  mProcessWashStateHalf.add_process_step(new ProcessStepOutputCleaner(BALL_VALVE_DELAY));
  for(int lWashIndex = 0; lWashIndex < 2; lWashIndex++)
  {
    mProcessWashStateHalf.add_process_step(new ProcessStepInputCleaner(BALL_VALVE_DELAY));
    mProcessWashStateHalf.add_process_step(new ProcessStepPumpOn(30));
    mProcessWashStateHalf.add_process_step(new ProcessStepPumpOff());
    mProcessWashStateHalf.add_process_step(new ProcessStepInputOff(BALL_VALVE_DELAY));

    for (int lnCo2Index = 0; lnCo2Index < 1; lnCo2Index++)
    {
      mProcessWashStateHalf.add_process_step(new ProcessStepCo2On(10));
      mProcessWashStateHalf.add_process_step(new ProcessStepCo2Off(0));
    }
    mProcessWashStateHalf.add_process_step(new ProcessStepDelay(60));
  }
  mProcessWashStateHalf.add_process_step(new ProcessStepOutputOff(BALL_VALVE_DELAY));
  TSPRINT(mProcessWashStateHalf.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessWashStateHalf.get_total_process_time_in_sec());


  // Pressurize State
  // --------------------------------------------------------------------------
  mProcessPressStateSixtel.add_process_step(new ProcessStepCo2On(10));
  mProcessPressStateSixtel.add_process_step(new ProcessStepCo2Off());
  TSPRINT(mProcessPressStateSixtel.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessPressStateSixtel.get_total_process_time_in_sec());

  // Pressurize State (Half)
  // --------------------------------------------------------------------------
  mProcessPressStateHalf.add_process_step(new ProcessStepCo2On(20));
  mProcessPressStateHalf.add_process_step(new ProcessStepCo2Off());
  TSPRINT(mProcessPressStateHalf.get_state_name());
  TPRINT(" Duration - ");
  TPRINTLN(mProcessPressStateHalf.get_total_process_time_in_sec());

  // Shutdown
  // --------------------------------------------------------------------------
  mProcessShutdownState.add_process_step(new ProcessStepResetOutputs(BALL_VALVE_DELAY));

  // Complete State
  // --------------------------------------------------------------------------
  mCompleteState.add_process_step(new ProcessStepDisplayPage(PAGE_ID_COMPLETE));
  mCompleteState.add_process_step(new ProcessStepWaitForInput());


  // Setup the process state list
  // --------------------------------------------------------------------------
  mProcessStatesSixtel.push_back(&mProcessInitState);
  mProcessStatesSixtel.push_back(&mProcessPurgeStateSixtel);
  mProcessStatesSixtel.push_back(&mProcessRinseStateSixtel);
  mProcessStatesSixtel.push_back(&mProcessWashStateSixtel);
  mProcessStatesSixtel.push_back(&mProcessRinseStateSixtel);
  mProcessStatesSixtel.push_back(&mProcessSaniStateSixtel);
  mProcessStatesSixtel.push_back(&mProcessPressStateSixtel);
  mProcessStatesSixtel.push_back(&mProcessShutdownState);
  mProcessStatesSixtel.push_back(&mCompleteState);

  // Setup the process state list (Half)
  // --------------------------------------------------------------------------
  mProcessStatesHalf.push_back(&mProcessInitState);
  mProcessStatesHalf.push_back(&mProcessPurgeStateHalf);
  mProcessStatesHalf.push_back(&mProcessRinseStateHalf);
  mProcessStatesHalf.push_back(&mProcessWashStateHalf);
  mProcessStatesHalf.push_back(&mProcessRinseStateHalf);
  mProcessStatesHalf.push_back(&mProcessSaniStateHalf);
  mProcessStatesHalf.push_back(&mProcessPressStateHalf);
  mProcessStatesHalf.push_back(&mProcessShutdownState);
  mProcessStatesHalf.push_back(&mCompleteState);

  // Default it to the end until we start the process from the start menu
  mProcessStateIter = NULL;  

#ifdef AUTO_START
  if(mKegType == KegType::KegType_Half)
    mProcessStateIter = mProcessStatesHalf.begin(); 
  else
    mProcessStateIter = mProcessStatesSixtel.begin(); 
#endif


  TSPRINTLN(F("Kleaner::setup - exit"));
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
    TSPRINT(F("Commanding State: "));
    TPRINTLN(mCommandState->get_state_name());

    mCurrentState = mCommandState;
    mCommandState = NULL;
    mFirstTimeInState = true;
    mInProcessWaitForInput = false;
    mInProcessDelay = false;
    mProcessStateIter = NULL;      
  }
  // Check to see if current state is completed.
  else if(true == mStateComplete)
  {
    TSPRINT(F("Complete State: "));
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
    else if(mProcessStateIter != NULL)
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
bool Kleaner::process_state(KleanerState *aState, bool aInitState)
{
  // TODO: ERROR CHECK FOR NULL STATE

  bool lProcessListComplete = false;

  // Check to see if this is our first time in this state
  if(aInitState)
  {
    TSPRINT(F("Init State: "));
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

    if(aState->is_process_state())
    {
      TSPRINTLN(F("IS PROCESS STATE"));
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
        TPRINTLN(F(" - Complete"));

        mInProcessDelay = false;
        mProcessDelayInSec = 0;
      }
    }
    else if(true == mInProcessWaitForInput)
    {
      // Nothing to do here. Once enter button is pressed, this 
      // flag will be reset and processing will continue
    }
    else if(aState->process_list_iter() != NULL)
    {
      ProcessStep *lCurrentStep = *aState->process_list_iter();

      switch(lCurrentStep->get_type())
      {
        case ProcessStep::Type::Reset_Outputs:
          TSPRINTLN(F("All OFF"));
          set_all_off();
        break;

        case ProcessStep::Type::Wait_For_Input:
          TSPRINTLN(F("Wait for input "));
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
            int lSec = lDelta % 60;
            int lMin = ((lDelta - lSec) / 60) % 60;

            char *lBuffer = (char *)malloc(12);
            sprintf(lBuffer, "%02d.%02d", lMin, lSec);
            mNextionWrapper.set_text(COMPLETE_COMP_ID_DURATION, lBuffer);
            free(lBuffer);
          }
          else if(PAGE_ID_CONFIRM == lStep->get_page_id())
          {
            if(mKegType == KegType::KegType_Half)
            {
              mNextionWrapper.click_button(CONFIRM_COMP_ID_HALF, NextionWrapper::TOUCH_EVENT_PRESS);
              mNextionWrapper.click_button(CONFIRM_COMP_ID_HALF, NextionWrapper::TOUCH_EVENT_RELEASE);
            }
            else if(mKegType == KegType::KegType_Sixtel)
            {
              mNextionWrapper.click_button(CONFIRM_COMP_ID_SIXTLE, NextionWrapper::TOUCH_EVENT_PRESS);
              mNextionWrapper.click_button(CONFIRM_COMP_ID_SIXTLE, NextionWrapper::TOUCH_EVENT_RELEASE);
            }
            else 
            {
              mKegType = KegType::KegType_Sixtel;
              mNextionWrapper.click_button(CONFIRM_COMP_ID_SIXTLE, NextionWrapper::TOUCH_EVENT_PRESS);
              mNextionWrapper.click_button(CONFIRM_COMP_ID_SIXTLE, NextionWrapper::TOUCH_EVENT_RELEASE);
            }
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
          TSPRINTLN(F("IN - W:C S:C C:C"));
          mInWaterWrapper.set_state(BallValveWrapper::State::Close);
          mInSaniWrapper.set_state(BallValveWrapper::State::Close);
          mInCleanerWrapper.set_state(BallValveWrapper::State::Close);
        } break;

        case ProcessStep::Type::Input_Water:
        {
          TSPRINTLN(F("IN - W:O S:C C:C"));
          mInWaterWrapper.set_state(BallValveWrapper::State::Open);
          mInSaniWrapper.set_state(BallValveWrapper::State::Close);
          mInCleanerWrapper.set_state(BallValveWrapper::State::Close);
        } break;

        case ProcessStep::Type::Input_Cleaner:
        {
          TSPRINTLN(F("IN - W:C S:C C:O"));
          mInWaterWrapper.set_state(BallValveWrapper::State::Close);
          mInSaniWrapper.set_state(BallValveWrapper::State::Close);
          mInCleanerWrapper.set_state(BallValveWrapper::State::Open);
        } break;

        case ProcessStep::Type::Input_Sanitizer:
        {
          TSPRINTLN(F("IN - W:C S:O C:C"));
          mInWaterWrapper.set_state(BallValveWrapper::State::Close);
          mInSaniWrapper.set_state(BallValveWrapper::State::Open);
          mInCleanerWrapper.set_state(BallValveWrapper::State::Close);
        } break;

        case ProcessStep::Type::Output_Off:
        {
          TSPRINTLN(F("RE - W:C S:C C:C"));
          mReWasteWrapper.set_state(BallValveWrapper::State::Close);
          mReSaniWrapper.set_state(BallValveWrapper::State::Close);
          mReCleanerWrapper.set_state(BallValveWrapper::State::Close); 
        } break;

        case ProcessStep::Type::Output_Waste:
        {
          TSPRINTLN(F("RE - W:O S:C C:C"));
          mReWasteWrapper.set_state(BallValveWrapper::State::Open);
          mReSaniWrapper.set_state(BallValveWrapper::State::Close);
          mReCleanerWrapper.set_state(BallValveWrapper::State::Close); 
        } break;

        case ProcessStep::Type::Output_Cleaner:
        {
          TSPRINTLN(F("RE - W:C S:C C:O"));
          mReWasteWrapper.set_state(BallValveWrapper::State::Close);
          mReSaniWrapper.set_state(BallValveWrapper::State::Close);
          mReCleanerWrapper.set_state(BallValveWrapper::State::Open); 
        } break;

        case ProcessStep::Type::Output_Sanitizer:
        {
          TSPRINTLN(F("RE - W:C S:O C:C"));
          mReWasteWrapper.set_state(BallValveWrapper::State::Close);
          mReSaniWrapper.set_state(BallValveWrapper::State::Open);
          mReCleanerWrapper.set_state(BallValveWrapper::State::Close); 
        } break;     

        case ProcessStep::Type::Co2_Off:  
        { 
          TSPRINTLN(F("CO2 OFF"));
          mCo2Wrapper.set_state(LOW);   
        } break;     
        
        case ProcessStep::Type::Co2_On:   
        { 
          TSPRINTLN(F("CO2 ON"));
          mCo2Wrapper.set_state(HIGH);  
        } break;    
        
        case ProcessStep::Type::Pump_Off: 
        {
          TSPRINTLN(F("PUMP OFF"));
          mPumpWrapper.set_state(LOW);  
        } break;     
        
        case ProcessStep::Type::Pump_On:  
        {
          TSPRINTLN(F("PUMP ON"));
          mPumpWrapper.set_state(HIGH); 
        } break;                      
        
        default:
          // TODO: Should not get here but lets flag an error
          TSPRINTLN(F("!!!! UNKNOWN Process step !!!!"));
        break;
      }

      // If this step is delayed, set the delay timer
      if(0 != lCurrentStep->get_delay())
      {
        TSPRINT(F("Delay "));
        TPRINT(lCurrentStep->get_delay());
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
    if(aState->is_process_state())
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
  TSPRINTLN(F("IN - W:C S:C C:C"));
  TSPRINTLN(F("RE - W:C S:C C:C"));
  TSPRINTLN(F("PUMP OFF"));
  TSPRINTLN(F("CO2 OFF"));

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
void Kleaner::nextion_touch_event(byte aPageId, byte aCompId, byte aEventType)
{
  TSPRINT(F("nextion_touch_event: Page "));
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
        if(mKegType == KegType::KegType_Half)
          mProcessStateIter = mProcessStatesHalf.begin(); 
        else
          mProcessStateIter = mProcessStatesSixtel.begin(); 
        
        mTotalProcessingTime = 0;
        
        while(mProcessStateIter != NULL)
        {
          KleanerState *lCurrentState = *mProcessStateIter;
          mTotalProcessingTime += lCurrentState->get_total_process_time_in_sec();
          mProcessStateIter++;
        }

        // Setup list for processing
        // Compute total processing time
        if(mKegType == KegType::KegType_Half)
          mProcessStateIter = mProcessStatesHalf.begin(); 
        else
          mProcessStateIter = mProcessStatesSixtel.begin(); 

        mInProcessWaitForInput = false;
        mProcessTimer.reset();
      }
      // No Button
      else if(aCompId == CONFIRM_BUTTON_ID_NO)
      {      
        mProcessStateIter = NULL;
        mInProcessWaitForInput = false;  
      }
      else if(aCompId == CONFIRM_RADIO_ID_SIXTEL)
      {
        TSPRINTLN(F("Sixtel Keg set"));
        mKegType = KegType::KegType_Sixtel;
      }
      else if(aCompId == CONFIRM_RADIO_ID_HALF)
      {
        TSPRINTLN(F("Half Keg set"));
        mKegType = KegType::KegType_Half;
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
          if(mReWasteWrapper.get_state() == BallValveWrapper::State::Close)
            mReWasteWrapper.set_open();
          else
            mReWasteWrapper.set_close();         
        } break;

        case TEST_OUTPUT_BUTTON_ID_RE_CLEANER:
        {
          if(mReCleanerWrapper.get_state() == BallValveWrapper::State::Close)
            mReCleanerWrapper.set_open();
          else
            mReCleanerWrapper.set_close();         
        } break;

        case TEST_OUTPUT_BUTTON_ID_RE_SANITIZER:
        {
          if(mReSaniWrapper.get_state() == BallValveWrapper::State::Close)
            mReSaniWrapper.set_open();
          else
            mReSaniWrapper.set_close();      
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
          mCommandState = &mProcessPurgeStateSixtel;
          mReturnToState = &mTestPhaseState;
          mNextionWrapper.set_page(PAGE_ID_PROCESS);
        } break;

        case TEST_PHASE_BUTTON_ID_WASH:
        {
          mCommandState = &mProcessWashStateSixtel;
          mReturnToState = &mTestPhaseState;
          mNextionWrapper.set_page(PAGE_ID_PROCESS);
        } break;

        case TEST_PHASE_BUTTON_ID_RINSE:
        {
          mCommandState = &mProcessRinseStateSixtel;
          mReturnToState = &mTestPhaseState;
          mNextionWrapper.set_page(PAGE_ID_PROCESS);
        } break;

        case TEST_PHASE_BUTTON_ID_SANI:
        {
          mCommandState = &mProcessSaniStateSixtel;
          mReturnToState = &mTestPhaseState;
          mNextionWrapper.set_page(PAGE_ID_PROCESS);
        } break;

        case TEST_PHASE_BUTTON_ID_PRESSURE:
        {
          mCommandState = &mProcessPressStateSixtel;
          mReturnToState = &mTestPhaseState;
          mNextionWrapper.set_page(PAGE_ID_PROCESS);
        } break;
      }
    }
  }  
  else if(mCurrentState->is_process_state())
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
  TSPRINT(F("nextion_page_event: Page "));
  TPRINTLN(aPageId);
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void Kleaner::update_output_display(OutputWrapper &aOutputWrapper, int &aPrevState, const char *aCompId)
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
void Kleaner::update_output_display(BallValveWrapper &aOutputWrapper, BallValveWrapper::State &aPrevState, const char *aCompId)
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