#ifndef KLEANER_STATE_H
#define KLEANER_STATE_H

#include <Arduino.h>
#include "KleanerDefines.h"
#include "OutputWrapper.h"

// ****************************************************************************
//
// ****************************************************************************
class KleanerState
{
public:
  // **************************************************************************
  //
  //***************************************************************************
  KleanerState(String                 aStateName, 
               KleanerState          *aNextState, 
               int                    aStateTimeInSec  = 0);

  // Accessor functions
  int                    get_id()                const { return mId;             }
  String                 get_state_name()        const { return mStateName;      }
  int                    get_state_time_in_sec() const { return mStateTimeInSec; }
  KleanerState*          get_next_state()        const { return mNextState;      }

protected:


private:
  unsigned char          mId;
  String                 mStateName;
  int                    mStateTimeInSec;
  KleanerState          *mNextState;
  static unsigned char   gStateCount;
};


#endif