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
#ifndef FCN_ACSCD
#define FCN_ACSCD

#define FUNCTIONS_LINKAGE_
/*
#if defined BUILDING_FUNCTIONS_DLL_
#define FUNCTIONS_LINKAGE_ __declspec(dllexport)
#else
#define FUNCTIONS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include "EExpressionError.h"
#include "ScalarFunction.h"

class FUNCTIONS_LINKAGE_ FCN_acscd : public ScalarFunction
{
public:
  static FCN_acscd *Definition()
  { return &fcn_acscd_; }
  
  void ScalarEval( int j, std::vector<double> &rStack ) const
  {
    if( rStack[j] == 0.0 )throw EExpressionError("ACSCD: argument = 0");
    rStack[j] = asin( 1.0/rStack[j] )*180./M_PI;
  }

private:
  FCN_acscd() : ScalarFunction( "ACSCD" )
  {}
  
  static FCN_acscd fcn_acscd_;
};

#endif

