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

#include "ExString.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

#include "FCN_interpolate.h"

FCN_interpolate FCN_interpolate::fcn_interpolate_;

void FCN_interpolate::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError("INTERPOLATE: first argument must be a vector");
    numberOfArguments_ = 1;
    nLoop = 1;
  }
  else
  {
    nLoop = ndmEff;
    if( ++numberOfArguments_ == 2 )
    {
      if( ndmEff != 1 )
        throw EExpressionError("INTERPOLATE: second argument must be a vector");
    }
    else if( numberOfArguments_ == 3 )
    {
      if( ndmEff != 1 )
        throw EExpressionError("INTERPOLATE: third argument must be a vector");
    }
  }
}

void FCN_interpolate::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  ExprCodes *arg3 = codes[2];
  ExprCodes *arg4 = 0;
  if( codes.size() > 4 )arg4 = codes[3];
  std::vector<double> d1, d2, d3;
  ExString s4;
  if( arg1->IsaNVariable() ) // arg1 is x
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )throw EExpressionError( "INTERPOLATE: first argument workspace is empty" );
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
      throw EExpressionError( "INTERPOLATE: second argument workspace is empty" );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
  }
  std::size_t size2 = d2.size();
  //
  if( size2 != size1 )
    throw EExpressionError("INTERPOLATE: vectors have different lengths");
  if( size1 < 2 )
    throw EExpressionError("INTERPOLATE: independent vector length < 2");
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
      throw EExpressionError( "INTERPOLATE: third argument workspace is empty" );
    d3.assign( w3->GetData().begin(), w3->GetData().end() );
  }
  std::size_t size3 = d3.size();
  //
  if( !arg4 ) // arg4 is a string determining the type of interpolation
  {
    s4 = "SPLINE";
  }
  else
  {
    if( arg4->IsaWorkspace() )
      s4 = arg4->GetWorkspacePtr()->GetFinalString();
    else if( arg4->IsaTString() )
      s4 = arg4->GetTString();
    else
      throw EExpressionError("INTERPOLATE: fourth argument must be character");
    s4.ToUpper();
  }
  for( std::size_t i=1; i<size1; ++i )
  {
    if( fabs(d1[i]-d1[i-1]) < std::numeric_limits<double>::epsilon() )
      throw EExpressionError(
        "INTERPOLATE: independent vector must be strictly monotonically increasing" );
  }
  if( s4 == "LINEAR" )
  {
    for( std::size_t i=1; i<size3; ++i )
    {
      if( fabs(d3[i]-d3[i-1]) < numeric_limits<double>::epsilon() )
        throw EExpressionError(
          "INTERPOLATE: output independent vector must be strictly monotonically increasing");
    }
  }
  else
  {
    if( s4 == "FC" && size1<3 )
      throw EExpressionError("INTERPOLATE: independent vector length < 3");
    for( std::size_t i=1; i<size3; ++i )
    {
      if( d3[i] < d3[i-1] )
        throw EExpressionError(
          "INTERPOLATE: output independent vector must be monotonically increasing" );
    }
  }
  std::vector<double> yy( size3, 0.0 );
  if( s4 == "SPLINE" )
  {
    UsefulFunctions::SplineInterpolate( d1, d2, d3, yy );
  }
  else if( s4 == "FC" )
  {
    UsefulFunctions::FritchCarlsonInterpolate( d1, d2, d3, yy, 0 );
  }
  else if( s4 == "LINEAR" )
  {
    UsefulFunctions::LinearInterpolate( d1, d2, d3, yy );
  }
  else if( s4 == "LAGRANGE" )
  {
    UsefulFunctions::LagrangeInterpolate( d1, d2, d3, yy );
  }
  else throw EExpressionError("INTERPOLATE: invalid fourth parameter");
  //
  ws->SetDimMag( 0, yy.size() );
  ws->SetNumberOfDimensions(1);
  ws->SetData( yy );
}

// end of file
