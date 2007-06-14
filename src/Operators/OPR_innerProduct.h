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
#ifndef OPR_INNERPRODUCT
#define OPR_INNERPRODUCT

#include "ArrayOperator.h"

class OPR_innerProduct : public ArrayOperator
{
public:
  static OPR_innerProduct *Instance()
  {
    if( !opr_innerProduct_ )opr_innerProduct_ = new OPR_innerProduct();
    return opr_innerProduct_;
  }
  
  void ProcessOperand( bool, int, ExprCodes *,
                       std::deque<ExprCodes*> &, int &, int & );
  void CalcOperand( bool, int, int *, Workspace * );
  void ArrayEval( Workspace * );
  
private:
  OPR_innerProduct() : ArrayOperator( wxT("<>"), 0, 8 )
  {}
  
  static OPR_innerProduct *opr_innerProduct_;
  unsigned short int firstOperandDimension_;
};

#endif
