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

#include "FCN_prod.h"

FCN_prod FCN_prod::fcn_prod_;

void FCN_prod::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( ndmEff != 1 )
    throw EExpressionError("PROD: argument must be a vector");
  numberOfArguments_ = 1;
  nLoop = 1;
}

void FCN_prod::ArrayEval( Workspace *ws )
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
      throw EExpressionError( "PROD: first argument workspace is empty" );
    d.assign( w->GetData().begin(), w->GetData().end() );
  }
  std::size_t size = d.size();
  //
  double prod = 1.0;
  for( std::size_t i=0; i<size; ++i )prod *= d[i];
  ws->SetNumberOfDimensions(0);
  ws->SetDimMag( 0, 0 );
  ws->SetValue( prod );
}

// end of file
