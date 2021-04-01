#ifndef KLEANER_STATE_H
#define KLEANER_STATE_H

#include "LinkedList.h"
#include "ProcessStep.h"
#include "KleanerConfig.h"
#include "Config.h"

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


  // **************************************************************************
  //
  //***************************************************************************
  bool init_process_cycle(ProcessStep::Type aOutputType, 
                          ProcessStep::Type aInputType,
                          CycleConfig       aCycleConfig)
  {
    bool lHasOutput = true;

    TSPRINT(get_state_name());

    TPRINT(" OUT: ");
    switch(aOutputType)
    {
      case ProcessStep::Type::Output_Cleaner:
        TPRINT("C ");
        add_process_step(new ProcessStepOutputCleaner(BALL_VALVE_DELAY));
        break;

      case ProcessStep::Type::Output_Waste:
        TPRINT("W ");
        add_process_step(new ProcessStepOutputWaste(BALL_VALVE_DELAY));
        break;

      case ProcessStep::Type::Output_Sanitizer:
        TPRINT("S ");
        add_process_step(new ProcessStepOutputSanitizer(BALL_VALVE_DELAY));
        break;

      case ProcessStep::Type::Output_Off:
        // Nothing to do here
        TPRINT("  ");
        lHasOutput = false;
        break;

      default:
      {
        // TODO: MAKE MORE VERBOSE
       TSPRINTLN(F("init_process_cycle - invalid output type"));
      } return false;
    }

    for(int lnRinseIndex = 0; lnRinseIndex < aCycleConfig.mRinseCycleCount; lnRinseIndex++)
    {
      if(aCycleConfig.mPumpOnDuration > 0)
      {
        if(0 == lnRinseIndex) TPRINT(F(" IN: "));

        switch(aInputType)
        {
          case ProcessStep::Type::Input_Cleaner:
            if(0 == lnRinseIndex) TPRINT(F("C "));
            add_process_step(new ProcessStepInputCleaner(BALL_VALVE_DELAY));
            break;

          case ProcessStep::Type::Input_Water:
            if(0 == lnRinseIndex) TPRINT(F("W "));
            add_process_step(new ProcessStepInputWater(BALL_VALVE_DELAY));
            break;

          case ProcessStep::Type::Input_Sanitizer:
            if(0 == lnRinseIndex) TPRINT(F("S "));
            add_process_step(new ProcessStepInputSanitizer(BALL_VALVE_DELAY));
            break;

          default:
            // TODO: MAKE MORE VERBOSE
            if(0 == lnRinseIndex) TPRINT(F(" "));
            TSPRINTLN(F("init_process_cycle - invalid input type"));
            return false;
        }

        add_process_step(new ProcessStepPumpOn(aCycleConfig.mPumpOnDuration));
        add_process_step(new ProcessStepPumpOff());
        add_process_step(new ProcessStepInputOff(BALL_VALVE_DELAY));
      }

      for (int lnCo2Index = 0; lnCo2Index < aCycleConfig.mCo2CycleCount; lnCo2Index++)
      {
        add_process_step(new ProcessStepCo2On(aCycleConfig.mCo2OnDuration));
        add_process_step(new ProcessStepCo2Off(aCycleConfig.mCo2OffDuration));
      }

      if(aCycleConfig.mPurgeDuration > 0)
        add_process_step(new ProcessStepDelay(aCycleConfig.mPurgeDuration));
    }

    if(true == lHasOutput)
      add_process_step(new ProcessStepOutputOff(BALL_VALVE_DELAY));
      
    TPRINT(F(" Dur - "));
    TPRINTLN(get_total_process_time_in_sec());

    return true;
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