/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "myHashMap.h"
#include "util.h"
#include "algorithm"
#include <map>
#include <list>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

enum fanint_type
{
    F1_CONST0,
    F1_CONST1,
    F2_CONST0,
    F2_CONST1,
    Equal_NoneInv,
    Equal_Inv,
    F1_Inv,
    F2_Inv,
    None
};

class simNode
{
public:
    simNode( unsigned int ID = 0 ): Id(ID) {}
    ~simNode() {;}
    
    bool operator == (const simNode& s) const { return ( Id == s.Id ); }
    simNode& operator = ( const simNode& s ) { Id = s.Id; return *(this); }
    unsigned int operator () () const { return Id; }
    
private:
    unsigned int Id ;
};

class CirMgr
{
public:
    CirMgr( bool sweep = false , bool sim = false ): issweep(sweep) , isSimulation(sim){}
    ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
    CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
    void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() ;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   void setdfs( CirGate* c );
   void mergeGate( CirGate* mergedGate , CirGate* mergingGate );
   fanint_type fanintype( CirGate* c );
   CirGate* gate( unsigned g ) { return GATE[g]; }
   unsigned int GateSim( unsigned int Id , unsigned int N );
   void Simulation( unsigned int patternNum );
   void setwrite( CirGate *g , vector<unsigned int>* PIlist , vector<unsigned int>* AIGlist ) const;
   bool getResult( unsigned int a , unsigned int b , SatSolver& s );
   void merge( CirGate* merged , CirGate* merging );
   void dofraig( SatSolver& solver );

private:
   ofstream           *_simLog;
    unsigned int M;
    unsigned int I;
    unsigned int L;
    unsigned int O;
    unsigned int A;
    unsigned int Real_AIG;
    vector<CirGate*> GATE;
    vector<CirGate*> PI;
    vector<CirGate*> PO;
    vector<CirGate*> DFS;
    bool issweep;
    bool isSimulation;
    vector<unsigned int> PIsimValue;
//    map< unsigned int ,  vector<unsigned int> > FECGroups;
    Cache<simNode,unsigned int> myCache;
    list< vector<unsigned int>* > FecGroups;

};

#endif // CIR_MGR_H
