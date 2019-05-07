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

    mInWaterWrapper  (DO_PIN_MOTOR_INPUT_WATER_1, 
                      DO_PIN_MOTOR_INPUT_WATER_2),


    mReWasteWrapper  (DO_PIN_RECIRC_WASTE,          LOW),
    mReSaniWrapper   (DO_PIN_RECIRC_SANITIZER,      LOW),
    mReCleanerWrapper(DO_PIN_RECIRC_CLEANER,        LOW),

    // State init
    // States             Name            State Duration
    mSplashState          (F("Splash")),
    mProcessInitState     (F("Init")),
    mProcessPurgeState    (F("Purge")),
    mProcessRinseState    (F("Rinse")),
    mProcessSaniState     (F("Sani")),
    mProcessWashState     (F("Wash")),
    mProcessPressState    (F("Pressure")),
    mCompleteState        (F("Complete")),
    mMenuState            (F("Menu")),
    mTestState            (F("Test")),
    mConfirmState         (F("Confirm")),

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

    mNextionWrapper(0)
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
  mSplashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display_Page,           0));
  mSplashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display_Text,           "t0",  KLEANER_VERSION));
  mSplashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mSplashState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,                  5));

  // Menu State
  mMenuState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display_Page,             1));
  mMenuState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));

  // Confirm State
  mConfirmState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display_Page,           2));
  mConfirmState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));

  // Init State
  mProcessInitState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mProcessInitState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display_Page,      3));

  // Purge State
  mProcessPurgeState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mProcessPurgeState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            10  ));
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
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Water,  BallValveWrapper::State::Open));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            15  ));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Pump,         HIGH));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            10  ));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Pump,         LOW ));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Water,  BallValveWrapper::State::Close));
  mProcessRinseState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Delay,            10  ));
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
  mCompleteState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
  mCompleteState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display_Page,      4));
  mCompleteState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));

  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::All_Off));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       0,  new String(F("Test - Co2"))));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,          HIGH));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Co2,          LOW ));

//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       0,  new String(F("Test - Pump"))));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Pump,   HIGH));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Pump,   LOW ));

//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       0,  new String(F("Test - In Cleaner"))));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Cleaner,HIGH));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Cleaner,LOW ));

//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       0,  new String(F("Test - In Sani"))));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Sani,   HIGH));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Sani,   LOW ));

//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       0,  new String(F("Test - In Water"))));
  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Water,  BallValveWrapper::State::Open));
  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));
  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Input_Water,  BallValveWrapper::State::Close));

//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       0,  new String(F("Test - Re Waste"))));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Recirc_Waste,   HIGH));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Recirc_Waste,   LOW ));

//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       0,  new String(F("Test - Re Cleaner"))));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Recirc_Cleaner,   HIGH));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Recirc_Cleaner,   LOW ));

//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       0,  new String(F("Test - Re Sani"))));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Recirc_Sani,   HIGH));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Set_Recirc_Sani,   LOW ));

//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       0,  new String(F("Complete"))));
//  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Display,       1,  new String(F("Enter To Restart"))));
  mTestState.add_process_step(new ProcessStep(ProcessStep::ProcessType::Wait_For_Input));


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

  // Check to see if current state is completed.
  if(true == mStateComplete)
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
  // A new state has been commanded, transition to it
  else if (mCommandState != NULL)
  {
    TPRINT(F("Commanding State: "));
    TPRINTLN(mCommandState->get_state_name());

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
    TPRINT(F("Init State: "));
    TPRINTLN(aState->get_state_name());

    // Reset the state timer
    mStateTimer.reset(); 

    // Initialize the state
    aState->init_state();
  }
  else
  {
    // TODO: Generic processing

    if(true == mInProcessDelay)
    {
//      TPRINT(aState->get_state_name());
//      TPRINTLN(F(" - In process delay"));

      // Check to see if our delay has elapsed
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
        case ProcessStep::ProcessType::All_Off:
          TPRINTLN(F("All OFF"));
          set_all_off();
        break;

        case ProcessStep::ProcessType::Set_Input_Water:
          TPRINT(F("Input Water: "));

          switch((BallValveWrapper::State)lCurrentStep->get_value())
          {
            case BallValveWrapper::State::Open:
              TPRINTLN(F("OPEN"));
              mInWaterWrapper.set_open();
            break;

            case BallValveWrapper::State::Close:
              TPRINTLN(F("CLOSE"));
              mInWaterWrapper.set_close();
            break;

            case BallValveWrapper::State::Idle:
              TPRINTLN(F("IDLE"));
              mInWaterWrapper.set_idle();
            break;

            default:
              TPRINTLN(F("???"));
            break;
          }
        break;

        case ProcessStep::ProcessType::Set_Input_Cleaner:
          TPRINT(F("Input Cleaner: "));
          TPRINTLN(lCurrentStep->get_value());
          mInCleanerWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Input_Sani:
          TPRINT(F("Input Sani: "));
          TPRINTLN(lCurrentStep->get_value());
          mInSaniWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Recirc_Waste:
          TPRINT(F("Recirc Waste: "));
          TPRINTLN(lCurrentStep->get_value());
          mReWasteWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Recirc_Cleaner:
          TPRINT(F("Recirc Cleaner: "));
          TPRINTLN(lCurrentStep->get_value());
          mReCleanerWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Recirc_Sani:
          TPRINT(F("Recirc Sani: "));
          TPRINTLN(lCurrentStep->get_value());
          mReSaniWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Pump:
          TPRINT(F("Pump: "));
          TPRINTLN(lCurrentStep->get_value());
          mPumpWrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Set_Co2:
          TPRINT(F("C02: "));
          TPRINTLN(lCurrentStep->get_value());
          mCo2Wrapper.set_state(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Wait_For_Input:
          TPRINTLN(F("Wait for input "));
          mInProcessWaitForInput = true;
        break;

        case ProcessStep::ProcessType::Delay:
          TPRINT(F("Delay: "));
          TPRINTLN(lCurrentStep->get_value());
          mInProcessDelay = true;
          mProcessDelayInSec = lCurrentStep->get_value();
          mProcessDelayTimer.reset();
        break;

        case ProcessStep::ProcessType::Display_Page:
          TPRINT(F("Display Page: "));
          TPRINTLN(lCurrentStep->get_value());
          mNextionWrapper.set_page(lCurrentStep->get_value());
        break;

        case ProcessStep::ProcessType::Display_Text:
          TPRINTLN(F("Display Text: "));
          TPRINT(lCurrentStep->get_sz_value_1());
          TPRINT(F(" - "));
          TPRINTLN(lCurrentStep->get_sz_value_2());
          mNextionWrapper.set_text(lCurrentStep->get_sz_value_1(), lCurrentStep->get_sz_value_2());
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

    // State Specific processing
    if(true == is_process_state(aState->get_id()))
    {
      // TODO: Display current state of outputs
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
     aStateId == mProcessPressState.get_id())
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
      mProcessStateIter = mProcessStates.begin();      
      mInProcessWaitForInput = false;
    }
    // No Button
    else if(aPageId == 2 && aCompId == 4 && aEventType == 0)
    {      
      mProcessStateIter = mProcessStates.end();
      mInProcessWaitForInput = false;  
    }
  }
  else if(is_process_state(mCurrentState->get_id()))
  {
    // Stop Button
    if(aPageId == 3 && aCompId == 3 && aEventType == 0)
    {
      mStateComplete = true;
      mCommandState = &mCompleteState;
      mProcessStateIter = mProcessStates.end();      
    }
  }
  else if(mCurrentState->get_id() == mCompleteState.get_id())
  {
    // Start over button
    if(aPageId == 3 && aCompId == 3 && aEventType == 0)
    {
      mInProcessWaitForInput = false;  
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