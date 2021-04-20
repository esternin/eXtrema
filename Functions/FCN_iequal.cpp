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

#include "FCN_iequal.h"

FCN_iequal FCN_iequal::fcn_iequal_;

void FCN_iequal::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError("first argument of IEQUAL function must be a vector");
    numberOfArguments_ = 1;
    nLoop = 0;
  }
  else
  {
    if( ndmEff != 0 )
      throw EExpressionError("second argument of IEQUAL function must be a scalar");
    numberOfArguments_ = 2;
  }
}

void FCN_iequal::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  std::vector<double> d1;
  std::size_t size1;
  int startIndex;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    size1 = nd.GetDimMag(0);
    startIndex = 1;
  }
  else    // argument 1 must be a workspace
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( "IEQUAL: first argument workspace is empty" );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    size1 = w1->GetDimMag(0);
    startIndex = 1;
  }
  double value;
  if( arg2->IsaNVariable() )
  {
    value = arg2->GetNVarPtr()->GetData().GetScalarValue();
  }
  else if( arg2->IsaWorkspace() )
  {
    value = arg2->GetWorkspacePtr()->GetValue();
  }
  else
  {
    value = arg2->GetConstantValue();
  }
  ws->SetValue( 0.0 );
  for( std::size_t i=0; i<size1; ++i )
  {
    if( fabs(d1[i]-value) <= numeric_limits<double>::epsilon() )
    {
      ws->SetValue( static_cast<double>(startIndex+i) );
      break;
    }
  }
  ws->SetNumberOfDimensions(0);
}

// end of file
