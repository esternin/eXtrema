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
#ifndef FCN_TINA
#define FCN_TINA

#include "wx/wx.h"

#include "ScalarFunction.h"

class FCN_tina : public ScalarFunction
{
public:
  static FCN_tina *Instance()
  {
    if( !fcn_tina_ )fcn_tina_ = new FCN_tina();
    return fcn_tina_;
  }
  
  void ScalarEval( int, std::vector<double> & ) const;

private:
  FCN_tina() : ScalarFunction( wxT("TINA"), 4, 4 )
  {}

  static FCN_tina *fcn_tina_;
};

#endif

