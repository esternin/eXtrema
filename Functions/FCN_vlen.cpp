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

#include "FCN_vlen.h"
 
FCN_vlen FCN_vlen::fcn_vlen_;

void FCN_vlen::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff!=1 && ndmEff!=2 )
      throw EExpressionError("argument of VLEN function must be a vector or a matrix");
    numberOfArguments_ = 1;
    nLoop = ndmEff;
  }
}

void FCN_vlen::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *arg1 = codes[0];
  std::size_t size1, size2;
  int ndm;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    ndm = nd.GetNumberOfDimensions();
    size1 = nd.GetDimMag(0);
    if( ndm == 2 )size2 = nd.GetDimMag(1);
  }
  else    // argument 1 must be a workspace
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( "VLEN: first argument workspace is empty" );
    ndm = w1->GetNumberOfDimensions();
    size1 = w1->GetDimMag(0);
    if( ndm == 2 )size2 = w1->GetDimMag(1);
  }
  ws->SetData( 0, static_cast<double>(size1) );
  if( ndm == 2 )ws->SetData( 1, static_cast<double>(size2) );
  ws->SetNumberOfDimensions(1);
  ws->SetDimMag( 0, ndm );
}

// end of file
