
#include "DisplayWrapper.h"
#include "KleanerConfig.h"
#include <Arduino.h>


// ****************************************************************************
// See header file for details
// ****************************************************************************
DisplayWrapper::DisplayWrapper(int aStartupPageId) : 
  mStartupPageId(aStartupPageId),
  mSerial(NEXT_SOFT_SERIAL_RX_PIN, NEXT_SOFT_SERIAL_TX_PIN)
{
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::setup()
{
  mSerial.begin(9600);  
  set_page(mStartupPageId);
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::loop()
{
  check_for_input();
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
  mSerial.print(aCmd);
  mSerial.write(0xFF);
  mSerial.write(0xFF);
  mSerial.write(0xFF);
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void DisplayWrapper::check_for_input()
{
  if (mSerial.available()) 
  {
    int incomingByte = mSerial.read();
    Serial.println(incomingByte, HEX);
  }  
}

