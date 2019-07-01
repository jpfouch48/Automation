
#include "NextionWrapper.h"
#include "KleanerConfig.h"


// ****************************************************************************
// See header file for details
// ****************************************************************************
NextionWrapper::NextionWrapper(int aStartupPageId) : 
  mStartupPageId(aStartupPageId),
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
  Serial1.begin(9600);
  delay(1000);
  Serial1.print("baud=115200");
  end_command();
  delay(1000);
  Serial1.begin(115200);

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

  Serial1.print(aComp);
  Serial1.print(".txt=\"");
  Serial1.print(aValue);
  Serial1.print("\"");
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

  Serial1.print(aComp);
  Serial1.print(".val=");
  Serial1.print(aValue);
  end_command();     
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::set_page(int aPageId)
{
  TPRINT("page ");
  TPRINTLN(aPageId);  

  Serial1.print("page ");
  Serial1.print(aPageId);
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

  Serial1.print(aComp);
  Serial1.print(".bco=");
  Serial1.print(aValue);
  end_command();     
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::end_command()
{
  Serial1.write(0xFF);
  Serial1.write(0xFF);
  Serial1.write(0xFF);
}

// ****************************************************************************
// See header file for details
// ****************************************************************************
void NextionWrapper::check_for_input()
{
  while (Serial1.available()) 
  {
    bool foundEnd = false;
    byte inData = Serial1.read();

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

