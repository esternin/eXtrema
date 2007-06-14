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
#include "FCN_prob.h"
#include "UsefulFunctions.h"
#include "EExpressionError.h"

FCN_prob *FCN_prob::fcn_prob_ = 0;

void FCN_prob::ScalarEval( int j, std::vector<double> &rStack ) const
{
  int n = static_cast<int>(rStack[j+1]);
  try
  {
    rStack[j] = UsefulFunctions::Chisq( rStack[j], n );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  rStack.pop_back();
}

