
#include "NextionWrapper.h"
#include "KleanerConfig.h"


// ****************************************************************************
// See header file for details
// ****************************************************************************
NextionWrapper::NextionWrapper(int aStartupPageId) : 
  mStartupPageId(aStartupPageId),
  mSerial(NEXT_SOFT_SERIAL_RX_PIN, NEXT_SOFT_SERIAL_TX_PIN),
  mEndCount(0),
  mBufferCount(0),
  mDataHandler(NULL)
{
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::setup()
{
  mSerial.begin(9600);  
  set_page(mStartupPageId);
}


// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::loop()
{
  check_for_input();
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::set_page(int aPageId)
{
  String lPage = "page " + String(aPageId); //Page
  send_command(lPage.c_str());   
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::send_command(const char* aCmd)
{
  mSerial.print(aCmd);
  mSerial.write(0xFF);
  mSerial.write(0xFF);
  mSerial.write(0xFF);
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::check_for_input()
{
  if (mSerial.available()) 
  {
    bool foundEnd = false;
    byte inData = mSerial.read();

    if(inData == 0xff)
    {
      mEndCount++;
      if(mEndCount == 3)
        foundEnd = true;
    }
    else
      mEndCount = 0;

    mBuffer[mBufferCount] = inData;
    mBufferCount++;


    if(true == foundEnd)
    {
      // Process the command
      if(NULL != mDataHandler)
      {
        mDataHandler->IncomingData(mBuffer, mBufferCount);
      }

      mEndCount = 0;
      mBufferCount = 0;
      memset(mBuffer, 0, sizeof(byte) * MAX_BUFFER_SIZE);
    }
  }  
}

