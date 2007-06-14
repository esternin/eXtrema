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
#include "FCN_fold.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"

FCN_fold *FCN_fold::fcn_fold_ = 0;

void FCN_fold::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff!=1 )
      throw EExpressionError( wxT("first argument of FOLD function must be a vector") );
    numberOfArguments_ = 1;
    nLoop = 0;
  }
  else
  {
    if( ndmEff != 0 )
      throw EExpressionError( wxT("second argument of FOLD function must be a scalar") );
    numberOfArguments_ = 2;
  }
}

void FCN_fold::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  std::vector<double> d;
  std::size_t size;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d.assign( nd.GetData().begin(), nd.GetData().end() );
    size = nd.GetDimMag(0);
  }
  else    // argument 1 must be a workspace
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("FOLD: first argument workspace is empty") );
    d.assign( w1->GetData().begin(), w1->GetData().end() );
    size = w1->GetDimMag(0);
  }
  double rows;
  if( arg2->IsaNVariable() )
    rows = arg2->GetNVarPtr()->GetData().GetScalarValue();
  else if( arg2->IsaWorkspace() )
  {
    if( arg2->GetWorkspacePtr()->IsEmpty() )
      throw EExpressionError( wxT("FOLD: second argument workspace is empty") );
    rows = arg2->GetWorkspacePtr()->GetValue();
  }
  else
    rows = arg2->GetConstantValue();
  if( rows == 0.0 )
    throw EExpressionError( wxT("number of rows specified in FOLD function cannot be zero") );
  if( rows < 0.0 )
    throw EExpressionError( wxT("number of rows specified in FOLD function cannot be < 0") );
  std::size_t nrows = static_cast<std::size_t>(rows);
  std::size_t ncols = size/nrows;
  if( nrows*ncols != size )
    throw EExpressionError( wxT("invalid FOLD function row size") );
  std::size_t k3 = 0;
  for( std::size_t k2=0; k2<ncols; ++k2 )
  {
    for( std::size_t k1=0; k1<nrows; ++k1 )ws->SetData( k1+k2*nrows, d[k3++] );
  }
  ws->SetNumberOfDimensions(2);
  ws->SetDimMag( 0, nrows );
  ws->SetDimMag( 1, ncols );
}

// end of file
