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

#include <ostream>
#include <vector>
#include <deque>

#include "wx/wx.h"

#include "Variable.h"
#include "TextData.h"

class NumericData;

class TextVariable : public Variable
{
public:
  TextVariable( wxString const & );
  
  ~TextVariable()
  {}
  
  TextVariable( TextVariable const & );
  TextVariable &operator=( TextVariable const & );

  TextData &GetData();
  void SetData( TextData const & );
  
  // scalar text variable
  static TextVariable *PutVariable( wxString, wxString const &,
                                    wxString const &, bool =true );

  // array text variable
  static TextVariable *PutVariable( wxString, std::vector<wxString> const &,
                                    wxString const &, bool =true );

  static void GetVariable( wxString const &, bool, wxString & );

  wxString GetString( NumericData const *, NumericData const * );

  bool DeletePartial( wxString &, wxString const & );

  friend std::ostream &operator<<( std::ostream &, TextVariable const & );

private:
  static void GetIndices( wxString &, int &, std::vector<double> *, bool * );

  TextData data_;
};

#endif
