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

#include "FCN_deriv.h"

FCN_deriv FCN_deriv::fcn_deriv_;

void FCN_deriv::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError("DERIV: first argument must be a vector");
    numberOfArguments_ = 1;
    nLoop = 1;
  }
  else
  {
    nLoop = ndmEff;
    ++numberOfArguments_;
    if( numberOfArguments_ == 2 )
    {
      if( ndmEff != 1 )
        throw EExpressionError("DERIV: second argument must be a vector");
    }
  }
}

void FCN_deriv::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  ExprCodes *arg3 = NULL;
  if( codes.size() > 3 )arg3 = codes[2];
  std::vector<double> d1, d2;
  ExString s3;
  if( arg1->IsaNVariable() ) // arg1 is x
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( "DERIV: first argument workspace is empty" );
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
      throw EExpressionError( "DERIV: second argument workspace is empty" );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
  }
  std::size_t size2 = d2.size();
  //
  if( size2 != size1 )
    throw EExpressionError("DERIV: vectors have different lengths");
  if( size1 < 2 )
    throw EExpressionError("DERIV: independent vector length < 2");
  //
  if( arg3 == NULL ) // arg3 is a string determining the type of integration
  {
    s3 = ExString("INTERP");
  }
  else
  {
    if( arg3->IsaWorkspace() )
      s3 = arg3->GetWorkspacePtr()->GetFinalString();
    else if( arg3->IsaTString() )
      s3 = arg3->GetTString();
    else
      throw EExpressionError("DERIV: third argument must be character");
    s3.ToUpper();
  }
  for( std::size_t i=1; i<size1; ++i )
  {
    if( fabs(d1[i]-d1[i-1]) < numeric_limits<double>::epsilon() )
      throw EExpressionError(
      "DERIV: independent vector must be strictly monotonically increasing");
  }
  std::vector<double> yderiv( size1, 0.0 );
  if( s3 == "INTERP" )
  {
    std::size_t deriv = 1;
    UsefulFunctions::SplineDerivative( d1, d2, d1, yderiv, deriv );
  }
  else if( s3 == "SMOOTH" )
  {
    std::vector<double> yy( size1, 0.0 );
    std::vector<double> weights( size1, 1.0 );
    UsefulFunctions::SplineSmooth( d1, d2, d1, yy, yderiv, weights );
  }
  else if( s3 == "FC" )
  {
    UsefulFunctions::FritchCarlsonInterpolate( d1, d2, d1, yderiv, 1 );
  }
  else if( s3 == "LAGRANGE3" )
  {
    UsefulFunctions::LagrangeDerivative( d1, d2, d1, yderiv, 3, 1 );
  }
  else if( s3 == "LAGRANGE5" )
  {
    UsefulFunctions::LagrangeDerivative( d1, d2, d1, yderiv, 5, 1 );
  }
  else if( s3 == "LAGRANGE7" )
  {
    UsefulFunctions::LagrangeDerivative( d1, d2, d1, yderiv, 7, 1 );
  }
  else if( s3 == "LAGRANGE9" )
  {
    UsefulFunctions::LagrangeDerivative( d1, d2, d1, yderiv, 9, 1 );
  }
  else throw EExpressionError("DERIV: invalid third parameter");
  //
  ws->SetDimMag( 0, yderiv.size() );
  ws->SetNumberOfDimensions(1);
  ws->SetData( yderiv );
}

// end of file
