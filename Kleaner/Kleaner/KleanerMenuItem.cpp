#include "KleanerMenuItem.h"


int KleanerMenuItem::gMenuItemCount = 0;

// ****************************************************************************
// See header file for details
// ****************************************************************************
KleanerMenuItem::KleanerMenuItem(String           aTitle,
                                 KleanerMenuItem *aPrevItem, 
                                 KleanerMenuItem *aNextItem) :
  mId(gMenuItemCount++),
  mTitle(aTitle),
  mPrevItem(aPrevItem),
  mNextItem(aNextItem)
{
}

