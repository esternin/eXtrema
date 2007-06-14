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
#include <cmath>

#include "FCN_atanh.h"
#include "EExpressionError.h"

FCN_atanh *FCN_atanh::fcn_atanh_ = 0;

void FCN_atanh::ScalarEval( int j, std::vector<double> &rStack ) const
{
  if( (rStack[j]<=-1.0) || (rStack[j]>=1.0) )
    throw EExpressionError( wxT("ATANH: argument must be in the range (-1,1)") );
  rStack[j] = 0.5 * log( (1.0 + rStack[j])/(1.0 - rStack[j]) );
}


