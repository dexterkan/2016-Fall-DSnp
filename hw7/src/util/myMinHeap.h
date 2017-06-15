/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d)
    {
        _data.push_back(d);
        size_t i = _data.size()-1;
        if( i != 0)
        {
            while(1)
            {
                if( i < 1 ) break;
                if( _data[i] < _data[((i-1)/2)] )
                {
                    change( i , (i-1)/2 );
                    i =  (i-1)/2;
                }
                else break;
            }
        }
    }
   void delMin()
    {
        delData(0);
    }
   void delData(size_t i)
    {
        if( _data.empty() ) return;
        else
        {
            _data[i] = _data[_data.size()-1];
            _data.pop_back();
            if( _data.empty() ) return;
            while(1)
            {
                if( i > _data.size() - 1 ) break;
                if( (i*2+2) > ( _data.size() - 1 ) )
                {
                    if( (i*2+1) > ( _data.size() - 1 ) ) break;
                    else
                    {
                        if( _data[i*2+1] < _data[i] )
                            change( i , (i*2+1) );
                        break;
                    }
                }
                else
                {
                    if( _data[i*2+1] < _data[i] || _data[i*2+2] < _data[i] )
                    {
                        if( _data[(i*2+2)] < _data[(i*2+1)] )
                        {
                            change( i , (i*2+2) );
                            i = i*2+2;
                        }
                        else
                        {
                            change( i , (i*2+1) );
                            i = i*2+1;
                        }
                    }
                    else break;
                }
            }
        }
    }

private:
   // DO NOT add or change data members
    void change( size_t i , size_t j )
    {
        Data d = _data[i];
        _data[i] = _data[j];
        _data[j] = d;
    }
    
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
