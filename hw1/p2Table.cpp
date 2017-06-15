#include "p2Table.h"
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <stdlib.h>


using namespace std;
int colNum = 0;

// Implement member functions of class Row and Table here
bool Table::read(const string& csvFile)
{
    
    char character;
    
    fstream inClientFile;
    inClientFile.open( csvFile.c_str() , ios::in );
    
    if ( inClientFile )
    {
        
        while( inClientFile.get( character ) )
        {
            if( character == '\r' )
                break;
            if( character == ',' )
                colNum++;
        }
        
        inClientFile.close();
        inClientFile.clear();
        
        inClientFile.open( csvFile.c_str() , ios::in );
        string str;
        Row row;
        while( getline( inClientFile , str , '\r' ) )
        {
            if( !str.empty() )
            {
                row.newdata( colNum+1 );
                istringstream strstream( str );
                for( int i = 0 ; i < ( colNum+1 ) ; i++ )
                {
                    string rowelement;
                    getline( strstream , rowelement , ',' );
                    if( !rowelement.empty() )
                    {
                        const char* ch = rowelement.c_str();
                        int number = atoi( ch );
                        row[ i ] = number;
                    }
                    else row[i] = -100;
                }
                _rows.push_back( row );
            }
            else break;
            
        }
        return true;
    }
    
    else
        return false;
    
}


void Table::print()
{
    for( int i = 0 ; i < _rows.size() ; i++ )
    {
        for( int j = 0 ; j < ( colNum + 1 ) ; j++ )
        {
            if( _rows[i][j] == -100 )
                cout<<"    ";
            else cout<<setw(4)<<right<<_rows[i][j];
        }
        cout<<endl;
    }

}

void Table::sum( string& SUM )
{
    const char* ch = SUM.c_str();
    int col = atoi( ch );
    int sum = 0;
    for( int i = 0 ; i < _rows.size() ; i++ )
    {
        if( _rows[i][col] != -100 )
            sum += _rows[i][col];
        
    }
    
    cout<<"The summation of data in column #"<<col<<" is "<<sum<<"."<<endl;
}

void Table::ave( string& AVE )
{
    const char* ch = AVE.c_str();
    int col = atoi( ch );
    double sum = 0 , count = 0;
    double ave = 0;
    for( int i = 0 ; i < _rows.size() ; i++ )
    {
        if( _rows[i][col] != -100 )
        {
            sum += _rows[i][col];
            count++;
        }
    }
    ave = sum / count;
    cout<<"The average of data in column #"<<col<<" is "<<fixed<<setprecision(1)<<ave<<"."<<endl;
}

void Table::max( string& MAX )
{
    const char* ch = MAX.c_str();
    int col = atoi( ch );
    int max = -100 ;
    for( int i = 0 ; i < _rows.size() ; i++ )
    {
        if( _rows[i][col] != -100 )
        {
            if( max < _rows[i][col] )
                max = _rows[i][col];
        }
    }
    cout<<"The maximum of data in column #"<<col<<" is "<<max<<"."<<endl;
}

void Table::min( string& MIN)
{
    const char* ch = MIN.c_str();
    int col = atoi( ch );
    int min = 101 ;
    for( int i = 0 ; i < _rows.size() ; i++ )
    {
        if( _rows[i][col] != -100 )
        {
            if( min > _rows[i][col] )
                min = _rows[i][col];
        }
    }
    cout<<"The minimum of data in column #"<<col<<" is "<<min<<"."<<endl;
    
}

void Table::count( string& COUNT )
{
    const char* ch = COUNT.c_str();
    int col = atoi( ch );
    size_t count = 0;
    vector<int> v;
    v.push_back( -100 );
    
    for( int i = 0 ; i < _rows.size() ; i++ )
    {
        int check = 1;
        for( int j = 0 ; j < v.size() ; j++ )
        {
            if( _rows[i][col] == v[j] )
            {
                check = 0;
            }
        }
        if( check == 1 )
        {
            v.push_back( _rows[i][col] );
        }
    }
    count = v.size() - 1;
    cout<<"The distinct count of data in column #"<<col<<" is "<<count<<"."<<endl;
}

void Table::add( string& ADD )
{
    if( !ADD.empty() )
    {
        Row row;
        row.newdata( colNum+1 );
        istringstream ADDstream( ADD );
        for( int i = 0 ; i < ( colNum+1 ) ; i++ )
        {
            string rowelement;
            getline( ADDstream , rowelement , ' ' );
            if( rowelement == "-" )
                row[ i ] = -100;
            else
            {
                const char * ch = rowelement.c_str();
                int number = atoi( ch );
                row[ i ] = number;
            }
        }
        _rows.push_back( row );
        
    }
    
}
