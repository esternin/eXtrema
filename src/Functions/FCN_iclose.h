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
#ifndef FCN_ICLOSE
#define FCN_ICLOSE

#include "wx/wx.h"
 
#include "ArrayFunction.h"

class Workspace;

class FCN_iclose : public ArrayFunction
{
public:
  static FCN_iclose *Instance()
  {
    if( !fcn_iclose_ )fcn_iclose_ = new FCN_iclose();
    return fcn_iclose_;
  }
  
  void ProcessArgument( bool, int, int & );
  void ArrayEval( Workspace * );
  
private:
  FCN_iclose() : ArrayFunction( wxT("ICLOSE"), 2, 2 )
  {}
  
  static FCN_iclose *fcn_iclose_;
};

#endif
 
