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
#ifndef FCN_CSCH
#define FCN_CSCH

#include "wx/wx.h"

#include "ScalarFunction.h"

class FCN_csch : public ScalarFunction
{
public:
  static FCN_csch *Instance()
  {
    if( !fcn_csch_ )fcn_csch_ = new FCN_csch();
    return fcn_csch_;
  }
  
  void ScalarEval( int, std::vector<double> & ) const;

private:
  FCN_csch() : ScalarFunction( wxT("CSCH") )
  {}
  
  static FCN_csch *fcn_csch_;
};

#endif

