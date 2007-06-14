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
#ifndef FCN_RCHAR
#define FCN_RCHAR

#include <vector>

#include "wx/wx.h"
 
#include "FunctionDefinition.h"

class Workspace;

class FCN_rchar : public FunctionDefinition
{
public:
  static FCN_rchar *Instance()
  {
    if( !fcn_rchar_ )fcn_rchar_ = new FCN_rchar();
    return fcn_rchar_;
  }
  
  void TextArrayEval( int, std::vector<Workspace*> &, std::vector<wxString> & ) const;
  
private:
  FCN_rchar() : FunctionDefinition( wxT("RCHAR"), 1, 2, MIXED2CHAR, NUMERIC, CHARACTER )
  {}
  
  static FCN_rchar *fcn_rchar_;
};
 
#endif

