/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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
#ifndef FCN_VAR
#define FCN_VAR

#include "wx/wx.h"

#include "ArrayFunction.h"

class Workspace;

class FCN_var : public ArrayFunction
{
public:
  static FCN_var *Instance()
  {
    if( !fcn_var_ )fcn_var_ = new FCN_var();
    return fcn_var_;
  }
  
  void ProcessArgument( bool, int, int & );
  void ArrayEval( Workspace * );
  
private:
  FCN_var() : ArrayFunction( wxT("VAR"), 1, 2 )
  {}
  
  static FCN_var *fcn_var_;
};

#endif
 
