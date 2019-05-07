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

    virtual void IncomingData(byte* mData, int mDataSize);

  protected:

  private:
    void process_state();
    bool process_state(const KleanerState *aState, bool aInitState);

    void set_all_off();

    bool is_process_state(unsigned char aStateId);

    // Display Manager
    NextionWrapper mNextionWrapper;

    // Output Wrappers
    OutputWrapper mCo2Wrapper;
    OutputWrapper mPumpWrapper;

    BallValveWrapper  mInWaterWrapper;


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
    
    KleanerState mProcessInitState;
    KleanerState mProcessPurgeState;
    KleanerState mProcessRinseState;
    KleanerState mProcessSaniState;
    KleanerState mProcessWashState;
    KleanerState mProcessPressState;
    KleanerState mProcessCompleteState;


    LinkedList<KleanerState*> mProcessStates;
    Iterator<KleanerState*>   mProcessStateIter;
    MilliTimer                mProcessDelayTimer;
    bool                      mInProcessDelay;
    bool                      mInProcessWaitForInput;
    uint8_t                   mProcessDelayInSec;

    MilliTimer mStateTimer;
    bool mStateComplete;
    bool mFirstTimeInState;
};


#endif