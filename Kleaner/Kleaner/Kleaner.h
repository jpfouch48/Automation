#ifndef KLEANER_H
#define KLEANER_H

#include "KleanerConfig.h"
#include "KleanerDefines.h"

#include "KleanerState.h"
#include "KleanerMenuItem.h"

#include "InputWrapper.h"
#include "OutputWrapper.h"
#include "DisplayWrapper.h"
#include "MilliTimer.h"

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
    void update_output_display(const OutputWrapper &aOutputWrapper, int &aPrevState, String aDisplayVal, int aDisplayCol);

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
    KleanerState mProcessState;
    KleanerState mCompleteState;

    MilliTimer mStateTimer;
    bool mStateComplete;
    bool mFirstTimeInState;

    // ****************************************************************************
    // Menu System
    // ****************************************************************************

    // Main Menu
    // ****************************************************************************
    KleanerMenuItem mStartMenuItem;
    KleanerMenuItem *mCurrentMenuItem;

    // Previous state variables used for
    // display of the process state
    int mPrevPumpState;
    int mPrevCo2State;
    int mPrevInWaterState;
    int mPrevInCleanerState;
    int mPrevInSanitizerState;
    int mPrevReWasteState;
    int mPrevReCleanerState;
    int mPrevReSanitizerState;    
};


#endif