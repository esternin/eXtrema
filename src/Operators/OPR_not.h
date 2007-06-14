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
#ifndef OPR_NOT
#define OPR_NOT

#include "ScalarOperator.h"

class OPR_not : public ScalarOperator
{
public:
  static OPR_not *Instance()
  {
    if( !opr_not_ )opr_not_ = new OPR_not();
    return opr_not_;
  }
  
  double ScalarEval( double a, double b ) const
  { return 0.0; }
  
  double ScalarEval( double a ) const
  { return (a==0.0) ? 1.0 : 0.0; }

private:
  OPR_not() : ScalarOperator( wxT("~"), 3, 0 )
  {}
  
  static OPR_not *opr_not_;
};

#endif
