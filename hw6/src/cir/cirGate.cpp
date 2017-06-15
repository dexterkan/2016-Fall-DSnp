/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"
#include <sstream>
#include <string>

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
    string str( 50 , '=' );
    cout<<str<<endl;
    stringstream strstream;
    strstream<<"= "<<getTypeStr()<<"("<<ID<<")";
    if( IO_Name != "")
        strstream<<"\""<<IO_Name<<"\"";
    strstream<<", line "<<line;
    cout<<strstream.str();
    cout<<string( (50 - 1 - strstream.str().length()) , ' ' )<<"="<<endl;
    cout<<str<<endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   vector<const CirGate*> done;
   report( level , 0 , true , false , &done );
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
    vector<const CirGate*> done;
    report( level , 0 , false , false , &done );
}

void CirGate::report( int level1 , int blank , bool fanin , bool inverse , vector<const CirGate*>* done ) const
{
    cout<<string( blank , ' ' )<<(inverse ? "!" : "" )<<getTypeStr()<<" "<<ID;
    bool check = false;
    for( vector<const CirGate*>::const_iterator it = done->begin() ; it != done->end() ; ++it )
        if( this == *it ) { check = true ; break; }
    if( fanin )
    {
        if( getTypeStr() == "PI" )
            level1 = 0;
    }
    else
    {
        if( getTypeStr() == "PO" )
            level1 = 0;
    }
    if( check && level1 > 0 )
    {
        cout<<" (*)";
        cout<<endl;
        return;
    }
    if( !check && level1 > 0 ) done->push_back(this);
    cout<<endl;
    level1--;
    blank += 2;
    if( level1 >= 0 )
    {
        if( fanin )
        {
            for( size_t i = 0 ; i < Fanin.size() ; ++i )
            {
                if( Fanin[i].gate()->getTypeStr() != "UNDEF" && Fanin[i].gate() != NULL )
                    Fanin[i].gate()->report( level1 , blank , true , Fanin[i].isInv() , done );
                else if( Fanin[i].gate()->getTypeStr() == "UNDEF" )
                    cout<<string( blank , ' ' )<<( Fanin[i].isInv() ? "!" : "" )<<"UNDEF "<<Fanin[i].gate()->getVar_ID()<<endl;
            }
        }
        else
        {
            for( size_t i = 0 ; i < Fanout.size() ; ++i )
            {
                if( Fanout[i].gate()->getTypeStr() != "UNDEF" && Fanout[i].gate() != NULL )
                    Fanout[i].gate()->report( level1 , blank , false , Fanout[i].isInv() , done );
                else if( Fanout[i].gate()->getTypeStr() == "UNDEF" )
                    cout<<string( blank , ' ' )<<( Fanout[i].isInv() ? "!" : "" )<<"UNDEF "<<Fanout[i].gate()->getVar_ID()<<endl;
            }
        }
    }
    
}

string CirGate::getTypeStr() const
{
    string str;
    switch( type )
    {
        case UNDEF_GATE:
            str = "UNDEF";
            break;
            
        case PI_GATE:
            str = "PI";
            break;
            
        case PO_GATE:
            str = "PO";
            break;
            
        case AIG_GATE:
            str = "AIG";
            break;
            
        case CONST_GATE:
            str = "CONST";
            break;
            
        default:
            str = "";
            break;
    }
    
    return str;
}


