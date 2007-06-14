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
#ifndef OPR_LT
#define OPR_LT

#include "ScalarOperator.h"

class OPR_lt : public ScalarOperator
{
public:
  static OPR_lt *Instance()
  {
    if( !opr_lt_ )opr_lt_ = new OPR_lt();
    return opr_lt_;
  }
  
  double ScalarEval( double a ) const
  { return 0.0; }
  
  double ScalarEval( double a, double b ) const
  { return (a<b) ? 1.0 : 0.0; }

private:
  OPR_lt() : ScalarOperator( wxT("<"), 0, 4 )
  {}
  
  static OPR_lt *opr_lt_;
};

#endif
