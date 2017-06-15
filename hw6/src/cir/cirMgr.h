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

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
    ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
    CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;
   void setdfs( CirGate* );
private:
    unsigned int M;
    unsigned int I;
    unsigned int L;
    unsigned int O;
    unsigned int A;
    vector<CirGate*> GATE;
    vector<CirGate*> PI;
    vector<CirGate*> PO;
    vector<CirGate*> AIG;
    vector<CirGate*> DFS;
//    vector<CirGate*> UNDEF;
    
};

#endif // CIR_MGR_H
