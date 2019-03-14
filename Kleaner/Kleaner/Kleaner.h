#ifndef KLEANER_H
#define KLEANER_H

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
    KleanerState mPurgeState;
    KleanerState mPreRinseState;
    KleanerState mWashState;
    KleanerState mPostRinseState;
    KleanerState mSanitizeState;
    KleanerState mPressurizeState;
    KleanerState mCompleteState;

#if defined KLEANER_TEST_MENU  
    KleanerState mTestMenuState;
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

#if defined KLEANER_TEST_MENU
    KleanerMenuItem mTestMenuItem;
    KleanerMenuItem mTestStateMenuItem;

    // Test Menu
    // ****************************************************************************
    KleanerMenuItem mTestMenuCycleInput;    
    KleanerMenuItem mTestMenuCycleRecirc;
    KleanerMenuItem mTestMenuTogglePump;
    KleanerMenuItem mTestMenuToggleCo2;
    KleanerMenuItem mTestMenuExit;

    // Test State Menu
    // ****************************************************************************
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
};


#endif