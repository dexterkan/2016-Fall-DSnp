/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"
#include "algorithm"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
class faninNode
{
public:
    faninNode( CirGate* c = NULL )
    {
        gate = c;
        if( gate != NULL )
        {
            unsigned int f1 = c->Fanin[0].getId_withInv();
            unsigned int f2 = c->Fanin[1].getId_withInv();
            if( f1 < f2 )
            {
                fanin1 = f1;
                fanin2 = f2;
            }
            
            else
            {
                fanin1 = f2;
                fanin2 = f1;
            }
        }
        else
        {
            fanin1 = 0;
            fanin2 = 0;
        }
    }
    ~faninNode() {;}
    
    bool operator == (const faninNode& f) const { return ( fanin1 == f.fanin1 && fanin2 == f.fanin2 ); }
    faninNode& operator = ( const faninNode& f )
    {
        gate = f.gate;
        fanin1 = f.fanin1;
        fanin2 = f.fanin2;
        return *(this);
    }
    size_t operator () () const
    {
        size_t k = 0;
        k ^= ( fanin1 << 6 );
        k ^= ( fanin2 << 12 );
        return k;
    }
    
    CirGate* getgate() const
    {
        return gate;
    }
    
private:
    CirGate*     gate;
    unsigned int fanin1;
    unsigned int fanin2;
};
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
    size_t num = getHashSize( GATE.size() );
    HashSet< faninNode > myset( num );
    bool isstrash = false;
    for( size_t i = 0 ; i < DFS.size() ; i++ )
    {
        if( DFS[i]->getTypeStr() == "AIG" )
        {
            faninNode get;
            if( !myset.insert( faninNode(DFS[i]) , get ) )
            {
                cout<<"Strashing: "<<get.getgate()->getVar_ID()<<" merging "<<DFS[i]->getVar_ID()<<"..."<<endl;
                mergeGate( DFS[i] , get.getgate() );
                isstrash = true;
            }
        }
    }
    if( isstrash )
    {
        DFS.clear();
        for( unsigned int i = 0 ; i < GATE.size() ; i++ )
            if( GATE[i] != NULL ) GATE[i]->indfs = false;
        for( unsigned int i = 0 ; i < PO.size() ; i++ )
            setdfs( PO[i] );
        for( unsigned int i = 0 ; i < DFS.size() ; i++ )
            DFS[i]->isVisit = false;
    }
}

void
CirMgr::fraig()
{
    if( FecGroups.empty() ) return;
    for( int i = 0 ; i < DFS.size() ; i++ )
    {
        DFS[i]->order = i + 1 ;
    }
    SatSolver solver;
    solver.initialize();
    GATE[0]->_var = solver.newVar();
    for( size_t i = 0 ; i < GATE.size() ; i++ )
    {
        if( GATE[i] != NULL && (GATE[i]->getTypeStr() == "AIG" || GATE[i]->getTypeStr() == "PI") )
            GATE[i]->_var = solver.newVar();
    }
    for( size_t i = 0 ; i < GATE.size() ; i++ )
    {
        if( GATE[i] != NULL && GATE[i]->getTypeStr() == "AIG" )
        {
            solver.addAigCNF( GATE[i]->_var , GATE[i]->Fanin[0].gate()->_var , GATE[i]->Fanin[0].isInv() , GATE[i]->Fanin[0].gate()->_var , GATE[i]->Fanin[0].isInv() );
        }
    }
    dofraig(solver);
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

void CirMgr::dofraig( SatSolver& solver )
{
    for( list< vector<unsigned int>* >::iterator iter = FecGroups.begin() ; iter != FecGroups.end() ; ++iter )
    {
        bool done = false;
        unsigned int now = 0;
        for( vector<unsigned int>::iterator iter2 = (*iter)->begin() ; iter2 != (*iter)->end() ; ++iter2 )
        {
            if( !getResult( *((*iter)->begin()) , *iter2 , solver ) ) //fec merge
            {
                if( GATE[*((*iter)->begin())/2]->order < GATE[*iter2/2]->order )
                    merge( GATE[*iter2/2] , GATE[*((*iter)->begin())/2] );
                else
                {
                    merge( GATE[*((*iter)->begin())/2] , GATE[*iter2/2] );
                    *((*iter)->begin()) = *iter2;
                }
                *iter2 = *( (*iter)->end() -1 );
                (*iter)->pop_back();
                iter2--;
            }
            else // not fec
            {
                if(!done)
                {
                    now = *iter2/2;
                    GATE[now]->FecGroup = new vector<unsigned int>( 1 , *iter2 );
                    done = true;
                }
                else
                {
                    GATE[*iter2/2]->FecGroup = GATE[now]->FecGroup;
                    GATE[*iter2/2]->FecGroup->push_back(*iter2);
                }
                *iter2 = *( (*iter)->end() -1 );
                (*iter)->pop_back();
                iter2--;
            }
        }
    }
    for( list< vector<unsigned int>* >::iterator iter = FecGroups.begin() ; iter != FecGroups.end() ; )
    {
        if( (*iter)->size() == 1 )
            FecGroups.erase(iter++);
        else iter++;
    }
}

void CirMgr::merge( CirGate* merged , CirGate* merging )
{
    
}
               
bool CirMgr::getResult( unsigned int a , unsigned int b , SatSolver& s )
{
    bool result;
    bool inv = ( a%2 != b%2 );
    s.assumeProperty( GATE[0]->_var , false );
    if( a/2 == 0 )
    {
        s.assumeProperty( GATE[b/2]->_var , !inv );
    }
    else
    {
        Var newV = s.newVar();
        s.addXorCNF( newV , GATE[a/2]->_var , false , GATE[b/2]->_var , inv );
        s.assumeRelease();
        s.assumeProperty(newV, true);
    }
    result = s.assumpSolve();
    return result;
}
