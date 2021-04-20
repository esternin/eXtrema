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
#include "UsefulFunctions.h"

#include "FCN_pfactors.h"

FCN_pfactors FCN_pfactors::fcn_pfactors_;

void FCN_pfactors::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( ndmEff != 0 )
    throw EExpressionError("PFACTORS: argument must be a scalar");
  numberOfArguments_ = 1;
  nLoop = 0;
}

void FCN_pfactors::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg = codes[0];
  double d;
  if( arg->IsaNVariable() )
    d = arg->GetNVarPtr()->GetData().GetScalarValue();
  else if( arg->IsaWorkspace() )
  {
    Workspace *w = arg->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( "PFACTORS: argument workspace is empty" );
    d = w->GetValue();
  }
  else
    d = arg->GetConstantValue();
  int n = abs(static_cast<int>(d));
  if( n == 0 )
    throw EExpressionError( "PFACTORS: zero has no prime factors" );
  int prime = 2;
  int k = 0;
  L10:
  if( n%prime == 0 ) // prime is a factor
  {
    if( k == 100 )
      throw EExpressionError("PFACTORS: maximum number of prime factors (100) exceeded");
    ws->SetData( k++, prime );
    n /= prime;
    if( n == 1 )
    {
      ws->SetNumberOfDimensions(1);
      ws->SetDimMag( 0, k );
      return;
    }
    goto L10;
  }
  prime = UsefulFunctions::NextPrime(prime+1);  // now test the next prime
  goto L10;
}

// end of file
