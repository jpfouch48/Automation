#ifndef KLEANER_DEFINES_H
#define KLEANER_DEFINES_H

#define KLEANER_TEST_MENU
//#define KLEANER_TEST_STATE_MENU


enum class InputSource
{
  Water     = 0,
  Cleaner   = 1,
  Sanitizer = 2,

  None      = 3
};

static InputSource cycle_input_source(InputSource aInputSource)
{
  InputSource lInputSource = InputSource::None;

  switch(aInputSource)
  {
    case InputSource::Water:     lInputSource = InputSource::Cleaner;   break;
    case InputSource::Cleaner:   lInputSource = InputSource::Sanitizer; break;
    case InputSource::Sanitizer: lInputSource = InputSource::None;      break;
    case InputSource::None:      lInputSource = InputSource::Water;     break;
  }
  
  return lInputSource;
}

enum class RecircDest
{
  Waste     = 0,
  Cleaner   = 1,
  Sanitizer = 2,

  None      = 3
};

static RecircDest cycle_recirc_desk(RecircDest aRecircDest)
{
  RecircDest lRecircDest = RecircDest::None;

  switch(aRecircDest)
  {
    case RecircDest::Waste:     lRecircDest = RecircDest::Cleaner;   break;
    case RecircDest::Cleaner:   lRecircDest = RecircDest::Sanitizer; break;
    case RecircDest::Sanitizer: lRecircDest = RecircDest::None;      break;    
    case RecircDest::None:      lRecircDest = RecircDest::Waste;     break;
  }
  
  return lRecircDest;
}

#endif