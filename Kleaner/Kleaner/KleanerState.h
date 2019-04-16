#ifndef KLEANER_STATE_H
#define KLEANER_STATE_H

#include <Arduino.h>
#include "KleanerDefines.h"
#include "OutputWrapper.h"
#include "LinkedList.h"

class ProcessStep
{
public:
  enum class ProcessType
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
    Delay,                    /* mValue   : Seconds     */
    Display,                  /* mSzValue : String      */
  };

  ProcessStep(ProcessType aType) : mType(aType), mValue(0), mSzValue(NULL)
  {
  }

  ProcessStep(ProcessType aType, int aValue) : mType(aType), mValue(aValue), mSzValue(NULL)
  {
  }

  ProcessStep(ProcessType aType, int aValue, String *aSzValue) : mType(aType), mValue(aValue), mSzValue(aSzValue) 
  {

  }

  ProcessType get_type() { return mType; }
  int get_value() { return mValue; }
  String *get_sz_value() { return mSzValue; }

protected:

private:
  ProcessType  mType;
  uint8_t      mValue;
  String      *mSzValue;
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
               int                    aStateTimeInSec  = 0);

  // **************************************************************************
  // Accessor functions
  //***************************************************************************
  int                    get_id()                const { return mId;             }
  String                 get_state_name()        const { return mStateName;      }
  int                    get_state_time_in_sec() const { return mStateTimeInSec; }

  // **************************************************************************
  //
  //***************************************************************************
  void add_process_step(ProcessStep *aStep)
  {
    mProcessSteps.push_back(aStep);
  }

  // **************************************************************************
  //
  //***************************************************************************
  void init_state()
  {
    mProcessStepIter = mProcessSteps.begin();
  }

  // **************************************************************************
  //
  //***************************************************************************
  Iterator<ProcessStep*>& process_list_iter()
  {
    return mProcessStepIter;
  }

  // **************************************************************************
  //
  //***************************************************************************
  LinkedList<ProcessStep*>& process_list()
  {
    return mProcessSteps;
  }


protected:


private:
  unsigned char            mId;
  String                   mStateName;
  int                      mStateTimeInSec;
  LinkedList<ProcessStep*> mProcessSteps;
  Iterator<ProcessStep*>   mProcessStepIter;
  static unsigned char     gStateCount;
};


#endif