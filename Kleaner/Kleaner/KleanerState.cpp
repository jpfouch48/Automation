#include "KleanerState.h"


unsigned char KleanerState::gStateCount = 0;

// ****************************************************************************
// See header file for details
// ****************************************************************************
KleanerState::KleanerState(char *aStateName, bool aIsProcessState) :
  mId(gStateCount++),
  mStateName(aStateName),
  mTotalProcessTimeInSec(0),
  mIsProcessState(aIsProcessState)
{
}
