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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "UsefulFunctions.h"
#include "EExpressionError.h"
#include "FCN_plmn.h"

FCN_plmn FCN_plmn::fcn_plmn_;

void FCN_plmn::ScalarEval( int j, std::vector<double> &rStack ) const
{
  try
  {
    rStack[j] = UsefulFunctions::NormalizedLegendre( static_cast<int>(rStack[j]),
                                                     static_cast<int>(rStack[j+1]),
                                                     rStack[j+2] );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  rStack.pop_back();
  rStack.pop_back();
}

