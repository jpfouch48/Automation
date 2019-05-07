#ifndef KLEANER_STATE_H
#define KLEANER_STATE_H

#include <Arduino.h>
#include "KleanerDefines.h"
#include "OutputWrapper.h"
#include "BallValveWrapper.h"
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
    Display_Page,             /* mValue   : Page Num    */
    Display_Text              /*                        */
  };

  ProcessStep(ProcessType aType, int aValue) : mType(aType), mValue(aValue), mSzValue1(NULL), mSzValue2(NULL)
  {
  }

  ProcessStep(ProcessType aType, char *aSzValue1, char *aSzValue2) : mType(aType), mValue(0), mSzValue1(aSzValue1), mSzValue2(aSzValue2)
  {
  }

  ProcessStep(ProcessType aType, char *aSzValue1) : mType(aType), mValue(0), mSzValue1(aSzValue1), mSzValue2(NULL)
  {
  }

  ProcessStep(ProcessType aType) : mType(aType), mValue(0), mSzValue1(NULL), mSzValue2(NULL)
  {
  }

  
  ProcessStep(ProcessType aType, BallValveWrapper::State aState) : ProcessStep(aType, (int)aState)
  {  
  } 

  ProcessType get_type() { return mType; }
  int get_value() { return mValue; }

  const char *get_sz_value_1() { return mSzValue1; }
  const char *get_sz_value_2() { return mSzValue2; }

protected:

private:
  ProcessType  mType;
  uint8_t      mValue;
  char        *mSzValue1;
  char        *mSzValue2;
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
  unsigned char             mId;
  char                     *mStateName;
  LinkedList<ProcessStep*>  mProcessSteps;
  Iterator<ProcessStep*>    mProcessStepIter;
  static unsigned char      gStateCount;
};


#endif