#ifndef KLEANER_STATE_H
#define KLEANER_STATE_H

#include <Arduino.h>
#include "KleanerDefines.h"

// ****************************************************************************
//
// ****************************************************************************
class KleanerState
{
public:
  // **************************************************************************
  //
  //***************************************************************************
  KleanerState(int aId, String aStateName, KleanerState *aNextState);

  // **************************************************************************
  //
  //***************************************************************************
  KleanerState(int aId, String aStateName, KleanerState *aNextState, int aStateTimeInSec);

  // **************************************************************************
  //
  //***************************************************************************
  KleanerState(int aId, String aStateName, KleanerState *aNextState, int aStateTimeInSec, InputSource aInputSource, RecircDest aRecircDest);

  // Accessor functions
  int           get_id()                const { return mId;             }
  String        get_state_name()        const { return mStateName;      }
  int           get_state_time_in_sec() const { return mStateTimeInSec; }
  InputSource   get_input_source()      const { return mInputSource;    }
  RecircDest    get_recirc_dest()       const { return mRecircDest;     } 
  KleanerState* get_next_state()        const { return mNextState;      }


protected:


private:
  int           mId;
  String        mStateName;
  int           mStateTimeInSec;
  InputSource   mInputSource;
  RecircDest    mRecircDest;
  KleanerState *mNextState;
};


#endif