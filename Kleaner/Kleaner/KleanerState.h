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
  KleanerState(String                 aStateName, 
               KleanerState          *aNextState, 
               int                    aStateTimeInSec = 0, 
               InputSource            aInputSource    = InputSource::None, 
               RecircDest             aRecircDest     = RecircDest::None);

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

  static int    gStateCount;
};


#endif