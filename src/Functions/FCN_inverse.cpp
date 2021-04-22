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
#include "FCN_inverse.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_inverse *FCN_inverse::fcn_inverse_ = 0;

void FCN_inverse::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 2 )
      throw EExpressionError( wxT("INVERSE: argument must be a matrix") );
    numberOfArguments_ = 1;
    nLoop = 2;
  }
}

void FCN_inverse::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  std::vector<double> d1;
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
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("INVERSE: first argument workspace is empty") );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    ndm1 = w1->GetNumberOfDimensions();
    for( int i=0; i<ndm1; ++i )size1a[i] = w1->GetDimMag(i);
  }
  if( size1a[0] != size1a[1] )
    throw EExpressionError( wxT("INVERSE: first argument must be a square matrix") );
  int size1 = size1a[0];
  //
  if( size1 < 2 )
    throw EExpressionError( wxT("INVERSE: matrix dimension < 2") );
  //
  std::vector<int> indx( size1, 0 );
  UsefulFunctions::LUDecomposition( d1, indx );
  std::vector<double> d2( size1*size1, 0.0 );
  std::vector<double> b( size1, 0.0 );
  for( int j=0; j<size1; ++j )
  {
    b.assign( size1, 0.0 );
    b[j] = 1.0;
    UsefulFunctions::LUSubstitution( d1, indx, b );
    for( int i=0; i<size1; ++i )d2[i+j*size1] = b[i];
  }
  ws->SetData( d2 );
  ws->SetNumberOfDimensions(2);
  ws->SetDimMag( 0, size1 );
  ws->SetDimMag( 1, size1 );
}

// end of file
