/****************************************************************************
 FileName     [ myHashSet.h ]
 PackageName  [ util ]
 Synopsis     [ Define HashSet ADT ]
 Author       [ Chung-Yang (Ric) Huang ]
 Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//


template <class Data>
class mySet
{
public:
    mySet(size_t b = 0,Data d = Data()) : _numBuckets(0), _buckets(0) , data(d) { if (b != 0) init(b); }
    ~mySet() { reset(); }
    
    // TODO: implement the HashSet<Data>::iterator
    // o An iterator should be able to go through all the valid Data
    //   in the Hash
    // o Functions to be implemented:
    //   - constructor(s), destructor
    //   - operator '*': return the HashNode
    //   - ++/--iterator, iterator++/--
    //   - operators '=', '==', !="
    //
    class iterator
    {
        friend class mySet<Data>;
        
    public:
        iterator(Data* d= NULL , const mySet<Data>* h = NULL ): _node(d) , hash(h) {}
        iterator(const iterator& i): _node(i._node) {}
        ~iterator() {}
        
        const Data& operator * () const { return *_node; }
        Data& operator * () { return (*_node); }
        iterator& operator ++ () {
            if( _node == NULL ) return *this;
            _node = hash->next(_node);
            return (*this);
        }
        iterator operator ++ (int) {
            if( _node == NULL ) return *this;
            iterator i = *this;
            _node = hash->next(_node);
            return i;
        }
        iterator& operator -- () {
            _node = hash->prev(_node);
            return (*this);
        }
        iterator operator -- (int) {
            iterator i = *this;
            _node = hash->prev(_node);
            return i;
        }
        
        iterator& operator = (const iterator& i) { _node = i._node; return (*this); }
        
        bool operator != (const iterator& i) const { return ( _node != i._node ); }
        bool operator == (const iterator& i) const { return ( _node == i._node ); }
        
    private:
        Data* _node;
        const mySet<Data>* hash;
    };
    
    void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
    void reset() {
        _numBuckets = 0;
        if (_buckets) { delete [] _buckets; _buckets = 0; }
    }
    void clear() {
        for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
    }
    size_t numBuckets() const { return _numBuckets; }
    
    vector<Data>& operator [] (size_t i) { return _buckets[i]; }
    const vector<Data>& operator [](size_t i) const { return _buckets[i]; }
    
    // TODO: implement these functions
    //
    // Point to the first valid data
    iterator begin() const {
        for( size_t i = 0 ; i < _numBuckets ; i++ )
        {
            if( !_buckets[i].empty() )
            {
                Data* dptr = &_buckets[i][0];
                return iterator(dptr,this);
            }
        }
        return iterator(); }
    // Pass the end
    iterator end() const { return iterator(); }
    // return true if no valid data
    bool empty() const { if( begin() == end() ) return true; else return false; }
    // number of valid data
    size_t size() const {
        size_t s = 0;
        for( iterator i = begin() ; i != end() ; ++i )
            s++;
        return s;
    }
    
    // check if d is in the hash...
    // if yes, return true;
    // else return false;
    Data& check(const Data& d) {
        size_t n = bucketNum(d);
        for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
            if( d == _buckets[n][i] )
            {
                return _buckets[n][i];
            }
        return data;
    }
    // return true if inserted successfully (i.e. d is not in the hash)
    // return false is d is already in the hash ==> will not insert
    void insert(const Data& d) {
        
        size_t n = bucketNum(d);
        _buckets[n].push_back(d);
    }
private:
    // Do not add any extra data member
    size_t            _numBuckets;
    vector<Data>*     _buckets;
    Data              data;
    
    size_t bucketNum(const Data& d) const {
        return (d() % _numBuckets); }
    
    Data* next( const Data* d ) const
    {
        size_t n = bucketNum(*d);
        size_t col = 0;
        for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
        {
            if( _buckets[n][i] == *d )
            {
                col = i;
                break;
            }
        }
        if( col < (_buckets[n].size()-1) )
        {
            Data* dptr = &_buckets[n][col+1];
            return dptr;
        }
        else
        {
            for( size_t i = (n+1) ; i < _numBuckets ; i++ )
            {
                if( !_buckets[i].empty() )
                {
                    Data* dptr = &_buckets[i][0];
                    return dptr;
                }
            }
        }
        return NULL;
    }
    
    Data* prev( const Data* d ) const
    {
        if( d == NULL )
        {
            Data* dptr;
            for( size_t i = 0 ; i < _numBuckets ; i++ )
            {
                if( !_buckets[i].empty())
                    for( size_t j = 0 ; j < _buckets[i].size() ; j++ )
                        dptr = &_buckets[i][j];
            }
            return dptr;
        }
        
        size_t n = bucketNum(*d);
        size_t col = 0;
        for( size_t i = 0 ; i < _numBuckets ; i++ )
        {
            if( _buckets[n][i] == *d )
            {
                col = i;
                break;
            }
        }
        if( col > 0 )
        {
            Data* dptr = &_buckets[n][col-1];
            return dptr;
        }
        else
        {
            for( int i = (n-1) ; i >= 0 ; i-- )
            {
                if( !_buckets[i].empty() )
                {
                    Data* dptr = &_buckets[i][_buckets[i].size()-1];
                    return dptr;
                }
            }
        }
        return d;
    }
};



template <class Data>
class HashSet
{
public:
    HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
    ~HashSet() { reset(); }
    
    // TODO: implement the HashSet<Data>::iterator
    // o An iterator should be able to go through all the valid Data
    //   in the Hash
    // o Functions to be implemented:
    //   - constructor(s), destructor
    //   - operator '*': return the HashNode
    //   - ++/--iterator, iterator++/--
    //   - operators '=', '==', !="
    //
    class iterator
    {
        friend class HashSet<Data>;
        
    public:
        iterator(Data* d= NULL , const HashSet<Data>* h = NULL ): _node(d) , hash(h) {}
        iterator(const iterator& i): _node(i._node) {}
        ~iterator() {}
        
        const Data& operator * () const { return *_node; }
        Data& operator * () { return (*_node); }
        iterator& operator ++ () {
            if( _node == NULL ) return *this;
            _node = hash->next(_node);
            return (*this);
        }
        iterator operator ++ (int) {
            if( _node == NULL ) return *this;
            iterator i = *this;
            _node = hash->next(_node);
            return i;
        }
        iterator& operator -- () {
            _node = hash->prev(_node);
            return (*this);
        }
        iterator operator -- (int) {
            iterator i = *this;
            _node = hash->prev(_node);
            return i;
        }
        
        iterator& operator = (const iterator& i) { _node = i._node; return (*this); }
        
        bool operator != (const iterator& i) const { return ( _node != i._node ); }
        bool operator == (const iterator& i) const { return ( _node == i._node ); }
        
    private:
        Data* _node;
        const HashSet<Data>* hash;
    };
    
    void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
    void reset() {
        _numBuckets = 0;
        if (_buckets) { delete [] _buckets; _buckets = 0; }
    }
    void clear() {
        for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
    }
    size_t numBuckets() const { return _numBuckets; }
    
    vector<Data>& operator [] (size_t i) { return _buckets[i]; }
    const vector<Data>& operator [](size_t i) const { return _buckets[i]; }
    
    // TODO: implement these functions
    //
    // Point to the first valid data
    iterator begin() const {
        for( size_t i = 0 ; i < _numBuckets ; i++ )
        {
            if( !_buckets[i].empty() )
            {
                Data* dptr = &_buckets[i][0];
                return iterator(dptr,this);
            }
        }
        return iterator(); }
    // Pass the end
    iterator end() const { return iterator(); }
    // return true if no valid data
    bool empty() const { if( begin() == end() ) return true; else return false; }
    // number of valid data
    size_t size() const {
        size_t s = 0;
        for( iterator i = begin() ; i != end() ; ++i )
            s++;
        return s;
    }
    
    // check if d is in the hash...
    // if yes, return true;
    // else return false;
    bool check(const Data& d , Data& get ) const {
        size_t n = bucketNum(d);
        for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
            if( d == _buckets[n][i] )
            {
                get = _buckets[n][i];
                return true;
            }
        return false;
    }
    
    // query if d is in the hash...
    // if yes, replace d with the data in the hash and return true;
    // else return false;
    bool query(Data& d) const {
        size_t n = bucketNum(d);
        for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
            if( d == _buckets[n][i] ) { d = _buckets[n][i]; return true; }
        return false;
    }
    
    // update the entry in hash that is equal to d (i.e. == return true)
    // if found, update that entry with d and return true;
    // else insert d into hash as a new entry and return false;
    bool update(const Data& d) {
        size_t n = bucketNum(d);
        for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
            if( d == _buckets[n][i] ) { _buckets[n][i] = d; return true; }
        insert(d);
        return false;
    }
    
    // return true if inserted successfully (i.e. d is not in the hash)
    // return false is d is already in the hash ==> will not insert
    bool insert(const Data& d, Data& get) {
        if( check(d,get) ) return false;
        else
        {
            size_t n = bucketNum(d);
            _buckets[n].push_back(d);
            return true;
        }
    }
    
    // return true if removed successfully (i.e. d is in the hash)
    // return fasle otherwise (i.e. nothing is removed)
    bool remove(const Data& d) {
        if( !check(d) ) return false;
        else
        {
            size_t n = bucketNum(d);
            for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
            {
                if( d == _buckets[n][i] )
                {
                    _buckets[n].erase( _buckets[n].begin() + i );
                    return true;
                }
            }
        }
        return false;
    }
    
private:
    // Do not add any extra data member
    size_t            _numBuckets;
    vector<Data>*     _buckets;
    
    size_t bucketNum(const Data& d) const {
        return (d() % _numBuckets); }
    
    Data* next( const Data* d ) const
    {
        size_t n = bucketNum(*d);
        size_t col = 0;
        for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
        {
            if( _buckets[n][i] == *d )
            {
                col = i;
                break;
            }
        }
        if( col < (_buckets[n].size()-1) )
        {
            Data* dptr = &_buckets[n][col+1];
            return dptr;
        }
        else
        {
            for( size_t i = (n+1) ; i < _numBuckets ; i++ )
            {
                if( !_buckets[i].empty() )
                {
                    Data* dptr = &_buckets[i][0];
                    return dptr;
                }
            }
        }
        return NULL;
    }
    
    Data* prev( const Data* d ) const
    {
        if( d == NULL )
        {
            Data* dptr;
            for( size_t i = 0 ; i < _numBuckets ; i++ )
            {
                if( !_buckets[i].empty())
                    for( size_t j = 0 ; j < _buckets[i].size() ; j++ )
                        dptr = &_buckets[i][j];
            }
            return dptr;
        }
        
        size_t n = bucketNum(*d);
        size_t col = 0;
        for( size_t i = 0 ; i < _numBuckets ; i++ )
        {
            if( _buckets[n][i] == *d )
            {
                col = i;
                break;
            }
        }
        if( col > 0 )
        {
            Data* dptr = &_buckets[n][col-1];
            return dptr;
        }
        else
        {
            for( int i = (n-1) ; i >= 0 ; i-- )
            {
                if( !_buckets[i].empty() )
                {
                    Data* dptr = &_buckets[i][_buckets[i].size()-1];
                    return dptr;
                }
            }
        }
        return d;
    }
};

#endif // MY_HASH_SET_H

/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>

using namespace std;

// TODO: (Optionally) Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) const { return true; }
// 
// private:
// };
//
template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap(size_t b=0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:
       iterator(HashNode* d= NULL , const HashMap<HashKey, HashData>* h = NULL ): _node(d) , hash(h) {}
       iterator(const iterator& i): _node(i._node) {}
       ~iterator() {}
       
       const HashNode& operator * () const { return *_node; }
       HashNode& operator * () { return (*_node); }
       iterator& operator ++ () {
           if( _node == NULL ) return *this;
           _node = hash->next(_node);
           return (*this);
       }
       iterator operator ++ (int) {
           if( _node == NULL ) return *this;
           iterator i = *this;
           _node = hash->next(_node);
           return i;
       }
       iterator& operator -- () {
           _node = hash->prev(_node);
           return (*this);
       }
       iterator operator -- (int) {
           iterator i = *this;
           _node = hash->prev(_node);
           return i;
       }
       
       iterator& operator = (const iterator& i) { _node = i._node; return (*this); }
       
       bool operator != (const iterator& i) const { return ( _node != i._node ); }
       bool operator == (const iterator& i) const { return ( _node == i._node ); }

   private:
       HashNode* _node;
       const HashMap<HashKey, HashData>* hash;
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
       for( size_t i = 0 ; i < _numBuckets ; i++ )
       {
           if( !_buckets[i].empty() )
           {
               HashNode* dptr = &_buckets[i][0];
               return iterator(dptr,this);
           }
       }
       return iterator();
   }
   // Pass the end
   iterator end() const { return iterator(); }
   // return true if no valid data
   bool empty() const { if( begin() == end() ) return true; else return false; }
   // number of valid data
   size_t size() const {
       size_t s = 0;
       for( iterator i = begin() ; i != end() ; ++i )
           s++;
       return s;
   }

   // check if k is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const HashKey& k, HashData& d ) const {
       size_t n = bucketNum(k);
       for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
           if( k == _buckets[n][i].first )
               return true;
       return false;
   }

   // query if k is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(const HashKey& k, HashData& d) const {
       size_t n = bucketNum(k);
       for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
           if( k == _buckets[n][i].first ) { d = _buckets[n][i].second; return true; }
       return false;
   }

   // update the entry in hash that is equal to k (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const HashKey& k, HashData& d) {
       size_t n = bucketNum(k);
       for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
           if( k == _buckets[n][i].first ) { _buckets[n][i].second = d; return true; }
       insert(k,d);
       return false;
   }

   // return true if inserted d successfully (i.e. k is not in the hash)
   // return false is k is already in the hash ==> will not insert
   bool insert(const HashKey& k, const HashData& d) {
       if( check(k) ) return false;
       else
       {
           size_t n = bucketNum(k);
           _buckets[n].push_back(HashNode(k,d));
           return true;
       }
   }
   
    void pureinsert(const HashKey& k, const HashData& d){
        size_t n = bucketNum(k);
        _buckets[n].push_back(HashNode(k,d));
    }

   // return true if removed successfully (i.e. k is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const HashKey& k) {
       if( !check(k) ) return false;
       else
       {
           size_t n = bucketNum(k);
           for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
           {
               if( k == _buckets[n][i].first )
               {
                   _buckets[n].erase( _buckets[n].begin() + i );
                   return true;
               }
           }
       }
       return false;
   }
private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }
    
    HashNode* next( const HashNode* d ) const
    {
        size_t n = bucketNum(*d);
        size_t col = 0;
        for( size_t i = 0 ; i < _buckets[n].size() ; i++ )
        {
            if( _buckets[n][i] == *d )
            {
                col = i;
                break;
            }
        }
        if( col < (_buckets[n].size()-1) )
        {
            HashNode* dptr = &_buckets[n][col+1];
            return dptr;
        }
        else
        {
            for( size_t i = (n+1) ; i < _numBuckets ; i++ )
            {
                if( !_buckets[i].empty() )
                {
                    HashNode* dptr = &_buckets[i][0];
                    return dptr;
                }
            }
        }
        return NULL;
    }
    
    HashNode* prev( const HashNode* d ) const
    {
        if( d == NULL )
        {
            HashNode* dptr;
            for( size_t i = 0 ; i < _numBuckets ; i++ )
            {
                if( !_buckets[i].empty())
                    for( size_t j = 0 ; j < _buckets[i].size() ; j++ )
                        dptr = &_buckets[i][j];
            }
            return dptr;
        }
        
        size_t n = bucketNum(*d);
        size_t col = 0;
        for( size_t i = 0 ; i < _numBuckets ; i++ )
        {
            if( _buckets[n][i] == *d )
            {
                col = i;
                break;
            }
        }
        if( col > 0 )
        {
            HashNode* dptr = &_buckets[n][col-1];
            return dptr;
        }
        else
        {
            for( int i = (n-1) ; i >= 0 ; i-- )
            {
                if( !_buckets[i].empty() )
                {
                    HashNode* dptr = &_buckets[i][_buckets[i].size()-1];
                    return dptr;
                }
            }
        }
        return d;
    }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { reset(); _size = s; _cache = new CacheNode[s]; }
   void reset() {  _size = 0; if (_cache) { delete [] _cache; _cache = 0; } }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const {
      size_t i = k() % _size;
      if (k == _cache[i].first) {
         d = _cache[i].second;
         return true;
      }
      return false;
   }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) {
      size_t i = k() % _size;
      _cache[i].first = k;
      _cache[i].second = d;
   }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H


