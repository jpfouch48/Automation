#ifndef KLEANER_MENU_ITEM_H
#define KLEANER_MENU_ITEM_H

#include <Arduino.h>

// ****************************************************************************
//
// ****************************************************************************
class KleanerMenuItem
{
public:
  // **************************************************************************
  //
  //***************************************************************************
  KleanerMenuItem(int             aId,
                  String          aTitle, 
                  KleanerMenuItem *aPrevItem, 
                  KleanerMenuItem *aNextItem);

  // Accessor functions
  String           get_title()     const { return mTitle;    }
  int              get_id()        const { return mId;       }
  KleanerMenuItem* get_next_item() const { return mNextItem; }
  KleanerMenuItem* get_prev_item() const { return mPrevItem; }

protected:


private:
  String           mTitle;
  int              mId;
  KleanerMenuItem *mNextItem;
  KleanerMenuItem *mPrevItem;
};


#endif