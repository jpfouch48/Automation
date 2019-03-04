#include "KleanerState.h"



// ****************************************************************************
// See header file for details
// ****************************************************************************
KleanerState::KleanerState(int           aId,
                           String        aStateName, 
                           KleanerState *aNextState) :
  KleanerState(aId, 
               aStateName, 
               aNextState, 
               0)
{

}

// ****************************************************************************
// See header file for details
// ****************************************************************************
KleanerState::KleanerState(int           aId,
                           String        aStateName, 
                           KleanerState *aNextState, 
                           int           aStateTimeInSec) :
  KleanerState(aId,
               aStateName, 
               aNextState, 
               aStateTimeInSec, 
               InputSource::None, 
               RecircDest::None)
{

}

// ****************************************************************************
// See header file for details
// ****************************************************************************
KleanerState::KleanerState(int           aId,
                           String        aStateName, 
                           KleanerState *aNextState,
                           int           aStateTimeInSec, 
                           InputSource   aInputSource, 
                           RecircDest    aRecircDest) :
  mId(aId),
  mStateName(aStateName),
  mNextState(aNextState),
  mStateTimeInSec(aStateTimeInSec),
  mInputSource(aInputSource),
  mRecircDest(aRecircDest)
{

}
