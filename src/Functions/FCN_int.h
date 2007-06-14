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
#ifndef FCN_INT
#define FCN_INT

#include "wx/wx.h"

#include "ScalarFunction.h"

class FCN_int : public ScalarFunction
{
public:
  static FCN_int *Instance()
  {
    if( !fcn_int_ )fcn_int_ = new FCN_int();
    return fcn_int_;
  }
  
  void ScalarEval( int j, std::vector<double> &rStack ) const
  { rStack[j] = static_cast<double>(static_cast<int>(rStack[j])); }

private:
  FCN_int() : ScalarFunction( wxT("INT") )
  {}
  
  static FCN_int *fcn_int_;
};

#endif

