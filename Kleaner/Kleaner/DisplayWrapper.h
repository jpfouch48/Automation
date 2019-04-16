#ifndef DISPLAY_WRAPPER_H
#define DISPLAY_WRAPPER_H

#include <ParallaxLCD.h>

#define DEBUG_DISPLAY_WRAPPER

class DisplayWrapper
{
  public:

    // ************************************************************************
    //
    // ************************************************************************
    DisplayWrapper(int aPin,
                   int aRowCount,
                   int aColCount);

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
    void clear(int aRow=-1);

    // ************************************************************************
    //
    // ************************************************************************
    void backlight_on(bool aOn);

    // ************************************************************************
    //
    // ************************************************************************
    void display(int aRow, int aCol, String aTxt, bool aCLear=true);

    // ************************************************************************
    //  ** Display Cenetered
    // ************************************************************************
    void display(int aRow, String aTxt, bool aClear=true);

  protected:

  private:
    ParallaxLCD mLCD;
    int mRowCount;
    int mColCount;

#if defined DEBUG_DISPLAY_WRAPPER
    void DebugDump();
    const static int gDebugRowCount = 2;
    const static int gDebugColCount = 16;
    char mDebugDisplay[gDebugRowCount][gDebugColCount];
#endif
};
#endif