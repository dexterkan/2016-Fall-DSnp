/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
       _isSorted = false;
   } // done
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *(this); }
      iterator operator ++ (int) { iterator i = *this; _node = _node->_next; return i; }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { iterator i = *this; _node = _node->_prev; return i; }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return ( _node != i._node ); }
      bool operator == (const iterator& i) const { return ( _node == i._node ); }
      // done
   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator( _head ); }
   iterator end() const { return iterator( _head->_prev ); }
   bool empty() const { return ( _head == _head->_prev ); }
   size_t size() const {
       size_t count = 0;
       DListNode<T>* n = _head;
       while( n != _head->_prev )
       {
           count++;
           n = n->_next;
       }
       return count;
   }

   void push_back(const T& x) {
       bool check = false;
       if( empty() ) check = true;
       DListNode<T>* n = new DListNode<T>(x , _head->_prev->_prev , _head->_prev );
       _head->_prev->_prev->_next = n;
       _head->_prev->_prev = n;
       if( check ) _head = n;
       _isSorted = false;
   }
   void pop_front() {
       if( !empty() )
       {
           DListNode<T>* delnode = _head;
           _head->_prev->_next = _head->_next;
           _head->_next->_prev = _head->_prev;
           _head = _head->_next;
           delete delnode;
       }
   }
   void pop_back() {
       if( !empty() )
       {
           DListNode<T>* delnode = _head->_prev->_prev;
           delnode->_prev->_next = _head->_prev;
           _head->_prev->_prev = delnode->_prev;
           if( delnode == _head )
               _head = delnode->_prev;
           delete delnode;
       }
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
       if( !empty() )
       {
           DListNode<T>* delnode = pos._node;
           if( delnode == _head )
               _head = delnode->_next;
           delnode->_prev->_next = delnode->_next;
           delnode->_next->_prev = delnode->_prev;
           delete delnode;
           return true;
       }
       return false;
   }
   bool erase(const T& x) {
       if( empty() ) return false;
       DListNode<T>* check = _head;
       while( check != _head->_prev )
       {
           if( check->_data == x )
           {
               if( check == _head )
                   _head = check->_next;
               check->_prev->_next = check->_next;
               check->_next->_prev = check->_prev;
               delete check;
               return true;
           }
           check = check->_next;
       }
       return false;
   }

   void clear() {
       DListNode<T>* dummy = _head->_prev;
       while( _head != dummy )
       {
           DListNode<T>* delnode = _head;
           _head = _head->_next;
           delete delnode;
       }
       _head->_prev = _head->_next = dummy;
   }  // delete all nodes except for the dummy node
   // done
   void sort() const {
       if( !_isSorted)
       {
           for( iterator i = ++begin() ; i != end() ; ++i )
           {
               for( iterator j = i ; j != begin() ; --j )
               {

                   if( *j < j._node->_prev->_data )
                   {
                       T temp = *j;
                       *j = j._node->_prev->_data;
                       j._node->_prev->_data = temp;
                   }
                   else break;
               }
           }
           _isSorted = true;
       }
   }
   // done
private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions

};

#endif // DLIST_H
