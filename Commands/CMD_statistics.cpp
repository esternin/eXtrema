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

#include "CMD_statistics.h"
#include "ExGlobals.h"
#include "ExString.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"
#include "NumericVariable.h"
#include "UsefulFunctions.h"

CMD_statistics CMD_statistics::cmd_statistics_;

CMD_statistics::CMD_statistics() : Command( "STATISTICS" )
{
  AddQualifier( "MESSAGES", true );
  AddQualifier( "MOMENTS", false );
  AddQualifier( "PEARSON", false );
  AddQualifier( "WEIGHTS", false );
}

void CMD_statistics::Execute( ParseLine const *p )
{
  // This routine finds the max, min, indices of the max & min,
  // mean, geometric mean, median, variance, standard deviation,
  // root mean square, kurtosis, and skewness of a vector, or,
  // it finds a moment.
  //
  // STATISTICS     X      ( display all statistics )
  // STATISTICS     X S1\MAX S2\IMAX S3\MIN S4\IMIN -
  //                  S5\MEAN S6\GMEAN S7\RMS S8\VARIANCE S9\SDEV -
  //                  S10\ADEV, S11\KURTOSIS S12\SKEWNESS S13\SUM
  // STAT\WEIGHT  W X      ( display all statistics )
  // STAT\WEIGHT  W X S1\MAX S2\IMAX S3\MIN S4\IMIN -
  //                  S5\MEAN S6\GMEAN S7\RMS S8\VARIANCE S9\SDEV -
  //                  S10\ADEV, S11\KURTOSIS S12\SKEWNESS S13\SUM
  // STAT\MOMENT  W X MOMENT_NUMBER SOUT
  // STAT\PEARSON X Y { R PROB }
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
  bool output = qualifiers["MESSAGES"] || ExGlobals::NotInaScript() || ExGlobals::GetEcho();
  if( !qualifiers["MESSAGES"] )output = false;
  if( qualifiers["PEARSON"] )
  {
    // STATISTICS\PEARSON X Y { R PROB }
    //
    if( p->GetNumberOfTokens() < 3 )
      throw ECommandError( "STATISTICS", "expecting independent vector" );
    std::vector<double> x, y;
    try
    {
      NumericVariable::GetVector( p->GetString(1), "independent vector", x );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "STATISTICS", e.what() );
    }
    AddToStackLine( p->GetString(1) );
    std::size_t numX = x.size();
    //
    try
    {
      NumericVariable::GetVector( p->GetString(2), "dependent vector", y );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "STATISTICS", e.what() );
    }
    AddToStackLine( p->GetString(2) );
    std::size_t numY = y.size();
    //
    if( numX != numY )
      throw ECommandError( "STATISTICS", "input vectors have different lengths" );
    //
    double r, prob;
    try
    {
      Pearson( x, y, r, prob );
    }
    catch( ECommandError &e )
    {
      throw;
    }
    if( output )
    {
      ExString c("Correlation Coeffidient = ");
      c += ExString( r );
      ExGlobals::WriteOutput( c );
      c = ExString("Significance Level = ") + ExString(prob);
      ExGlobals::WriteOutput( c );
    }
    if( p->GetNumberOfTokens() == 5 )
    {
      if( !p->IsString(3) )
        throw ECommandError( "STATISTICS", "expecting output correlation coefficient variable" );
      if( !p->IsString(4) )
        throw ECommandError("STATISTICS","expecting output significance level variable");
      AddToStackLine( p->GetString(3) );
      AddToStackLine( p->GetString(4) );
      try
      {
        NumericVariable::PutVariable( p->GetString(3), r, p->GetInputLine() );
        NumericVariable::PutVariable( p->GetString(4), prob, p->GetInputLine() );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( "STATISTICS", e.what() );
      }
    }
    return;
  }
  if( qualifiers["MOMENTS"] )
  {
    //  calculate moments
    //  syntax:   stat\moments w x moment_number sout
    //     x = input independent vector
    //     w = input weight vector
    //
    if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
      throw ECommandError( "STATISTICS", "expecting weight vector" );
    std::vector<double> w;
    try
    {
      NumericVariable::GetVector( p->GetString(1), "weight vector", w );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "STATISTICS", e.what() );
    }
    AddToStackLine( p->GetString(1) );
    std::size_t numW = w.size();
    if( numW < 2 )
      throw ECommandError( "STATISTICS", "weight vector length < 2" );
    //
    if( p->GetNumberOfTokens()<3 || !p->IsString(2) )
      throw ECommandError( "STATISTICS", "expecting data vector" );
    std::vector<double> x;
    try
    {
      NumericVariable::GetVector( p->GetString(2), "data vector", x );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "STATISTICS", e.what() );
    }
    AddToStackLine( p->GetString(2) );
    std::size_t numX = x.size();
    if( numX != numW )
      throw ECommandError( "STATISTICS", "input vectors have different lengths" );
    //
    if( p->GetNumberOfTokens() < 4 )
      throw ECommandError( "STATISTICS", "expecting moment number" );
    double tmp;
    try
    {
      NumericVariable::GetScalar( p->GetString(3), "data vector", tmp );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "STATISTICS", e.what() );
    }
    AddToStackLine( p->GetString(3) );
    int n = static_cast<int>(tmp);
    if( n < 0 )throw ECommandError( "STATISTICS", "moment number < 0" );
    if( n == 0 )throw ECommandError( "STATISTICS", "moment number = 0" );
    ExString momentName;
    if( p->GetNumberOfTokens() == 5 )
    {
      if( !p->IsString(4) )
        throw ECommandError( "STATISTICS", "expecting output moment variable" );
      momentName = p->GetString(4);
      AddToStackLine( momentName );
    }
    double wsum = 0.0;
    for( std::size_t i=0; i<numW; ++i )wsum += w[i];
    if( wsum == 0.0 )throw ECommandError( "STATISTICS", "sum of the weights = 0" );
    double xmom = 0.0;
    for( std::size_t i=0; i<numW; ++i )xmom += w[i]*pow(x[i],n);
    xmom /= wsum;
    if( !momentName.empty() )NumericVariable::PutVariable( momentName, xmom, p->GetInputLine() );
    if( output )
    {
      ExString c;
      switch (n)
      {
        case 1:
          c = ExString("first moment = ");
          break;
        case 2:
          c = ExString("second moment = ");
          break;
        case 3:
          c = ExString("third moment = ");
          break;
        default:
          c = ExString(n) + ExString("_th moment = ");
      }
      ExGlobals::WriteOutput( c+ExString(xmom) );
    }
    return;
  }
  // calculate statistics with weights, w   assumes w>=0
  // x = input independent vector
  //
  // statistics          x      ( display all statistics )
  // statistics          x s1\max s2\imax s3\min s4\imin -
  //                       s5\mean s6\gmean s7\rms s8\variance s9\sdev -
  //                       s10\kurtosis s11\skewness
  // statistics\weight w x      ( display all statistics )
  // statistics\weight w x s1\max s2\imax s3\min s4\imin -
  //                       s5\mean s6\gmean s7\rms s8\variance s9\sdev -
  //                       s10\kurtosis s11\skewness
  //
  int counter = 1;
  std::size_t numW = 0;
  std::vector<double> w;
  if( qualifiers["WEIGHTS"] )
  {
    if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
      throw ECommandError( "STATISTICS", "expecting weight vector" );
    try
    {
      NumericVariable::GetVector( p->GetString(1), "weight vector", w );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "STATISTICS", e.what() );
    }
    AddToStackLine( p->GetString(1) );
    numW = w.size();
    if( numW < 2 )throw ECommandError( "STATISTICS", "weight vector length < 2" );
    ++counter;
  }
  if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
    throw ECommandError( "STATISTICS", "expecting data vector" );
  std::vector<double> x;
  int ndim;
  double value;
  int dimSizes[3];
  try
  {
    NumericVariable::GetVariable( p->GetString(counter), ndim, value, x, dimSizes );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "STATISTICS", e.what() );
  }
  AddToStackLine( p->GetString(counter) );
  std::size_t numX = x.size();
  if( numX < 2 )throw ECommandError( "STATISTICS", "data variable length < 2" );
  if( qualifiers["WEIGHTS"] )
  {
    if( ndim == 2 )throw ECommandError( "STATISTICS", "cannot apply weights to matrix data" );
    if( numW != numX )throw ECommandError( "STATISTICS", "input variables have different lengths" );
  }
  std::vector<ExString> validNames;
  validNames.push_back( ExString("MAX") );
  validNames.push_back( ExString("IMAX") );
  validNames.push_back( ExString("JMAX") );
  validNames.push_back( ExString("MIN") );
  validNames.push_back( ExString("IMIN") );
  validNames.push_back( ExString("JMIN") );
  validNames.push_back( ExString("MEAN") );
  validNames.push_back( ExString("GMEAN") );
  validNames.push_back( ExString("RMS") );
  validNames.push_back( ExString("VARIANCE") );
  validNames.push_back( ExString("SDEV") );
  validNames.push_back( ExString("ADEV") );
  validNames.push_back( ExString("KURTOSIS") );
  validNames.push_back( ExString("SKEWNESS") );
  validNames.push_back( ExString("MEDIAN") );
  validNames.push_back( ExString("SUM") );
  std::size_t nValid = validNames.size();
  std::vector<bool> valid( nValid, false );
  int nfld = p->GetNumberOfTokens();
  std::vector<int> validIdx( nfld, -1 );
  for( int i=counter+1; i<nfld; ++i )
  {
    if( !p->IsString(i) )
    {
      ExString c("parameter ");
      c += ExString(i+1) + ExString(" is invalid");
      throw ECommandError( "STATISTICS", c );
    }
    int nqual = p->GetToken(i)->GetNumberOfQualifiers();
    if( nqual < 1 )
    {
      ExString c("parameter ");
      c += ExString(i+1) + ExString(" has no identifying qualifier");
      throw ECommandError( "STATISTICS", c );
    }
    if( nqual > 1 )
    {
      ExString c("parameter ");
      c += ExString(i+1) + ExString(" has more than one qualifier");
      throw ECommandError( "STATISTICS", c );
    }
    ExString qualifier( *p->GetToken(i)->GetQualifier(0) );
    for( std::size_t j=0; j<nValid; ++j )
    {
      if( validNames[j] == qualifier )
      {
        validIdx[i] = j;
        valid[j] = true;
        break;
      }
    }
    if( validIdx[i] == -1 )
    {
      ExString c("parameter ");
      c += ExString(i+1) + " has an invalid qualifier: \"" + qualifier + "\"";
      throw ECommandError( "STATISTICS", c );
    }
    ExString s( p->GetString(i) );
    s += ExString(1,'\\') + qualifier;
    AddToStackLine( s );
  }
  double xmeang = 0.0;
  double xsum = 0.0;
  double sum = 0.0;
  double rms = 0.0;
  bool flag = true;  // test for x[i] >= 0.0
  bool flag2 = true; // test for rms > 0.0
  std::size_t imin, imax, jmin, jmax;
  double xmin, xmax;
  double wsum, wp;
  if( ndim == 1 )
  {
    if( qualifiers["WEIGHTS"] )
    {
      wsum = 0.0;
    }
    else
    {
      wp = 1.0;
      wsum = numX;
    }
    imax = 1;
    imin = 1;
    xmin = x[0];
    xmax = xmin;
    for( std::size_t i=0; i<numX; ++i )
    {
      double xp = x[i];
      if( qualifiers["WEIGHTS"] )
      {
        wp = w[i];
        wsum += wp;
      }
      if( xmin > xp )
      {
        xmin = xp;
        imin = i+1;
      }
      if( xmax < xp )
      {
        xmax = xp;
        imax = i+1;
      }
      xsum += wp*xp;
      sum += xp;
      xp>0.0 ? xmeang+=wp*log(xp) : flag=false;
      rms += wp*xp*xp;
    }
  }
  else if( ndim == 2 )
  {
    int nrow = dimSizes[0];
    int ncol = dimSizes[1];
    if( qualifiers["WEIGHTS"] )
    {
      wsum = 0.0;
    }
    else
    {
      wp = 1.0;
      wsum = nrow*ncol;
    }
    imax = 1;
    imin = 1;
    jmax = 1;
    jmin = 1;
    xmin = x[0];
    xmax = xmin;
    for( int j=0; j<ncol; ++j )
    {
      for( int i=0; i<nrow; ++i )
      {
        double xp = x[i+j*nrow];
        if( qualifiers["WEIGHTS"] )
        {
          wp = w[i+j*nrow];
          wsum += wp;
        }
        if( xmin > xp )
        {
          xmin = xp;
          imin = i+1;
          jmin = j+1;
        }
        if( xmax < xp )
        {
          xmax = xp;
          imax = i+1;
          jmax = j+1;
        }
        xsum += xp*wp;
        sum += xp;
        if( xp > 0.0 )
        {
          xmeang += wp*log(xp);
        }
        else
        {
          flag = false;
        }
        rms += wp*xp*xp;
      }
    }
  }
  if( wsum == 0.0 )throw ECommandError( "STATISTICS", "sum of the weights = 0" );
  double xmean = xsum/wsum;
  if( flag )xmeang = exp(xmeang/wsum);
  flag2 = (rms/wsum >= 0.0);
  if( flag2 )rms = sqrt(rms/wsum);
  //
  double var = 0.0;
  double adev = 0.0;
  double skew = 0.0;
  double xkurt = 0.0;
  //
  wp = 1.0;
  std::size_t num = ndim==2 ? dimSizes[0]*dimSizes[1] : numX;
  for( std::size_t i=0; i<num; ++i )
  {
    if( qualifiers["WEIGHTS"] )wp = w[i];
    double s = x[i] - xmean;
    adev += wp*fabs(s);
    double p = s*s;
    var += wp*p;
    p *= s;
    skew += wp*p;
    p *= s;
    xkurt += wp*p;
  }
  adev /= wsum;
  var *= num/(num-1.0)/wsum;
  double sdev = 0.0;
  if( var >= 0.0 )sdev = sqrt( var );
  if( var*var != 0.0 )
  {
    skew = skew/(wsum*var*sdev);
    xkurt = xkurt/(wsum*var*var) - 3.0;
  }
  double xmedian = 0.0;
  if( valid[14] )xmedian = UsefulFunctions::Median( x );
  //
  std::vector<double> stats;
  stats.push_back( xmax );
  stats.push_back( static_cast<double>(imax) );
  stats.push_back( static_cast<double>(jmax) );
  stats.push_back( xmin );
  stats.push_back( static_cast<double>(imin) );
  stats.push_back( static_cast<double>(jmin) );
  stats.push_back( xmean );
  stats.push_back( xmeang );
  stats.push_back( rms );
  stats.push_back( var );
  stats.push_back( sdev );
  stats.push_back( adev );
  stats.push_back( xkurt );
  stats.push_back( skew );
  stats.push_back( xmedian );
  stats.push_back( sum );
  //
  if( output || nfld==counter+1 )
  {
    char c[100];
    sprintf( c, "minimum =            %10lg | maximum =           %10lg", xmin, xmax );
    ExGlobals::WriteOutput( c );
    //
    if( ndim == 1 )
    {
      ExString s("index of minimum = [");
      s += ExString(imin) + ExString("]");
      std::size_t len = s.size();
      std::string blanks( 32-len, ' ' );
      s += blanks;
      s += ExString("| index of maximum = [") + ExString(imax) + ExString("]");
      ExGlobals::WriteOutput( s );
    }
    else if( ndim == 2 )
    {
      ExString s("index of minimum = [");
      s += ExString(imin) + ExString(",") + ExString(jmin) + ExString("]");
      std::size_t len = s.size();
      std::string blanks( 32-len, ' ' );
      s += blanks;
      s += ExString("| index of maximum = [") + ExString(imax) + ExString(",") + ExString(jmax) + ExString("]");
      ExGlobals::WriteOutput( s );
    }
    sprintf( c, "sum  =               %10lg |", sum );
    ExGlobals::WriteOutput( c );
    if( flag )
    {
      sprintf( c, "mean =               %10lg | geometric mean =    %10lg", xmean, xmeang );
    }
    else
    {
      sprintf( c, "mean =               %10lg | geometric mean unavailable", xmean );
    }
    ExGlobals::WriteOutput( c );
    if( valid[14] )
    {
      sprintf( c, "median =             %10lg | variance =          %10lg", xmedian, var );
    }
    else
    {
      sprintf( c, "median not requested            | variance =          %10lg", var );
    }
    ExGlobals::WriteOutput( c );
    if( var >= 0.0 )
    {
      sprintf( c, "standard deviation = %10lg | average deviation = %10lg", sdev, adev );
    }
    else
    {
      sprintf( c, "standard deviation unavailable | average deviation = %10lg", adev );
    }
    ExGlobals::WriteOutput( c );
    if( var*var != 0.0 )
    {
      sprintf( c, "skewness =           %10lg | kurtosis =          %10lg", skew, xkurt );
    }
    else
    {
      sprintf( c, "skewness unavailable       | kurtosis unavailable" );
    }
    ExGlobals::WriteOutput( c );
  }
  for( int i=counter+1; i<nfld; ++i )
  {
    if( (validIdx[i]==2||validIdx[i]==5) && ndim==1 )
    {
      ExGlobals::WarningMessage( "STATISTICS: column indices requested from a vector" );
      continue;
    }
    if( validIdx[i]==7 && !flag )
    {
      ExGlobals::WarningMessage( "STATISTICS: geometric mean unavailable because of negative data" );
      continue;
    }
    if( validIdx[i]==8 && !flag2 )
    {
      ExGlobals::WarningMessage( "STATISTICS: root-mean-square unavailable because of negative data" );
      continue;
    }
    try
    {
      NumericVariable::PutVariable( p->GetString(i), stats[validIdx[i]], p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( "STATISTICS", e.what() );
    }
  }
}

void CMD_statistics::Pearson( std::vector<double> &x, std::vector<double> &y, double &r, double &prob )
{
  // Given two arrays X and Y of length N, this routine computes their
  // correlation coefficient r.  The significance level at which the
  // null hypothesis of zero correlation is disproved, prob, whose small
  // value indicates a significant correlation.
  //
  // From NUMERICAL RECIPES, THE ART OF SCIENTIFIC COMPUTING
  // William H. Press, Brian P. Flannery, Saul A. Teukolsky
  // and William T. Vetterling; 1987; Cambridge University Press
  // pg. 487
  //
  double const tiny=1.e-20;
  //
  double ax = 0.0;
  double ay = 0.0;
  std::size_t const n = x.size();
  for( std::size_t j=0; j<n; ++j )
  {
    ax += x[j];
    ay += y[j];
  }
  ax /= n;
  ay /= n;
  double sxx = 0.0;
  double syy = 0.0;
  double sxy = 0.0;
  for( std::size_t j=0; j<n; ++j )
  {
    double xt = x[j]-ax;
    double yt = y[j]-ay;
    sxx += xt*xt;
    syy += yt*yt;
    sxy += xt*yt;
  }
  if( sxx*syy <= 0.0 )throw ECommandError( "STATISTICS", "cannot compute statistics with constant data" );
  r = sxy/sqrt(sxx*syy);
  double df = n-2;
  double t = r*sqrt( df/((1.0-r)+tiny)*((1.0+r)+tiny) );
  try
  {
    prob = UsefulFunctions::IncompleteBeta( df/(df+t*t), 0.5*df, 0.5 ); // i<x>(a,b)
  }
  catch( EExpressionError &e )
  {
    throw ECommandError( "STATISTICS", e.what() );
  }
  return;
}

// end of file
