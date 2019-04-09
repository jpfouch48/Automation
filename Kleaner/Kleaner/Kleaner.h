#ifndef KLEANER_H
#define KLEANER_H

#include "KleanerConfig.h"
#include "KleanerDefines.h"

#include "KleanerState.h"
#include "KleanerMenuItem.h"

#include "InputWrapper.h"
#include "OutputWrapper.h"
#include "DisplayWrapper.h"

class Kleaner
{
  public:
    Kleaner();

    void on_up_button(int aState);
    void on_dn_button(int aState);
    void on_en_button(int aState);

    void setup();
    void loop();

  protected:

  private:
    void process_state();
    bool process_state(const KleanerState *aState, bool aInitState);

    void set_input(InputSource aInputSource);
    void set_input_off();
    void set_input_water();
    void set_input_cleaner();
    void set_input_sanitizer();

    void set_recirc(RecircDest aRecircDest);
    void set_recirc_off();
    void set_recirc_waste();
    void set_recirc_sanitizer();
    void set_recirc_cleaner();

    void set_all_off();

    // Display Wrappers
    DisplayWrapper mDisplayWrapper;

    // Output Wrappers
    OutputWrapper mCo2Wrapper;
    OutputWrapper mPumpWrapper;

    OutputWrapper mInWaterWrapper;
    OutputWrapper mInSaniWrapper;
    OutputWrapper mInCleanerWrapper;

    OutputWrapper mReWasteWrapper;
    OutputWrapper mReSaniWrapper;
    OutputWrapper mReCleanerWrapper;  


    // States
    KleanerState *mCurrentState;
    KleanerState *mCommandState;
    KleanerState *mReturnToState;

    KleanerState mSplashState;
    KleanerState mMenuState;
    KleanerState mDumpState;
    KleanerState mPreRinseState;
    KleanerState mPurge1State;
    KleanerState mWashState;
    KleanerState mPurge2State;
    KleanerState mPostRinseState;
    KleanerState mPurge3State;
    KleanerState mSanitizeState;
    KleanerState mPurge4State;
    KleanerState mPressurizeState;
    KleanerState mCompleteState;

    KleanerState mPrimePumpState;

#if defined KLEANER_TEST_MENU  
    KleanerState mTestMenuState;
#endif

#if defined KLEANER_TEST_STATE_MENU
    KleanerState mTestStateMenuState;
#endif

    MilliTimer mStateTimer;
    bool mStateComplete;
    bool mFirstTimeInState;

    // ****************************************************************************
    // Menu System
    // ****************************************************************************

    // Main Menu
    // ****************************************************************************
    KleanerMenuItem mStartMenuItem;
    KleanerMenuItem mPrimePumpMenuItem;

#if defined KLEANER_TEST_MENU
    // Test Menu
    // ****************************************************************************
    KleanerMenuItem mTestMenuItem;

    KleanerMenuItem mTestMenuCycleInput;    
    KleanerMenuItem mTestMenuCycleRecirc;
    KleanerMenuItem mTestMenuTogglePump;
    KleanerMenuItem mTestMenuToggleCo2;
    KleanerMenuItem mTestMenuExit;
#endif

#if defined KLEANER_TEST_STATE_MENU
    // Test State Menu
    // ****************************************************************************
    KleanerMenuItem mTestStateMenuItem;

    KleanerMenuItem mTestStateMenuDump;    
    KleanerMenuItem mTestStateMenuPurge;    
    KleanerMenuItem mTestStateMenuPreRinse;
    KleanerMenuItem mTestStateMenuWash;
    KleanerMenuItem mTestStateMenuPostRinse;
    KleanerMenuItem mTestStateMenuSanitize;
    KleanerMenuItem mTestStateMenuPressurize;
    KleanerMenuItem mTestStateMenuExit;
#endif

    KleanerMenuItem *mCurrentMenuItem;

    InputSource mTestInputSource;
    RecircDest  mTestRecircDest;

    // Previous state variables used for
    // display of the process state
    int mPrevPumpState;
    int mPrevCo2State;
    int mPrevInputState;
    int mPrevRecircState;    
};


#endif