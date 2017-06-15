/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
    for( size_t i = 0 ; i < GATE.size() ; i++ )
    {
        if( GATE[i] == NULL ) continue;
        if( GATE[i]->getTypeStr() == "AIG" )
        {
            if( GATE[i]->indfs == false )
            {
                cout<<"Sweeping: AIG("<<GATE[i]->getVar_ID()<<") removed..."<<endl;
                delete  GATE[i];
                GATE[i] = NULL;
                Real_AIG--;
            }
        }
        else if( GATE[i]->getTypeStr() == "UNDEF" )
        {
            if( GATE[i]->indfs == false )
            {
                cout<<"Sweeping: UNDEF("<<GATE[i]->getVar_ID()<<") removed..."<<endl;
                delete  GATE[i];
                GATE[i] = NULL;
            }
        }
    }
    issweep = true;
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
    bool do_optimize = false;
    for( size_t i = 0 ; i < DFS.size() ; i++ )
    {
        if( DFS[i]->getTypeStr() == "AIG" )
        {
            switch( fanintype(DFS[i]) )
            {
                case F1_CONST0:
                    cout<<"Simplifying: 0 merging "<<DFS[i]->getVar_ID()<<"..."<<endl;
                    mergeGate( DFS[i] , GATE[0] );
                    do_optimize = true;
                    break;
                    
                case F1_CONST1:
                    if( DFS[i]->Fanin[1].isInv() )
                        cout<<"Simplifying: "<<DFS[i]->Fanin[1].gate()->getVar_ID()<<" merging !"<<DFS[i]->getVar_ID()<<"..."<<endl;
                    else
                        cout<<"Simplifying: "<<DFS[i]->Fanin[1].gate()->getVar_ID()<<" merging "<<DFS[i]->getVar_ID()<<"..."<<endl;
                    mergeGate( DFS[i] , DFS[i]->Fanin[1].gate() );
                    do_optimize = true;
                    break;
                    
                case F2_CONST0:
                    cout<<"Simplifying: 0 merging "<<DFS[i]->getVar_ID()<<"..."<<endl;
                    mergeGate( DFS[i] , GATE[0] );
                    do_optimize = true;
                    break;
                    
                case F2_CONST1:
                    if( DFS[i]->Fanin[0].isInv() )
                        cout<<"Simplifying: "<<DFS[i]->Fanin[0].gate()->getVar_ID()<<" merging !"<<DFS[i]->getVar_ID()<<"..."<<endl;
                    else
                        cout<<"Simplifying: "<<DFS[i]->Fanin[0].gate()->getVar_ID()<<" merging "<<DFS[i]->getVar_ID()<<"..."<<endl;
                    mergeGate( DFS[i] , DFS[i]->Fanin[0].gate() );
                    do_optimize = true;
                    break;
                    
                case Equal_NoneInv:
                    cout<<"Simplifying: "<<DFS[i]->Fanin[0].gate()->getVar_ID()<<" merging "<<DFS[i]->getVar_ID()<<"..."<<endl;
                    mergeGate( DFS[i] , DFS[i]->Fanin[0].gate() );
                    do_optimize = true;
                    break;

                case Equal_Inv:
                    cout<<"Simplifying: "<<DFS[i]->Fanin[0].gate()->getVar_ID()<<" merging !"<<DFS[i]->getVar_ID()<<"..."<<endl;
                    mergeGate( DFS[i] , DFS[i]->Fanin[0].gate() );
                    do_optimize = true;
                    break;
                
                case F1_Inv:
                    cout<<"Simplifying: 0 merging "<<DFS[i]->getVar_ID()<<"..."<<endl;
                    mergeGate( DFS[i] , GATE[0] );
                    do_optimize = true;
                    break;
                
                case F2_Inv:
                    cout<<"Simplifying: 0 merging "<<DFS[i]->getVar_ID()<<"..."<<endl;
                    mergeGate( DFS[i] , GATE[0] );
                    do_optimize = true;
                    break;
                    
                case None:
                    break;
                    
                default:
                    break;
            }
            
        }
    }
    if( do_optimize )
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

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
fanint_type CirMgr::fanintype( CirGate* c )
{
    if( c->Fanin[0].gate()->getTypeStr() == "CONST" )
    {
        if( c->Fanin[0].isInv() )
            return F1_CONST1;
        else
            return F1_CONST0;
    }
    else
    {
        if( c->Fanin[1].gate()->getTypeStr() == "CONST" )
        {
            if( c->Fanin[1].isInv() )
                return F2_CONST1;
            else
                return F2_CONST0;
        }
    }
    if( c->Fanin[0].gate()->getVar_ID() == c->Fanin[1].gate()->getVar_ID() )
    {
        if( !c->Fanin[0].isInv() && !c->Fanin[1].isInv() )
            return Equal_NoneInv;
        else if( !c->Fanin[0].isInv() && c->Fanin[1].isInv() )
            return F2_Inv;
        else if( c->Fanin[0].isInv() && !c->Fanin[1].isInv() )
            return F1_Inv;
        else
            return Equal_Inv;
    }
    return None;
}

void CirMgr::mergeGate( CirGate* mergedGate , CirGate* mergingGate )
{
    for( int i = 0 ; i < mergedGate->Fanout.size() ; i++ )
    {
        if( GATE[ mergedGate->Fanout[i].getId() ] != NULL )
        {
            if( mergedGate->Fanout[i].gate()->Fanin[0].getId() == mergedGate->getVar_ID() )
            {
                int inv = 0;
                if( mergedGate->Fanout[i].gate()->Fanin[0].isInv() ) inv++;
                if( mergedGate->Fanin[0].getId() == mergingGate->getVar_ID() )
                {
                    if( mergedGate->Fanin[0].isInv() )
                    {
                        if( mergedGate->Fanin[0].getId() != 0 )
                            inv++;
                    }
                }
                else if( mergedGate->Fanin[0].getId() != mergingGate->getVar_ID() )
                {
                    if( mergedGate->Fanin[1].getId() == mergingGate->getVar_ID() && mergedGate->Fanin[1].isInv() ) inv++;
                }
                mergedGate->Fanout[i].gate()->Fanin[0] = CirGateV( mergingGate , ( inv == 1 ) ? 1 : 0 , mergingGate->getVar_ID() );
                bool strash = true;
                for( int j = 0 ; j < mergingGate->Fanout.size() ; j++ )
                {
                    if( mergingGate->Fanout[j].getId() == mergedGate->getVar_ID() )
                    {
                        mergingGate->Fanout[j] = CirGateV( mergedGate->Fanout[i].gate() , ( inv == 1 ) ? 1 : 0 , mergedGate->Fanout[i].getId() );
                        strash = false;
                    }
                }
                if(strash) mergingGate->setFanout( mergedGate->Fanout[i].gate() , ( inv == 1 ) ? 1 : 0 , mergedGate->Fanout[i].getId() );
            }
            if( mergedGate->Fanout[i].gate()->Fanin[1].getId() == mergedGate->getVar_ID() )
            {
                int inv = 0;
                if( mergedGate->Fanout[i].gate()->Fanin[1].isInv() ) inv++;
                if( mergedGate->Fanin[0].getId() == mergingGate->getVar_ID() )
                {
                    if( mergedGate->Fanin[0].isInv() )
                    {
                        if( mergedGate->Fanin[0].getId() != 0 )
                            inv++;
                    }
                }
                else if( mergedGate->Fanin[0].getId() != mergingGate->getVar_ID() )
                {
                    if( mergedGate->Fanin[1].getId() == mergingGate->getVar_ID() && mergedGate->Fanin[1].isInv() ) inv++;
                }
                mergedGate->Fanout[i].gate()->Fanin[1] = CirGateV( mergingGate , ( inv == 1 ) ? 1 : 0 , mergingGate->getVar_ID() );
                bool strash = true;
                for( int j = 0 ; j < mergingGate->Fanout.size() ; j++ )
                {
                    if( mergingGate->Fanout[j].getId() == mergedGate->getVar_ID() )
                    {
                        mergingGate->Fanout[j] = CirGateV( mergedGate->Fanout[i].gate() , ( inv == 1 ) ? 1 : 0 , mergedGate->Fanout[i].getId() );
                        strash = false;
                    }
                }
                if(strash) mergingGate->setFanout( mergedGate->Fanout[i].gate() , ( inv == 1 ) ? 1 : 0 , mergedGate->Fanout[i].getId() );
            }
        }
    }
    
    delete GATE[ mergedGate->getVar_ID() ];
    GATE[ mergedGate->getVar_ID() ] = NULL;
    Real_AIG--;
}


