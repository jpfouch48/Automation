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

    Input_Off,
    Input_Water,
    Input_Cleaner,
    Input_Sanitizer,

    Output_Off,
    Output_Waste,
    Output_Cleaner,
    Output_Sanitizer,

    Co2_Off,
    Co2_On,

    Pump_Off,
    Pump_On
  };

  ProcessStep(Type aType) : ProcessStep(aType, 0)
  {
  }

  ProcessStep(Type aType, int aDelayInSec) : mType(aType), mDelayInSec(aDelayInSec)
  {
  }

  Type get_type()  const { return mType;  }
  int  get_delay() const { return mDelayInSec; }

protected:
private:
  Type    mType;
  uint8_t mDelayInSec;
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepCo2Off : public ProcessStep
{
public:
  ProcessStepCo2Off(uint8_t aDelay=0) : ProcessStep(Type::Co2_Off, aDelay)
  {
  }

protected:
private:
};


// ****************************************************************************
//
// ****************************************************************************
class ProcessStepCo2On : public ProcessStep
{
public:
  ProcessStepCo2On(uint8_t aDelay=0) : ProcessStep(Type::Co2_On, aDelay)
  {
  }

protected:
private:
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepPumpOff : public ProcessStep
{
public:
  ProcessStepPumpOff(uint8_t aDelay=0) : ProcessStep(Type::Pump_Off, aDelay)
  {
  }

protected:
private:
};


// ****************************************************************************
//
// ****************************************************************************
class ProcessStepPumpOn : public ProcessStep
{
public:
  ProcessStepPumpOn(uint8_t aDelay=0) : ProcessStep(Type::Pump_On, aDelay)
  {
  }

protected:
private:
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepInputOff : public ProcessStep
{
public:
  ProcessStepInputOff(uint8_t aDelay=0) : ProcessStep(Type::Input_Off, aDelay)
  {
  }

protected:
private:
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepInputWater : public ProcessStep
{
public:
  ProcessStepInputWater(uint8_t aDelay=0) : ProcessStep(Type::Input_Water, aDelay)
  {
  }

protected:
private:
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepInputCleaner : public ProcessStep
{
public:
  ProcessStepInputCleaner(uint8_t aDelay=0) : ProcessStep(Type::Input_Cleaner, aDelay)
  {
  }

protected:
private:
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepInputSanitizer : public ProcessStep
{
public:
  ProcessStepInputSanitizer(uint8_t aDelay=0) : ProcessStep(Type::Input_Sanitizer, aDelay)
  {
  }

protected:
private:
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepOutputOff : public ProcessStep
{
public:
  ProcessStepOutputOff(uint8_t aDelay=0) : ProcessStep(Type::Output_Off, aDelay)
  {
  }

protected:
private:
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepOutputWaste : public ProcessStep
{
public:
  ProcessStepOutputWaste(uint8_t aDelay=0) : ProcessStep(Type::Output_Waste, aDelay)
  {
  }

protected:
private:
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepOutputCleaner : public ProcessStep
{
public:
  ProcessStepOutputCleaner(uint8_t aDelay=0) : ProcessStep(Type::Output_Cleaner, aDelay)
  {
  }

protected:
private:
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepOutputSanitizer : public ProcessStep
{
public:
  ProcessStepOutputSanitizer(uint8_t aDelay=0) : ProcessStep(Type::Output_Sanitizer, aDelay)
  {
  }

protected:
private:
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepDisplayValue : public ProcessStep
{
public:
  ProcessStepDisplayValue(const char *aComp, int16_t aValue) : 
    ProcessStep(Type::Display_Value), 
    mComp(aComp), 
    mValue(aValue)
  {
  }

  const char *get_comp()  const { return mComp; }
  int16_t     get_value() const { return mValue; }
  
protected:
private:
  const char *mComp;
  int16_t     mValue;
};

// ****************************************************************************
//
// ****************************************************************************
class ProcessStepDisplayText : public ProcessStep
{
public:
  ProcessStepDisplayText(const char *aComp, const char *aValue) : 
    ProcessStep(Type::Display_Text), 
    mComp(aComp), 
    mValue(aValue)
  {
  }

  const char *get_comp()  const { return mComp; }
  const char *get_value() const { return mValue; }
  
protected:
private:
  const char    *mComp;
  const char    *mValue;
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

  uint8_t get_page_id() const { return mPageId; }
  
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
    ProcessStep(Type::Delay, aDelayInSec)
  {
  }
protected:
private:
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
  ProcessStepResetOutputs(uint8_t aDelay=0) : 
    ProcessStep(Type::Reset_Outputs, aDelay)
  {
  }
  
protected:
private:

};

#endif