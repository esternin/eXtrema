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
#ifndef EX_TEXTVARIABLE
#define EX_TEXTVARIABLE

#define VARIABLES_LINKAGE_
/*
#ifdef BUILDING_VARIABLES_DLL_
#define VARIABLES_LINKAGE_ __declspec(dllexport)
#else
#define VARIABLES_LINKAGE_ __declspec(dllimport)
#endif
*/

#include <ostream>
#include <vector>
#include <deque>

#include "ExString.h"
#include "TextData.h"
#include "TVariableTable.h"

#include "Variable.h"

class NumericData;

class VARIABLES_LINKAGE_ TextVariable : public Variable
{
public:
  TextVariable( ExString const &name ) : Variable(name)
  { TVariableTable::GetTable()->AddEntry(this); }
  
  ~TextVariable()
  {}
  
  TextVariable( TextVariable const &rhs ) : Variable(rhs)
  { data_ = rhs.data_; }
  
  TextVariable &operator=( TextVariable const &rhs )
  {
    if( this != &rhs )
    {
      Variable::CopyStuff( rhs );
      data_ = rhs.data_;
    }
    return *this;
  }

  TextData &GetData()
  { return data_; }
  
  void SetData( TextData const &data )
  { data_ = data; }
  
  // scalar text variable
  static TextVariable *PutVariable( ExString, ExString const &, ExString const &, bool =true );

  // array text variable
  static TextVariable *PutVariable( ExString, std::vector<ExString> const &, ExString const &, bool =true );

  static void GetVariable( ExString const &, bool, ExString & );

  ExString GetString( NumericData const *, NumericData const * );

  bool DeletePartial( ExString &, ExString const & );

  friend std::ostream &operator<<( std::ostream &, TextVariable const & );

private:
  static void GetIndices( ExString &, int &, std::vector<double> *, bool * );

  TextData data_;
};

#endif
