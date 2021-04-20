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

#include "FCN_where.h"

FCN_where FCN_where::fcn_where_;

void FCN_where::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError("argument of WHERE function must be a vector");
    numberOfArguments_ = 1;
    nLoop = 0;
  }
}

void FCN_where::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *arg1 = codes[0];
  std::vector<double> d1;
  std::size_t size1;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    size1 = nd.GetDimMag(0);
  }
  else    // argument 1 must be a workspace
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( "WHERE: argument workspace is empty" );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    size1 = w1->GetDimMag(0);
  }
  std::size_t counter = 0;
  for( std::size_t i=0; i<size1; ++i )
  {
    if( d1[i] != 0.0 )ws->SetData( counter++, static_cast<double>(i+1) );
  }
  if( counter > 0 )ws->SetAscending();
  ws->SetDimMag( 0, counter );
  ws->SetNumberOfDimensions(1);
}

// end of file
