/*
Copyright (C) 2009 Joseph L. Chuma

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

#include "FCN_unfold.h"
 
FCN_unfold FCN_unfold::fcn_unfold_;

void FCN_unfold::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 2 )
      throw EExpressionError("argument of UNFOLD function must be a matrix");
    numberOfArguments_ = 1;
    nLoop = ndmEff;
  }
}

void FCN_unfold::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *arg1 = codes[0];
  std::size_t size1, size2;
  int ndm;
  std::vector<double> d1;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    ndm = nd.GetNumberOfDimensions();
    size1 = nd.GetDimMag(0);
    size2 = nd.GetDimMag(1);
  }
  else    // argument 1 must be a workspace
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( "UNFOLD: argument workspace is empty" );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    ndm = w1->GetNumberOfDimensions();
    size1 = w1->GetDimMag(0);
    size2 = w1->GetDimMag(1);
  }
  ws->SetData( d1 );
  ws->SetNumberOfDimensions(1);
  ws->SetDimMag( 0, size1*size2 );
}

// end of file
