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
#include "EExpressionError.h"

#include "FCN_evaluate.h"

FCN_evaluate FCN_evaluate::fcn_evaluate_;

void FCN_evaluate::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  nLoop = 1;
  numberOfArguments_ = 1;
}

void FCN_evaluate::ArrayEval( Workspace *w )
{
  ExprCodes *ec = w->GetCodes().at(0);
  if( !ec->IsaWorkspace() )throw EExpressionError( "ArrayEval error in EVALUATE function" );
  Workspace *ws = ec->GetWorkspacePtr();
  if( ws->IsEmpty() )
    throw EExpressionError( "EVALUATE: argument workspace is empty" );
  std::size_t ndim = ws->GetNumberOfDimensions();
  w->SetNumberOfDimensions( ndim );
  if( ndim == 0 )
  {
    w->SetValue( ws->GetValue() );
  }
  else
  {
    w->SetData( ws->GetData() );
    for( std::size_t i=0; i<ndim; ++i )w->SetDimMag( i, ws->GetDimMag(i) );
  }
}

// end of file

