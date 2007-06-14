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
#include "FCN_rprod.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"

FCN_rprod *FCN_rprod::fcn_rprod_ = 0;

void FCN_rprod::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( ndmEff != 1 )
    throw EExpressionError( wxT("RPROD: argument must be a vector") );
  numberOfArguments_ = 1;
  nLoop = 1;
}

void FCN_rprod::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
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
      throw EExpressionError( wxT("RPROD: first argument workspace is empty") );
    d.assign( w->GetData().begin(), w->GetData().end() );
  }
  std::size_t size = d.size();
  //
  std::vector<double> rprod( size, 0.0 );
  rprod[0] = d[0];
  for( std::size_t i=1; i<size; ++i )rprod[i] = rprod[i-1]*d[i];
  ws->SetNumberOfDimensions(1);
  ws->SetDimMag( 0, size );
  ws->SetData( rprod );
}

// end of file
