#ifndef NEXTION_WRAPPER_H
#define NEXTION_WRAPPER_H

#include <SoftwareSerial.h>
#include <Arduino.h>


class NextionDataHandler
{
  public:
    virtual void nextion_touch_event(byte aPageId, byte aCompId, byte aEventType) {};
    virtual void nextion_page_event(byte aPageId) {};
};

class NextionWrapper
{
  public:
    // Interface defines
    static const byte MSG_TOUCH_EVENT  = 0x65;
    static const byte MSG_CURRENT_PAGE = 0x66;

    static const byte TOUCH_EVENT_RELEASE = 0x00;
    static const byte TOUCH_EVENT_PRESS   = 0x01;


    // ************************************************************************
    //
    // ************************************************************************
    NextionWrapper(int aStartupPageId);

    // ************************************************************************
    //
    // ************************************************************************
    void setup(NextionDataHandler *aDataHandler=NULL);

    // ************************************************************************
    //
    // ************************************************************************
    void loop();

    // ************************************************************************
    //
    // ************************************************************************
    void set_text(char *aComp, char *aValue);

    // ************************************************************************
    //
    // ************************************************************************
    void set_value(char *aComp, int aValue);

    // ************************************************************************
    //
    // ************************************************************************
    void set_page(int aPageId);   

    // ************************************************************************
    //
    // ************************************************************************
    void set_background_color(char *aComp, uint16_t aValue);

  protected:

  private:
    // TODO: REVIST THIS SIZE
    static const int MAX_BUFFER_SIZE = 32;

    // ************************************************************************
    //
    // ************************************************************************
    void end_command();

    // ************************************************************************
    //
    // ************************************************************************
    void check_for_input();

    int mStartupPageId;
    SoftwareSerial mSerial;
    int mEndCount;
    int mBufferCount;
    byte mBuffer[MAX_BUFFER_SIZE];
    NextionDataHandler *mDataHandler;
};
#endif