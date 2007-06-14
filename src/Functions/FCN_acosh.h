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
#ifndef FCN_ACOSH
#define FCN_ACOSH

#include "wx/wx.h"

#include "ScalarFunction.h"

class FCN_acosh : public ScalarFunction
{
public:
  static FCN_acosh *Instance()
  {
    if( !fcn_acosh_ )fcn_acosh_ = new FCN_acosh();
    return fcn_acosh_;
  }
  
  void ScalarEval( int, std::vector<double> & ) const;

private:
  FCN_acosh() : ScalarFunction( wxT("ACOSH")  )
  {}
  
  static FCN_acosh *fcn_acosh_;
};

#endif

