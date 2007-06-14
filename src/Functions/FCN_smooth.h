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
#ifndef FCN_SMOOTH
#define FCN_SMOOTH

#include "wx/wx.h"
 
#include "ArrayFunction.h"

class Workspace;

class FCN_smooth : public ArrayFunction
{
public:
  static FCN_smooth *Instance()
  {
    if( !fcn_smooth_ )fcn_smooth_ = new FCN_smooth();
    return fcn_smooth_;
  }
  
  void ProcessArgument( bool, int, int & );
  void ArrayEval( Workspace * );
  
private:
  FCN_smooth() : ArrayFunction( wxT("SMOOTH"), 3, 4 )
  {}
  
  static FCN_smooth *fcn_smooth_;
};

#endif
 
