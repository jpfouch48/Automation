#ifndef LINKED_LIST_HEADER
#define LINKED_LIST_HEADER

// ****************************************************************************
//
// ****************************************************************************
template<class T>
class Node
{
public:
  Node<T> *next;
  T        data;
};

// ****************************************************************************
//
// ****************************************************************************
template<class T>
class Iterator
{
public:
  // **************************************************************************
  //
  // **************************************************************************
  Iterator(Node<T> *aNode) : mCurrent(aNode)
  { 
  }

  // **************************************************************************
  //
  // **************************************************************************
  Iterator& operator=(Node<T> *aNode)
  {
    mCurrent = aNode;
    return *this;
  }

  // **************************************************************************
  //
  // **************************************************************************
  Iterator& operator++(int)
  {
    Iterator lIter = *this;
    ++*this;
    return lIter;
  }

  // **************************************************************************
  //
  // **************************************************************************
  int operator*()
  {
    return mCurrent->data;
  }

protected:

private:
  Node<T> *mCurrent;
};

// ****************************************************************************
//
// ****************************************************************************
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
    mSize(0)
  {
  }

  // **************************************************************************
  //
  // **************************************************************************
  Iterator<T> begin() 
  { 
    return Iterator<T>(mHead); 
  }

  // **************************************************************************
  //
  // **************************************************************************
  Iterator<T> end()
  {
    return Iterator<T>(NULL);
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
  void push_back(T aData)
  {
    // Create the new node
    Node<T> lTemp = new Node<T>();
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

protected:

private:
  Node<T> *mHead;
  Node<T> *mTail;
  size_t   mSize;
};

#endif