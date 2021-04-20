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
#ifndef FCN_SIND
#define FCN_SIND

#define FUNCTIONS_LINKAGE_
/*
#if defined BUILDING_FUNCTIONS_DLL_
#define FUNCTIONS_LINKAGE_ __declspec(dllexport)
#else
#define FUNCTIONS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include "ScalarFunction.h"

class FUNCTIONS_LINKAGE_ FCN_sind : public ScalarFunction
{
public:
  static FCN_sind *Definition()
  { return &fcn_sind_; }
  
  void ScalarEval( int j, std::vector<double> &rStack ) const
  { rStack[j] = sin( rStack[j]*M_PI/180. ); }

private:
  FCN_sind() : ScalarFunction( "SIND" )
  {}
  
  static FCN_sind fcn_sind_;
};

#endif

