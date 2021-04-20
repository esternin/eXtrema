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
#include "UsefulFunctions.h"

#include "FCN_fft.h"

FCN_fft FCN_fft::fcn_fft_;

void FCN_fft::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError("FFT: first argument must be a vector");
    numberOfArguments_ = 1;
    nLoop = 1;
  }
}

void FCN_fft::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = 0;
  if( codes.size() > 2 )arg2 = codes[1];
  std::vector<double> a;
  ExString s;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    a.assign( nd.GetData().begin(), nd.GetData().end() );
  }
  else
  {
    Workspace *w = arg1->GetWorkspacePtr();
    if( w->IsEmpty() )
      throw EExpressionError( "FFT: first argument workspace is empty" );
    a.assign( w->GetData().begin(), w->GetData().end() );
  }
  if( !arg2 ) // arg2 is a string determining the type of fft
  {
    s = "AMP&PHASE";
  }
  else
  {
    if( arg2->IsaWorkspace() )
      s = arg2->GetWorkspacePtr()->GetFinalString();
    else if( arg2->IsaTString() )
      s = arg2->GetTString();
    else
      throw EExpressionError("FFT: second argument must be character");
    s.ToUpper();
    if( s != "AMP&PHASE" && s != "COS&SIN" )
      throw EExpressionError("FFT: second argument must be either AMP&PHASE or COS&SIN");
  }
  int nr = a.size();
  int nr2 = nr/2;
  double deg2rad = M_PI/180.;
  std::vector<double> b( nr, 0.0 );
  try
  {
    UsefulFunctions::ComplexFourierTransform( a, b, nr, nr, false );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  //
  // Construct the AMP&PHASE or the H and G coefficients of SIN&COS
  //
  ws->SetData( 0, 2.0*a[0]/nr );
  ws->SetData( nr2+1, 0.0 );
  double hamp, gamp;
  for( int k=2; k<=nr2; ++k )
  {
    hamp = 2.0*a[k-1]/nr;
    gamp = 2.0*b[k-1]/nr;
    if( s == "COS&SIN" )
    {
      ws->SetData(k-1,hamp);
      ws->SetData(nr2+k,gamp);
    }
    else if( s == "AMP&PHASE" )
    {
      double amp = sqrt(hamp*hamp+gamp*gamp);
      double phase = 0.0;
      if( amp != 0.0 )
      {
        phase = atan2(gamp,hamp)/deg2rad;
        if( phase < 0.0 )phase += 360.;
      }
      ws->SetData( k-1, amp );
      ws->SetData( nr2+k, phase );
    }
  }
  hamp = a[nr2]/nr;
  if( s == "COS&SIN" )
  {
    ws->SetData( nr2, hamp );
    ws->SetData( nr+1, 0.0 );
  }
  else if( s == "AMP&PHASE" )
  {
    double amp = fabs(hamp);
    double phase = 0.0;
    if( hamp < 0.0 )phase = 180.;
    ws->SetData( nr2, amp );
    ws->SetData( nr+1, phase );
  }
  ws->SetDimMag( 0, nr2+1 );
  ws->SetDimMag( 1, 2 );
  ws->SetNumberOfDimensions(2);
}

// end of file
