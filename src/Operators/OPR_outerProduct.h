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
#ifndef OPR_OUTERPRODUCT
#define OPR_OUTERPRODUCT
 
#include "ArrayOperator.h"

class OPR_outerProduct : public ArrayOperator
{
public:
  static OPR_outerProduct *Instance()
  {
    if( !opr_outerProduct_ )opr_outerProduct_ = new OPR_outerProduct();
    return opr_outerProduct_;
  }
  
  void ProcessOperand( bool, int, ExprCodes *,
                       std::deque<ExprCodes*> &, int &, int & );
  void CalcOperand( bool, int, int *, Workspace * );
  void ArrayEval( Workspace * );
  
private:
  OPR_outerProduct() : ArrayOperator( wxT("><"), 0, 8 )
  {}
  
  static OPR_outerProduct *opr_outerProduct_;
  unsigned short int firstOperandDimension_;  
};

#endif
