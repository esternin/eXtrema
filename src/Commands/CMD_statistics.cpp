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

#include "wx/wx.h"

#include "CMD_statistics.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "EExpressionError.h"

CMD_statistics *CMD_statistics::cmd_statistics_ = 0;

CMD_statistics::CMD_statistics() : Command( wxT("STATISTICS") )
{
  AddQualifier( wxT("MESSAGES"), true );
  AddQualifier( wxT("MOMENTS"), false );
  AddQualifier( wxT("PEARSON"), false );
  AddQualifier( wxT("WEIGHTS"), false );
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
  SetUp( p, qualifiers );
  wxString command( Name()+wxT(": ") );
  bool output = qualifiers[wxT("MESSAGES")] || ExGlobals::NotInaScript() || ExGlobals::GetEcho();
  if( !qualifiers[wxT("MESSAGES")] )output = false;
  if( qualifiers[wxT("PEARSON")] )
  {
    // STATISTICS\PEARSON X Y { R PROB }
    //
    if( p->GetNumberOfTokens() < 3 )
      throw ECommandError( command+wxT("expecting independent vector") );
    std::vector<double> x, y;
    try
    {
      NumericVariable::GetVector( p->GetString(1), wxT("independent vector"), x );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(1) );
    std::size_t numX = x.size();
    //
    try
    {
      NumericVariable::GetVector( p->GetString(2), wxT("dependent vector"), y );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(2) );
    std::size_t numY = y.size();
    //
    if( numX != numY )
      throw ECommandError( command+wxT("input vectors have different lengths") );
    //
    double r, prob;
    Pearson( x, y, r, prob );
    if( output )
    {
      wxString c( wxT("Correlation Coeffidient = ") );
      ExGlobals::WriteOutput( c << r );
      c = wxT("Significance Level = ");
      ExGlobals::WriteOutput( c << prob );
    }
    if( p->GetNumberOfTokens() == 5 )
    {
      if( !p->IsString(3) )
        throw ECommandError( command+wxT("expecting output correlation coefficient variable") );
      if( !p->IsString(4) )
        throw ECommandError( command+wxT("expecting output significance level variable") );
      AddToStackLine( p->GetString(3) );
      AddToStackLine( p->GetString(4) );
      try
      {
        NumericVariable::PutVariable( p->GetString(3), r, p->GetInputLine() );
        NumericVariable::PutVariable( p->GetString(4), prob, p->GetInputLine() );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
    }
    return;
  }
  if( qualifiers[wxT("MOMENTS")] )
  {
    //  calculate moments
    //  syntax:   stat\moments w x moment_number sout
    //     x = input independent vector
    //     w = input weight vector
    //
    if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
      throw ECommandError( command+wxT("expecting weight vector") );
    std::vector<double> w;
    try
    {
      NumericVariable::GetVector( p->GetString(1), wxT("weight vector"), w );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(1) );
    std::size_t numW = w.size();
    if( numW < 2 )
      throw ECommandError( command+wxT("weight vector length < 2") );
    //
    if( p->GetNumberOfTokens()<3 || !p->IsString(2) )
      throw ECommandError( command+wxT("expecting data vector") );
    std::vector<double> x;
    try
    {
      NumericVariable::GetVector( p->GetString(2), wxT("data vector"), x );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(2) );
    std::size_t numX = x.size();
    if( numX != numW )
      throw ECommandError( command+wxT("input vectors have different lengths") );
    //
    if( p->GetNumberOfTokens() < 4 )
      throw ECommandError( command+wxT("expecting moment number") );
    double tmp;
    try
    {
      NumericVariable::GetScalar( p->GetString(3), wxT("data vector"), tmp );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(3) );
    int n = static_cast<int>(tmp);
    if( n < 0 )throw ECommandError( command+wxT("moment number < 0") );
    if( n == 0 )throw ECommandError( command+wxT("moment number = 0") );
    wxString momentName;
    if( p->GetNumberOfTokens() == 5 )
    {
      if( !p->IsString(4) )
        throw ECommandError( command+wxT("expecting output moment variable") );
      momentName = p->GetString(4);
      AddToStackLine( momentName );
    }
    double wsum = 0.0;
    for( std::size_t i=0; i<numW; ++i )wsum += w[i];
    if( wsum == 0.0 )throw ECommandError( command+wxT("sum of the weights = 0") );
    double xmom = 0.0;
    for( std::size_t i=0; i<numW; ++i )xmom += w[i]*pow(x[i],n);
    xmom /= wsum;
    if( !momentName.empty() )NumericVariable::PutVariable( momentName, xmom, p->GetInputLine() );
    if( output )
    {
      wxString c;
      switch (n)
      {
        case 1:
          c = wxT("first moment = ");
          break;
        case 2:
          c = wxT("second moment = ");
          break;
        case 3:
          c = wxT("third moment = ");
          break;
        default:
          c << n << wxT("_th moment = ");
      }
      ExGlobals::WriteOutput( c << xmom );
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
  if( qualifiers[wxT("WEIGHTS")] )
  {
    if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
      throw ECommandError( command+wxT("expecting weight vector") );
    try
    {
      NumericVariable::GetVector( p->GetString(1), wxT("weight vector"), w );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(1) );
    numW = w.size();
    if( numW < 2 )throw ECommandError( command+wxT("weight vector length < 2") );
    ++counter;
  }
  if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
    throw ECommandError( command+wxT("expecting data vector") );
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
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(counter) );
  std::size_t numX = x.size();
  if( numX < 2 )throw ECommandError( command+wxT("data variable length < 2") );
  if( qualifiers[wxT("WEIGHTS")] )
  {
    if( ndim == 2 )throw ECommandError( command+wxT("cannot apply weights to matrix data") );
    if( numW != numX )throw ECommandError( command+wxT("input variables have different lengths") );
  }
  std::vector<wxString> validNames;
  validNames.push_back( wxString(wxT("MAX")) );
  validNames.push_back( wxString(wxT("IMAX")) );
  validNames.push_back( wxString(wxT("JMAX")) );
  validNames.push_back( wxString(wxT("MIN")) );
  validNames.push_back( wxString(wxT("IMIN")) );
  validNames.push_back( wxString(wxT("JMIN")) );
  validNames.push_back( wxString(wxT("MEAN")) );
  validNames.push_back( wxString(wxT("GMEAN")) );
  validNames.push_back( wxString(wxT("RMS")) );
  validNames.push_back( wxString(wxT("VARIANCE")) );
  validNames.push_back( wxString(wxT("SDEV")) );
  validNames.push_back( wxString(wxT("ADEV")) );
  validNames.push_back( wxString(wxT("KURTOSIS")) );
  validNames.push_back( wxString(wxT("SKEWNESS")) );
  validNames.push_back( wxString(wxT("MEDIAN")) );
  validNames.push_back( wxString(wxT("SUM")) );
  std::size_t nValid = validNames.size();
  std::vector<bool> valid( nValid, false );
  int nfld = p->GetNumberOfTokens();
  std::vector<int> validIdx( nfld, -1 );
  for( int i=counter+1; i<nfld; ++i )
  {
    if( !p->IsString(i) )
    {
      wxString c(wxT("parameter "));
      throw ECommandError( command+(c << i+1 << wxT(" is invalid")) );
    }
    int nqual = p->GetToken(i)->GetNumberOfQualifiers();
    if( nqual < 1 )
    {
      wxString c(wxT("parameter "));
      throw ECommandError( command+(c << i+1 << wxT(" has no identifying qualifier")) );
    }
    if( nqual > 1 )
    {
      wxString c(wxT("parameter "));
      throw ECommandError( command+(c << i+1 << wxT(" has more than one qualifier")) );
    }
    wxString qualifier( *p->GetToken(i)->GetQualifier(0) );
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
      wxString c(wxT("parameter "));
      throw ECommandError( command+(c << i+1 << wxT(" has an invalid qualifier: \"")
                                      << qualifier << wxT('\"')) );
    }
    wxString s( p->GetString(i) );
    s += wxString(1,wxT('\\')) + qualifier;
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
    if( qualifiers[wxT("WEIGHTS")] )
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
      if( qualifiers[wxT("WEIGHTS")] )
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
    if( qualifiers[wxT("WEIGHTS")] )
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
        if( qualifiers[wxT("WEIGHTS")] )
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
  if( wsum == 0.0 )throw ECommandError( command+wxT("sum of the weights = 0") );
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
    if( qualifiers[wxT("WEIGHTS")] )wp = w[i];
    double s = x[i] - xmean;
    adev += wp*fabs(s);
    double p = s*s;
    var += wp*p;
    p *= s;
    skew += wp*p;
    p *= s;
    xkurt += wp*p;
  }
  adev = adev/wsum;
  var = var/wsum*num/(num-1.0);
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
    wxChar c[100];
    ::wxSnprintf( c, 100, wxT("minimum =            %10lg | maximum =           %10lg"), xmin, xmax );
    ExGlobals::WriteOutput( c );
    //
    if( ndim == 1 )
    {
      wxString s(wxT("index of minimum = ["));
      s << imin << wxT("]");
      std::size_t len = s.size();
      s += wxString( 32-len, ' ' );
      s << wxT("| index of maximum = [") << imax << wxT(']');
      ExGlobals::WriteOutput( s );
    }
    else if( ndim == 2 )
    {
      wxString s(wxT("index of minimum = ["));
      s << imin << wxT(",") << jmin << wxT(']');
      std::size_t len = s.size();
      s += wxString( 32-len, ' ' );
      s << wxT("| index of maximum = [") << imax << wxT(',') << jmax << wxT(']');
      ExGlobals::WriteOutput( s );
    }
    ::wxSnprintf( c, 100, wxT("sum  =               %10lg |"), sum );
    ExGlobals::WriteOutput( c );
    if( flag )
    {
      ::wxSnprintf( c, 100, wxT("mean =               %10lg | geometric mean =    %10lg"), xmean, xmeang );
    }
    else
    {
      ::wxSnprintf( c, 100, wxT("mean =               %10lg | geometric mean unavailable"), xmean );
    }
    ExGlobals::WriteOutput( c );
    if( valid[14] )
    {
      ::wxSnprintf( c, 100, wxT("median =             %10lg | variance =          %10lg"), xmedian, var );
    }
    else
    {
      ::wxSnprintf( c, 100, wxT("median not requested            | variance =          %10lg"), var );
    }
    ExGlobals::WriteOutput( c );
    if( var >= 0.0 )
    {
      ::wxSnprintf( c, 100, wxT("standard deviation = %10lg | average deviation = %10lg"), sdev, adev );
    }
    else
    {
      ::wxSnprintf( c, 100, wxT("standard deviation unavailable | average deviation = %10lg"), adev );
    }
    ExGlobals::WriteOutput( c );
    if( var*var != 0.0 )
    {
      ::wxSnprintf( c, 100, wxT("skewness =           %10lg | kurtosis =          %10lg"), skew, xkurt );
    }
    else
    {
      ::wxSnprintf( c, 100, wxT("skewness unavailable       | kurtosis unavailable") );
    }
    ExGlobals::WriteOutput( c );
  }
  for( int i=counter+1; i<nfld; ++i )
  {
    if( (validIdx[i]==2||validIdx[i]==5) && ndim==1 )
    {
      ExGlobals::WarningMessage( wxT("STATISTICS: column indices requested from a vector") );
      continue;
    }
    if( validIdx[i]==7 && !flag )
    {
      ExGlobals::WarningMessage( wxT("STATISTICS: geometric mean unavailable because of negative data") );
      continue;
    }
    if( validIdx[i]==8 && !flag2 )
    {
      ExGlobals::WarningMessage( wxT("STATISTICS: root-mean-square unavailable because of negative data") );
      continue;
    }
    try
    {
      NumericVariable::PutVariable( p->GetString(i), stats[validIdx[i]], p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
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
  wxString command( Name()+wxT(": ") );
  if( sxx*syy <= 0.0 )throw ECommandError( command+wxT("cannot compute statistics with constant data") );
  r = sxy/sqrt(sxx*syy);
  double df = n-2;
  double t = r*sqrt( df/((1.0-r)+tiny)*((1.0+r)+tiny) );
  try
  {
    prob = UsefulFunctions::IncompleteBeta( df/(df+t*t), 0.5*df, 0.5 ); // i<x>(a,b)
  }
  catch( EExpressionError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  return;
}
 
// end of file
