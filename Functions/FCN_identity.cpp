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

#include "FCN_identity.h"

FCN_identity FCN_identity::fcn_identity_;

void FCN_identity::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 0 )
      throw EExpressionError("IDENTITY: argument must be a scalar");
    numberOfArguments_ = 1;
    nLoop = 2;
  }
}

void FCN_identity::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  double value;
  if( arg1->IsaNVariable() )
    value = arg1->GetNVarPtr()->GetData().GetScalarValue();
  else if( arg1->IsaWorkspace() )
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )throw EExpressionError( "IDENTITY: argument workspace is empty" );
    value = w1->GetValue();
  }
  else
    value = arg1->GetConstantValue();
  //
  int n = static_cast<int>(value);
  if( n < 2 )throw EExpressionError("IDENTITY: specified matrix dimension < 2");
  //
  std::vector<double> d( n*n, 0.0 );
  for( int j=0; j<n; ++j )
  {
    for( int i=0; i<n; ++i )
    {
      if( i == j )d[i+j*n] = 1.0;
    }
  }
  ws->SetData( d );
  ws->SetNumberOfDimensions(2);
  ws->SetDimMag( 0, n );
  ws->SetDimMag( 1, n );
}

// end of file
