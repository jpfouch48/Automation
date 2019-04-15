#include "Kleaner.h"
#include <Arduino.h>

#include "KleanerConfig.h"

//#define F(s)(s)

// ****************************************************************************
// Init
//  - All ouputs OFF
//  - Load keg and wait for input
//*****************************************************************************
process_step gProcessSteps_Init[] = 
{
  { process_type::All_Off,        0 },
  { process_type::Wait_For_Input, 0 }
};

// ****************************************************************************
// Purge
//*****************************************************************************
process_step gProcessSteps_Purge[] = 
{
  { process_type::Set_Recirc_Waste, HIGH },
  { process_type::Delay,            10   },
  { process_type::Set_Co2,          HIGH },
  { process_type::Delay,            10   },
  { process_type::Set_Co2,          LOW  },
  { process_type::Delay,            10   },
  { process_type::All_Off,          0    }
};

// ****************************************************************************
// Rinse
//*****************************************************************************
process_step gProcessSteps_Rinse[] = 
{
  { process_type::Set_Recirc_Waste,   HIGH },
  { process_type::Set_Input_Water,    HIGH },
  { process_type::Delay,              15   },
  { process_type::Set_Pump,           HIGH },
  { process_type::Delay,              10   },
  { process_type::Set_Pump,           LOW  },
  { process_type::Set_Input_Water,    LOW  },
  { process_type::Set_Co2,            HIGH },
  { process_type::Delay,              10   },
  { process_type::Set_Co2,            LOW  },
  { process_type::Delay,              10   },
  { process_type::All_Off,            0    }
};

// ****************************************************************************
// Wash
//*****************************************************************************
process_step gProcessSteps_Wash[] = 
{
  { process_type::Set_Recirc_Cleaner, HIGH },
  { process_type::Set_Input_Cleaner,  HIGH },
  { process_type::Delay,              15   },
  { process_type::Set_Pump,           HIGH },
  { process_type::Delay,              10   },
  { process_type::Set_Pump,           LOW  },
  { process_type::Set_Input_Cleaner,  LOW  },
  { process_type::Set_Co2,            HIGH },
  { process_type::Delay,              10   },
  { process_type::Set_Co2,            LOW  },
  { process_type::Delay,              10   },
  { process_type::All_Off,            0    }
};

// ****************************************************************************
// Sanitize
//*****************************************************************************
process_step gProcessSteps_Sani[] = 
{
  { process_type::Set_Recirc_Sani,    HIGH },
  { process_type::Set_Input_Sani,     HIGH },
  { process_type::Delay,              15   },
  { process_type::Set_Pump,           HIGH },
  { process_type::Delay,              10   },
  { process_type::Set_Pump,           LOW  },
  { process_type::Set_Input_Sani,     LOW  },
  { process_type::Set_Co2,            HIGH },
  { process_type::Delay,              10   },
  { process_type::Set_Co2,            LOW  },
  { process_type::Delay,              10   },
  { process_type::All_Off,            0    }
};

// ****************************************************************************
// Pressurize
//*****************************************************************************
process_step gProcessSteps_Pressure[] = 
{
  // **************************************************************************
  // Pressurize
  // **************************************************************************
  { process_type::Set_Co2,            HIGH },
  { process_type::Delay,              10   },
  { process_type::Set_Co2,            LOW  },
  { process_type::All_Off,            0    }
};

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
    // States             Name              Next State         State Duration
    mSplashState  (F("Splash"),   &mMenuState,       2),
    mProcessState (F("Process"),  NULL),
    mCompleteState(F("Complete"), NULL),
        
    // Menu States
    mMenuState    (F("Menu"),     NULL),

    // State Pointers
    mCurrentState(&mSplashState),
    mCommandState(NULL),
    mReturnToState(NULL),    
    mStateComplete(false),
    mFirstTimeInState(true),

    // Menu init
    mStartMenuItem(F("Start"), NULL, NULL),
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
        mCommandState = &mProcessState;
      }
    }
    // Process Complete and Splash State Options
    // ************************************************************************
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

  // Check to see if current state is completed.
  if(true == mStateComplete)
  {
    // Current state complete, setup next state

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
bool Kleaner::process_state(const KleanerState *aState, bool aInitState)
{
  // TODO: ERROR CHECK FOR NULL STATE

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

    // Reset the display and update with state name
    mDisplayWrapper.clear();
    mDisplayWrapper.display(0, aState->get_state_name());

    // State Specific initialization
    if(aState->get_id() == mSplashState.get_id())
    {
      mDisplayWrapper.display(0, F(SPLASH_LINE_1));
      mDisplayWrapper.display(1, F(SPLASH_LINE_2));
    }
    else if(aState->get_id() == mProcessState.get_id())
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
    static int gTempState;

    // TODO: Generic processing
    // TODO : State Specific processing
    
    if(aState->get_id() == mProcessState.get_id())
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
  if(aState->get_state_time_in_sec() != 0 &&
     aState->get_state_time_in_sec() < mStateTimer.delta_in_secs())
  {
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
    case InputSource::Cleaner:    set_input_cleaner(); break;
    case InputSource::Sanitizer:  set_input_sanitizer(); break;
    case InputSource::Water:      set_input_water(); break;
    case InputSource::None:
    default:
      set_input_off();
      break;
  }
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
    case RecircDest::Cleaner:   set_recirc_cleaner(); break;
    case RecircDest::Sanitizer: set_recirc_sanitizer(); break;
    case RecircDest::Waste:     set_recirc_waste(); break;
    case RecircDest::None:
    default:
      set_recirc_off();
      break;
  }
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
  mCo2Wrapper.reset(); 
  mPumpWrapper.reset();
  set_input(InputSource::None);
  set_recirc(RecircDest::None); 
}
