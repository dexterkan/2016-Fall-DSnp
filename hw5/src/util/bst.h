/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

int count = 0;

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;
    
    BSTreeNode(const T& b, BSTreeNode<T>* p = NULL, BSTreeNode<T>* l = NULL, BSTreeNode<T>* r = NULL ):
    _data(b), _parent(p), _left(l), _right(r) {}
    
    
    T               _data;
    BSTreeNode<T>*  _parent;
    BSTreeNode<T>*  _left;
    BSTreeNode<T>*  _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   
    BSTree() {
        _dummy = new BSTreeNode<T>( T() , NULL , NULL , NULL );
        _root = NULL;
        _lowerbound = _upperbound = _dummy;
    }
    ~BSTree() { clear();}
    
   class iterator
   {
       friend class BSTree;
       
   public:
       iterator(BSTreeNode<T>* n = NULL, const BSTree<T>* t = NULL ): _node(n) , tree(t) {}
       iterator(const iterator& i) : _node(i._node) {}
       ~iterator () {}
       
       
       const T& operator * () const { return _node->_data; }
       T& operator * () { return _node->_data; }
       iterator& operator ++ () { _node = tree->successor(_node); return *(this); }
       iterator operator ++ (int) { iterator i = *this; _node = tree->successor(_node); return i; }
       iterator& operator -- () { _node = tree->predecessor(_node); return *(this); }
       iterator operator -- (int) { iterator i = *this; _node = tree->predecessor(_node); return i; }
       
       iterator& operator = (const iterator& i) { _node = i._node; return *(this); }
       
       bool operator != (const iterator& i) const { return ( _node != i._node ); }
       bool operator == (const iterator& i) const { return ( _node == i._node ); }
   private:
       BSTreeNode<T>* _node;
       const BSTree<T>* tree;
   };
    
    iterator begin() const { return iterator(_lowerbound, (this));}
    iterator end() const { return iterator(_dummy ,(this));}
    bool empty() const { return ( _root == NULL ); }
    size_t size() const {
        if( empty() ) return 0;
        else
        {
            size_t count = 0;
            for( iterator i = begin() ; i != end() ; ++i )
            {
                count++;
            }
            return count;
        }
    }
    
    void pop_front() {
        if( !empty() )
            deleteNode(_lowerbound);
    }
    void pop_back() {
        if( !empty() )
            deleteNode(_upperbound);
    }
    
    bool erase(iterator pos) {
        if( _root != NULL )
        {
            BSTreeNode<T>* d = pos._node;
            deleteNode(d);
            return true;
        }
        return false;
    }
    bool erase(const T& x) {
        if( !empty() )
        {
            for( iterator i = begin() ; i != end() ; ++i )
            {
                if( *i == x )
                {
                    deleteNode(i._node);
                    return true;
                }
            }
        }
        return false;
    }
    void insert( const T& x )
    {
        if( empty() )
        {
            BSTreeNode<T>* add = new BSTreeNode<T>( x , NULL ,NULL , NULL );
            _root = _upperbound = _lowerbound = add;
            add->_right = _dummy;
        }
        else
        {
            bool check = false;
            BSTreeNode<T>* add = new BSTreeNode<T>( x , NULL ,NULL , NULL );
            if( x <= _root->_data )
            {
                BSTreeNode<T>* now = _root;
                while( now->_left != NULL )
                {
                    if( now->_left->_data < x )
                    {
                        now->_left->_parent = add;
                        add->_left = now->_left;
                        now->_left = add;
                        add->_parent = now;
                        check = true;
                        break;
                    }
                    now = now->_left;
                }
                if( !check )
                {
                    now->_left = add;
                    add->_parent = now;
                    _lowerbound = add;
                }
            }
            else
            {
                BSTreeNode<T>* now = _root;
                while( now->_right != NULL && now->_right != _dummy )
                {
                    if( now->_right->_data >= x )
                    {
                        now->_right->_parent = add;
                        add->_right = now->_right;
                        now->_right = add;
                        add->_parent = now;
                        check = true;
                        break;
                    }
                    now = now->_right;
                }
                if( !check )
                {
                    now->_right = add;
                    add->_parent = now;
                    _upperbound = add;
                    add->_right = _dummy;
                }
            }
        }
    }
    
    void clear() {
        while( !empty() )
        {
            pop_back();
        }
    }
    BSTreeNode<T>* successor(const BSTreeNode<T>* n) const{
        if( n == _dummy || n == _upperbound )
        {
            return _dummy;
        }
        else
        {
            if( n->_right )
            {
                BSTreeNode<T>* find = n->_right;
                while( find->_left != NULL )
                {
                    find = find->_left;
                }
                return find;
            }
           else
            {
                if( n->_parent )
                {
                    if( isRight(n) )
                    {
                        BSTreeNode<T> * find = n->_parent;
                        while( isRight(find) )
                        {
                            find = find->_parent;
                        }
                        return find->_parent;
                    }
                    else return n->_parent;
                }
                else
                    return _dummy;
            }
        }
    }
    BSTreeNode<T>* predecessor (const BSTreeNode<T>* n) const{
        if( n == _dummy ) return _upperbound;
        else if( n == _lowerbound ) return _lowerbound;
        else
        {
            if( n->_left )
            {
                BSTreeNode<T>* find = n->_left;
                while( find->_right != NULL )
                {
                    find = find->_right;
                }
                return find;
            }
            else
            {
                if( n->_parent )
                {
                    if( isRight(n) )
                        return n->_parent;
                    else
                    {
                        BSTreeNode<T>* find = n->_parent;
                        while( !isRight(find) )
                        {
                            find = find->_parent;
                        }
                        return find->_parent;
                    }
                }
                else
                   return _lowerbound;
            }
        }
    }
    

    void sort() {;}
    void print() {;}
private:
    BSTreeNode<T>*  _root;
    BSTreeNode<T>*  _upperbound;
    BSTreeNode<T>*  _lowerbound;
    BSTreeNode<T>*  _dummy;

    void deleteNode(BSTreeNode<T>* d){
        if(  NodeStyle(d) == 0 ) // no child
        {
            if( d == _root )
            {
                _root = NULL;
                _upperbound = _lowerbound =  _dummy;
                delete d;
            }
            else
            {
                if( isRight(d) )
                {
                    if( d == _upperbound )
                    {
                        _upperbound = d->_parent;
                        d->_parent->_right = _dummy;
                    }
                    else
                        d->_parent->_right = NULL;
                    delete d;
                }
                
                else
                {
                    if( d == _lowerbound )
                    {
                        _lowerbound = d->_parent;
                        _lowerbound->_left = NULL;
                    }
                    else
                        d->_parent->_left = NULL;
                    delete d;
                }
            }
        }
        //================================================//
        else if( NodeStyle(d) == 1 )  // one child
        {
            if( d == _root )
            {
                if( d->_right != NULL && d->_right != _dummy ) // 有右手
                {
                    _root = d->_right;
                    d->_right->_parent = NULL;
                    BSTreeNode<T>* find = d->_right;
                    while( find->_left != NULL )
                    {
                        find = find->_left;
                    }
                    _lowerbound = find;
                    delete d;
                }
                
                else // 有左手
                {
                    _root = d->_left;
                    d->_left->_parent = NULL;
                    BSTreeNode<T>* find = d->_left;
                    while( find->_right != NULL )
                    {
                        find = find->_right;
                    }
                    _upperbound = find;
                    find->_right = _dummy;
                    delete d;
                }
            }
            
            else
            {
                if( isRight(d) ) // 在右邊
                {
                    if( (d->_right != NULL) && (d->_right != _dummy) ) // 有右手
                    {
                        d->_parent->_right = d->_right;
                        d->_right->_parent = d->_parent;
                        delete d;
                    }
                        
                    else  // 有左手
                    {
                        if( d == _upperbound )  // 是最大
                        {
                            BSTreeNode<T>* find = d->_left;
                            while( find->_right != NULL )
                            {
                                find = find->_right;
                            }
                            _upperbound = find;
                            find->_right = _dummy;
                            
                            d->_parent->_right = d->_left;
                            d->_left->_parent = d->_parent;
                            delete d;
                        }
                        
                        else  // 不是最大
                        {
                            d->_parent->_right = d->_left;
                            d->_left->_parent = d->_parent;
                            delete d;
                        }
                    }
                }
                
                else // 在左邊
                {
                    if( d->_left ) // 有左手
                    {
                        d->_parent->_left = d->_left;
                        d->_left->_parent = d->_parent;
                        delete d;
                    }
                    
                    else  // 有右手
                    {
                        if( d == _lowerbound ) // 是最小
                        {
                            BSTreeNode<T>* find = d->_right;
                            while( find->_left != NULL )
                            {
                                find = find->_left;
                            }
                            _lowerbound = find;
                            find->_left = NULL;
                            
                            d->_parent->_left = d->_right;
                            d->_right->_parent = d->_parent;
                            delete d;
                        }
                        
                        else  // 不是最小
                        {
                            d->_parent->_left = d->_right;
                            d->_right->_parent = d->_parent;
                            delete d;
                        }
                    }
                    
                }
            }
        }
        //======================================================//
        else // two children
        {
            BSTreeNode<T>* s = successor(d);
            if( d == _root )  // root
            {
                if( NodeStyle(s) == 0 ) // S 沒有手
                {
                    if( s->_parent == d )
                    {
                        s->_parent = NULL;
                        s->_left = d->_left;
                        d->_left->_parent = s;
                        if( s == _upperbound ) s->_right = _dummy;
                        else s->_right = NULL;
                    }
                    
                    else
                    {
                        if( isRight(s) ) s->_parent->_right = NULL;
                        else s->_parent->_left = NULL;
                        s->_parent = NULL;
                        s->_right = d->_right;
                        s->_left = d->_left;
                        d->_left->_parent = s;
                        d->_right->_parent = s;
                    }
                    _root = s;
                    delete d;
                }
                
                else   // s 有手
                {
                    if( s->_parent == d )
                    {
                        s->_parent = NULL;
                        s->_left = d->_left;
                        d->_left->_parent = s;
                    }
                    
                    else
                    {
                        if( isRight(s) )
                        {
                            s->_parent->_right = s->_right;
                            s->_right->_parent = s->_parent;
                        }
                        else
                        {
                            s->_parent->_left = s->_right;
                            s->_right->_parent = s->_parent;
                        }
                        s->_parent = NULL;
                        s->_right = d->_right;
                        s->_left = d->_left;
                        d->_left->_parent = s;
                        d->_right->_parent = s;
                    }
                    _root = s;
                    delete d;
                }
            }
            
            else  // not root
            {
                if( NodeStyle(s) == 0 ) // 沒有手
                {
                    if( s->_parent == d )
                    {
                        s->_parent = d->_parent;
                        s->_left = d->_left;
                        d->_left->_parent = s;
                        if( isRight(d) ) d->_parent->_right = s;
                        else d->_parent->_left = s;
                    }
                    
                    else
                    {
                        if( isRight(s) ) s->_parent->_right = NULL;
                        else s->_parent->_left = NULL;
                        s->_parent = d->_parent;
                        s->_left = d->_left;
                        s->_right = d->_right;
                        d->_left->_parent = s;
                        d->_right->_parent = s;
                        if( isRight(d) ) d->_parent->_right = s;
                        else d->_parent->_left = s;
                    }
                    delete d;
                }
                    
                else  // 有手
                {
                    if( s->_parent == d )
                    {
                        if( isRight(d) ) d->_parent->_right = s;
                        else d->_parent->_left = s;
                        s->_parent = d->_parent;
                        s->_left = d->_left;
                        d->_left->_parent = s;
                    }
                    
                    else
                    {
                        if( isRight(s) )
                        {
                            s->_parent->_right = s->_right;
                            s->_right->_parent = s->_parent;
                        }
                        else
                        {
                            s->_parent->_left = s->_right;
                            s->_right->_parent = s->_parent;
                        }
                        if( isRight(d) ) d->_parent->_right = s;
                        else d->_parent->_left = s;
                        s->_parent = d->_parent;
                        s->_left = d->_left;
                        s->_right = d->_right;
                        d->_left->_parent = s;
                        d->_right->_parent = s;
                    }
                    delete d;
                }
            }
        }
    }
    
    int NodeStyle(const BSTreeNode<T>* n)
    {
        int count = 0;
        if( n->_left != NULL ) count++;
        if( n-> _right != NULL && ( n-> _right != _dummy ) ) count++;
        return count;
    }
    bool isRight(const BSTreeNode<T>* n) const{ return (n->_parent->_right == n)? true : false; }
    
};

#endif // BST_H
