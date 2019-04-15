#include "KleanerState.h"


unsigned char KleanerState::gStateCount = 0;

// ****************************************************************************
// See header file for details
// ****************************************************************************
KleanerState::KleanerState(String                 aStateName, 
                           KleanerState          *aNextState,
                           int                    aStateTimeInSec) :
  mId(gStateCount++),
  mStateName(aStateName),
  mNextState(aNextState),
  mStateTimeInSec(aStateTimeInSec)
{

}
