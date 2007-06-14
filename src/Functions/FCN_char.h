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
#ifndef FCN_CHAR
#define FCN_CHAR

#include "wx/wx.h"

#include "FunctionDefinition.h"

class Workspace;

class FCN_char : public FunctionDefinition
{
public:
  static FCN_char *Instance()
  {
    if( !fcn_char_ )fcn_char_ = new FCN_char();
    return fcn_char_;
  }

  void TextArrayEval( int const, std::vector<Workspace*> &, std::vector<wxString> & ) const;

private:
  FCN_char() : FunctionDefinition( wxT("CHAR"), 1, 1, MIXED2CHAR )
  {}

  static FCN_char *fcn_char_;
};
 
#endif
 
