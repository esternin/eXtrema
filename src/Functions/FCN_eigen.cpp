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
#include "FCN_eigen.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_eigen *FCN_eigen::fcn_eigen_ = 0;

void FCN_eigen::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 2 )
      throw EExpressionError( wxT("argument of EIGEN function must be a matrix") );
    numberOfArguments_ = 1;
    nLoop = 0;
  }
}

void FCN_eigen::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  std::vector<double> d;
  std::size_t rowSize, colSize;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d.assign( nd.GetData().begin(), nd.GetData().end() );
    rowSize = nd.GetDimMag(0);
    colSize = nd.GetDimMag(1);
  }
  else    // argument 1 must be a workspace
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("EIGEN: argument workspace is empty") );
    d.assign( w1->GetData().begin(), w1->GetData().end() );
    rowSize = w1->GetDimMag(0);
    colSize = w1->GetDimMag(1);
  }
  if( rowSize != colSize )
    throw EExpressionError( wxT("EIGEN: argument must be a square matrix") );
  std::size_t n = rowSize;
  for( std::size_t j=1; j<=n; ++j )
  {
    for( std::size_t i=1; i<=n; ++i )
    {
      if( d[i-1+(j-1)*n] != d[j-1+(i-1)*n] )
        throw EExpressionError( wxT("EIGEN: argument must be a symmetric matrix") );
    }
  }
  std::vector<double> temp(n*(n+1),0.0);
  for( std::size_t i=1; i<=n; ++i )
  {
    for( std::size_t j=1; j<=i; ++j )temp[i-1+(j-1)*n] = d[i-1+(j-1)*n];
  }
  try
  {
    UsefulFunctions::Eigen( temp );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  ws->SetData( temp );
  ws->SetNumberOfDimensions(2);
  ws->SetDimMag( 0, n );
  ws->SetDimMag( 1, n+1 );
}

// end of file
