#include "KleanerMenuItem.h"

// ****************************************************************************
// See header file for details
// ****************************************************************************
KleanerMenuItem::KleanerMenuItem(int              aId,
                                 String           aTitle,
                                 KleanerMenuItem *aPrevItem, 
                                 KleanerMenuItem *aNextItem) :
  mTitle(aTitle),
  mId(aId),
  mPrevItem(aPrevItem),
  mNextItem(aNextItem)
{
}

