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

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGateV
{
public:
    #define NEG 0x1
    CirGateV(CirGate* g, size_t phase):
        _gateV( size_t(g) + phase ) {}
    CirGate* gate() const {
        return (CirGate*)(_gateV & ~size_t(NEG));
    }
    bool isInv() const { return (_gateV & NEG); }
    
private:
    size_t      _gateV;
    
};
class CirGate
{
public:
    friend class CirMgr;
    CirGate( unsigned int l = 0 , unsigned int i = 0 , GateType t = UNDEF_GATE , string io = "" , bool v = false ):
        line(l) , ID(i) , type(t) , IO_Name(io) , isVisit(v) {}
    ~CirGate() {;}

   // Basic access methods
   string getTypeStr() const;
   unsigned getLineNo() const { return line; }
    unsigned int getVar_ID() const { return ID; }

   // Printing functions
   //virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   void report( int level , int blank , bool fanin , bool inverse , vector<const CirGate*>* done ) const;
    void setFanin( CirGate* g , int phase )
    {
        Fanin.push_back( CirGateV( g , phase ) );
    }
    
    void setFanout( CirGate* g , int phase )
    {
        Fanout.push_back( CirGateV( g , phase ) );
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
            if( Fanin[i].gate()->getTypeStr() != "UNDEF" && Fanin[i].gate() != NULL )
                count++;
        return count;
    }
    int Num_out()
    {
        int count =0;
        for( int i = 0 ; i < Fanout.size() ; i++ )
            if( Fanout[i].gate()->getTypeStr() != "UNDEF" && Fanout[i].gate() != NULL )
                count++;
        return count;
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
};
/*
class AigGate: public CirGate
{
public:
    friend class CirMgr;
    
private:
    
};

class PiGate: public CirGate
{
public:
    friend class CirMgr;
    
private:
    
};

class PoGate: public CirGate
{
public:
    friend class CirMgr;
    
private:
    
};

class ConstGate: public CirGate
{
public:
    friend class CirMgr;
    
private:
    
};

class UndefGate: public CirGate
{
public:
    friend class CirMgr;
    
private:
    
};
*/

#endif // CIR_GATE_H
