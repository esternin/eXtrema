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
#ifndef FCN_BESY0
#define FCN_BESY0

#include "wx/wx.h"

#include "ScalarFunction.h"

class FCN_besy0 : public ScalarFunction
{
public:
  static FCN_besy0 *Instance()
  {
    if( !fcn_besy0_ )fcn_besy0_ = new FCN_besy0();
    return fcn_besy0_;
  }
  
  void ScalarEval( int, std::vector<double> & ) const;

private:
  FCN_besy0() : ScalarFunction( wxT("BESY0") )
  {}
  
  static FCN_besy0 *fcn_besy0_;
};

#endif

