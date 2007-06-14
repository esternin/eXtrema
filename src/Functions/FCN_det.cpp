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
#include "FCN_det.h"

#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_det *FCN_det::fcn_det_ = 0;

void FCN_det::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 2 )
      throw EExpressionError( wxT("DET: argument must be a matrix") );
    numberOfArguments_ = 1;
    nLoop = 2;
  }
}

void FCN_det::ArrayEval( Workspace *ws )
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
      throw EExpressionError( wxT("DET: first argument workspace is empty") );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    ndm1 = w1->GetNumberOfDimensions();
    for( int i=0; i<ndm1; ++i )size1a[i] = w1->GetDimMag(i);
  }
  if( size1a[0] != size1a[1] )
    throw EExpressionError( wxT("DET: first argument must be a square matrix") );
  int size1 = size1a[0];
  //
  if( size1 < 2 )
    throw EExpressionError( wxT("DET: matrix dimension < 2") );
  //
  std::vector<int> indx( size1, 0 );
  int id;
  try
  {
    id = UsefulFunctions::LUDecomposition( d1, indx );
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  double d = static_cast<double>(id);
  for( int i=0; i<size1; ++i )d *= d1[i+i*size1];
  ws->SetValue( d );
  ws->SetNumberOfDimensions(0);
}

// end of file
