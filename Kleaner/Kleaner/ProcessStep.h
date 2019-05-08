#ifndef PROCESS_STEP_H
#define PROCESS_STEP_H

#include "BallValveWrapper.h"
#include "OutputWrapper.h"

class ProcessStep
{
public:
  enum class Type
  {
    Reset_Outputs,
    Wait_For_Input,
    Delay,
    Display_Page,
    Display_Text,
    Display_Value,

    Control_Ball_Valve_Wrapper,
    Control_Output_Wrapper,
  };

  ProcessStep(Type aType, int aValue) : mType(aType), mValue(aValue)
  {
  }

  ProcessStep(Type aType) : mType(aType), mValue(0)
  {
  }

  ProcessStep(Type aType, BallValveWrapper::State aState) : ProcessStep(aType, (int)aState)
  {  
  } 

  Type get_type() { return mType; }
  int get_value() { return mValue; }

protected:
private:
  Type  mType;
  uint8_t      mValue;
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepDisplayValue : public ProcessStep
{
public:
  ProcessStepDisplayValue(char *aComp, int16_t aValue) : 
    ProcessStep(Type::Display_Value), 
    mComp(aComp), 
    mValue(aValue)
  {
  }

  const char *get_comp()  { return mComp; }
  int16_t     get_value() { return mValue; }
  
protected:
private:
  char    *mComp;
  int16_t  mValue;
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepDisplayText : public ProcessStep
{
public:
  ProcessStepDisplayText(char *aComp, char *aValue) : 
    ProcessStep(Type::Display_Text), 
    mComp(aComp), 
    mValue(aValue)
  {
  }

  const char *get_comp()  { return mComp; }
  const char *get_value() { return mValue; }
  
protected:
private:
  char    *mComp;
  char    *mValue;
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepDisplayPage : public ProcessStep
{
public:
  ProcessStepDisplayPage(uint8_t aPageId) : 
    ProcessStep(Type::Display_Page), 
    mPageId(aPageId)
  {
  }

  uint8_t get_page_id()  { return mPageId; }
  
protected:
private:
  uint8_t mPageId;
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepDelay: public ProcessStep
{
public:
  ProcessStepDelay(uint8_t aDelayInSec) : 
    ProcessStep(Type::Delay), 
    mDelayInSec(aDelayInSec)
  {
  }

  uint8_t get_delay()  { return mDelayInSec; }
  
protected:
private:
  uint8_t mDelayInSec;
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepWaitForInput: public ProcessStep
{
public:
  ProcessStepWaitForInput() : 
    ProcessStep(Type::Wait_For_Input)
  {
  }
  
protected:
private:

};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepResetOutputs: public ProcessStep
{
public:
  ProcessStepResetOutputs() : 
    ProcessStep(Type::Reset_Outputs)
  {
  }
  
protected:
private:

};


// ****************************************************************************
//
// ****************************************************************************
class ProcessStepControlOutputWrapper: public ProcessStep
{
public:
  ProcessStepControlOutputWrapper(OutputWrapper *aOutputWrapper, uint8_t aState) : 
    ProcessStep(Type::Control_Output_Wrapper),
    mOutputWrapper(aOutputWrapper),
    mState(aState)
  {
  }

  OutputWrapper *get_wrapper() { return mOutputWrapper; }
  uint8_t        get_state()          { return mState; }

protected:
private:
  OutputWrapper *mOutputWrapper;
  uint8_t        mState;
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepControlBallValveWrapper: public ProcessStep
{
public:
  ProcessStepControlBallValveWrapper(BallValveWrapper *aWrapper, BallValveWrapper::State aState) : 
    ProcessStep(Type::Control_Ball_Valve_Wrapper),
    mWrapper(aWrapper),
    mState(aState)
  {
  }

  BallValveWrapper        *get_wrapper() { return mWrapper; }
  BallValveWrapper::State  get_state()          { return mState; }

protected:
private:
  BallValveWrapper        *mWrapper;
  BallValveWrapper::State  mState;
};

#endif