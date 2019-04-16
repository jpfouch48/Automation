
#include "DisplayWrapper.h"


// ****************************************************************************
// See header file for details
// ****************************************************************************
DisplayWrapper::DisplayWrapper(int aPin,
                               int aRowCount,
                               int aColCount) :
  mRowCount(aRowCount),
  mColCount(aColCount),
  mLCD(aPin, mRowCount, mColCount)
{
#if defined DEBUG_DISPLAY_WRAPPER
    for(int lIndex=0;lIndex < gDebugRowCount; lIndex++)
      strcpy(mDebugDisplay[lIndex], "                ");
#endif  
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::display(int aRow, int aCol, String aTxt, bool aClear)
{
  if(aClear)
    clear(aRow);

  mLCD.at(aRow, aCol, aTxt);

#if defined DEBUG_DISPLAY_WRAPPER
  strcpy(&mDebugDisplay[aRow][aCol], aTxt.c_str());
  DebugDump();
#endif  

}
// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::display(int aRow, String aTxt, bool aClear)
{
  int lCol = 0;

  if(aClear)
    clear(aRow);

  lCol = (mColCount - aTxt.length())/2;

  mLCD.at(aRow, lCol, aTxt);

#if defined DEBUG_DISPLAY_WRAPPER
  strcpy(&mDebugDisplay[aRow][lCol], aTxt.c_str());
  DebugDump();
#endif    
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::clear(int aRow) 
{
  if(aRow == -1)
  {
    mLCD.empty();

#if defined DEBUG_DISPLAY_WRAPPER
    for(int lIndex=0;lIndex < gDebugRowCount; lIndex++)
      strcpy(mDebugDisplay[lIndex], "                ");
    DebugDump();
#endif      
  }
  else
  {
    // TODO: Find better way to clear a single line
    mLCD.at(aRow, 0, "                ");

#if defined DEBUG_DISPLAY_WRAPPER
    strcpy(mDebugDisplay[aRow], "                ");
    DebugDump();
#endif      
    
  }
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::backlight_on(bool aOn)
{
  if(true == aOn)
    mLCD.backLightOn();
  else
    mLCD.backLightOff();

}


// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::setup()
{
  mLCD.setup();  
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::loop()
{
}

#if defined DEBUG_DISPLAY_WRAPPER
// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::DebugDump()
{
  for(int lIndex=0;lIndex < gDebugRowCount; lIndex++)
  {
    Serial.print("[");
    Serial.print(mDebugDisplay[lIndex]);
    Serial.println("[");
  }
}
#endif      
