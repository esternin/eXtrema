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
#ifndef OPR_EXPONENTIATION2
#define OPR_EXPONENTIATION2

#include "ScalarOperator.h"

class OPR_exponentiation2 : public ScalarOperator
{
public:
  static OPR_exponentiation2 *Instance()
  {
    if( !opr_exponentiation2_ )opr_exponentiation2_ = new OPR_exponentiation2();
    return opr_exponentiation2_;
  }
  
  double ScalarEval( double a ) const
  { return 0.0; }
  
  double ScalarEval( double, double ) const;

private:
  OPR_exponentiation2() : ScalarOperator( wxT("**"), 0, -7 )
  {}
  
  static OPR_exponentiation2 *opr_exponentiation2_;
};

#endif
