
#include "DisplayWrapper.h"
#include <Arduino.h>


// ****************************************************************************
// See header file for details
// ****************************************************************************
DisplayWrapper::DisplayWrapper(int aStartupPageId) : 
  mStartupPageId(aStartupPageId)
{
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::setup()
{
  set_page(mStartupPageId);
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::loop()
{
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::set_page(int aPageId)
{
  String lPage = "page " + String(aPageId); //Page
  send_command(lPage.c_str());   
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::send_command(const char* aCmd)
{
  Serial.print(aCmd);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}

