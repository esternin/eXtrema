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
#ifndef FCN_EXPAND
#define FCN_EXPAND

#include "wx/wx.h"

#include "ScalarFunction.h"

class FCN_expand : public ScalarFunction
{
public:
  static FCN_expand *Instance()
  {
    if( !fcn_expand_ )fcn_expand_ = new FCN_expand();
    return fcn_expand_;
  }
  
  void TextScalarEval( int, std::vector<wxString> & ) const;

private:
  FCN_expand() : ScalarFunction( wxT("EXPAND"), 1, 1, CHAR2CHAR, CHARACTER )
  {}

  static FCN_expand *fcn_expand_;

  bool FillOut( wxString & ) const;
};

#endif
 
