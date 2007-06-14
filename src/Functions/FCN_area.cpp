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

#include "FCN_area.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"

FCN_area *FCN_area::fcn_area_ = 0;

void FCN_area::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )throw EExpressionError(wxT("AREA: first argument must be a vector"));
    numberOfArguments_ = 1;
    nLoop = 1;
  }
  else
  {
    nLoop = ndmEff;
    ++numberOfArguments_;
    if( numberOfArguments_==2 && ndmEff!=1 )
      throw EExpressionError(wxT("AREA: second argument must be a vector"));
  }
}

void FCN_area::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  std::vector<double> d1, d2;
  if( arg1->IsaNVariable() ) // arg1 is x
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("AREA: first argument workspace is empty") );
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
      throw EExpressionError( wxT("AREA: second argument workspace is empty") );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
  }
  std::size_t size2 = d2.size();
  //
  if( size2 != size1 )
    throw EExpressionError( wxT("AREA: vectors have different lengths") );
  if( size1 < 2 )throw EExpressionError( wxT("AREA: vector length < 2") );
  //
  double temp = d1[size1-1]*d2[0]-d1[0]*d2[size1-1];
  for( std::size_t k1=1; k1<size1; ++k1 )temp += d1[k1-1]*d2[k1]-d1[k1]*d2[k1-1];
  ws->SetValue( 0.5*fabs(temp) );
  ws->SetNumberOfDimensions(0);
  ws->SetDimMag( 0, 0 );
}

// end of file
