#ifndef LINKED_LIST_HEADER
#define LINKED_LIST_HEADER

template<class T>
struct ListNode
{
  ListNode<T> *next;
  T            data;
};

template<class T>
class LinkedList
{
public:
  // **************************************************************************
  //
  // **************************************************************************
  LinkedList<T>() : 
    mHead(NULL), 
    mTail(NULL), 
    mCurrentMarker(NULL), 
    mSize(0)
  {

  }

  // **************************************************************************
  //
  // **************************************************************************
  int size() 
  {
    return mSize;
  }

  // **************************************************************************
  //
  // **************************************************************************
  void add(T aData)
  {
    // Create the new node
    ListNode<T> lTemp = new ListNode<T>;
    lTemp->data = aData;
    lTemp->next = NULL;  

    if(mHead)
    {
      mTail->next = lTemp;
      mTail = lTemp;
    }
    else
    {
      mHead = mTail = lTemp;
    }
  
    // Increment the size
    mSize++;
  }

  // **************************************************************************
  //
  // **************************************************************************
  bool reset_marker()
  {
    if(mHead == NULL)
      return false;

    mCurrentMarker = mHead;
    return true;
  }

  // **************************************************************************
  //
  // **************************************************************************
  bool next_marker()
  {
    if(mCurrentMarker->next == NULL)
      return false;

    mCurrentMarker = mCurrentMarker->next;
    return true;
  }

  // **************************************************************************
  //
  // **************************************************************************
  //T& List_Iterator<T>::operator *() 
  //{
  //}  

  // **************************************************************************
  //
  // **************************************************************************
  //T& List_Iterator<T>::operator ++() 
  //{
  //}  


protected:

private:
  ListNode<T> *mHead;
  ListNode<T> *mTail;
  ListNode<T> *mCurrentMarker;
  int          mSize;
};

#endif