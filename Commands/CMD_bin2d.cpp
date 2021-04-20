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

#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"
#include "NumericVariable.h"

#include "CMD_bin2d.h"

CMD_bin2d CMD_bin2d::cmd_bin2d_;

CMD_bin2d::CMD_bin2d() : Command( "BIN2D" )
{
  AddQualifier( "WEIGHTS", false );
  AddQualifier( "EMPTY", false );
  AddQualifier( "MATRIX", false );
  AddQualifier( "XDISCARD", false );
  AddQualifier( "YDISCARD", false );
}

void CMD_bin2d::Execute( ParseLine const *p )
{
  // BIN2D                xin yin xb yb mc nx ny { xmin xmax ymin ymax }
  // BIN2D\WEIGHT       w xin yin xb yb mc nx ny { xmin xmax ymin ymax }
  // BIN2D\EMPTY          xin yin xb yb mc nx ny { xmin xmax ymin ymax }
  // BIN2D\EMPTY\WEIGHT w xin yin xb yb mc nx ny { xmin xmax ymin ymax }
  // BIN2D\MATRIX       mdata mxin myin mout
  //
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  if( qualifiers["MATRIX"] )
  {
    //
    // bin2d\matrix mdata mxin myin mout
    //
    if( p->GetNumberOfTokens() < 5 )throw ECommandError( "BIN2D", "expecting: mdata mx my mc" );
    std::vector<double> data;
    std::size_t nRow, nCol;
    try
    {
      NumericVariable::GetMatrix( p->GetString(1), "data matrix", data, nRow, nCol  );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN2D", e.what() );
    }
    AddToStackLine( p->GetString(1) );
    //
    std::vector<double> x;
    std::size_t nRowx, nColx;
    try
    {
      NumericVariable::GetMatrix( p->GetString(2), "x matrix", x, nRowx, nColx  );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN2D", e.what() );
    }
    AddToStackLine( p->GetString(2) );
    //
    std::vector<double> y;
    std::size_t nRowy, nColy;
    try
    {
      NumericVariable::GetMatrix( p->GetString(3), "y matrix", y, nRowy, nColy  );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN2D", e.what() );
    }
    AddToStackLine( p->GetString(3) );
    //
    if( (nRowy!=nRowx) || (nColy!=nColx) )
      throw ECommandError( "BIN2D", "x and y matrices must be the same size" );
    //
    ExString moutName( p->GetString(4) );
    //
    int nrout = nRowx-1;
    int ncout = nColx-1;
    std::vector<double> out(nrout*ncout,0.0);
    //
    int kc = 1;
    int kr = 1;
    for( std::size_t j=1; j<=nRow; ++j )
    {
      for( std::size_t i=1; i<=nCol; ++i )  // do 1030 i = 1, ncxy
      {
        int icnt = 0;
        bool test = true;
        while( test )
        {
          std::vector<double> x, y;
          std::vector<double>().swap( x );
          std::vector<double>().swap( y );
          test = false;
          if( ++icnt > nrout*ncout )continue;
          x.push_back( x[kc-1+(kr-1)*nColx] );
          y.push_back( y[kc-1+(kr-1)*nColx] );
          x.push_back( x[kc+(kr-1)*nColx] );
          y.push_back( y[kc+(kr-1)*nColx] );
          x.push_back( x[kc+(kr)*nColx] );
          y.push_back( y[kc+(kr)*nColx] );
          x.push_back( x[kc-1+(kr)*nColx] );
          y.push_back( y[kc-1+(kr)*nColx] );
          if( UsefulFunctions::InsidePolygon(static_cast<double>(i),static_cast<double>(j),x,y) )
          {
            out[kc-1+(kr-1)*ncout] = out[kc-1+(kr-1)*ncout] + data[i-1+(j-1)*nCol];
          }
          else
          {
            if( ++kc > ncout )
            {
              kc = 1;
              if( ++kr > nrout )kr = 1;
            }
            test = true;
          }
        }
      }
    }
    NumericVariable::PutVariable( moutName, out, nrout, ncout, p->GetInputLine() );
    return;
  }
  //
  // bin2d                xin yin xb yb mc nx ny { xmin xmax ymin ymax }
  // bin2d\weight       w xin yin xb yb mc nx ny { xmin xmax ymin ymax }
  // bin2d\empty          xin yin xb yb mc nx ny { xmin xmax ymin ymax }
  // bin2d\empty\weight w xin yin xb yb mc nx ny { xmin xmax ymin ymax }
  //
  int counter = 1;
  std::vector<double> w;
  if( qualifiers["WEIGHTS"] )
  {
    if( p->GetNumberOfTokens() < counter+1 )
      throw ECommandError( "BIN2D", "expecting weight vector" );
    try
    {
      NumericVariable::GetVector( p->GetString(counter), "weight vector", w );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN2D", e.what() );
    }
    AddToStackLine( p->GetString(counter++) );
  }
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( "BIN2D", "expecting x vector" );
  std::vector<double> x;
  try
  {
    NumericVariable::GetVector( p->GetString(counter), "x vector", x );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "BIN2D", e.what() );
  }
  AddToStackLine( p->GetString(counter++) );
  //
  if( qualifiers["WEIGHTS"] )
  {
    if( w.size() != x.size() )
      throw ECommandError( "BIN2D", "weight vector length must be equal to data vector length" );
  }
  int numx = x.size();
  //
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( "BIN2D", "expecting y vector" );
  std::vector<double> y;
  try
  {
    NumericVariable::GetVector( p->GetString(counter), "y vector", y );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "BIN2D", e.what() );
  }
  AddToStackLine( p->GetString(counter++) );
  int numy = y.size();
  //
  if( numy != numx )
    throw ECommandError( "BIN2D", "x vector length must be equal to y vector length" );
  //
  if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
    throw ECommandError( "BIN2D", "expecting output x bin vector name" );
  ExString xbName( p->GetString(counter) );
  AddToStackLine( xbName );
  ++counter;
  //
  if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
    throw ECommandError( "BIN2D", "expecting output y bin vector name" );
  ExString ybName( p->GetString(counter) );
  AddToStackLine( ybName );
  ++counter;
  //
  if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
    throw ECommandError( "BIN2D", "expecting output counts matrix name" );
  ExString cName( p->GetString(counter) );
  AddToStackLine( cName );
  ++counter;
  //
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( "BIN2D", "expecting number of x bins" );
  double v;
  try
  {
    NumericVariable::GetScalar( p->GetString(counter), "number of x bins", v );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "BIN2D", e.what() );
  }
  int nbinsx, nbinsy;
  if( v > 0.0 )
    nbinsx = static_cast<int>( v+0.5 );
  else if( v < 0.0 )
    throw ECommandError( "BIN2D", "number of x bins < 0" );
  else
    throw ECommandError( "BIN2D", "number of x bins = 0" );
  ++counter;
  //
  if( p->GetNumberOfTokens() < counter+1 )throw ECommandError( "BIN2D", "expecting number of y bins" );
  try
  {
    NumericVariable::GetScalar( p->GetString(counter), "number of y bins", v );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "BIN2D", e.what() );
  }
  if( v > 0.0 )
    nbinsy = static_cast<int>( v+0.5 );
  else if( v < 0.0 )
    throw ECommandError( "BIN2D", "number of y bins < 0" );
  else
    throw ECommandError( "BIN2D", "number of y bins = 0" );
  ++counter;
  //
  double xmins, xmaxs, ymins, ymaxs;
  UsefulFunctions::MinMax( x, 0, numx, xmins, xmaxs );
  UsefulFunctions::MinMax( y, 0, numy, ymins, ymaxs );
  double xminsf = xmins;
  double xmaxsf = xmaxs;
  double yminsf = ymins;
  double ymaxsf = ymaxs;
  double dist, xminf, xmaxf, yminf, ymaxf;
  try
  {
    UsefulFunctions::Scale2(xminf,xmaxf,dist,nbinsx,xminsf,xmaxsf);
    UsefulFunctions::Scale2(yminf,ymaxf,dist,nbinsy,yminsf,ymaxsf);
  }
  catch ( runtime_error &e )
  {
    throw ECommandError( "BIN2D", e.what() );
  }
  double xmin = xminf;
  double xmax = xmaxf;
  double ymin = yminf;
  double ymax = ymaxf;
  //
  if( p->GetNumberOfTokens() >= counter+4 )
  {
    try
    {
      NumericVariable::GetScalar( p->GetString(counter++), "x bin minimum", xmin );
      NumericVariable::GetScalar( p->GetString(counter++), "x bin maximum", xmax );
      NumericVariable::GetScalar( p->GetString(counter++), "y bin minimum", ymin );
      NumericVariable::GetScalar( p->GetString(counter++), "y bin maximum", ymax );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN2D", e.what() );
    }
  }
  if( xmax == xmin )throw ECommandError( "BIN2D", "x bin minimum = x bin maximum" );
  double bincx = (xmax-xmin)/nbinsx;
  if( ymax == ymin )throw ECommandError( "BIN2D", "y bin minimum = y bin maximum" );
  double bincy = (ymax-ymin)/nbinsy;
  std::vector<double> bx( nbinsx );
  std::vector<double> by( nbinsy );
  std::vector<double> m( nbinsx*nbinsy, 0.0 );
  for( int i=0; i<nbinsx; ++i )bx[i] = xmin+(i+0.5)*bincx;
  for( int i=0; i<nbinsy; ++i )by[i] = ymin+(i+0.5)*bincy;
  for( int i=1; i<=numx; ++i )
  {
    if( qualifiers["XDISCARD"] && (x[i-1]<xmin || x[i-1]>xmax) )continue;
    if( qualifiers["YDISCARD"] && (y[i-1]<ymin || y[i-1]>ymax) )continue;
    int j = static_cast<int>((x[i-1]-xmin)/bincx) + 1;
    int k = static_cast<int>((y[i-1]-ymin)/bincy) + 1;
    //
    // events outside of range are placed in extreme bins
    //
    if( j < 1 )j = 1;
    else if( j > nbinsx )j = nbinsx;
    if( k < 1 )k = 1;
    else if( k > nbinsy )k = nbinsy;
    if( qualifiers["EMPTY"] )
    {
      if( m[k-1+(j-1)*nbinsy] == 0.0 )
      {
        qualifiers["WEIGHTS"] ? m[k-1+(j-1)*nbinsy]+=w[i-1] : ++m[k-1+(j-1)*nbinsy];
      }
    }
    else
    {
      qualifiers["WEIGHTS"] ? m[k-1+(j-1)*nbinsy]+=w[i-1] : ++m[k-1+(j-1)*nbinsy];
    }
  }
  try
  {
    NumericVariable::PutVariable( xbName, bx, 0, p->GetInputLine() );
    NumericVariable::PutVariable( ybName, by, 0, p->GetInputLine() );
    NumericVariable::PutVariable( cName, m, nbinsy, nbinsx, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( "BIN2D", e.what() );
  }
}

// end of file
