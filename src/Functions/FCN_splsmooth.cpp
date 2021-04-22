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
#include "FCN_splsmooth.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_splsmooth *FCN_splsmooth::fcn_splsmooth_ = 0;

void FCN_splsmooth::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("SPLSMOOTH: first argument must be a vector") );
    numberOfArguments_ = 1;
    nLoop = 1;
  }
  else
  {
    nLoop = ndmEff;
    switch (++numberOfArguments_)
    {
      case 2:
        if( ndmEff != 1 )
          throw EExpressionError( wxT("SPLSMOOTH: second argument must be a vector") );
        break;
      case 3:
        if( ndmEff != 0 )
          throw EExpressionError( wxT("SPLSMOOTH: third argument must be a scalar") );
        break;
      case 4:
        if( ndmEff != 1 )
          throw EExpressionError( wxT("SPLSMOOTH: fourth argument must be a vector") );
        break;
    }
  }
}

void FCN_splsmooth::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  ExprCodes *arg3 = codes[2];
  std::vector<double> d1, d2;
  if( arg1->IsaNVariable() ) // arg1 is x
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("SPLSMOOTH: first argument workspace is empty") );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
  }
  std::size_t size1 = d1.size();
  if( arg2->IsaNVariable() ) // arg2 is y
  {
    NumericData nd( arg2->GetNVarPtr()->GetData() );
    d2.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w2 = arg2->GetWorkspacePtr();
    if( w2->IsEmpty() )
      throw EExpressionError( wxT("SPLSMOOTH: second argument workspace is empty") );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
  }
  std::size_t size2 = d2.size();
  //
  if( size2 != size1 )
    throw EExpressionError( wxT("SPLSMOOTH: vectors have different lengths") );
  if( size1 < 2 )
    throw EExpressionError( wxT("SPLSMOOTH: independent vector length < 2") );
  //
  double d3;
  if( arg3->IsaNVariable() )
  {
    d3 = arg3->GetNVarPtr()->GetData().GetScalarValue();
  }
  else if( arg3->IsaWorkspace() )
  {
    if( arg3->GetWorkspacePtr()->IsEmpty() )
      throw EExpressionError( wxT("SPLSMOOTH: third argument workspace is empty") );
    d3 = arg3->GetWorkspacePtr()->GetValue();
  }
  else
  {
    d3 = arg3->GetConstantValue();
  }
  int npts = static_cast<int>(d3+0.5);
  if( npts < 2 )throw EExpressionError( wxT("SPLSMOOTH: number of output points must be > 1") );
  //
  std::vector<double> d4;
  if( numberOfArguments_ == 4 ) // arg4 is the weights
  {
    ExprCodes *arg4 = codes[3];
    if( arg4->IsaNVariable() )
    {
      NumericData nd( arg4->GetNVarPtr()->GetData() );
      d4.assign( nd.GetData().begin(), nd.GetData().end() );
    }
    else
    {
      Workspace *w4 = arg4->GetWorkspacePtr();
      if( w4->IsEmpty() )
        throw EExpressionError( wxT("SPLSMOOTH: fourth argument workspace is empty") );
      d4.assign( w4->GetData().begin(), w4->GetData().end() );
    }
    if( d4.size() != size1 )
      throw EExpressionError( wxT("SPLSMOOTH: vectors have different lengths") );
  }
  else
  {
    d4.insert( d4.begin(), size1, 1.0 );
  }
  std::vector<double> xx( npts, 0.0 );
  std::vector<double> yy( npts, 0.0 );
  UsefulFunctions::Splsmooth( d1, d2, d4, xx, yy );
  for( int i=0; i<npts; ++i )xx.push_back(yy[i]);
  //
  ws->SetNumberOfDimensions(2);
  ws->SetDimMag( 0, npts );
  ws->SetDimMag( 1, 2 );
  ws->SetData( xx );
}

// end of file
