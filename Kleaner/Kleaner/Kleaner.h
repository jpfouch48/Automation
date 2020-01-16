#ifndef KLEANER_H
#define KLEANER_H

#include "KleanerConfig.h"
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

    enum class KegType
    {
      KegType_Sixtel,
      KegType_Half,
      KegType_Unknown      
    };


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

    void update_output_display(const OutputWrapper &aOutputWrapper, int &aPrevState, char *aCompId);
    void update_output_display(const BallValveWrapper &aOutputWrapper, BallValveWrapper::State &aPrevState, char *aCompId);

    // Display Manager
    NextionWrapper            mNextionWrapper;

    // Output Wrappers
    OutputWrapper             mCo2Wrapper;
    OutputWrapper             mPumpWrapper;

    BallValveWrapper          mInWaterWrapper;
    BallValveWrapper          mInSaniWrapper;
    BallValveWrapper          mInCleanerWrapper;

    OutputWrapper             mReWasteWrapper;
    OutputWrapper             mReSaniWrapper;
    OutputWrapper             mReCleanerWrapper; 

    // States
    KleanerState             *mCurrentState;
    KleanerState             *mCommandState;
    KleanerState             *mReturnToState;

    KleanerState              mSplashState;
    KleanerState              mMenuState;
    KleanerState              mTestOutputState;
    KleanerState              mTestPhaseState;
    KleanerState              mConfirmState;
    KleanerState              mCompleteState;
    
    KleanerState              mProcessInitState;
    KleanerState              mProcessPurgeStateSixtel;
    KleanerState              mProcessRinseStateSixtel;
    KleanerState              mProcessSaniStateSixtel;
    KleanerState              mProcessWashStateSixtel;
    KleanerState              mProcessPressStateSixtel;

    KleanerState              mProcessPurgeStateHalf;
    KleanerState              mProcessRinseStateHalf;
    KleanerState              mProcessSaniStateHalf;
    KleanerState              mProcessWashStateHalf;
    KleanerState              mProcessPressStateHalf;

    KleanerState              mProcessShutdownState;


    LinkedList<KleanerState*> mProcessStatesSixtel;
    LinkedList<KleanerState*> mProcessStatesHalf;

    Iterator<KleanerState*>   mProcessStateIter;
    MilliTimer                mProcessDelayTimer;
    bool                      mInProcessDelay;
    bool                      mInProcessWaitForInput;
    uint8_t                   mProcessDelayInSec;

    MilliTimer                mStateTimer;
    bool                      mStateComplete;
    bool                      mFirstTimeInState;

    // Progress Bar params
    MilliTimer                mCurrentStateTimer;
    MilliTimer                mProcessTimer;
    int                       mTotalProcessingTime;

    // Previous state variables used for
    // display of the process state
    int                       mPrevPumpState;
    int                       mPrevCo2State;
    BallValveWrapper::State   mPrevInWaterState;
    BallValveWrapper::State   mPrevInCleanerState;
    BallValveWrapper::State   mPrevInSanitizerState;
    int                       mPrevReWasteState;
    int                       mPrevReCleanerState;
    int                       mPrevReSanitizerState;   
    int                       mPrevStatePercentComplete;
    int                       mPrevProcessPercentComplete;
    KegType                   mKegType;
};


#endif