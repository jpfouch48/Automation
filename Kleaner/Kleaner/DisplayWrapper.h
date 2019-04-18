#ifndef DISPLAY_WRAPPER_H
#define DISPLAY_WRAPPER_H

#include <ParallaxLCD.h>
#include "KleanerConfig.h"

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
    //
    // ************************************************************************
    void display(int aRow, int aCol, char aChar);

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
    const static int gDebugRowCount = LCD_ROW_COUNT;
    const static int gDebugColCount = LCD_COL_COUNT;
    char mDebugDisplay[gDebugRowCount][gDebugColCount];
#endif
};
#endif