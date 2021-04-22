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
#include "FCN_smooth.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_smooth *FCN_smooth::fcn_smooth_ = 0;

void FCN_smooth::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("SMOOTH: first argument must be a vector") );
    numberOfArguments_ = 1;
    nLoop = 1;
  }
  else
  {
    nLoop = ndmEff;
    switch (++numberOfArguments_)
    {
      case 2:
        if( ndmEff != 1 )
          throw EExpressionError( wxT("SMOOTH: second argument must be a vector") );
        break;
      case 3:
        if( ndmEff != 1 )
          throw EExpressionError( wxT("SMOOTH: third argument must be a vector") );
        break;
      case 4:
        if( ndmEff != 1 )
          throw EExpressionError( wxT("SMOOTH: fourth argument must be a vector") );
        break;
    }
  }
}

void FCN_smooth::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  ExprCodes *arg3 = codes[2];
  std::vector<double> d1, d2, d3;
  if( arg1->IsaNVariable() ) // arg1 is x
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("SMOOTH: first argument workspace is empty") );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
  }
  std::size_t size1 = d1.size();
  if( arg2->IsaNVariable() ) // arg2 is y
  {
    NumericData nd( arg2->GetNVarPtr()->GetData() );
    d2.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w2 = arg2->GetWorkspacePtr();
    if( w2->IsEmpty() )
      throw EExpressionError( wxT("SMOOTH: second argument workspace is empty") );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
  }
  std::size_t size2 = d2.size();
  //
  if( size2 != size1 )
    throw EExpressionError( wxT("SMOOTH: vectors have different lengths") );
  if( size1 < 2 )
    throw EExpressionError( wxT("SMOOTH: independent vector length < 2") );
  //
  if( arg3->IsaNVariable() ) // arg3 is xout
  {
    NumericData nd( arg3->GetNVarPtr()->GetData() );
    d3.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w3 = arg3->GetWorkspacePtr();
    if( w3->IsEmpty() )
      throw EExpressionError( wxT("SMOOTH: third argument workspace is empty") );
    d3.assign( w3->GetData().begin(), w3->GetData().end() );
  }
  std::size_t size3 = d3.size();
  //
  for( std::size_t i=1; i<size1; ++i )
  {
    if( d1[i] <= d1[i-1] )
      throw EExpressionError( wxT("SMOOTH: independent vector not monotonically increasing") );
  }
  for( std::size_t i=1; i<size3; ++i )
  {
    if( d3[i] <= d3[i-1] )
      throw EExpressionError( wxT("SMOOTH: output independent vector not monotonically increasing") );
  }
  if( d3[0]<d1[0] || d3[size3-1]>d1[size1-1] )
    throw EExpressionError( wxT("SMOOTH: output independent vector has values outside range of input independent vector") );
  //
  std::vector<double> d4;
  if( numberOfArguments_ == 4 ) // arg4 is the weights
  {
    ExprCodes *arg4 = codes[3];
    if( arg4->IsaNVariable() )
    {
      NumericData nd( arg4->GetNVarPtr()->GetData() );
      d4.assign( nd.GetData().begin(), nd.GetData().end() );
    }
    else
    {
      Workspace *w4 = arg4->GetWorkspacePtr();
      if( w4->IsEmpty() )
        throw EExpressionError( wxT("SMOOTH: fourth argument workspace is empty") );
      d4.assign( w4->GetData().begin(), w4->GetData().end() );
    }
    if( d4.size() != size1 )
      throw EExpressionError( wxT("SMOOTH: vectors have different lengths") );
  }
  else
  {
    d4.insert( d4.begin(), size1, 1.0 );
  }
  std::vector<double> yy( size3, 0.0 );
  std::vector<double> yderiv( size3, 0.0 );
  UsefulFunctions::SplineSmooth( d1, d2, d3, yy, yderiv, d4 );
  ws->SetDimMag( 0, yy.size() );
  ws->SetNumberOfDimensions(1);
  ws->SetData( yy );
}

// end of file
