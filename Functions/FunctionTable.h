/*
Copyright (C) 2005 Joseph L. Chuma, TRIUMF

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef FCN_FUNCTIONTABLE
#define FCN_FUNCTIONTABLE

#define FUNCTIONS_LINKAGE_
/*
#if defined BUILDING_FUNCTIONS_DLL_
#define FUNCTIONS_LINKAGE_ __declspec(dllexport)
#else
#define FUNCTIONS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include "ExString.h"
#include "FunctionDefinition.h"

class FUNCTIONS_LINKAGE_ FunctionTable
{
  // FunctionTable is the table of pointers to FunctionDefinitions
  // FunctionTable is a "singleton" (only one static object)
  // In FunctionTable, each FunctionDefinition pointer is stored
  // with its name as a key to itself, so each FunctionDefinition
  // object must have a unique name.
  
public:
  typedef std::map<ExString const,FunctionDefinition*> functionMap;
  typedef functionMap::value_type entryType;

protected:
  FunctionTable()
  { functionMap_ = new functionMap; }

  FunctionTable( FunctionTable const &rhs )
  {
    delete functionMap_;
    functionMap_ = new functionMap(*(rhs.functionMap_));
  }
  
public:
  static FunctionTable *GetTable()
  {
    // returns the pointer to the FunctionTable object
    // FunctionTable is a "singleton", and can get its
    // pointer by this function.  The first time this
    // function is called, the FunctionTable object
    // is instantiated.
    //
    if( functionTable_ == 0 )functionTable_ = new FunctionTable();
    return functionTable_;
  }

  virtual ~FunctionTable()
  { delete functionMap_; }
  
  bool Contains( FunctionDefinition const *f ) const
  { return Contains( f->Name() ); }

  bool Contains( ExString const &name ) const
  { return ( functionMap_->find(name) != functionMap_->end() ); }
  
  int Entries() const
  { return functionMap_->size(); }

  FunctionDefinition *GetFunction( ExString const &name ) const
  {
    functionMap::iterator const i = functionMap_->find(name);
    if( i != functionMap_->end() )return (*i).second;
    return 0;
  }
  
  void AddEntry( FunctionDefinition * );

private:
  functionMap *functionMap_;
  static FunctionTable *functionTable_;
};

#endif
