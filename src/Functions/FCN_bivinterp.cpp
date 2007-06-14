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
#include <stdexcept>

#include "FCN_bivinterp.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_bivariateInterpolate *FCN_bivariateInterpolate::fcn_bivariateInterpolate_ = 0;

void FCN_bivariateInterpolate::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("BIVINTERP: first argument must be a vector") );
    numberOfArguments_ = 1;
    nLoop = 1;
  }
  else
  {
    nLoop = ndmEff;
    switch ( ++numberOfArguments_ )
    {
      case 2:
        if( ndmEff != 1 )
          throw EExpressionError( wxT("BIVINTERP: second argument must be a vector") );
        break;
      case 3:
        if( ndmEff != 2 )
          throw EExpressionError( wxT("BIVINTERP: third argument must be a matrix") );
        break;
      case 4:
        if( ndmEff != 1 )
          throw EExpressionError( wxT("BIVINTERP: fourth argument must be a vector") );
        break;
      case 5:
        if( ndmEff != 1 )
          throw EExpressionError( wxT("BIVINTERP: fifth argument must be a vector") );
        break;
    }
  }
}

void FCN_bivariateInterpolate::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  ExprCodes *arg3 = codes[2];
  ExprCodes *arg4 = codes[3];
  ExprCodes *arg5 = codes[4];
  std::vector<double> d1, d2, d3, d4, d5;
  if( arg1->IsaNVariable() ) // arg1 is x
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w = arg1->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( wxT("BIVINTERP: first argument workspace is empty" ) );
    d1.assign( w->GetData().begin(), w->GetData().end() );
  }
  int size1 = d1.size();
  if( size1 < 2 )
    throw EExpressionError( wxT("BIVINTERP: independent vector length < 2") );
  //
  if( arg2->IsaNVariable() ) // arg2 is y
  {
    NumericData nd( arg2->GetNVarPtr()->GetData() );
    d2.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w = arg2->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( wxT("BIVINTERP: second argument workspace is empty") );
    d2.assign( w->GetData().begin(), w->GetData().end() );
  }
  int size2 = d2.size();
  if( size2 < 2 )
    throw EExpressionError( wxT("BIVINTERP: dependent vector length < 2") );
  //
  int size3a[2];
  if( arg3->IsaNVariable() ) // arg3 is xout
  {
    NumericData nd( arg3->GetNVarPtr()->GetData() );
    d3.assign( nd.GetData().begin(), nd.GetData().end() );
    size3a[0] = nd.GetDimMag(0);
    size3a[1] = nd.GetDimMag(1);
  }
  else
  {
    Workspace *w = arg3->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( wxT("BIVINTERP: input matrix argument workspace is empty") );
    d3.assign( w->GetData().begin(), w->GetData().end() );
    size3a[0] = w->GetDimMag(0);
    size3a[1] = w->GetDimMag(1);
  }
  if( size3a[0] != size2 )
    throw EExpressionError( wxT("BIVINTERP: #rows of input matrix must = dependent vector length") );
  if( size3a[1] != size1 )
    throw EExpressionError( wxT("BIVINTERP: #columns of input matrix must = independent vector length") );
  //
  if( arg4->IsaNVariable() )
  {
    NumericData nd( arg4->GetNVarPtr()->GetData() );
    d4.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w = arg4->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( wxT("BIVINTERP: fourth argument workspace is empty") );
    d4.assign( w->GetData().begin(), w->GetData().end() );
  }
  int size4 = d4.size();
  if( size4 < 1 )
    throw EExpressionError( wxT("BIVINTERP: output independent vector is empty") );
  //
  if( arg5->IsaNVariable() )
  {
    NumericData nd( arg5->GetNVarPtr()->GetData() );
    d5.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w = arg5->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( wxT("BIVINTERP: fifth argument workspace is empty") );
    d5.assign( w->GetData().begin(), w->GetData().end() );
  }
  int size5 = d5.size();
  if( size5 != size4 )
    throw EExpressionError( wxT("BIVINTERP: output dependent vector length must = independent vector length") );
  //
  std::vector<double> yy( size4, 0.0 );
  try
  {
    UsefulFunctions::BivariateInterpolation( d1, d2, d3, d4, d5, yy );
  }
  catch ( std::runtime_error &e )
  {
    throw EExpressionError( wxString(wxT("BIVINTERP: "))+wxString(e.what(),wxConvUTF8) );
  }
  //
  ws->SetNumberOfDimensions( 1 );
  ws->SetDimMag( 0, size4 );
  ws->SetData( yy );
}

// end of file
