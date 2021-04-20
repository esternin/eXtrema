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

#include "FCN_imax.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"

FCN_imax FCN_imax::fcn_imax_;

void FCN_imax::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  numberOfArguments_ = 1;
  nLoop = 0;
}
 
void FCN_imax::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1=codes[0];
  std::vector<double> d1;
  int ndm1, size1;
  //
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    ndm1 = nd.GetNumberOfDimensions();
    if( ndm1 != 1 )
      throw EExpressionError( "IMAX: argument must be a vector" );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    size1 = d1.size();
  }
  else if( arg1->IsaWorkspace() )
  {
    Workspace *w = arg1->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( "IMAX: argument workspace is empty" );
    ndm1 = w->GetNumberOfDimensions();
    if( ndm1 != 1 )
      throw EExpressionError( "IMAX: argument must be a vector" );
    d1.assign( w->GetData().begin(), w->GetData().end() );
    size1 = d1.size();
  }
  else           // argument must be a constant
    throw EExpressionError( "IMAX: argument must be a vector" );
  //
  int imax = 0;
  double xmax = d1[0];
  for( int i=0; i<size1; ++i )
  {
    if( xmax < d1[i] )
    {
      imax = i;
      xmax = d1[i];
    }
  }
  ws->SetValue( static_cast<double>(imax+1) );
  ws->SetNumberOfDimensions(0);
  ws->SetDimMag( 0, 0 );
}

// end of file
