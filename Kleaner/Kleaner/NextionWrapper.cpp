
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
void NextionWrapper::setup(NextionDataHandler *aDataHandler)
{      
  mDataHandler = aDataHandler;
  mSerial.begin(115200);
  delay(1000);

 // mSerial.print("baud=115200");
 // end_command();
 // delay(1000);
 // mSerial.begin(115200);

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
void NextionWrapper::set_text(char *aComp, char *aValue)
{
  TPRINT(aComp);
  TPRINT(".txt=\"");
  TPRINT(aValue);
  TPRINTLN("\"");

  mSerial.print(aComp);
  mSerial.print(".txt=\"");
  mSerial.print(aValue);
  mSerial.print("\"");
  end_command();   
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::set_value(char *aComp, int aValue)
{
  TPRINT(aComp);
  TPRINT(".val=");
  TPRINTLN(aValue);

  mSerial.print(aComp);
  mSerial.print(".val=");
  mSerial.print(aValue);
  end_command();     
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::set_page(int aPageId)
{
  TPRINT("page ");
  TPRINTLN(aPageId);  

  mSerial.print("page ");
  mSerial.print(aPageId);
  end_command();   
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::set_background_color(char *aComp, uint16_t aValue)
{
  TPRINT(aComp);
  TPRINT(".bco=");
  TPRINTLN(aValue);

  mSerial.print(aComp);
  mSerial.print(".bco=");
  mSerial.print(aValue);
  end_command();     
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::end_command()
{
  mSerial.write(0xFF);
  mSerial.write(0xFF);
  mSerial.write(0xFF);
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::check_for_input()
{
  while (mSerial.available()) 
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

    // TODO: Need to handle overflow of buffer
    mBuffer[mBufferCount] = inData;
    mBufferCount++;

    if(true == foundEnd)
    {
      // Process the command
      if(NULL != mDataHandler)
      {
        if(mBuffer[0] == MSG_TOUCH_EVENT)
        {
          mDataHandler->nextion_touch_event(mBuffer[1], mBuffer[2], mBuffer[3]);
        }
        else if(mBuffer[0] == MSG_CURRENT_PAGE)
        {
          mDataHandler->nextion_page_event(mBuffer[1]);
        }
      }

      mEndCount = 0;
      mBufferCount = 0;
      memset(mBuffer, 0, sizeof(byte) * MAX_BUFFER_SIZE);
    }
  }  
}

