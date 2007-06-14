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

#include <map>

#include "wx/wx.h"

class OperatorDefinition;

class OperatorTable
{
  // OperatorTable is the table of pointers to OperatorDefinitions
  // OperatorTable is a "singleton" (only one static object)
  // In OperatorTable, each OperatorDefinition pointer is stored
  // with its name as a key to itself, so each OperatorDefinition
  // object must have a unique name.
  //
private:
  typedef std::map< wxString const, OperatorDefinition* > operatorMap;
  typedef operatorMap::value_type entryType;

protected:
  OperatorTable();
  OperatorTable( OperatorTable const & );

public:
  static OperatorTable *GetTable();

  ~OperatorTable();

  void ClearTable();

  bool Contains( OperatorDefinition const *opd ) const;

  bool Contains( wxString const &name ) const
  { return ( operatorMap_->find( name ) != operatorMap_->end() ); }

  int Entries() const
  { return operatorMap_->size(); }

  OperatorDefinition *GetOperator( wxString const &name ) const;

  void AddEntry( OperatorDefinition *opd );

  void CreateOperators();

private:
  operatorMap *operatorMap_;
  static OperatorTable *operatorTable_;
};

#endif
 
