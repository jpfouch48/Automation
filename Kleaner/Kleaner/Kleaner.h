#ifndef KLEANER_H
#define KLEANER_H

#include "KleanerConfig.h"
#include "KleanerDefines.h"
#include "KleanerState.h"
#include "OutputWrapper.h"
#include "BallValveWrapper.h"
#include "NextionWrapper.h"

#include "MilliTimer.h"

// ****************************************************************************
//
// ****************************************************************************
class Kleaner : NextionDataHandler
{
  public:
    Kleaner();

    void setup();
    void loop();

    virtual void nextion_touch_event(byte aPageId, byte aCompId, byte aEventType);
    virtual void nextion_page_event(byte aPageId);
    
  protected:

  private:
    void process_state();
    bool process_state(const KleanerState *aState, bool aInitState);

    void set_all_off();

    bool is_process_state(unsigned char aStateId);
    void update_output_display(const OutputWrapper &aOutputWrapper, int &aPrevState, char *aCompId);
    void update_output_display(const BallValveWrapper &aOutputWrapper, BallValveWrapper::State &aPrevState, char *aCompId);

    // Display Manager
    NextionWrapper mNextionWrapper;

    // Output Wrappers
    OutputWrapper mCo2Wrapper;
    OutputWrapper mPumpWrapper;

    BallValveWrapper mInWaterWrapper;
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
    KleanerState mTestState;
    KleanerState mConfirmState;
    KleanerState mCompleteState;
    
    KleanerState mProcessInitState;
    KleanerState mProcessPurgeState;
    KleanerState mProcessRinseState;
    KleanerState mProcessSaniState;
    KleanerState mProcessWashState;
    KleanerState mProcessPressState;
    KleanerState mProcessShutdownState;


    LinkedList<KleanerState*> mProcessStates;
    Iterator<KleanerState*>   mProcessStateIter;
    MilliTimer                mProcessDelayTimer;
    bool                      mInProcessDelay;
    bool                      mInProcessWaitForInput;
    uint8_t                   mProcessDelayInSec;

    MilliTimer mStateTimer;
    bool mStateComplete;
    bool mFirstTimeInState;

    // Progress Bar params
    MilliTimer                mCurrentStateTimer;
    MilliTimer                mProcessTimer;
    int                       mTotalProcessingTime;

    // Previous state variables used for
    // display of the process state
    int mPrevPumpState;
    int mPrevCo2State;
    BallValveWrapper::State mPrevInWaterState;
    int mPrevInCleanerState;
    int mPrevInSanitizerState;
    int mPrevReWasteState;
    int mPrevReCleanerState;
    int mPrevReSanitizerState;        
    int mPrevStatePercentComplete;
    int mPrevProcessPercentComplete;
};


#endif