#ifndef NEXTION_WRAPPER_H
#define NEXTION_WRAPPER_H

#include <SoftwareSerial.h>
#include <Arduino.h>


class NextionDataHandler
{
  public:
    virtual void IncomingData(byte* mData, int mDataSize)
    {
        // Override this funtion in implementing class
    };
};

class NextionWrapper
{
  public:
    // ************************************************************************
    //
    // ************************************************************************
    NextionWrapper(int aStartupPageId);

    // ************************************************************************
    //
    // ************************************************************************
    void setup();

    // ************************************************************************
    //
    // ************************************************************************
    void loop();

    // ************************************************************************
    //
    // ************************************************************************
    void set_page(int aPageId);   

  protected:

  private:
    // TODO: REVIST THIS SIZE
    static const int MAX_BUFFER_SIZE = 254;

    // ************************************************************************
    //
    // ************************************************************************
    void send_command(const char* aCmd);

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