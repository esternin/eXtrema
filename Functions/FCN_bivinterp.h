/*
Copyright (C) 2006 Joseph L. Chuma, TRIUMF

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
#ifndef FCN_BIVARIATEINTERPOLATE
#define FCN_BIVARIATEINTERPOLATE

#include "ArrayFunction.h"

class Workspace;

class FCN_bivariateInterpolate : public ArrayFunction
{
public:
  static FCN_bivariateInterpolate *Definition()
  { return &fcn_bivariateInterpolate_; }
  
  void ProcessArgument( bool, int, int & );
  void ArrayEval( Workspace * );
  
private:
  FCN_bivariateInterpolate() : ArrayFunction( "BIVINTERP", 5, 5 )
  {}
  
  static FCN_bivariateInterpolate fcn_bivariateInterpolate_;
};

#endif
