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
#ifndef EX_NUMERICVARIABLE
#define EX_NUMERICVARIABLE

#define VARIABLES_LINKAGE_
/*
#ifdef BUILDING_VARIABLES_DLL_
#define VARIABLES_LINKAGE_ __declspec(dllexport)
#else
#define VARIABLES_LINKAGE_ __declspec(dllimport)
#endif
*/

#include <vector>
#include <deque>
#include <ostream>

#include "ExString.h"
#include "NumericData.h"
#include "NVariableTable.h"

#include "Variable.h"

class VARIABLES_LINKAGE_ NumericVariable : public Variable
{
public:
  NumericVariable( ExString const &name ) : Variable(name)
  { NVariableTable::GetTable()->AddEntry(this); }

  ~NumericVariable()
  {}

  NumericVariable( NumericVariable const &rhs ) : Variable(rhs)
  { CopyStuff(rhs); }

  NumericVariable & operator=( NumericVariable const &rhs )
  {
    if( this != &rhs )
    {
      Variable::CopyStuff(rhs);
      CopyStuff(rhs);
    }
    return *this;
  }

  NumericData &GetData()
  { return data_; }

  void SetData( NumericData const &data )
  { data_ = data; }

  // scalar variable
  static NumericVariable *PutVariable( ExString, double, ExString const &, bool =true );

  // vector variable
  static NumericVariable *PutVariable( ExString, std::vector<double> const &, int,
                                       ExString const &, bool =true );

  // matrix variable
  static NumericVariable *PutVariable( ExString, std::vector<double> &, std::size_t,
                                       std::size_t, ExString const &, bool =true );

  static void GetVariable( ExString const &, int &, double &, std::vector<double> &, int * );
  static void GetScalar( ExString &, char const *, double & );
  static void GetVector( ExString const &, char const *, std::vector<double> & );
  static void GetMatrix( ExString const &, char const *, std::vector<double> &, std::size_t &,
                         std::size_t & );

  bool DeletePartial( ExString const &, ExString const & );

  friend std::ostream &operator<<( std::ostream &, NumericVariable const & );

private:
  void CopyStuff( NumericVariable const &rhs )
  { data_ = rhs.data_; }

  static void GetIndices( ExString &, int &, std::vector<double> *, bool * );

  static NumericVariable *GetPreExisting( ExString const &, int, std::vector<double> *,
                                          bool *, int *, int *, int *, int * );

  NumericData data_;
};

#endif
