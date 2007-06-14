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

#include "FCN_ifft.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"

FCN_ifft *FCN_ifft::fcn_ifft_ = 0;

void FCN_ifft::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 2 )
      throw EExpressionError( wxT("IFFT: first argument must be a matrix") );
    numberOfArguments_ = 1;
    nLoop = 2;
  }
}

void FCN_ifft::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  std::vector<double> a;
  int nrows, ncols;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    a.assign( nd.GetData().begin(), nd.GetData().end() );
    nrows = nd.GetDimMag(0);
    ncols = nd.GetDimMag(1);
  }
  else
  {
    Workspace *w = arg1->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( wxT("IFFT: first argument workspace is empty") );
    a.assign( w->GetData().begin(), w->GetData().end() );
    nrows = w->GetDimMag(0);
    ncols = w->GetDimMag(1);
  }
  if( ncols != 2 )
    throw EExpressionError( wxT("IFFT: first argument matrix must have exactly 2 columns") );
  if( nrows <= 2 )
    throw EExpressionError( wxT("IFFT: first argument matrix must have more than 2 rows") );
  ExprCodes *arg2 = NULL;
  if( codes.size() > 2 )arg2 = codes[1];
  wxString s;
  if( !arg2 ) // arg2 is a string determining the type of ifft
  {
    s = wxT("AMP&PHASE");
  }
  else
  {
    if( arg2->IsaWorkspace() )
      s = arg2->GetWorkspacePtr()->GetFinalString();
    else if( arg2->IsaTString() )
      s = arg2->GetTString();
    else
      throw EExpressionError( wxT("IFFT: second argument must be character") );
    s.UpperCase();
    if( s != wxT("AMP&PHASE") && s != wxT("COS&SIN") )
      throw EExpressionError( wxT("IFFT: second argument must be either AMP&PHASE or COS&SIN") );
  }
  double const deg2rad = M_PI/180.0;
  int const npt = 2*(nrows-1);
  std::vector<double> work1(npt,0.0), work2(npt,0.0);
  if( s == wxT("AMP&PHASE") )
  {
    work1[0] = a[0]*cos(a[nrows]*deg2rad);
    work2[0] = a[0]*sin(a[nrows]*deg2rad);
    for( int k=1; k<nrows; ++k )
    {
      work1[k] = a[k]*cos(a[nrows+k]*deg2rad);
      work2[k] = a[k]*sin(a[nrows+k]*deg2rad);
      work1[2*(nrows-1)-k] = work1[k];
      work2[2*(nrows-1)-k] = -work2[k];
    }
  }
  else
  {
    work1[0] = a[0];
    work2[0] = a[nrows];
    for( int k=1; k<nrows; ++k )
    {
      work1[k] = a[k];
      work2[k] = a[nrows+k];
      work1[2*(nrows-1)-k] = work1[k];
      work2[2*(nrows-1)-k] = -work2[k];
    }
  }
  work1[nrows-1] = 2.0*work1[nrows-1];
  try
  {
    UsefulFunctions::ComplexFourierTransform( work1, work2, npt, npt, true );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  //
  // construct the function
  //
  for( int k=0; k<npt; ++k )ws->SetData( k, work1[k]/2.0 ); 
  ws->SetDimMag( 0, npt );
  ws->SetNumberOfDimensions(1);
}

// end of file
