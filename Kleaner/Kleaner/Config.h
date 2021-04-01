#ifndef CONFIG_H
#define CONFIG_H

// Type 0 - Sixtel
// Type 1 - Half
enum ProcessType
{
  Sixtel = 0,
  Half   = 1,
  ProcessTypeCount  = 2
};

enum CycleType
{
  Purge    = 0,
  Rinse    = 1,
  Wash     = 2,
  Sanitize = 3,
  Pressure = 4,
  CycleTypeCount    = 5
};

#define EEPROM_HDR_V1       "KK01"

struct CycleConfig
{
  unsigned char mRinseCycleCount;
  unsigned char mCo2CycleCount;
  unsigned char mPumpOnDuration;
  unsigned char mCo2OnDuration;
  unsigned char mCo2OffDuration;
  unsigned char mPurgeDuration;
};

struct TypeConfig
{
  unsigned char mBallValveDelay;    
  CycleConfig mCycles[ProcessType::ProcessTypeCount][CycleType::CycleTypeCount];
};


class Config
{
public:
  Config();

  void setup();
  void dump_config();

  void save_current_config();

  void reset_factory_config();

  TypeConfig mConfig;

  static const char * process_type_to_string(ProcessType aId)
  {
    switch(aId)
    {
      case ProcessType::Sixtel: return "Sixtel";
      case ProcessType::Half:   return "Half";
      default:                  return "???";
    }
  }

  static const char * cycle_type_to_string(CycleType aId)
  {
    switch(aId)
    {
      case CycleType::Purge:     return "Purge";
      case CycleType::Rinse:     return "Rinse";
      case CycleType::Wash:      return "Wash";
      case CycleType::Sanitize:  return "Sani";
      case CycleType::Pressure:  return "Press";
      default:                   return "???";
    }
  }

protected:


private:
};

#endif