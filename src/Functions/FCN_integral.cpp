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
#include "FCN_integral.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_integral *FCN_integral::fcn_integral_ = 0;

void FCN_integral::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("INTEGRAL: first argument must be a vector") );
    numberOfArguments_ = 1;
    nLoop = 1;
  }
  else
  {
    nLoop = ndmEff;
    ++numberOfArguments_;
    if( numberOfArguments_==2 && ndmEff!=1 )
      throw EExpressionError( wxT("INTEGRAL: second argument must be a vector") );
  }
}

void FCN_integral::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  ExprCodes *arg3 = NULL;
  if( codes.size() > 3 )arg3 = codes[2];
  std::vector<double> d1, d2;
  wxString s3;
  if( arg1->IsaNVariable() ) // arg1 is x
  {
    NumericData &nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("INTEGRAL: first argument workspace is empty") );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
  }
  std::size_t size1 = d1.size();
  if( arg2->IsaNVariable() ) // arg2 is y
  {
    NumericData &nd( arg2->GetNVarPtr()->GetData() );
    d2.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w2 = arg2->GetWorkspacePtr();
    if( w2->IsEmpty() )throw EExpressionError( wxT("INTEGRAL: second argument workspace is empty") );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
  }
  std::size_t size2 = d2.size();
  //
  if( size2 != size1 )
    throw EExpressionError( wxT("INTEGRAL: vectors have different lengths") );
  if( size1 < 2 )
    throw EExpressionError( wxT("INTEGRAL: independent vector length < 2") );
  //
  if( !arg3 ) // arg3 is a string determining the type of integration
  {
    s3 = wxT("INTERP");
  }
  else
  {
    if( arg3->IsaWorkspace() )
      s3 = arg3->GetWorkspacePtr()->GetFinalString();
    else if( arg3->IsaTString() )
      s3 = arg3->GetTString();
    else
      throw EExpressionError( wxT("INTEGRAL: third argument must be character") );
    s3.UpperCase();
  }
  for( std::size_t i=1; i<size1; ++i )
  {
    if( d1[i] <= d1[i-1] )throw EExpressionError(
      wxT("INTEGRAL: independent vector must be strictly monotonically increasing") );
  }
  std::vector<double> yy( size1, 0.0 );
  if( s3 == wxT("INTERP") )
  {
    UsefulFunctions::SplineIntegral( d1, d2, d1, yy );
  }
  else throw EExpressionError( wxT("INTEGRAL: invalid third parameter") );
  //
  ws->SetDimMag( 0, yy.size() );
  ws->SetNumberOfDimensions(1);
  ws->SetData( yy );
}

// end of file
