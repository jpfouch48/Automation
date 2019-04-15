#ifndef KLEANER_STATE_H
#define KLEANER_STATE_H

#include <Arduino.h>
#include "KleanerDefines.h"
#include "OutputWrapper.h"
#include "LinkedList.h"


// TODO: CREATE A CLASS FOR THE PROCESS STATE
enum class process_type
{
  All_Off,                  /* No Input               */
  Set_Input_Water,          /* mValue   : HIGH or LOW */
  Set_Input_Cleaner,        /* mValue   : HIGH or LOW */
  Set_Input_Sani,           /* mValue   : HIGH or LOW */
  Set_Recirc_Waste,         /* mValue   : HIGH or LOW */
  Set_Recirc_Cleaner,       /* mValue   : HIGH or LOW */
  Set_Recirc_Sani,          /* mValue   : HIGH or LOW */
  Set_Pump,                 /* mValue   : HIGH or LOW */
  Set_Co2,                  /* mValue   : HIGH or LOW */
  Wait_For_Input,           /* No Input               */
  Delay                     /* mValue   : Seconds     */
};

struct process_step
{
  process_type mProcessType;
  uint8_t      mValue;
}; 


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
  unsigned char             mId;
  String                    mStateName;
  int                       mStateTimeInSec;
  KleanerState             *mNextState;

  LinkedList<process_step*> mProcessSteps;

  static unsigned char      gStateCount;
};


#endif