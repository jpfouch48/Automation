#ifndef KLEANER_STATE_H
#define KLEANER_STATE_H

#include "LinkedList.h"
#include "ProcessStep.h"

// ****************************************************************************
//
// ****************************************************************************
class KleanerState
{
public:
  // **************************************************************************
  //
  //***************************************************************************
  KleanerState(char *aStateName);

  // **************************************************************************
  // Accessor functions
  //***************************************************************************
  int                    get_id()                const { return mId;             }
  const char *           get_state_name()        const { return mStateName;      }

  // **************************************************************************
  //
  //***************************************************************************
  void add_process_step(ProcessStep *aStep)
  {
    if(aStep->get_type() == ProcessStep::Type::Delay)
    {
      ProcessStepDelay *lDelayStep = (ProcessStepDelay*)aStep;
      mTotalProcessTimeInSec += lDelayStep->get_delay() + 1;
    } 
    
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

  // **************************************************************************
  //
  //***************************************************************************
  uint16_t get_total_process_time_in_sec()
  {
    return mTotalProcessTimeInSec;
  }

protected:

private:
  uint16_t                  mTotalProcessTimeInSec;
  unsigned char             mId;
  char                     *mStateName;
  LinkedList<ProcessStep*>  mProcessSteps;
  Iterator<ProcessStep*>    mProcessStepIter;
  static unsigned char      gStateCount;
};


#endif