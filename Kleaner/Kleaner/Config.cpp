#include "Config.h"
#include "KleanerConfig.h"

#include <EEPROM.h>

TypeConfig mDefalutConfig
{
  // Ball Valve Delay
  8,

  {
    // Cycles, Co2 Cyles, Pump On, Co2 On, Co2 Off, Purge Delay
    { // Sixtel Config
      {  1,  1,  0, 10,  5,  0 }, // Purge
      {  3,  1, 15, 10,  0, 10 }, // Rinse
      {  2,  1, 30, 10,  0, 30 }, // Wash
      {  1,  1, 10,  7,  0, 10 }, // Sani
      {  1,  1,  0, 10,  0,  0 }  // Pressure
    },

    { // Half Keg Config
      {  1,  1,  0, 15,  5,  0 }, // Purge
      {  3,  1, 15, 10,  0, 35 }, // Rinse
      {  2,  1, 30, 10,  0, 60 }, // Wash
      {  1,  1, 10, 10,  0, 50 }, // Sani
      {  1,  1,  0, 20,  0,  0 }  // Pressure
    }
  }
};

// ****************************************************************************
//
// ****************************************************************************
template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.update(ee++, *p++);
    return i;
}

// ****************************************************************************
//
// ****************************************************************************
template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}


// ****************************************************************************
//
// ****************************************************************************
Config::Config()
{
}

// ****************************************************************************
//
// ****************************************************************************
void Config::setup()
{
  unsigned char lHdr[5];
  lHdr[0] = EEPROM.read(0);
  lHdr[1] = EEPROM.read(1);
  lHdr[2] = EEPROM.read(2);
  lHdr[3] = EEPROM.read(3);
  lHdr[4] = '\0';

  TSPRINT("Config Setup Hdr - ");
  TPRINTLN((char*)lHdr);

  // Read the header from the EEPROM
  if(EEPROM_HDR_V1[0] == lHdr[0] &&
     EEPROM_HDR_V1[1] == lHdr[1] &&
     EEPROM_HDR_V1[2] == lHdr[2] &&
     EEPROM_HDR_V1[3] == lHdr[3])
  {
    TSPRINTLN("Config Setup - Valid Header Found");
    EEPROM_readAnything(4, mConfig);
  }
  else
  {
    mConfig.mBallValveDelay = mDefalutConfig.mBallValveDelay;

    // No valid header, copy the default and write to eeprom
    memcpy(mConfig.mCycles, mDefalutConfig.mCycles, sizeof(mConfig.mCycles));

    TSPRINT("Config Setup - Invalid Header Found - ");
    TPRINTLN((char *)lHdr);
    TSPRINTLN("Config Setup - Writing default");

    EEPROM_writeAnything(0, EEPROM_HDR_V1);
    EEPROM_writeAnything(4, mConfig);
  }

  dump_config();
}

// ****************************************************************************
//
// ****************************************************************************
void Config::save_current_config()
{
  TSPRINT("Config Write - ");
  TPRINTLN(EEPROM_HDR_V1);
  TSPRINTLN("Config Setup - Writing config");

  EEPROM_writeAnything(0, EEPROM_HDR_V1);
  EEPROM_writeAnything(4, mConfig);
}

// ****************************************************************************
//
// ****************************************************************************
void Config::reset_factory_config()
{
  memcpy(&mConfig, &mDefalutConfig, sizeof(mConfig));
  TSPRINT("Config Reset Factory - ");
}


// ****************************************************************************
//
// ****************************************************************************
void Config::dump_config()
{
  TSPRINT("Ball Valve Delay: ");
  TPRINTLN((int)mConfig.mBallValveDelay);

  for(int lTypeIndex = 0; lTypeIndex < ProcessType::ProcessTypeCount; lTypeIndex++)
  {
    TSPRINT(process_type_to_string((ProcessType)lTypeIndex));
    TPRINTLN(" CFG ---");

    for(int lCycleIndex = 0; lCycleIndex < CycleType::CycleTypeCount; lCycleIndex++)
    {
      TSPRINT(cycle_type_to_string((CycleType)lCycleIndex));
      TPRINT(": ");

      TPRINT((int)mConfig.mCycles[lTypeIndex][lCycleIndex].mRinseCycleCount); TPRINT(", ");
      TPRINT((int)mConfig.mCycles[lTypeIndex][lCycleIndex].mCo2CycleCount);   TPRINT(", ");
      TPRINT((int)mConfig.mCycles[lTypeIndex][lCycleIndex].mPumpOnDuration);  TPRINT(", ");
      TPRINT((int)mConfig.mCycles[lTypeIndex][lCycleIndex].mCo2OnDuration);   TPRINT(", ");
      TPRINT((int)mConfig.mCycles[lTypeIndex][lCycleIndex].mCo2OffDuration);  TPRINT(", ");
      TPRINTLN((int)mConfig.mCycles[lTypeIndex][lCycleIndex].mPurgeDuration);
    }
  }
}
