#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

// ****************************************************************************
//
// ****************************************************************************
template<class T>
struct LinkedListNode
{
  T data;
  LinkedListNode<T> *next;
};

// ****************************************************************************
//
// ****************************************************************************
template<class T>
class LinkedList
{
public:
  LinkedList();
  ~LinkedList();

  int                      size();
  bool                     add(T);
  void                     reset();
  T                        pop();
  const *LinkedListNode<T> get_head();

protected:

private:
  int mSize;
  LinkedListNode<T> *mHead;
  LinkedListNode<T> *mTail;
};

// ****************************************************************************
//
// ****************************************************************************
template<class T>
LinkedList<T>::LinkedList()
{
  mHead = mTail = NULL;
  mSize = 0;
}

// ****************************************************************************
//
// ****************************************************************************
template<class T>
LinkedList<T>::~LinkedList()
{
  LinkedListNode<T> *lTemp = NULL;

  while(mHead != NULL)
  {
    lTemp = mHead;
    mHead = mHead->next;
    delete lTemp;
  }

  mSize = 0;
}

// ****************************************************************************
//
// ****************************************************************************
template<class T>
int LinkedList<T>::size()
{
  return mSize;
}

// ****************************************************************************
//
// ****************************************************************************
template<class T>
bool LinkedList<T>::add(T aItem)
{
  LinkedListNode<T> *lTemp = new LinkedListNode<T>();
  lTemp->data = aItem;
  lTemp->next = NULL;

  if(NULL == mHead)
  {
    mHead = mTail = lTemp;
  }
  else
  {
    mTail->next = lTemp;
    mTail = lTemp;
  }

  mSize++;
  
  return true;
}


// ****************************************************************************
//
// ****************************************************************************
template<tyclasspename T>
void LinkedList<T>::reset()
{
  LinkedListNode<T> *lTemp = NULL;

  while(mHead != NULL)
  {
    lTemp = mHead;
    mHead = mHead->next;
    delete lTemp;
  }

  mHead = mTail = NULL;
  mSize = 0;
}

// ****************************************************************************
//
// ****************************************************************************
template<typename T>
T LinkedList<T>::pop()
{
  if(mSize <= 0)
    return T();

  LinkedListNode<T> *lTemp = mHead;
  T lRet = lTemp->data;

  if(NULL != mHead->next)
    mHead = mHead->next;

  delete(lTemp);

  mSize--;

  return lRet;
}

template<typename T>
const *LinkedListNode<T> LinkedList<T>::get_head()
{
  return mHead;
}

#endif