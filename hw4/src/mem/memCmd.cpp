/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
    vector<string> options;
    if (!CmdExec::lexOptions(option, options))
        return CMD_EXEC_ERROR;
    
    if (options.empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    
    else
    {
        int num = 0 , arraynum = 0;
        if( options.size() == 1 )
        {
            if( myStr2Int( options[0] , num ) )
            {
                if( num <= 0 )
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0] );
                else
                    mtest.newObjs( num );
            }
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0] );
        }
        else if( options.size() == 3 )
        {
            if( myStr2Int( options[0] , num ) )
            {
                if( num <= 0 )
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0] );
                else
                {
                    if( myStrNCmp( "-Array" , options[1] , 2 ) == 0 )
                    {
                        if( myStr2Int( options[2] , arraynum ) )
                        {
                            if( arraynum > 0 )
                            {
                                try
                                {
                                    mtest.newArrs( num , arraynum );
                                }
                                catch (const char* c)
                                {
                                    return CMD_EXEC_DONE;
                                }
                            }
                            else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );
                        }
                        else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );
                    }
                    else
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
                }
            }
            else if( myStrNCmp( "-Array" , options[0] , 2 ) == 0 )
            {
                if( myStr2Int( options[1] , arraynum ) )
                {
                    if( arraynum > 0 )
                    {
                        if( myStr2Int( options[2] , num ) )
                        {
                            if( num > 0 )
                            {
                                try
                                {
                                    mtest.newArrs( num , arraynum );
                                }
                                catch (const char* c)
                                {
                                    return CMD_EXEC_DONE;
                                }
                            }
                            else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );
                        }
                        else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );

                    }
                }
                else CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
            }
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0] );
        }
        else if( options.size() == 2 )
        {
            if( myStr2Int( options[0] , num ) )
                return CmdExec::errorOption(CMD_OPT_MISSING, options[0] );
            else
                return CMD_EXEC_ERROR;
        }
            
    }
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
    vector<string> options;
    if (!CmdExec::lexOptions(option, options))
        return CMD_EXEC_ERROR;
    
    if (options.empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, "");

    else
    {
        int num = 0;
        if( options.size() == 1 )
        {
            if( myStrNCmp( "-Index" , options[0] , 2 ) == 0 )
                return CmdExec::errorOption(CMD_OPT_MISSING, options[0] );
            else if( myStrNCmp( "-Random" , options[0] , 2 ) == 0 )
                return CmdExec::errorOption(CMD_OPT_MISSING, options[0] );
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0] );
        }
        else if( options.size() == 2 )
        {
            if( myStrNCmp( "-Index" , options[0] , 2 ) == 0 )
            {
                if( myStr2Int( options[1] , num ) )
                {
                    if( num >= 0 )
                    {
                        if( num < mtest.getObjListSize() )
                        {
                            mtest.deleteObj(num);
                        }
                        else
                        {
                            cerr<<num<<" is greater than _objList's size ("<<" mtest.getObjListSize() ) "<<endl;
                            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
                        }
                    }
                    else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
                }
                else
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
            }
            else if( myStrNCmp( "-Random" , options[0] , 2 ) == 0 )
            {
                if( myStr2Int( options[1] , num ) )
                {
                    if( num >= 0 && mtest.getObjListSize() > 0 )
                    {
                        for( int i = 0 ; i < num ; i++ )
                        {
                            size_t d = rnGen( mtest.getObjListSize() );
                            mtest.deleteObj(d);
                        }
                    }
                    else
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
                }
                else
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
            }
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0] );

        }
        else if( options.size() == 3 )
        {
            if( myStrNCmp( "-Index" , options[0] , 2 ) == 0 )
            {
                if( myStr2Int( options[1] , num ) )
                {
                    if( num >= 0 )
                    {
                        if( myStrNCmp( "-Array" , options[2] , 2 ) == 0 )
                        {
                            if( num < mtest.getArrListSize() )
                                mtest.deleteArr(num);
                            else
                            {
                                cerr<<num<<" is greater than _arrList's size ( "<<mtest.getArrListSize()<<" )"<<endl;
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
                            }
                        }
                        else
                            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );
                    }
                    else
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
                }
                else
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
            }
            else if( myStrNCmp( "-Random" , options[0] , 2 ) == 0 )
            {
                if( myStr2Int( options[1] , num ) )
                {
                    if( num >= 0 && mtest.getArrListSize() > 0 )
                    {
                        if( myStrNCmp( "-Array" , options[2] , 2 ) == 0 )
                        {
                            for( int i = 0 ; i < num ; i++ )
                            {
                                size_t d = rnGen( mtest.getArrListSize() );
                                mtest.deleteArr(d);
                            }
                        }
                        else
                            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );
                    }
                    else
                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
                }
                else
                    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
            }
            else if( myStrNCmp( "-Array" , options[0] , 2 ) == 0 )
            {
                if( myStrNCmp( "-Index" , options[1] , 2 ) == 0 )
                {
                    if( myStr2Int( options[2] , num ) )
                    {
                        if( num >= 0 )
                        {
                            if( num < mtest.getArrListSize() )
                                mtest.deleteArr(num);
                            else
                            {
                                cerr<<num<<" is greater than _arrList's size ( "<<mtest.getArrListSize()<<" )"<<endl;
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );
                            }
                        }
                        else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );
                    }
                    else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );
                }
                else if( myStrNCmp( "-Random" , options[1] , 2 ) == 0 )
                {
                    if( myStr2Int( options[2] , num ) )
                    {
                        if( num >= 0 && mtest.getArrListSize() > 0 )
                        {
                            for( int i = 0 ; i < num ; i++ )
                            {
                                size_t d = rnGen( mtest.getArrListSize() );
                                mtest.deleteArr(d);
                            }
                        }
                        else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );
                    }
                    else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2] );
                }
                else return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1] );
            }
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0] );
        }
        else
            return CMD_EXEC_ERROR;
    }
    

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


