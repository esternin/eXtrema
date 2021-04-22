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
#include "FCN_gaussj.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_gaussj *FCN_gaussj::fcn_gaussj_ = 0;

void FCN_gaussj::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 2 )
      throw EExpressionError( wxT("GAUSSJ: first argument must be a matrix") );
    numberOfArguments_ = 1;
    nLoop = 1;
  }
  else
  {
    nLoop = ndmEff;
    ++numberOfArguments_;
    if( numberOfArguments_ == 2 )
    {
      if( ndmEff != 1 )
        throw EExpressionError( wxT("GAUSSJ: second argument must be a vector") );
    }
  }
}

void FCN_gaussj::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  std::vector<double> d1, d2;
  int size1a[2];
  int ndm1;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    ndm1 = nd.GetNumberOfDimensions();
    for( int i=0; i<ndm1; ++i )size1a[i] = nd.GetDimMag(i);
  }
  else
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )throw EExpressionError( wxT("GAUSSJ: first argument workspace is empty") );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    ndm1 = w1->GetNumberOfDimensions();
    for( int i=0; i<ndm1; ++i )size1a[i] = w1->GetDimMag(i);
  }
  if( size1a[0] != size1a[1] )
    throw EExpressionError( wxT("GAUSSJ: first argument must be a square matrix") );
  int size1 = size1a[0];
  //
  if( arg2->IsaNVariable() )
  {
    NumericData nd( arg2->GetNVarPtr()->GetData() );
    d2.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w2 = arg2->GetWorkspacePtr();
    if( w2->IsEmpty() )throw EExpressionError( wxT("GAUSSJ: second argument workspace is empty") );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
  }
  int size2 = d2.size();
  //
  if( size2 != size1 )
    throw EExpressionError( wxT("GAUSSJ: row dimension of matrix first argument must be same as the length of the vector argument") );
  if( size1 < 2 )
    throw EExpressionError( wxT("GAUSSJ: vector length < 2") );
  //
  std::vector<int> indx( size1, 0 );
  UsefulFunctions::LUDecomposition( d1, indx );
  UsefulFunctions::LUSubstitution( d1, indx, d2 );
  /*
    std::vector<double> indxr( size1, 0.0 );
    std::vector<double> indxc( size1, 0.0 );
    std::vector<int> piv( size1, 0 );
    int row, col;
    for( int k1=1; k1<=size1; ++k1 )
    {
    double big = 0.0;
    for( int k2=1; k2<=size1; ++k2 )
    {
    if( piv[k2-1] != 1 )
    {
    for( int k3=1; k3<=size1; ++k3 )
    {
    if( piv[k3-1] == 0 )
    {
    if( fabs(d1[k2-1+(k3-1)*size1]) >= big )
    {
    big = fabs(d1[k2-1+(k3-1)*size1]);
    row = k2;
    col = k3;
    }
    }
    else if( piv[k3-1] > 1 )
    {
    throw EExpressionError("GAUSSJ: matrix argument is singular");
    }
    } 
    }
    }
    ++piv[col-1];
    if( row != col )
    {
    double dum;
    for( int k2=1; k2<=size1; ++k2 )
    {
    dum = d1[row-1+(k2-1)*size1];
    d1[row-1+(k2-1)*size1] = d1[col-1+(k2-1)*size1];
    d1[col-1+(k2-1)*size1] = dum;
    }
    dum = d2[row-1];
    d2[row-1] = d2[col-1];
    d2[col-1] = dum;
    }
    indxr[k1-1] = row;
    indxc[k1-1] = col;
    if( d1[col-1+(col-1)*size1] == 0.0 )
    throw EExpressionError("GAUSSJ: matrix argument is singular");
    double pivinv = 1.0/d1[col-1+(col-1)*size1];
    d1[col-1+(col-1)*size1] = 1.0;
    for( int k2=1; k2<=size1; ++k2 )
    d1[col-1+(k2-1)*size1] = d1[col-1+(k2-1)*size1]*pivinv;
    d2[col-1] *= pivinv;
    for( int k2=1; k2<=size1; ++k2 )
    {
    if( k2 != col )
    {
    double dum = d1[k2-1+(col-1)*size1];
    d1[k2-1+(col-1)*size1] = 0.0;
    for( int k3=1; k3<=size1; ++k3 )
    d1[k2-1+(k3-1)*size1] -= d1[col-1+(k3-1)*size1]*dum;
    d2[k2-1] -= d2[col-1]*dum;
    }
    }
    }
  */
  ws->SetData( d2 );
  ws->SetNumberOfDimensions(1);
  ws->SetDimMag( 0, size1 );
}

// end of file
