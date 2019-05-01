#include "KleanerState.h"


unsigned char KleanerState::gStateCount = 0;

// ****************************************************************************
// See header file for details
// ****************************************************************************
KleanerState::KleanerState(String aStateName) :
  mId(gStateCount++),
  mStateName(aStateName)
{

}
