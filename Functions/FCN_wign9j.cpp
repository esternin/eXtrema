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
#include "FCN_wign9j.h"

FCN_wign9j FCN_wign9j::fcn_wign9j_;

void FCN_wign9j::ScalarEval( int j, std::vector<double> &rStack ) const
{
  try
  {
    rStack[j] = UsefulFunctions::Wigner9JSymbol( rStack[j], rStack[j+1], rStack[j+2],
                                                 rStack[j+3], rStack[j+4], rStack[j+5],
                                                 rStack[j+6], rStack[j+7], rStack[j+8] );

  }
  catch (EExpressionError &e)
  {
    throw;
  }
  rStack.pop_back();
  rStack.pop_back();
  rStack.pop_back();
  rStack.pop_back();
  rStack.pop_back();
  rStack.pop_back();
  rStack.pop_back();
  rStack.pop_back();
}

