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
#ifndef FCN_DIGAMMA
#define FCN_DIGAMMA

#include "wx/wx.h"

#include "ScalarFunction.h"

class FCN_digamma : public ScalarFunction
{
public:
  static FCN_digamma *Instance()
  {
    if( !fcn_digamma_ )fcn_digamma_ = new FCN_digamma();
    return fcn_digamma_;
  }
  
  void ScalarEval( int, std::vector<double> & ) const;

private:
  FCN_digamma() : ScalarFunction( wxT("DIGAMMA") )
  {}

  static FCN_digamma *fcn_digamma_;
};
 
#endif

