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
#include <cmath>
#include <limits>

#include "FCN_exp.h"
#include "EExpressionError.h"

FCN_exp *FCN_exp::fcn_exp_ = 0;

void FCN_exp::ScalarEval( int j, std::vector<double> &rStack ) const
{
  if( rStack[j] > log(std::numeric_limits<double>::max()) )
    throw EExpressionError( wxT("max value for exp exceeded") );
  rStack[j] = exp( rStack[j] );
}

