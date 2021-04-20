/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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

#include "FCN_median.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_median FCN_median::fcn_median_;

void FCN_median::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  numberOfArguments_ = 1;
  nLoop = 0;
}
 
void FCN_median::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg=codes[0];
  std::vector<double> d;
  int ndm, size;
  //
  if( arg->IsaNVariable() )
  {
    NumericData nd( arg->GetNVarPtr()->GetData() );
    ndm = nd.GetNumberOfDimensions();
    if( ndm != 1 )
      throw EExpressionError( "MEDIAN: argument must be a vector" );
    d.assign( nd.GetData().begin(), nd.GetData().end() );
    size = d.size();
  }
  else if( arg->IsaWorkspace() )
  {
    Workspace *w = arg->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( "MEDIAN: argument workspace is empty" );
    ndm = w->GetNumberOfDimensions();
    if( ndm != 1 )
      throw EExpressionError( "MEDIAN: argument must be a vector" );
    d.assign( w->GetData().begin(), w->GetData().end() );
    size = d.size();
  }
  else           // first argument must be a constant
    throw EExpressionError( "MEDIAN: argument must be a vector" );
  //
  ws->SetValue( UsefulFunctions::Median(d) );
  ws->SetNumberOfDimensions(0);
  ws->SetDimMag( 0, 0 );
}

// end of file
