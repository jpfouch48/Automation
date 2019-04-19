
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
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::display(int aRow, int aCol, String aTxt, bool aClear)
{
  if(aClear)
    clear(aRow);

  mLCD.at(aRow, aCol, aTxt);
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::display(int aRow, int aCol, char aChar)
{
   mLCD.at(aRow, aCol, aChar); 
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
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::clear(int aRow) 
{
  if(aRow == -1)
  {
    mLCD.empty();
  }
  else
  {
    // TODO: Find better way to clear a single line
    mLCD.at(aRow, 0, "                ");    
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
