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

#include <map>

#include "wx/wx.h"

class FunctionDefinition;

class FunctionTable
{
  // FunctionTable is the table of pointers to FunctionDefinitions
  // FunctionTable is a "singleton" (only one static object)
  // In FunctionTable, each FunctionDefinition pointer is stored
  // with its name as a key to itself, so each FunctionDefinition
  // object must have a unique name.
  
public:
  typedef std::map<wxString const,FunctionDefinition*> functionMap;
  typedef functionMap::value_type entryType;

protected:
  FunctionTable();
  FunctionTable( FunctionTable const & );
  
public:
  static FunctionTable *GetTable();

  virtual ~FunctionTable();
  
  void ClearTable();
  
  bool Contains( FunctionDefinition const * ) const;
  bool Contains( wxString const & ) const;
  
  int Entries() const
  { return functionMap_->size(); }

  FunctionDefinition *GetFunction( wxString const & ) const;
  void AddEntry( FunctionDefinition * );

  void CreateFunctions();

private:
  functionMap *functionMap_;
  static FunctionTable *functionTable_;
};

#endif
