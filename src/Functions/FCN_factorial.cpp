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
#include "FCN_factorial.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_factorial *FCN_factorial::fcn_factorial_ = 0;

void FCN_factorial::ScalarEval( int j, std::vector<double> &rStack ) const
{
  try
  {
    int n = static_cast<int>( rStack[j] );
    rStack[j] = UsefulFunctions::Factorial( n );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
}

