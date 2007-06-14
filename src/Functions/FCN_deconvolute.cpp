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
#include <cmath>

#include "FCN_deconvolute.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_deconvolute *FCN_deconvolute::fcn_deconvolute_ = 0;

void FCN_deconvolute::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff!=1 )
      throw EExpressionError( wxT("DECONVOLUTE: first argument must be a vector") );
    numberOfArguments_ = 1;
    nLoop = 0;
  }
  else
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("DECONVOLUTE: second argument must be a vector") );
    numberOfArguments_ = 2;
  }
}

void FCN_deconvolute::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  std::vector<double> x, b;
  std::size_t sizeX, sizeB;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    x.assign( nd.GetData().begin(), nd.GetData().end() );
    sizeX = nd.GetDimMag(0);
  }
  else    // argument 1 must be a workspace
  {
    Workspace *w = arg1->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( wxT("DECONVOLUTE: first argument workspace is empty") );
    x.assign( w->GetData().begin(), w->GetData().end() );
    sizeX = w->GetDimMag(0);
  }
  if( arg2->IsaNVariable() )
  {
    NumericData nd( arg2->GetNVarPtr()->GetData() );
    b.assign( nd.GetData().begin(), nd.GetData().end() );
    sizeB = nd.GetDimMag(0);
  }
  else    // argument 1 must be a workspace
  {
    Workspace *w = arg2->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( wxT("DECONVOLUTE: first argument workspace is empty") );
    b.assign( w->GetData().begin(), w->GetData().end() );
    sizeB = w->GetDimMag(0);
  }
  //
  std::size_t nonZeroB = 0;
  double sumB = 0.0;
  for( std::size_t i=0; i<sizeB; ++i )
  {
    if( b[i] != 0.0 )
    {
      ++nonZeroB;
      sumB += b[i];
    }
  }
  if( sizeB/2*2 != sizeB )
    throw EExpressionError( wxT("DECONVOLUTE: length second argument is not even") );
  if( sizeX != sizeB )
    throw EExpressionError( wxT("DECONVOLUTE: vector argument lengths must be equal") );
  //
  // Sum the blurring function array (it should sum to 1; if it
  // doesn't then normalize accordingly)
  //
  if( fabs(sumB-1.0) >= 0.00001 ) // The blurring function has not been normalised, so
  {                               // we now normalise it to unity
    for( std::size_t k=0; k<sizeB; ++k )b[k] /= sumB;
  }
  std::vector<double> work1( sizeX, 0.0 );
  std::vector<double> work2( sizeX, 0.0 );
  //
  // Call the discrete fast fourier transform
  //
  try
  {
    UsefulFunctions::ComplexFourierTransform( x, work1, sizeX, sizeX, false );
    UsefulFunctions::ComplexFourierTransform( b, work2, sizeX, sizeX, false );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  for( std::size_t k=0; k<sizeX; ++k )
  {
    double denom = b[k]*b[k] + work2[k]*work2[k];
    double temp  = x[k];
    x[k] = x[k]*b[k] + work1[k]*work2[k];
    work1[k] = work1[k]*b[k] - temp*work2[k];
    if( denom != 0.0 )
    {
      x[k] /= denom;
      work1[k] /= denom;
    }
  }
  //
  // Call the inverse discete fast fourier transform (deconvolution)
  //
  for( std::size_t k=0; k<sizeX; ++k )
  {
    x[k] /= sizeX;
    work1[k] /= sizeX;
  }
  try
  {
    UsefulFunctions::ComplexFourierTransform( x, work1, sizeX, sizeX, true );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  ws->SetData( x );
  ws->SetNumberOfDimensions(1);
  ws->SetDimMag( 0, sizeX );
}

// end of file
