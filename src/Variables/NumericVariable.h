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

#include <vector>
#include <deque>
#include <ostream>

#include "wx/wx.h"

#include "Variable.h"
#include "NumericData.h"

class NVariableTable;

class NumericVariable : public Variable
{
public:
  NumericVariable( wxString const & );

  ~NumericVariable()
  {}

  NumericVariable( NumericVariable const & );
  NumericVariable &operator=( NumericVariable const & );

  NumericData &GetData();
  void SetData( NumericData const & );

  // scalar variable
  static NumericVariable *PutVariable( wxString, double, wxString const &, bool =true );

  // vector variable
  static NumericVariable *PutVariable( wxString, std::vector<double> const &, int,
                                       wxString const &, bool =true );

  // matrix variable
  static NumericVariable *PutVariable( wxString, std::vector<double> &, std::size_t,
                                       std::size_t, wxString const &, bool =true );

  static void GetVariable( wxString const &, int &, double &, std::vector<double> &, int * );
  static void GetScalar( wxString &, wxChar const *, double & );
  static void GetVector( wxString const &, wxChar const *, std::vector<double> & );
  static void GetMatrix( wxString const &, wxChar const *, std::vector<double> &, std::size_t &,
                         std::size_t & );

  bool DeletePartial( wxString const &, wxString const & );

  friend std::ostream &operator<<( std::ostream &, NumericVariable const & );

private:
  void CopyStuff( NumericVariable const &rhs )
  { data_ = rhs.data_; }

  static void GetIndices( wxString &, int &, std::vector<double> *, bool * );

  static NumericVariable *GetPreExisting( wxString const &, int, std::vector<double> *,
                                          bool *, int *, int *, int *, int * );

  NumericData data_;
};

#endif
