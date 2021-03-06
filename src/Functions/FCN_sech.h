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
#ifndef FCN_SECH
#define FCN_SECH

#include "wx/wx.h"

#include "ScalarFunction.h"

class FCN_sech : public ScalarFunction
{
public:
  static FCN_sech *Instance()
  {
    if( !fcn_sech_ )fcn_sech_ = new FCN_sech();
    return fcn_sech_;
  }
  
  void ScalarEval( int, std::vector<double> & ) const;

private:
  FCN_sech() : ScalarFunction( wxT("SECH") )
  {}
  
  static FCN_sech *fcn_sech_;
};

#endif

