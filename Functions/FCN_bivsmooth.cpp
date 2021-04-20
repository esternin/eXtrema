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

#include <stdexcept>

#include "FCN_bivsmooth.h"

#include "ExString.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_bivariateSmooth FCN_bivariateSmooth::fcn_bivariateSmooth_;

void FCN_bivariateSmooth::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError("BIVSMOOTH: first argument must be a vector");
    numberOfArguments_ = 1;
    nLoop = 2;
  }
  else
  {
    nLoop = ndmEff;
    switch ( ++numberOfArguments_ )
    {
      case 2:
        if( ndmEff != 1 )
          throw EExpressionError("BIVSMOOTH: second argument must be a vector");
        break;
      case 3:
        if( ndmEff != 2 )
          throw EExpressionError("BIVSMOOTH: third argument must be a matrix");
        break;
      case 4:
        if( ndmEff != 0 )
          throw EExpressionError("BIVSMOOTH: fourth argument must be a scalar");
        break;
      case 5:
        if( ndmEff != 0 )
          throw EExpressionError("BIVSMOOTH: fifth argument must be a scalar");
        break;
    }
  }
}

void FCN_bivariateSmooth::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  ExprCodes *arg3 = codes[2];
  ExprCodes *arg4 = codes[3];
  ExprCodes *arg5 = codes[4];
  std::vector<double> d1, d2, d3;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w = arg1->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( "BIVSMOOTH: first argument workspace is empty" );
    d1.assign( w->GetData().begin(), w->GetData().end() );
  }
  int size1 = d1.size();
  if( size1 < 2 )
    throw EExpressionError("BIVSMOOTH: independent vector length < 2");
  //
  if( arg2->IsaNVariable() )
  {
    NumericData nd( arg2->GetNVarPtr()->GetData() );
    d2.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w = arg2->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( "BIVSMOOTH: second argument workspace is empty" );
    d2.assign( w->GetData().begin(), w->GetData().end() );
  }
  int size2 = d2.size();
  if( size2 < 2 )
    throw EExpressionError("BIVSMOOTH: dependent vector length < 2");
  //
  int size3a[2];
  if( arg3->IsaNVariable() )
  {
    NumericData nd( arg3->GetNVarPtr()->GetData() );
    d3.assign( nd.GetData().begin(), nd.GetData().end() );
    size3a[0] = nd.GetDimMag(0);
    size3a[1] = nd.GetDimMag(1);
  }
  else
  {
    Workspace *w = arg3->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( "BIVSMOOTH: input matrix argument workspace is empty" );
    d3.assign( w->GetData().begin(), w->GetData().end() );
    size3a[0] = w->GetDimMag(0);
    size3a[1] = w->GetDimMag(1);
  }
  if( size3a[0] != size2 )
    throw EExpressionError("BIVSMOOTH: #rows of input matrix must = dependent vector length");
  if( size3a[1] != size1 )
    throw EExpressionError("BIVSMOOTH: #columns of input matrix must = independent vector length");
  //
  double value4, value5;
  if( arg4->IsaNVariable() )
  {
    value4 = arg4->GetNVarPtr()->GetData().GetScalarValue();
  }
  else if( arg4->IsaWorkspace() )
  {
    Workspace *w = arg4->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( "BIVSMOOTH: fourth argument workspace is empty" );
    value4 = w->GetValue();
  }
  else
    value4 = arg4->GetConstantValue();
  //
  if( arg5->IsaNVariable() )
  {
    value5 = arg5->GetNVarPtr()->GetData().GetScalarValue();
  }
  else if( arg5->IsaWorkspace() )
  {
    Workspace *w = arg5->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( "BIVSMOOTH: fifth argument workspace is empty" );
    value5 = w->GetValue();
  }
  else
    value5 = arg5->GetConstantValue();
  //
  int mx = static_cast<int>(value4+0.5);
  int my = static_cast<int>(value5+0.5);
  int uLen = mx*(size1-1)+1;
  int vLen = my*(size2-1)+1;
  //
  std::vector<double> u( uLen, 0.0 );
  std::vector<double> v( vLen, 0.0 );
  std::vector<double> w( uLen*vLen, 0.0 );
  try
  {
    UsefulFunctions::BivariateSmoothing( d1, d2, d3, mx, my, u, v, w );
  }
  catch ( std::runtime_error &e )
  {
    throw EExpressionError( ExString("BIVSMOOTH: ")+e.what() );
  }
  std::vector<double> ww( (uLen+1)*(vLen+1), 0.0 );
  for( int i=0; i<uLen; ++i )ww[0+(i+1)*(vLen+1)] = u[i];
  for( int j=0; j<vLen; ++j )ww[1+j] = v[j];
  for( int i=0; i<uLen; ++i )
  {
    for( int j=0; j<vLen; ++j )ww[j+1+(i+1)*(vLen+1)] = w[j+i*vLen];
  }
  //
  ws->SetNumberOfDimensions( 2 );
  ws->SetDimMag( 0, vLen+1 );
  ws->SetDimMag( 1, uLen+1 );
  ws->SetData( ww );
}

// end of file
