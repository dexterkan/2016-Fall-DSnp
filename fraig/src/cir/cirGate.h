/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGateV
{
public:
#define NEG 0x1
    CirGateV(CirGate* g, size_t phase , unsigned int ID):
    _gateV( size_t(g) + phase ), Id( ID * 2 + phase ) {}
    CirGate* gate() const {
        return (CirGate*)(_gateV & ~size_t(NEG));
    }
    bool isInv() const { return (_gateV & NEG); }
    unsigned int getId() const { return Id / 2 ; }
    unsigned int getId_withInv() const { return Id; }

private:
    size_t      _gateV;
    unsigned int    Id;
    
};

class CirGate
{
public:
    friend class CirMgr;
    friend class faninNode;
    CirGate( unsigned int l = 0 , unsigned int i = 0 , GateType t = UNDEF_GATE , string io = "" , bool v = false , bool d = false , unsigned int sv = 0 , size_t w = 0 , size_t o = 0 ):
    line(l) , ID(i) , type(t) , IO_Name(io) , isVisit(v) , indfs(d) , SimValue(sv) , writegate(w) , order(o) {}
    ~CirGate() {;}

   // Basic access methods
    string getTypeStr() const;
   unsigned getLineNo() const { return line; }
     unsigned int getVar_ID() const { return ID; }
    bool isAig() const;

   // Printing functions
   //virtual void printGate() const {}
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;
    void report( int level , int blank , bool fanin , bool inverse , vector<const CirGate*>* done ) const;
    void setFanin( CirGate* g , int phase , unsigned int Id )
    {
        Fanin.push_back( CirGateV( g , phase , Id ) );
    }
    
    void setFanout( CirGate* g , int phase , unsigned int Id )
    {
        Fanout.push_back( CirGateV( g , phase , Id ) );
    }
    
    void setLine( unsigned int li )
    {
        line = li;
    }
    
    void setID( unsigned int Id )
    {
        ID = Id;
    }
    
    void setType( GateType ty )
    {
        type = ty;
    }
    
    void setIO_Name( string io_name )
    {
        IO_Name = io_name;
    }
    int Num_in()
    {
        int count =0;
        for( int i = 0 ; i < Fanin.size() ; i++ )
            if( Fanin[i].gate() != NULL && Fanin[i].gate()->getTypeStr() != "UNDEF" )
                count++;
        return count;
    }
    int Num_out()
    {
        return Fanout.size();
    }
private:

protected:
    unsigned int line;
    unsigned int ID;
    vector<CirGateV> Fanin;
    vector<CirGateV> Fanout;
    GateType type;
    string IO_Name;
    bool isVisit;
    bool indfs;
    size_t writegate;
    static size_t globalref;
    unsigned int SimValue;
    vector<unsigned int>* FecGroup;
    Var _var;
    size_t order;

};

#endif // CIR_GATE_H
