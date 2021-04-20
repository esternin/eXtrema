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
#include "CMD_bin.h"

CMD_bin CMD_bin::cmd_bin_;

CMD_bin::CMD_bin() : Command( "BIN" )
{
  AddQualifier( "WEIGHTS", false );
  AddQualifier( "EDGES", false );
  AddQualifier( "NBINS", false );
  AddQualifier( "DISCARD", false );
  AddQualifier( "EMPTY", false );
  AddQualifier( "AVERAGE", false );
  AddQualifier( "LAGRANGE", false );
}

void CMD_bin::Execute( ParseLine const *p )
{
  // BIN  x xbin xcount
  // BIN\NBINS x xbin xcount n { xmin xmax }
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
  if( qualifiers["LAGRANGE"] )
  {
    if( qualifiers["WEIGHTS"] )
      throw ECommandError( "BIN", "\LAGRANGE qualifier not compatible with \WEIGHTS qualifier" );
    else if( qualifiers["EDGES"] )
      throw ECommandError( "BIN", "\LAGRANGE qualifier not compatible with \EDGES qualifier" );
    else if( qualifiers["AVERAGE"] )
      throw ECommandError( "BIN", "\LAGRANGE qualifier not compatible with \AVERAGE qualifier" );
    else if( qualifiers["EMPTY"] )
      throw ECommandError( "BIN", "\LAGRANGE qualifier not compatible with \EMPTY qualifier" );
  }
  if( qualifiers["EMPTY"] && qualifiers["AVERAGE"] )
    throw ECommandError( "BIN", "\AVERAGE qualifier not compatible with \EMPTY qualifier" );
  int counter = 1;
  std::vector<double> w;
  if( qualifiers["WEIGHTS"] )
  {
    if( p->GetNumberOfTokens() < counter+1 )throw ECommandError( "BIN", "expecting weight vector" );
    try
    {
      NumericVariable::GetVector( p->GetString(counter), "weight vector", w );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN", e.what() );
    }
    AddToStackLine( p->GetString(counter++) );
  }
  if( p->GetNumberOfTokens() < counter+1 )throw ECommandError( "BIN", "expecting data vector" );
  std::vector<double> x;
  try
  {
    NumericVariable::GetVector( p->GetString(counter), "data vector", x );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "BIN", e.what() );
  }
  AddToStackLine( p->GetString(counter++) );
  //
  if( qualifiers["WEIGHTS"] && (w.size()!=x.size()) )
    throw ECommandError( "BIN", "weight vector length not equal to data vector length" );
  std::size_t numx = x.size();
  //
  if( qualifiers["EDGES"] )
  {
    if( p->GetNumberOfTokens() < counter+1 )
      throw ECommandError( "BIN", "expecting bin edges vector" );
    std::vector<double> e;
    try
    {
      NumericVariable::GetVector( p->GetString(counter), "bin edges vector", e );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN", e.what() );
    }
    AddToStackLine( p->GetString(counter++) );
    //
    if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
      throw ECommandError( "BIN", "expecting output counts vector name" );
    ExString countsName( p->GetString(counter) );
    AddToStackLine( countsName );
    //
    std::size_t nbins = e.size()-1;
    std::vector<double> c( nbins, 0.0 );
    std::vector<double> a;
    if( qualifiers["AVERAGE"] )a.insert( a.begin(), nbins, 0.0 );
    std::size_t k = 0;
    for( std::size_t i=0; i<numx; ++i )
    {
      std::size_t j = 0;
L101:
      ++j;
      if( j == nbins )continue;
      //
      // the j counter makes sure we will test each bin only once for any
      // data point.  the k bin counter is incremented only if the i-th
      // data point is not in the k-th bin.
      //
      if( x[i]>=e[k] && x[i]<e[k+1] )
      {
        //
        // test if data point is inside k-th bin,
        // where inside means on left edge or interior of bin.
        // do not increment k if i-th data point is found inside k-th bin.
        //
        if( qualifiers["EMPTY"] )
        {
          if( c[k] == 0.0 )
          {
            if( qualifiers["WEIGHTS"] )c[k] += w[i];
            else                       c[k] += 1.0;
          }
        }
        else
        {
          if( qualifiers["WEIGHTS"] )c[k] += w[i];
          else                       c[k] += 1.0;
          if( qualifiers["AVERAGE"] )a[k] += 1.0;
        }
      }
      else
      {
        //
        // the i-th data point not found inside the k-th bin,
        // so increment k by 1 to look at the next bin.
        //
        if( ++k == nbins )k = 0;
        //
        // if at the last bin, loop back to the first bin.
        //
        goto L101;
      }
    }
    if( qualifiers["AVERAGE"] )
    {
      for( std::size_t i=0; i<nbins; ++i )
      {
        if( a[i] != 0.0 )c[i] /= a[i];
      }
    }
    try
    {
      NumericVariable::PutVariable( countsName, c, 0, p->GetInputLine() );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN", e.what() );
    }
  }
  else if( qualifiers["NBINS"] )
  {
    //
    // BIN\NBINS                   xin xbnew  xcount nbin { xmin xmax }
    // BIN\NBINS\WEIGHTS         w xin xbnew  xcount nbin { xmin xmax }
    // BIN\NBINS\AVERAGE           xin xbnew  xcount nbin { xmin xmax }
    // BIN\NBINS\AVERAGE\WEIGHTS w xin xbnew  xcount nbin { xmin xmax }
    // BIN\NBINS\EMPTY             xin xbnew  xcount nbin { xmin xmax }
    // BIN\NBINS\EMPTY\WEIGHTS   w xin xbnew  xcount nbin { xmin xmax }
    //
    if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
      throw ECommandError( "BIN", "expecting output bins vector name" );
    //
    ExString binsName( p->GetString(counter++) );
    AddToStackLine( binsName );
    //
    if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
      throw ECommandError( "BIN", "expecting output counts vector name" );
    //
    ExString countsName( p->GetString(counter++) );
    AddToStackLine( countsName );
    //
    std::size_t nbins;
    double v;
    try
    {
      NumericVariable::GetScalar( p->GetString(counter), "number of bins", v );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN", e.what() );
    }
    if( static_cast<int>(v+0.5) < 0 )throw ECommandError( "BIN", "number of bins < 0" );
    if( static_cast<int>(v+0.5) == 0 )throw ECommandError( "BIN", "number of bins = 0" );
    nbins = static_cast<std::size_t>(v+0.5);
    AddToStackLine( p->GetString(counter++) );
    //
    double xmin, xmax;
    if( p->GetNumberOfTokens()>=counter+1 )
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(counter), "x minimum", xmin );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "BIN", e.what() );
      }
      AddToStackLine( p->GetString(counter++) );
      try
      {
        NumericVariable::GetScalar( p->GetString(counter), "x maximum", xmax );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "BIN", e.what() );
      }
      AddToStackLine( p->GetString(counter++) );
    }
    else
    {
      UsefulFunctions::MinMax( x, 0, numx, xmin, xmax );
    }
    if( xmin == xmax )throw ECommandError( "BIN", "x minimum = x maximum" );
    std::vector<double> c( nbins, 0.0 );
    std::vector<double> a;
    if( qualifiers["AVERAGE"] )a.assign( nbins, 0.0 );
    //
    std::vector<double> b;
    double binc = (xmax-xmin)/nbins;
    for( std::size_t i=0; i<nbins; ++i )b.push_back( xmin+(i+0.5)*binc );
    if( qualifiers["LAGRANGE"] )
    {
      for( std::size_t i=0; i<numx; ++i )
      {
        std::size_t j = static_cast<std::size_t>( (x[i]-xmin)/binc+1 );
        if( ((j<1)||(j>nbins)) && qualifiers["DISCARD"] )continue;
        //
        // events outside of range are placed in extreme bins
        //
        if( j < 1 )         j = 1;
        else if( j > nbins )j = nbins;
        if( x[i] >= b[j-1] )
        {
          if( j == nbins )
          {
            c[j-1] += 1.0;
          }
          else
          {
            double weight = (x[i]-b[j-1])/binc;
            c[j-1] += 1.0 - weight;
            c[j] += weight;
          }
        }
        else if( x[i] < b[j-1] )
        {
          if( j == 1 )
          {
            c[0] += 1.0;
          }
          else
          {
            double weight = (x[i]-b[j-2])/binc;
            c[j-2] += 1.0 - weight;
            c[j-1] += weight;
          }
        }
      }
    }
    else
    {
      for( std::size_t i=0; i<numx; ++i )
      {
        if( qualifiers["DISCARD"] && (x[i]<xmin || x[i]>xmax) )continue;
        std::size_t j = static_cast<std::size_t>( (x[i]-xmin)/binc+1 );
        //
        // events outside of range are placed in extreme bins
        //
        if( j < 1 )         j = 1;
        else if( j > nbins )j = nbins;
        if( qualifiers["EMPTY"] )
        {
          if( c[j-1] == 0.0 )
          {
            if( qualifiers["WEIGHTS"] )c[j-1] += w[i];
            else                       c[j-1] += 1.0;
          }
        }
        else
        {
          if( qualifiers["WEIGHTS"] )c[j-1] += w[i];
          else                       c[j-1] += 1.0;
          if( qualifiers["AVERAGE"] )a[j-1] += 1.0;
        }
      }
      if( qualifiers["AVERAGE"] )
      {
        for( std::size_t i=0; i<nbins; ++i )
        {
          if( a[i] != 0.0 )c[i] /= a[i];
        }
      }
    }
    try
    {
      NumericVariable::PutVariable( binsName, b, 0, p->GetInputLine() );
      NumericVariable::PutVariable( countsName, c, 0, p->GetInputLine() );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN", e.what() );
    }
  }
  else
  {
    // BIN                        xin xbold  xcount 
    // BIN\WEIGHT               w xin xbold  xcount 
    // BIN\AVERAGE                xin xbold  xcount 
    // BIN\AVERAGE\WEIGHT       w xin xbold  xcount 
    // BIN\EMPTY                  xin xbold  xcount 
    // BIN\EMPTY\WEIGHT         w xin xbold  xcount 
    //
    if( p->GetNumberOfTokens() < counter+1 )
      throw ECommandError( "BIN", "expecting bin centers vector" );
    std::vector<double> bin;
    try
    {
      NumericVariable::GetVector( p->GetString(counter), "bin centers vector", bin );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN", e.what() );
    }
    std::size_t nbins = bin.size();
    for( std::size_t i=0; i<nbins-1; ++i )
    {
      if( bin[i+1] <= bin[i] )
        throw ECommandError( "BIN", "bin centers vector must be strictly monotonically increasing" );
    }
    AddToStackLine( p->GetString(counter++) );
    //
    if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
      throw ECommandError( "BIN", "expecting output counts vector name" );
    ExString countsName( p->GetString(counter) );
    AddToStackLine( countsName );
    //
    std::vector<double> c( nbins, 0.0 );
    std::vector<double> a;
    if( qualifiers["AVERAGE"] )a.insert( a.begin(), nbins, 0.0 );
    std::vector<double> b( nbins+1 );
    b[0] = bin[0] - (bin[1]-bin[0])/2.0;
    for( std::size_t i=1; i<nbins; ++i )b[i] = bin[i] - (bin[i]-bin[i-1])/2.0;
    b[nbins] = bin[nbins-1] + (bin[nbins-1]-bin[nbins-2])/2.0;
    if( qualifiers["LAGRANGE"] )
    {
      for( std::size_t i=0; i<numx; ++i )
      {
        double xdum = x[i];
        std::size_t j;
        if( xdum < b[0] )
        {
          if( qualifiers["DISCARD"] )continue;
          j = 1;
          goto L50;
        }
        if( xdum >= b[nbins] )
        {
          if( qualifiers["DISCARD"] )continue;
          j = nbins;
          goto L50;
        }
        for( std::size_t k=0; k<nbins; ++k )
        {
          if( xdum>=b[k] && xdum<b[k+1] )
          {
            j = k+1;
            goto L50;
          }
        }
L50:
        if( j == nbins )
        {
          c[nbins-1] += 1.0;
        }
        else
        {
          double weight = (xdum-bin[j-1])/(b[j]+b[j-1])*0.5;
          c[j-1] += 1.0 - weight;
          c[j] += weight;
        }
      }
    }
    else
    {
      for( std::size_t i=0; i<numx; ++i )
      {
        double xdum = x[i];
        std::size_t j;
        if( xdum < b[0] )
        {
          if( qualifiers["DISCARD"] )continue;
          j = 1;
          goto L70;
        }
        if( xdum >= b[nbins] )
        {
          if( qualifiers["DISCARD"] )continue;
          j = nbins;
          goto L70;
        }
        for( std::size_t k=0; k<nbins; ++k )
        {
          if( xdum>=b[k] && xdum<b[k+1] )
          {
            j = k+1;
            goto L70;
          }
        }
L70:
        if( qualifiers["EMPTY"] )
        {
          if( c[j-1] == 0.0 )
          {
            if( qualifiers["WEIGHTS"] )c[j-1] += w[i];
            else                       c[j-1] += 1.0;
          }
        }
        else
        {
          if( qualifiers["WEIGHTS"] )c[j-1] += w[i];
          else                       c[j-1] += 1.0;
          if( qualifiers["AVERAGE"] )a[j-1] += 1.0;
        }
      }
      if( qualifiers["AVERAGE"] )
      {
        for( std::size_t i=0; i<nbins; ++i )
        {
          if( a[i] != 0.0 )c[i] /= a[i];
        }
      }
    }
    try
    {
      NumericVariable::PutVariable( countsName, c, 0, p->GetInputLine() );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "BIN", e.what() );
    }
  }
}

// end of file
