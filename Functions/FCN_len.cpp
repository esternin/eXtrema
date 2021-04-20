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

#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"

#include "FCN_len.h"
 
FCN_len FCN_len::fcn_len_;

void FCN_len::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError("argument of LEN function must be a vector");
    numberOfArguments_ = 1;
    nLoop = 0;
  }
}

void FCN_len::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  std::size_t size1;
  arg1->IsaNVariable() ? size1=arg1->GetNVarPtr()->GetData().GetDimMag(0) :
                         size1=arg1->GetWorkspacePtr()->GetDimMag(0);
  ws->SetValue( static_cast<double>(size1) );
  ws->SetNumberOfDimensions(0);
}

// end of file
