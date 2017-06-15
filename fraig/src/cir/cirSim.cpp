/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include "myHashMap.h"
#include <limits>
#include <cstddef>
#include <cmath>
#include <sstream>
#include <time.h>

using namespace std;
int aaacount = 0;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

class SimValueKey
{
public:
    SimValueKey( unsigned int s = 0 ){
        if( s > ~s ) SimValue = s;
        else SimValue = ~s;
    }
    
    ~SimValueKey() {;}
    
    bool operator == (const SimValueKey& s) const { return( SimValue == s.SimValue); }
    SimValueKey& operator = ( const SimValueKey& s )
    {
        SimValue = s.SimValue;
        return *(this);
    }
    size_t operator () () const
    {
        return SimValue;
    }
private:
    unsigned int SimValue;
};





/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
    unsigned int circle = 0;
    unsigned int max;
    max = ((sqrt(Real_AIG+PI.size())/32 + 1) <=10) ? (sqrt(Real_AIG+PI.size())/32 + 1) : 10;
    cout<<"MAX_FAILS = "<<" "<<max<<endl;
    srand(time(NULL));
    if( !isSimulation )
    {
        for( size_t i = 0 ; i < PI.size() ; i++ )
        {
            PIsimValue.push_back( rand() % UINT_MAX );
        }
    }
    else
    {
        for( size_t i = 0 ; i < PI.size() ; i++ )
        {
            PIsimValue[i] = rand() % UINT_MAX;
        }
    }
    while( max )
    {
        size_t originalSize = FecGroups.size();
        Simulation(32);
        if( FecGroups.size() ==  originalSize )
            max--;
        else
            max = ((sqrt(Real_AIG+PI.size())/32 + 1) <=10) ? (sqrt(Real_AIG+PI.size())/32 + 1) : 10;
        circle++;
    }
    cout<<(circle*32)<<" patterns simulated"<<endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
    
    if( !isSimulation )
    {
        for( size_t i = 0 ; i < PI.size() ; i++ )
        {
            PIsimValue.push_back(0);
        }
    }
    else
    {
        for( size_t i = 0 ; i < PI.size() ; i++ )
        {
            PIsimValue[i] = 0;
        }
    }
    bool isSim = true;
    unsigned int bitNum = 0, circle = 0;
    string str ;
    while( getline( patternFile , str ) )
    {
        if( !str.empty() )
        {
            string simstr;
            istringstream stream(str);
            while( getline( stream , simstr , ' ' ) )
            {
                if(!simstr.empty())
                {
                    if( simstr.length() != PI.size() )
                    {
                        cerr<<"Error: Pattern("<<simstr<<") length("<<str.length()<<") does not match the number of inputs("<<PI.size()<<") in a circuit!!"<<endl;
                        isSim = false;
                        break;
                    }
                    if( simstr.find_first_not_of("10") != string::npos )
                    {
                        cerr<<"Error: Pattern("<<simstr<<") contains a non-0/1 character(‘"<<simstr[simstr.find_first_not_of("10")]<<"’)."<<endl;
                        isSim = false;
                        break;
                    }
                    
                    for( size_t i = 0 ; i < PI.size() ; i++ )
                    {
                        if( simstr[i] == '1' )
                            PIsimValue[i] ^= ( 1 << bitNum );
                    }
                    bitNum++;
                    
                    if( bitNum == 32 )
                    {
                        Simulation(32);
                        for( size_t i = 0 ; i < PI.size() ; i++ )
                        {
                            PIsimValue[i] = 0;
                        }
                        bitNum = 0;
                        circle++;
                    }
                }
                else continue;
            }
        }
        else continue;
    }
    if( !isSim )
    {
        cout<<"0 patterns simulated"<<endl;
        return;
    }
    if( bitNum != 0 )
    {
        Simulation(bitNum);
        for( size_t i = 0 ; i < PI.size() ; i++ )
        {
            PIsimValue[i] = 0;
        }
    }
    cout<<(circle*32+bitNum)<<" patterns simulated"<<endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

void CirMgr::Simulation( unsigned int patternNum )
{
    for( size_t i = 0 ; i < PI.size() ; i++ )
        PI[i]->SimValue = PIsimValue[i];
    myCache.init(M+O+1);

    for( size_t i = 0 ; i < PO.size() ; i++ )
    {
        unsigned int temp = 0;
        unsigned int input = 0;
        if( PO[i]->Fanin[0].gate()->getTypeStr() != "UNDEF" || PO[i]->Fanin[0].gate()->getTypeStr() != "CONST" );
        {
            if( myCache.read( simNode(PO[i]->Fanin[0].getId()) , input ) );
            else
            {
                if( PO[i]->Fanin[0].gate()->getTypeStr() == "PI" )
                    input = PO[i]->Fanin[0].gate()->SimValue;
                else
                    input = GateSim( PO[i]->Fanin[0].getId() , patternNum );
            }
        }
        if( !PO[i]->Fanin[0].isInv() ) temp = input;
        else temp = ~input;
        myCache.write( simNode(PO[i]->getVar_ID()) , temp );
        PO[i]->SimValue = temp;
    }
    myCache.reset();

    if( !isSimulation )
    {
        HashMap< SimValueKey , vector<unsigned int>* > simset( getHashSize(DFS.size()) );
        GATE[0]->FecGroup = new vector<unsigned int>(1,0);
        simset.pureinsert( SimValueKey(0) , GATE[0]->FecGroup );
        FecGroups.push_back( GATE[0]->FecGroup );
        for( size_t i = 0 ; i < DFS.size() ; i++ )
        {
            if( DFS[i]->getTypeStr() == "AIG" || DFS[i]->getTypeStr() == "CONST" )
            {
                if( simset.query( SimValueKey(DFS[i]->SimValue) , DFS[i]->FecGroup ) )
                {
                    if( GATE[(DFS[i]->FecGroup->at(0))/2]->SimValue ==  DFS[i]->SimValue )
                        DFS[i]->FecGroup->push_back( DFS[i]->getVar_ID() * 2 );
                    else
                        DFS[i]->FecGroup->push_back( DFS[i]->getVar_ID() * 2 + 1 );
                }
                else
                {
/*
                    if( DFS[i]->SimValue == 0 ||  ~(DFS[i]->SimValue) == 0  )
                    {
                        DFS[i]->FecGroup = new vector<unsigned int>(1,0);
                        if( DFS[i]->SimValue == 0 ) DFS[i]->FecGroup->push_back( DFS[i]->getVar_ID() * 2 );
                        else DFS[i]->FecGroup->push_back( DFS[i]->getVar_ID() * 2 + 1 );
                        FecGroups.push_back( DFS[i]->FecGroup );
                        GATE[0]->FecGroup = DFS[i]->FecGroup;
                        simset.pureinsert( SimValueKey(0) , DFS[i]->FecGroup );
                        continue;
                    }
*/
                    DFS[i]->FecGroup = new vector<unsigned int>(1,DFS[i]->getVar_ID() * 2);
                    FecGroups.push_back( DFS[i]->FecGroup );
                    simset.pureinsert( SimValueKey(DFS[i]->SimValue) , DFS[i]->FecGroup );
                }

            }
        }
        isSimulation = true;
    }
    else
    {
        list< vector<unsigned int>* >::iterator origin = FecGroups.end();
        for( list< vector<unsigned int>* >::iterator iter = FecGroups.begin() ; iter != origin ; ++iter )
        {
            HashMap< SimValueKey , vector<unsigned int>* > simset( getHashSize( (*iter)->size() ) );
            simset.pureinsert( SimValueKey( GATE[(**iter)[0]/2]->SimValue ) , GATE[(**iter)[0]/2]->FecGroup );
            for( vector<unsigned int>::iterator iter2 = GATE[(**iter)[0]/2]->FecGroup->begin() + 1 ; iter2 != GATE[(**iter)[0]/2]->FecGroup->end() ; ++iter2 )
            {
                if( simset.query( SimValueKey(GATE[*iter2/2]->SimValue) , GATE[*iter2/2]->FecGroup ) )
                {
                    if( GATE[*iter2/2]->FecGroup == GATE[(**iter)[0]/2]->FecGroup ) continue;
                    if( GATE[(GATE[*iter2/2]->FecGroup->at(0))/2]->SimValue ==  GATE[*iter2/2]->SimValue )
                        GATE[*iter2/2]->FecGroup->push_back( GATE[*iter2/2]->getVar_ID() * 2 );
                    else
                        GATE[*iter2/2]->FecGroup->push_back( GATE[*iter2/2]->getVar_ID() * 2 + 1 );
                    *iter2 = *(GATE[(**iter)[0]/2]->FecGroup->end() - 1);
                    GATE[(**iter)[0]/2]->FecGroup->pop_back();
                    iter2--;
                }
                else
                {
                    GATE[*iter2/2]->FecGroup = new vector<unsigned int>(1,GATE[*iter2/2]->getVar_ID() * 2);
                    FecGroups.push_back( GATE[*iter2/2]->FecGroup );
                    simset.pureinsert( SimValueKey(GATE[*iter2/2]->SimValue) , GATE[*iter2/2]->FecGroup );
                    *iter2 = *(GATE[(**iter)[0]/2]->FecGroup->end() - 1);
                    GATE[(**iter)[0]/2]->FecGroup->pop_back();
                    iter2--;
                }
            }
        }
    }


    for( list< vector<unsigned int>* >::iterator iter = FecGroups.begin() ; iter != FecGroups.end() ; )
    {
        if( (*iter)->size() == 1 )
            FecGroups.erase(iter++);
        else iter++;
    }


    if(_simLog)
    {
        for( unsigned int i  = 0 ; i < patternNum ; i++ )
        {
            for( size_t j = 0 ; j < PI.size() ; j++ )
            {
                if( PI[j]->SimValue & ( 1 << i ) )
                    *_simLog<<"1";
                else
                    *_simLog<<"0";
            }
            
            *_simLog << " ";
            
            for( size_t j = 0 ; j < PO.size() ; j++ )
            {
                if( PO[j]->SimValue & ( 1 << i ) )
                    *_simLog<<"1";
                else
                    *_simLog<<"0";
            }
            
            *_simLog << endl;
        }
        _simLog->flush();
    }
}

unsigned int CirMgr::GateSim( unsigned int Id , unsigned int patternNum )
{
    unsigned int temp = 0;
    unsigned int input1 = 0, input2 = 0;
    if( GATE[Id]->Fanin[0].gate()->getTypeStr() != "UNDEF" && GATE[Id]->Fanin[0].gate()->getTypeStr() != "CONST" )
    {
        if( myCache.read( simNode(GATE[Id]->Fanin[0].getId()) , input1 ) );
        else
        {
            if( GATE[Id]->Fanin[0].gate()->getTypeStr() == "PI" )
                input1 = GATE[Id]->Fanin[0].gate()->SimValue;
            else
                input1 = GateSim( GATE[Id]->Fanin[0].getId() , patternNum );
        }
    }
    if( GATE[Id]->Fanin[1].gate()->getTypeStr() != "UNDEF" && GATE[Id]->Fanin[1].gate()->getTypeStr() != "CONST" )
    {
        if( myCache.read( simNode(GATE[Id]->Fanin[1].getId()) , input2 ) );
        else
        {
            if( GATE[Id]->Fanin[1].gate()->getTypeStr() == "PI" )
                input2 = GATE[Id]->Fanin[1].gate()->SimValue;
            else
                input2 = GateSim( GATE[Id]->Fanin[1].getId() , patternNum );
        }
    }
    if( !GATE[Id]->Fanin[0].isInv() && !GATE[Id]->Fanin[1].isInv() )  temp =  ( input1 & input2 );
    else if( GATE[Id]->Fanin[0].isInv() && !GATE[Id]->Fanin[1].isInv() ) temp = ( ~input1 & input2 );
    else if( !GATE[Id]->Fanin[0].isInv() && GATE[Id]->Fanin[1].isInv() ) temp = ( input1 & ~ input2 );
    else temp = ( ~input1 & ~input2 );
    myCache.write( simNode(Id) , temp );
    GATE[Id]->SimValue = temp;
    return temp;
}
