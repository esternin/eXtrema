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
#include <cmath>

#include "FCN_wrap.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"

FCN_wrap *FCN_wrap::fcn_wrap_ = 0;

void FCN_wrap::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff!=1 && ndmEff!=2 )
      throw EExpressionError( wxT("first argument of WRAP function must be a vector or matrix") );
    numberOfArguments_ = 1;
    nLoop = 0;
  }
  else
  {
    if( ndmEff != 0 )
      throw EExpressionError( wxT("second argument of WRAP function must be a scalar") );
    numberOfArguments_ = 2;
  }
}

void FCN_wrap::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  std::vector<double> d1;
  std::size_t size1[2];
  int ndm1;
  double step;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    ndm1 = nd.GetNumberOfDimensions();
    for( unsigned short int i=0; i<ndm1; ++i )size1[i] = nd.GetDimMag(i);
  }
  else    // argument 1 must be a workspace
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("WRAP: first argument workspace is empty") );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    ndm1 = w1->GetNumberOfDimensions();
    for( unsigned short int i=0; i<ndm1; ++i )size1[i] = w1->GetDimMag(i);
  }
  if( arg2->IsaNVariable() )
  {
    step = arg2->GetNVarPtr()->GetData().GetScalarValue();
  }
  else if( arg2->IsaWorkspace() )
  {
    step = arg2->GetWorkspacePtr()->GetValue();
  }
  else
  {
    step = arg2->GetConstantValue();
  }
  if( step == 0.0 )
  {
    if( ndm1 == 1 )
    {
      for( std::size_t i=0; i<size1[0]; ++i )ws->SetData( i, d1[i] );
      ws->SetNumberOfDimensions(1);
      ws->SetDimMag( 0, size1[0] );
    }
    else if( ndm1 == 2 )
    {
      for( std::size_t i=0; i<size1[0]*size1[1]; ++i )ws->SetData( i, d1[i] );
      ws->SetNumberOfDimensions(2);
      ws->SetDimMag( 0, size1[0] );
      ws->SetDimMag( 1, size1[1] );
    }
  }
  else
  {
    int istep = static_cast<int>( fabs(step) );
    int num;
    ndm1 == 1 ? num = size1[0] : num = size1[0]*size1[1];
    if( istep > num )istep = num;
    if( step < 0.0 )
    {
      for( int i=0;         i<num-istep; ++i )ws->SetData( i, d1[i+istep] );
      for( int i=num-istep; i<num;       ++i )ws->SetData( i, 0.0 );
    }
    else
    {
      for( int i=num-1; i>=istep; --i )ws->SetData( i, d1[i-istep] );
      for( int i=0;   i<istep;    ++i )ws->SetData( i, 0.0 );
    }
    ws->SetNumberOfDimensions(ndm1);
    ws->SetDimMag( 0, size1[0] );
    if( ndm1 > 1 )ws->SetDimMag( 1, size1[1] );
  }
}

// end of file
