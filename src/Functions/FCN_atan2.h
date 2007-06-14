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
#ifndef FCN_ATAN2
#define FCN_ATAN2

#include "wx/wx.h"

#include "ScalarFunction.h"

class FCN_atan2 : public ScalarFunction
{
public:
  static FCN_atan2 *Instance()
  {
    if( !fcn_atan2_ )fcn_atan2_ = new FCN_atan2();
    return fcn_atan2_;
  }
  
  void ScalarEval( int, std::vector<double> & ) const;

private:
  FCN_atan2() : ScalarFunction( wxT("ATAN2"), 2, 2 )
  {}
  
  static FCN_atan2 *fcn_atan2_;
};

#endif

