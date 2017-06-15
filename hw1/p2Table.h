#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdlib.h>


using namespace std;

class Row
{
public:
    const int operator[] (size_t i) const { return _data[ i-1 ]; } // TODO
    int& operator[] (size_t i) { return _data[ i-1 ]; } // TODO
    void newdata( int x ) { _data=new int [x]; }
    
private:
    int  *_data;
};

class Table
{
public:
    const Row& operator[] (size_t i) const { return _rows[ i-1 ]; }
    Row& operator[] (size_t i) { return _rows[ i-1 ]; }
    
    bool read(const string&);
    
    void print();
    void sum( string& );
    void ave( string& );
    void max( string& );
    void min( string& );
    void count( string& );
    void add( string& );
    
    
private:
    vector<Row>  _rows;
};

#endif // P2_TABLE_H
