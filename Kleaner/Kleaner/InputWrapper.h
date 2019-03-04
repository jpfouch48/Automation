#ifndef INPUT_WRAPPER_H
#define INPUT_WRAPPER_H

#include <Arduino.h>
// ****************************************************************************
//
// ****************************************************************************
class InputWrapper
{
  public:
    // Definition of callback function 
    typedef void (*InputChangedCallback)(int aCurrentState);

    // ************************************************************************
    //
    // ************************************************************************
    InputWrapper(int                  aPin, 
                 unsigned char        aInputMode, 
                 unsigned long        aDebounceDelay=50,
                 InputChangedCallback aInputChanedCallback=NULL);

    // ************************************************************************
    //
    // ************************************************************************
    void setup();

    // ************************************************************************
    //
    // ************************************************************************
    void loop();

  protected:

  private:
    int mPin;
    unsigned long mDebounceDelay;
    unsigned long mDebounceTime;
    unsigned char mInputMode;

    int mCurrentState;
    int mLastState;
    bool mSkipFirst;

    InputChangedCallback mInputChanedCallback;
};
#endif