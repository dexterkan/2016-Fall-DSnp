/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <set>
#include <algorithm>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
    for( size_t i = 0 ; i < r.size() ; i++ )
    {
        if( r[i] == INT_MAX )
        {
            os<<".";
        }
        
        else
        {
            os<<r[i];
        }
        if( i != r.size() - 1 )
            cout<<" ";
    }
    cout<<endl;
    return os;

}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
    for( size_t i = 0 ; i < t.nRows() ; i++ )
    {
        for( size_t j = 0 ; j < t.nCols() ; j++ )
        {
            if( t[i][j] == INT_MAX )
            {
                os<<setw(6)<<right<<".";
            }
            else
            {
                os<<setw(6)<<right<<t[i][j];
            }
        }
        cout<<endl;
    }
   return os;
}

ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
    string str;
    string rowelement = "";
    while( getline( ifs , str , '\r' ) )
    {
        if( !str.empty() )
        {
            vector<int> v;
            for( int i = 0 ; i < str.length() ; i++ )
            {
                if( str[i] != ',' )
                {
                    rowelement += str[i];
                }
                
                else
                {
                    if( rowelement == "" )
                        v.push_back(INT_MAX);
                    else
                    {
                        const char* ch = rowelement.c_str();
                        v.push_back( atoi( ch ) );
                        rowelement = "";
                    }
                }
            }
            if( rowelement == "" )
                v.push_back(INT_MAX);
            else
            {
                const char* ch = rowelement.c_str();
                v.push_back( atoi( ch ) );
                rowelement ="";
            }
            
            DBRow r( v );
            t.addRow( r );
            r.reset();
        }
        else break;

    }

   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
    _data.erase( _data.begin() + c );
}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
    for( size_t i = 0 ; i < _sortOrder.size() ; i++ )
    {
        if( r1[ _sortOrder[i] ] > r2[ _sortOrder[i] ] )
        {
            return false;
        }
        
        else if( r1[ _sortOrder[i] ] < r2[ _sortOrder[i] ] )
        {
            return true;
        }
    }

    return true;

}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
    vector<DBRow> tmp;
    tmp.swap( _table );
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
    if( d.size() <= _table.size() )
    {
        for( size_t i = 0 ; i < d.size() ; i++ )
        {
            _table[i].addData( d[i] );
        }
        for( size_t i = 0 ; i < ( _table.size() - d.size() ) ; i++ )
        {
            _table[i].addData( INT_MAX );
        }
    }
    else
        for( size_t i = 0 ; i < _table.size() ; i++ )
        {
            _table[i].addData( d[i] );
        }
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
    _table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
    bool check = false;
    for( size_t i = 0 ; i < _table.size() ; i++ )
    {
        if( _table[i][c] != INT_MAX )
        {
            check = true;
            break;
        }
    }
    
    if( !check )
        return NAN;
    
    float max = INT_MIN;
    for( size_t i = 0 ; i < _table.size() ; i++ )
    {
        if( max < _table[i][c] && _table[i][c] != INT_MAX )
        {
            max = _table[i][c];
        }
    }
    
    return max;

}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
    bool check = false;
    for( size_t i = 0 ; i < _table.size() ; i++ )
    {
        if( _table[i][c] != INT_MAX )
        {
            check = true;
            break;
        }
    }
    
    if( !check )
        return NAN;
    
    float min = INT_MAX;
    for( size_t i = 0 ; i < _table.size() ; i++ )
    {
        if( min > _table[i][c] )
        {
            min = _table[i][c];
        }
    }
    
    return min;
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
    bool check = false;
    for( size_t i = 0 ; i < _table.size() ; i++ )
    {
        if( _table[i][c] != INT_MAX )
        {
            check = true;
            break;
        }
    }
    
    if( !check )
        return NAN;
    
    float sum = 0.0;
    for( size_t i = 0 ; i < _table.size() ; i++ )
    {
        if( _table[i][c] != INT_MAX )
        {
            sum += _table[i][c];
        }
    }
    
    return sum;

}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
    int count = 0;
    vector<int> v;
    v.push_back( INT_MAX );
    for( size_t i = 0 ; i < _table.size() ; i++ )
    {
        bool check = true;
        for( size_t j = 0 ; j < v.size() ; j++ )
        {
            if( _table[i][c] == v[j] )
            {
                check = false;
            }
        }
        
        if( check )
        {
            v.push_back( _table[i][c] );
        }
    }
    
    count = v.size() - 1;
    return count;

}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
    float num = 0.0;
    float sum = 0.0;
    for( size_t i = 0 ; i < _table.size() ; i++ )
    {
        if( _table[i][c] != INT_MAX )
        {
            num++;
            sum += _table[i][c];
        }
    }
    
    if( num == 0.0 )
        return NAN;
    
    return sum / num;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
    std::sort(_table.begin(),_table.end(),s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
    for(int i=0;i<_table.size();i++)
    {
        if(_table[i][c]==INT_MAX)
            cout<<".";
        else
            cout<<_table[i][c];
        if( i != _table.size() - 1 )
            cout<<" ";
    }
    cout<<endl;
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

