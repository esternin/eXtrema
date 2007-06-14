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
#include "FCN_volume.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "GRA_thiessenTriangulation.h"

FCN_volume *FCN_volume::fcn_volume_ = 0;

void FCN_volume::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("VOLUME: first argument must be a vector") );
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
        throw EExpressionError( wxT("VOLUME: second argument must be a vector") );
    }
    else if( numberOfArguments_ == 3 )
    {
      if( ndmEff!=1 && ndmEff!=2 )
        throw EExpressionError( wxT("VOLUME: third argument must be a vector or a matrix") );
    }
  }
}

void FCN_volume::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  ExprCodes *arg3 = codes[2];
  std::vector<double> x, y, z;
  if( arg1->IsaNVariable() ) // arg1 is x
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    x.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("VOLUME: first argument workspace is empty") );
    x.assign( w1->GetData().begin(), w1->GetData().end() );
  }
  std::size_t size1 = x.size();
  //
  if( arg2->IsaNVariable() ) // arg2 is y
  {
    NumericData nd( arg2->GetNVarPtr()->GetData() );
    y.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w2 = arg2->GetWorkspacePtr();
    if( w2->IsEmpty() )
      throw EExpressionError( wxT("VOLUME: second argument workspace is empty") );
    y.assign( w2->GetData().begin(), w2->GetData().end() );
  }
  std::size_t size2 = y.size();
  if( size2 != size1 )throw EExpressionError( wxT("VOLUME: vectors have different lengths") );
  //
  bool matrix = false;
  std::size_t nrows, ncols;
  if( arg3->IsaNVariable() ) // arg3 is z
  {
    NumericData nd( arg3->GetNVarPtr()->GetData() );
    z.assign( nd.GetData().begin(), nd.GetData().end() );
    if( nd.GetNumberOfDimensions() == 2 )
    {
      matrix = true;
      nrows = nd.GetDimMag(0);
      ncols = nd.GetDimMag(1);
    }
  }
  else
  {
    Workspace *w3 = arg3->GetWorkspacePtr();
    if( w3->IsEmpty() )
      throw EExpressionError( wxT("VOLUME: third argument workspace is empty") );
    z.assign( w3->GetData().begin(), w3->GetData().end() );
    if( w3->GetNumberOfDimensions() == 2 )
    {
      matrix = true;
      nrows = w3->GetDimMag(0);
      ncols = w3->GetDimMag(1);
    }
  }
  std::size_t size3 = z.size();
  //
  double volume = 0.0;
  //
  if( matrix )
  {
    if( nrows != size2 )throw EExpressionError(
      wxT("VOLUME: length of dependent vector not equal to number of rows of the matrix") );
    if( ncols != size1 )throw EExpressionError(
      wxT("VOLUME: length of independent vector not equal to number of columns of the matrix") );
    for( std::size_t j=1; j<=nrows-1; ++j )
    {
      for( std::size_t i=1; i<=ncols-1; ++i )
      {
        double dx = x[j] - x[j-1];
        double dy = y[i] - y[i-1];
        double area = 0.5*dx*dy;
        double hite = z[i-1+(j-1)*ncols] + z[i+(j-1)*ncols] + z[i-1+j*ncols];
        volume += area*hite/3.0;
        hite = z[i-1+j*ncols] + z[i+j*ncols] + z[i+(j-1)*ncols];
        volume += area*hite/3.0;
      }
    }
  }
  else
  {
    if( size3 != size1 )throw EExpressionError( wxT("VOLUME: vectors have different lengths") );
    if( size1 < 4 )throw EExpressionError( wxT("VOLUME: vector length < 4") );
    try
    {
      GRA_thiessenTriangulation tt( x, y, z );
      tt.CreateMesh();
      volume = tt.Volume();
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  ws->SetValue( volume );
  ws->SetNumberOfDimensions(0);
  ws->SetDimMag( 0, 0 );
}

// end of file
