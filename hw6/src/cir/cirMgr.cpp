/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirMgr::~CirMgr()
{
    for( int i = 0 ; i < GATE.size() ; i++ )
        delete GATE[i];
}
bool
CirMgr::readCircuit(const string& fileName)
{
    fstream file;
    file.open( fileName.c_str() , ios::in );
    if( !file ) { cerr<<"Cannot open design \""<<fileName<<"\"!!"<<endl; return false; }
    else
    {
        vector<string> str_all , h_element;
        string str;
        while( getline( file , str ) )
        {
            if( !str.empty() && str != "c" )
            {
                str_all.push_back(str);
            }
            else break;
        }
/*=====================header begin===============================*/
        lineNo = 1;
        istringstream strstream( str_all[0] );
        str = "";
        while( getline( strstream , str , ' ' ) )
        {
            if( !str.empty() ) h_element.push_back(str);
            else break;
        }
        int num = 0;
        myStr2Int( h_element[1] , num ); M = (unsigned int)num;
        myStr2Int( h_element[2] , num ); I = (unsigned int)num;
        myStr2Int( h_element[3] , num ); L = (unsigned int)num;
        myStr2Int( h_element[4] , num ); O = (unsigned int)num;
        myStr2Int( h_element[5] , num ); A = (unsigned int)num;
/*=====================header end=================================*/
        for( unsigned int i = 0 ; i < M+1+O ; i++ )
        {
            CirGate* g = new CirGate( 0 , i );
            GATE.push_back(g);
        }
        cout<<GATE.size()<<endl;
/*=====================input begin================================*/
        GATE[0] = new CirGate( 0 , 0 , CONST_GATE );
        for( unsigned int i = 0 ; i < I ; i++ )
        {
            lineNo++;
            int _id;
            myStr2Int( str_all[lineNo-1] , _id );
            CirGate* gate = new CirGate( lineNo , _id/2 , PI_GATE );
            PI.push_back(gate);
            GATE[_id/2] = gate;
        }
/*=====================input end==================================*/

/*=====================lauch begin================================*/
        // do nothing
/*=====================lauch end==================================*/

/*=====================output begin===============================*/
        for( unsigned int i = 0 ; i < O ; i++ )
        {
            lineNo++;
            CirGate* gate = new CirGate( lineNo , (M + 1 + i) , PO_GATE );
            PO.push_back(gate);
            GATE[M + 1 + i] = gate;
        }
/*=====================output end=================================*/
        
/*=====================aig begin==================================*/
        for( unsigned int i = 0 ; i < A ; i++ )
        {
            lineNo++;
            int _id;
            string id;
            istringstream strstream1( str_all[lineNo-1] );
            getline( strstream1 , id , ' ' );
            myStr2Int( id , _id );
            CirGate* gate = new CirGate( lineNo , _id/2 , AIG_GATE );
            AIG.push_back(gate);
            GATE[_id/2] = gate;
        }
/*=====================symbol begin===============================*/
        if( str_all.size() > ( 1 + I + O + A ) )
        {
            for( unsigned int i = ( 1 + I + O + A ) ; i < str_all.size() ; i++ )
            {
                lineNo++;
                istringstream strstream1( str_all[lineNo-1] );
                string num = "";
                getline( strstream1 , num , ' ' );
                num = num.substr(1);
                int number = 0;
                myStr2Int( num , number );
                char s_type = str_all[lineNo -1][0];
                string symbol = str_all[lineNo -1].substr( str_all[lineNo -1].find_first_of(' ') + 1 );
                if( s_type == 'i' )
                {
                    PI[number]->setIO_Name(symbol);
                }
                else
                {
                    PO[number]->setIO_Name(symbol);
                }
                
            }
        }
/*=====================symbol end=================================*/
/******************************************************************/
/******************************************************************/
        for( unsigned int i = 0 ; i < O ; i++ )
        {
            string id;
            int _id;
            istringstream strstream2( str_all[1+I+i] );
            getline( strstream2 , id , ' ' );
            myStr2Int( id , _id );
                PO[i]->setFanin( GATE[_id/2] , ( _id % 2 ) );
                GATE[_id/2]->setFanout( PO[i] , _id % 2 );
        }
        for( unsigned int i = 0 ; i < A ; i++ )
        {
            string g;
            int gates[3] = { 0 , 0 , 0 };
            istringstream strstream3( str_all[1+I+O+i] );
            for( int i = 0 ; i < 3 ; i++ )
            {
                getline( strstream3 , g , ' ' );
                gates[i] = atoi( (const char*)(g.c_str()) );
            }
                GATE[gates[0]/2]->setFanin( GATE[gates[1]/2] , ( gates[1] % 2 ) );
                GATE[gates[1]/2]->setFanout( GATE[gates[0]/2] , ( gates[1] % 2 ) );
                GATE[gates[0]/2]->setFanin( GATE[gates[2]/2] , ( gates[2] % 2 ) );
                GATE[gates[2]/2]->setFanout( GATE[gates[0]/2] , ( gates[2] % 2 ) );
        }
//=================================================================//
        for( unsigned int i = 0 ; i < PO.size() ; i++ )
            setdfs( PO[i] );
    }
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
    cout<<"Circuit Statistics"<<endl;
    cout<<"=================="<<endl;
    cout<<"  PI "<<right<<setw(11)<<PI.size()<<endl;
    cout<<"  PO "<<right<<setw(11)<<PO.size()<<endl;
    cout<<"  AIG"<<right<<setw(11)<<AIG.size()<<endl;
    cout<<"------------------"<<endl;
    cout<<"  Total"<<right<<setw(9)<<( PI.size() + PO.size() + AIG.size() )<<endl;
}

void
CirMgr::printNetlist() const
{
    cout<<endl;
    for( unsigned int i = 0 ; i < DFS.size() ; i++ )
    {
        cout<<"["<<i<<"] ";
        if( DFS[i]->getTypeStr() == "PI" )
        {
            cout<<DFS[i]->getTypeStr()<<"  "<<DFS[i]->getVar_ID();
            if( DFS[i]->IO_Name != "" ) cout<<" ("<<DFS[i]->IO_Name<<")";
        }
        else if( DFS[i]->getTypeStr() == "PO" )
        {
            cout<<DFS[i]->getTypeStr()<<"  "<<DFS[i]->getVar_ID()<<" ";
            if( DFS[i]->Fanin[0].gate()->getTypeStr() == "UNDEF" )
            {
                cout<<"*";
            }
            if( DFS[i]->Fanin[0].isInv() )
            {
                cout<<"!";
            }
            cout<<DFS[i]->Fanin[0].gate()->getVar_ID();
            if( DFS[i]->IO_Name != "" ) cout<<" ("<<DFS[i]->IO_Name<<")";
        }
        else if( DFS[i]->getTypeStr() == "AIG" )
        {
            cout<<DFS[i]->getTypeStr()<<" "<<DFS[i]->getVar_ID()<<" ";
            if( DFS[i]->Fanin[0].gate()->getTypeStr() == "UNDEF" )
            {
                cout<<"*";
            }
            if( DFS[i]->Fanin[0].isInv() )
            {
                cout<<"!";
            }
            cout<<DFS[i]->Fanin[0].gate()->getVar_ID()<<" ";
            if( DFS[i]->Fanin[1].gate()->getTypeStr() == "UNDEF" )
            {
                cout<<"*";
            }
            if( DFS[i]->Fanin[1].isInv() )
            {
                cout<<"!";
            }
            cout<<DFS[i]->Fanin[1].gate()->getVar_ID();
        }
        else if( DFS[i]->getTypeStr() == "CONST" )
            cout<<"CONST0";
        cout<<endl;

    }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
    for(int i = 0 ; i < PI.size() ; i++ )
        cout<<" "<<PI[i]->getVar_ID();
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
    for(int i = 0 ; i < PO.size() ; i++ )
        cout<<" "<<PO[i]->getVar_ID();
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
    vector<unsigned int>f_fanin;
    vector<unsigned int>f_fanout;
    for( int i= 0 ; i < AIG.size() ; i++ )
    {
        if( AIG[i]->Num_in() != 2 )
            f_fanin.push_back( AIG[i]->getVar_ID() );
        if( AIG[i]->Num_out() == 0 )
            f_fanout.push_back( AIG[i]->getVar_ID() );
    }
    for( int i = 0 ; i < PI.size() ; i++ )
    {
        if( PI[i]->Num_out() == 0 )
            f_fanout.push_back( PI[i]->getVar_ID() );
    }
    std::sort( f_fanin.begin() , f_fanin.end() );
    std::sort( f_fanout.begin() , f_fanout.end() );
    if(!f_fanin.empty() )
    {
        cout<<"Gates with floating fanin(s):";
        for( int i = 0 ; i < f_fanin.size() ; i++ )
            cout<<" "<<f_fanin[i];
        cout<<endl;
    }
    if(!f_fanout.empty() )
    {
        cout<<"Gates defined but not used  :";
        for( int i = 0 ; i < f_fanout.size() ; i++ )
            cout<<" "<<f_fanout[i];
        cout<<endl;
    }
}

void
CirMgr::writeAag(ostream& outfile) const
{
    vector<CirGate*> aig;
    for( int i = 0 ; i < DFS.size() ; i++ )
    {
        if( DFS[i]->getTypeStr() == "AIG" ) aig.push_back(DFS[i]);
    }
    outfile<<"agg "<<M<<" "<<I<<" 0 "<<O<<" "<<aig.size()<<endl;
    for( int i = 0 ; i < PI.size() ; i++ )
        outfile<<( PI[i]->getVar_ID() ) * 2<<endl;
    for( int i = 0 ; i < PO.size() ; i++ )
    {
        int phase = ( PO[i]->Fanin[0].isInv() ) ? 1 : 0;
        outfile<<( PO[i]->Fanin[0].gate()->getVar_ID() ) * 2 + phase << endl;
    }
    for( int i = 0 ; i < aig.size() ; i++ )
    {
        outfile<<( aig[i]->getVar_ID() ) * 2;
        int phase = ( aig[i]->Fanin[0].isInv() ) ? 1 : 0;
        outfile<<" "<<( aig[i]->Fanin[0].gate()->getVar_ID() ) * 2 + phase;
        phase = ( aig[i]->Fanin[1].isInv() ) ? 1 : 0;
        outfile<<" "<<( aig[i]->Fanin[1].gate()->getVar_ID() ) * 2 + phase << endl;
    }
    for( int i = 0 ; i < PI.size() ; i++ )
    {
        if( PI[i]->IO_Name != "" ) outfile<<"i"<<i<<" "<<PI[i]->IO_Name<<endl;
    }
    for( int i = 0 ; i < PO.size() ; i++ )
    {
        if( PO[i]->IO_Name != "" ) outfile<<"o"<<i<<" "<<PO[i]->IO_Name<<endl;
    }
    outfile<<"c"<<endl<<"AAG output by Te-Yi Kan"<<endl;
}

void CirMgr::setdfs( CirGate* c )
{
    for( int i = 0 ; i < c->Fanin.size() ; i++ )
    {
        if( c->Fanin[i].gate()->isVisit == false && c->Fanin[i].gate()->getTypeStr() != "UNDEF" )
        {
            setdfs( c->Fanin[i].gate() );
        }
    }
    c->isVisit = true;
    DFS.push_back(c);
}
CirGate* CirMgr::getGate( unsigned gid ) const
{
    if( gid >= GATE.size() ) return 0;
    if( GATE[gid]->getTypeStr() == "UNDEF" || GATE[gid] == NULL )
    {
        if( GATE[gid]->Num_out() != 0 ) return GATE[gid];
        else return 0;
    }
    return GATE[gid];
}
