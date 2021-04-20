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

#include "FCN_rsum.h"

FCN_rsum FCN_rsum::fcn_rsum_;

void FCN_rsum::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( ndmEff != 1 )
    throw EExpressionError("RSUM: argument must be a vector");
  numberOfArguments_ = 1;
  nLoop = 1;
}

void FCN_rsum::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *arg = codes[0];
  std::vector<double> d;
  if( arg->IsaNVariable() ) // arg is x
  {
    NumericData nd( arg->GetNVarPtr()->GetData() );
    d.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w = arg->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( "RSUM: first argument workspace is empty" );
    d.assign( w->GetData().begin(), w->GetData().end() );
  }
  std::size_t size = d.size();
  //
  std::vector<double> rsum( size, 0.0 );
  rsum[0] = d[0];
  for( std::size_t i=1; i<size; ++i )rsum[i] = rsum[i-1]+d[i];
  ws->SetNumberOfDimensions(1);
  ws->SetDimMag( 0, size );
  ws->SetData( rsum );
}

// end of file
