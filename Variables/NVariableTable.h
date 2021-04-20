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
#ifndef EX_NVARIABLETABLE
#define EX_NVARIABLETABLE

#define VARIABLES_LINKAGE_
/*
#ifdef BUILDING_VARIABLES_DLL_
#define VARIABLES_LINKAGE_ __declspec(dllexport)
#else
#define VARIABLES_LINKAGE_ __declspec(dllimport)
#endif
*/

#include <map>
#include <ostream>

#include "ExString.h"

class NumericVariable;

class VARIABLES_LINKAGE_ NVariableTable
{
private:
  typedef std::map<ExString const,NumericVariable*> NVariableMap;
  typedef NVariableMap::value_type entry_type;

  // NVariableTable is the table of pointers to NumericVariables
  // NVariableTable is a "singleton" (only one static object)
  // In NVariableTable, each NumericVariable pointer is stored
  // with its name as a key to itself, so each variable
  // object must have a unique name

public:
  static NVariableTable *GetTable()
  {
    // returns the pointer to the NVariableTable object
    // NVariableTable is a "singleton", and can get its
    // pointer by this variable.  The first time this
    // variable is called, the NVariableTable object
    // is instantiated.
    if( !variableTable_ )variableTable_ = new NVariableTable();
    return variableTable_;
  }

  virtual ~NVariableTable()
  { DeleteTable(); }

  static void DeleteTable()
  {
    if( variableTable_ )
    {
      variableTable_->ClearTable();
      delete variableTable_;
      variableTable_ = 0;
    }
  }

  bool Contains( ExString const &name ) const
  { return ( vMap_.find(name.UpperCase())!=vMap_.end() ); }

  NumericVariable *GetVariable( ExString const &name ) const
  {
    NVariableMap::const_iterator i = vMap_.find( name.UpperCase() );
    if( i != vMap_.end() )return (*i).second;
    return 0;
  }

  void RemoveScalars()
  { if( RemoveVariables(0) )RemoveScalars(); }

  void RemoveVectors()
  { if( RemoveVariables(1) )RemoveVectors(); }

  void RemoveMatrices()
  { if( RemoveVariables(2) )RemoveMatrices(); }

  void RemoveTensors()
  { if( RemoveVariables(3) )RemoveTensors(); }

  int Entries() const
  { return vMap_.size(); }

  NumericVariable *GetEntry( int i )
  {
    NVariableMap::iterator k = vMap_.begin();
    for( int j=0; j<i; ++j )++k;
    return (*k).second;
  }

  void AddEntry( NumericVariable * );
  void RemoveEntry( NumericVariable * );

  void NVariableTable::RemoveEntry( ExString const &name )
  { RemoveEntry( GetVariable(name) ); }

  bool RemoveVariables( std::size_t );
  void ClearTable();

  friend std::ostream &operator<<( std::ostream &, NVariableTable const & );

protected:
  NVariableTable()
  {}

  NVariableTable( NVariableTable const &rhs )
  { vMap_ = rhs.vMap_; }

private:
  NVariableMap vMap_;
  static NVariableTable *variableTable_;
};

#endif

