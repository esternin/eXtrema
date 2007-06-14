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
#include "FCN_eqs.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "TextVariable.h"
#include "EExpressionError.h"

FCN_eqs *FCN_eqs::fcn_eqs_ = 0;

void FCN_eqs::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    nLoop = 0;
    numberOfArguments_ = 1;
  }
  else
  {
    ++numberOfArguments_;
    nLoop = ndmEff;
  }
}

void FCN_eqs::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  if( !arg1->IsaTString() || !arg2->IsaTString() )
    throw EExpressionError( wxT("both arguments of EQS function must be character") );
  if( arg1->GetTString() == arg2->GetTString() )
    ws->SetValue( 1.0 );    // true
  else
    ws->SetValue( 0.0 );    // false
}
 
// end of file
