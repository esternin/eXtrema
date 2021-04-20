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
#ifndef OPR_OPERATORTABLE
#define OPR_OPERATORTABLE

#define OPERATORS_LINKAGE_
/*
#if defined BUILDING_OPERATORS_DLL_
#define OPERATORS_LINKAGE_ __declspec(dllexport)
#else
#define OPERATORS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include <map>

#include "ExString.h"
#include "EExpressionError.h"

class OperatorDefinition;

class OPERATORS_LINKAGE_ OperatorTable
{
  // OperatorTable is the table of pointers to OperatorDefinitions
  // OperatorTable is a "singleton" (only one static object)
  // In OperatorTable, each OperatorDefinition pointer is stored
  // with its name as a key to itself, so each OperatorDefinition
  // object must have a unique name.
  //
private:
  typedef std::map< ExString const, OperatorDefinition* > operatorMap;
  typedef operatorMap::value_type entryType;

protected:
  OperatorTable()
  { operatorMap_ = new operatorMap; }

  OperatorTable( OperatorTable const &rhs )
  {
    delete operatorMap_;
    operatorMap_ = new operatorMap(*(rhs.operatorMap_));
  }

public:
  static OperatorTable *GetTable()
  {
    // returns the pointer to the OperatorTable object
    // OperatorTable is a "singleton", and can get its
    // pointer by this operator.  The first time this
    // operator is called, the OperatorTable object
    // is instantiated.
    if( !operatorTable_ )operatorTable_ = new OperatorTable();
    return operatorTable_;
  }

  ~OperatorTable()
  { delete operatorMap_; }

  bool Contains( OperatorDefinition const *opd ) const;

  bool Contains( ExString const &name ) const
  { return ( operatorMap_->find( name ) != operatorMap_->end() ); }

  int Entries() const
  { return operatorMap_->size(); }

  OperatorDefinition *GetOperator( ExString const &name ) const
  {
    operatorMap::iterator i = operatorMap_->find( name );
    if( i != operatorMap_->end() )return (*i).second;
    return 0;
  }

  void AddEntry( OperatorDefinition *opd );

private:
  operatorMap *operatorMap_;
  static OperatorTable *operatorTable_;
};

#endif
 
