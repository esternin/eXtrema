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
#ifndef OPR_CONCATENATION
#define OPR_CONCATENATION

#include "OperatorDefinition.h"

class OPR_concatenation : public OperatorDefinition
{
public:
  static OPR_concatenation *Instance()
  {
    if( !opr_concatenation_ )opr_concatenation_ = new OPR_concatenation();
    return opr_concatenation_;
  }
  
  void ProcessOperand( bool, int, ExprCodes *, std::deque<ExprCodes*> &,
                       int &, int & );
  void CalcOperand( bool, int, int *, Workspace * );
  void ArrayEval( Workspace * );
  
  void TextScalarEval( int j, std::vector<wxString> &sStack ) const
  { sStack[j] += sStack[j+1]; }
  
private:
  OPR_concatenation()
      : OperatorDefinition(  wxT("//"), 0, 9, OperatorDefinition::CHARACTER )
  {}
  
  static OPR_concatenation *opr_concatenation_;
  unsigned short int firstOperandDimension_;
};

#endif
