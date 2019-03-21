#include "KleanerState.h"


int KleanerState::gStateCount = 0;

// ****************************************************************************
// See header file for details
// ****************************************************************************
KleanerState::KleanerState(String                 aStateName, 
                           KleanerState          *aNextState,
                           int                    aStateTimeInSec, 
                           InputSource            aInputSource, 
                           RecircDest             aRecircDest,
                           OutputWrapper::Config *aPumpConfig,
                           OutputWrapper::Config *aCo2Config,
                           bool                   aIsProcessState) :
  mId(gStateCount++),
  mStateName(aStateName),
  mNextState(aNextState),
  mStateTimeInSec(aStateTimeInSec),
  mInputSource(aInputSource),
  mRecircDest(aRecircDest),
  mPumpConfig(aPumpConfig),
  mCo2Config(aCo2Config),
  mIsProcessState(aIsProcessState)
{

}
