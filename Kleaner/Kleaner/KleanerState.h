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
  KleanerState(const char *aStateName, bool aIsProcessState);

  // **************************************************************************
  // Accessor functions
  //***************************************************************************
  int                    get_id()                const { return mId;             }
  const char *           get_state_name()        const { return mStateName;      }
  bool                   is_process_state()      const { return mIsProcessState; }

  // **************************************************************************
  //
  //***************************************************************************
  void add_process_step(ProcessStep *aStep)
  {
    if(0 != aStep->get_delay())
    {
      mTotalProcessTimeInSec += aStep->get_delay();
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
  uint16_t get_total_process_time_in_sec() const
  {
    return mTotalProcessTimeInSec;
  }

protected:

private:
  uint16_t                  mTotalProcessTimeInSec;
  unsigned char             mId;
  const char               *mStateName;
  LinkedList<ProcessStep*>  mProcessSteps;
  Iterator<ProcessStep*>    mProcessStepIter;
  bool                      mIsProcessState;  
  static unsigned char      gStateCount;
};


#endif