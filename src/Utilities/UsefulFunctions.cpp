/*
Copyright (C) 2005, 2006, 2007 Joseph L. Chuma, TRIUMF

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
#include <limits>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "UsefulFunctions.h"
#include "EExpressionError.h"
#include "ExGlobals.h"
#include "Expression.h"
#include "NumericData.h"

namespace UsefulFunctions
{
void LeastSquaresFit( Expression *expr, wxString &lhs,
                      std::vector<double> &yVec, std::vector<double> &weightVec,
                      std::vector<double> &e1, std::vector<double> &e2,
                      std::vector<double> &p, std::vector<double> &pSave,
                      unsigned int ntmax, double tolerance, double &chi2,
                      double &confidenceLevel,
                      std::vector< std::vector<double> > &errmat, bool output,
                      bool zeros, bool poisson, bool marquardt, unsigned int &nfree )
{
  double lambda = 1.0e-3;
  double lambda_max = 1.0e4;
  double xxp = 1.0e35;
  double xxd = -1.0e30;
  double alpha = 1.0;
  std::size_t const N = yVec.size();
  std::size_t const nparam = expr->GetNumberOfFitParameters();
  std::vector<double> yf( N );
  std::vector<double> dd( N*nparam );
  std::vector<double> cc( nparam*(nparam+1)/2 );
  std::vector<double> pp( nparam ), vp( nparam );
  std::vector< std::vector<double> > vv( 1 );
  vv[0].resize( nparam );
  //
  //if( output )ExGlobals::WriteOutput( wxT("Iter#   Residual  Step Size") );
  //
  std::size_t nonZero = 0;      // number of non-zero weights
  double ymax = yVec[0];
  double ymin = ymax;
  for( std::size_t i=0; i<N; ++i )
  {
    if( yVec[i] > ymax )ymax = yVec[i];
    if( yVec[i] < ymin )ymin = yVec[i];
    if( zeros && weightVec[i] != 0.0 )++nonZero;
  }
  nfree = zeros ? nonZero : N;
  nfree -= nparam;
  //
  std::size_t nt = 1;
  int iv = 0;
TOP:
  vv[0].assign( nparam, 0.0 );
  cc.assign( nparam*(nparam+1)/2, 0.0 );
  double tt = 0.0;
  //
  // Update parameter values to current values
  //
  for( std::size_t j=0; j<nparam; ++j )expr->SetFitParameterValue( j, p[j] );
  //
  try
  {
    expr->FinalPass();
  }
  catch ( EExpressionError &e )
  {
    throw;
  }
  //std::vector<double> ymid;
  //ymid.reserve(expr->GetFinalAnswer().GetData().size());
  //ymid = expr->GetFinalAnswer().GetData();
  std::vector<double> ymid( expr->GetFinalAnswer().GetData() );
  //
  // Fill the array dd of partial derivatives
  // Using central difference formulation
  //
  for( std::size_t j=0; j<nparam; ++j )
  {
    p[j]==0.0 ? expr->SetFitParameterValue( j, -1.0 ) :
                expr->SetFitParameterValue( j, p[j]*(1.0-tolerance) );
    try
    {
      expr->FinalPass();
    }
    catch ( EExpressionError &e )
    {
      throw;
    }
    //std::vector<double> ylow;
    //ylow.reserve(expr->GetFinalAnswer().GetData().size());
    //ylow = expr->GetFinalAnswer().GetData();
    std::vector<double> ylow( expr->GetFinalAnswer().GetData() );
    //
    p[j]==0.0 ? expr->SetFitParameterValue( j, 1.0 ) :
                expr->SetFitParameterValue( j, p[j]*(1.0+tolerance) );
    try
    {
      expr->FinalPass();
    }
    catch ( EExpressionError &e )
    {
      throw;
    }
    //std::vector<double> yhi;
    //yhi.reserve(expr->GetFinalAnswer().GetData().size());
    //yhi = expr->GetFinalAnswer().GetData();
    std::vector<double> yhi( expr->GetFinalAnswer().GetData() );
    if( yhi.size() == 1 )
    {
      double tmp = p[j]==0.0 ? (yhi[0]-ylow[0])/2.0 : (yhi[0]-ylow[0])/(2.0*tolerance*p[j]);
      for( std::size_t i=0; i<N; ++i )dd[j+i*nparam] = tmp;
    }
    else
    {
      if( p[j] == 0.0 )
      {
        for( std::size_t i=0; i<N; ++i )dd[j+i*nparam] = (yhi[i]-ylow[i])/2.0;
      }
      else
      {
        for( std::size_t i=0; i<N; ++i )dd[j+i*nparam] = (yhi[i]-ylow[i])/(2.0*tolerance*p[j]);
      }
    }
    expr->SetFitParameterValue( j, p[j] );
  }
AGAIN:
  double xx = 0.0;
  int ij;
  for( std::size_t k=0; k<N; ++k )
  {
    double ymidk = ymid.size()==1 ? ymid[0] : ymid[k];
    ij = -1;
    double dy = yVec[k] - ymidk;
    if( poisson )
    {
      xx += yVec[k]*log(ymidk) - ymidk;
      for( std::size_t j=0; j<nparam; ++j )
      {
        for( std::size_t i=j; i<nparam; ++i )
        {
          cc[++ij] += dd[i+k*nparam]*dd[j+k*nparam]*yVec[k]/(ymidk*ymidk);
        }
        vv[0][j] += dd[j+k*nparam]*(yVec[k]/ymidk-1.0);
      }
    }
    else
    {
      xx += weightVec[k]*dy*dy;
      for( std::size_t j=0; j<nparam; ++j )
      {
        for( std::size_t i=j; i<nparam; ++i )
        {
          cc[++ij] += weightVec[k]*dd[i+k*nparam]*dd[j+k*nparam];
          if( marquardt && (i==j) )cc[ij] *= 1.0+lambda;
        }
        vv[0][j] += weightVec[k]*dy*dd[j+k*nparam];
      }
    }
  }
  //if( output )
  //{
  //  wxChar c[100];
  //  sprintf( c, wxT("%5d%11.3lg%11.3lg"), nt, xx, alpha );
  //  ExGlobals::WriteOutput( c );
  //}
  if( poisson )
  {
    if( xx-xxd < -0.01 )
    {
      alpha /= 2.0;
      if( alpha < 0.0001 )
        throw EExpressionError( wxT("fit fails due to ill-conditioning: alpha < 0.0001\n\ntry adjusting the parameter start values") );
      vv[0] = vp;
      for( std::size_t i=0; i<nparam; ++i )
      {
        double tc;
        p[i] = pp[i] + alpha*vp[i];
        p[i]==0.0 ? tc=fabs(vv[0][i]) : tc=fabs(vv[0][i]/p[i]);
        if( tc > tt )tt = tc;
      }
      if( tt<tolerance && alpha==1.0 )iv = 1;
      goto TOP;
    }
    xxd = xx;
    alpha = 1.0;
  }
  else
  {
    if( xx <= 1.0e-10 ) // take care of very small residual xx
    {
      iv = 1;         // indicate a successful fit
      for( std::size_t i=0; i<nparam; ++i )
      {
        errmat[i].assign( nparam, 0.0 );
        errmat[i][i] = 1.0;
      }
      try
      {
        SolveLinearEquations( cc, errmat );
      }
      catch ( EExpressionError &e )
      {
        throw;
      }
      for( std::size_t i=0; i<nparam; ++i )
      {
        for( std::size_t j=0; j<nparam; ++j )p[i] += errmat[j][i]*vv[0][j];
      }
      goto DONE;
    }
    if( marquardt )
    {
      if( xx > xxp )
      {
        lambda *= 10.0;
        xxp = xx;
        goto AGAIN;
      }
      if( xx <= xxp )
      {
        lambda /= 10.0;
        xxp = xx;
      }
    }
    else
    {
      if( xx/xxp >= 1.05 )
      {
        alpha /= 2.0;
        if( alpha < 0.0001 )
          throw EExpressionError(wxT("fit fails due to ill-conditioning: alpha < 0.0001\n\ntry adjusting the parameter start values") );
        vv[0] = vp;
        for( std::size_t i=0; i<nparam; ++i )
        {
          double tc;
          p[i] = pp[i] + alpha*vp[i];
          p[i]==0.0 ? tc=fabs(vv[0][i]) : tc=fabs(vv[0][i]/p[i]);
          if( tc > tt )tt = tc;
        }
        if( tt<tolerance && alpha==1.0 )iv = 1;
        goto TOP;
      }
      xxp = xx;
      alpha = 1.0;
    }
  }
  pSave = p;
  if( iv == 1 )
  {
    for( std::size_t i=0; i<nparam; ++i )
    {
      errmat[i].assign( nparam, 0.0 );
      errmat[i][i] = 1.0;
    }
    try
    {
      SolveLinearEquations( cc, errmat );
    }
    catch ( EExpressionError &e )
    {
      throw;
    }
    for( std::size_t i=0; i<nparam; ++i )
    {
      for( std::size_t j=0; j<nparam; ++j )p[i] += errmat[j][i]*vv[0][j];
    }
    goto DONE;
  }
  if( nt >= ntmax )
  {
    iv = 1;         // emulates a successful termination
    for( std::size_t i=0; i<nparam; ++i )
    {
      errmat[i].assign( nparam, 0.0 );
      errmat[i][i] = 1.0;
    }
    try
    {
      SolveLinearEquations( cc, errmat );
    }
    catch ( EExpressionError &e )
    {
      throw;
    }
    for( std::size_t i=0; i<nparam; ++i )
    {
      for( std::size_t j=0; j<nparam; ++j )p[i] += errmat[j][i]*vv[0][j];
    }
    goto DONE;
  }
  if( nt < ntmax )
  {
    ++nt;
    try
    {
      SolveLinearEquations( cc, vv );
    }
    catch ( EExpressionError &e )
    {
      throw;
    }
    pp = p;
    vp = vv[0];
    for( std::size_t i=0; i<nparam; ++i )
    {
      double tc;
      p[i] += alpha*vv[0][i];
      p[i]==0.0 ? tc=fabs(vv[0][i]) : tc=fabs(vv[0][i]/p[i]);
      if( tc > tt )tt = tc;
    }
    if( tt < tolerance && alpha == 1.0 )iv = 1;
    goto TOP;
  }
  else if( nt == ntmax )
  {
    for( std::size_t i=0; i<nparam; ++i )
    {
      errmat[i].assign( nparam, 0.0 );
      errmat[i][i] = 1.0;
    }
    try
    {
      SolveLinearEquations( cc, errmat );
    }
    catch ( EExpressionError &e )
    {
      throw;
    }
    for( std::size_t i=0; i<nparam; ++i )
    {
      for( std::size_t j=0; j<nparam; ++j )p[i] += errmat[j][i]*vv[0][j];
    }
  }
DONE:
  for( std::size_t i=0; i<nparam; ++i )
  {
    e1[i] = sqrt(errmat[i][i]);
    expr->SetFitParameterValue( i, p[i] ); // update parameter values to current values
  }
  pSave = p;
  try
  {
    expr->FinalPass();
  }
  catch ( EExpressionError &e )
  {
    throw;
  }
  std::vector<double> temp( expr->GetFinalAnswer().GetData() );
  ymid.assign( temp.begin(), temp.end() );
  //ymid.reserve(expr->GetFinalAnswer().GetData().size());
  //ymid = expr->GetFinalAnswer().GetData();
  chi2 = 0.0;
  if( ymid.size() == 1 )
  {
    if( poisson )
    {
      for( std::size_t i=0; i<N; ++i )
      {
        yVec[i]==0.0 ? chi2 += 2.0*(ymid[0] - yVec[i]) :
                       chi2 += 2.0*(ymid[0] - yVec[i] + yVec[i]*log(yVec[i]/ymid[0]));
      }
    }
    else
    {
      for( std::size_t i=0; i<N; ++i )
        chi2 += weightVec[i]*(yVec[i]-ymid[0])*(yVec[i]-ymid[0]);
    }
  }
  else
  {
    if( poisson )
    {
      for( std::size_t i=0; i<N; ++i )
      {
        yVec[i]==0.0 ? chi2 += 2.0*(ymid[i] - yVec[i]) :
                       chi2 += 2.0*(ymid[i] - yVec[i] + yVec[i]*log(yVec[i]/ymid[i]));
      }
    }
    else
    {
      for( std::size_t i=0; i<N; ++i )
        chi2 += weightVec[i]*(yVec[i]-ymid[i])*(yVec[i]-ymid[i]);
    }
  }
  double fi = chi2/nfree;
  double sfi = sqrt(fi);
  for( std::size_t i=0; i<nparam; ++i )e2[i] = e1[i]*sfi;
  double rms = sqrt(chi2/N);
  //
  // confidenceLevel = confidence level
  // as given by `physical review d - particles and fields', volume 45,
  // third series, number 11, 1 june 1992, part ii, pages iii.32 - iii.42
  //
  confidenceLevel = 100.0*Chisq( chi2, static_cast<int>(nfree) );
  if( output )
  {
    ExGlobals::WriteOutput(wxT(" "));
    ExGlobals::WriteOutput(wxT("********************* FIT RESULTS **********************"));
    ExGlobals::WriteOutput(wxT(" "));
    ExGlobals::WriteOutput(wxString(wxT(" FIT: "))+lhs+wxT(" = ")+expr->GetString());
    ExGlobals::WriteOutput(wxT(" "));
    ExGlobals::WriteOutput(wxT("   parameter            value               e1             e2"));
    for( std::size_t i=0; i<nparam; ++i )
    {
      wxChar c[100];
      ::wxSnprintf( c, 100, wxT("%12s    %15.5lg      %15.5lg%15.5lg"),
                    expr->GetFitParameterName(i).c_str(), pp[i], e1[i], e2[i] );
      ExGlobals::WriteOutput( c );
    }
    ExGlobals::WriteOutput(wxT(" "));
    wxString s( wxT("number of degrees of freedom = ") );
    s << nfree;
    ExGlobals::WriteOutput( s );
    if( zeros )
    {
      s = wxT("number of zero weights = ");
      ExGlobals::WriteOutput( s << N-nonZero );
    }
    s = wxT("total chisq = ");
    ExGlobals::WriteOutput( s << chi2-0 );
    s = wxT("sqrt( chisq/point) = ");
    ExGlobals::WriteOutput( s << rms-0 );
    s = wxT("chisq/(degrees of freedom) = ");
    ExGlobals::WriteOutput( s << fi );
    s = wxT("confidence level = ");
    ExGlobals::WriteOutput( s << confidenceLevel << wxT('%') );
    ExGlobals::WriteOutput(wxT(" "));
    ExGlobals::WriteOutput( wxT("correlation matrix:") );
    std::vector<double> corr( nparam*nparam, 0.0 );
    for( std::size_t i=0; i<nparam; ++i )
    {
      for( std::size_t j=0; j<=i; ++j )
      {
        corr[j+i*nparam] = 1.0;
        if( errmat[j][i] == 0.0 )corr[j+i*nparam] = 0.0;
        if( errmat[j][j]*errmat[i][i] > 0.0 )
        {
          corr[j+i*nparam] = errmat[j][i]/sqrt(errmat[j][j]*errmat[i][i]);
          corr[i+j*nparam] = corr[j+i*nparam];
        }
      }
    }
    for( std::size_t i=0; i<nparam; ++i )
    {
      wxString tmp;
      for( std::size_t j=0; j<nparam; ++j )
      {
        if( corr[j+i*nparam] >= 0.0 )tmp += wxT(" ");
        tmp += wxString::Format(wxT("%10.3e"),corr[j+i*nparam]) + wxT("  ");
        /*
        std::stringstream ss;
        std::string s;
        ss << std::setw(10) << std::setprecision(3) << std::scientific << corr[j+i*nparam];
        ss >> s;
        if( corr[j+i*nparam] >= 0.0 )tmp = wxT(' ') + wxString((wxChar*)s.c_str()) + wxT("  ");
        else                         tmp = wxString((wxChar*)s.c_str()) + wxT("  ");
        */
      }
      ExGlobals::WriteOutput( tmp );
    }
    ExGlobals::WriteOutput(wxT(" "));
    ExGlobals::WriteOutput( wxT("covariance matrix:") );
    for( std::size_t i=0; i<nparam; ++i )
    {
      wxString tmp;
      for( std::size_t j=0; j<nparam; ++j )
      {
        if( errmat[j][i] >= 0.0 )tmp += wxT(" ");
        tmp += wxString::Format(wxT("%10.3e"),errmat[j][i]) + wxT("  ");
        /*
        std::stringstream ss;
        std::string s;
        ss << std::setw(10) << std::setprecision(3) << std::scientific << errmat[j][i];
        ss >> s;
        if( errmat[j][i] >= 0.0 )tmp = wxT(' ') + wxString((wxChar*)s.c_str()) + wxT("  ");
        else                     tmp = wxString((wxChar*)s.c_str()) + wxT("  ");
        */
      }
      ExGlobals::WriteOutput( tmp );
    }
    ExGlobals::WriteOutput(wxT(" "));
    ExGlobals::WriteOutput(wxT("********************************************************"));
    ExGlobals::WriteOutput(wxT(" "));
  }
  if( iv != 1 )
  {
    wxString s( wxT("more than ") );
    s += ntmax + wxT(" iterations needed for convergence");
    throw EExpressionError( s );
  }
}

void SolveLinearEquations( std::vector<double> &a, std::vector< std::vector<double> > &b )
{
  //
  // This routine, obtained sans documentation from the University of
  // British Columbia computing centre, solves a linear symmetric system
  // of equations with multiple right hand side vectors. The lower half of
  // the symmetric matrix representing the system is packed linearly into
  // the 1-dim. array A. The array of right hand sides is passed in B,
  // with L indicating the number of right hand sides for which the system
  // is to be solved. M is the number of points passed in A, and N is the
  // number of parameters in the least squares fit.
  // Note: M=N*(N+1)/2
  //
  // Parameters:
  //  a: array containing lower triangular half of symmetric
  //     matrix (left hand side of linear system to be solved),
  //     dimensioned as a linear (1-dim.) array
  //  b: array of right hand side vectors for which the system
  //     is to be solved, dimensioned as a 2-dim. array
  //
  std::size_t m = a.size();
  std::size_t nRhs = b.size();      // number of right hand sides stored in b
  std::size_t nparam = b[0].size(); // number of parameters in original least squares fit
  //
  if( a[0] <= 0.0 )
    throw EExpressionError( wxT("(Error #1) Fit fails due to ill-conditioning\n\ntry adjusting the parameter start values") );
  //
  if( m == 1 )
  {
    b[0][0] = b[0][0]/a[0];
    return;
  }
  a[0] = 1.0/sqrt(a[0]);
  for( std::size_t i=1; i<nparam; ++i )a[i] *= a[0];
  int inc = nparam;
  int in = nparam;
L20:
  --inc;
  int i1 = in+1;
  in += inc;
  //
  double x = 0.0;
  std::size_t isub = i1;
  for( std::size_t i=inc; i<nparam; ++i )
  {
    isub -= i;
    x += a[isub-1]*a[isub-1];
  }
  if( a[i1-1] < x )
    throw EExpressionError( wxT("(Error #2) Fit fails due to ill-conditioning\n\ntry adjusting the parameter start values") );
  //
  a[i1-1] = sqrt(a[i1-1] - x);
  if( a[i1-1] == 0.0 )
    throw EExpressionError( wxT("(Error #3) Fit fails due to ill-conditioning\n\ntry adjusting the parameter start values") );
  //
  a[i1-1] = 1.0/a[i1-1];
  if( inc != 1 )
  {
    for( int i=i1+1; i<=in; ++i )
    {
      x = 0.0;
      int l1 = i1 - inc;
      int l2 = i - inc;
      for( std::size_t j=1; j<=nparam-inc; ++j )
      {
        x += a[l1-1]*a[l2-1];
        l1 -= inc + j;
        l2 -= inc + j;
      }
      a[i-1] = (a[i-1]-x)*a[i1-1];
    }
    goto L20;
  }
  for( std::size_t k=0; k<nRhs; ++k )
  {
    b[k][0] = b[k][0]*a[0];
    for( std::size_t i=2; i<=nparam; ++i )
    {
      std::size_t jm = i-1;
      isub = i;
      inc = nparam;
      x = 0.0;
      for( std::size_t j=1; j<=jm; ++j )
      {
        x = a[isub-1]*b[k][j-1] + x;
        --inc;
        isub += inc;
      }
      b[k][i-1] = (b[k][i-1]-x)*a[isub-1];
    }
    b[k][nparam-1] = b[k][nparam-1]*a[m-1];
    inc = -1;
    std::size_t j1 = m+1;
    for( std::size_t i=2; i<=nparam; ++i )
    {
      ++inc;
      std::size_t jm = j1 - 2;
      j1 = jm - inc;
      std::size_t jsub = nparam-inc-1;
      std::size_t ii = jsub;
      x = 0.0;
      for( std::size_t j=j1; j<=jm; ++j )
      {
        ++jsub;
        x += a[j-1]*b[k][jsub-1];
      }
      b[k][ii-1] = (b[k][ii-1]-x)*a[j1-2];
    }
  }
}

void LQline( std::vector<double> const &xin, std::vector<double> const &yin,
             double &a, double &b, double &xcm, double &ycm )
{
  // Fit line y=ax+b to data by least y squares 
  // also calculate xcm and ycm (center of mass)
  //
  // Calculate sums of:  xin, yin, xin^2, yin^2, xin*yin
  //
  double sumx = 0.0;
  double sumy = 0.0;
  double sumxsq = 0.0;
  double sumysq = 0.0;
  double sumxy = 0.0;
  int npts = xin.size();
  for( int i=0; i<npts; ++i )
  {
    sumx += xin[i];
    sumy += yin[i];
    sumxsq += xin[i]*xin[i];
    sumysq += yin[i]*yin[i];
    sumxy += xin[i]*yin[i];
  }
  //
  // Calculate center of masses
  //
  xcm = sumx/npts;
  ycm = sumy/npts;
  //
  // Find a and b for least y^2 line y=ax+b
  //
  a = (npts*sumxy - sumx*sumy) / (npts*sumxsq - sumx*sumx);
  b = (sumy - a*sumx) / npts;
}

void LQline( std::vector<double> const &xin, std::vector<double> const &yin,
             double &a, double &b, double &c, double &xcm, double &ycm )
{
  // Fit line ax+by+c=0 by least perpendicular square method
  //           (equation is normalized to a*a+b*b=1, a>=0)
  // also calculate xcm and ycm (centers of mass)
  //
  // Calculate sums of:  xin, yin, xin^2, yin^2, xin*yin
  //
  double sumx = 0.0;
  double sumy = 0.0;
  double sumxsq = 0.0;
  double sumysq = 0.0;
  double sumxy = 0.0;
  int npts = xin.size();
  for( int i=0; i<npts; ++i )
  {
    sumx += xin[i];
    sumy += yin[i];
    sumxsq += xin[i]*xin[i];
    sumysq += yin[i]*yin[i];
    sumxy += xin[i]*yin[i];
  }
  //
  // Calculate center of masses
  //
  xcm = sumx/npts;
  ycm = sumy/npts;
  //
  // Find a, b, and c for least perpendicular**2 line  a*x + b*y + c = 0
  //
  a = 0;
  b = 1;
  c = ycm;
  double d = npts*sumxsq - npts*sumysq - sumx*sumx + sumy*sumy;
  double e = npts*sumxy - sumx*sumy;
  double const eps = 0.000001;
  if( fabs(d)<eps && fabs(e)<eps )return;
  double f = e*e / (d*d + 4*e*e);
  double asq = d>0.0 ? (1-sqrt(1-4*f)) : (1+sqrt(1-4*f));
  a = sqrt(asq/2);
  b = a>eps ? Sign(1.0,-e)*sqrt(1-asq/2) : 1;
  if( (a>eps) && (fabs(d)>eps) )b = ((asq-1)*e)/(a*d);
  c = -(a*sumx+b*sumy)/npts;
}

void EllipseFit( std::vector<double> const &xin, std::vector<double> const &yin, double const frac,
                 double &xcentre, double &ycentre, double &a, double &b, double &angle )
{
  //
  // EllipseFit fits an ellipse to the input data points (xin,yin) such that the ellipse
  // contains frac of the input points. The center of the ellipse (xcentre,ycentre), the
  // major and minor axis (a & b), and the angle in degrees with the x-axis, angle,
  // where 0 < angle <= 180, are returned.  The ellipse is found as follows:
  //  1  the major axis is found by least squares fitting a line
  //     through the data points, also the ellipse center is found.
  //  2  The ratio of the major to minor axis is found by computing
  //     the second moments (standard deviations) about the major
  //     and minor axes.
  //  3  The minimum value of the major axis is found for each point
  //     so that that point will be inside the ellipse, then a value
  //     of the major axis is picked to be greater than or equal to
  //     frac of the values.
  //
  //  A check for small aspect ratio (and possible floating overflow) could cause
  //  an erroneous ellipse, not enclosing the correct fraction of points, to be
  //  returned in cases where the xin and yin data differ in order of magnitude.
  //
  // Fit least perpendicular squared line to data, equation of line is
  // u*x + v*y + w = 0, where u*u + v*v = 1
  //
  double u, v, w;
  LQline( xin, yin, u, v, w, xcentre, ycentre );
  //
  // Find angle of ellipse (angle from x-axis counterclockwise)
  //
  double angle2 = u==0.0 ? 0.0 : atan2(u,-v);
  //
  // The x" axis is the axis along the line u*x + v*y + w = 0
  // with zero at the point (xcentre,ycentre)
  // The y" axis is perpendicular to the x" axis
  // calculate new arrays x" (called xpp) and y" (called ypp)
  //
  double sina = sin(angle2);
  double cosa = cos(angle2);
  double const conv = 180.0/acos(-1.0);
  angle = angle2*conv;
  std::vector<double> xpp, ypp, effa;
  int const npts = xin.size();
  for( int i=0; i<npts; ++i )
  {
    double xp = xin[i]-xcentre;
    double yp = yin[i]-ycentre;
    xpp.push_back( xp*cosa + yp*sina );
    ypp.push_back( yp*cosa - xp*sina );
  }
  //
  // Find variances of xpp, and of ypp
  //
  double syppsq = 0.0;
  double sxppsq = 0.0;
  for( int i=0; i<npts; ++i )
  {
    syppsq += ypp[i]*ypp[i];
    sxppsq += xpp[i]*xpp[i];
  }
  double xvari = sxppsq/npts;
  double yvari = syppsq/npts;
  double const eps = 1.0e-30;
  if( xvari <= yvari*eps )throw std::runtime_error("EllipseFit: xvari<=yvari*1e-30");
  double bovera = sqrt(yvari/xvari);
  double boasq = bovera*bovera;
  //
  // Now find values effa for each point, which is the a of ellipse
  // which would place each point on the perimeter
  //
  for( int i=0; i<npts; ++i )
  {
    if( fabs(boasq) <= ypp[i]*ypp[i]*eps )
      throw std::runtime_error("EllipseFit: fabs(boasq)<=ypp[i]^2*1e-30");
    effa.push_back( sqrt(xpp[i]*xpp[i]+ypp[i]*ypp[i]/boasq) );
  }
  //
  // Sort effective a's into increasing order
  //
  QuickSort( effa, true );
  //
  // Finally, find a value to be used, which is the value
  // which frac of the values in effa is less than
  //
  a = effa[static_cast<int>(frac*npts+0.5)];
  b = a*bovera;
}


double Median( std::vector<double> &x )
{
  // Derived from:
  //  NUMERICAL RECIPES, THE ART OF SCIENTIFIC COMPUTING   by
  //  William H. Press, Brian P. Flannery, Saul A. Teukolsky, and William T. Vetterling;
  //  Cambridge Univ. Press, 1987;   pp 460-462
  //
  double const big = std::numeric_limits<double>::max();
  double const afac = 1.5;
  double const amp = 1.5;
  //
  std::size_t n = x.size();
  double a = 0.5*(x[0]+x[n-1]);
  double eps = fabs(x[n-1]-x[0]);
  double ap = big;
  double am = -big;
  std::size_t npass = 0;
  double result;
L10:
  double sum = 0.0;
  ++npass;
  if( npass > 20 )
  {
    QuickSort( x, true );
    n%2 == 0 ? result = (x[n/2-1]+x[n/2])/2.0 : result = x[(n+1)/2-1];
    return result;
  }
  double sumx = 0.0;
  int np = 0;
  int nm = 0;
  double xp = big;
  double xm = -big;
  for( std::size_t j=0; j<n; ++j )
  {
    double xx = x[j];
    if( xx != a )
    {
      if( xx > a )
      {
        ++np;
        if( xx < xp )xp = xx;
      }
      else if( xx < a )
      {
        ++nm;
        if( xx > xm )xm = xx;
      }
      double dum = 1./(eps+fabs(xx-a));
      sum += dum;
      sumx += xx*dum;
    }
  }
  if( np-nm >= 2 )
  {
    am = a;
    double aa = xp + std::max(0.0,sumx/sum-a)*amp;
    if( aa > ap )aa = 0.5*(a+ap);
    eps = afac*fabs(aa-a);
    a = aa;
    goto L10;
  }
  else if( nm-np >= 2 )
  {
    ap = a;
    double aa = xm + std::min(0.0,sumx/sum-a)*amp;
    if( aa < am )aa = 0.5*(a+am);
    eps = afac*fabs(aa-a);
    goto L10;
  }
  if( n%2 == 0 )
  {
    if( np == nm )
    {
      result = 0.5*(xp+xm);
    }
    else if( np > nm )
    {
      result = 0.5*(a+xp);
    }
    else
    {
      result = 0.5*(xm+a);
    }
  }
  else
  {
    if( np == nm )
    {
      result = a;
    }
    else if( np > nm )
    {
      result = xp;
    }
    else
    {
      result = xm;
    }
  }
  return result;
}

void Scale1( double &xminp, double &xmaxp, double &xinc, int n, double xmin, double xmax )
{
  //
  // Scale1:  given xmin, xmax, and n, Scale1 finds a new range xminp to xmaxp
  //          divisible into approximately n linear intervals of size xinc.
  //          xminp and xmaxp are integer multiples of xinc.
  //
  // source:  algorithm 463
  //          Lewart, C. R.  1973
  //          Comm. ACM 16: 639-640
  //
  // parameter definitions:
  //
  // xmin   : minimum of values that axis must encompass
  // xmax   : maximum of values that axis must encompass
  // n      : requested number of axis intervals
  // xminp  : returned axis minimum
  // xmaxp  : returned axis maximum
  // xinc   : returned length of axis intervals
  //
  // check whether proper input supplied to scale1
  //
  if( n <= 0 )throw std::runtime_error( "Scale1: requested number of intervals <= 0" );
  //
  if( xmin > xmax )throw std::runtime_error( "Scale1: minimum > maximum" );
  //
  if( xmin == xmax )
  {
    if( xmin == 0.0 )
    {
      xmin = -1.0;
      xmax = +1.0;
    }
    else if( xmin < 0.0 )
    {
      xmin = 2*xmin;
      xmax = 0.0;
    }
    else
    {
      xmin = 0.0;
      xmax = 2*xmax;
    }
  }
  //
  // vint is an array of acceptable values for xinc (times an integer power of 10)
  // sor is an array of geometric means of adjacent values of vint;
  // sor is used as break points to determine which value of vint to assign to xinc
  //
  double const vint[4] = { 1., 2., 5., 10. };
  double const sor[3] = { 1.414214, 3.162278, 7.071068 };
  //
  double fn = n;
  //
  // find approximate interval size a
  //
  double a = (xmax - xmin)/fn;
  double al = log10(a);
  int nal = int(al);
  if( a < 1.0 )--nal;
  //
  // a is scaled into variable b between 1 and 10
  //
  if( nal > std::numeric_limits<double>::max_exponent10 )
    throw std::runtime_error( "Scale1: exponent of 10 too large" );
  double b = a/pow(10.,nal);
  //
  // the closest permissible value for b is found
  //
  int i = 3;
  for( int j = 0; j < 3; ++j )
  {
    if( b < sor[j] )
    {
      i = j;
      break;
    }
  }
  //
  // del accounts for computer roundoff.  del should be greater
  // than the roundoff expected from a division and double
  // operation, it should be less than the minimum increment of
  // the plotting device used by the main program divided by
  // the plot size times the number of intervals n.
  //
  double const del = 0.00002;
  //
  // the interval size is computed
  //
  xinc = vint[i] * pow(10.,nal);
  double fm1 = xmin / xinc;
  int m1 = int(fm1);
  if( fm1 < 0.0 )--m1;
  if( fabs(m1+1-fm1) < del )++m1;
  //
  // the new minimum and maximum limits are found
  //
  xminp = xinc * m1;
  double fm2 = xmax / xinc;
  int m2 = int(fm2 + 1);
  if( fm2 < -1.0 )--m2;
  if( fabs(fm2+1-m2) < del )--m2;
  xmaxp = xinc * m2;
  //
  // adjust limits to account for roundoff if necessary
  //
  if( xminp > xmin )xminp = xmin;
  if( xmaxp < xmax )xmaxp = xmax;
}

void Scale2( double &xmins, double &xmaxs, double &dist, int n, double xmin, double xmax )
{
  // Scale2: given xmin and xmax (xmin <= xmax) and n > 1, the
  //         desired range and number of intervals of the scale,
  //         Scale2 determines xmins, xmaxs, and dist the new
  //         range and interval of the scale. xmins, xmaxs,
  //         and dist have the following properties:
  //
  //        1) xmins <= xmin <= xmax <= xmaxs = xmins+n*dist
  //        2) xmins and xmaxs are integral multiples of dist
  //        3) dist = sval[i]*pow(10,m) for some exponent m, where
  //           sval[i] is a nice number between 1 and 10
  //
  // algorithm used for scaling is from: page 640 of
  // Communications of the ACM, Oct. 1973, vol. 16, number 10
  //
  // input  parameters: xmin,xmax,n,sval
  //
  // output parameters: xmins,xmaxs,dist
  //
  // xmin  : minimum x-value of the desired range of the scale
  // xmax  : maximum x-value of the desired range of the scale
  // n     : number of scale intervals, (n > 1)
  // xmins : minimum x-value of the new scale
  // xmaxs : maximum x-value of the new scale
  // dist  : interval size of the new scale (dist > 0)
  //
  if( xmin > xmax )throw std::runtime_error( "Scale2: xmin must be <= xmax" );
  //
  if( n < 1 )throw std::runtime_error( "Scale2: n must be > 1" );
  //
  double const sval[10] = { 1.0, 1.2, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 8.0 };
  //
  // find approximate interval size a
  //
  double xmaxx = xmax;
  if( xmaxx == xmin )xmaxx = xmin + n*fabs(xmin);
  if( xmaxx == xmin )xmaxx = xmin + n;
  double a = (xmaxx-xmin)/n;
  int nal = static_cast<int>(log10(a));
  if( a < 1.0 )--nal;
  //
  // scale a into a variable b between 1 and 10 (1 <= b <= 10)
  //
  if( nal > std::numeric_limits<double>::max_exponent10 )
    throw std::runtime_error( "Scale2: exponent of 10 too large" );
  double b = a/pow(10.,nal);
  //
  // find the closest wxT("nice") value => b
  //
  double const del = 0.00002;
  int i;
  bool test = true;
  for( i=0; i<10; ++i )
  {
    if( b <= sval[i]+del )
    {
      test = false;
      break;
    }
  }
  if( test )
  {
    i = 0;
    ++nal;
  }
  //
  // compute the interval size: dist
  //
  int ns = n+1;
  while ( ns > n )
  {
    dist = sval[i]*pow(10.,nal);
    //
    // find the new maximum and minimum scale limits: xmins and xmaxs
    //
    double fmin = xmin/dist;
    int min = int(fmin);
    if( fmin < 0.0 )--min;
    if( fabs(min+1-fmin) <= del )++min;
    xmins = min*dist;
    //
    double fmax = xmaxx/dist;
    int max = int(fmax);
    if( fmax > 0.0 )++max;
    if( fabs(max-1-fmax) <= del )--max;
    xmaxs = max*dist;
    //
    // check whether another pass is required
    //
    ns = max-min;
    if( ns <= n )break;
    if( ++i >= 10 )
    {
      i = 0;
      ++nal;
    }
  }
  int nd = (n-ns)/2;
  xmins -= nd*dist;
  if( xmin >= 0.0 && xmins < 0.0 )xmins = 0.0;
  xmaxs = xmins + n*dist;
  if( xmaxx <= 0.0 && xmaxs < 0.0 )xmaxs = 0.0;
  xmins = xmaxs - n*dist;
  //
  // adjust limits to account for round-off if necessary
  //
  if( xmins > xmin )xmins = xmin;
  if( xmaxs < xmaxx )xmaxs = xmaxx;
}

double GetSimpleRandomNumber()
{
  // Prime modulus multiplicative linear congruential generator
  // Z(I) = (630360016 * Z(I - 1)) (MOD(2**31 - 1)), based on Marse
  // and Roberts portable random-number generator UNIRAN.
  //
  int const mult1 = 24112;
  int const mult2 = 26143;
  int const b2e15 = 32768;
  int const b2e16 = 65536;
  int const modlus = 2147483647;
  //
  // set the default seeds for all 100 streams
  //
  static int iz[100] =
  { 1973272912, 281629770,  20006270,1280689831,2096730329,
    1933576050, 913566091, 246780520,1363774876, 604901985,
    1511192140,1259851944, 824064364, 150493284, 242708531,
      75253171,1964472944,1202299975, 233217322,1911216000,
     726370533, 403498145, 993232223,1103205531, 762430696,
    1922803170,1385516923,  76271663, 413682397, 726466604,
     336157058,1432650381,1120463904, 595778810, 877722890,
    1046574445,  68911991,2088367019, 748545416, 622401386,
    2122378830, 640690903,1774806513,2132545692,2079249579,
      78130110, 852776735,1187867272,1351423507,1645973084,
    1997049139, 922510944,2045512870, 898585771, 243649545,
    1004818771, 773686062, 403188473, 372279877,1901633463,
     498067494,2087759558, 493157915, 597104727,1530940798,
    1814496276, 536444882,1663153658, 855503735,  67784357,
    1432404475, 619691088, 119025595, 880802310, 176192644,
    1116780070, 277854671,1366580350,1142483975,2026948561,
    1053920743, 786262391,1792203830,1494667770,1923011392,
    1433700034,1244184613,1147297105, 539712780,1545929719,
     190641742,1645390429, 264907697, 620389253,1502074852,
     927711160, 364849192,2049576050, 638580085, 547070247 };
  //
  // generate the next random number
  //
  int const istrm = 10;
  int izi = iz[istrm];
  int hi15 = izi/b2e16;
  int lowprd = (izi - hi15*b2e16)*mult1;
  int low15 = lowprd/b2e16;
  int hi31 = hi15*mult1 + low15;
  int ovflow = hi31/b2e15;
  izi = (((lowprd - low15*b2e16) - modlus) +
         (hi31 - ovflow*b2e15)*b2e16) + ovflow;
  if( izi < 0 )izi += modlus;
  hi15 = izi/b2e16;
  lowprd = (izi - hi15*b2e16)*mult2;
  low15 = lowprd/b2e16;
  hi31 = hi15*mult2 + low15;
  ovflow = hi31/b2e15;
  izi = (((lowprd - low15*b2e16) - modlus) +
         (hi31 - ovflow*b2e15)*b2e16) + ovflow;
  if( izi < 0 )izi += modlus;
  iz[istrm] = izi;
  return (2*(izi/256) + 1)/16777216.0;
}

void QuickSort( std::vector<double> &x, bool increasing )
{
  // Reference: Slatec Library, category M1
  //
  // Sort sorts array x. The array x may be sorted in increasing order
  // or decreasing order. A slightly modified QuickSort algorithm is used.
  //
  // Alogrithm reference
  // Singleton, R.C., Algorithm 347, An Efficient Algorithm for Sorting with
  //                                 Minimal Storage, CACM,12(3),1969,185-7.
  //
  double il[21], iu[21];
  std::size_t n = x.size();
  //
  // alter array x to get decreasing order if needed
  //
  if( !increasing )
  {
    for( std::size_t i=0; i<n; ++i )x[i] = -1.*x[i];
  }
  int m = 1;
  int i = 1;
  int j = n;
  int ij, k, l;
  double r = 0.375;
  double t, tt;
L110:
  if( i == j )
  {
    if( --m == 0 ) // clean up
    {
      if( !increasing )
      {
        for( std::size_t i=0; i<n; ++i )x[i] = -1.*x[i];
      }
      return;
    }
    i = static_cast<int>(il[m-1]);
    j = static_cast<int>(iu[m-1]);
    goto L160;
  }
  r > 0.5898437 ? r -= 0.21875 : r += 0.0390625;
L125:
  k = i;
  //
  // select a central element of the array and save it in location t
  //
  ij = i + static_cast<int>( (j-i)*r );
  t = x[ij-1];
  //
  // if first element of array is greater than t, interchange with t
  //
  if( x[i-1] > t )
  {
    x[ij-1] = x[i-1];
    x[i-1] = t;
    t = x[ij-1];
  }
  l = j;
  //
  if( x[j-1] < t ) // interchange with t
  {
    x[ij-1] = x[j-1];
    x[j-1] = t;
    t = x[ij-1];
    if( x[i-1] > t ) // interchange with t
    {
      x[ij-1] = x[i-1];
      x[i-1] = t;
      t = x[ij-1];
    }
  }
L140:
  do // find an element in the second half of the array which is smaller than t
  {
    --l;
  }
  while( x[l-1] > t );
  //
  // find an element in the first half of the array which is greater than t
  //
  do
  {
    ++k;
  }
  while( x[k-1] < t );
  if( k <= l ) // interchange these elements
  {
    tt = x[l-1];
    x[l-1] = x[k-1];
    x[k-1] = tt;
    goto L140;
  }
  //
  // save upper and lower subscripts of the array yet to be sorted
  //
  if( l-i > j-k )
  {
    il[m-1] = i;
    iu[m-1] = l;
    i = k;
  }
  else
  {
    il[m-1] = k;
    iu[m-1] = j;
    j = l;
  }
  ++m;
  //
  // begin again on another portion of the unsorted array
  //
L160:
  if( j-i >= 1 )goto L125;
  if( i == 1 )goto L110;
  --i;
  for( ;; )
  {
    if( ++i == j )
    {
      if( --m == 0 ) // clean up
      {
        if( !increasing )
        {
          for( std::size_t i=0; i<n; ++i )x[i] = -1.*x[i];
        }
        return;
      }
      i = static_cast<int>(il[m-1]);
      j = static_cast<int>(iu[m-1]);
      goto L160;
    }
    t = x[i];
    if( x[i-1] > t )
    {
      k = i;
      do
      {
        x[k] = x[k-1];
        --k;
      }
      while( t < x[k-1] );
      x[k] = t;
    }
  }
}

void QuickSort( std::vector<double> &x, std::vector<double> &y,
                                 bool increasing )
{
  // Reference: Slatec Library, category M1
  //
  // Sort sorts array x and makes the same interchanges in array y.
  // The array x may be sorted in increasing order or decreasing order.
  // A slightly modified QuickSort algorithm is used.
  //
  // Alogrithm reference
  // Singleton, R.C., Algorithm 347, An Efficient Algorithm for Sorting with
  //                                 Minimal Storage, CACM,12(3),1969,185-7.
  //
  double il[21], iu[21];
  std::size_t n = x.size();
  //
  // alter array x to get decreasing order if needed
  //
  if( !increasing )
  {
    for( std::size_t i=0; i<n; ++i )x[i] = -1.*x[i];
  }
  int m = 1;
  int i = 1;
  int j = n;
  double t, ty, tt, tty, r = 0.375;
  int ij, k, l;
L210:
  if( i == j )
  {
    if( --m == 0 ) // clean up
    {
      if( !increasing )
      {
        for( std::size_t i=0; i<n; ++i )x[i] = -1.*x[i];
      }
      return;
    }
    i = static_cast<int>(il[m-1]);
    j = static_cast<int>(iu[m-1]);
    goto L260;
  }
  r > 0.5898437 ? r -= 0.21875 : r += 0.0390625;
L225:
  k = i;
  //
  // select a central element of the array and save it in location t
  //
  ij = i + static_cast<int>( (j-i)*r );
  t = x[ij-1];
  ty = y[ij-1];
  if( x[i-1] > t ) // interchange with t
  {
    x[ij-1] = x[i-1];
    x[i-1]  = t;
    t = x[ij-1];
    y[ij-1] = y[i-1];
    y[i-1] = ty;
    ty = y[ij-1];
  }
  l = j;
  if( x[j-1] < t ) // interchange with t
  {
    x[ij-1] = x[j-1];
    x[j-1] = t;
    t = x[ij-1];
    y[ij-1] = y[j-1];
    y[j-1] = ty;
    ty = y[ij-1];
    if( x[i-1] > t ) // interchange with t
    {
      x[ij-1] = x[i-1];
      x[i-1] = t;
      t = x[ij-1];
      y[ij-1] = y[i-1];
      y[i-1] = ty;
      ty = y[ij-1];
    }
  }
L240:
  do // find an element in the second half of the array which is smaller than t
  {
    --l;
  }
  while( x[l-1] > t );
  do // find an element in the first half of the array which is greater than t
  {
    ++k;
  }
  while( x[k-1] < t );
  if( k <= l ) // interchange these elements
  {
    tt = x[l-1];
    x[l-1] = x[k-1];
    x[k-1] = tt;
    tty = y[l-1];
    y[l-1] = y[k-1];
    y[k-1] = tty;
    goto L240;
  }
  //
  // save upper and lower subscripts of the array yet to be sorted
  //
  if( l-i > j-k )
  {
    il[m-1] = i;
    iu[m-1] = l;
    i = k;
  }
  else
  {
    il[m-1] = k;
    iu[m-1] = j;
    j = l;
  }
  ++m;
  //
  // begin again on another portion of the unsorted array
  //
L260:
  if( j-i >= 1 )goto L225;
  if( i == 1 )goto L210;
  --i;
  for( ;; )
  {
    if( ++i == j )
    {
      if( --m == 0 ) // clean up
      {
        if( !increasing )
        {
          for( std::size_t i=0; i<n; ++i )x[i] = -1.*x[i];
        }
        return;
      }
      i = static_cast<int>(il[m-1]);
      j = static_cast<int>(iu[m-1]);
      goto L260;
    }
    t = x[i];
    ty = y[i];
    if( x[i-1] > t )
    {
      k = i;
      do
      {
        x[k] = x[k-1];
        y[k] = y[k-1];
        --k;
      }
      while( t < x[k-1] );
      x[k] = t;
      y[k] = ty;
    }
  }
}

void MinMax( std::vector<double> const &x, int start, int end, double &min, double &max )
{
  min = x[start];
  max = min;
  for( int i=start; i<end; ++i )
  {
    if( min > x[i] )min = x[i];
    if( max < x[i] )max = x[i];
  }
}

void MinMax( std::vector<double> const &x, int nrows, int i1,
             int i2, int j1, int j2, double &min, double &max )
{
  min = x[j1+i1*nrows];
  max = min;
  for( int i=i1; i<i2; ++i )
  {
    for( int j=j1; j<j2; ++j )
    {
      double val = x[j+i*nrows];
      if( min > val )min = val;
      if( max < val )max = val;
    }
  }
}

bool InsidePolygon( double xp, double yp, std::vector<double> const &x,
                    std::vector<double> const &y )
{
  // test if point xp,yp is inside polygon
  // specified by array of points x,y
  //
  // test if a corner point
  //
  int size = x.size();
  for( int i=0; i<size; ++i )
  {
    if( (xp==x[i]) && (yp==y[i]) )return true;
  }
  //
  // find range of x and y
  //
  double xmin, xmax, ymin, ymax;
  MinMax( x, 0, size, xmin, xmax );
  MinMax( y, 0, size, ymin, ymax );
  double deltay = (ymax-ymin)*0.00001;
  double xinf = xmax + 0.1*(xmax-xmin);
  //
  int nsect = 0;          // number of intersections of test line
  for( int i=1; i<size; ++i )
  {
    double dy = y[i] - y[i-1];
    if( dy != 0.0 )
    {
      double yp2 = yp;
      if( xp<x[i] && yp==y[i] )
      {
        yp2 = yp+deltay;
        if( yp2 == y[i-1] )yp2 = yp+0.5*deltay;
      }
      if( xp<x[i-1] && yp==y[i-1] )
      {
        yp2 = yp+deltay;
        if( yp2 == y[i] )yp2 = yp+0.5*deltay;
      }
      if( (yp2-y[i-1])*(yp2-y[i]) <= 0.0 )
      {
        double xi = (yp2*x[i]-y[i-1]*x[i]-yp2*x[i-1]+y[i]*x[i-1])/dy;
        if( (xi-xp)*(xi-xinf) <= 0.0 )++nsect;
      }
    }
  }
  double dy = y[size-1] - y[0];
  if( dy != 0.0 )
  {
    double yp2 = yp;
    if( xp<x[size-1] && yp==y[size-1] )
    {
      yp2 = yp+deltay;
      if( yp2 == y[0] )yp2 = yp+0.5*deltay;
    }
    if( xp<x[0] && yp==y[0] )
    {
      yp2 = yp+deltay;
      if( yp2 == y[size-1] )yp2 = yp+0.5*deltay;
    }
    if( (yp2-y[0])*(yp2-y[size-1]) <= 0.0 )
    {
      double xi = (yp2*x[size-1]-y[0]*x[size-1]-yp2*x[0]+y[size-1]*x[0])/dy;
      if( (xi-xp)*(xi-xinf) <= 0.0 )++nsect;
    }
  }
  if( nsect/2*2 != nsect )return true;
  //
  // test if point lies on one of the edges
  //
  for( int i=0; i<size-1; ++i )
  {
    if( (xp-x[i])*(x[i+1]-xp)>=0.0 && (yp-y[i])*(y[i+1]-yp)>=0.0 )
    {
      if( x[i+1] == x[i] )return true;
      if( yp == (y[i+1]-y[i])/(x[i+1]-x[i])*(xp-x[i])+y[i] )return true;
    }
  }
  //
  // test last edge
  //
  if( (xp-x[0])*(x[size-1]-xp)>=0.0 && (yp-y[0])*(y[size-1]-yp)>=0.0 )
  {
    if( x[size-1] == x[0] )return true;
    if( yp == (y[size-1]-y[0])/(x[size-1]-x[0])*(xp-x[0])+y[0] )return true;
  }
  return false;
}

double exp10( double r, int power )
{
  // returns r/pow(10.0,power)
  //
  // exp10 means wxT("divide by exponentiation of base 10")
  //
  // exp10 returns a value for those cases when
  // r/pow(10.0,power) is defined but |power| is too large for
  // the exponent evaluation of pow(10.0,power)
  //
  // example: if r = 1.e-70 and power = -100 then
  //          pow(10.0,power) is undefined (underflow), but
  //          exp10(r,power) = 1.e-70/pow(10,-100) = 1.e30 is
  //          well defined
  //
  // note: exp10 does not take special note of those cases when
  // r/pow(10.0,power) is undefined due to an underflow or overflow.
  // These cases are handled by the system
  //
  // d1mach(1)  --  numeric_limits<double>::min()
  // d1mach(2)  --  numeric_limits<double>::max()
  // d1mach(3)  --  numeric_limits<double>::epsilon()
  // d1mach(4)  --  2*numeric_limits<double>::epsilon()
  //
  int powa = abs(power);
  double result;
  if( powa <= 70 )               // |power| <= 70
    result = r/pow(10.0,power);
  else                           // |power| > 70
  {
    // divide r by pow(10,(+/-1)*70) and subtract 70
    // off of |power| until |power| is <= 70
    //
    result = r;
    int ips70, pows;
    power < 0 ? pows = -1 : pows = 1;
    ips70 = pows*70;
    while( powa > 70 )
    {
      result /= pow(10.0,static_cast<double>(ips70));
      powa -= 70;
    }
    result /= pow(10.0,static_cast<double>(pows*powa)); // divide by the remaining power
  }
  return result;
}

double Factorial( int n )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5C
  //
  // COMPUTES THE  FACTORIAL OF N
  //
  if( n < 0 )throw EExpressionError(wxT("Factorial: factorial of negative integer undefined"));
  //
  if( n <= 30 )
  {
    double const facn[31] = {
      1.0, 1.0,
      2.0, 6.0,
      24.0, 120.0,
      720.0, 5040.0,
      40320.0, 362880.0,
      3628800.0, 39916800.0,
      479001600.0, 6227020800.0,
      87178291200.0, 1307674368000.0,
      20922789888000.0, 355687428096000.0,
      6402373705728000.0, 121645100408832000.0,
      2432902008176640000.0, 51090942171709440000.0,
      1124000727777607680000.0, 25852016738884976640000.0,
      620448401733239439360000.0, 15511210043330985984000000.0,
      403291461126605635584000000.0, 10888869450418352160768000000.0,
      304888344611713860501504000000.0, 8841761993739701954543616000000.0,
      265252859812191058636308480000000.0};
    return facn[n];
  }
  //
  double xmin, xmax;
  ComputeGammaBounds( xmin, xmax );
  int nmax = static_cast<int>(xmax-1.0);
  if( n > nmax )throw EExpressionError(wxT("Factorial: argument is too large"));
  //
  double x = static_cast<double>(n+1);
  return exp((x-0.5)*log(x)-x+log(sqrt(2*M_PI))+LogGammaCorrection(x));
}

double Chisq( double x, int n )
{
  // Chisq computes the probability that chi-squared, with n degrees of freedom,
  // will exceed x
  //  Chisq(x,n)=1/[2^(n/2)*gamma(n/2)]*integral from x to infinity of
  //  t^(n/2-1)*exp(-t/2) dt ; where 0 <= x < infinity
  // In Abramowitz & Stegun: Handbook of Mathematical Functions,
  // chisq(x,n) is denoted as q(x|n)
  //
  // Reference: cern library routine g100
  //
  // restrictions:  x >= 0  ;  n >= 1 is an integer
  // external routine referenced: gauss (cern c100)
  //
  //  accuracy: to at least 0.00001 in tested region:
  //  0.001 <= x <= 74, various n from 1 to 30
  //
  if( n < 0 )throw EExpressionError(wxT("CHISQ: second argument < 0"));
  if( x < 0.0 )throw EExpressionError(wxT("CHISQ: first argument < 0"));
  if( n > 100 )
  {
    // if n>100, z has a gaussian distribution
    //                  1
    //                  -
    //               (x)3     (     1  )
    //        z = (  (-)   -  (1- -----)  ) * sqrt (4.5*n)
    //               (n)      (   4.5*n)
    //
    double anu = 1./static_cast<double>(n);
    double an9 = anu/4.5;
    double z = (pow(x*anu,0.3333) - (1.-an9))/sqrt(an9);
    return 1.0-Gauss(z);
  }
  if( x > 1300.0 )return 0.0;
  //
  double const emyo2 = exp(-0.5*x);
  double sum = 1.0;
  double term = 1.0;
  int const m = n/2;
  if( 2*m == n ) // n is even
  {
    for( int i=2; i<=m; ++i )
    {
      double fi = i-1.0;
      term *= 0.5*x/fi;
      sum += term;
    }
    return emyo2*sum;
  }
  //
  // entry if n is odd
  //
  double const d = 2.*(1.-Gauss(sqrt(x)));
  if( n == 1 )return d;
  double const c = sqrt(x*2/M_PI)*emyo2;
  if( n != 3 )
  {
    for( int i=1; i<=m-1; ++i )
    {
      term *= x/(2*i+1);
      sum += term;
    }
  }
  return c*sum+d;
  /*
  if( x < 0.0 )return 0.0;
  if( n > 100 ) // use asymptotic formula
  {
    double a = 1.0/n/4.5;
    double z = (pow((x/n),1.0/3.0)-(1.0-a))/sqrt(a);
    return (1.0 - Gauss(z));
  }
  if( x > 1300.0 )return 0.0;
  double const emyo2 = exp(-0.5*x);
  double sum = 1.0;
  double term = 1.0;
  unsigned int const m = n/2;
  if( 2*m == n ) // n is even
  {
    for( unsigned int i = 1; i < m; ++i )
    {
      term *= 0.5*x/i;
      sum += term;
    }
    return (emyo2*sum);
  }
  //
  // n is odd
  //
  double const d = 2*( 1 - Gauss( sqrt(x) ) );
  if( n == 1 )return d;
  double const c = sqrt(x*2/M_PI)*emyo2;
  for( unsigned int i = 1; i < m; ++i )
  {
    term *= x/(2*i+1);
    sum += term;
  }
  return (c*sum + d);
  */
}

double ChisqInverse( double p, int n )
{
  // This routine computes the inverse of the distribution function of the
  // chi-squared distribution with n degrees of freedom input parameters:
  // p = probability at which the function is inverted; 0<=p<1, n >= 1
  //
  // REFERENCE: CACM ALGORITHM #451, CERN library routine G101
  // Accuracy: to at least 5 significant figures
  //
  if( p<0.0 || p>=1.0 )
    throw EExpressionError(wxT("ChisqInverse: first argument must be >=0 and <1") );
  if( n < 1 )
    throw EExpressionError(wxT("ChisqInverse: number of degrees of freedom must be >=1") );
  //
  double result = 0.0;
  if( p == 0.0 )return result;
  p = 1.0 - p;
  if( n < 2 )
  {
    result = GaussInverse(0.5*p);
    result = result*result;
  }
  else if( n == 2 )
  {
    result = -2.0*log(p);
  }
  else
  {
    double f = n;
    double f1 = 1./f;
    double t = GaussInverse(p);
    double f2 = sqrt(f1)*t;
    if( n < (2+static_cast<int>(4.0*fabs(t))) )
    {
      double const c[] = { 1.565326E-3,  1.060438E-3, -6.950356E-3, -1.323293E-2,
                           2.277679E-2, -8.986007E-3, -1.513904E-2,  2.530010E-3,
                          -1.450117E-3,  5.169654E-3, -1.153761E-2,  1.128186E-2,
                           2.607083E-2, -0.2237368,    9.780499E-5, -8.426812E-4,
                           3.125580E-3, -8.553069E-3,  1.348028E-4,  0.4713941, 1.0000886 };
      result = (((((((c[0]*f2+c[1])*f2+c[2])*f2+c[3])*f2+c[4])*f2+c[5])*f2+c[6])*f1+
               ((((((c[7]+c[8]*f2)*f2+c[9])*f2+c[10])*f2+c[11])*f2+c[12])*f2+c[13]))*f1+
               (((((c[14]*f2+c[15])*f2+c[16])*f2+c[17])*f2+c[18])*f2+c[19])*f2+c[20];
    }
    else
    {
      double const a[] = { 1.264616E-2, -1.425296E-2, 1.1400483E-2, -5.886090E-3,
                          -1.091214E-2, -2.304527E-2,  3.135411E-3, -2.728484E-4,
                          -9.699681E-3,  1.316872E-2,  2.618914E-2, -0.22222222,
                           5.406674E-5,  3.483789E-5, -7.274761E-4,  3.292181E-3,
                          -8.729713E-3,  0.4714045,    1.0 };
      result = (((a[0]+a[1]*f2)*f1+(((a[2]+a[3]*f2)*f2+a[4])*f2+a[5]))*f1+
               (((((a[6]+a[7]*f2)*f2+a[8])*f2+a[9])*f2+a[10])*f2+a[11]))*f1+
               (((((a[12]*f2+a[13])*f2+a[14])*f2+a[15])*f2+a[16])*f2*f2+a[17])*f2+a[18];
    }
    result = result*result*result*f;
  }
  return result;
}

double Normal( double x, double a, double b )
{
  // This routine computes the normalized gaussian distribution function
  // NORMAL(X,A,B) = 1/[B*SQRT(2*PI)]*EXP{-(1/2)*[(X-A)/B]^2}
  //
  if( b == 0.0 )throw EExpressionError(wxT("Normal: third argument must not =0") );
  return exp(-0.5*((x-a)/b)*((x-a)/b))/(b*sqrt(2*M_PI));
}

double Gauss( double x )
{
  // from cern library routine c300
  // gauss(x)=(1/2) + (1/2)*erf(x/sqrt(2))    for x>=0
  //        =(1/2) - (1/2)*erf(x/sqrt(2))    for x< 0
  // in general: gauss(x)=(i/sqrt(2*pi))*integral(exp(-t*t))dt,
  //             limits -infinity to x
  // freq
  //
  // gauss is the normal (gaussian) probability frequency function,
  // usually denoted as p(x).
  // accuracy: to at least 1.0e-9
  //
  double const c = sqrt(0.5);
  double const xmax = 8.9;
  //
  // xmax=sqrt(-log(rmin)-10.0)/sqrt(0.5), where rmin is the smallest normalized
  // representable number.  erfc(xmax) is close to the underflow threshold.
  //
  double result = 0.0;
  if( x >= -xmax/c )
  {
    x>=0.0 ? result=0.5+0.5*Error(c*x) : result=0.5-0.5*Error(-c*x);
  }
  return result;
}

double GaussInverse( double p )
{
  // This routine computes the inverse of the distribution function of the
  // standard normal distribution. The routine was written by S.W. Cunningham
  // gaussin
  //
  // REFRENCE: CERN library routine C301
  //   also: S.W.CUNNINGHAM Algorithm AS24-from APPLIED STATISTICS,
  //         NORMAL INTEGRAL TO DEVIATE.
  //
  // RESTRICTION ON P(the argument): 0<= P <=1
  // N.B. If P=0, GAUSSIN is set arbitrarily to -1E10 (-infinity)
  //      If P=1,   "      "        "           +1E10 (+infinity)
  //
  // ACCURACY : TO AT LEAST 1.0D-8
  //
  if( p<0.0 || p>1.0 )throw EExpressionError(wxT("GaussInverse: argument must be >=0 and <=1") );
  if( p == 0.0 )return -1.0e10;
  if( p == 1.0 )return 1.0e10;
  //
  double const rthfpi = 1.2533141373;
  double const rrt2pi = 0.3989422804;
  double const termin = 1.e-11;
  //
  // get first approximation by hastings formula
  //
  if( p > 0.5 )p = 1.0-p;
  double f = -log(p);
  double e = sqrt(f+f);
  double xo = -e+((0.010328*e+0.802853)*e+2.515517)/(((0.001308*e+0.189269)*e+1.432788)*e+1.);
  //
  double po, x1;
  if( xo >= 0.0 )
  {
    xo = 0.0;
    po = 0.5;
    x1 = -rthfpi;
  }
  else
  {
    //
    // find the area corresponding to x0
    //
    double y = xo*xo;
    if( xo <= -1.9 ) // continued fraction approximation
    {
      double z = 1./y;
      double a1 = 1.0;
      double a2 = 1.0;
      double a3 = z+1.0;
      double a4 = 1.0;
      do
      {
        a1 = a4+a1*2*z;
        a2 = a3+a2*2*z;
        a3 = a2+a3*3*z;
        a4 = a1+a4*3*z;
      }
      while( a1/a2-a4/a3 >= termin );
      x1 = a4/a3/xo;
      po = -x1*rrt2pi*exp(-0.5*y);
    }
    else
    {
      y *= -0.5;
      //
      // series approximation
      //
      double const connor[17] =
       { 8.0327350124e-17, 1.4483264644e-15, 2.4668270103e-14,
        23.9554295164e-13, 5.9477940136e-12, 8.3507027951e-11,
        31.0892221037e-9,  1.3122532964e-8,  1.4503852223e-7,
        41.4589169001e-6,  1.3227513228e-5,  1.0683760684e-4,
        57.5757575758e-4,  4.6296296296e-3,  2.3809523810e-2,
        60.1,              0.333333333333 };
      po = connor[0];
      for( int i=1; i<17; ++i )po = po*y+connor[i];
      po = (po*y+1.0)*xo;
      x1 = -(po+rthfpi)*exp(-y);
      po = po*rrt2pi+0.5;
    }
  }
  //
  // get accurate value by taylor expansion
  //
  double d = f+log(po);
  double x2 = xo*x1*x1-x1;
  double x3 = (x1*x1+2.0*xo*x2)*x1-x2;
  double x = ((x3/3.*d+x2)*d*0.5+x1)*d+xo;
  if( p > 0.5 )x *= -1.0;
  return x;
}

double GaussInverse2( double z )
{
  double const p = -sqrt(2.0);
  z *= 2.0;
  double sig, zbar, x, u;
  if( z <= 0.1573 )
  {
    sig = 1.0;
    zbar = z;
  }
  else if( z <= 1.8427 )
  {
    sig = Sign(1.0,1.0-z);
    zbar = fabs(1.0-z);
    double const a0 = 0.88494094;
    double const a1 = 0.27579095;
    double const a2 = -0.10896122;
    double const a3 = 0.44609630;
    x = (a0+zbar*zbar*(a1+zbar*zbar*(a2+a3*zbar*zbar)))*zbar;
    u = (Error(x)-zbar)*0.886226925452758*exp(x*x);
    x = x-u+x*u*u;
    u = (Error(x)-zbar)*0.886226925452758*exp(x*x);
    x = x-u+x*u*u;
    return sig*x*p;
  }
  else
  {
    sig = -1.0;
    zbar = 2.0 - z;
  }
  if( zbar <= 0.0 )return sig*1.E+38;
  double w = sqrt(-log(zbar));
  double b0 =  1.00327457;
  double b1 = -0.09845895;
  double b2 = -0.87290308;
  double b3 =  0.97861210;
  double b4 = -0.38589878;
  x = (b0+(b1+(b2+(b3+b4/w)/w)/w)/w)/w;
  u = -(ComplementaryError(x)-zbar)*0.886226925452758*exp(x*x);
  x = x-u+x*u*u;
  u = -(ComplementaryError(x)-zbar)*0.886226925452758*exp(x*x);
  x = x-u+x*u*u;
  return sig*x*p;
}

double Error( double x )
{
  // reference: cern library routine c300
  // erf(x)=(2/sqrt(pi))*integral(exp(-t*t)dt), limits 0 to x
  // erf
  //
  // the error function is computed from the rational approximations
  // of W.J. Cody, Mathematics of Computation, volume 22 (1969),
  // pages 631-637.
  //
  // for abs(x) <= 0.47 the basic function is erf
  // for abs(x) >  0.47 the basic function is erfc
  // The final result is obtained in terms of the basic function as
  // shown in the following table, in which a = abs(x).
  //
  //  function  |   a <= (0.47)   |                a > (0.47)
  //            |                 |
  //  required  |     (all x)     |    (negative x)       (positive x)
  //------------|-----------------|-----------------------------------
  //  erf(x)    |     erf(x)      |      erfc(a)-1          1-erfc(x)
  //            |                 |
  //  erfc(x)   |    1-erf(x)     |       2-erfc(a)           erfc(x)
  //            |                 |
  //
  // accuracy: to 1.0e-7
  //
  double const p1[3] = {21.3853322, 1.72227577, 0.316652891};
  double const q1[3] = {18.9522572, 7.84374571, 1.00000000};
  double const p2[5] = {7.37388831, 6.86501848, 3.03179934,
                        0.563169619, 0.0000431877874};
  double const q2[5] = {7.37396089, 15.1849082, 12.7955295,
                        5.35421679, 1.00000000};
  double const p3[3] = {-0.0425799644, -0.196068974, -0.0516882262};
  double const q3[3] = {0.150942071, 0.921452412, 1.00000000};
  //
  double const a = fabs(x);
  double const c = sqrt(1/M_PI);
  double const xunit = 4.4;
  //
  // ( xunit = sqrt(-log(relpr)+1), where relpr is the smallest number
  //   for which 1+relpr differs from 1.  erf(xunit) is indistinguishable
  //   from 1 ), i.e. erf(>4.4) = 1
  //
  if( a > xunit )return Sign( 1.0, x );
  double s = x*x;
  double y;
  if( a <= 0.47 )
  {
    return (x*(p1[0]+s*(p1[1]+s*p1[2]))/(q1[0]+s*(q1[1]+s*q1[2])));
  }
  else if( a > 4.0 )
  {
    double r = 1.0/a;
    double u = r*r;
    y = r*exp(-s)*( c + u*(p3[0]+u*p3[1])/(q3[0]+u*q3[1]) );
  }
  else
  {
    y = exp(-s)*
        (p2[0]+a*(p2[1]+a*(p2[2]+a*(p2[3]+a*p2[4]))))/
        (q2[0]+a*(q2[1]+a*(q2[2]+a*(q2[3]+a*q2[4]))));
  }
  return Sign( 1-y, x );
}

double ErrorInverse( double z )
{
  double x, u;
  double zbar = fabs(z);
  double sig = Sign(1.0,z);
  if( zbar < 0.8427 )
  {
    double const a0 = 0.88494094;
    double const a1 = 0.27579095;
    double const a2 = -0.10896122;
    double const a3 = 0.44609630;
    x = (a0+zbar*zbar*(a1+zbar*zbar*(a2+a3*zbar*zbar)))*zbar;
    u = (Error(x)-zbar)*0.886226925452758*exp(x*x);
    x = x-u+x*u*u;
    u = (Error(x)-zbar)*0.886226925452758*exp(x*x);
    x = x-u+x*u*u;
  }
  else
  {
    zbar = 1.0 - zbar;
    if( zbar <= 0.0 )return sig*1.E+38;
    double w = sqrt(-log(zbar));
    double w1 = 1.0/w;
    double b0 = 1.00327457;
    double b1 = -0.09845895;
    double b2 = -0.87290308;
    double b3 = 0.97861210;
    double b4 = -0.38589878;
    x = w*(b0+w1*(b1+w1*(b2+w1*(b3+w1*b4))));
    u = -(ComplementaryError(x)-zbar)*0.886226925452758*exp(x*x);
    x = x-u+x*u*u;
    u = -(ComplementaryError(x)-zbar)*0.886226925452758*exp(x*x);
    x = x-u+x*u*u;
  }
  return sig*x;
}

double ComplementaryError( double x )
{
  // ERFC(X)=(2/sqrt(pi))*INTEGRAL(exp(-t**2))dt, limits X to infinity
  // ERFC(X) is sometimes denoted as Q(X)
  // erfc
  //
  // REFERENCE: CERN library routine C300, see also ERF(X)
  // THE ERFC  FUNCTION IS COMPUTED FROM THE RATIONAL APPROXIMATIONS OF W.J.CODY,
  // MATHEMATICS OF COMPUTATION, VOLUME 22 (1969), PAGES 631-637.
  //
  // THE ERFC FUNCTION IS RELATED TO THE ERF FUNCTION BY: ERFC(X) = 1 - ERF(X)
  // ACCURACY : TO 1.0E-7
  //
  // erfc(xmax) is close to the underflow threshold
  //
  double const xmax = sqrt(-log(std::numeric_limits<double>::min())-10.0);
  //
  // erf(xunit) is indistinguishable from 1.0
  //
  double const xunit = sqrt(-log(std::numeric_limits<double>::epsilon())+1.0);
  //
  double const p1[] = {2.13853322E1, 1.72227577E0, 3.16652891E-1};
  double const q1[] = {1.89522572E1, 7.84374571E0, 1.00000000};
  double const p2[] = {7.37388831E0, 6.86501848E0, 3.03179934, 5.63169619E-1, 4.31877874E-5};
  double const q2[] = {7.37396089E0, 1.51849082E1, 1.27955295E1, 5.35421679, 1.00000000};
  double const p3[] = {-4.25799644E-2, -1.96068974E-1, -5.16882262E-2};
  double const q3[] = {1.50942071E-1, 9.21452412E-1, 1.00000000};
  //
  double a = fabs(x);
  if( x < -xunit )return 2.0;
  if( x > xmax )return 0.0;
  double s = x*x;
  if( a <= 0.47 )return 1.0-x*(p1[0]+s*(p1[1]+s*p1[2]))/(q1[0]+s*(q1[1]+s*q1[2]));
  //
  // set y=erfc(a), then terminate.
  //
  double y;
  if( a <= 4.0 )
  {
    y = exp(-s)*(p2[0]+a*(p2[1]+a*(p2[2]+a*(p2[3]+a*p2[4]))))/
                       (q2[0]+a*(q2[1]+a*(q2[2]+a*(q2[3]+a*q2[4]))));
    if( x >= 0.0 )return y;
    else          return 2.0-y;
  }
  double r = 1.0/a;
  double u = r*r;
  y = r*exp(-s)*(sqrt(1./M_PI)+u*(p3[0]+u*p3[1])/(q3[0]+u*q3[1]));
  if( x >= 0.0 )return y;
  else          return 2.0-y;
}

double ComplementaryErrorInverse( double z )
{
  double sig, zbar, x, u;
  if( z <= 0.1573 )
  {
    sig = 1.0;
    zbar = z;
  }
  else if( z <= 1.8427 )
  {
    sig = Sign(1.0,1.0-z);
    zbar = fabs(1.0-z);
    double const a0 = 0.88494094;
    double const a1 = 0.27579095;
    double const a2 = -0.10896122;
    double const a3 = 0.44609630;
    x = (a0+zbar*zbar*(a1+zbar*zbar*(a2+a3*zbar*zbar)))*zbar;
    u = (Error(x)-zbar)*0.886226925452758*exp(x*x);
    x = x-u+x*u*u;
    u = (Error(x)-zbar)*0.886226925452758*exp(x*x);
    x = x-u+x*u*u;
    return sig*x;
  }
  else
  {
    sig = -1.0;
    zbar = 2.0 - z;
  }
  if( zbar <= 0.0 )return sig*1.E+38;
  double w = sqrt(-log(zbar));
  double b0 =  1.00327457;
  double b1 = -0.09845895;
  double b2 = -0.87290308;
  double b3 =  0.97861210;
  double b4 = -0.38589878;
  x = (b0+(b1+(b2+(b3+b4/w)/w)/w)/w)/w;
  u = -(ComplementaryError(x)-zbar)*0.886226925452758*exp(x*x);
  x = x-u+x*u*u;
  u = -(ComplementaryError(x)-zbar)*0.886226925452758*exp(x*x);
  x = x-u+x*u*u;
  return sig*x;
}

int Sign( int a, int b )
{
  if( b >= 0 )return  abs(a);
  else        return -abs(a);
}

double Sign( double a, double b )
{
  if( b >= 0.0 )return  fabs(a);
  else          return -fabs(a);
}

void Splsmooth( std::vector<double> &x,
                                 std::vector<double> &y,
                                 std::vector<double> &weights,
                                 std::vector<double> &xout,
                                 std::vector<double> &yout )
{
  // This routine smoothes a curve through an ordered set of points.
  // It first parameterizes the points in terms of normalized arclength.
  // Normalized length in X is real length divided by the range of X, ie.
  // the maximum X value of input points minus the minimum. Normalized
  // length in Y is real length divided by the range of Y, ie. the maximum
  // Y value of input points minus the minimum.  The arclength at an input
  // point is approximated by the sum of the lengths of straight lines
  // connecting all points up to that input point.  Next, a spline with
  // tension curve is computed for X versus arclength and Y versus arclength.
  // For equally spaced arclength, X and Y values are interpolated separatly
  // and then combined to form output points.
  //
  // Note:  The smoothing of the spline with tension for X versus arclength
  //        and Y versus arclength is done by subroutine DSPLFT
  //
  // input
  //        x: array of the x values of the ordered input points, length n
  //        y: array of the y values of the ordered input points, length n
  //  weights: array controls amount of smoothing at each point, length n
  //
  // output
  //     xout: array of the x values of the interpolated points, length m
  //     yout: array of the y values of the interpolated points, length m
  //
  // Calculate arclength parameterization for input points
  //
  std::size_t n = x.size();
  std::vector<double> w1( n );
  w1[0] = 0.0;
  for( std::size_t i=1; i<n; ++i )
  {
    w1[i] = w1[i-1] + sqrt((x[i]-x[i-1])*(x[i]-x[i-1])+(y[i]-y[i-1])*(y[i]-y[i-1]));
  }
  //
  // calculate arclengths of output points
  //
  std::size_t m = xout.size();
  std::vector<double> w3( m );
  double tmp = w1[n-1]/(m-1.0);
  for( std::size_t i=0; i<m-1; ++i )w3[i] = i*tmp;
  w3[m-1] = w1[n-1];
  //
  // perform spline with tension interpolation for
  // functional values x versus arclength and y versus arclength
  //
  std::vector<double> yderiv( m, 0.0 );
  try
  {
    SplineSmooth( w1, x, w3, xout, yderiv, weights );
    SplineSmooth( w1, y, w3, yout, yderiv, weights );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
}

void SplineSmooth( std::vector<double> &x,
                                    std::vector<double> &y,
                                    std::vector<double> &xout,
                                    std::vector<double> &yout,
                                    std::vector<double> &yderiv,
                                    std::vector<double> &weight )
{
  //  This routine fits a cubic spline function to a set of data points
  //  (x[i],y[i]), for i = 1, n. The routine will try to achieve
  //  sum(i=1,n) of (g(x[i])-y[i])^2/(weight[i]^2) < tension where weight[i]>0,
  //  i=1,...,n and tension >= 0  are given numbers and g is the cubic spline.
  //  This routine was taken from U.B.C *NUMLIB
  //  For more complete write-up see UBC CURVE p.53-58 (Mar/76)
  //
  // input
  //      x: monotonically increasing array containing the abscissae of the data
  //      y: array containing the ordinates of the data
  //   xout: array containing the abscissae at which the fiited curve is to be evaluated
  //         Note: x[0] <= xout[i] <= x[N-1] for i=0,...,M
  // weight: array which controls the amount of smoothing at each abscissa
  //         If possible use the standard deviation of y[i] for weight[i]
  // output
  //   yout: array containing the returned ordinates of the function at xout[i]
  //
  std::size_t n = x.size();
  std::size_t m = xout.size();
  //
  double sumy = 0.0;
  double sumyy = 0.0;
  for( std::size_t i=0; i<n; ++i )
  {
    sumy += y[i];
    sumyy += y[i]*y[i];
  }
  double ss = ExGlobals::GetTension()*(sumyy-sumy*sumy/n);
  double ds = ss;
  double ee = ss*0.0000005;
  if( ss <= 0.0 )
  {
    ss = 0.00000001;
    ee = 0.0000005;
  }
  //
  double h = x[1]-x[0];
  double f = (y[1]-y[0])/h;
  double e, g;
  std::vector<double> w( 11*n+14, 0.0 );
  for( std::size_t i=1; i<n-1; ++i )
  {
    g = h;
    h = x[i+1]-x[i];
    e = f;
    f = (y[i+1]-y[i])/h;
    w[i] = f-e;
    w[7*n+7+i] = .66666666666667*(g+h);
    w[8*n+9+i] = .33333333333333*h;
    w[6*n+5+i] = weight[i-1]/g;
    w[4*n+1+i] = weight[i+1]/h;
    w[5*n+3+i] = -(weight[i]/g + weight[i]/h);
  }
  for( std::size_t i=1; i<n-1; ++i )
  {
    w[n+i] = w[4*n+i+1]*w[4*n+i+1] + w[5*n+3+i]*w[5*n+3+i] +
                  w[6*n+5+i]*w[6*n+5+i];
    w[2*n+i] = w[4*n+i+1]*w[5*n+4+i] + w[5*n+3+i]*w[6*n+6+i];
    w[3*n+i] = w[4*n+i+1]*w[6*n+7+i];
  }
  //
  // LDU decompositon
  //
  bool done = false;
  double p = 0.0;
  for( std::size_t count=0; count<100; ++count )
  {
    for( std::size_t i=1; i<n-1; ++i )
    {
      w[5*n+2+i] = f*w[4*n+i];
      w[6*n+3+i] = g*w[4*n+i-1];
      w[4*n+1+i] = 1.0/(w[n+i]+p*w[7*n+7+i]-f*w[5*n+2+i]-g*w[6*n+3+i]);
      w[9*n+11+i] = w[i] - w[5*n+2+i]*w[9*n+10+i] - w[6*n+3+i]*w[9*n+9+i];
      f = w[2*n+i] + p*w[8*n+9+i] - h*w[5*n+2+i];
      g = h;
      h = w[3*n+i];
    }
    //
    // back substitution
    //
    for( std::size_t i=1; i<n-1; ++i )
    {
      w[10*n+10-i] = w[5*n-i]*w[10*n+10-i] - w[6*n+2-i]*w[10*n+11-i] -
                        w[7*n+4-i]*w[10*n+12-i];
    }
    e = 0.0;
    h = 0.0;
    for( std::size_t i=0; i<n-1; ++i )
    {
      g = h;
      h = (w[9*n+12+i]-w[9*n+11+i])/(x[i+1]-x[i]);
      w[10*n+13+i] = (h-g)*weight[i]*weight[i];
      e += w[10*n+13+i]*(h-g);
    }
    g = -h*weight[n-1]*weight[n-1];
    w[11*n+13] = g;
    e -= g*h;
    if( e<=ds || fabs(e-ds)<=ee )
    {
      done = true;
      break;
    }
    else
    {
      f = 0.0;
      g = 0.0;
      for( std::size_t i=1; i<n-1; ++i )
      {
        h = w[9*n+10+i]*w[8*n+8+i] + w[9*n+11+i]*w[7*n+7+i] +
            w[9*n+12+i]*w[8*n+9+i];
        f += w[9*n+11+i]*h;
        h -= w[5*n+2+i]*w[4*n+i] + w[6*n+3+i]*w[4*n+i-1];
        g += h*h*w[4*n+i+1];
        w[4*n+i+1] = h;
      }
      h = f - p*g;
      if( h <= 0.0 )
      {
        done = true;
        break;
      }
      p += sqrt(e/ss)*(e-sqrt(ds*e))/h;
    }
  }
  if( !done )throw EExpressionError(wxT("SplineSmooth: function does not converge"));
  //
  for( std::size_t i=0; i<n; ++i )
  {
    w[i] = y[i]-w[10*n+13+i];
    w[2*n+i] = p*w[9*n+11+i];
  }
  for( std::size_t i=0; i<n-1; ++i )
  {
    h = x[i+1]-x[i];
    w[3*n+i] = (w[2*n+i+1]-w[2*n+i])/(3.0*h);
    w[n+i] = (w[i+1]-w[i])/h-(h*w[3*n+i]+w[2*n+i])*h;
  }
  std::size_t j = 0;
  std::size_t a = j;
  std::size_t b = a+n;
  std::size_t c = b+n;
  std::size_t d = c+n;
  for( std::size_t i=0; i<m; ++i )
  {
    while( xout[i] < x[j] )
    {
      if( j == 0 )throw EExpressionError(wxT("SplineSmooth: j == 0"));
      j = 0;
      a = j;
      b = a+n;
      c = b+n;
      d = c+n;
    }
    if( xout[i] < x[j+1] )
    {
      double diff = xout[i] - x[j];
      yout[i] = w[a] + diff*(w[b]+diff*(w[c]+diff*w[d]));
      yderiv[i] = w[b] + diff*(2.0*w[c]+3.0*diff*w[d]);
    }
    else
    {
      if( j < n-2 )
      {
        a = ++j;
        b = a+n;
        c = b+n;
        d = c+n;
        --i;
      }
      else
      {
        if( xout[i] == x[j+1] )
        {
          double diff = xout[i] - x[j];
          yout[i] = w[a] + diff*(w[b]+diff*(w[c]+diff*w[d]));
          yderiv[i] = w[b] + diff*(2.0*w[c]+3.0*diff*w[d]);
        }
        else
          throw EExpressionError(wxT("SplineSmooth: xout[i] != x[j+1]"));
      }
    }
  }
}

void SplineIntegral( std::vector<double> &x,
                                      std::vector<double> &y,
                                      std::vector<double> &xout,
                                      std::vector<double> &yout )
{
  // This routine calculates the integral at specified abcissae.
  // The integral from x[0] to xout[j] is returned in yout[j]
  //
  // input
  //       x:  array of increasing abcissae of the functional values
  //       y:  array of ordinates of the values, i.e.,
  //           y[k] is the functional value corresponding to x[k]
  //    xout:  array of the abcissae of the output points
  //           The values do not have to be in increasing order
  //  output
  //    yout:  yout[j] = integral from x[0] to xout[j]
  //
  std::size_t n = x.size();
  std::size_t m = xout.size();
  //
  double const expmax = 80.0;
  double sigmap = std::max(0.0001,fabs(ExGlobals::GetTension()))*(n-1)/(x[n-1]-x[0]); // denormalized tension
  std::vector<double> yp( n, 0.0 );  // work array
  std::vector<double> w( n, 0.0 );   // work array
  //
  SplineSetup( x, y, yp, w, sigmap, expmax );
  //
  if( sigmap*(x[1]-x[0]) > expmax )
    w[0] = yp[0]/tanh(sigmap*(x[1]-x[0]))/sigmap +
        (y[1]-yp[1])*x[0]*x[0]/2.0/(x[1]-x[0])-(y[0]-yp[0])*(x[1]-x[0]/2.0)*x[0]/(x[1]-x[0]);
  else
    w[0] = -(yp[1]-yp[0]*cosh(sigmap*(x[1]-x[0])))/sigmap/sinh(sigmap*(x[1]-x[0])) +
        (y[1]-yp[1])*x[0]*x[0]/2.0/(x[1]-x[0])-(y[0]-yp[0])*(x[1]-x[0]/2.0)*x[0]/(x[1]-x[0]);
  for( std::size_t i=1; i<n-1; ++i )
  {
    if( sigmap*(x[i]-x[i-1])>expmax || sigmap*(x[i+1]-x[i])>expmax )
      w[i] = w[i-1] +
          (-yp[i-1]*x[i]*x[i]/(x[i]-x[i-1])/2.0-yp[i+1]*x[i]*x[i]/(x[i+1]-x[i])/2.0+
           yp[i]*(1.0/tanh(sigmap*(x[i]-x[i-1]))+1.0/tanh(sigmap*(x[i+1]-x[i])))/sigmap -
           yp[i]*((0.5*x[i]-x[i-1])*x[i]/(x[i]-x[i-1])-(x[i+1]-0.5*x[i])*x[i]/(x[i+1]-x[i])) +
           (y[i+1]*x[i]*x[i]/2.0-y[i]*(x[i+1]-x[i]/2.0)*x[i])/(x[i+1]-x[i]) +
           (y[i]*(x[i]/2.0-x[i-1])*x[i]+y[i-1]*x[i]*x[i]/2.0)/(x[i]-x[i-1]));
    else
      w[i] = w[i-1] +
          (-yp[i-1]*(1.0/sigmap/sinh(sigmap*(x[i]-x[i-1]))+x[i]*x[i]/(x[i]-x[i-1])/2.0) -
           yp[i+1]*(1.0/sigmap/sinh(sigmap*(x[i+1]-x[i]))+x[i]*x[i]/(x[i+1]-x[i])/2.0) +
           yp[i]*(1.0/tanh(sigmap*(x[i]-x[i-1]))+1.0/tanh(sigmap*(x[i+1]-x[i])))/sigmap -
           yp[i]*((0.5*x[i]-x[i-1])*x[i]/(x[i]-x[i-1])-(x[i+1]-0.5*x[i])*x[i]/(x[i+1]-x[i])) +
           (y[i+1]*x[i]*x[i]/2.0-y[i]*(x[i+1]-x[i]/2.0)*x[i])/(x[i+1]-x[i]) +
           (y[i]*(x[i]/2.0-x[i-1])*x[i]+y[i-1]*x[i]*x[i]/2.0)/(x[i]-x[i-1]));
  }
  for( std::size_t j=0; j<m; ++j )
  {
    double xt = xout[j];
    if( xt < x[0] )xt = x[0];
    if( xt > x[n-1] )xt = x[n-1];
    std::size_t i;
    for( std::size_t ii=1; ii<n; ++ii )
    {
      i = ii;
      if( x[ii] >= xt )break;
    }
    double del1 = xt - x[i-1];
    double del2 = x[i] - xt;
    double dels = x[i] - x[i-1];
    //
    if( sigmap*dels > expmax )
      yout[j] = (y[i]-yp[i])*(xt/2.0-x[i-1])*xt/dels +
          (y[i-1]-yp[i-1])*(x[i]-xt/2.0)*xt/dels+w[i-1];
    else
      yout[j] = (yp[i]*cosh(sigmap*del1)-yp[i-1]*cosh(sigmap*del2))/
          sigmap/sinh(sigmap*dels)+(y[i]-yp[i])*(xt/2.0-x[i-1])*
          xt/dels+(y[i-1]-yp[i-1])*(x[i]-xt/2.0)*xt/dels+w[i-1];
  }
}

void Splinterp( std::vector<double> const &x, std::vector<double> const &y,
                                 std::vector<double> &xout, std::vector<double> &yout )
{
  // This routine interpolates a curve through an ordered set of points.
  // It first parameterizes the points in terms of normalized arclength.
  // Normalized length in x is real length divided by the range of x, ie.
  // the maximum x value of input points minus the minimum.  Normalized
  // length in y is real length divided by the range of y, ie, the maximum
  // y value of input points minus the minimum.  The arclength at an input
  // point is approximated by the sum of the lengths of straight lines
  // connecting all points up to that input point.  A spline with tension
  // curve is computed for x versus arclength and y versus arclength.  For
  // equally spaced arclength, x and y values are interpolated separatly and
  // then combined to form output points.
  //
  // input
  //      x: array of the x values of the ordered input points (length N)
  //      y: array of the y values of the ordered input points (length N)
  //   npts: is the number of output points
  //
  // output
  //   xout: array of the x values of the interpolated points (length npts)
  //   yout: array of the y values of the interpolated points (length npts)
  //
  std::size_t n = x.size();
  std::size_t start=0;
  double xmin, xmax, ymin, ymax;
  MinMax( x, start, n, xmin, xmax );
  MinMax( y, start, n, ymin, ymax );
  //
  std::vector<double> w1( n );
  //
  // calculate arclength parameterization for input points
  //
  w1[0] = 0.0;
  for( std::size_t i=1; i<n; ++i )
  {
    double xtmp = (x[i]-x[i-1])/(xmax-xmin);
    double ytmp = (y[i]-y[i-1])/(ymax-ymin);
    w1[i] = w1[i-1] + sqrt(xtmp*xtmp+ytmp*ytmp);
  }
  std::size_t npts = xout.size();
  std::vector<double> w2( npts );
  double tmp = w1[n-1]/(npts-1.0);
  for( std::size_t i=0; i<npts; ++i ) // calculate arclengths of output points
  {
    w2[i] = i*tmp;
  }
  //
  // perform spline with tension interpolation for
  // x vs arclength and y vs arclength
  //
  SplineInterpolate( w1, x, w2, xout );
  SplineInterpolate( w1, y, w2, yout );
}

void SplineInterpolate( std::vector<double> const &x,
                                         std::vector<double> const &y,
                                         std::vector<double> &xout,
                                         std::vector<double> &yout )
{
  // This routine interpolates points through function values using a spline under tension.
  // For tensions close to zero, eg. 0.001, the interpolation is close to a cubic spline.
  // For large tensions, eg. 100., the interpolation is basically polygonal.
  //
  // input
  //       x:  array of increasing abcissae of the functional values
  //       y:  array of ordinates of the values, i.e.,
  //           y[k] is the functional value corresponding to x[k]
  //    xout:  array of the abcissae of the output points
  //           The values do not have to be in increasing order
  //  output
  //    yout:  yout[j] = interpolated ordinate of xout[j]
  //
  std::size_t n = x.size();
  std::size_t m = xout.size();
  //
  double const expmax = 80.0;
  double sigmap = std::max(0.0001,fabs(ExGlobals::GetTension()))*(n-1)/(x[n-1]-x[0]); // denormalized tension
  std::vector<double> yp( n, 0.0 );  // work array
  std::vector<double> w( n, 0.0 );   // work array
  //
  SplineSetup( x, y, yp, w, sigmap, expmax );
  //
  for( std::size_t j=0; j<m; ++j )
  {
    double xt = xout[j];
    if( xt < x[0] )xt = x[0];
    if( xt > x[n-1] )xt = x[n-1];
    std::size_t i;
    for( std::size_t ii=1; ii<n; ++ii )
    {
      i = ii;
      if( x[ii] >= xt )break;
    }
    double del1 = xt - x[i-1];
    double del2 = x[i] - xt;
    double dels = x[i] - x[i-1];
    //
    if( sigmap*dels > expmax )
      yout[j] = ((y[i]-yp[i])*del1+(y[i-1]-yp[i-1])*del2)/dels;
    else
      yout[j] = (yp[i]*sinh(sigmap*del1)+yp[i-1]*sinh(sigmap*del2))/
          sinh(sigmap*dels)+((y[i]-yp[i])*del1+(y[i-1]-yp[i-1])*del2)/dels;
  }
}

void SplineDerivative( std::vector<double> &x,
                                        std::vector<double> &y,
                                        std::vector<double> &xout,
                                        std::vector<double> &yout,
                                        std::size_t deriv )
{
  // This routine calculates either the first or second derivative at specified abcissae.
  //
  // input
  //    deriv = 1: first derivative at xout[j] is returned in yout[j]
  //          = 2: second derivative at xout[j] is returned in yout[j]
  //       x:  array of increasing abcissae of the functional values
  //       y:  array of ordinates of the values, i.e.,
  //           y[k] is the functional value corresponding to x[k]
  //    xout:  array of the abcissae of the output points
  //           The values do not have to be in increasing order
  //  output
  //    yout:  array of the ordinate values of the output points
  //
  std::size_t n = x.size();
  std::size_t m = xout.size();
  //
  double const expmax = 80.0;
  double sigmap = std::max(0.0001,fabs(ExGlobals::GetTension()))*(n-1)/(x[n-1]-x[0]); // denormalized tension
  std::vector<double> yp( n, 0.0 );  // work array
  std::vector<double> w( n, 0.0 );   // work array
  //
  SplineSetup( x, y, yp, w, sigmap, expmax );
  //
  for( std::size_t j=0; j<m; ++j )
  {
    double xt = xout[j];
    if( xt < x[0] )xt = x[0];
    if( xt > x[n-1] )xt = x[n-1];
    std::size_t i;
    for( std::size_t ii=1; ii<n; ++ii )
    {
      i = ii;
      if( x[ii] >= xt )break;
    }
    double del1 = xt - x[i-1];
    double del2 = x[i] - xt;
    double dels = x[i] - x[i-1];
    //
    if( deriv == 1 )  // first derivative
    {
      if( sigmap*dels > expmax )
        yout[j] = (y[i]-yp[i]-y[i-1]+yp[i-1])/dels;
      else
        yout[j] = (yp[i]*cosh(sigmap*del1)-yp[i-1]*cosh(sigmap*del2))*
            sigmap/sinh(sigmap*dels)+(y[i]-yp[i]-y[i-1]+yp[i-1])/dels;
    }
    else if( deriv == 2 )  // second derivative
    {
      if( sigmap*dels > expmax )
        yout[j] = 0.0;
      else
        yout[j] = (yp[i]*sinh(sigmap*del1)+yp[i-1]*sinh(sigmap*del2))*
            sigmap*sigmap/sinh(sigmap*dels);
    }
  }
}
 
double SplineDefiniteIntegral( std::vector<double> &x,
                                                std::vector<double> &y,
                                                double xlow, double xhi )
{
  // This routine evaluates the definite integral of the interpolated curve
  // between two input values.  The second value does not have to be greater
  // than the first.  The two abcissae must be within the range of x[0] and x[n-1].
  //
  // input
  //       x:  array of increasing abcissae of the functional values
  //       y:  array of ordinates of the values, i.e.,
  //           y[k] is the functional value corresponding to x[k]
  //    xlow:  low  x value
  //     xhi:  high x value
  //
  std::size_t n = x.size();
  if( ((xlow-x[0])*(xlow-x[n-1])>0.0 || (xhi-x[0])*(xhi-x[n-1])>0.0) )
    throw EExpressionError(wxT("DefiniteIntegral: must have x[1] < xlow,xhi < x[n]"));
  //
  double const expmax = 80.0;
  double sigmap = std::max(0.0001,fabs(ExGlobals::GetTension()))*(n-1)/(x[n-1]-x[0]); // denormalized tension
  std::vector<double> yp( n, 0.0 );  // work array
  std::vector<double> w( n, 0.0 );   // work array
  //
  SplineSetup( x, y, yp, w, sigmap, expmax );
  //
  // find which interval contains xlow, xhi
  //
  std::size_t i;
  for( std::size_t ii=1; ii<n; ++ii )
  {
    i = ii;
    if( (xlow-x[ii-1])*(xlow-x[ii]) <= 0.0 )break;
  }
  std::size_t low = i;
  for( std::size_t ii=1; ii<n; ++ii )
  {
    i = ii;
    if( (xhi-x[ii-1])*(xhi-x[ii]) <= 0.0 )break;
  }
  std::size_t hi = i;
  //
  // fill the w[i] array of integration constants
  //
  if( sigmap*(x[1]-x[0]) > expmax )
    w[0] = yp[0]/tanh(sigmap*(x[1]-x[0]))/sigmap+(y[1]-yp[1])*x[0]*x[0]/2.0/(x[1]-x[0])-
        (y[0]-yp[0])*(x[1]-x[0]/2.0)*x[0]/(x[1]-x[0]);
  else
    w[0] = -(yp[1]-yp[0]*cosh(sigmap*(x[1]-x[0])))/sigmap/sinh(sigmap*(x[1]-x[0]))+
        (y[1]-yp[1])*x[0]*x[0]/2.0/(x[1]-x[0])-(y[0]-yp[0])*(x[1]-x[0]/2.0)*x[0]/(x[1]-x[0]);
  for( std::size_t i=1; i<n-1; ++i )
  {
    if( sigmap*(x[i]-x[i-1])>expmax || sigmap*(x[i+1]-x[i])>expmax )
      w[i] = w[i-1] +
          (-yp[i-1]*x[i]*x[i]/(x[i]-x[i-1])/2.0-yp[i+1]*x[i]*x[i]/(x[i+1]-x[i])/2.0+
           yp[i]*(1.0/tanh(sigmap*(x[i]-x[i-1]))+1.0/tanh(sigmap*(x[i+1]-x[i])))/sigmap-
           yp[i]*((0.5*x[i]-x[i-1])*x[i]/(x[i]-x[i-1])-(x[i+1]-0.5*x[i])*x[i]/(x[i+1]-x[i]))+
           (y[i+1]*x[i]*x[i]/2.0-y[i]*(x[i+1]-x[i]/2.0)*x[i])/(x[i+1]-x[i])+
           (y[i]*(x[i]/2.0-x[i-1])*x[i]+y[i-1]*x[i]*x[i]/2.0)/(x[i]-x[i-1]));
    else
      w[i] = w[i-1] +
          (-yp[i-1]*(1.0/sigmap/sinh(sigmap*(x[i]-x[i-1]))+x[i]*x[i]/(x[i]-x[i-1])/2.0)-
           yp[i+1]*(1.0/sigmap/sinh(sigmap*(x[i+1]-x[i]))+x[i]*x[i]/(x[i+1]-x[i])/2.0)+
           yp[i]*(1.0/tanh(sigmap*(x[i]-x[i-1]))+1.0/tanh(sigmap*(x[i+1]-x[i])))/sigmap-
           yp[i]*((0.5*x[i]-x[i-1])*x[i]/(x[i]-x[i-1])-(x[i+1]-0.5*x[i])*x[i]/(x[i+1]-x[i]))+
           (y[i+1]*x[i]*x[i]/2.0-y[i]*(x[i+1]-x[i]/2.0)*x[i])/(x[i+1]-x[i])+
           (y[i]*(x[i]/2.0-x[i-1])*x[i]+y[i-1]*x[i]*x[i]/2.0)/(x[i]-x[i-1]));
  }
  //
  // find f(low) = ylow
  //
  double ylow;
  if( sigmap*(x[low]-x[low-1]) > expmax )
    ylow = (y[low]-yp[low])*(xlow/2.0-x[low-1])*xlow/(x[low]-x[low-1])+
        (y[low-1]-yp[low-1])*(x[low]-xlow/2.0)*xlow/(x[low]-x[low-1])+w[low-1];
  else
    ylow = (yp[low]*cosh(sigmap*(xlow-x[low-1]))-
            yp[low-1]*cosh(sigmap*(x[low]-xlow)))/sigmap/sinh(sigmap*(x[low]-x[low-1]))+
        (y[low]-yp[low])*(xlow/2.0-x[low-1])*xlow/(x[low]-x[low-1])+
        (y[low-1]-yp[low-1])*(x[low]-xlow/2.0)*xlow/(x[low]-x[low-1])+w[low-1];
  //
  // find f(xhi) = yhi
  //
  double yhi;
  if( sigmap*(x[hi]-x[hi-1]) > expmax )
    yhi = (y[hi]-yp[hi])*(xhi/2.0-x[hi-1])*xhi/(x[hi]-x[hi-1])+
        (y[hi-1]-yp[hi-1])*(x[hi]-xhi/2.0)*xhi/(x[hi]-x[hi-1])+w[hi-1];
  else
    yhi = (yp[hi]*cosh(sigmap*(xhi-x[hi-1]))-
           yp[hi-1]*cosh(sigmap*(x[hi]-xhi)))/sigmap/sinh(sigmap*(x[hi]-x[hi-1]))+
        (y[hi]-yp[hi])*(xhi/2.0-x[hi-1])*xhi/(x[hi]-x[hi-1])+
        (y[hi-1]-yp[hi-1])*(x[hi]-xhi/2.0)*xhi/(x[hi]-x[hi-1])+w[hi-1];
  //
  return yhi-ylow;
}

void SplineSetup( std::vector<double> const &x,
                                   std::vector<double> const &y,
                                   std::vector<double> &yp, std::vector<double> &w,
                                   double sigmap, double expmax )
{
  // This routine calculates the spline function parameters.
  //
  std::size_t n = x.size();
  //
  if( n == 2 )return; // use straight line for curve
  //
  // if no derivatives are given use second order polynomial
  // interpolation on input data for values at endpoints
  //
  double delx1 = x[1]-x[0];
  double delx2 = x[2]-x[1];
  double delx12 = x[2]-x[0];
  double c1 = -(delx12+delx1)/delx12/delx1;
  double c2 = delx12/delx1/delx2;
  double c3 = -delx1/delx12/delx2;
  double slpp1 = c1*y[0]+c2*y[1]+c3*y[2];
  double deln = x[n-1] - x[n-2];
  double delnm1 = x[n-2] - x[n-3];
  double delnn = x[n-1] - x[n-3];
  c1 = (delnn+deln)/delnn/deln;
  c2 = -delnn/deln/delnm1;
  c3 = deln/delnn/delnm1;
  double slppn = c3*y[n-3] + c2*y[n-2] + c1*y[n-1];
  //
  // set up right hand side and tridiagonal system for yp
  // and perform forwards elimination
  //
  double dels = sigmap*delx1;
  double diag1 = (dels/tanh(dels)-1.0)/delx1;
  double diagin = 1.0/diag1;
  double dx1 = (y[1]-y[0])/(x[1]-x[0]);
  yp[0] = diagin*(dx1-slpp1);
  double spdiag;
  dels > expmax ? spdiag = 1.0/delx1 : spdiag = (1.0-dels/sinh(dels))/delx1;
  w[0] = diagin*spdiag;
  double dx2;
  for( std::size_t i=1; i<n-1; ++i )
  {
    double delx2 = x[i+1] - x[i];
    dx2 = (y[i+1]-y[i])/delx2;
    dels = sigmap*delx2;
    double diag2 = (dels/tanh(dels)-1.0)/delx2;
    diagin = 1.0/(diag1+diag2-spdiag*w[i-1]);
    yp[i] = diagin*(dx2-dx1-spdiag*yp[i-1]);
    dels > expmax ? spdiag = 1.0/delx2 : spdiag = (1.0-dels/sinh(dels))/delx2;
    w[i] = diagin*spdiag;
    dx1 = dx2;
    diag1 = diag2;
  }
  diagin = 1.0/(diag1-spdiag*w[n-2]);
  yp[n-1] = diagin*(slppn-dx2-spdiag*yp[n-2]);
  //
  // perform back substitution
  //
  for( std::size_t i=1; i<n; ++i )yp[n-1-i] -= w[n-1-i]*yp[n-i];
}

void LinearInterpolate( std::vector<double> &x, std::vector<double> &y,
                                         std::vector<double> &xout,
                                         std::vector<double> &yout )
{
  // Linear interpolation (with extrapolation)
  // 
  // input
  //       x:  array of abcissae of the functional values
  //           must be monotonically increasing
  //       y:  array of ordinates of the values, i.e.,
  //           y[k] is the functional value corresponding to x[k]
  //    xout:  array of the abcissae of the output points
  //           must be monotonically increasing
  //  output
  //    yout:  yout[j] = interpolated ordinate of xout[j]
  //
  std::size_t nin = x.size();
  std::size_t nout = xout.size();
  std::size_t nl = 0;
  std::size_t nu = 1;
  for( std::size_t i=0; i<nout; ++i )
  {
    double xi = xout[i];
TOP:
    if( xi >= x[nin-1] )  // upper end extrapolation case
    {
      nl = nin-2;
      nu = nin-1;
      double frac = (xout[i]-x[nl])/(x[nu]-x[nl]);
      yout[i] = (1.0-frac)*y[nl] + frac*y[nu];
    }
    else if( xi <= x[0] )  // lower end extrapolation case
    {
      nl = 0;
      nu = 1;
      double frac = (xout[i]-x[nl])/(x[nu]-x[nl]);
      yout[i] = (1.0-frac)*y[nl] + frac*y[nu];
    }
    else  // interior point
    {
      if( xout[i] > x[nu] )
      {
        ++nu;
        ++nl;
        goto TOP;
      }
      else
      {
        double frac = (xout[i]-x[nl])/(x[nu]-x[nl]);
        yout[i] = (1.0-frac)*y[nl] + frac*y[nu];
      }
    }
  }
}

void LagrangeInterpolate( std::vector<double> &x, std::vector<double> &y,
                                           std::vector<double> &xout,
                                           std::vector<double> &yout )
{
  // General Lagrange interpolation (with extrapolation)
  //
  // input
  //       x:  array of abcissae of the functional values
  //           must be monotonically increasing
  //       y:  array of ordinates of the values, i.e.,
  //           y[k] is the functional value corresponding to x[k]
  //    xout:  array of the abcissae of the output points
  //           The values do not have to be in increasing order
  //  output
  //    yout:  yout[j] = interpolated ordinate of xout[j]
  //
  std::size_t nin = x.size();
  std::size_t nout = xout.size();
  if( nin == 1 )
  {
    for( std::size_t i=0; i<nout; ++i )yout[i] = y[0];
    return;
  }
  if( nin == 2 )
  {
    for( std::size_t i=0; i<nout; ++i )
      yout[i] = y[0]+(y[1]-y[0])/(x[1]-x[0])*(xout[i]-x[0]);
    return;
  }
  std::size_t ind = 1;
  if( nin <= 4 )ind = 0;
  //
  std::size_t np = 2*ind + 3;
  double rupr = (x[nin-ind-2]+x[nin-ind-1])/2.0;
  double rlow = (x[ind]+x[1+ind])/2.0;
  //
  // output points array xout need not be monotonic
  //
  std::size_t nlow = 0;
  for( std::size_t i=0; i<nout; ++i )
  {
    double xi = xout[i];
    if( xi < rupr )
    {
      if( xi > rlow )
      {
        for( ;; )
        {
          std::size_t i3 = nlow+ind+1;
          double rbu = (x[i3]+x[i3+1])/2.0;
          double rbl = (x[i3-1]+x[i3])/2.0;
          if( xi<=rbu && xi>=rbl )break;
          if( xi > rbu )++nlow;
          if( xi < rbl )--nlow;
        }
      }
      else // lower end extrapolation case
      {
        nlow = 0;
      }
    }
    else // upper end extrapolation case
    {
      nlow = nin-np;
    }
    for( std::size_t j=0; j<np; ++j )
    {
      double p = 1.0;
      std::size_t j1 = j+nlow;
      for( std::size_t l=0; l<np; ++l )
      {
        std::size_t l1 = l+nlow;
        if( l != j )p *= (xi-x[l1])/(x[j1]-x[l1]);
      }
      j == 0 ? yout[i] = y[j1]*p : yout[i] += y[j1]*p;
    }
    if( xi < x[0] )yout[i] = y[0]+(xi-x[0])/(x[1]-x[0])*(y[1]-y[0]);
    if( xi > x[nin-1] )
      yout[i] = y[nin-2] +
          (xi-x[nin-2])/(x[nin-1]-x[nin-2])*(y[nin-1]-y[nin-2]);
  }
}

void LagrangeDerivative( std::vector<double> &x, std::vector<double> &y,
                                          std::vector<double> &xout,
                                          std::vector<double> &yout,
                                          int np, int deriv )
{
  // Input:
  //        x    -array specifying montonic increasing independent variable
  //        y    -array specifying the dependent variable
  //     xout    -array specifying the locations of the desired derivatives
  //       np    -specifies the degree of interpolation
  //              e.g., np=5 means 5 point general Lagrange
  //              maximum value = 10, minimum value = 2
  //    deriv    -1 first derivative, 2 second derivative
  // Output:
  //     yout    -array returning the first or second derivative
  //
  // Restrictions: x must be strictly monotonically increasing
  //
  std::size_t nin = x.size();
  std::size_t nout = xout.size();
  double xw[10];
  int nlow = 0;
  if( deriv == 1 )
  {
    for( std::size_t i=1; i<=nout; ++i )
    {
      double xi = xout[i-1];
      if( xi > (x[nin-3]+x[nin-2])/2. )
      {
        nlow = nin-np;
        goto L120;
      }
L110:
      if( xi > (x[nlow+2]+x[nlow+3])/2. )
      {
        ++nlow;
        goto L110;
      }
L120:
      for( int k=1; k<=np; ++k )
      {
        double sump = 0.0;
        for( int j=1; j<=np; ++j )
        {
          if( j != k )
          {
            double p = 1.0;
            for( int m=1; m<=np; ++m )
            {
              if( m!=k && m!=j )p *= (xi-x[m+nlow-1])/(x[k+nlow-1]-x[m+nlow-1]);
            }
            p /= x[k+nlow-1]-x[j+nlow-1];
            sump += p;
          }
        }
        xw[k] = sump;
      }
      double fp = 0.0;
      for( int k=1; k<=np; ++k )fp += xw[k-1]*y[k+nlow-1];
      yout[i-1] = fp;
    }
  }
  else
  {
    double p2[10];
    for( std::size_t i=0; i<nout; ++i )
    {
      double xi = xout[i];
      if( xi > (x[nin-3]+x[nin-2])/2. )nlow = nin-np;
      if( xi <= (x[nin-3]+x[nin-2])/2. )
      {
        do
        {
          if( xi > (x[nlow+2]+x[nlow+3])/2. )++nlow;
        }
        while ( xi > (x[nlow+2]+x[nlow+3])/2. );
      }
      double p1 = 1.0;
      for( int k=0; k<np; ++k )p1 *= xi - x[k+nlow];
      for( int k=0; k<np; ++k )
      {
        double p = 1.0;
        for( int j=0; j<np; ++j )
        {
          if( j != k )p *= x[k+nlow] - x[j+nlow];
        }
        p2[k] = p;
        double sum1 = 0.0;
        for( int j=0; j<np; ++j )
        {
          if( j == k )continue;
          double sum2 = 0.0;
          for( int m=0; m<np; ++m )
          {
            if( m==k || m==j )continue;
            sum2 += p1/(p2[k]*(xi-x[m+nlow])*(xi-x[k+nlow])*(xi-x[j+nlow]));
          }
          sum1 += sum2;
        }
        xw[k] = sum1;
      }
      double fp = 0.0;
      for( int k=0; k<np; ++k )fp += xw[k]*y[k+nlow];
      yout[i] = fp;
    }
  }
}

void FritchCarlsonInterpolate( std::vector<double> &x,
                                                std::vector<double> &y,
                                                std::vector<double> &xout,
                                                std::vector<double> &yout,
                                                int deriv )
{
  // Fritch-Carlson interpolation (with extrapolation)
  //
  // input
  //       x:  array of abcissae of the functional values
  //           must be strictly monotonically increasing
  //       y:  array of ordinates of the values, i.e.,
  //           y[k] is the functional value corresponding to x[k]
  //    xout:  array of the abcissae of the output points
  //           must be increasing
  //  output
  //    yout:  yout[j] = interpolated ordinate of xout[j]
  //
  std::size_t nin = x.size();
  std::size_t nout = xout.size();
  std::vector<double> yp( nin, 0.0 );
  std::vector<double> c1( nin, 0.0 );
  std::vector<double> c2( nin, 0.0 );
  std::vector<double> c3( nin, 0.0 );
  //
  double one = 1.0;
  yp[0] = Deriv3( x[0], y[0], y[1], y[2], x[0], x[1], x[2] );
  for( std::size_t i=1; i<nin-1; ++i )
   yp[i] = Deriv3( x[i], y[i-1], y[i], y[i+1], x[i-1], x[i] ,x[i+1] );
  yp[nin-1] =
   Deriv3( x[nin-1], y[nin-3], y[nin-2], y[nin-1], x[nin-3], x[nin-2], x[nin-1] );
  for( std::size_t i=0; i<nin-1; ++i )
  {
    double hi = x[i+1]-x[i];
    double di = yp[i];
    double dip1 = yp[i+1];
    double delta = (y[i+1]-y[i])/hi;
    if( Sign(one,di)==Sign(one,dip1) && Sign(one,di)==Sign(one,delta) && delta!=0.0 )
    {
      // monotone condition satisfied - now does di & dip1 need adjusting ?
      //
      double alpha = di/delta;
      double beta = dip1/delta;
      if( alpha+beta>2.0 && 2.0*alpha+beta>3.0 && alpha+2.0*beta>3.0 &&
          3.0*alpha<(2.0*alpha+beta-3.0)*(2.0*alpha+beta-3.0)/(alpha+beta-2.0) )
      {
        // adjust di & dip1  (the derivatives of the two points )
        //
        double tau = 3.0/sqrt(alpha*alpha+beta*beta);
        di *= tau;
        dip1 *= tau;
      }
    }
    c1[i] = (di+dip1-2.0*delta)/(hi*hi);
    c2[i] = (3.0*delta-2.0*di-dip1)/hi;
    c3[i] = di;
  }
  //
  // perform interpolation on proper cubic
  //
  std::size_t j = 0;
  for( std::size_t i=0; i<nout; ++i )
  {
    for( ;; )
    {
      if( j==nin-2 || xout[i]<x[j+1] )
      {
        double diff = xout[i] - x[j];
        switch ( deriv )
        {
          case 0:
            yout[i] = c1[j]*diff*diff*diff + c2[j]*diff*diff + c3[j]*diff + y[j];
            break;
          case 1:
            yout[i] = 3.0*c1[j]*diff*diff + 2.0*c2[j]*diff + c3[j];
            break;
          case 2:
            yout[i] = 6.0*c1[j]*diff + 2.0*c2[j];
            break;
        }
        break;
      }
      else ++j; // bump interval
    }
  }
}

double Deriv3( double xout,
                                double x0, double x1, double x2,
                                double y0, double y1, double y2 )
{
  // returns the general 3 point Lagrange derivative at xout
  // given the 3 points (x0,y0), (x1,y1), and (x2,y2)
  //
  double t1 = (2.*xout-x1-x2)/((x0-x1)*(x0-x2));
  double t2 = (2.*xout-x0-x2)/((x1-x2)*(x1-x0));
  double t3 = (2.*xout-x0-x1)/((x2-x0)*(x2-x1));
  return y0*t1 + y1*t2 + y2*t3;
}

double IncompleteBeta( double x, double pin, double qin )
{
  // Reference: Slatec library; category b5f
  //   based on Bosten & Battiste, Remark on algorithm 179,
  //   Comm. ACM, vol. 17, p.153, (1974)
  //   betai
  //
  //  This function calculates the incomplete beta function, i<x>(p,q)
  //  i<x>(p,q) = b<x>(p,q) / b<1>(p,q) where b<x>(p,q)= integral from 0 to x of
  //  (t^(p-1) * (1-t)^(q-1)) dt; p,q>0; 0<=x<=1
  //  b<1>(p,q)= complete beta function
  //
  // input arguments
  // x -- upper limit of integration.  x must be in [0,1]
  // p -- first beta distribution parameter.  p must be > 0
  // q -- second beta distribution parameter.  q must be > 0
  //
  // The incomplete beta function ratio is the probability that a
  // random variable from a beta distribution having parameters
  // p and q will be less than or equal to x.
  //
  if( x<0.0 || x>1.0 )
    throw EExpressionError( wxT("IncompleteBeta: first argument must be in the range [0,1]") );
  if( pin < 0.0 )throw EExpressionError( wxT("IncompleteBeta: second argument < 0") );
  if( qin < 0.0 )throw EExpressionError( wxT("IncompleteBeta: third argument < 0") );
  //
  double const eps = std::numeric_limits<double>::epsilon();
  double const alneps = log( eps );
  double const sml = std::numeric_limits<double>::min();
  double const alnsml = log( sml );
  //
  double y, p, q;
  if( (qin<=pin && x<0.8) || x<0.2 )
  {
    y = x;
    p = pin;
    q = qin;
  }
  else
  {
    y = 1.0 - x;
    p = qin;
    q = pin;
  }
  double result = 0.0;
  if( (p+q)*y/(p+1.0) < eps )
  {
    try
    {
      double xb = p*log(std::max(y,sml)) - log(p) - LnCompleteBeta(p,q);
      if( xb>alnsml && y!=0.0 )result = exp(xb);
      if( y!=x || p!=pin )result = 1.0 - result;
    }
    catch( EExpressionError &e )
    {
      throw;
    }
    return result;
  }
  //
  // Evaluate the infinite sum first
  // term will equal y^p/beta(ps,p) * (1.-ps)-sub-i * y^i / fac(i)
  //
  double ps = q - static_cast<int>(q);
  if( ps == 0.0 )ps = 1.0;
  double xb;
  try
  {
    xb = p*log(y) - LnCompleteBeta(ps,p) - log(p);
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  if( xb >= alnsml )
  {
    result = exp( xb );
    double term = result*p;
    if( ps != 1.0 )
    {
      std::size_t n = static_cast<std::size_t>(std::max(alneps/log(y),4.0));
      for( std::size_t i=1; i<=n; ++i )
      {
        term *= (i-ps)*y/i;
        result += term/(p+i);
      }
    }
  }
  //
  // now evaluate the finite sum, maybe.
  //
  if( q > 1.0 )
  {
    try
    {
      xb = p*log(y) + q*log(1.0-y) - LnCompleteBeta(p,q) - log(q);
    }
    catch( EExpressionError &e )
    {
      throw;
    }
    std::size_t ib = static_cast<std::size_t>(std::max(xb/alnsml,0.0));
    double term = exp( xb-ib*alnsml );
    double c = 1.0/(1.0-y);
    double p1 = q*c/(p+q-1.0);
    //
    double finsum = 0.0;
    int n = static_cast<int>(q);
    if( q == static_cast<double>(n) )--n;
    for( int i=1; i<=n; ++i )
    {
      if( p1<=1.0 && term/eps<=finsum )break;
      term = (q-i+1.0)*c*term/(p+q-i);
      if( term > 1.0 )
      {
        --ib;
        term *= sml;
      }
      if( ib == 0 )finsum += term;
    }
    result += finsum;
  }
  if( y!=x || p!=pin )result = 1.0 - result;
  result = std::max( std::min( result, 1.0 ), 0.0 );
  return result;
}

double LnCompleteBeta( double a, double b )
{
  // Reference: Slatec library; category b5f,b3
  // dlbeta
  //
  // Computes the  natural logarithm of the complete beta function.
  //
  // input parameters:
  //  a --  real and positive
  //  b --  real and positive
  //
  double p = std::min( a, b );
  double q = std::max( a, b );
  if( p <= 0.0 )
    throw EExpressionError( wxT("LnCompleteBeta: both arguments must be > zero") );
  //
  try
  {
    if( p >= 10.0 ) // p and q are big
    {
      double corr = LogGammaCorrection(p) + LogGammaCorrection(q) - LogGammaCorrection(p+q);
      return -0.5*log(q) + log(sqrt(2*M_PI)) + corr + (p-0.5)*log(p/(p+q)) +
          q*LnRel(-p/(p+q));
    }
    if( q >= 10.0 ) // p is small, but q is big
    {
      double corr = LogGammaCorrection(q) - LogGammaCorrection(p+q);
      return LnAbsGamma(p) + corr + p - p*log(p+q) + (q-0.5)*LnRel(-p/(p+q));
    }
    //
    // p and q are small
    //
    return log( CompleteGamma(p)*(CompleteGamma(q)/CompleteGamma(p+q)) );
  }
  catch( EExpressionError &e )
  {
    throw;
  }
}

double LnRel( double x )
{
  // Reference: Slatec library; category b3
  // dlnrel
  //
  // Evaluates  ln(1+x)  accurate in the relative error sense
  //
  // Series for alnr  on the interval -3.75000e-01 to  3.75000e-01
  // with weighted error   6.35e-32
  // log weighted error  31.20
  // significant figures required  30.93
  // decimal places required  32.01
  //
  double const eps = std::numeric_limits<double>::epsilon();
  double const xmin = -1.0 + sqrt(2*eps);
  if( x <= -1.0 )
    throw EExpressionError( wxT("LnRel: argument must be > -1") );
  if( x < xmin )
    throw EExpressionError( wxT("LnRel: answer < half precision because argument is too near -1") );
  //
  double const a[43] =
      { +.10378693562743769800686267719098e+1,  -.13364301504908918098766041553133e+0,
        +.19408249135520563357926199374750e-1,  -.30107551127535777690376537776592e-2,
        +.48694614797154850090456366509137e-3,  -.81054881893175356066809943008622e-4,
        +.13778847799559524782938251496059e-4,  -.23802210894358970251369992914935e-5,
        +.41640416213865183476391859901989e-6,  -.73595828378075994984266837031998e-7,
        +.13117611876241674949152294345011e-7,  -.23546709317742425136696092330175e-8,
        +.42522773276034997775638052962567e-9,  -.77190894134840796826108107493300e-10,
        +.14075746481359069909215356472191e-10, -.25769072058024680627537078627584e-11,
        +.47342406666294421849154395005938e-12, -.87249012674742641745301263292675e-13,
        +.16124614902740551465739833119115e-13, -.29875652015665773006710792416815e-14,
        +.55480701209082887983041321697279e-15, -.10324619158271569595141333961932e-15,
        +.19250239203049851177878503244868e-16, -.35955073465265150011189707844266e-17,
        +.67264542537876857892194574226773e-18, -.12602624168735219252082425637546e-18,
        +.23644884408606210044916158955519e-19, -.44419377050807936898878389179733e-20,
        +.83546594464034259016241293994666e-21, -.15731559416479562574899253521066e-21,
        +.29653128740247422686154369706666e-22, -.55949583481815947292156013226666e-23,
        +.10566354268835681048187284138666e-23, -.19972483680670204548314999466666e-24,
        +.37782977818839361421049855999999e-25, -.71531586889081740345038165333333e-26,
        +.13552488463674213646502024533333e-26, -.25694673048487567430079829333333e-27,
        +.48747756066216949076459519999999e-28, -.92542112530849715321132373333333e-29,
        +.17578597841760239233269760000000e-29, -.33410026677731010351377066666666e-30,
        +.63533936180236187354180266666666e-31 };
  //
  double result = 0.0;
  try
  {
    int const n = Initds( a, 43, 0.1*eps );
    fabs(x) <= 0.375 ? result = x*(1.0-x*NTermChebyshevSeries(x/.375,a,n)) :
                       result = log(1.0+x);
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  return result;
}

int Initds( double const *dos, int nos, double eta )
{
  // Reference: Slatec library, category b5r
  // initds
  //
  // Initialize the double precision orthogonal series dos so that initds
  // is the number of terms needed to insure the error is no larger than
  // eta.  Ordinarily eta will be chosen to be one-tenth machine precision
  //
  // input arguments --
  //  dos    dble prec array of nos coefficients in an orthogonal series.
  //  nos    number of coefficients in dos.
  //  eta    requested accuracy of series.
  //
  if( nos < 1 )
    throw EExpressionError( wxT("Initds: number of coefficients < 1") );
  double err = 0.;
  int i;
  for( int ii=1; ii<=nos; ++ii )
  {
    i = nos + 1 - ii;
    err += fabs(dos[i-1]);
    if( err > eta )break;
  }
  if( i == nos )
    throw EExpressionError( wxT("Initds: eta may be too small") );
  return i;
}

double LnAbsGamma( double x )
{
  // Reference: Slatec library; category b5f, b3
  // dlngam
  //
  // Computes the logarithm of the absolute value of the gamma function
  //
  double xmax = std::numeric_limits<double>::max();
  xmax /= log(xmax);
  double dxrel = sqrt(2*std::numeric_limits<double>::epsilon());
  double y = fabs(x);
  try
  {
    if( y <= 10.0 )return log(fabs(CompleteGamma(x)));
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  //
  // log(fabs(dgamma(x))) for fabs(x) > 10.0
  //
  if( y > xmax )
    throw EExpressionError( wxT("LnAbsGamma: fabs(x) causes overflow") );
  try
  {
    if( x > 0.0 )return log(sqrt(2*M_PI)) + (x-0.5)*log(x) - x + LogGammaCorrection(y);
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  double sinpiy = fabs(sin(M_PI*y));
  if( sinpiy == 0.0 )
    throw EExpressionError( wxT("LnAbsGamma: x is a negative integer") );
  if( fabs((x-static_cast<int>(x-0.5))/x) < dxrel )
    throw EExpressionError(
     wxT("LnAbsGamma: answer < half precision because x too near negative integer") );
  try
  {
    return log(sqrt(M_PI/2)) + (x-0.5)*log(y) - x - log(sinpiy) - LogGammaCorrection(y);
  }
  catch( EExpressionError &e )
  {
    throw;
  }
}

double LnGamma( double x )
{
  // Natural logarithm of Gamma function
  // from:  NUMERICAL RECIPES  THE ART OF SCIENTIFIC COMPUTING
  // William H. Press, Brian P. Flannery, Saul A. Teukolsky, and
  // William T. Vetterling    Cambridge University Press   1986  pg 156
  // gammln
  //
  if( x <= 0.0 )throw EExpressionError(wxT("LnGamma: argument is negative") );
  //
  double const cof[] = {76.18009173e0, -86.50532033e0, 24.01409822e0,
                        -1.231739516e0, 0.120858003e-2, -0.536382e-5};
  double const stp = 2.50662827465;
  x -= 1.0;
  double tmp = x + 5.5;
  tmp = (x + 0.5)*log(tmp)-tmp;
  double ser = 1.0;
  for( int j=0; j<6; ++j )
  {
    x += 1.0;
    ser += cof[j]/x;
  }
  return tmp + log(stp*ser);
}

double CompleteGamma( double x )
{
  // Reference: Slatec library; category b5f
  // dgamma
  //
  // Computes the complete gamma function
  //
  // series for gamma on the interval  0. to 1.00000e+00
  // with weighted error   5.79e-32
  // log weighted error  31.24
  // significant figures required  30.00
  // decimal places required  32.05
  //
  double gamcs[42] = {
      +.8571195590989331421920062399942e-2,  +.4415381324841006757191315771652e-2,
      +.5685043681599363378632664588789e-1,  -.4219835396418560501012500186624e-2,
      +.1326808181212460220584006796352e-2,  -.1893024529798880432523947023886e-3,
      +.3606925327441245256578082217225e-4,  -.6056761904460864218485548290365e-5,
      +.1055829546302283344731823509093e-5,  -.1811967365542384048291855891166e-6,
      +.3117724964715322277790254593169e-7,  -.5354219639019687140874081024347e-8,
      +.9193275519859588946887786825940e-9,  -.1577941280288339761767423273953e-9,
      +.2707980622934954543266540433089e-10, -.4646818653825730144081661058933e-11,
      +.7973350192007419656460767175359e-12, -.1368078209830916025799499172309e-12,
      +.2347319486563800657233471771688e-13, -.4027432614949066932766570534699e-14,
      +.6910051747372100912138336975257e-15, -.1185584500221992907052387126192e-15,
      +.2034148542496373955201026051932e-16, -.3490054341717405849274012949108e-17,
      +.5987993856485305567135051066026e-18, -.1027378057872228074490069778431e-18,
      +.1762702816060529824942759660748e-19, -.3024320653735306260958772112042e-20,
      +.5188914660218397839717833550506e-21, -.8902770842456576692449251601066e-22,
      +.1527474068493342602274596891306e-22, -.2620731256187362900257328332799e-23,
      +.4496464047830538670331046570666e-24, -.7714712731336877911703901525333e-25,
      +.1323635453126044036486572714666e-25, -.2270999412942928816702313813333e-26,
      +.3896418998003991449320816639999e-27, -.6685198115125953327792127999999e-28,
      +.1146998663140024384347613866666e-28, -.1967938586345134677295103999999e-29,
      +.3376448816585338090334890666666e-30, -.5793070335782135784625493333333e-31 };
  double const pi = 3.14159265358979323846264338327950;
  double const sq2pil = 0.91893853320467274178032973640562;
  double eps = std::numeric_limits<double>::epsilon();
  int ngam;
  try
  {
    ngam = Initds( gamcs, 42, 0.1*eps );
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  double xmin, xmax;
  try
  {
    ComputeGammaBounds( xmin, xmax );
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  double dxrel = sqrt(2*eps);
  double y = fabs(x);
  double result = 0.0;
  if( y > 10.0 ) // gamma(x) for fabs(x) > 10
  {
    if( x > xmax )throw EExpressionError( wxT("CompleteGamma: x is too large") );
    if( x < xmin )throw EExpressionError( wxT("CompleteGamma: x is too small") );
    try
    {
      result = exp( (y-0.5)*log(y) - y + sq2pil + LogGammaCorrection(y) );
    }
    catch( EExpressionError &e )
    {
      throw;
    }
    if( x > 0.0 )return result;
    if( fabs((x-static_cast<int>(x-0.5))/x) < dxrel )
      throw EExpressionError(
       wxT("CompleteGamma: answer < half precision, x too near negative integer") );
    double sinpiy = sin(pi*y);
    if( sinpiy == 0.0 )
      throw EExpressionError( wxT("CompleteGamma: x is a negative integer") );
    result = -pi/(y*sinpiy*result);
  }
  //
  // Compute gamma(x) for -xbnd <= x <= xbnd
  // Reduce interval and find gamma(1+y) for 0 <= y < 1 first of all
  //
  int n = static_cast<int>(x);
  if( x < 0.0 )--n;
  y = x - n;
  --n;
  try
  {
    result = 0.9375 + NTermChebyshevSeries( 2.0*y-1.0, gamcs, ngam );
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  if( n == 0 )return result;
  if( n > 0 ) // gamma(x) for x >= 2 and x <= 10
  {
    for( int i=1; i<=n; ++i )result *= y+i;
    return result;
  }
  //
  // compute gamma(x) for x < 1.0
  //
  n *= -1;
  if( x ==  0.0 )
    throw EExpressionError( wxT("CompleteGamma: x is 0 ") );
  if( x < 0.0 && x+n-2==0.0 )
    throw EExpressionError( wxT("CompleteGamma: x is a negative integer ") );
  if( x<-0.5 && fabs((x-static_cast<int>(x-0.5))/x)<dxrel )
    throw EExpressionError(
     wxT("CompleteGamma: answer < half precision because x too near negative integer") );
  for( int i=1; i<=n; ++i )result /= x+i-1;
  return result;
}

double ReciprocalGamma( double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5F
  // CALCULATES  RECIPROCAL GAMMA FUNCTION.
  // N.B. THIS ROUTINE, NOT DGAMMA(X), SHOULD BE THE FUNDAMENTAL ONE.
  // dgamr
  //
  if( x <= 0.0 && static_cast<double>(static_cast<int>(x))==x )return 0.0;
  if( fabs(x) <= 10.0 )return 1.0/CompleteGamma(x);
  //
  double alngx = LnAbsGamma(x);
  int sgngx;
  x<=0.0 && static_cast<int>(-static_cast<int>(x)%2+0.1)==0 ? sgngx=-1 : sgngx=1;
  //
  return sgngx*exp(-alngx);
}

double IncompleteGamma( double a, double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5F
  // CALCULATES THE  INCOMPLETE GAMMA FUNCTION.
  // GAMMAI = INTEGRAL FROM T = 0 TO X OF EXP(-T) * T**(A-1.0)
  // gammai
  //
  // GAMMAI IS EVALUATED FOR POSITIVE VALUES OF A AND NON-NEGATIVE VALUES
  // OF X.  A SLIGHT DETERIORATION OF 2 OR 3 DIGITS ACCURACY WILL OCCUR
  // WHEN GAMMAI IS VERY LARGE OR VERY SMALL, BECAUSE LOGARITHMIC
  // VARIABLES ARE USED.
  //
  if( a <= 0.0 )throw EExpressionError( wxT("IncompleteGamma: first argument <= 0") );
  if( x < 0.0 )throw EExpressionError( wxT("IncompleteGamma: second argument < 0") );
  if( x == 0.0 )return 0.0;
  //
  // the only error possible in the expression below is a fatal overflow.
  //
  return (exp(LnAbsGamma(a)+a*log(x)))*TricomiSIncomplete(a,x);
}

double IncompleteGamma2( double a, double x )
{
  // Incomplete Gamma function GAMMQ(a,x) = 1 - gammap(a,x)  where
  // gammap(a,x) = integral from 0 to x of e^(-t) * t^(a-1) * dt divided by gamma(a)
  // gammq
  //
  // from:  NUMERICAL RECIPES  THE ART OF SCIENTIFIC COMPUTING
  //        William H. Press, Brian P. Flannery, Saul A. Teukolsky, and
  //        William T. Vetterling    Cambridge University Press   1986  pg 160
  //
  if( a <= 0.0 )throw EExpressionError( wxT("IncompleteGamma2: first argument <= 0") );
  if( x < 0.0 )throw EExpressionError( wxT("IncompleteGamma2: second argument < 0") );
  if( x < a+1.0 )
  {
    // calculate the incomplete gamma function P(a,x)
    // evaluated by its series representation
    //
    int const itmax = 1000;
    double const eps = 3.0e-7;
    //
    double gamser;
    if( x == 0.0 )gamser = 0.0;
    else
    {
      double ap = a;
      double sum = 1.0/a;
      double del = sum;
      bool flag = false;
      for( int n=1; n<=itmax; ++n )
      {
        ap += 1.0;
        del *= x/ap;
        sum += del;
        if( fabs(del) < fabs(sum)*eps )
        {
          flag = true;
          break;
        }
      }
      if( !flag )throw EExpressionError( wxT("IncompleteGamma2: failed to converge") );
      gamser = sum*exp(-x+a*log(x)-LnGamma(a));
    }
    return 1.0-gamser;
  }
  else
  {
    // Calculate the incomplete gamma function Q(a,x) evaluated by its
    // continued fraction representation.
    //
    int const itmax=1000;
    double const eps=3.0e-7;
    double g, gold=0.0;
    double a0 = 1.0;
    double a1 = x;
    double b0 = 0.0;
    double b1 = 1.0;
    double fac = 1.0;
    bool flag = false;
    for( int n=1; n<=itmax; ++n )
    {
      a0 = (a1 + a0*(n-a))*fac;
      b0 = (b1 + b0*(n-a))*fac;
      a1 = x*a0 + n*fac*a1;
      b1 = x*b0 + n*fac*b1;
      if( a1 != 0.0 )
      {
        fac = 1.0/a1;
        g = b1*fac;
        if( fabs((g-gold)/g) < eps )
        {
          flag = true;
          break;
        }
        gold = g;
      }
    }
    if( !flag )throw EExpressionError( wxT("IncompleteGamma2: failed to converge") );
    return exp(-x+a*log(x)-LnGamma(a))*g;
  }
}

double ComplementaryIncompleteGamma( double a, double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5F
  // CALCULATES THE  COMPLEMENTARY INCOMPLETE GAMMA FUNCTION.
  // GAMIC = INTEGRAL FROM T = X TO INFINITY OF EXP(-T) * T**(A-1.)  .
  // gamaic
  //
  // GAMIC IS EVALUATED FOR ARBITRARY REAL VALUES OF A AND FOR NON-NEGATIVE
  // VALUES OF X (EVEN THOUGH GAMIC IS DEFINED FOR X < 0.0), EXCEPT THAT
  // FOR X = 0 AND A <= 0.0, GAMIC IS UNDEFINED.
  //
  // A SLIGHT DETERIORATION OF 2 OR 3 DIGITS ACCURACY WILL OCCUR WHEN
  // GAMIC IS VERY LARGE OR VERY SMALL IN ABSOLUTE VALUE, BECAUSE LOG-
  // ARITHMIC VARIABLES ARE USED.  ALSO, IF THE PARAMETER A IS VERY CLOSE
  // TO A NEGATIVE INTEGER (BUT NOT A NEGATIVE INTEGER), THERE IS A LOSS
  // OF ACCURACY, WHICH IS REPORTED IF THE RESULT IS LESS THAN HALF
  // MACHINE PRECISION.
  //
  // REF. -- W. GAUTSCHI, AN EVALUATION PROCEDURE FOR INCOMPLETE GAMMA
  // FUNCTIONS, ACM TRANS. MATH. SOFTWARE.
  //
  if( x < 0.0 )throw EExpressionError(wxT("ComplementaryIncompleteGamma: second argument < 0"));
  if( x == 0.0 )
  {
    if( a <= 0.0 )throw EExpressionError(
     wxT("ComplementaryIncompleteGamma: first argument = 0 and second argument < 0"));
    return exp(LnAbsGamma(a+1.0)-log(a));
  }
  //
  double eps = 0.5*std::numeric_limits<double>::epsilon();
  double sqeps = sqrt(2*std::numeric_limits<double>::epsilon());
  double alneps = -log(std::numeric_limits<double>::epsilon());
  //
  double alx = log(x);
  double sga;
  a != 0.0 ? sga=Sign(1.0,a) : sga=1.0;
  int ainta = static_cast<int>(a+0.5*sga);
  double aeps = a - ainta;
  int sgngs, sgngam, izero=0;
  double algap1, alngs;
  if( x >= 1.0 )
  {
    if( a < x )return exp(LogComplementaryIncompleteGamma(a,x,alx));
    sgngam = 1;
    algap1 = LnAbsGamma(a+1.0);
    sgngs = 1;
    alngs = LogTricomiS(a,x,algap1);
  }
  else
  {
    if( a<=0.5 && fabs(aeps)<=0.001 )
    {
      double e = 2.0;
      if( -ainta > 1 )e = 2.0*(-ainta+2.0)/(ainta*ainta-1.0);
      e -= alx * pow(x,-0.001);
      if( e*fabs(aeps) <= eps )return ComplementaryIncompleteGamma2(a,x,alx);
    }
    algap1 = LnAbsGamma( a+1.0 );
    a<=-1.0 && static_cast<int>(-static_cast<int>(a+1)%2+0.1)==0 ? sgngam=-1 : sgngam=1;
    double gstar = TricomiS(a,x,algap1,sgngam);
    if( gstar == 0.0 )izero = 1;
    else
    {
      alngs = log(fabs(gstar));
      sgngs = static_cast<int>(Sign(1.0,gstar));
    }
  }
  //
  // evaluation in terms of tricomi-s incomplete gamma function
  //
  if( izero == 1 )return sga*sgngam*exp(algap1-log(fabs(a)));
  double t = a*alx + alngs;
  if( t > alneps )return -sgngs*sga*sgngam*exp(t+algap1-log(fabs(a)));
  double h;
  t > -alneps ? h=1.0-sgngs*exp(t) : h=1.0;
  if( fabs(h) < sqeps )
    throw EExpressionError(wxT("ComplementaryIncompleteGamma: result < half machine precision"));
  return Sign(1.0,h)*sga*sgngam*exp(log(fabs(h))+algap1-log(fabs(a)));
}

double ComplementaryIncompleteGamma2( double a, double x, double alx )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5F
  // COMPUTE THE COMPLEMENTARY INCOMPLETE GAMMA FUNCTION FOR A NEAR A NEGATIVE
  // INTEGER AND FOR SMALL X
  // d9gmic
  //
  if( a > 0.0 )
    throw EExpressionError(wxT("ComplementaryIncompleteGamma2: a must be near a negative integer"));
  if( x <= 0.0 )
    throw EExpressionError(wxT("ComplementaryIncompleteGamma2: x must be > 0"));
  //
  double const euler = 0.57721566490153286060651209008240;
  double const eps = 0.5*std::numeric_limits<double>::epsilon();
  double const bot = log(std::numeric_limits<double>::min());
  //
  int m = static_cast<int>(-(a-0.5));
  double te = 1.0;
  double s = 1.0;
  bool flag = false;
  for( int k=1; k<=200; ++k )
  {
    te *= -x/(m+k+1);
    s += te/(k+1);
    if( fabs(te/(k+1)) < eps*s )
    {
      flag = true;
      break;
    }
  }
  if( !flag )throw EExpressionError(
    wxT("ComplementaryIncompleteGamma2: no convergence in 200 terms of continued fraction"));
  //
  double result = -alx-euler+x*s/(m+1);
  if( m == 0 )return result;
  if( m == 1 )return -result-1.0+1.0/x;
  //
  te = m;
  s = 1.0;
  for( int k=1; k<=m-1; ++k )
  {
    te = -x*te/k;
    s += te/(m-k);
    if( fabs(te/(m-k)) < eps*fabs(s) )break;
  }
  for( int k=1; k<=m; ++k )result += 1.0/k;
  double sgng;
  m%2 == 1 ? sgng=-1.0 : sgng=1.0;
  double alng = log(result) - LnAbsGamma(m+1.0);
  result = 0.0;
  if( alng > bot )result = sgng*exp(alng);
  if( s != 0.0 )result += Sign( exp(-m*alx+log(fabs(s)/m)), s );
  if( result == 0.0 && s==0.0 )
    throw EExpressionError(wxT("ComplementaryIncompleteGamma2: result underflows"));
  return result;
}

double LogComplementaryIncompleteGamma( double a, double x, double alx )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5F
  // COMPUTE THE LOG COMPLEMENTARY INCOMPLETE GAMMA FUNCTION FOR LARGE X AND FOR A <= X
  // d9lgic
  //
  if( x<=0.0 || a>x )
    throw EExpressionError(wxT("LogComplementaryIncompleteGamma: x should > 0 and >= a"));
  double eps = 0.5*std::numeric_limits<double>::epsilon();
  double xpa = x + 1.0 - a;
  double xma = x - 1.0 - a;
  double r = 0.0;
  double p = 1.0;
  double s = p;
  for( int k=1; k<=300; ++k )
  {
    double fk = k;
    double t = fk*(a-fk)*(1.0+r);
    r = -t/((xma+2.0*fk)*(xpa+2.0*fk)+t);
    p *= r;
    s += p;
    if( fabs(p) < eps*s )return a*alx-x+log(s/xpa);
  }
  throw EExpressionError(
   wxT("LogComplementaryIncompleteGamma: no convergence in 300 terms of continued fraction"));
}

double TricomiSIncomplete( double a, double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5F
  // CALCULATES TRICOMI-S FORM of the  INCOMPLETE GAMMA FUNCTION ,
  // GAMIT = X**(-A)/GAMMA(A) * INTEGRAL T = 0 TO X OF EXP(-T) * T**(A-1.)
  //       = X**(-A)/GAMMA(A) * GAMMAI(A,X)
  // gamait
  //
  // and ANALYTIC CONTINUATION for A <= 0.0.  GAMMA(X) is the  COMPLETE
  // GAMMA FUNCTION OF X.  GAMIT is evaluated for arbitrary REAL VALUES
  // of A and for NON-NEGATIVE VALUES OF X (even though GAMIT is defined
  // for X < 0.0), except that for X = 0 and A <= 0.0, GAMIT is INFINITE,
  // A FATAL ERROR.
  //
  // A SLIGHT DETERIORATION OF 2 OR 3 DIGITS ACCURACY WILL OCCUR WHEN
  // GAMIT IS VERY LARGE OR VERY SMALL IN ABSOLUTE VALUE, BECAUSE LOG-
  // ARITHMIC VARIABLES ARE USED.  ALSO, IF THE PARAMETER A IS VERY CLOSE
  // TO A NEGATIVE INTEGER (BUT NOT A NEGATIVE INTEGER), THERE IS A LOSS
  // OF ACCURACY, WHICH IS REPORTED IF THE RESULT IS LESS THAN HALF
  // MACHINE PRECISION.
  //
  // REF. -- W. GAUTSCHI, AN EVALUATION PROCEDURE FOR INCOMPLETE GAMMA
  //         FUNCTIONS, ACM TRANS. MATH. SOFTWARE.
  //
  if( x < 0.0 )throw EExpressionError(wxT("TricomiSIncomplete: second argument < 0"));
  if( x==0.0 && a<=0.0 )throw EExpressionError(wxT("TricomiSIncomplete: result is infinite"));
  //
  double alneps = -log(std::numeric_limits<double>::epsilon());
  double sqeps = sqrt(2*std::numeric_limits<double>::epsilon());
  //
  double alx;
  x != 0.0 ? alx=log(x) : alx=0.0;
  double sga;
  a != 0.0 ? sga=Sign(1.0,a) : sga=1.0;
  int ainta = static_cast<int>(a+0.5*sga);
  double aeps = a - ainta;
  if( x == 0.0 )
  {
    double result = 0.0;
    if( ainta>0 || aeps!=0.0 )result = ReciprocalGamma(a+1);
    return result;
  }
  if( x <= 1.0 )
  {
    double algap1;
    int sgngam;
    if( a>=-0.5 || aeps!=0.0 )
    {
      algap1 = LnAbsGamma(a+1);
      a+1<=0.0 && static_cast<int>(-static_cast<int>(a+1)%2+0.1)==0 ? sgngam=-1 : sgngam=1;
    }
    return TricomiS(a,x,algap1,sgngam);
  }
  if( a >= x )return exp(LogTricomiS(a,x,LnAbsGamma(a+1.0)));
  //
  // evaluate gamait in terms of log(dgamic(a,x))
  //
  double h = 1.0;
  if( aeps==0.0 && ainta<=0.0 )return exp(-a*alx);
  //
  double algap1 = LnAbsGamma(a+1);
  int sgngam;
  a+1<=0.0 && static_cast<int>(-static_cast<int>(a+1)%2+0.1)==0 ? sgngam=-1 : sgngam=1;
  //
  double t = log(fabs(a)) + LogComplementaryIncompleteGamma(a,x,alx) - algap1;
  if( t > alneps )return -sga*sgngam*exp(t-a*alx);
  //
  if( t > -alneps )h = 1.0 - sga*sgngam*exp(t);
  if( fabs(h) > sqeps )return Sign(exp(-a*alx+log(fabs(h))),h);
  //
  throw EExpressionError(wxT("TricomiSIncomplete: result < half precision"));
}

double TricomiS( double a, double x, double algap1, int sgngam )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5F
  // COMPUTES  TRICOMI-S INCOMPLETE GAMMA FUNCTION FOR SMALL X
  // d9gmit
  //
  if( x <= 0.0 )throw EExpressionError(wxT("TricomiS: x should be > 0"));
  //
  double const eps = 0.5*std::numeric_limits<double>::epsilon();
  double const bot = log(std::numeric_limits<double>::min());
  int ma;
  ma = a<0.0 ? static_cast<int>(a-0.5) : static_cast<int>(a+0.5);
  double aeps = a - ma;
  double ae;
  a < -0.5 ? ae=aeps : ae=a;
  double t = 1.0;
  double te = ae;
  double s = t;
  bool flag = false;
  for( int k=1; k<=200; ++k )
  {
    te = -x*te/k;
    t = te/(ae+k);
    s += t;
    if( fabs(t) < eps*fabs(s) )
    {
      flag = true;
      break;
    }
  }
  if( !flag )
    throw EExpressionError(wxT("TricomiS: no convergence in 200 terms of Taylow-s series"));
  double algs;
  if( a >= -0.5 )
  {
    algs = -algap1 + log(s);
    return exp(algs);
  }
  algs = -LnAbsGamma(1.0+aeps) + log(s);
  s = 1.0;
  int m = -ma - 1;
  if( m != 0 )
  {
    t = 1.0;
    for( int k=1; k<=m; ++k )
    {
      t *= x/(aeps-(m+1-k));
      s += t;
      if( fabs(t) < eps*fabs(s) )break;
    }
  }
  double result = 0.0;
  algs = -ma*log(x) + algs;
  if( s==0.0 || aeps==0.0 )return exp(algs);
  double sgng2 = sgngam * Sign(1.0,s);
  double alg2 = -x - algap1 + log(fabs(s));
  if( alg2 > bot )result = sgng2 * exp(alg2);
  if( algs > bot )result += exp(algs);
  return result;
}

double LogTricomiS( double a, double x, double algap1 )
{
  // reference: slatec library; category b5f
  // computes  the log of tricomi-s incomplete gamma function with
  // perron-s continued fraction for large x and a >= x
  // d9lgit
  //
  double eps = 0.5*std::numeric_limits<double>::epsilon();
  double sqeps = sqrt(2*std::numeric_limits<double>::epsilon());
  if( x<=0.0 || a<x )
    throw EExpressionError(wxT("LogTricomiS: x should be > 0.0 and <= a"));
  double ax = a + x;
  double a1x = ax + 1.0;
  double r = 0.0;
  double p = 1.0;
  double s = p;
  for( int k=1; k<=200; ++k )
  {
    double fk = k;
    double t = (a+fk)*x*(1.0+r);
    r = t/((ax+fk)*(a1x+fk)-t);
    p *= r;
    s += p;
    if( fabs(p) < eps*s )
    {
      double hstar = 1.0 - x*s/a1x;
      if( hstar < sqeps )
        throw EExpressionError(wxT("LogTricomiS: result less than half precision"));
      return -x - algap1 - log(hstar);
    }
  }
  throw EExpressionError(wxT("LogTricomiS: no convergence in 200 terms of continued fraction"));
}

double LogGammaCorrection( double x )
{
  // Reference: Slatec library; category b5f
  // d9lgmc
  //
  // Computes the log gamma correction factor for x >= 10 so that
  // log(gamma(x)) = log(sqrt(2*pi)) + (x-5.)*log(x) - x + LogGammaCorrection(x)
  //
  // series for algm on the interval  0. to 1.00000e-02
  // with weighted error   1.28e-31
  // log weighted error  30.89
  // significant figures required  29.81
  // decimal places required  31.48
  //
  double const algmcs[15] = {
    +.1666389480451863247205729650822e+0,
    -.1384948176067563840732986059135e-4,
    +.9810825646924729426157171547487e-8,
    -.1809129475572494194263306266719e-10,
    +.6221098041892605227126015543416e-13,
    -.3399615005417721944303330599666e-15,
    +.2683181998482698748957538846666e-17,
    -.2868042435334643284144622399999e-19,
    +.3962837061046434803679306666666e-21,
    -.6831888753985766870111999999999e-23,
    +.1429227355942498147573333333333e-24,
    -.3547598158101070547199999999999e-26,
    +.1025680058010470912000000000000e-27,
    -.3401102254316748799999999999999e-29,
    +.1276642195630062933333333333333e-30 };
  double const eps = std::numeric_limits<double>::epsilon();
  int nalgm;
  try
  {
    nalgm = Initds( algmcs, 15, eps );
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  double const xbig = 1.0/sqrt(eps);
  double const xmax = exp( std::min( log(std::numeric_limits<double>::max()/12.0),
                               -log(12.0*std::numeric_limits<double>::min()) ) );
  if( x < 10.0 )
    throw EExpressionError( wxT("LogGammaCorrection: x must be >= 10") );
  if( x >= xmax )
    throw EExpressionError( wxT("LogGammaCorrection: x is too large") );
  double result = 1.0/(12.0*x);
  try
  {
    if( x < xbig )result = NTermChebyshevSeries(200.0/(x*x)-1.0,algmcs,nalgm)/x;
  }
  catch( EExpressionError &e )
  {
    throw;
  }
  return result;
}

double NTermChebyshevSeries( double x, double const *a, int n )
{
  // Reference: Slatec library, category b5r
  // dcsevl
  //
  // Evaluate the n-term Chebyshev series a at x.
  // Adapted from R. Broucke, Algorithm 446, CACM, 16, 254 (1973)
  //
  // input arguments --
  // x    Value at which the series is to be evaluated
  // a    Array of n terms of a Chebyshev series
  //      In evaluating a, only half the first coefficient is summed
  // n    Number of terms in array a
  //
  if( n < 1 )throw EExpressionError( wxT("NTermChebyshevSeries: number of terms < 0"));
  if( n > 1000 )throw EExpressionError( wxT("NTermChebyshevSeries: number of terms > 1000"));
  if( x<-1.0 || x>1.0 )throw EExpressionError( wxT("NTermChebyshevSeries: x must be in [-1,1]"));
  double twox = 2.0*x;
  double b1 = 0.0;
  double b0 = 0.0;
  double b2;
  for( int i=1; i<=n; ++i )
  {
    b2 = b1;
    b1 = b0;
    int ni = n - i + 1;
    b0 = twox*b1 - b2 + a[ni-1];
  }
  return 0.50*(b0-b2);
}

void ComputeGammaBounds( double &xmin, double &xmax )
{
  // Reference: Slatec library; category b5f
  // dgamlm
  //
  // Computes the  minimum and maximum bounds for x in gamma(x)
  // xmin and xmax are not the only bounds,
  // but they are the only nontrivial ones to calculate
  //
  // output arguments --
  //  xmin-- minimum legal value of x in gamma(x)
  //         any smaller value of x might result in underflow
  //  xmax-- maximum legal value of x in gamma(x)
  //         any larger value of x might cause overflow
  //
  double alnsml = log(std::numeric_limits<double>::min());
  xmin = -alnsml;
  for( std::size_t i=1; i<=10; ++i )
  {
    double xold = xmin;
    double xln = log(xmin);
    xmin = xmin - xmin*((xmin+0.5)*xln - xmin - 0.2258 + alnsml)/(xmin*xln+0.5);
    if( fabs(xmin-xold) < 0.005 )
    {
      xmin = -xmin + 0.01;
      double alnbig = log(std::numeric_limits<double>::max());
      xmax = alnbig;
      for( std::size_t j=1; j<=10; ++j )
      {
        xold = xmax;
        xln = log(xmax);
        xmax = xmax-xmax*((xmax-0.5)*xln-xmax+0.9189-alnbig)/(xmax*xln-0.5);
        if( fabs(xmax-xold) > 0.005 )
        {
          xmax -= 0.01;
          xmin = std::max( xmin, -xmax+1.0 );
          return;
        }
      }
      throw EExpressionError( wxT("ComputeGammaBounds: unable to find xmax") );
    }
  }
  throw EExpressionError( wxT("ComputeGammaBounds: unable to find xmin") );
}

double Airy( double x )
{
  // Reference: Slatec library, category b5i,b5l
  // calculates the Airy function
  //
  double const aifcs[13]={-.37971358496669997496197089469414e-1,
                          +.59191888537263638574319728013777e-1,
                          +.98629280577279975365603891044060e-3,
                          +.68488438190765667554854830182412e-5,
                          +.25942025962194713019489279081403e-7,
                          +.61766127740813750329445749697236e-10,
                          +.10092454172466117901429556224601e-12,
                          +.12014792511179938141288033225333e-15,
                          +.10882945588716991878525295466666e-18,
                          +.77513772196684887039238400000000e-22,
                          +.44548112037175638391466666666666e-25,
                          +.21092845231692343466666666666666e-28,
                          +.83701735910741333333333333333333e-32};
  double const aigcs[13]={+.18152365581161273011556209957864e-1,
                          +.21572563166010755534030638819968e-1,
                          +.25678356987483249659052428090133e-3,
                          +.14265214119792403898829496921721e-5,
                          +.45721149200180426070434097558191e-8,
                          +.95251708435647098607392278840592e-11,
                          +.13925634605771399051150420686190e-13,
                          +.15070999142762379592306991138666e-16,
                          +.12559148312567778822703205333333e-19,
                          +.83063073770821340343829333333333e-23,
                          +.44657538493718567445333333333333e-26,
                          +.19900855034518869333333333333333e-29,
                          +.74702885256533333333333333333333e-33};
  int naif, naig;
  try
  {
    naif = Initds(aifcs,13,0.1*std::numeric_limits<double>::epsilon());
    naig = Initds(aigcs,13,0.1*std::numeric_limits<double>::epsilon());
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  double const x3sml = pow(std::numeric_limits<double>::epsilon(),0.3334);
  double xmax = pow((-1.5*log(std::numeric_limits<double>::min())),0.6667);
  xmax = xmax - xmax*log(xmax)/(4.0*sqrt(xmax)+1.0) - 0.01;
  if( x >= -1.0 )
  {
    if( x > 1.0 )
    {
      if( x > xmax )throw EExpressionError(wxT("Airy: x so big AI underflows"));
      try
      {
        return AiryNegative(x)*exp(-2.0*x*sqrt(x)/3.0);
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
    else
    {
      double z = 0.0;
      if( fabs(x) > x3sml )z = pow(x,3.0);
      try
      {
        return 0.375+NTermChebyshevSeries(z,aifcs,naif) -
            x*(0.25+NTermChebyshevSeries(z,aigcs,naig));
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
  }
  else
  {
    double xm, theta;
    try
    {
      AiryModulusPhase( x, xm, theta );
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    return xm*cos(theta);
  }
}

void AiryModulusPhase( double x, double &ampl, double &theta )
{
  // Reference: Slatec library, category b5i,b5l
  // D9AIMP
  // evaluates the Airy modulus and phase for x <= -1.0
  //
  double const pi4 = 0.78539816339744830961566084581988;
  double const am20cs[57] = {
      +.108716749086561856615730588125e-1,
      +.369489228982663555091728665146e-3,
      +.440680100484689563667507001327e-5,
      +.143686762361911153929183952833e-6,
      +.824275552390078308670628855353e-8,
      +.684426758893661606173927278180e-9,
      +.739566697282739287731004740213e-10,
      +.974595633696825017638702600847e-11,
      +.150076885829405775650973119497e-11,
      +.262147910221527634206252854802e-12,
      +.508354111376487180357278966914e-13,
      +.107684753358811440492985997070e-13,
      +.246091286618433429335914062617e-14,
      +.600786380358656418436110373550e-15,
      +.155449156102388071150651388384e-15,
      +.423535125035576604426382780182e-16,
      +.120862166289299840154401109189e-16,
      +.359609651214658240861499706423e-17,
      +.111134218386395638261774604677e-17,
      +.355559532432366609893680289225e-18,
      +.117433021600139309998766947387e-18,
      +.399397454661077561389162200966e-19,
      +.139576671528916310425606325640e-19,
      +.500240055309236041393459280716e-20,
      +.183552760958132679184834866457e-20,
      +.688490998179202743197790112404e-21,
      +.263631035611417012359996885105e-21,
      +.102924890237338360287153563785e-21,
      +.409246966671594885489762960571e-22,
      +.165558573406734651039727903828e-22,
      +.680797467063033356116599685727e-23,
      +.284326559934079832419751134476e-23,
      +.120507398348965255097287818819e-23,
      +.517961243287505217976613610424e-24,
      +.225622613427562816303268640887e-24,
      +.995418801147745168832117078246e-25,
      +.444551696397342424308280582053e-25,
      +.200865195461501101425916097338e-25,
      +.917786344151775165973885645402e-26,
      +.423872958105589240661672197948e-26,
      +.197789272007846092370846251490e-26,
      +.932116351284620665680435253373e-27,
      +.443482133249918099955611379722e-27,
      +.212945672365573895594589552837e-27,
      +.103158569651075977552209344907e-27,
      +.504023773022591199157904590029e-28,
      +.248301304570155945304046541005e-28,
      +.123301783128562196054198238560e-28,
      +.617033449920521746121976730507e-29,
      +.311092617415918897233869792213e-29,
      +.157983085201706173015269071503e-29,
      +.807931987538283607678121339092e-30,
      +.415997394138667562722951360052e-30,
      +.215610934097716900471935862504e-30,
      +.112468857265869178296752823613e-30,
      +.590331560632838091123040811797e-31,
      +.311735667692928562046280505333e-31};
  double const ath0cs[53]={
      -.8172601764161634499840208700543e-1,
      -.8004012824788273287596481113068e-3,
      -.3186525268782113203795553628242e-5,
      -.6688388266477509330741698865033e-7,
      -.2931759284994564516506822463184e-8,
      -.2011263760883621669049030307186e-9,
      -.1877522678055973426074008166652e-10,
      -.2199637137704601251899002199848e-11,
      -.3071616682592272449025746605586e-12,
      -.4936140553673418361025600985389e-13,
      -.8902833722583660416935236969866e-14,
      -.1768987764615272613656814199467e-14,
      -.3817868689032277014678199609600e-15,
      -.8851159014819947594156286509984e-16,
      -.2184818181414365953149677679568e-16,
      -.5700849046986452380599442295119e-17,
      -.1563121122177875392516031795495e-17,
      -.4481437996768995067906688776353e-18,
      -.1337794883736188022044566044098e-18,
      -.4143340036874114453776852445442e-19,
      -.1327263385718805025080481164652e-19,
      -.4385728589128440522215756835955e-20,
      -.1491360695952818067686201743956e-20,
      -.5208104738630711377154238188773e-21,
      -.1864382222390498923872526604979e-21,
      -.6830263751167969012975435381881e-22,
      -.2557117058029329629296207591347e-22,
      -.9770158640254300218246907254046e-23,
      -.3805161433416679084068428254886e-23,
      -.1509022750737054063493926482995e-23,
      -.6087551341242424929005568014525e-24,
      -.2495879513809711495425982124058e-24,
      -.1039157654581920948909588084274e-24,
      -.4390235913976846536974594969051e-25,
      -.1880790678447990211675826820582e-25,
      -.8165070764199462948863022205753e-26,
      -.3589944503749750514266435585041e-26,
      -.1597658126632132872981291608708e-26,
      -.7193250175703823969113802835305e-27,
      -.3274943012727856506209351132721e-27,
      -.1507042445783690665816975047272e-27,
      -.7006624198319904717843967949140e-28,
      -.3289907402983718226528815678356e-28,
      -.1559518084365146526445322711496e-28,
      -.7460690508208254582833851119721e-29,
      -.3600877034824662020563277249431e-29,
      -.1752851437473772257350402219197e-29,
      -.8603275775188512909623778628724e-30,
      -.4256432603226946534668039480105e-30,
      -.2122161865044262927723650698206e-30,
      -.1065996156704879052472060798561e-30,
      -.5393568608816949116410688086892e-31,
      -.2748174851043954822278496517870e-31};
  double const am21cs[60]={
      +.592790266721309588375717482814e-2,
      +.200569405393165186428695217690e-2,
      +.911081850262275893553072526291e-4,
      +.849894306372047155633172107475e-5,
      +.113297908976913076637929215494e-5,
      +.187517946100666496180950627804e-6,
      +.359306519018245832699035211192e-7,
      +.765757714071683864039093517470e-8,
      +.176999967168039173925953460744e-8,
      +.436259555654598932720546585535e-9,
      +.113291641337853230035520085219e-9,
      +.307257690982419244137868398126e-10,
      +.864482416482201075541200465766e-11,
      +.251015250060924402115104562212e-11,
      +.749102496764440371601802227751e-12,
      +.228996928487994073089565214432e-12,
      +.715113658927987694949327491175e-13,
      +.227607924959566841946395165061e-13,
      +.736942142760886513969953227782e-14,
      +.242328675267827490463991742006e-14,
      +.808153774548239869283406558403e-15,
      +.273008079804356086659174563386e-15,
      +.933236070891385318473519474326e-16,
      +.322508099681084622213867546973e-16,
      +.112581932346444541217757573416e-16,
      +.396699463986938821660259459530e-17,
      +.141006567944319504660865034527e-17,
      +.505302086537851213375537393032e-18,
      +.182461523215945141197999102789e-18,
      +.663584568262130466928029121642e-19,
      +.242963731631276179741747455826e-19,
      +.895238915123687802013669922963e-20,
      +.331845289350050791260229250755e-20,
      +.123706196188658315384437905922e-20,
      +.463636677012390840306767734243e-21,
      +.174653135947764475469758765989e-21,
      +.661116810234991176307910643111e-22,
      +.251409918994072486176125666459e-22,
      +.960274995571732568694034386998e-23,
      +.368324952289296395686436898078e-23,
      +.141843138269159136145535939553e-23,
      +.548342674276935830106345800990e-24,
      +.212761054623118806650372562616e-24,
      +.828443700849418591487734760953e-25,
      +.323670563926127001421028600927e-25,
      +.126868882963286057355055062493e-25,
      +.498843818992121626935068934362e-26,
      +.196734584467649390967119381790e-26,
      +.778135971020326957713212064836e-27,
      +.308633941498911152919192968451e-27,
      +.122744647045453119789338037234e-27,
      +.489431279134292205885241216204e-28,
      +.195646879802909821175925099724e-28,
      +.783988952922426171166311492266e-29,
      +.314896914002484223748298978099e-29,
      +.126769763137250681307067842559e-29,
      +.511470691906900141641632107724e-30,
      +.206801709795538770250900316706e-30,
      +.837891344768519001325996867583e-31,
      +.340168991971489802052339079577e-31};
  double const ath1cs[58]={
      -.6972849916208883845888148415037e-1,
      -.5108722790650044987073448077961e-2,
      -.8644335996989755094525334749512e-4,
      -.5604720044235263542188698916125e-5,
      -.6045735125623897409156376640077e-6,
      -.8639802632488334393219721138499e-7,
      -.1480809484309927157147782480780e-7,
      -.2885809334577236039999449908712e-8,
      -.6191631975665699609309191231800e-9,
      -.1431992808860957830931365259879e-9,
      -.3518141102137214721504616874321e-10,
      -.9084761919955078290070339808051e-11,
      -.2446171672688598449343283664767e-11,
      -.6826083203213446240828996710264e-12,
      -.1964579931194940171278546257802e-12,
      -.5808933227139693164009191265856e-13,
      -.1759042249527441992795400959024e-13,
      -.5440902932714896613632538945319e-14,
      -.1715247407486806802622358519451e-14,
      -.5500929233576991546871101847161e-15,
      -.1791878287739317259495152638754e-15,
      -.5920372520086694197778411062231e-16,
      -.1981713027876483962470972206590e-16,
      -.6713232347016352262049984343790e-17,
      -.2299450243658281116122358619832e-17,
      -.7957300928236376595304637145634e-18,
      -.2779994027291784157172290233739e-18,
      -.9798924361326985224406795480814e-19,
      -.3482717006061574386702645565849e-19,
      -.1247489122558599057173300058084e-19,
      -.4501210041478228113487751824452e-20,
      -.1635346244013352135596114164667e-20,
      -.5980102897780336268098762265941e-21,
      -.2200246286286123454028196295475e-21,
      -.8142463073515085897408205291519e-22,
      -.3029924773660042537432330709674e-22,
      -.1133390098574623537722943969689e-22,
      -.4260766024749295719283049889791e-23,
      -.1609363396278189718797500634453e-23,
      -.6106377190825026293045330444287e-24,
      -.2326954318021694061836577887573e-24,
      -.8903987877472252604474129558186e-25,
      -.3420558530005675024117914752341e-25,
      -.1319026715257272659017212100607e-25,
      -.5104899493612043091316191177386e-26,
      -.1982599478474547451242444663466e-26,
      -.7725702356880830535636111851519e-27,
      -.3020234733664680100815776863573e-27,
      -.1184379739074169993712946380800e-27,
      -.4658430227922308520573252840106e-28,
      -.1837554188100384647157502006613e-28,
      -.7268566894427990953321876684800e-29,
      -.2882863120391468135527089875626e-29,
      -.1146374629459906350417591664639e-29,
      -.4570031437748533058179991688533e-30,
      -.1826276602045346104809934028799e-30,
      -.7315349993385250469111066350933e-31,
      -.2936925599971429781637815773866e-31};
  double const am22cs[74]={
      -.156284448062534112753545828583e-1,
      +.778336445239681307018943100334e-2,
      +.867057770477189528406072812110e-3,
      +.156966273156113719469953482266e-3,
      +.356396257143286511324100666302e-4,
      +.924598335425043154495080090994e-5,
      +.262110161850422389523194982066e-5,
      +.791882216516012561489469982263e-6,
      +.251041527921011847803162690862e-6,
      +.826522320665407734472997712940e-7,
      +.280571166281305264396384290014e-7,
      +.976821090484680786674631273890e-8,
      +.347407923227710343287279035573e-8,
      +.125828132169836914219092738164e-8,
      +.462988260641895264497330784625e-9,
      +.172728258813604072468143128696e-9,
      +.652319200131154135148574124970e-10,
      +.249047168520982056019881087112e-10,
      +.960156820553765948078189890126e-11,
      +.373448002067726856974776596757e-11,
      +.146417565032053391722216189678e-11,
      +.578265471168512825475827881553e-12,
      +.229915407244706118560254184494e-12,
      +.919780711231997257150883662365e-13,
      +.370060068813090065807504045556e-13,
      +.149675761698672987823326345205e-13,
      +.608361194938461148720451399443e-14,
      +.248404087115121397635425326873e-14,
      +.101862476526769080727914465339e-14,
      +.419383856352753989429640310957e-15,
      +.173318901762930756149702493501e-15,
      +.718821902388508517820445406811e-16,
      +.299123633598403607712470896113e-16,
      +.124868990433238627855713110880e-16,
      +.522829344609483661928651193632e-17,
      +.219532961724713396595998454359e-17,
      +.924298325229777281154410024332e-18,
      +.390157708236091407825543197309e-18,
      +.165093892693863707213759030367e-18,
      +.700221815715994367565716554487e-19,
      +.297651833616786915573214963506e-19,
      +.126796539086902072571134261229e-19,
      +.541243400697077628687581725061e-20,
      +.231487350218155252296382133283e-20,
      +.991920288386566563462623851167e-21,
      +.425803015323732357158897608174e-21,
      +.183101842973024501678402003088e-21,
      +.788678712311075375564526811022e-22,
      +.340254607386229874956582997235e-22,
      +.147020881405712530791860892535e-22,
      +.636211018324916957733348071767e-23,
      +.275707050680980721919395987768e-23,
      +.119645858090104071356261780457e-23,
      +.519912545729242147981768210567e-24,
      +.226217674847104475260575286850e-24,
      +.985526113754431819448565068283e-25,
      +.429870630332508717223681286187e-25,
      +.187723641661580639829657670189e-25,
      +.820721941772842137268801052115e-26,
      +.359214665604615507812767944463e-26,
      +.157390594612773315611458940587e-26,
      +.690329781039333834965319153586e-27,
      +.303092079078968534607859331415e-27,
      +.133204934160481219185689121944e-27,
      +.585978836851523490117937981442e-28,
      +.258016868489487806338425080457e-28,
      +.113712433637283667223632182863e-28,
      +.501592557226068509236430548549e-29,
      +.221445829395509373322569708484e-29,
      +.978470283886507289984691416411e-30,
      +.432695414934180170112000952983e-30,
      +.191497288193994570612929860440e-30,
      +.848164622402392354171298331562e-31,
      +.375947065173955919947455052934e-31};
  double const ath2cs[72]={
      +.4405273458718778997061127057775e-2,
      -.3042919452318454608483844239873e-1,
      -.1385653283771793791602692842653e-2,
      -.1804443908954952302670486910952e-3,
      -.3380847108327308671057465323618e-4,
      -.7678183535229023055257676817765e-5,
      -.1967839443716035324690935417077e-5,
      -.5483727115877700361586143659281e-6,
      -.1625461550532612452712696212258e-6,
      -.5053049981268895015277637842078e-7,
      -.1631580701124066881183851715617e-7,
      -.5434204112348517507963436694817e-8,
      -.1857398556409900325763850109630e-8,
      -.6489512033326108816213513640676e-9,
      -.2310594885800944720482995987079e-9,
      -.8363282183204411682819329546745e-10,
      -.3071196844890191462660661303891e-10,
      -.1142367142432716819409514579892e-10,
      -.4298116066345803065822470108971e-11,
      -.1633898699596715440601646086632e-11,
      -.6269328620016619432123443754076e-12,
      -.2426052694816257357356159203991e-12,
      -.9461198321624039090742527765052e-13,
      -.3716060313411504806847798281269e-13,
      -.1469155684097526763170138810309e-13,
      -.5843694726140911944556401363094e-14,
      -.2337502595591951298832675034934e-14,
      -.9399231371171435401160167358411e-15,
      -.3798014669372894500076335263715e-15,
      -.1541731043984972524883443681775e-15,
      -.6285287079535307162925662365202e-16,
      -.2572731812811455424755383992774e-16,
      -.1057098119354017809340974866555e-16,
      -.4359080267402696966695992699964e-17,
      -.1803634315959978013953176945540e-17,
      -.7486838064380536821719431676914e-18,
      -.3117261367347604656799597209985e-18,
      -.1301687980927700734792871620696e-18,
      -.5450527587519522468973883909909e-19,
      -.2288293490114231872268635931903e-19,
      -.9631059503829538655655060440088e-20,
      -.4063281001524614089092195416434e-20,
      -.1718203980908026763900413858510e-20,
      -.7281574619892536367415322473328e-21,
      -.3092352652680643127960680345790e-21,
      -.1315917855965440490383417023254e-21,
      -.5610606786087055512664907412668e-22,
      -.2396621894086355206020304337895e-22,
      -.1025574332390581200832954423924e-22,
      -.4396264138143656476403607323663e-23,
      -.1887652998372577373342508719450e-23,
      -.8118140359576807603579433230445e-24,
      -.3496734274366286856375952089214e-24,
      -.1508402925156873215171751475867e-24,
      -.6516268284778671059787773834341e-25,
      -.2818945797529207424505942114583e-25,
      -.1221127596512262744598094464505e-25,
      -.5296674341169867168620011705073e-26,
      -.2300359270773673431358870971744e-26,
      -.1000279482355367494781220348930e-26,
      -.4354760404180879394806893162179e-27,
      -.1898056134741477522515482827030e-27,
      -.8282111868712974697554009309315e-28,
      -.3617815493066569006586213484374e-28,
      -.1582018896178003654858941843636e-28,
      -.6925068597802270011772820383247e-29,
      -.3034390239778629128908629727335e-29,
      -.1330889568166725224761977446509e-29,
      -.5842848522173090120487606971706e-30,
      -.2567488423238302631121274357678e-30,
      -.1129232322268882185791505819151e-30,
      -.4970947029753336916550570105023e-31};
  //
  double const eta = 0.1*std::numeric_limits<double>::epsilon();
  int nam20, nath0, nam21, nath1, nam22, nath2;
  try
  {
    nam20 = Initds(am20cs,57,eta);
    nath0 = Initds(ath0cs,53,eta);
    nam21 = Initds(am21cs,60,eta);
    nath1 = Initds(ath1cs,58,eta);
    nam22 = Initds(am22cs,74,eta);
    nath2 = Initds(ath2cs,72,eta);
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  double xsml = -1.0/pow(std::numeric_limits<double>::epsilon(),0.3333);
  if( x >= -4.0 )
  {
    if( x >= -2.0 )
    {
      if( x >= -1.0 )
        throw EExpressionError(wxT("AiryModulusPhase: x must be <= -1.0"));
      double z = (16.0/pow(x,3.0)+9.0)/7.0;
      try
      {
        ampl = 0.3125 + NTermChebyshevSeries( z, am22cs, nam22 );
        theta = -0.625 + NTermChebyshevSeries( z, ath2cs, nath2 );
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
    else
    {
      double z = (128.0/pow(x,3.0)+9.0)/7.0;
      try
      {
        ampl = 0.3125 + NTermChebyshevSeries( z, am21cs, nam21 );
        theta = -0.625 + NTermChebyshevSeries( z, ath1cs, nath1 );
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
  }
  else
  {
    double z = 1.0;
    if( x > xsml )z = 128.0/pow(x, 3.0) + 1.0;
    try
    {
      ampl = 0.3125 + NTermChebyshevSeries( z, am20cs, nam20 );
      theta = -0.625 + NTermChebyshevSeries( z, ath0cs, nath0 );
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  double sqrtx = sqrt(-x);
  ampl = sqrt(ampl/sqrtx);
  theta = pi4-x*sqrtx*theta;
}

double AiryNegative( double x )
{
  // Refrence: Slatec library, category b5i,b5l
  // DAIE
  // Calculates the Airy function for negative x and an exponentially
  // scaled Airy function for positive x, i.e.,
  // evaluate AI(x) for x < 0.0 and AI(x)*exp(zeta) where
  // zeta = 2/3*pow(x,3/2)  for x >= 0.0
  //
  double const aifcs[13]={
      -.37971358496669997496197089469414e-1,
      +.59191888537263638574319728013777e-1,
      +.98629280577279975365603891044060e-3,
      +.68488438190765667554854830182412e-5,
      +.25942025962194713019489279081403e-7,
      +.61766127740813750329445749697236e-10,
      +.10092454172466117901429556224601e-12,
      +.12014792511179938141288033225333e-15,
      +.10882945588716991878525295466666e-18,
      +.77513772196684887039238400000000e-22,
      +.44548112037175638391466666666666e-25,
      +.21092845231692343466666666666666e-28,
      +.83701735910741333333333333333333e-32};
  double const aigcs[13]={
      +.18152365581161273011556209957864e-1,
      +.21572563166010755534030638819968e-1,
      +.25678356987483249659052428090133e-3,
      +.14265214119792403898829496921721e-5,
      +.45721149200180426070434097558191e-8,
      +.95251708435647098607392278840592e-11,
      +.13925634605771399051150420686190e-13,
      +.15070999142762379592306991138666e-16,
      +.12559148312567778822703205333333e-19,
      +.83063073770821340343829333333333e-23,
      +.44657538493718567445333333333333e-26,
      +.19900855034518869333333333333333e-29,
      +.74702885256533333333333333333333e-33};
  double const aip1cs[57]={
      -.2146951858910538455460863467778e-1,
      -.7535382535043301166219720865565e-2,
      +.5971527949026380852035388881994e-3,
      -.7283251254207610648502368291548e-4,
      +.1110297130739299666517381821140e-4,
      -.1950386152284405710346930314033e-5,
      +.3786973885159515193885319670057e-6,
      -.7929675297350978279039072879154e-7,
      +.1762247638674256075568420122202e-7,
      -.4110767539667195045029896593893e-8,
      +.9984770057857892247183414107544e-9,
      -.2510093251387122211349867730034e-9,
      +.6500501929860695409272038601725e-10,
      -.1727818405393616515478877107366e-10,
      +.4699378842824512578362292872307e-11,
      -.1304675656297743914491241246272e-11,
      +.3689698478462678810473948382282e-12,
      -.1061087206646806173650359679035e-12,
      +.3098414384878187438660210070110e-13,
      -.9174908079824139307833423547851e-14,
      +.2752049140347210895693579062271e-14,
      -.8353750115922046558091393301880e-15,
      +.2563931129357934947568636168612e-15,
      -.7950633762598854983273747289822e-16,
      +.2489283634603069977437281175644e-16,
      -.7864326933928735569664626221296e-17,
      +.2505687311439975672324470645019e-17,
      -.8047420364163909524537958682241e-18,
      +.2604097118952053964443401104392e-18,
      -.8486954164056412259482488834184e-19,
      +.2784706882142337843359429186027e-19,
      -.9195858953498612913687224151354e-20,
      +.3055304318374238742247668225583e-20,
      -.1021035455479477875902177048439e-20,
      +.3431118190743757844000555680836e-21,
      -.1159129341797749513376922463109e-21,
      +.3935772844200255610836268229154e-22,
      -.1342880980296717611956718989038e-22,
      +.4603287883520002741659190305314e-23,
      -.1585043927004064227810772499387e-23,
      +.5481275667729675908925523755008e-24,
      -.1903349371855047259064017948945e-24,
      +.6635682302374008716777612115968e-25,
      -.2322311650026314307975200986453e-25,
      +.8157640113429179313142743695359e-26,
      -.2875824240632900490057489929557e-26,
      +.1017329450942901435079714319018e-26,
      -.3610879108742216446575703490559e-27,
      +.1285788540363993421256640342698e-27,
      -.4592901037378547425160693022719e-28,
      +.1645597033820713725812102485333e-28,
      -.5913421299843501842087920271360e-29,
      +.2131057006604993303479369509546e-29,
      -.7701158157787598216982761745066e-30,
      +.2790533307968930417581783777280e-30,
      -.1013807715111284006452241367039e-30,
      +.3692580158719624093658286216533e-31};
  double const aip2cs[37]={
      -.174314496929375513390355844011e-2,
      -.167893854325541671632190613480e-2,
      +.359653403352166035885983858114e-4,
      -.138081860273922835457399383100e-5,
      +.741122807731505298848699095233e-7,
      -.500238203900133013130422866325e-8,
      +.400693917417184240675446866355e-9,
      -.367331242795905044199318496207e-10,
      +.376034439592373852439592002918e-11,
      -.422321332718747538026564938968e-12,
      +.513509454033657070919618754120e-13,
      -.669095850390477595651681356676e-14,
      +.926667545641290648239550724382e-15,
      -.135514382416070576333397356591e-15,
      +.208115496312830995299006549335e-16,
      -.334116499159176856871277570256e-17,
      +.558578584585924316868032946585e-18,
      -.969219040152365247518658209109e-19,
      +.174045700128893206465696557738e-19,
      -.322640979731130400247846333098e-20,
      +.616074471106625258533259618986e-21,
      -.120936347982490059076420676266e-21,
      +.243632763310138108261570095786e-22,
      -.502914221497457468943403144533e-23,
      +.106224175543635689495470626133e-23,
      -.229284284895989241509856324266e-24,
      +.505181733929503744986884778666e-25,
      -.113498123714412404979793920000e-25,
      +.259765565985606980698374144000e-26,
      -.605124621542939506172231679999e-27,
      +.143359777966772800720295253333e-27,
      -.345147757060899986280721066666e-28,
      +.843875190213646740427025066666e-29,
      -.209396142298188169434453333333e-29,
      +.527008873478945503182848000000e-30,
      -.134457433014553385789030399999e-30,
      +.347570964526601147340117333333e-31};
  double const eta = 0.1*std::numeric_limits<double>::epsilon();
  int naif, naig, naip1, naip2;
  try
  {
    naif = Initds(aifcs,13,eta);
    naig = Initds(aigcs,13,eta);
    naip1 = Initds(aip1cs,57,eta);
    naip2 = Initds(aip2cs,37,eta);
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  //
  double x3sml = pow(eta,0.3333);
  double x32sml = 1.3104*pow(x3sml,2.0);
  double xbig = pow(std::numeric_limits<double>::max(), 0.6666);
  double ans;
  //
  if( x >= -1.0 )
  {
    if( x > 1.0 )
    {
      double const sqrtx = sqrt(x);
      if( x > 4.0 )
      {
        double z = -1.0;
        if( x < xbig )z = 16.0/(x*sqrtx)-1.0;
        try
        {
          ans = (0.28125+NTermChebyshevSeries(z,aip2cs,naip2))/sqrt(sqrtx);
        }
        catch (EExpressionError &e)
        {
          throw;
        }
      }
      else
      {
        double const z = (16.0/(x*sqrtx)-9.0)/7.0;
        try
        {
          ans = (0.28125+NTermChebyshevSeries(z,aip1cs,naip1))/sqrt(sqrtx);
        }
        catch (EExpressionError &e)
        {
          throw;
        }
      }
    }
    else
    {
      double z = 0.0;
      if( fabs(x) > x3sml )z = pow(x,3.0);
      try
      {
        ans = 0.375 + (NTermChebyshevSeries(z,aifcs,naif)-
                       x*(0.25+NTermChebyshevSeries(z,aigcs,naig)));
      }
      catch (EExpressionError &e)
      {
        throw;
      }
      if( x > x32sml )ans *= exp(2.0*x*sqrt(x)/3.0);
    }
  }
  else
  {
    double xm, theta;
    try
    {
      AiryModulusPhase( x, xm, theta );
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    ans = xm*cos(theta);
  }
  return ans;
}

double BesselI0( double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5J
  // COMPUTES THE HYPERBOLIC BESSEL FUNCTION OF THE FIRST KIND OF ORDER ZERO
  // besi0
  //
  //   SERIES FOR BI0    ON THE INTERVAL  0.          TO  9.00000E+00
  //                                WITH WEIGHTED ERROR   9.51E-34
  //                                 LOG WEIGHTED ERROR  33.02
  //                       SIGNIFICANT FIGURES REQUIRED  33.31
  //                            DECIMAL PLACES REQUIRED  33.65
  //     ARGUMENT: overflow @ X ~> 88.0296
  //
  double const xmax = std::numeric_limits<double>::max();
  double const eps = std::numeric_limits<double>::epsilon();
  double answer;
  double const y = fabs(x);
  if( y > 3.0 )
  {
    if( y > log(xmax) )throw EExpressionError( wxT("BesselI0: argument too large") );
    try
    {
      answer = exp(y)*EBesselI0(x);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  else
  {
    answer = 1.0;
    double const b[18] =
    { -.7660547252839144951081894976243285e-1 , +.1927337953993808269952408750881196e+1 ,
      +.2282644586920301338937029292330415e+0 , +.1304891466707290428079334210691888e-1 ,
      +.4344270900816487451378682681026107e-3 , +.9422657686001934663923171744118766e-5 ,
      +.1434006289510691079962091878179957e-6 , +.1613849069661749069915419719994611e-8 ,
      +.1396650044535669699495092708142522e-10, +.9579451725505445344627523171893333e-13,
      +.5333981859862502131015107744000000e-15, +.2458716088437470774696785919999999e-17,
      +.9535680890248770026944341333333333e-20, +.3154382039721427336789333333333333e-22,
      +.9004564101094637431466666666666666e-25, +.2240647369123670016000000000000000e-27,
      +.4903034603242837333333333333333333e-30, +.9508172606122666666666666666666666e-33 };
    try
    {
      int const n = Initds(b,18,0.1*eps);
      if( y > sqrt(8.0*eps) )answer = 2.75+NTermChebyshevSeries(y*y/4.5-1.0,b,n);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  return answer;
}

double EBesselI0( double x )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5J
  //   COMPUTES THE  EXPONENTIALLY SCALED HYPERBOLIC BESSEL FUNCTION
  //      OF THE FIRST KIND OF ORDER ZERO.
  //      EBESI0(X) = EXP(-X) * BESI0(X)
  // ebesio
  //
  //   SERIES FOR BI0     ON THE INTERVAL  0.          TO  9.00000E+00
  //                                 WITH WEIGHTED ERROR   9.51E-34
  //                                  LOG WEIGHTED ERROR  33.02
  //                        SIGNIFICANT FIGURES REQUIRED  33.31
  //                             DECIMAL PLACES REQUIRED  33.65
  //
  //   SERIES FOR AI0     ON THE INTERVAL  1.25000E-01 TO  3.33333E-01
  //                                 WITH WEIGHTED ERROR   2.74E-32
  //                                  LOG WEIGHTED ERROR  31.56
  //                        SIGNIFICANT FIGURES REQUIRED  30.15
  //                             DECIMAL PLACES REQUIRED  32.39
  //
  //   SERIES FOR AI02    ON THE INTERVAL  0.          TO  1.25000E-01
  //                                 WITH WEIGHTED ERROR   1.97E-32
  //                                  LOG WEIGHTED ERROR  31.71
  //                        SIGNIFICANT FIGURES REQUIRED  30.15
  //                             DECIMAL PLACES REQUIRED  32.63
  //
  double const eps = std::numeric_limits<double>::epsilon();
  double const y = fabs(x);
  double answer;
  if( y > 3.0 )
  {
    if( y <= 8.0 )
    {
      double const a[46] =
      { +.7575994494023795942729872037438e-1, +.7591380810823345507292978733204e-2,
        +.4153131338923750501863197491382e-3, +.1070076463439073073582429702170e-4,
        -.7901179979212894660750319485730e-5, -.7826143501438752269788989806909e-6,
        +.2783849942948870806381185389857e-6, +.8252472600612027191966829133198e-8,
        -.1204463945520199179054960891103e-7, +.1559648598506076443612287527928e-8,
        +.2292556367103316543477254802857e-9, -.1191622884279064603677774234478e-9,
        +.1757854916032409830218331247743e-10, +.1128224463218900517144411356824e-11,
        -.1146848625927298877729633876982e-11, +.2715592054803662872643651921606e-12,
        -.2415874666562687838442475720281e-13, -.6084469888255125064606099639224e-14,
        +.3145705077175477293708360267303e-14, -.7172212924871187717962175059176e-15,
        +.7874493403454103396083909603327e-16, +.1004802753009462402345244571839e-16,
        -.7566895365350534853428435888810e-17, +.2150380106876119887812051287845e-17,
        -.3754858341830874429151584452608e-18, +.2354065842226992576900757105322e-19,
        +.1114667612047928530226373355110e-19, -.5398891884396990378696779322709e-20,
        +.1439598792240752677042858404522e-20, -.2591916360111093406460818401962e-21,
        +.2238133183998583907434092298240e-22, +.5250672575364771172772216831999e-23,
        -.3249904138533230784173432285866e-23, +.9924214103205037927857284710400e-24,
        -.2164992254244669523146554299733e-24, +.3233609471943594083973332991999e-25,
        -.1184620207396742489824733866666e-26, -.1281671853950498650548338687999e-26,
        +.5827015182279390511605568853333e-27, -.1668222326026109719364501503999e-27,
        +.3625309510541569975700684800000e-28, -.5733627999055713589945958399999e-29,
        +.3736796722063098229642581333333e-30, +.1602073983156851963365512533333e-30,
        -.8700424864057229884522495999999e-31, +.2741320937937481145603413333333e-31 };
      try
      {
        int const n = Initds(a,46,0.1*eps);
        answer = (0.375+NTermChebyshevSeries((48.0/y-11.0)/5.0,a,n))/sqrt(y);
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
    else  // y > 8.0
    {
      double const a[69] =
      { +.5449041101410883160789609622680e-1, +.3369116478255694089897856629799e-2,
        +.6889758346916823984262639143011e-4, +.2891370520834756482966924023232e-5,
        +.2048918589469063741827605340931e-6, +.2266668990498178064593277431361e-7,
        +.3396232025708386345150843969523e-8, +.4940602388224969589104824497835e-9,
        +.1188914710784643834240845251963e-10, -.3149916527963241364538648629619e-10,
        -.1321581184044771311875407399267e-10, -.1794178531506806117779435740269e-11,
        +.7180124451383666233671064293469e-12, +.3852778382742142701140898017776e-12,
        +.1540086217521409826913258233397e-13, -.4150569347287222086626899720156e-13,
        -.9554846698828307648702144943125e-14, +.3811680669352622420746055355118e-14,
        +.1772560133056526383604932666758e-14, -.3425485619677219134619247903282e-15,
        -.2827623980516583484942055937594e-15, +.3461222867697461093097062508134e-16,
        +.4465621420296759999010420542843e-16, -.4830504485944182071255254037954e-17,
        -.7233180487874753954562272409245e-17, +.9921475412173698598880460939810e-18,
        +.1193650890845982085504399499242e-17, -.2488709837150807235720544916602e-18,
        -.1938426454160905928984697811326e-18, +.6444656697373443868783019493949e-19,
        +.2886051596289224326481713830734e-19, -.1601954907174971807061671562007e-19,
        -.3270815010592314720891935674859e-20, +.3686932283826409181146007239393e-20,
        +.1268297648030950153013595297109e-22, -.7549825019377273907696366644101e-21,
        +.1502133571377835349637127890534e-21, +.1265195883509648534932087992483e-21,
        -.6100998370083680708629408916002e-22, -.1268809629260128264368720959242e-22,
        +.1661016099890741457840384874905e-22, -.1585194335765885579379705048814e-23,
        -.3302645405968217800953817667556e-23, +.1313580902839239781740396231174e-23,
        +.3689040246671156793314256372804e-24, -.4210141910461689149219782472499e-24,
        +.4791954591082865780631714013730e-25, +.8459470390221821795299717074124e-25,
        -.4039800940872832493146079371810e-25, -.6434714653650431347301008504695e-26,
        +.1225743398875665990344647369905e-25, -.2934391316025708923198798211754e-26,
        -.1961311309194982926203712057289e-26, +.1503520374822193424162299003098e-26,
        -.9588720515744826552033863882069e-28, -.3483339380817045486394411085114e-27,
        +.1690903610263043673062449607256e-27, +.1982866538735603043894001157188e-28,
        -.5317498081491816214575830025284e-28, +.1803306629888392946235014503901e-28,
        +.6213093341454893175884053112422e-29, -.7692189292772161863200728066730e-29,
        +.1858252826111702542625560165963e-29, +.1237585142281395724899271545541e-29,
        -.1102259120409223803217794787792e-29, +.1886287118039704490077874479431e-30,
        +.2160196872243658913149031414060e-30, -.1605454124919743200584465949655e-30,
        +.1965352984594290603938848073318e-31 };
      try
      {
        int const n = Initds(a,69,0.1*eps);
        answer = (0.375+NTermChebyshevSeries((16.0/y-1.0),a,n))/sqrt(y);
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
  }
  else // y <= 3.0
  {
    answer = 1.0;
    if( y > sqrt(8.0*eps) )
    {
      double const b[18] =
      { -.7660547252839144951081894976243285e-1, +.1927337953993808269952408750881196e+1,
        +.2282644586920301338937029292330415e+0, +.1304891466707290428079334210691888e-1,
        +.4344270900816487451378682681026107e-3, +.9422657686001934663923171744118766e-5,
        +.1434006289510691079962091878179957e-6, +.1613849069661749069915419719994611e-8,
        +.1396650044535669699495092708142522e-10, +.9579451725505445344627523171893333e-13,
        +.5333981859862502131015107744000000e-15, +.2458716088437470774696785919999999e-17,
        +.9535680890248770026944341333333333e-20, +.3154382039721427336789333333333333e-22,
        +.9004564101094637431466666666666666e-25, +.2240647369123670016000000000000000e-27,
        +.4903034603242837333333333333333333e-30, +.9508172606122666666666666666666666e-33 };
      try
      {
        int const n = Initds(b,18,0.1*eps);
        answer = exp(-y)*(2.75+NTermChebyshevSeries(y*y/4.5-1.0,b,n));
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
  }
  return answer;
}

double BesselI1( double x )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5J
  //   COMPUTES THE MODIFIED (HYPERBOLIC) BESSEL FUNCTION OF THE FIRST
  //     KIND OF ORDER ONE.
  // besi1
  //
  //   SERIES FOR BI1     ON THE INTERVAL  0.          TO  9.00000E+00
  //                                 WITH WEIGHTED ERROR   1.44E-32
  //                                  LOG WEIGHTED ERROR  31.84
  //                        SIGNIFICANT FIGURES REQUIRED  31.45
  //                             DECIMAL PLACES REQUIRED  32.46
  //
  //     ARGUMENT: overflow @ X ~> 88.0296
  //
  if( x == 0.0 )return 0.0;
  double const xmin = std::numeric_limits<double>::min();
  double const xmax = std::numeric_limits<double>::max();
  double const eps = std::numeric_limits<double>::epsilon();
  double const y = fabs(x);
  if( y > 3.0 )
  {
    if( y > log(xmax) )throw EExpressionError( wxT("BesselI1: argument too large") );
    double answer;
    try
    {
      answer = exp(y)*EBesselI1(x);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    return answer;
  }
  if( y < 2.0*xmin )throw EExpressionError( wxT("BesselI1: argument too small") );
  double answer = 0.5*x;
  if( y > sqrt(8.0*eps) )
  {
    double const b[17] =
    { -.19717132610998597316138503218149e-2, +.40734887667546480608155393652014e+0,
      +.34838994299959455866245037783787e-1, +.15453945563001236038598401058489e-2,
      +.41888521098377784129458832004120e-4, +.76490267648362114741959703966069e-6,
      +.10042493924741178689179808037238e-7, +.99322077919238106481371298054863e-10,
      +.76638017918447637275200171681349e-12, +.47414189238167394980388091948160e-14,
      +.24041144040745181799863172032000e-16, +.10171505007093713649121100799999e-18,
      +.36450935657866949458491733333333e-21, +.11205749502562039344810666666666e-23,
      +.29875441934468088832000000000000e-26, +.69732310939194709333333333333333e-29,
      +.14367948220620800000000000000000e-31 };
    try
    {
      int n = Initds(b,17,0.1*eps);
      answer = x*(0.875+NTermChebyshevSeries(y*y/4.5-1.0,b,n));
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  return answer;
}

double EBesselI1( double x )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5K
  //   COMPUTES THE EXPONENTIALLY SCALED MODIFIED (HYPERBOLIC)
  //     BESSEL FUNCTION OF THE FIRST KIND OF ORDER ONE.
  //     EBesselI1(X) = EXP(-X) * BesselI1(X)
  // ebesi1
  //
  //   SERIES FOR BI1     ON THE INTERVAL  0.          TO  9.00000E+00
  //                                 WITH WEIGHTED ERROR   1.44E-32
  //                                  LOG WEIGHTED ERROR  31.84
  //                        SIGNIFICANT FIGURES REQUIRED  31.45
  //                             DECIMAL PLACES REQUIRED  32.46
  //
  //   SERIES FOR AI1     ON THE INTERVAL  1.25000E-01 TO  3.33333E-01
  //                                 WITH WEIGHTED ERROR   2.81E-32
  //                                  LOG WEIGHTED ERROR  31.55
  //                        SIGNIFICANT FIGURES REQUIRED  29.93
  //                             DECIMAL PLACES REQUIRED  32.38
  //
  //   SERIES FOR AI12    ON THE INTERVAL  0.          TO  1.25000E-01
  //                                 WITH WEIGHTED ERROR   1.83E-32
  //                                  LOG WEIGHTED ERROR  31.74
  //                        SIGNIFICANT FIGURES REQUIRED  29.97
  //                             DECIMAL PLACES REQUIRED  32.66
  //
  if( x == 0.0 )return 0.0;
  //
  double const xmin = std::numeric_limits<double>::min();
  double const eps = std::numeric_limits<double>::epsilon();
  double answer;
  double const y = fabs(x);
  if( y > 3.0 )
  {
    if( y <= 8.0 )
    {
      double const a[46] =
      { -.2846744181881478674100372468307e-1  , -.1922953231443220651044448774979e-1  ,
        -.6115185857943788982256249917785e-3  , -.2069971253350227708882823777979e-4  ,
        +.8585619145810725565536944673138e-5  , +.1049498246711590862517453997860e-5  ,
        -.2918338918447902202093432326697e-6  , -.1559378146631739000160680969077e-7  ,
        +.1318012367144944705525302873909e-7  , -.1448423418183078317639134467815e-8  ,
        -.2908512243993142094825040993010e-9  , +.1266388917875382387311159690403e-9  ,
        -.1664947772919220670624178398580e-10 , -.1666653644609432976095937154999e-11 ,
        +.1242602414290768265232168472017e-11 , -.2731549379672432397251461428633e-12 ,
        +.2023947881645803780700262688981e-13 , +.7307950018116883636198698126123e-14 ,
        -.3332905634404674943813778617133e-14 , +.7175346558512953743542254665670e-15 ,
        -.6982530324796256355850629223656e-16 , -.1299944201562760760060446080587e-16 ,
        +.8120942864242798892054678342860e-17 , -.2194016207410736898156266643783e-17 ,
        +.3630516170029654848279860932334e-18 , -.1695139772439104166306866790399e-19 ,
        -.1288184829897907807116882538222e-19 , +.5694428604967052780109991073109e-20 ,
        -.1459597009090480056545509900287e-20 , +.2514546010675717314084691334485e-21 ,
        -.1844758883139124818160400029013e-22 , -.6339760596227948641928609791999e-23 ,
        +.3461441102031011111108146626560e-23 , -.1017062335371393547596541023573e-23 ,
        +.2149877147090431445962500778666e-24 , -.3045252425238676401746206173866e-25 ,
        +.5238082144721285982177634986666e-27 , +.1443583107089382446416789503999e-26 ,
        -.6121302074890042733200670719999e-27 , +.1700011117467818418349189802666e-27 ,
        -.3596589107984244158535215786666e-28 , +.5448178578948418576650513066666e-29 ,
        -.2731831789689084989162564266666e-30 , -.1858905021708600715771903999999e-30 ,
        +.9212682974513933441127765333333e-31 , -.2813835155653561106370833066666e-31 };
      try
      {
        int n = Initds(a,46,0.1*eps);
        answer = (0.375+NTermChebyshevSeries((48.0/y-11)/5.0,a,n))/sqrt(y);
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
    else
    {
      double const a[69] =
      { +.2857623501828012047449845948469e-1   , -.9761097491361468407765164457302e-2   ,
        -.1105889387626237162912569212775e-3   , -.3882564808877690393456544776274e-5   ,
        -.2512236237870208925294520022121e-6   , -.2631468846889519506837052365232e-7   ,
        -.3835380385964237022045006787968e-8   , -.5589743462196583806868112522229e-9   ,
        -.1897495812350541234498925033238e-10  , +.3252603583015488238555080679949e-10  ,
        +.1412580743661378133163366332846e-10  , +.2035628544147089507224526136840e-11  ,
        -.7198551776245908512092589890446e-12  , -.4083551111092197318228499639691e-12  ,
        -.2101541842772664313019845727462e-13  , +.4272440016711951354297788336997e-13  ,
        +.1042027698412880276417414499948e-13  , -.3814403072437007804767072535396e-14  ,
        -.1880354775510782448512734533963e-14  , +.3308202310920928282731903352405e-15  ,
        +.2962628997645950139068546542052e-15  , -.3209525921993423958778373532887e-16  ,
        -.4650305368489358325571282818979e-16  , +.4414348323071707949946113759641e-17  ,
        +.7517296310842104805425458080295e-17  , -.9314178867326883375684847845157e-18  ,
        -.1242193275194890956116784488697e-17  , +.2414276719454848469005153902176e-18  ,
        +.2026944384053285178971922860692e-18  , -.6394267188269097787043919886811e-19  ,
        -.3049812452373095896084884503571e-19  , +.1612841851651480225134622307691e-19  ,
        +.3560913964309925054510270904620e-20  , -.3752017947936439079666828003246e-20  ,
        -.5787037427074799345951982310741e-22  , +.7759997511648161961982369632092e-21  ,
        -.1452790897202233394064459874085e-21  , -.1318225286739036702121922753374e-21  ,
        +.6116654862903070701879991331717e-22  , +.1376279762427126427730243383634e-22  ,
        -.1690837689959347884919839382306e-22  , +.1430596088595433153987201085385e-23  ,
        +.3409557828090594020405367729902e-23  , -.1309457666270760227845738726424e-23  ,
        -.3940706411240257436093521417557e-24  , +.4277137426980876580806166797352e-24  ,
        -.4424634830982606881900283123029e-25  , -.8734113196230714972115309788747e-25  ,
        +.4045401335683533392143404142428e-25  , +.7067100658094689465651607717806e-26  ,
        -.1249463344565105223002864518605e-25  , +.2867392244403437032979483391426e-26  ,
        +.2044292892504292670281779574210e-26  , -.1518636633820462568371346802911e-26  ,
        +.8110181098187575886132279107037e-28  , +.3580379354773586091127173703270e-27  ,
        -.1692929018927902509593057175448e-27  , -.2222902499702427639067758527774e-28  ,
        +.5424535127145969655048600401128e-28  , -.1787068401578018688764912993304e-28  ,
        -.6565479068722814938823929437880e-29  , +.7807013165061145280922067706839e-29  ,
        -.1816595260668979717379333152221e-29  , -.1287704952660084820376875598959e-29  ,
        +.1114548172988164547413709273694e-29  , -.1808343145039336939159368876687e-30  ,
        -.2231677718203771952232448228939e-30  , +.1619029596080341510617909803614e-30  ,
        -.1834079908804941413901308439210e-31 };
      try
      {
        int n = Initds(a,69,0.1*eps);
        answer = (0.375+NTermChebyshevSeries(16.0/y-1.0,a,n))/sqrt(y);
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
    if( x >= 0.0 )answer =  fabs(answer);
    else          answer = -fabs(answer);
  }
  else
  {
    if( y < 2.0*xmin )throw EExpressionError( wxT("EBesselI1: argument too small") );
    answer = 0.5*x;
    if( y > sqrt(8.0*eps) )
    {
      double const b[17] =
      { -.19717132610998597316138503218149e-2  , +.40734887667546480608155393652014e+0  ,
        +.34838994299959455866245037783787e-1  , +.15453945563001236038598401058489e-2  ,
        +.41888521098377784129458832004120e-4  , +.76490267648362114741959703966069e-6  ,
        +.10042493924741178689179808037238e-7  , +.99322077919238106481371298054863e-10 ,
        +.76638017918447637275200171681349e-12 , +.47414189238167394980388091948160e-14 ,
        +.24041144040745181799863172032000e-16 , +.10171505007093713649121100799999e-18 ,
        +.36450935657866949458491733333333e-21 , +.11205749502562039344810666666666e-23 ,
        +.29875441934468088832000000000000e-26 , +.69732310939194709333333333333333e-29 ,
        +.14367948220620800000000000000000e-31 };
      try
      {
        int n = Initds(b,17,0.1*eps);
        answer = x*(0.875+NTermChebyshevSeries(y*y/4.5-1.0,b,n));
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
    answer *= exp(-y);
  }
  return answer;
}

double BesselJ0( double x )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5I
  //   COMPUTES THE BESSEL FUNCTION OF THE FIRST KIND OF ORDER ZERO.
  // besj0
  //
  //  SERIES FOR BJ0      ON THE INTERVAL  0.          TO  1.60000E+01
  //                                 WITH WEIGHTED ERROR   4.39E-32
  //                                  LOG WEIGHTED ERROR  31.36
  //                        SIGNIFICANT FIGURES REQUIRED  31.21
  //                             DECIMAL PLACES REQUIRED  32.00
  //
  //     ARGUMENT: underflow @ X ~> 3.602E+16
  //
  double const eps = std::numeric_limits<double>::epsilon();
  double answer;
  double const y = fabs(x);
  if( y > 4.0 )
  {
    try
    {
      double ampl, theta;
      Bessel0ModulusPhase(y,ampl,theta);
      answer = ampl*cos(theta);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  else
  {
    answer = 1.0;
    if( y > sqrt(4.0*eps) )
    {
      double const b[19] =
      { +.10025416196893913701073127264074e+0 , -.66522300776440513177678757831124e+0 ,
        +.24898370349828131370460468726680e+0 , -.33252723170035769653884341503854e-1 ,
        +.23114179304694015462904924117729e-2 , -.99112774199508092339048519336549e-4 ,
        +.28916708643998808884733903747078e-5 , -.61210858663032635057818407481516e-7 ,
        +.98386507938567841324768748636415e-9 , -.12423551597301765145515897006836e-10,
        +.12654336302559045797915827210363e-12, -.10619456495287244546914817512959e-14,
        +.74706210758024567437098915584000e-17, -.44697032274412780547627007999999e-19,
        +.23024281584337436200523093333333e-21, -.10319144794166698148522666666666e-23,
        +.40608178274873322700800000000000e-26, -.14143836005240913919999999999999e-28,
        +.43910905496698880000000000000000e-31 };
      try
      {
        int n = Initds(b,19,0.1*eps);
        answer = NTermChebyshevSeries(0.125*y*y-1.0,b,n);
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
  }
  return answer;
}

double BesselJ1( double x )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5I
  //   COMPUTES THE BESSEL FUNCTION OF THE FIRST KIND OF ORDER ONE.
  // besj1
  //
  //  SERIES FOR BJ1     ON THE INTERVAL  0.          TO  1.60000E+0
  //                                WITH WEIGHTED ERROR   1.16E-33
  //                                 LOG WEIGHTED ERROR  32.93
  //                       SIGNIFICANT FIGURES REQUIRED  32.36
  //                            DECIMAL PLACES REQUIRED  33.57
  //     ARGUMENT: underflow @ X ~> 3.602E+16
  //
  if( x == 0.0 )return 0.0;
  double const y = fabs(x);
  double answer;
  if( y > 4.0 )
  {
    try
    {
      double ampl, theta;
      Bessel1ModulusPhase(y,ampl,theta);
      answer = ampl*cos(theta);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  else
  {
    double const xmin = std::numeric_limits<double>::min();
    double const eps = std::numeric_limits<double>::epsilon();
    if( y < 2.0*xmin )throw EExpressionError( wxT("BesselJ1: argument too small") );
    answer = 0.0;
    if( y > sqrt(4.0*eps) )
    {
      double const b[19] =
      { -.117261415133327865606240574524003e+0, -.253615218307906395623030884554698e+0,
        +.501270809844695685053656363203743e-1, -.463151480962508191842619728789772e-2,
        +.247996229415914024539124064592364e-3, -.867894868627882584521246435176416e-5,
        +.214293917143793691502766250991292e-6, -.393609307918317979229322764073061e-8,
        +.559118231794688004018248059864032e-10, -.632761640466139302477695274014880e-12,
        +.584099161085724700326945563268266e-14, -.448253381870125819039135059199999e-16,
        +.290538449262502466306018688000000e-18, -.161173219784144165412118186666666e-20,
        +.773947881939274637298346666666666e-23, -.324869378211199841143466666666666e-25,
        +.120223767722741022720000000000000e-27, -.395201221265134933333333333333333e-30,
        +.116167808226645333333333333333333e-32 };
      try
      {
        int const n = Initds(b,19,0.1*eps);
        answer = x*(0.25+NTermChebyshevSeries(0.125*y*y-1.0,b,n));
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
  }
  return answer;
}

void BesselJn( double x, double a, int nnn, int D, double *j )
{
  //     BesselJn CALCULATES THE BESSEL FUNCTIONS J(X) OF ORDER A, A+1,
  //       A+2, ..., A+NNN IN DOUBLE PRECISION
  // besjn
  //
  //     REFERENCE : CERN library routine C330
  //     ARGUMENT TYPE :
  //       X ----- ARGUMENT OF J(X), double, X > 0
  //       A ----- double, 0 <= A < 1; if NNN < 0, A # 0
  //       NNN --- int, -100 <= NNN <= +100
  //       D ----- int, NO. OF SIGNIFICANT FIGURES DESIRED
  //       J ----- double ARRAY, DIMENSION 101, J(X) order A+I is stored
  //               in J(ABS(I)+1)
  //       If any parameter is out of range, error message results
  //     EXTERNAL ROUTINE REFERENCED: GAMMA (C322)
  //     ACCURACY : WITH A=0,NNN=20,D=10; X=1,5,10,50,100
  //       DBesselJn is at least as accurate as specified (i.e. 10 sig. fig. )
  //
  //for C++ zero referencing arrays j(x) of order a+i is stored in j[abs(i)]
  //
  double l, m1, n1, r1, s1, rr[101], j1[101], lamda;
  double p, q, y, t1;
  int m, n, lim, nu, nm;
  int exit_flag=0;
  //
  if( a<0.0 || a>=1.0 || x<=0.0 || abs(nnn) > 100 )
    throw EExpressionError( wxT("BesselJn: unreasonable argument or order") );
  int nmax = nnn;
  bool ll = (nmax >= 0);
  if( !ll )
  {
    if( a == 0.0 )
      throw EExpressionError( wxT("BesselJn: unreasonable argument or order") );
    nm = -nmax;
    nmax = 1;
  }
  double const eps = 0.5*pow(10.0,static_cast<double>(-D));
  double D1 = 2.3026*static_cast<double>(D)+1.3863;
  double sum;
  try
  {
    sum = pow((x/2.0),a)/CompleteGamma(1.0+a);
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  int kk = 1;
  double t2 = 0.0;
  double x1 = x;
  int nma = nmax+1;
  for( int i=0; i<nma; ++i )j1[i] = 0.0;
  if( nmax == 0 )
  {
    t1 = t2*static_cast<double>(nmax);
    y = (D1*0.73576)/x1;
    ++kk;
  }
  else y = D1*0.5/static_cast<double>(nmax);
  while( 1==1 )
  {
    if( y > 10 )
    {
      q = log(y)-0.775;
      p = (0.775-log(q))/(1.0+q);
      p = 1.0/(1.0+p);
      t2 = y*p/q;
    }
    else
    {
      p = y*5.7941e-5-1.76148e-3;
      p = y*p+0.0208645;
      p = y*p-0.129013;
      p = y*p+0.857770;
      t2 = y*p+1.01250;
    }
    if( (kk-2) >= 0 )break;
    t1 = t2*static_cast<double>(nmax);
    y = D1*0.73576/x1;
    ++kk;
  }
  t2 = 1.3591*x1*t2;
  (t1-t2) <= 0 ? nu = static_cast<int>(1+t2) : nu = static_cast<int>(1+t1);
  while( 1==1 )
  {
    l = 1.0;
    m = 0;
    lim = nu/2;
    do
    {
      ++m;
      m1 = static_cast<double>(m);
      l *= (m1+a)/(m1+1.0);
    } while( (m-lim) < 0 );
    n = m+m;
    r1 = 0.0;
    s1 = 0.0;
    do
    {
      n1 = static_cast<double>(n);
      r1 = 1.0/(2.0*(a+n1)/x-r1);
      lamda = 0.0;
      if( (n/2)*2 == n )
      {
        l *= (n1+2.0)/(n1+2.0*a);
        lamda = l*(n1+a);
      }
      s1 = r1*(lamda+s1);
      if( n <= nmax )rr[n-1] = r1;
      --n;
    } while( n >= 1 );
    j[0] = sum/(1.0+s1);
    if( nmax != 0 )
    {
      for( int i=0; i<nmax; ++i )j[i+1] = rr[i]*j[i];
    }
    for( int i=0; i<nma; ++i )
    {
      if( fabs((j[i]-j1[i])/j[i]) > eps )break;
      if( i == (nma-1))exit_flag = 1;
    }
    if( exit_flag == 1 )break;
    nu += 5;
    for( int i=0; i<nma; ++i )j1[i] = j[i];
  }
  if( !ll )
  {
    j[1] = 2.0*a*j[0]/x-j[1];
    for( int i=1; i<nm; ++i )j[i+1] = 2.0*(a-i+1.0)*j[i]/x-j[i-1];
  }
  return;
}

double BesselK0( double x )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5J
  //   COMPUTES THE MODIFIED (HYPERBOLIC) BESSEL FUNCTION OF THE THIRD
  //      KIND OF ORDER ZERO, WHERE X>0
  // besk0
  //
  // SERIES FOR BK0     ON THE INTERVAL  0.          TO  4.00000E+00
  //                                 WITH WEIGHTED ERROR   3.08E-33
  //                                  LOG WEIGHTED ERROR  32.51
  //                        SIGNIFICANT FIGURES REQUIRED  32.05
  //                             DECIMAL PLACES REQUIRED  33.11
  //
  //     ARGUMENT: overflow  @ X ~< 0.588E-38
  //               underflow @ X ~> 86.492
  //
  if( x <= 0 )throw EExpressionError( wxT("BesselK0: argument must be > 0") );
  double const xmin = std::numeric_limits<double>::min();
  double const eps = std::numeric_limits<double>::epsilon();
  double answer;
  if( x > 2.0 )
  {
    double t = -log(xmin);
    if( x > t-0.5*t*log(t)/(t+0.5) )
      throw EExpressionError( wxT("BesselK0: argument is too large") );
    try
    {
      answer = exp(-x)*EBesselK0(x);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  else
  {
    double y = 0.0;
    if( x > sqrt(4*eps) )y = x*x;
    double const b[16] =
    { -.353273932339027687201140060063153e-1, +.344289899924628486886344927529213e+0,
      +.359799365153615016265721303687231e-1, +.126461541144692592338479508673447e-2,
      +.228621210311945178608269830297585e-4, +.253479107902614945730790013428354e-6,
      +.190451637722020885897214059381366e-8, +.103496952576336245851008317853089e-10,
      +.425981614279108257652445327170133e-13, +.137446543588075089694238325440000e-15,
      +.357089652850837359099688597333333e-18, +.763164366011643737667498666666666e-21,
      +.136542498844078185908053333333333e-23, +.207527526690666808319999999999999e-26,
      +.271281421807298560000000000000000e-29, +.308259388791466666666666666666666e-32 };
    try
    {
      int const n = Initds(b,16,0.1*eps);
      answer = -log(0.5*x)*BesselI0(x)-0.25+NTermChebyshevSeries(0.5*y-1.0,b,n);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  return answer;
}

double EBesselK0( double x )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5J
  //  EBesselK0 COMPUTES THE EXPONENTIALLY SCALED MODIFIED (HYPERBOLIC) BESSEL
  //     FUNCTION OF THIRD KIND OF ORDER 0 AT X, WHERE X > 0.
  //     EBesselK0(X) = EXP(X) * BesselK0(X)
  // ebesk0
  //
  //  SERIES FOR BK0      ON THE INTERVAL  0.          TO  4.00000E+00
  //                                 WITH WEIGHTED ERROR   3.08E-33
  //                                  LOG WEIGHTED ERROR  32.51
  //                        SIGNIFICANT FIGURES REQUIRED  32.05
  //                             DECIMAL PLACES REQUIRED  33.11
  //
  //  SERIES FOR AK0      ON THE INTERVAL  1.25000E-01 TO  5.00000E-01
  //                                 WITH WEIGHTED ERROR   2.85E-32
  //                                   LOG WEIGHTED ERROR  31.54
  //                         SIGNIFICANT FIGURES REQUIRED  30.19
  //                              DECIMAL PLACES REQUIRED  32.33
  //
  //  SERIES FOR AK02     ON THE INTERVAL  0.          TO  1.25000E-01
  //                                 WITH WEIGHTED ERROR   2.30E-32
  //                                  LOG WEIGHTED ERROR  31.64
  //                        SIGNIFICANT FIGURES REQUIRED  29.68
  //                             DECIMAL PLACES REQUIRED  32.40
  //
  if( x <= 0.0 )throw EExpressionError( wxT("EBesselK0: argument must be > 0") );
  double const eps = std::numeric_limits<double>::epsilon();
  double answer;
  if( x >= 2.0 )
  {
    if( x <= 8.0 )
    {
      double const a[38] =
      { -.7643947903327941424082978270088e-1, -.2235652605699819052023095550791e-1,
        +.7734181154693858235300618174047e-3, -.4281006688886099464452146435416e-4,
        +.3081700173862974743650014826660e-5, -.2639367222009664974067448892723e-6,
        +.2563713036403469206294088265742e-7, -.2742705549900201263857211915244e-8,
        +.3169429658097499592080832873403e-9, -.3902353286962184141601065717962e-10,
        +.5068040698188575402050092127286e-11, -.6889574741007870679541713557984e-12,
        +.9744978497825917691388201336831e-13, -.1427332841884548505389855340122e-13,
        +.2156412571021463039558062976527e-14, -.3349654255149562772188782058530e-15,
        +.5335260216952911692145280392601e-16, -.8693669980890753807639622378837e-17,
        +.1446404347862212227887763442346e-17, -.2452889825500129682404678751573e-18,
        +.4233754526232171572821706342400e-19, -.7427946526454464195695341294933e-20,
        +.1323150529392666866277967462400e-20, -.2390587164739649451335981465599e-21,
        +.4376827585923226140165712554666e-22, -.8113700607345118059339011413333e-23,
        +.1521819913832172958310378154666e-23, -.2886041941483397770235958613333e-24,
        +.5530620667054717979992610133333e-25, -.1070377329249898728591633066666e-25,
        +.2091086893142384300296328533333e-26, -.4121713723646203827410261333333e-27,
        +.8193483971121307640135680000000e-28, -.1642000275459297726780757333333e-28,
        +.3316143281480227195890346666666e-29, -.6746863644145295941085866666666e-30,
        +.1382429146318424677635413333333e-30, -.2851874167359832570811733333333e-31 };
      try
      {
        int const n = Initds(a,38,0.1*eps);
        answer = (1.25+NTermChebyshevSeries((16.0/x-5)/3.0,a,n))/sqrt(x);
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
    else
    {
      double const a[33] =
      { -.1201869826307592239839346212452e-1, -.9174852691025695310652561075713e-2   ,
        +.1444550931775005821048843878057e-3, -.4013614175435709728671021077879e-5   ,
        +.1567831810852310672590348990333e-6, -.7770110438521737710315799754460e-8   ,
        +.4611182576179717882533130529586e-9, -.3158592997860565770526665803309e-10  ,
        +.2435018039365041127835887814329e-11, -.2074331387398347897709853373506e-12  ,
        +.1925787280589917084742736504693e-13, -.1927554805838956103600347182218e-14  ,
        +.2062198029197818278285237869644e-15, -.2341685117579242402603640195071e-16  ,
        +.2805902810643042246815178828458e-17, -.3530507631161807945815482463573e-18  ,
        +.4645295422935108267424216337066e-19, -.6368625941344266473922053461333e-20  ,
        +.9069521310986515567622348800000e-21, -.1337974785423690739845005311999e-21  ,
        +.2039836021859952315522088960000e-22, -.3207027481367840500060869973333e-23  ,
        +.5189744413662309963626359466666e-24, -.8629501497540572192964607999999e-25  ,
        +.1472161183102559855208038400000e-25, -.2573069023867011283812351999999e-26  ,
        +.4601774086643516587376640000000e-27, -.8411555324201093737130666666666e-28  ,
        +.1569806306635368939301546666666e-28, -.2988226453005757788979199999999e-29  ,
        +.5796831375216836520618666666666e-30, -.1145035994347681332155733333333e-30  ,
        +.2301266594249682802005333333333e-31 };
      try
      {
        int const n = Initds(a,33,0.1*eps);
        answer = (1.25+NTermChebyshevSeries(16.0/x-1.0,a,n))/sqrt(x);
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
  }
  else
  {
    double y = 0.0;
    if( x > sqrt(4.0*eps) )y = x*x;
    double const b[16] =
    { -.353273932339027687201140060063153e-1, +.344289899924628486886344927529213e+0 ,
      +.359799365153615016265721303687231e-1 , +.126461541144692592338479508673447e-2 ,
      +.228621210311945178608269830297585e-4 , +.253479107902614945730790013428354e-6 ,
      +.190451637722020885897214059381366e-8 , +.103496952576336245851008317853089e-10,
      +.425981614279108257652445327170133e-13, +.137446543588075089694238325440000e-15,
      +.357089652850837359099688597333333e-18, +.763164366011643737667498666666666e-21,
      +.136542498844078185908053333333333e-23, +.207527526690666808319999999999999e-26,
      +.271281421807298560000000000000000e-29, +.308259388791466666666666666666666e-32 };
    try
    {
      int const n = Initds(b,16,0.1*eps);
      answer = (exp(x)*(-log(0.5*x)*BesselI0(x)-0.25+NTermChebyshevSeries(0.5*y-1.0,b,n)));
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  return answer;
}

double BesselK1( double x )
{
  //REFERENCE: SLATEC LIBRARY; CATEGORY B5J
  //  BesselK1 COMPUTES THE MODIFIED (HYPERBOLIC) BESSEL FUNCTION OF THIRD
  //    KIND OF ORDER ONE AT X, WHERE X .GT. 0.
  // besk1
  //
  //   SERIES FOR BK1     ON THE INTERVAL  0.          TO  4.00000E+00
  //                                 WITH WEIGHTED ERROR   9.16E-32
  //                                  LOG WEIGHTED ERROR  31.04
  //                        SIGNIFICANT FIGURES REQUIRED  30.61
  //                             DECIMAL PLACES REQUIRED  31.64
  //
  //     ARGUMENT:  overflow @ X ~< 0.594E-38
  //               underflow @ X ~> 86.492
  //
  if( x <= 0.0 )throw EExpressionError( wxT("BesselK1: argument must be > 0") );
  double const xmin = std::numeric_limits<double>::min();
  double const xmax = std::numeric_limits<double>::max();
  double const eps = std::numeric_limits<double>::epsilon();
  double answer;
  if( x > 2.0 )
  {
    double t = -log(xmin);
    if( x > t-0.5*t*log(t)/(t+0.5) )throw EExpressionError( wxT("BesselK1: argument too large") );
    try
    {
      answer = exp(-x)*EBesselK1(x);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  else
  {
    if( x < exp(std::max(log(xmin),-log(xmax))+0.01) )
      throw EExpressionError( wxT("BesselK1: argument too small") );
    double y = 0.0;
    if( x > sqrt(4.0*eps) )y = x*x;
    double const b[16] =
    { +.25300227338947770532531120868533e-1 , -.35315596077654487566723831691801e+0 ,
      -.12261118082265714823479067930042e+0 , -.69757238596398643501812920296083e-2 ,
      -.17302889575130520630176507368979e-3 , -.24334061415659682349600735030164e-5 ,
      -.22133876307347258558315252545126e-7 , -.14114883926335277610958330212608e-9 ,
      -.66669016941993290060853751264373e-12, -.24274498505193659339263196864853e-14,
      -.70238634793862875971783797120000e-17, -.16543275155100994675491029333333e-19,
      -.32338347459944491991893333333333e-22, -.53312750529265274999466666666666e-25,
      -.75130407162157226666666666666666e-28, -.91550857176541866666666666666666e-31 };
    try
    {
      int n = Initds(b,16,0.1*eps);
      answer = log(0.5*x)*BesselI1(x)+(0.75+NTermChebyshevSeries(0.5*y-1.0,b,n))/x;
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  return answer;
}

double EBesselK1( double x )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5J
  //  EBesselK1 COMPUTES THE EXPONENTIALLY SCALED MODIFIED (HYPERBOLIC) BESSEL
  //    FUNCTION OF THIRD KIND OF ORDER ONE AT X, WHERE X .GT. 0.
  //    EBesselK1(X) = EXP(X) * BesselK1(X)
  // ebesk1
  //
  //   SERIES FOR BK1      ON THE INTERVAL  0.          TO  4.00000E+00
  //                                  WITH WEIGHTED ERROR   9.16E-32
  //                                   LOG WEIGHTED ERROR  31.04
  //                         SIGNIFICANT FIGURES REQUIRED  30.61
  //                              DECIMAL PLACES REQUIRED  31.64
  //
  //   SERIES FOR AK1      ON THE INTERVAL  1.25000E-01 TO  5.00000E-01
  //                                  WITH WEIGHTED ERROR   3.07E-32
  //                                   LOG WEIGHTED ERROR  31.51
  //                         SIGNIFICANT FIGURES REQUIRED  30.71
  //                              DECIMAL PLACES REQUIRED  32.30
  //
  //   SERIES FOR AK12     ON THE INTERVAL  0.          TO  1.25000E-01
  //                                  WITH WEIGHTED ERROR   2.41E-32
  //                                   LOG WEIGHTED ERROR  31.62
  //                         SIGNIFICANT FIGURES REQUIRED  30.25
  //                              DECIMAL PLACES REQUIRED  32.38
  //
  if( x <= 0.0 )
    throw EExpressionError( wxT("EBesselK1: argument must be > 0") );
  double const xmin = std::numeric_limits<double>::min();
  double const xmax = std::numeric_limits<double>::max();
  double const eps = std::numeric_limits<double>::epsilon();
  double answer;
  if( x > 2.0 )
  {
    if( x <= 8.0 )
    {
      double const a[38] =
      { +.27443134069738829695257666227266e+0  , +.75719899531993678170892378149290e-1  ,
        -.14410515564754061229853116175625e-2  , +.66501169551257479394251385477036e-4  ,
        -.43699847095201407660580845089167e-5  , +.35402774997630526799417139008534e-6  ,
        -.33111637792932920208982688245704e-7  , +.34459775819010534532311499770992e-8  ,
        -.38989323474754271048981937492758e-9  , +.47208197504658356400947449339005e-10 ,
        -.60478356628753562345373591562890e-11 , +.81284948748658747888193837985663e-12 ,
        -.11386945747147891428923915951042e-12 , +.16540358408462282325972948205090e-13 ,
        -.24809025677068848221516010440533e-14 , +.38292378907024096948429227299157e-15 ,
        -.60647341040012418187768210377386e-16 , +.98324256232648616038194004650666e-17 ,
        -.16284168738284380035666620115626e-17 , +.27501536496752623718284120337066e-18 ,
        -.47289666463953250924281069568000e-19 , +.82681500028109932722392050346666e-20 ,
        -.14681405136624956337193964885333e-20 , +.26447639269208245978085894826666e-21 ,
        -.48290157564856387897969868800000e-22 , +.89293020743610130180656332799999e-23 ,
        -.16708397168972517176997751466666e-23 , +.31616456034040694931368618666666e-24 ,
        -.60462055312274989106506410666666e-25 , +.11678798942042732700718421333333e-25 ,
        -.22773741582653996232867840000000e-26 , +.44811097300773675795305813333333e-27 ,
        -.88932884769020194062336000000000e-28 , +.17794680018850275131392000000000e-28 ,
        -.35884555967329095821994666666666e-29 , +.72906290492694257991679999999999e-30 ,
        -.14918449845546227073024000000000e-30 , +.30736573872934276300799999999999e-31 };
      try
      {
        int n = Initds(a,38,0.1*eps);
        answer = (1.25+NTermChebyshevSeries((16.0/x-5.0)/3.0,a,n))/sqrt(x);
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
    else
    {
      double const a[33] =
      { +.6379308343739001036600488534102e-1  , +.2832887813049720935835030284708e-1  ,
        -.2475370673905250345414545566732e-3  , +.5771972451607248820470976625763e-5  ,
        -.2068939219536548302745533196552e-6  , +.9739983441381804180309213097887e-8  ,
        -.5585336140380624984688895511129e-9  , +.3732996634046185240221212854731e-10 ,
        -.2825051961023225445135065754928e-11 , +.2372019002484144173643496955486e-12 ,
        -.2176677387991753979268301667938e-13 , +.2157914161616032453939562689706e-14 ,
        -.2290196930718269275991551338154e-15 , +.2582885729823274961919939565226e-16 ,
        -.3076752641268463187621098173440e-17 , +.3851487721280491597094896844799e-18 ,
        -.5044794897641528977117282508800e-19 , +.6888673850418544237018292223999e-20 ,
        -.9775041541950118303002132480000e-21 , +.1437416218523836461001659733333e-21 ,
        -.2185059497344347373499733333333e-22 , +.3426245621809220631645388800000e-23 ,
        -.5531064394246408232501248000000e-24 , +.9176601505685995403782826666666e-25 ,
        -.1562287203618024911448746666666e-25 , +.2725419375484333132349439999999e-26 ,
        -.4865674910074827992378026666666e-27 , +.8879388552723502587357866666666e-28 ,
        -.1654585918039257548936533333333e-28 , +.3145111321357848674303999999999e-29 ,
        -.6092998312193127612416000000000e-30 , +.1202021939369815834623999999999e-30 ,
        -.2412930801459408841386666666666e-31 };
      try
      {
        int n = Initds(a,33,0.1*eps);
        answer = (1.25+NTermChebyshevSeries(16.0/x-1.0,a,n))/sqrt(x);
      }
      catch (EExpressionError &e)
      {
        throw;
      }
    }
  }
  else
  {
    if( x < exp(std::max(log(xmin),-log(xmax))+0.01) )
      throw EExpressionError( wxT("EBesselK1: argument too small") );
    double y = 0.0;
    if( x > sqrt(4.0*eps) )y = x*x;
    double const b[16] =
    { +.25300227338947770532531120868533e-1, -.35315596077654487566723831691801e+0  ,
      -.12261118082265714823479067930042e+0  , -.69757238596398643501812920296083e-2  ,
      -.17302889575130520630176507368979e-3  , -.24334061415659682349600735030164e-5  ,
      -.22133876307347258558315252545126e-7  , -.14114883926335277610958330212608e-9  ,
      -.66669016941993290060853751264373e-12 , -.24274498505193659339263196864853e-14 ,
      -.70238634793862875971783797120000e-17 , -.16543275155100994675491029333333e-19 ,
      -.32338347459944491991893333333333e-22 , -.53312750529265274999466666666666e-25 ,
      -.75130407162157226666666666666666e-28 , -.91550857176541866666666666666666e-31 };
    try
    {
      int n = Initds(b,16,0.1*eps);
      answer = exp(x)*(log(0.5*x)*BesselI1(x)+(0.75+NTermChebyshevSeries(0.5*y-1.0,b,n))/x);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  return answer;
}

double BesselY0( double x )
{
  //  REFERENCE: SLATEC LIBRARY; CATEGORY B5I
  //  COMPUTES THE  BESSEL FUNCTION OF THE SECOND KIND OF ORDER ZERO
  // besy0
  //
  // SERIES FOR BY0     ON THE INTERVAL  0.          TO  1.60000E+01
  //                               WITH WEIGHTED ERROR   8.14E-32
  //                                LOG WEIGHTED ERROR  31.09
  //                      SIGNIFICANT FIGURES REQUIRED  30.31
  //                           DECIMAL PLACES REQUIRED  31.73
  //
  //   ARGUMENT:  overflow @ X ~< 0.588E-38
  //             underflow @ X ~> 3.602E+16
  //
  if( x <= 0.0 )
    throw EExpressionError( wxT("BesselY0: argument must be > 0") );
  double const eps = std::numeric_limits<double>::epsilon();
  double answer;
  if( x > 4.0 )
  {
    try
    {
      double ampl, theta;
      Bessel0ModulusPhase( x, ampl, theta );
      answer = ampl*sin(theta);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  else
  {
    double y = 0.0;
    if( x > sqrt(4.0*eps) )y = x*x;
    double const b[19] =
    { -.1127783939286557321793980546028e-1 , -.1283452375604203460480884531838e+0 ,
      -.1043788479979424936581762276618e+0 , +.2366274918396969540924159264613e-1 ,
      -.2090391647700486239196223950342e-2 , +.1039754539390572520999246576381e-3 ,
      -.3369747162423972096718775345037e-5 , +.7729384267670667158521367216371e-7 ,
      -.1324976772664259591443476068964e-8 , +.1764823261540452792100389363158e-10,
      -.1881055071580196200602823012069e-12, +.1641865485366149502792237185749e-14,
      -.1195659438604606085745991006720e-16, +.7377296297440185842494112426666e-19,
      -.3906843476710437330740906666666e-21, +.1795503664436157949829120000000e-23,
      -.7229627125448010478933333333333e-26, +.2571727931635168597333333333333e-28,
      -.8141268814163694933333333333333e-31 };
    try
    {
      int n = Initds( b, 19, 0.1*eps );
      answer = 2.0/M_PI*log(0.5*x)*BesselJ0(x) + 0.375 +
          NTermChebyshevSeries(0.125*y-1.0,b,n);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  return answer;
}

double BesselY1( double x )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5I
  //   COMPUTES THE BESSEL FUNCTION OF THE SECOND KIND OF ORDER ONE.
  // besy1
  //
  //   SERIES FOR BY1     ON THE INTERVAL  0.          TO  1.60000E+01
  //                                 WITH WEIGHTED ERROR   8.65E-33
  //                                  LOG WEIGHTED ERROR  32.06
  //                        SIGNIFICANT FIGURES REQUIRED  32.17
  //                             DECIMAL PLACES REQUIRED  32.71
  //
  //     ARGUMENT:  overflow @ X ~< 0.933E-38
  //               underflow @ X ~> 3.602E+16
  //
  if( x <= 0.0 )throw EExpressionError( wxT("BesselY1: argument must be > 0") );
  double answer;
  if( x > 4.0 )
  {
    try
    {
      double ampl, theta;
      Bessel1ModulusPhase( x, ampl, theta );
      answer = ampl*sin(theta);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  else
  {
    double const xmin = std::numeric_limits<double>::min();
    double const xmax = std::numeric_limits<double>::max();
    double const eps = std::numeric_limits<double>::epsilon();
    if( x < 1.571*exp(std::max(log(xmin),-log(xmax))+0.01) )
      throw EExpressionError( wxT("BesselY1: argument too small") );
    double y = 0.0;
    if( x > sqrt(4.0*eps) )y = x*x;
    double const b[20] =
    { +.320804710061190862932352018628015e-1, +.126270789743350044953431725999727e+1,
      +.649996189992317500097490637314144e-2, -.893616452886050411653144160009712e-1,
      +.132508812217570954512375510370043e-1, -.897905911964835237753039508298105e-3,
      +.364736148795830678242287368165349e-4, -.100137438166600055549075523845295e-5,
      +.199453965739017397031159372421243e-7, -.302306560180338167284799332520743e-9,
      +.360987815694781196116252914242474e-11, -.348748829728758242414552947409066e-13,
      +.278387897155917665813507698517333e-15, -.186787096861948768766825352533333e-17,
      +.106853153391168259757070336000000e-19, -.527472195668448228943872000000000e-22,
      +.227019940315566414370133333333333e-24, -.859539035394523108693333333333333e-27,
      +.288540437983379456000000000000000e-29, -.864754113893717333333333333333333e-32 };
    try
    {
      int n = Initds(b,20,0.1*eps);
      answer = 2.0/M_PI*log(0.5*x)*BesselJ1(x)+(0.5+NTermChebyshevSeries(0.125*y-1.0,b,n))/x;
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  return answer;
}

void Bessel0ModulusPhase( double x, double &ampl, double &theta )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5L
  //   EVALUATES THE  MODULUS AND PHASE FOR J0 AND Y0 BESSEL FUNCTIONS.
  // d9b0mp
  //
  //   SERIES FOR BM0    ON THE INTERVAL  1.56250E-02 TO  6.25000E-02
  //                                WITH WEIGHTED ERROR   4.40E-32
  //                                 LOG WEIGHTED ERROR  31.36
  //                       SIGNIFICANT FIGURES REQUIRED  30.02
  //                            DECIMAL PLACES REQUIRED  32.14
  //
  //   SERIES FOR BTH0   ON THE INTERVAL  0.          TO  1.56250E-02
  //                                WITH WEIGHTED ERROR   2.66E-32
  //                                 LOG WEIGHTED ERROR  31.57
  //                       SIGNIFICANT FIGURES REQUIRED  30.67
  //                            DECIMAL PLACES REQUIRED  32.40
  //
  //   SERIES FOR BM02   ON THE INTERVAL  0.          TO  1.56250E-02
  //                                WITH WEIGHTED ERROR   4.72E-32
  //                                 LOG WEIGHTED ERROR  31.33
  //                       SIGNIFICANT FIGURES REQUIRED  30.00
  //                            DECIMAL PLACES REQUIRED  32.13
  //
  //   SERIES FOR BT02   ON THE INTERVAL  1.56250E-02 TO  6.25000E-02
  //                                WITH WEIGHTED ERROR   2.99E-32
  //                                 LOG WEIGHTED ERROR  31.52
  //                       SIGNIFICANT FIGURES REQUIRED  30.61
  //                            DECIMAL PLACES REQUIRED  32.32
  //
  if( x < 4.0 )
    throw EExpressionError( wxT("Bessel0ModulusPhase: argument must be >= 4") );
  double const eps = std::numeric_limits<double>::epsilon();
  if( x > 8.0 )
  {
    if( x > 1.0/(2.0*eps) )
      throw EExpressionError( wxT("Bessel0ModulusPhase: argument too large") );
    double z = 128.0/(x*x)-1.0;
    double const b1[40] =
    { +.9500415145228381369330861335560e-1  , -.3801864682365670991748081566851e-3  ,
      +.2258339301031481192951829927224e-5  , -.3895725802372228764730621412605e-7  ,
      +.1246886416512081697930990529725e-8  , -.6065949022102503779803835058387e-10 ,
      +.4008461651421746991015275971045e-11 , -.3350998183398094218467298794574e-12 ,
      +.3377119716517417367063264341996e-13 , -.3964585901635012700569356295823e-14 ,
      +.5286111503883857217387939744735e-15 , -.7852519083450852313654640243493e-16 ,
      +.1280300573386682201011634073449e-16 , -.2263996296391429776287099244884e-17 ,
      +.4300496929656790388646410290477e-18 , -.8705749805132587079747535451455e-19 ,
      +.1865862713962095141181442772050e-19 , -.4210482486093065457345086972301e-20 ,
      +.9956676964228400991581627417842e-21 , -.2457357442805313359605921478547e-21 ,
      +.6307692160762031568087353707059e-22 , -.1678773691440740142693331172388e-22 ,
      +.4620259064673904433770878136087e-23 , -.1311782266860308732237693402496e-23 ,
      +.3834087564116302827747922440276e-24 , -.1151459324077741271072613293576e-24 ,
      +.3547210007523338523076971345213e-25 , -.1119218385815004646264355942176e-25 ,
      +.3611879427629837831698404994257e-26 , -.1190687765913333150092641762463e-26 ,
      +.4005094059403968131802476449536e-27 , -.1373169422452212390595193916017e-27 ,
      +.4794199088742531585996491526437e-28 , -.1702965627624109584006994476452e-28 ,
      +.6149512428936330071503575161324e-29 , -.2255766896581828349944300237242e-29 ,
      +.8399707509294299486061658353200e-30 , -.3172997595562602355567423936152e-30 ,
      +.1215205298881298554583333026514e-30 , -.4715852749754438693013210568045e-31 };
    try
    {
      int n1 = Initds(b1,40,0.1*eps);
      ampl = (0.75+NTermChebyshevSeries(z,b1,n1))/sqrt(x);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    double const b2[44] =
    { -.24901780862128936717709793789967e+0 , +.48550299609623749241048615535485e-3 ,
      -.54511837345017204950656273563505e-5 , +.13558673059405964054377445929903e-6 ,
      -.55691398902227626227583218414920e-8 , +.32609031824994335304004205719468e-9 ,
      -.24918807862461341125237903877993e-10, +.23449377420882520554352413564891e-11,
      -.26096534444310387762177574766136e-12, +.33353140420097395105869955014923e-13,
      -.47890000440572684646750770557409e-14, +.75956178436192215972642568545248e-15,
      -.13131556016891440382773397487633e-15, +.24483618345240857495426820738355e-16,
      -.48805729810618777683256761918331e-17, +.10327285029786316149223756361204e-17,
      -.23057633815057217157004744527025e-18, +.54044443001892693993017108483765e-19,
      -.13240695194366572724155032882385e-19, +.33780795621371970203424792124722e-20,
      -.89457629157111779003026926292299e-21, +.24519906889219317090899908651405e-21,
      -.69388422876866318680139933157657e-22, +.20228278714890138392946303337791e-22,
      -.60628500002335483105794195371764e-23, +.18649748964037635381823788396270e-23,
      -.58783732384849894560245036530867e-24, +.18958591447999563485531179503513e-24,
      -.62481979372258858959291620728565e-25, +.21017901684551024686638633529074e-25,
      -.72084300935209253690813933992446e-26, +.25181363892474240867156405976746e-26,
      -.89518042258785778806143945953643e-27, +.32357237479762298533256235868587e-27,
      -.11883010519855353657047144113796e-27, +.44306286907358104820579231941731e-28,
      -.16761009648834829495792010135681e-28, +.64292946921207466972532393966088e-29,
      -.24992261166978652421207213682763e-29, +.98399794299521955672828260355318e-30,
      -.39220375242408016397989131626158e-30, +.15818107030056522138590618845692e-30,
      -.64525506144890715944344098365426e-31, +.26611111369199356137177018346367e-31 };
    try
    {
      int n2 = Initds(b2,44,0.1*eps);
      theta = x-M_PI/4.0+NTermChebyshevSeries(z,b2,n2)/x;
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  else
  {
    double z = (128.0/(x*x)-5.0)/3.0;
    double const b1[37] =
    { +.9211656246827742712573767730182e-1  , -.1050590997271905102480716371755e-2  ,
      +.1470159840768759754056392850952e-4  , -.5058557606038554223347929327702e-6  ,
      +.2787254538632444176630356137881e-7  , -.2062363611780914802618841018973e-8  ,
      +.1870214313138879675138172596261e-9  , -.1969330971135636200241730777825e-10 ,
      +.2325973793999275444012508818052e-11 , -.3009520344938250272851224734482e-12 ,
      +.4194521333850669181471206768646e-13 , -.6219449312188445825973267429564e-14 ,
      +.9718260411336068469601765885269e-15 , -.1588478585701075207366635966937e-15 ,
      +.2700072193671308890086217324458e-16 , -.4750092365234008992477504786773e-17 ,
      +.8615128162604370873191703746560e-18 , -.1605608686956144815745602703359e-18 ,
      +.3066513987314482975188539801599e-19 , -.5987764223193956430696505617066e-20 ,
      +.1192971253748248306489069841066e-20 , -.2420969142044805489484682581333e-21 ,
      +.4996751760510616453371002879999e-22 , -.1047493639351158510095040511999e-22 ,
      +.2227786843797468101048183466666e-23 , -.4801813239398162862370542933333e-24 ,
      +.1047962723470959956476996266666e-24 , -.2313858165678615325101260800000e-25 ,
      +.5164823088462674211635199999999e-26 , -.1164691191850065389525401599999e-26 ,
      +.2651788486043319282958336000000e-27 , -.6092559503825728497691306666666e-28 ,
      +.1411804686144259308038826666666e-28 , -.3298094961231737245750613333333e-29 ,
      +.7763931143074065031714133333333e-30 , -.1841031343661458478421333333333e-30 ,
      +.4395880138594310737100799999999e-31 };
    try
    {
      int n1 = Initds(b1,37,0.1*eps);
      ampl = (0.75+NTermChebyshevSeries(z,b1,n1))/sqrt(x);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    double const b2[39] =
    { -.24548295213424597462050467249324e+0 , +.12544121039084615780785331778299e-2 ,
      -.31253950414871522854973446709571e-4 , +.14709778249940831164453426969314e-5 ,
      -.99543488937950033643468850351158e-7 , +.85493166733203041247578711397751e-8 ,
      -.86989759526554334557985512179192e-9 , +.10052099533559791084540101082153e-9 ,
      -.12828230601708892903483623685544e-10, +.17731700781805131705655750451023e-11,
      -.26174574569485577488636284180925e-12, +.40828351389972059621966481221103e-13,
      -.66751668239742720054606749554261e-14, +.11365761393071629448392469549951e-14,
      -.20051189620647160250559266412117e-15, +.36497978794766269635720591464106e-16,
      -.68309637564582303169355843788800e-17, +.13107583145670756620057104267946e-17,
      -.25723363101850607778757130649599e-18, +.51521657441863959925267780949333e-19,
      -.10513017563758802637940741461333e-19, +.21820381991194813847301084501333e-20,
      -.46004701210362160577225905493333e-21, +.98407006925466818520953651199999e-22,
      -.21334038035728375844735986346666e-22, +.46831036423973365296066286933333e-23,
      -.10400213691985747236513382399999e-23, +.23349105677301510051777740800000e-24,
      -.52956825323318615788049749333333e-25, +.12126341952959756829196287999999e-25,
      -.28018897082289428760275626666666e-26, +.65292678987012873342593706666666e-27,
      -.15337980061873346427835733333333e-27, +.36305884306364536682359466666666e-28,
      -.86560755713629122479172266666666e-29, +.20779909972536284571238399999999e-29,
      -.50211170221417221674325333333333e-30, +.12208360279441714184191999999999e-30,
      -.29860056267039913454250666666666e-31 };
    try
    {
      int n2 = Initds(b2,39,0.1*eps);
      theta = x-M_PI/4.0+NTermChebyshevSeries(z,b2,n2)/x;
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
}

void Bessel1ModulusPhase( double x, double &ampl, double &theta )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5L
  //   EVALUATE THE MODULUS AND PHASE FOR THE J1 AND Y1 BESSEL FUNCTIONS.
  // d9blmp
  //
  // SERIES FOR BM1      ON THE INTERVAL  1.56250E-02 TO  6.25000E-02
  //                                WITH WEIGHTED ERROR   4.91E-32
  //                                 LOG WEIGHTED ERROR  31.31
  //                       SIGNIFICANT FIGURES REQUIRED  30.04
  //                            DECIMAL PLACES REQUIRED  32.09
  //
  //SERIES FOR BT12     ON THE INTERVAL  1.56250E-02 TO  6.25000E-02
  //                                WITH WEIGHTED ERROR   3.33E-32
  //                                 LOG WEIGHTED ERROR  31.48
  //                       SIGNIFICANT FIGURES REQUIRED  31.05
  //                            DECIMAL PLACES REQUIRED  32.27
  //
  // SERIES FOR BM12     ON THE INTERVAL  0.          TO  1.56250E-02
  //                                WITH WEIGHTED ERROR   5.01E-32
  //                                 LOG WEIGHTED ERROR  31.30
  //                       SIGNIFICANT FIGURES REQUIRED  29.99
  //                            DECIMAL PLACES REQUIRED  32.10
  //
  // SERIES FOR BTH1     ON THE INTERVAL  0.          TO  1.56250E-02
  //                                WITH WEIGHTED ERROR   2.82E-32
  //                                 LOG WEIGHTED ERROR  31.55
  //                       SIGNIFICANT FIGURES REQUIRED  31.12
  //                            DECIMAL PLACES REQUIRED  32.37
  //
  if( x < 4.0 )
    throw EExpressionError( wxT("Bessel1ModulusPhase: argument must be >= 4") );
  //
  double const eps = std::numeric_limits<double>::epsilon();
  if( x > 8.0 )
  {
    if( x > 1.0/(2.0*eps) )
      throw EExpressionError( wxT("Bessel1ModulusPhase: argument too large") );
    double z = 128.0/(x*x)-1.0;
    double const b1[40] =
    { +.9807979156233050027272093546937e-1  , +.1150961189504685306175483484602e-2  ,
      -.4312482164338205409889358097732e-5  , +.5951839610088816307813029801832e-7  ,
      -.1704844019826909857400701586478e-8  , +.7798265413611109508658173827401e-10 ,
      -.4958986126766415809491754951865e-11 , +.4038432416421141516838202265144e-12 ,
      -.3993046163725175445765483846645e-13 , +.4619886183118966494313342432775e-14 ,
      -.6089208019095383301345472619333e-15 , +.8960930916433876482157048041249e-16 ,
      -.1449629423942023122916518918925e-16 , +.2546463158537776056165149648068e-17 ,
      -.4809472874647836444259263718620e-18 , +.9687684668292599049087275839124e-19 ,
      -.2067213372277966023245038117551e-19 , +.4646651559150384731802767809590e-20 ,
      -.1094966128848334138241351328339e-20 , +.2693892797288682860905707612785e-21 ,
      -.6894992910930374477818970026857e-22 , +.1830268262752062909890668554740e-22 ,
      -.5025064246351916428156113553224e-23 , +.1423545194454806039631693634194e-23 ,
      -.4152191203616450388068886769801e-24 , +.1244609201503979325882330076547e-24 ,
      -.3827336370569304299431918661286e-25 , +.1205591357815617535374723981835e-25 ,
      -.3884536246376488076431859361124e-26 , +.1278689528720409721904895283461e-26 ,
      -.4295146689447946272061936915912e-27 , +.1470689117829070886456802707983e-27 ,
      -.5128315665106073128180374017796e-28 , +.1819509585471169385481437373286e-28 ,
      -.6563031314841980867618635050373e-29 , +.2404898976919960653198914875834e-29 ,
      -.8945966744690612473234958242979e-30 , +.3376085160657231026637148978240e-30 ,
      -.1291791454620656360913099916966e-30 , +.5008634462958810520684951501254e-31 };
    try
    {
      int n1 = Initds(b1,40,0.1*eps);
      ampl = (0.75+NTermChebyshevSeries(z,b1,n1))/sqrt(x);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    double const b2[44] =
    { +.74749957203587276055443483969695e+0  , -.12400777144651711252545777541384e-2  ,
      +.99252442404424527376641497689592e-5  , -.20303690737159711052419375375608e-6  ,
      +.75359617705690885712184017583629e-8  , -.41661612715343550107630023856228e-9  ,
      +.30701618070834890481245102091216e-10 , -.28178499637605213992324008883924e-11 ,
      +.30790696739040295476028146821647e-12 , -.38803300262803434112787347554781e-13 ,
      +.55096039608630904934561726208562e-14 , -.86590060768383779940103398953994e-15 ,
      +.14856049141536749003423689060683e-15 , -.27519529815904085805371212125009e-16 ,
      +.54550796090481089625036223640923e-17 , -.11486534501983642749543631027177e-17 ,
      +.25535213377973900223199052533522e-18 , -.59621490197413450395768287907849e-19 ,
      +.14556622902372718620288302005833e-19 , -.37022185422450538201579776019593e-20 ,
      +.97763074125345357664168434517924e-21 , -.26726821639668488468723775393052e-21 ,
      +.75453300384983271794038190655764e-22 , -.21947899919802744897892383371647e-22 ,
      +.65648394623955262178906999817493e-23 , -.20155604298370207570784076869519e-23 ,
      +.63417768556776143492144667185670e-24 , -.20419277885337895634813769955591e-24 ,
      +.67191464220720567486658980018551e-25 , -.22569079110207573595709003687336e-25 ,
      +.77297719892989706370926959871929e-26 , -.26967444512294640913211424080920e-26 ,
      +.95749344518502698072295521933627e-27 , -.34569168448890113000175680827627e-27 ,
      +.12681234817398436504211986238374e-27 , -.47232536630722639860464993713445e-28 ,
      +.17850008478186376177858619796417e-28 , -.68404361004510395406215223566746e-29 ,
      +.26566028671720419358293422672212e-29 , -.10450402527914452917714161484670e-29 ,
      +.41618290825377144306861917197064e-30 , -.16771639203643714856501347882887e-30 ,
      +.68361997776664389173535928028528e-31 , -.28172247861233641166739574622810e-31 };
    try
    {
      int n2 = Initds(b2,44,0.1*eps);
      theta = x-3.0*M_PI/4.0+NTermChebyshevSeries(z,b2,n2)/x;
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
  else
  {
    double z = (128.0/(x*x)-5.0)/3.0;
    double const b1[37] =
    { +.1069845452618063014969985308538e+0, +.3274915039715964900729055143445e-2,
      -.2987783266831698592030445777938e-4, +.8331237177991974531393222669023e-6,
      -.4112665690302007304896381725498e-7, +.2855344228789215220719757663161e-8,
      -.2485408305415623878060026596055e-9, +.2543393338072582442742484397174e-10,
      -.2941045772822967523489750827909e-11, +.3743392025493903309265056153626e-12,
      -.5149118293821167218720548243527e-13, +.7552535949865143908034040764199e-14,
      -.1169409706828846444166290622464e-14, +.1896562449434791571721824605060e-15,
      -.3201955368693286420664775316394e-16, +.5599548399316204114484169905493e-17,
      -.1010215894730432443119390444544e-17, +.1873844985727562983302042719573e-18,
      -.3563537470328580219274301439999e-19, +.6931283819971238330422763519999e-20,
      -.1376059453406500152251408930133e-20, +.2783430784107080220599779327999e-21,
      -.5727595364320561689348669439999e-22, +.1197361445918892672535756799999e-22,
      -.2539928509891871976641440426666e-23, +.5461378289657295973069619199999e-24,
      -.1189211341773320288986289493333e-24, +.2620150977340081594957824000000e-25,
      -.5836810774255685901920938666666e-26, +.1313743500080595773423615999999e-26,
      -.2985814622510380355332778666666e-27, +.6848390471334604937625599999999e-28,
      -.1584401568222476721192960000000e-28, +.3695641006570938054301013333333e-29,
      -.8687115921144668243012266666666e-30, +.2057080846158763462929066666666e-30,
      -.4905225761116225518523733333333e-31 };
    try
    {
      int n1 = Initds(b1,37,0.1*eps);
      ampl = (0.75+NTermChebyshevSeries(z,b1,n1))/sqrt(x);
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    double const b2[39] =
    { +.73823860128742974662620839792764e+0  , -.33361113174483906384470147681189e-2  ,
      +.61463454888046964698514899420186e-4  , -.24024585161602374264977635469568e-5  ,
      +.14663555577509746153210591997204e-6  , -.11841917305589180567005147504983e-7  ,
      +.11574198963919197052125466303055e-8  , -.13001161129439187449366007794571e-9  ,
      +.16245391141361731937742166273667e-10 , -.22089636821403188752155441770128e-11 ,
      +.32180304258553177090474358653778e-12 , -.49653147932768480785552021135381e-13 ,
      +.80438900432847825985558882639317e-14 , -.13589121310161291384694712682282e-14 ,
      +.23810504397147214869676529605973e-15 , -.43081466363849106724471241420799e-16 ,
      +.80202544032771002434993512550400e-17 , -.15316310642462311864230027468799e-17 ,
      +.29928606352715568924073040554666e-18 , -.59709964658085443393815636650666e-19 ,
      +.12140289669415185024160852650666e-19 , -.25115114696612948901006977706666e-20 ,
      +.52790567170328744850738380799999e-21 , -.11260509227550498324361161386666e-21 ,
      +.24348277359576326659663462400000e-22 , -.53317261236931800130038442666666e-23 ,
      +.11813615059707121039205990399999e-23 , -.26465368283353523514856789333333e-24 ,
      +.59903394041361503945577813333333e-25 , -.13690854630829503109136383999999e-25 ,
      +.31576790154380228326413653333333e-26 , -.73457915082084356491400533333333e-27 ,
      +.17228081480722747930705920000000e-27 , -.40716907961286507941068800000000e-28 ,
      +.96934745136779622700373333333333e-29 , -.23237636337765716765354666666666e-29 ,
      +.56074510673522029406890666666666e-30 , -.13616465391539005860522666666666e-30 ,
      +.33263109233894654388906666666666e-31 };
    try
    {
      int n2 = Initds(b2,39,0.1*eps);
      theta = x-3.0*M_PI/4.0+NTermChebyshevSeries(z,b2,n2)/x;
    }
    catch (EExpressionError &e)
    {
      throw;
    }
  }
}

double CompleteBeta( double a, double b )
{
  //   REFERENCE: SLATEC LIBRARY; CATEGORY B5F
  //   COMPUTES THE COMPLETE BETA FUNCTION.
  // beta
  //
  //     INPUT PARAMETERS:
  //       A   REAL AND POSITIVE
  //       B   REAL AND POSITIVE
  //
  if( a<=0.0 || b<=0.0 )
    throw EExpressionError( wxT("CompleteBeta: both arguments must be > 0") );
  //
  double xmin, xmax, answer;
  try
  {
    ComputeGammaBounds(xmin,xmax);
    if( a+b < xmax )return CompleteGamma(a)*CompleteGamma(b)/CompleteGamma(a+b);
    //
    answer = LnCompleteBeta(a,b);
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  if( answer < log(std::numeric_limits<double>::min()) )
    throw EExpressionError( wxT("CompleteBeta: parameter too large") );
  return exp(answer);
}

double Psi( double x )
{
  //  REFERENCE: SLATEC LIBRARY; CATEGORY B5F
  //  COMPUTES THE  PSI (OR DIGAMMA) FUNCTION.
  // psi
  //
  //  SERIES FOR PSI    ON THE INTERVAL  0.          TO  1.00000E+00
  //                               WITH WEIGHTED ERROR   5.79E-32
  //                                LOG WEIGHTED ERROR  31.24
  //                      SIGNIFICANT FIGURES REQUIRED  30.93
  //                           DECIMAL PLACES REQUIRED  32.05
  //    SERIES FOR APSI   ON THE INTERVAL  0.          TO  1.00000E-02
  //                               WITH WEIGHTED ERROR   7.75E-33
  //                                LOG WEIGHTED ERROR  32.11
  //                      SIGNIFICANT FIGURES REQUIRED  28.88
  //                           DECIMAL PLACES REQUIRED  32.71
  //
  // ARGUMENT: overflow @ X ~< 0.588E-38  or  X ~> 1.701E+38
  //
  double const psics[42] =
  { -.38057080835217921520437677667039e-1 , +.49141539302938712748204699654277e+0 ,
    -.56815747821244730242892064734081e-1 , +.83578212259143131362775650747862e-2 ,
    -.13332328579943425998079274172393e-2 , +.22031328706930824892872397979521e-3 ,
    -.37040238178456883592889086949229e-4 , +.62837936548549898933651418717690e-5 ,
    -.10712639085061849855283541747074e-5 , +.18312839465484165805731589810378e-6 ,
    -.31353509361808509869005779796885e-7 , +.53728087762007766260471919143615e-8 ,
    -.92116814159784275717880632624730e-9 , +.15798126521481822782252884032823e-9 ,
    -.27098646132380443065440589409707e-10, +.46487228599096834872947319529549e-11,
    -.79752725638303689726504797772737e-12, +.13682723857476992249251053892838e-12,
    -.23475156060658972717320677980719e-13, +.40276307155603541107907925006281e-14,
    -.69102518531179037846547422974771e-15, +.11856047138863349552929139525768e-15,
    -.20341689616261559308154210484223e-16, +.34900749686463043850374232932351e-17,
    -.59880146934976711003011081393493e-18, +.10273801628080588258398005712213e-18,
    -.17627049424561071368359260105386e-19, +.30243228018156920457454035490133e-20,
    -.51889168302092313774286088874666e-21, +.89027730345845713905005887487999e-22,
    -.15274742899426728392894971904000e-22, +.26207314798962083136358318079999e-23,
    -.44964642738220696772598388053333e-24, +.77147129596345107028919364266666e-25,
    -.13236354761887702968102638933333e-25, +.22709994362408300091277311999999e-26,
    -.38964190215374115954491391999999e-27, +.66851981388855302310679893333333e-28,
    -.11469986654920864872529919999999e-28, +.19679385886541405920515413333333e-29,
    -.33764488189750979801907200000000e-30, +.57930703193214159246677333333333e-31 };
  double const apsics[16] =
  { -.832710791069290760174456932269e-3 , -.416251842192739352821627121990e-3 ,
    +.103431560978741291174463193961e-6 , -.121468184135904152987299556365e-9 ,
    +.311369431998356155521240278178e-12, -.136461337193177041776516100945e-14,
    +.902051751315416565130837974000e-17, -.831542997421591464829933635466e-19,
    +.101224257073907254188479482666e-20, -.156270249435622507620478933333e-22,
    +.296542716808903896133226666666e-24, -.674686886765702163741866666666e-26,
    +.180345311697189904213333333333e-27, -.556901618245983607466666666666e-29,
    +.195867922607736251733333333333e-30, -.775195892523335680000000000000e-32 };
  double const eps = std::numeric_limits<double>::epsilon();
  int ntpsi, ntapsi;
  try
  {
    ntpsi = Initds( psics, 42, 0.1*eps );
    ntapsi = Initds( apsics, 16, 0.1*eps );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  double const xbig = 1.0/sqrt(eps);
  double const dxrel = sqrt(2.0*eps);
  //
  double answer;
  double y = fabs(x);
  if( y > 10.0 ) // psi(x) for fabs(x) > 10.0
  {
    double aux = 0.0;
    try
    {
      if( y < xbig )aux = NTermChebyshevSeries( 2.0*(100.0/y/y)-1.0, apsics, ntapsi );
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    if( x < 0.0 )answer = log(fabs(x)) - 0.5/x + aux - M_PI/tan(M_PI*x);
    if( x > 0.0 )answer = log(x) - 0.5/x + aux;
  }
  else
  {
    //
    // psi(x) for fabs(x) <= 2
    //
    int n = static_cast<int>(x);
    if( x < 0.0 )--n;
    y = x - n;
    try
    {
      answer = NTermChebyshevSeries( 2*y-1, psics, ntpsi );
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    if( --n == 0 )return answer;
    if( n > 0 ) // psi(x) for x >= 2.0 and x <= 10.0
    {
      for( int i=1; i<=n; ++i )answer += 1.0/(y+i);
    }
    else
    {
      n *= -1;
      if( x == 0.0 )throw EExpressionError( wxT("Psi: argument is 0") );
      if( x<0.0 && x+n-2==0.0 )
        throw EExpressionError( wxT("Psi: argument is a negative integer") );
      if( x<-0.5 && fabs((x-static_cast<int>(x-0.5))/x)<dxrel )
        throw EExpressionError(
         wxT("Psi: answer < half precision because argument is too near a negative integer") );
      for( int i=0; i<n; ++i )answer -= 1.0/(x+i);
    }
  }
  return answer;
}

double DiGamma( double x )
{
  // The DiGamma function (Psi function)
  //       DiGamma(x) = d(log GAMMA(X))/dx
  // adigam
  //
  //     REFERENCE : CERN library routine C317
  //     RESTRICTION ON X : X IS REAL BUT X # -N (N = 0,1,2,...)
  //       if X = -N, ADIGAM is set to 0 with error message
  //
  //     ACCURACY : to at least 1E-6.
  //       Accuracy is lost near zeros of PSI, these zeros are all negative
  //         except one at x=1.461633...
  //       REGION TESTED: 1.000 <= X <= 2.000 for real X,
  //                      2 <= X <= 101 for integer X.
  //
  double a = fabs(x);
  if( -a == static_cast<double>(static_cast<int>(x)) )
    throw EExpressionError( wxT("DiGamma: argument is non-positive integer") );
  double v = a;
  double h = 0.0;
  int n;
  if( a < 15.0 )
  {
    n = 14-static_cast<int>(a);
    h = 1.0/v;
    if( n != 0 )
    {
      for( int i=1; i<=n; ++i )h += 1.0/(++v);
    }
    ++v;
  }
  double r = 1.0/(v*v);
  double const b[6] = { +8.3333333333e-2, -8.3333333333e-3,
                        +3.9682539683e-3, -4.1666666667e-3,
                        +7.5757575758e-3, -2.1092796093e-2 };
  double answer = log(v)-
      0.5/v-r*(b[0]+r*(b[1]+r*(b[2]+r*(b[3]+r*(b[4]+r*(b[5]+r*b[0]))))))-h;
  if( x < 0.0 )
  {
    h = M_PI*a;
    answer += 1.0/a + M_PI*cos(h)/sin(h);
  }
  return answer;
}

double AssociatedLegendreFunctions( int degree,
                                                    int order,
                                                    double y,
                                                    bool normalized )
{
  // Returns the normalized/non-normalized associated Legendre function
  // determined by order, degree and argument y.
  // aslegf
  //
  // Also known as the spherical harmonics
  // when order=0, they are the Legendre polynomials, or the Legendre
  // functions of the first kind (of degree)
  // Reference: CERN library routine C315 (old) C330 (new)
  //
  // ARGUMENT RESTRICTIONS:
  //  degree ----- POSITIVE INTEGER OR ZERO
  //   order ----- -degree <= order <= degree;
  //               if abs(order) > degree, return 0
  //       y ----- -1 <= y <= 1
  //
  // ACCURACY: with degree=2,10; order=0
  //  i) 0 <= y <= 0.1,  accurate to at least 1.0E-6
  // ii) 0.2 <= y <= 1.0, 6 significant figures are correct
  //
  if( degree < 0 )throw EExpressionError(
   wxT("AssociatedLegendreFunctions: degree must be > 0") );
  if( fabs(y) > 1.0 )throw EExpressionError(
   wxT("AssociatedLegendreFunctions: argument not in range (-1,1)") );
  double answer;
  double fact = 1.0;
  if( degree < abs(order) )return 0.0;
  if( order < 0 )
  {
    double prod = 1.0;
    for( int i=degree-abs(order)+1; i<=degree+abs(order); ++i )prod *= i;
    fact = 1.0/prod;
  }
  if( abs(order) <= 1 )
  {
    double p0=1.0, p1=y;
    if( abs(order) != 0 )
    {
      p0 = 0.0;
      p1 = sqrt(1.0-y*y);
    }
    if( degree == 0 )answer = p0;
    else if( degree == 1 )answer = p1*fact;
    else
    {
      double pnmn1 = p0;
      double pn = p1;
      double pnpl1;
      for( int i=1; i<degree; ++i )
      {
        pnpl1 = (1.0/(i-abs(order)+1))*((2*i+1)*y*pn-(i+abs(order))*pnmn1);
        pnmn1 = pn;
        pn = pnpl1;
      }
      answer = pnpl1*fact;
    }
    if( normalized )goto calculate_factor;
    return answer;
  }
  else // abs(order) > 1
  {
    double z2 = 1.0 - y*y;
    double zham = pow(z2,abs(order)/2.0);
    if( abs(order) == degree-1 )
    {
      double prod = 1.0;
      for( int i=degree; i<=2*degree-1; ++i )prod *= i;
      answer = y*prod/pow(2.0,degree-1.0)*zham*fact;
      if( normalized )goto calculate_factor;
      return answer;
    }
    else
    {
      double prod = 1.0;
      for( int i=degree+1; i<=2*degree; ++i )prod *= i;
      double dnn = prod/pow(2.0,static_cast<double>(degree));
      if( abs(order) == degree )
      {
        answer = dnn*zham*fact;
        if( normalized )goto calculate_factor;
        return answer;
      }
      else
      {
        double prod = 1.0;
        for( int i=degree; i<=2*degree-1; ++i )prod *= i;
        double dnm2n, dnm1n=y*prod/pow(2.0,degree-1.0);
        for( int i=1; i<=degree-1-abs(order); ++i )
        {
          dnm2n = (1.0/((1+i)*(2*degree-i)))*(2*(degree-i)*y*dnm1n-z2*dnn);
          dnn = dnm1n;
          dnm1n = dnm2n;
        }
        answer = dnm2n*zham*fact;
        if( normalized )goto calculate_factor;
        return answer;
      }
    }
  }
calculate_factor:
  if( order != 0 )
  {
    double prod = 1.0;
    for( int i=degree-abs(order)+1; i<=degree+abs(order); ++i )prod *= i;
    fact = 0.5*(2*degree+1)*pow(prod,static_cast<double>(-order/abs(order)));
  }
  else fact = 0.5*(2*degree+1);
  fact = sqrt(fact);
  answer *= fact;
  return answer;
}

double BinomialCoefficient( int n, int m )
{
  // Reference: SLATEC library; category B5C
  // Computes the binomial coefficients
  // BinomialCoefficient(n,m) = n!/((n-m)!m!)
  // binom
  //
  double const bilnmx = log(std::numeric_limits<double>::max()) - 0.0001;
  //double const fintmx = 0.9/std::numeric_limits<double>::epsilon();
  //
  if( n<0 || m<0 )throw EExpressionError(
   wxT("BinomialCoefficient: both arguments must be >= 0") );
  if( n < m )throw EExpressionError(
   wxT("BinomialCoefficient: first argument < second argument") );
  int const k = std::min(m,n-m);
  if( k > 20 )throw EExpressionError(
   wxT("BinomialCoefficient: result overflows because n and/or m too big") );
  //
  double result;
  //
  if( k*log(static_cast<double>(std::max(n,1))) > bilnmx )
  {
    if( k >= 9 )throw EExpressionError(
     wxT("BinomialCoefficient: result overflows because n and/or m too big") );
    double const xn = static_cast<double>(n+1);
    double const xk = static_cast<double>(k+1);
    double const xnk = static_cast<double>(n-k+1);
    double const corr = LogGammaCorrection(xn) - LogGammaCorrection(xk) -
        LogGammaCorrection(xnk);
    result = xk*log(xnk/xk) - xn*LnRel(-(xk-1.0)/xn) -
        0.5*log(xn*xnk/xk) + 1.0 - log(sqrt(2*M_PI)) + corr;
    //
    if( result > bilnmx )throw EExpressionError(
     wxT("BinomialCoefficient: result overflows because n and/or m too big") );
    result = exp(result);
  }
  else
  {
    result = 1.0;
    if( k > 0 )
    {
      for( int i=1; i<=k; ++i )
        result *= static_cast<double>(n-i+1)/static_cast<double>(i);
    }
  }
  //if( result < fintmx )result += 0.5;
  //return static_cast<int>(result);
  return result;
}

double ChebyshevPolynomial( int n, double x )
{
  // compute the n-term Chebyshev polynomial at x T<n>(x)
  // reference: CACM algorithm #10
  // cheby
  //
  if( n < 0 )throw EExpressionError(wxT("ChebyshevPolynomial: polynomial order must be >= 0"));
  if( n == 0 )return 1.0;
  if( n == 1 )return x;
  double a = 1.0;
  double b = x;
  double c;
  for( int i=2; i<=n; ++i )
  {
    c = 2*x*b - a;
    a = b;
    b = c;
  }
  return c;
}

double HermitePolynomial( int n, double x )
{
  if( n < 0 )throw EExpressionError(wxT("HermitePolynomial: polynomial order must be >= 0"));
  if( n == 0 )return 1.0;
  if( n == 1 )return 2*x;
  double a=1.0, b=2*x, c;
  for( int i=1; i<=n-1; ++i )
  {
    c = 2*x*b - 2*i*a;
    a = b;
    b = c;
  }
  return c;
}

double JacobiPolynomial( double aa, double bb, int n, double x )
{
  if( n < 0 )throw EExpressionError(wxT("JacobiPolynomial: polynomial order must be >= 0"));
  if( n > 25 )throw EExpressionError(wxT("JacobiPolynomial: polynomial order must be <= 25"));
  if( n == 0 )return 1.0;
  if( n == 1 )return (aa-bb)/2.0+(1.0+(aa+bb)/2.0)*x;
  //
  // calculate the u(j),v(j),w(j) in the recurrence relation:
  // p(j)=p(j-1)*(u(j)+v(j)*x) - p(j-2)*w(j)
  //
  double u[25], v[25], w[25], p[25];
  double a = aa + bb;
  double b = aa - bb;
  u[0] = b/2.0;
  v[0] = 1.0 + a/2.0;
  w[0] = 0.0;
  u[1] = a*b*(a+3)/(4*(a+2)*(a+2));
  v[1] = (a+3)*(a+4)/(4*(a+2));
  w[1] = (1+aa)*(1+bb)*(a+4)/(2*(a+2)*(a+2));
  for( int j=2; j<=n-1; ++j )
  {
    a = 2*j+2+aa+bb;
    b = (aa+bb)*(a-1)*(aa-bb);
    double c = 2*(j+1)*(a-2)*(j+1+aa+bb);
    u[j] = b/c;
    v[j] = a*(a-1)*(a-2)/c;
    a = 2*(j+aa)*(j+bb)*a;
    w[j] = a/c;
  }
  p[0] = u[0] + v[0]*x;
  p[1] = (u[1]+v[1]*x)*p[0]-w[1];
  for( int j=3; j<=n; ++j )p[j-1] = (u[j-1]+v[j-1]*x)*p[j-2]-w[j-1]*p[j-3];
  return p[n-1];
}

double PoissonCharlierPolynomial( double a, int n, double x )
{
  if( n < 0 )
    throw EExpressionError(wxT("PoissonCharlierPolynomial: polynomial order must be >= 0"));
  double s;
  n == n/2*2 ? s=1.0 : s=-1.0;
  double uu = s;
  double c = 1.0;
  for( int j=0; j<n; ++j )
  {
    uu *= (j-n)*(x-j)/(a*(j+1));
    s += uu;
    c *= a/(j+1);
  }
  return sqrt(c)*s;
}

double LaguerrePolynomial( int n, double x )
{
  if( n < 0 )throw EExpressionError(wxT("LagurrePolynomial: polynomial order must be >= 0"));
  double a = 1.0;
  double b = 1.0-x;
  if( n == 0 )return a/Factorial(n);
  if( n == 1 )return b/Factorial(n);
  double c;
  for( int i=1; i<n; ++i )
  {
    c = (1+2*i-x)*b - (i*i*a);
    a = b;
    b = c;
  }
  return c/Factorial(n);
}

double LegendrePolynomial( int n, double x )
{
  if( n < 0 )throw EExpressionError(wxT("LegendrePolynomial: polynomial order must be >= 0"));
  double a = 1.0;
  double b = x;
  if( n == 0 )return a;
  if( n == 1 )return b;
  double c;
  for( int i=1; i<n; ++i )
  {
    c = b*x + (i/(i+1.0))*(x*b-a);
    a = b;
    b = c;
  }
  return c;
}

double UnnormalizedLegendre( int l, int m, double y )
{
  // UNNORMALIZED LEGENDRE FUNCTION
  // plmu
  //
  // NB. THE ASSOCIATED LEGENDRE FUNCTIONS ARE ALSO KNOWN AS THE SPHERICAL HARMONICS
  //     WHEN M=0, THEY ARE THE LEGENDRE POLYNOMIALS, OR THE LEGENDRE FUNCTIONS OF
  //     THE FIRST KIND ( OF DEGREE L )
  // REFERENCE: CERN library routine C315 (old) C330 (new)
  // ARGUMENT RESTRICTIONS:
  //  L ----- POSITIVE INTEGER OR ZERO, N.B. L is integral by default
  //  M ----- -L <= M <= L; if |M|>L, return 0 is correct
  //  Y ----- -1 <= Y <= 1
  // ACCURACY: with L=2,10; M=0; using PLMU
  //       i) 0<=Y<=0.1,  ASLEGF is accurate to at least 1.0E-6
  //      ii) 0.2<=Y<=1.0, 6 significant figures are correct
  //
  if( l<0 || l!=static_cast<int>(l) )
    throw EExpressionError(wxT("UnnormalizedLegendre: first argument must be a nonnegative integer"));
  if( fabs(y) > 1.0 )
    throw EExpressionError(wxT("UnnormalizedLegendre: third argument must be between -1 and 1"));
  //
  if( l < abs(m) )return 0.0;
  //
  double prod, fact=1.0;
  if( m < 0 )
  {
    prod = 1.0;
    for( int i=l-abs(m)+1; i<=l+abs(m); ++i )prod *= i;
    fact = 1.0/prod;
  }
  double result;
  if( abs(m) > 1 )
  {
    double z2 = 1.0 - y*y;
    double zham = pow(z2,abs(m)/2.0);
    double dnn;
    if( abs(m) != l-1 )
    {
      prod = 1.0;
      for( int i=l+1; i<=2*l; ++i )prod *= i;
      dnn = prod/pow(2.0,static_cast<double>(l));
      if( abs(m) == l )result = dnn*zham*fact;
    }
    else
    {
      prod = 1.0;
      for( int i=l; i<=2*l-1; ++i )prod *= i;
      double dnm1n = y*prod/pow(2.0,static_cast<double>(l-1));
      if( abs(m) == l-1 )
      {
        result = dnm1n*zham*fact;
      }
      else
      {
        double dnm2n;
        for( int i=1; i<=l-1-abs(m); ++i )
        {
          dnm2n = 1.0/((1+i)*(2*l-i))*(2*(l-i)*y*dnm1n-z2*dnn);
          dnn = dnm1n;
          dnm1n = dnm2n;
        }
        result = dnm2n*zham*fact;
      }
    }
  }
  else
  {
    double p0=1.0, p1=y;
    if( abs(m) != 0 )
    {
      p0 = 0.0;
      p1 = sqrt(1.0-y*y);
    }
    if( l < 1 )result = p0;
    else if( l == 1 )result = p1*fact;
    else
    {
      double pnmn1 = p0;
      double pn = p1;
      double pnpl1;
      for( int i=1; i<=l-1; ++i )
      {
        pnpl1 = (1.0/(i-abs(m)+1.0))*((2*i+1)*y*pn-(i+abs(m))*pnmn1);
        pnmn1 = pn;
        pn = pnpl1;
      }
      result = pnpl1*fact;
    }
  }
  return result;
}

double NormalizedLegendre( int l, int m, double y )
{
  // NORMALIZED LEGENDRE FUNCTION
  // plmn
  //
  // NB. THE ASSOCIATED LEGENDRE FUNCTIONS ARE ALSO KNOWN AS THE SPHERICAL HARMONICS
  //     WHEN M=0, THEY ARE THE LEGENDRE POLYNOMIALS, OR THE LEGENDRE FUNCTIONS OF
  //     THE FIRST KIND ( OF DEGREE L )
  // REFERENCE: CERN library routine C315 (old) C330 (new)
  // ARGUMENT RESTRICTIONS:
  //  L ----- POSITIVE INTEGER OR ZERO, N.B. L is integral by default
  //  M ----- -L <= M <= L; if |M|>L, return 0 is correct
  //  Y ----- -1 <= Y <= 1
  // ACCURACY: with L=2,10; M=0; using PLMU
  //       i) 0<=Y<=0.1,  ASLEGF is accurate to at least 1.0E-6
  //      ii) 0.2<=Y<=1.0, 6 significant figures are correct
  //
  if( l<0 || l!=static_cast<int>(l) )
    throw EExpressionError(wxT("NormalizedLegendre: first argument must be a nonnegative integer"));
  if( fabs(y) > 1.0 )
    throw EExpressionError(wxT("NormalizedLegendre: third argument must be between -1 and 1"));
  double fact = 0.5*(2*l+1);
  if( m != 0 )
  {
    double prod = 1.0;
    for( int i=l-abs(m)+1; i<=l+abs(m); ++i )prod *= i;
    fact *= pow(prod,static_cast<double>(-m/abs(m)));
  }
  return UnnormalizedLegendre(l,m,y)*sqrt(fact);
}

double Rademacher( int k, double x )
{
  // RADEMACHER FUNCTION
  // radmac
  //
  // RADMAC(K,X) = SIGN(SIN(2*pi*2^K*X))
  // The RADEMACHER functions form an incomplete set of orthogonal,
  // normalized,periodic wave functions with period of one.
  // K=non-negative integer
  // NB.  RADMAC  can assume values of 1 or -1 only.
  // REFERENCE: CACM ALGORITHMS # 388
  //
  double y = x-static_cast<int>(x);
  double r = static_cast<int>(y*pow(2.0,static_cast<double>(k+1)));
  double result;
  r/2.0 == static_cast<double>(static_cast<int>(r/2.0)) ? result=1.0 : result=-1.0;
  return result;
}

double BinaryOrderedWalsh( int k, double x )
{
  // BINARY ORDERED WALSH FUNCTION
  // walsh
  //
  // Let K be a binary number, i.e.,
  //   K = a(0)*2^0 + a(1)*2^1 + a(2)*2^2 + a(3)*2^3 + ...  with a(i) = 0 or 1, i=0,1,2,...
  // then WALSH(K,X)= (RADMAC(0,X)^a(0)) * (RADMAC(1,X)^a(1)) * ...
  // Again, the WALSH functions form a complete set of orthogonal,
  // normalized rectangular periodic functions with period of one.
  // where K=non-negative integer, X is real
  // NB. WALSH can assume values of 1 or -1 only.
  //
  // REFERENCE: CACM ALGORITHMS # 389
  //
  int const j = k;
  int m = 1;
  int mw = 1;
  double result = 1.0;
  int i = -1;
  while( m <= j )
  {
    ++i;
    if( static_cast<int>(k/(2.0*m)) != k/(2*m) )
    {
      mw *= static_cast<int>(Rademacher(i,x));
      k -= m;
    }
    result = mw;
    m *= 2;
  }
  return result;
}

double VoigtProfile( double e, double e1, double eps, double sig )
{
  // Calculation of voigt profile function
  // adapted from complex error fn algorithm
  //
  double rt2 = 1.0/(sig*sqrt(2.0));
  if( eps*rt2 <= 0.0 )throw EExpressionError(wxT("VoigtProfile: Lorentzian width <= 0"));
  double x = fabs((e1-e)*rt2);
  double y = eps*rt2;
  double h, lambda;
  int nc, nu;
  bool b;
  if( y>=4.29 || x>=5.33 )
  {
    h = 0.0;
    nc = 0;
    nu = 8;
    lambda = 0.0;
    b = true;
  }
  else
  {
    double s = (1.0-y/4.29)*sqrt(1.0-x*x/28.41);
    h = 1.6*s;
    nc = 6+static_cast<int>(23.0*s);
    nu = 9+static_cast<int>(21.0*s);
    lambda = pow(2.*h,nc);
    b = (lambda==0.0);
  }
  double r1 = 0.0;
  double r2 = 0.0;
  double s1 = 0.0;
  double s2 = 0.0;
  do
  {
    double t1 = y+h+(nu+1)*r1;
    double t2 = x-(nu+1)*r2;
    double c = 0.5/(t1*t1+t2*t2);
    r1 = c*t1;
    r2 = c*t2;
    if( h>0.0 && nu<=nc )
    {
      t1 = lambda+s1;
      s1 = r1*t1-r2*s2;
      s2 = r2*t1+r1*s2;
      lambda /= 2.*h;
    }
  }
  while ( --nu >= 0 );
  double rs1;
  b ? rs1=r1 : rs1=s1;
  double const c1 = 1.12837916709551;
  return c1*rt2*rs1*M_PI/y;
}

double Struve0( double x )
{
  // STRH0 IS THE STRUVE FUNCTION OF ORDER ZERO H<0>(X)
  // strh0
  //
  // REFERENCE : CERN library routine C342
  // RECURRENCE RELATION:
  // H<n+1> = H<n-1> + 2*n/x*H<n> + (0.5*x)^n/(sqrt(pi)*gamma(n+3/2))
  //     gamma(x) is the complete gamma function
  // ACCURACY : 6 TO 7 SIGNIFICANT FIGURES ARE CORRECT
  //
  double v = fabs(x);
  double y, a, b, result;
  if( v >= 8.0 )
  {
    y = 256.0/x*x-2.0;
    b =      +0.000000000000001;
    a = y*b  -0.000000000000002;
    b = y*a-b+0.000000000000004;
    a = y*b-a-0.000000000000012;
    b = y*a-b+0.000000000000034;
    a = y*b-a-0.000000000000099;
    b = y*a-b+0.000000000000298;
    a = y*b-a-0.000000000000942;
    b = y*a-b+0.000000000003120;
    a = y*b-a-0.000000000010915;
    b = y*a-b+0.000000000040657;
    a = y*b-a-0.000000000162887;
    b = y*a-b+0.000000000711191;
    a = y*b-a-0.000000003443582;
    b = y*a-b+0.000000018940833;
    a = y*b-a-0.000000122506454;
    b = y*a-b+0.000000981982943;
    a = y*b-a-0.000010632582528;
    b = y*a-b+0.000182051037870;
    a = y*b-a-0.006968912811386;
    a = y*a-b+1.985674551528478;
    result = 0.318309886183791*(a-b)/v + BesselY0(v);
    if( x < 0.0 )result = -result;
  }
  else
  {
    y = 0.0625*x*x-2.0;
    b =      +0.000000000000012;
    a = y*b  -0.000000000000589;
    b = y*a-b+0.000000000025793;
    a = y*b-a-0.000000000963266;
    b = y*a-b+0.000000030215932;
    a = y*b-a-0.000000782444085;
    b = y*a-b+0.000016374616926;
    a = y*b-a-0.000269650143126;
    b = y*a-b+0.003375614473752;
    a = y*b-a-0.030670520229880;
    b = y*a-b+0.189553273710931;
    a = y*b-a-0.724851153021219;
    b = y*a-b+1.502369396182928;
    a = y*b-a-1.639692926813091;
    a = y*a-b+2.004316912198240;
    result = 0.0625*(a-b)*x;
  }
  return result;
}

double Struve1( double x )
{
  // THE STRUVE FUNCTION OF ORDER ONE H<1>(x)
  // strh1
  //
  // REFERENCE : CERN library routine C342
  // ACCURACY : 6 TO 7 SIGNIFICANT FIGURES
  //
  double v = fabs(x);
  double y, a, b, result;
  if( v >= 8.0 )
  {
    y = 256.0/x*x-2.0;
    a =      +0.000000000000001;
    b = y*a  -0.000000000000002;
    a = y*b-a+0.000000000000007;
    b = y*a-b-0.000000000000023;
    a = y*b-a+0.000000000000076;
    b = y*a-b-0.000000000000266;
    a = y*b-a+0.000000000000985;
    b = y*a-b-0.000000000003907;
    a = y*b-a+0.000000000016800;
    b = y*a-b-0.000000000079552;
    a = y*b-a+0.000000000423627;
    b = y*a-b-0.000000002612620;
    a = y*b-a+0.000000019490150;
    b = y*a-b-0.000000188411578;
    a = y*b-a+0.000002662053934;
    b = y*a-b-0.000070439332645;
    a = y*b-a+0.007503160512483;
    a = y*a-b+2.015152945877312;
    result = 0.318309886183791*(a-b) + BesselY1(v);
  }
  else
  {
    y = 0.0625*x*x-2.0;
    b =      +0.000000000000003;
    a = y*b  -0.000000000000168;
    b = y*a-b+0.000000000007960;
    a = y*b-a-0.000000000323185;
    b = y*a-b+0.000000011111916;
    a = y*b-a-0.000000318580129;
    b = y*a-b+0.000007475536317;
    a = y*b-a-0.000140316473679;
    b = y*a-b+0.002047039876366;
    a = y*b-a-0.022368648962794;
    b = y*a-b+0.174397354024184;
    a = y*b-a-0.909691353775337;
    b = y*a-b+2.935262636422712;
    a = y*b-a-5.614352244078113;
    a = y*a-b+7.845908822670735;
    result = 0.0078125*(a-b)*x*x;
  }
  return result;
}

double Hypergeometric2F1( double a, double b, double c, double x )
{
  // THE HYPERGEOMETRIC FUNCTION 2F1(a,b,c,x)
  // hypgeo
  //
  // REFERENCE: CACM ALGORITHM #191
  //
  double s = 1.0;
  double y = 1.0;
  for( int i=0; i<=100; ++i )
  {
    y *= ((a+i)/(c+i))*(b+i)*x/(i+1);
    if( y == 0.0 )break;
    s += y;
  }
  return s;
}

double LogarithmicConfluentHypergeometric( double a, double b, double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5N
  // CALCULATE THE  LOGARITHMIC CONFLUENT HYPERGEOMETRIC FUNCTION.
  // chlogu
  //
  // THIS ROUTINE IS NOT VALID WHEN 1+A-B IS CLOSE TO ZERO IF X IS SMALL.
  //
  // SPECIAL NOTE:
  // THE ALGORITHM PRODUCES AN ARITHMETIC FAULT WHEN A IS A NEGATIVE
  // INTEGER AND X EXCEEDS A CUTOFF POINT, WHERE X<cutoff>  ~= A*(1+A-B).
  // NO TEST IS PROVIDED FOR THIS CONDITION.
  //
  double const eps = std::numeric_limits<double>::epsilon();
  if( x == 0.0 )
   throw EExpressionError(wxT("LogarithmicConfluentHypergeometric: third argument = 0") );
  if( x < 0.0 )
   throw EExpressionError(wxT("LogarithmicConfluentHypergeometric: third argument < 0") );
  //
  // when a and b are negative integers with a less than b, a simple pole
  //
  if( a == -1.0 )return x-b;
  //
  if( std::max(fabs(a),1.0)*std::max(fabs(1.0+a-b),1.0) < 0.99*fabs(x) )
  {
    // use luke-s rational approximation in the asymptotic region
    //
    return pow(x,-a)*Chu(a,b,x);
  }
  //
  // the ascending series will be used, because the descending rational
  // approximation (which is based on the asymptotic series) is unstable.
  //
  if( fabs(1.0+a-b) < sqrt(eps) )throw EExpressionError(
   wxT("LogarithmicConfluentHypergeometric: algorithm is bad when 1+a-b is near zero f or small x"));
  //
  int aintb;
  b >= 0.0 ? aintb=static_cast<int>(b+0.5) : aintb=static_cast<int>(b-0.5);
  double beps = b - aintb;
  int n = aintb;
  double alnx = log(x);
  double xtoeps = exp(-beps*alnx);
  //
  // evaluate the finite sum
  //
  double sum;
  if( n < 1 )
  {
    // consider the case b < 1 first
    //
    sum = 1.0;
    if( n != 0 )
    {
      double t = 1.0;
      for( int i=1; i<=-n; ++i )
      {
        t *= (a+i-1)*x/((b+i-1)*i);
        sum += t;
      }
    }
    sum *= PochhammerSSymbol(1.0+a-b,-a);
  }
  else // now consider the case b >= 1
  {
    sum = 0.0;
    if( n >= 2 )
    {
      double t = 1.0;
      sum = 1.0;
      for( int i=1; i<=n-2; ++i )
      {
        t *= (a-b+i)*x/((1-b+i)*i);
        sum += t;
      }
      sum *= CompleteGamma(b-1.0)*ReciprocalGamma(a)*pow(x,static_cast<double>(1-n))*xtoeps;
    }
  }
  //
  // next evaluate the infinite sum
  //
  int istrt;
  n < 1 ? istrt=1-n : istrt=0;
  double factor = pow(-1.0,static_cast<double>(n))*ReciprocalGamma(1+a-b)*
      pow(x,static_cast<double>(istrt));
  if( beps != 0.0 )factor *= beps*M_PI/sin(beps*M_PI);
  double pochai = PochhammerSSymbol(a,istrt);
  double gamri1 = ReciprocalGamma(istrt+1.0);
  double gamrni = ReciprocalGamma(aintb+istrt);
  double b0 = factor*PochhammerSSymbol(a,istrt-beps)*gamrni*ReciprocalGamma(istrt+1-beps);
  double chlogu;
  bool flag;
  if( fabs(xtoeps-1.0) <= 0.5 )
  {
    // x^(-beps) is close to 1, so we must be careful in evaluating the differences
    //
    double pch1ai = PochhammerSSpecial(a+istrt,-beps);
    double pch1i = PochhammerSSpecial(istrt+1-beps,beps);
    double c0 = factor*pochai*gamrni*gamri1*(-PochhammerSSpecial(b+istrt,-beps)+pch1ai-
                                             pch1i+beps*pch1ai*pch1i);
    //
    double xeps1 = alnx*RelativeErrorExponential(-beps*alnx);
    chlogu = sum + c0 + xeps1*b0;
    flag = false;
    for( int i=1; i<=1000; ++i )
    {
      b0 = (a+istrt+i-1-beps)*b0*x/((n+istrt+i-1)*(istrt+i-beps));
      c0 = (a+istrt+i-1)*c0*x/((b+istrt+i-1)*(istrt+i)) -
           ((a-1.0)*(n+2.0*(istrt+i)-1.0)+(istrt+i)*(istrt+i-beps))*b0/
            ((istrt+i)*(b+istrt+i-1)*(a+istrt+i-1-beps));
      double t = c0 + xeps1*b0;
      chlogu += t;
      if( fabs(t) < eps*fabs(chlogu) )
      {
        flag = true;
        break;
      }
    }
    if( !flag )
     throw EExpressionError(wxT("LogarithmicConfluentHypergeometric: no convergence in 1000 terms of the ascending series") );
  }
  //
  // x^(-beps) is very different from 1, so the straightforward formulation is stable
  //
  double a0 = factor*pochai*ReciprocalGamma(b+istrt)*gamri1/beps;
  b0 = xtoeps * b0 / beps;
  //
  chlogu = sum + a0 - b0;
  flag = false;
  for( int i=1; i<=1000; ++i )
  {
    a0 = (a+istrt+i-1)*a0*x/((b+istrt+i-1)*(istrt+i));
    b0 = (a+istrt+i-1-beps)*b0*x/((aintb+istrt+i-1)*(istrt+i-beps));
    double t = a0 - b0;
    chlogu += t;
    if( fabs(t) < eps*fabs(chlogu) )
    {
      flag = true;
      break;
    }
  }
  if( !flag )
   throw EExpressionError(wxT("LogarithmicConfluentHypergeometric: no convergence in 1000 terms of the ascending series") );
  //
  // use luke-s rational approximation in the asymptotic region
  //
  chlogu = pow(x,-a)*Chu(a,b,x);
  return chlogu;
}

double PochhammerSSpecial( double a, double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5Z
  // gamra1
  //
  // CALCULATES A GENERALIZATION OF POCHHAMMER-S SYMBOL STARTING FROM
  // FIRST ORDER. i.e. EVALUATE A GENERALIZATION OF POCHHAMMER-S SYMBOL FOR SPECIAL
  // SITUATIONS THAT REQUIRE ESPECIALLY ACCURATE VALUES WHEN X IS SMALL IN
  // GAMRA1(A,X) = (POCH(A,X)-1)/X
  //             = (GAMMA(A+X)/GAMMA(A) - 1.0)/X .
  // THIS SPECIFICATION IS PARTICULARLY SUITED FOR STABLY COMPUTING EXPRESSIONS SUCH AS
  // (GAMMA(A+X)/GAMMA(A) - GAMMA(B+X)/GAMMA(B))/X = GAMRA1(A,X) - GAMRA1(B,X)
  // NOTE THAT GAMRA1(A,0.0) = PSI(A)
  //
  // WHEN |X| IS SO SMALL THAT SUBSTANTIAL CANCELLATION WILL OCCUR IF
  // THE STRAIGHTFORWARD FORMULA IS USED, WE  USE AN EXPANSION DUE
  // TO FIELDS AND DISCUSSED BY Y. L. LUKE, THE SPECIAL FUNCTIONS AND THEIR
  // APPROXIMATIONS, VOL. 1, ACADEMIC PRESS, 1969, PAGE 34.
  //
  // THE RATIO POCH(A,X) = GAMMA(A+X)/GAMMA(A) IS WRITTEN BY LUKE AS
  // (A+(X-1)/2)**X * POLYNOMIAL IN (A+(X-1)/2)**(-2) .
  // IN ORDER TO MAINTAIN SIGNIFICANCE IN POCH1, WE WRITE FOR POSITIVE+ A
  // (A+(X-1)/2)**X = EXP(X*ALOG(A+(X-1)/2)) = EXP(Q)
  //                = 1.0 + Q*EXPREL(Q) .
  // LIKEWISE THE POLYNOMIAL IS WRITTEN POLY = 1.0 + X*POLY1(A,X) .
  // THUS,  GAMRA1(A,X) = (GAMRA(A,X) - 1) / X
  //                    = EXPREL(Q)*(Q/X + Q*POLY1(A,X)) + POLY1(A,X)
  //
  // BERN(I) IS THE 2*I BERNOULLI NUMBER DIVIDED BY FACTORIAL(2*I).
  //
  if( x == 0.0 )return Psi(a);
  if( fabs(x)>0.1*fabs(a) || fabs(x)*log(std::max(fabs(a),2.0))>0.1 )
    return (PochhammerSSymbol(a,x)-1.0)/x;
  //
  double const bern[20] = {
    +.833333333333333333333333333333333e-1,  -.138888888888888888888888888888888e-2,
    +.330687830687830687830687830687830e-4,  -.826719576719576719576719576719576e-6,
    +.208767569878680989792100903212014e-7,  -.528419013868749318484768220217955e-9,
    +.133825365306846788328269809751291e-10, -.338968029632258286683019539124944e-12,
    +.858606205627784456413590545042562e-14, -.217486869855806187304151642386591e-15,
    +.550900282836022951520265260890225e-17, -.139544646858125233407076862640635e-18,
    +.353470703962946747169322997780379e-20, -.895351742703754685040261131811274e-22,
    +.226795245233768306031095073886816e-23, -.574472439520264523834847971943400e-24,
    +.145517247561486490186626486727132e-26, -.368599494066531017818178247990866e-28,
    +.933673425709504467203255515278562e-30, -.236502241570062993455963519636983e-31};
  double const sqtbig = 1.0/sqrt(24.0*std::numeric_limits<double>::min());
  double const alneps = log(std::numeric_limits<double>::epsilon());
  //
  double bp;
  a < -0.5 ? bp=1.0-a-x : bp=a;
  int incr;
  incr = bp<10.0 ? static_cast<int>(11.0-bp) : 0;
  double b = bp + incr;
  double var = b + 0.5*(x-1.0);
  double alnvar = log(var);
  double q = x*alnvar;
  double poly1 = 0.0;
  if( var < sqtbig )
  {
    double var2 = 1.0/var/var;
    //
    double rho = 0.5*(x+1.0);
    double gbern[21];
    gbern[0] = 1.0;
    gbern[1] = -rho/12.0;
    double term = var2;
    poly1 = gbern[1]*term;
    int nterms = static_cast<int>(-0.5*alneps/alnvar) + 1;
    if( nterms > 20 )throw EExpressionError(wxT("PochhammerSSpecial: nterms is too big"));
    if( nterms >= 2 )
    {
      for( int k=2; k<=nterms; ++k )
      {
        double gbk = 0.0;
        for( int j=1; j<=k; ++j )gbk += bern[k-j]*gbern[j-1];
        gbern[k] = -rho*gbk/k;
        term *= (2*k-2-x)*(2*k-1-x)*var2;
        poly1 += gbern[k]*term;
      }
    }
  }
  poly1 *= x - 1.0;
  double result = RelativeErrorExponential(q)*(alnvar+q*poly1) + poly1;
  if( incr != 0 )
  {
    // we have gamra1(b,x), but bp is small, so we use backwards recursion
    // to obtain gamra1(bp,x)
    //
    for( int i=1; i<=incr; ++i )
    {
      result -= 1.0/(bp+incr-i);
      result /= 1.0+x/(bp+incr-i);
    }
  }
  if( bp == a )return result;
  //
  // we have gamra1(bp,x), but a is lt -0.5.  we therefore use a reflection
  // formula to obtain gamra1(a,x)
  //
  double sinpxx = sin(M_PI*x)/x;
  double sinpx2 = sin(0.5*M_PI*x);
  double trig = sinpxx/tan(M_PI*b) - 2.0*sinpx2*(sinpx2/x);
  result *= 1.0+x*trig;
  result += trig;
  return result;
}

double RelativeErrorExponential( double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B3
  // dexprl
  //
  // CALCULATES THE  RELATIVE ERROR EXPONENTIAL  (DEXP(X)-1)/X. i.e.
  // EVALUATE  EXPREL(X) = (EXP(X) - 1.0) / X.   FOR SMALL ABS(X) THE
  // TAYLOR SERIES IS USED.  IF X IS NEGATIVE THE REFLECTION FORMULA
  //       EXPREL(X) = EXP(X) * EXPREL(ABS(X))
  // MAY BE USED  THIS REFLECTION FORMULA WILL BE OF USE WHEN THE
  // EVALUATION FOR SMALL ABS(X) IS DONE BY CHEBYSHEV SERIES RATHER THAN
  // TAYLOR SERIES.
  //
  double const xbnd = std::numeric_limits<double>::epsilon();
  if( fabs(x) > 0.5 )return (exp(x)-1.0)/x;
  if( fabs(x) < xbnd )return 1.0;
  //
  double const alneps = log(std::numeric_limits<double>::epsilon());
  double const xn = 3.72 - 0.3*alneps;
  double const xln = log((xn+1.0)/1.36);
  int nterms = static_cast<int>(xn-(xn*xln+alneps)/(xln+1.36)+1.5);
  double result = 0.0;
  for( int i=1; i<=nterms; ++i )result = 1.0+result*x/(nterms+2-i);
  return result;
}

double Chu( double a, double b, double z )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5N
  // d9chu
  //
  // EVALUATE FOR LARGE Z  Z**A * U(A,B,Z)  WHERE U IS THE LOGARITHMIC
  // CONFLUENT HYPERGEOMETRIC FUNCTION.  A RATIONAL APPROXIMATION DUE TO Y
  // L. LUKE IS USED. WHEN U IS NOT IN THE ASYMPTOTIC REGION, I.E., WHEN A
  // OR B IS LARGE COMPARED WITH Z, CONSIDERABLE SIGNIFICANCE LOSS OCCURS.
  // A WARNING IS PROVIDED WHEN THE COMPUTED RESULT IS LESS THAN HALF PRECISION
  //
  double eps = 8*std::numeric_limits<double>::epsilon();
  double sqeps = sqrt(2*std::numeric_limits<double>::epsilon());
  double bp = 1.0 + a - b;
  double ab = a*bp;
  double ct2 = 2.0*(z-ab);
  double sab = a + bp;
  //
  double bb[4], aa[4];
  bb[0] = 1.0;
  aa[0] = 1.0;
  double ct3 = sab + 1.0 + ab;
  bb[1] = 1.0 + 2.0*z/ct3;
  aa[1] = 1.0 + ct2/ct3;
  double anbn = ct3 + sab + 3.0;
  double ct1 = 1.0 + 2.0*z/anbn;
  bb[2] = 1.0 + 6.0*ct1*z/ct3;
  aa[2] = 1.0 + 6.0*ab/anbn + 3.0*ct1*ct2/ct3;
  //
  bool flag = false;
  for( int i=4; i<=300; ++i )
  {
    double x2i1 = 2*i - 3;
    double ct1 = x2i1/(x2i1-2.0);
    anbn += x2i1 + sab;
    ct2 = (x2i1 - 1.0)/anbn;
    double c2 = x2i1*ct2 - 1.0;
    double d1z = x2i1*2.0*z/anbn;
    ct3 = sab*ct2;
    double g1 = d1z + ct1*(c2+ct3);
    double g2 = d1z - c2;
    double g3 = ct1*(1.0 - ct3 - 2.0*ct2);
    bb[3] = g1*bb[2] + g2*bb[1] + g3*bb[0];
    aa[3] = g1*aa[2] + g2*aa[1] + g3*aa[0];
    if( fabs(aa[3]*bb[0]-aa[0]*bb[3]) < eps*fabs(bb[3]*bb[0]))
    {
      flag = true;
      break;
    }
    //
    // if overflows or underflows prove to be a problem, the statements
    // below could be altered to incorporate a dynamically adjusted scale factor
    //
    for( int j=0; j<3; ++j )
    {
      aa[j] = aa[j+1];
      bb[j] = bb[j+1];
    }
  }
  if( !flag )
   throw EExpressionError(wxT("LogarithmicConfluentHypergeometric: no convergence in 300 terms") );
  double chu = aa[3]/bb[3];
  if( chu<sqeps || chu>1.0/sqeps )
   throw EExpressionError(wxT("LogarithmicConfluentHypergeometric: answer < half precision") );
  return chu;
}

double PochhammerSSymbol( double a, double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5Z
  // gamra
  //
  // EVALUATE A GENERALIZATION OF POCHHAMMER-S SYMBOL
  // (A)-SUB-X = GAMMA(A+X)/GAMMA(A).  FOR X A NON-NEGATIVE INTEGER,
  // GAMRA(A,X) IS JUST POCHHAMMER-S SYMBOL.
  //
  // ERROR HANDLING WHEN GAMRA (A, X) IS LESS THAN HALF PRECISION IS
  // PROBABLY INCORRECT.  GROSSLY WRONG ARGUMENTS ARE NOT HANDLED RIGHT.
  //
  if( a+x<=0.0 && static_cast<double>(static_cast<int>(a+x))==a+x )
  {
    if( a>0.0 || static_cast<double>(static_cast<int>(a))!=a )
     throw EExpressionError(wxT("PochhammerSSymbol: a+x is non-positive integer but a is not"));
    //
    // we know here that both a+x and a are non-positive integers
    //
    if( x == 0.0 )return 1.0;
    //
    int n = static_cast<int>(x);
    if( std::min(a+x,a) < -20.0 )
    {
      return pow(-1.0,static_cast<double>(n))*exp((a-0.5)*LnRel(x/(a-1.0))+x*log(-a+1.0-x)-x+
                                                  LogGammaCorrection(-a+1.0)-
                                                  LogGammaCorrection(-a-x+1));
    }
    int ia = static_cast<int>(a);
    return pow(-1.0,static_cast<double>(n))*Factorial(-ia)/Factorial(-ia-n);
  }
  //
  // a+x is not zero or a negative integer
  //
  if( a<=0.0 && static_cast<double>(static_cast<int>(a))==a )return 0.0;
  //
  int n = static_cast<int>(fabs(x));
  if( static_cast<double>(n)==x && n<=20 )
  {
    // x is a small non-positive integer, presummably a common case
    //
    double gamra = 1.0;
    for( int i=1; i<=n; ++i )gamra *= a+i-1;
    return gamra;
  }
  double absax = fabs(a+x);
  double absa = fabs(a);
  if( std::max(absax,absa) <= 20.0 )return CompleteGamma(a+x)*ReciprocalGamma(a);
  if( fabs(x) > 0.5*absa )
  {
    double alnga = LnAbsGamma(a);
    double sgnga;
    a<=0.0 && -static_cast<int>(a)%2+0.1==0 ? sgnga=-1.0 : sgnga=1.0;
    double alngax = LnAbsGamma(a+x);
    double sgngax;
    a+x<=0.0 && -static_cast<int>(a+x)%2+0.1==0 ? sgngax=-1.0 : sgngax=1.0;
    return sgngax*sgnga*exp(alngax-alnga);
  }
  //
  // |x| is small and both |a+x| and |a| are large.  thus,
  // a+x and a must have the same sign.  for negative a, we use
  // gamma(a+x)/gamma(a) = gamma(-a+1)/gamma(-a-x+1) * sin(pi*a)/sin(pi*(a+x))
  //
  double b = a;
  if( b < 0.0 )b = -a - x + 1.0;
  double gamra = exp((b-0.5)*LnRel(x/b)+x*log(b+x)-x+
                     LogGammaCorrection(b+x)-LogGammaCorrection(b));
  if( a<0.0 && gamra!=0.0 )gamra /= cos(M_PI*x)+sin(M_PI*x)/tan(M_PI*a);
  return gamra;
}

double BivariateNormalDistribution( double ah, double ak, double r )
{
  // computes the  bivariate normal distribution of upper right area,
  // i.e., the probability for two normal variables x and y whose correlation is r,
  // that x > ah and y > ak.
  // bivnor
  //
  // reference: cacm algorithm #462
  //
  int idig = 15; // the desired number of significant figures right of
                 // the decimal point, if within machine's capacity
  double b = 0.0;
  double gh = Gauss(-ah)/2.0;
  double gk = Gauss(-ak)/2.0;
  if( r == 0.0 )
  {
    b = 4.0*gh*gk;
    if( b < 0.0 )b = 0.0;
    else if( b > 1.0 )b = 1.0;
    return b;
  }
  double rr = 1.0 - r*r;
  if( rr < 0.0 )
  {
    throw EExpressionError(
     wxT("BivariateNormalDistribution: correlation r is illegal, |r|>1") );
  }
  else if( rr == 0.0 )
  {
    if( r >= 0.0 )
    {
      ah >= ak ? b=2.0*gh : b=2.0*gk;
    }
    else
    {
      if( ah < -ak )b = 2.0*(gh+gk) - 1.0;
    }
    if( b < 0.0 )b = 0.0;
    else if( b > 1.0 )b = 1.0;
    return b;
  }
  double sqr = sqrt(rr);
  double con = 2.*M_PI - 7.5;
  if( idig != 15 )
  {
    con = M_PI;
    for( int i=1; i<=idig; ++i )con /= 10.0;
  }
  if( ah==0.0 && ak==0.0 )
  {
    b = atan(r/sqr)/2.0/M_PI + 0.25;
    if( b < 0.0 )b = 0.0;
    else if( b > 1.0 )b = 1.0;
    return b;
  }
  double wh, wk, gw;
  int is;
  if( ah != 0.0 )
  {
    b = gh;
    if( ah+ak != 0.0 )
    {
      if( ah+ak < 0.0 )b -= 0.5;
      b += gk;
    }
    wh = -ah;
    wk = (ak/ah-r)/sqr;
    gw = 2.0*gh;
    is = -1;
  }
  else
  {
    b += gk;
    wh = -ak;
    wk = (ah/ak-r)/sqr;
    gw = 2.0*gk;
    is = 1;
  }
  for( ;; )
  {
    double sgn = -1.0;
    double t;
    if( wk == 0.0 )
    {
      if( is>=0 || ak==0.0 )
      {
        if( b < 0.0 )b = 0.0;
        else if( b > 1.0 )b = 1.0;
        return b;
      }
      wh = -ak;
      wk = (ah/ak-r)/sqr;
      gw = 2.0*gk;
      is = 1;
    }
    else if( fabs(wk) < 1.0 )
    {
      double h2 = wh*wh;
      double a2 = wk*wk;
      double h4 = h2/2.0;
      double ex = exp(-h4);
      double w2 = h4*ex;
      double ap = 1.0;
      double s2 = ap-ex;
      double sp = ap;
      double s1 = 0.0;
      double sn = s1;
      double conex = fabs(con/wk);
      bool flag = true;
      while( flag )
      {
        flag = false;
        double cn = ap*s2/(sn+sp);
        s1 += cn;
        if( fabs(cn) > conex )
        {
          sn = sp;
          sp += 1.0;
          s2 -= w2;
          w2 *= h4/sp;
          ap = -ap*a2;
          flag = true;
        }
      }
      t = (atan(wk)-wk*s1)/2./M_PI;
      b += sgn*t;
      if( is>=0 || ak==0.0 )
      {
        if( b < 0.0 )b = 0.0;
        else if( b > 1.0 )b = 1.0;
        return b;
      }
      wh = -ak;
      wk = (ah/ak-r)/sqr;
      gw = 2.0*gk;
      is = 1;
    }
    else if( fabs(wk) == 1.0 )
    {
      t = wk*gw*(1.0-gw)/2.0;
      b += sgn*t;
      if( is>=0 || ak==0.0 )
      {
        if( b < 0.0 )b = 0.0;
        else if( b > 1.0 )b = 1.0;
        return b;
      }
      wh = -ak;
      wk = (ah/ak-r)/sqr;
      gw = 2.0*gk;
      is = 1;
    }
    else
    {
      sgn = -sgn;
      wh *= wk;
      double g2 = Gauss(wh);
      wk = 1.0/wk;
      if( wk < 0.0 )b += 0.5;
      b += -(gw+g2)/2.0+gw*g2;
      double h2 = wh*wh;
      double a2 = wk*wk;
      double h4 = h2/2.0;
      double ex = exp(-h4);
      double w2 = h4*ex;
      double ap = 1.0;
      double s2 = ap-ex;
      double sp = ap;
      double s1 = 0.0;
      double sn = s1;
      double conex = fabs(con/wk);
      bool flag = true;
      while( flag )
      {
        flag = false;
        double cn = ap*s2/(sn+sp);
        s1 += cn;
        if( fabs(cn) > conex )
        {
          sn = sp;
          sp += 1.0;
          s2 -= w2;
          w2 *= h4/sp;
          ap = -ap*a2;
          flag = true;
        }
      }
      t = (atan(wk)-wk*s1)/2./M_PI;
      b += sgn*t;
      if( is>=0 || ak==0.0 )
      {
        if( b < 0.0 )b = 0.0;
        else if( b > 1.0 )b = 1.0;
        return b;
      }
      wh = -ak;
      wk = (ah/ak-r)/sqr;
      gw = 2.0*gk;
      is = 1;
    }
  }
}

double StudentTDistribution( double t, int n )
{
  // THE STUDENT T-DISTRIBUTION WITH N DEGREES OF FREEDOM.
  // stud
  //
  // DEF: STUD(T,N)=2/[SQRT(N)*BETA(1/2,N/2)]*INTEGRAL from -infinity to T
  //           of (1+(x**2)/N)**(-(N+1)/2) dx
  // LIMITING CASE: AS N --> INFINITY, STUD(T,N) --> FREQ(T)
  //                [i.e. the NORMAL DISTRIBUTION P(T)]
  // INPUT PARAMETERS:
  //   T=VALUE OF THE INDEPENDENT VARIABLE,
  //   N>=1
  // REFERENCE: CERN library routine G104
  // ACCURACY: INCREASES WITH INCREASING N, cf. STUDIN
  //
  if( n < 1 )
    throw EExpressionError(wxT("StudentTDistribution: number of degrees of freedom must be >=1"));
  if( n == 1 )return 0.5+atan(t)/M_PI;
  double fn = n;
  double a = t/sqrt(fn);
  double b = fn/(fn+t*t);
  double s = 1.0;
  if( n >= 4 )
  {
    double c = 1.0;
    int k = 2 + n%2;
    for( int i=k; i<=n-2; ++(++i) )
    {
      c *= b*(k-1.0)/k;
      s += c;
      ++(++k);
    }
  }
  if( n%2 == 0 )return 0.5+0.5*a*sqrt(b)*s;
  return 0.5+(a*b*s+atan(a))/M_PI;
}

double StudentTDistributionInverse( double q, int n )
{
  // THE INVERSE OF THE STUDENT T-DISTRIBUTION WITH N DEGREES OF FREEDOM.
  // studin
  //
  // STUDIN(Q,N)=T, where Q = 1/[SQRT(N)*BETA(1/2,N/2)]*INTEGRAL from -infinity to T of
  //        (1+(x**2)/N)**(-(N+1)/2) dx
  // INPUT PARAMETERS:
  //   Q=PROBABILITY AT WHICH THE FUNCTION IS INVERTED; 0<Q<1
  //   N>=1
  // REFERENCE: CACM ALGORITHM #396 ; CERN library routine G104
  // ACCURACY: INCREASES WITH INCREASING N, e.g.:
  //  for N=2, 5 significant figures are correct; for N=5, 8 sig, fig.
  //  for N=10, 8-9 sig. fig. ; for N=30, 9-11 sig. fig.
  //
  if( n < 1 )throw EExpressionError(
    wxT("StudentTDistributionInverse: number of degrees of freedom must be >=1") );
  if( q<=0.0 || q>1.0 )throw EExpressionError(
    wxT("StudentTDistributionInverse: first argument must be >0 and <=1"));
  double p, rl;
  if( q < 0.5 )
  {
    rl = -1.0;
    p = 2.*q;
  }
  else
  {
    rl = 1.0;
    p = 2.*(1.-q);
  }
  double result;
  if( n == 1 )
  {
    double pp = cos(M_PI*p/2.0);
    result = pp/sqrt(1.-pp*pp)*rl;  // sqrt(1-pp^2) = sin(pp)
  }
  else if( n == 2 )
  {
    result = sqrt(2./(p*(2.-p))-2.)*rl;
  }
  else
  {
    double rn = n;
    double a = 1./(rn-0.5);
    double b = 48./(a*a);
    double c = ((20700.*a/b-98.)*a-16.)*a+96.36;
    double d = ((94.5/(b+c)-3.)/b+1.)*sqrt(a*M_PI/2.0)*rn;
    double x = d*p;
    double y = pow(x,2./rn);
    if( y > 0.05+a ) // asymptotic inverse expansion about normal distribution:
    {
      double pp = 0.5*p;
      x = GaussInverse(pp);
      y = x*x;
      if( n >= 5 )c += 0.3*(rn-4.5)*(x+0.6);
      c += (((0.05*d*x-5.)*x-7.)*x-2.)*x+b;
      y = (((((0.4*y+6.3)*y+36.)*y+94.5)/c-y-3.)/b+1.)*x;
      y = a*y*y;
      if( y <= 0.002 )y = 0.5*y*y+y;
      else            y = exp(y)-1.;
    }
    else
    {
      y = ((1./(((rn+6.)/(rn*y)-0.089*d-0.822)*(rn+2.)*3.)+
            0.5/(rn+4.))*y-1.)*(rn+1.)/(rn+2.)+1./y;
    }
    result = sqrt(rn*y)*rl;
  }
  return result;
}

double NormalizedTina( double x, double a, double b, double c )
{
  // This routine computes the normalized TINA resolution function
  // tina
  //
  // TINA(X,a,b,c) = EXP((X-a)/c)*{1-(2/SQRT(PI))*
  // integral[EXP(-T^2)dT][from 0 to (X-a)/b] / [2*c*exp(b^2/(2*c)^2]
  //
  if( b == 0.0 )throw EExpressionError(wxT("NormalizedTina: third argument must not be 0") );
  if( c == 0.0 )throw EExpressionError(wxT("NormalizedTina: fourth argument must not be 0") );
  double xx = 2.0*c*exp(b*b/(4.0*c*c));
  if( (x-a)/c>80.0 || (x-a)/b>80.0 )return 0.0;
  else                              return exp((x-a)/c)*(1.0-Error((x-a)/b))/xx;
}

double CosineIntegral( double x )
{
  // CosineIntegral returns the cosine integral of argument x
  //    CosineIntegral(x)= euler + log(|x|) + integral((cos(t)-1)/t)dt,
  //         limits 0 to x, euler is Euler's constant
  // REFERENCE : CERN library routine C336
  //
  // RESTRICTION ON X : X != 0
  //
  // ACCURACY : TO AT LEAST 1.0E-6 IN REGION TESTED
  // TESTED REGION : from X = 0.5 to 10.0, however, with small X, accuracy is lost:
  // e.g., @X=0.4, accurate to 1.0E-5, @X=0.01, to 1.0E-1 only
  //
  if( x == 0.0 )throw EExpressionError(wxT("CosineIntegral: argument must not =0") );
  //
  double y, h, alfa;
  double b0 = 0.0;
  double b1 = 0.0;
  double b2 = 0.0;
  if( fabs(x) <= 8.0 )
  {
    y = 0.125 * x;
    h = 2.0*y*y - 1.0;
    alfa = -2.0*h;
    double const c[11] = {+1.94054915, +0.94134091, -0.57984503, +0.30915720,
                          -0.09161018, +0.01644374, -0.00197131, +0.00016925,
                          -0.00001094, +0.00000055, -0.00000002};
    for( int j=10; j>=0; --j )
    {
      b0 = c[j] - alfa*b1 - b2;
      b2 = b1;
      b1 = b0;
    }
    double const e = 0.57721566; // EULER'S CONSTANT
    return e + log(fabs(x)) - b0 + h*b2;
  }
  double r = 1.0 / x;
  y = 8.0 * r;
  h = 2.0*y*y - 1.0;
  alfa = -2.0 * h;
  double const p[10] = {+0.96074784, -0.03711390, +0.00194144, -0.00017166,
                        +0.00002113, -0.00000327, +0.00000060, -0.00000013,
                        +0.00000003, -0.00000001};
  for( int j=9; j>=0; --j )
  {
    b0 = p[j] - alfa*b1 - b2;
    b2 = b1;
    b1 = b0;
  }
  double pp = b0 - h*b2;
  b1 = 0.0;
  b2 = 0.0;
  double const q[8] = {+0.98604066, -0.01347174, +0.00045329, -0.00003067,
                       +0.00000313, -0.00000042, +0.00000007, -0.00000001};
  for( int j=7; j>=0; --j )
  {
    b0 = q[j] - alfa*b1 - b2;
    b2 = b1;
    b1 = b0;
  }
  double qq = b0 - h*b2;
  return r*(qq*sin(x)-r*pp*cos(x));
}

double SineIntegral( double x )
{
  // SineIntegral is the sine integral
  // SININT(X)= INTEGRAL(sin(t)/t)dt, limits 0 to X
  // REFERENCE: CERN library routine C336
  // sinint
  //
  // ACCURACY : TO AT LEAST 1.0E-6 IN REGION TESTED
  // TESTED REGION: 0.00 <= X <= 10.00
  //
  double y, h, alfa;
  double b0 = 0.0;
  double b1 = 0.0;
  double b2 = 0.0;
  if( fabs(x) <= 8.0 )
  {
    y = 0.125*x;
    h = 2.0*y*y - 1.0;
    alfa = -2.0*h;
    double const s[] = {+1.95222098, -0.68840423, +0.45518551, -0.18045712,
                        +0.04104221, -0.00595862, +0.00060014, -0.00004447,
                        +0.00000253, -0.00000011};
    for( int j=9; j>=0; --j )
    {
      b0 = s[j] - alfa*b1 - b2;
      b2 = b1;
      b1 = b0;
    }
    return y*(b0-b2);
  }
  double r = 1.0/x;
  y = 8.0*r;
  h = 2.0*y*y - 1.0;
  alfa = -2.0*h;
  double const p[] = {+0.96074784, -0.03711390, +0.00194144, -0.00017166,
                      +0.00002113, -0.00000327, +0.00000060, -0.00000013,
                      +0.00000003, -0.00000001};
  for( int j=9; j>=0; --j )
  {
    b0 = p[j] - alfa*b1 - b2;
    b2 = b1;
    b1 = b0;
  }
  double pp = b0 - h*b2;
  b1 = 0.0;
  b2 = 0.0;
  double const q[] = {+0.98604066, -0.01347174, +0.00045329, -0.00003067,
                      +0.00000313, -0.00000042, +0.00000007, -0.00000001};
  for( int j=7; j>=0; --j )
  {
    b0 = q[j] - alfa*b1 - b2;
    b2 = b1;
    b1 = b0;
  }
  double qq = b0 - h*b2;
  return Sign(0.5*M_PI,x) - r*(r*pp*sin(x)+qq*cos(x));
}

double DawsonIntegral( double x )
{
  // returns the dawson integral of x
  // dawson(x)= exp(-x**2) * integral(exp(t**2))dt, limits 0 to x
  // reference : cern library routine c339
  //
  // accuracy: to at least 1.0e-6 in tested region
  // tested region: 0.0 <= x <= 14; in general, accuracy improves as x increases.
  //
  double const p[] = {+1.739713835872306e+8, -2.359035430949078e+7,
                      +7.945951125626975e+6, -4.494089599795345e+5,
                      +6.264352248053304e+4, -1.642942304487861e+3,
                      +1.104151585964097e+2, -1.238060112669044e+0,
                      +1.701415625164813e-2};
  double const q[] = {+1.739713835872306e+8, +9.239056808199582e+7,
                      +2.314729422370434e+7, +3.599598259590670e+6,
                      +3.834980451271686e+5, +2.900221293895164e+4,
                      +1.544804495325198e+3, +5.423572743506117e+1};
  double const a1[] = {+5.002368960886679e-1, -5.976780868234889e+0,
                       +1.526440996236986e+1, -8.891064797478123e+0,
                       -7.579319180893693e-2, -4.000008936435497e+1,
                       +2.933657473954485e+1, -1.506956511871606e+0};
  double const a2[] = {+4.999999027050536e-1, -1.498380420366907e+0,
                       -4.985448029866077e+0, +5.064601537422308e+0,
                       -1.505077034966920e+1, -9.168048798135517e+0,
                       -2.661676748963993e+1, +4.764056452732288e+0};
  double const a3[] = {+5.000000000087358e-1, -2.500002783030495e+0,
                       -4.510578277783270e+0, -7.826362810336344e+0,
                       -4.052398173880339e+1, +4.127163327469802e+0};
  double const b1[] = {+2.260646660743092e-1, +1.158402925518881e+2,
                       +7.291775564155315e+1, +1.124616620245754e+2,
                       +7.211932176002291e+0, +1.244867882622516e+3,
                       -6.731060697448133e-1};
  double const b2[] = {+2.500114596118389e-1, -1.487158117871947e+0,
                       +3.307077246761144e+1, +1.465151677831093e+2,
                       +7.517012777440669e+1, +2.561057223422264e+1,
                       +2.877761229731874e+2};
  double const b3[] = {+7.499999926358122e-1, -2.499630060678980e+0,
                       -6.588346800131477e+0, -6.896361143376131e-1,
                       +5.204161728969395e+2};
  double z = fabs(x);
  double y = x*x;
  if( z >= 5.0 )
   return 0.5*(1.0+(a3[0]+b3[0]/(a3[1]+y+b3[1]/(a3[2]+y+b3[2]/(a3[3]+
          y+b3[3]/(a3[4]+y+b3[4]/(a3[5]+y))))))/y)/x;
  if( z >= 3.5 )
    return (a2[0]+b2[0]/(a2[1]+y+b2[1]/(a2[2]+y+b2[2]/(a2[3]+y+b2[3]/
           (a2[4]+y+b2[4]/(a2[5]+y+b2[5]/(a2[6]+y+b2[6]/(a2[7]+y))))))))/x;
  if( z >= 2.5 )
    return (a1[0]+b1[0]/(a1[1]+y+b1[1]/(a1[2]+y+b1[2]/(a1[3]+y+b1[3]/
           (a1[4]+y+b1[4]/(a1[5]+y+b1[5]/(a1[6]+y+b1[6]/(a1[7]+y))))))))/x;
  return x*(p[0]+y*(p[1]+y*(p[2]+y*(p[3]+y*(p[4]+y*(p[5]+y*(p[6]+y*(p[7]+y*p[8]))))))))/
         (q[0]+y*(q[1]+y*(q[2]+y*(q[3]+y*(q[4]+y*(q[5]+y*(q[6]+y*(q[7]+y))))))));
}

double Airy2( double x )
{
  // REFERENCE: SLATEC LIBRARY, CATEGORY B5I,B5L
  // PURPOSE
  // CALCULATES THE  BAIRY FUNCTION (AIRY OF SECOND KIND).
  //
  // SERIES FOR BIF        ON THE INTERVAL -1.00000E+00 TO  1.00000E+00
  //                                  WITH WEIGHTED ERROR   1.45E-32
  //                                   LOG WEIGHTED ERROR  31.84
  //                         SIGNIFICANT FIGURES REQUIRED  30.85
  //                              DECIMAL PLACES REQUIRED  32.40
  //
  // SERIES FOR BIG        ON THE INTERVAL -1.00000E+00 TO  1.00000E+00
  //                                  WITH WEIGHTED ERROR   1.29E-33
  //                                   LOG WEIGHTED ERROR  32.89
  //                          SIGNIFICANT FIGURES REQUIRED  31.48
  //                              DECIMAL PLACES REQUIRED  33.45
  //
  // SERIES FOR BIF2       ON THE INTERVAL  1.00000E+00 TO  8.00000E+00
  //                                  WITH WEIGHTED ERROR   6.08E-32
  //                                   LOG WEIGHTED ERROR  31.22
  //                  APPROX SIGNIFICANT FIGURES REQUIRED  30.8
  //                              DECIMAL PLACES REQUIRED  31.80
  //
  // SERIES FOR BIG2       ON THE INTERVAL  1.00000E+00 TO  8.00000E+00
  //                                  WITH WEIGHTED ERROR   4.91E-33
  //                                   LOG WEIGHTED ERROR  32.31
  //                  APPROX SIGNIFICANT FIGURES REQUIRED  31.6
  //                              DECIMAL PLACES REQUIRED  32.90
  //
  double const bifcs[13] = {
    -.16730216471986649483537423928176E-1,
    +.10252335834249445611426362777757E+0,
    +.17083092507381516539429650242013E-2,
    +.11862545467744681179216459210040E-4,
    +.44932907017792133694531887927242E-7,
    +.10698207143387889067567767663628E-9,
    +.17480643399771824706010517628573E-12,
    +.20810231071761711025881891834399E-15,
    +.18849814695665416509927971733333E-18,
    +.13425779173097804625882666666666E-21,
    +.77159593429658887893333333333333E-25,
    +.36533879617478566399999999999999E-28,
    +.14497565927953066666666666666666E-31};
  double const bigcs[13] = {
    +.22466223248574522283468220139024E-1,
    +.37364775453019545441727561666752E-1,
    +.44476218957212285696215294326639E-3,
    +.24708075636329384245494591948882E-5,
    +.79191353395149635134862426285596E-8,
    +.16498079851827779880887872402706E-10,
    +.24119906664835455909247501122841E-13,
    +.26103736236091436985184781269333E-16,
    +.21753082977160323853123792000000E-19,
    +.14386946400390433219483733333333E-22,
    +.77349125612083468629333333333333E-26,
    +.34469292033849002666666666666666E-29,
    +.12938919273216000000000000000000E-32};
  double const bif2cs[15] = {
    +.09984572693816041044682842579930E+0,
    +.47862497786300553772211467318231E+0,
    +.25155211960433011771324415436675E-1,
    +.58206938852326456396515697872216E-3,
    +.74997659644377865943861457378217E-5,
    +.61346028703493836681403010356474E-7,
    +.34627538851480632900434268733359E-9,
    +.14288910080270254287770846748931E-11,
    +.44962704298334641895056472179200E-14,
    +.11142323065833011708428300106666E-16,
    +.22304791066175002081517866666666E-19,
    +.36815778736393142842922666666666E-22,
    +.50960868449338261333333333333333E-25,
    +.60003386926288554666666666666666E-28,
    +.60827497446570666666666666666666E-31};
  double const big2cs[15] = {
    +.033305662145514340465176188111647E+0,
    +.161309215123197067613287532084943E+0,
    +.631900730961342869121615634921173E-2,
    +.118790456816251736389780192304567E-3,
    +.130453458862002656147116485012843E-5,
    +.937412599553521729546809615508936E-8,
    +.474580188674725153788510169834595E-10,
    +.178310726509481399800065667560946E-12,
    +.516759192784958180374276356640000E-15,
    +.119004508386827125129496251733333E-17,
    +.222982880666403517277063466666666E-20,
    +.346551923027689419722666666666666E-23,
    +.453926336320504514133333333333333E-26,
    +.507884996513522346666666666666666E-29,
    +.491020674696533333333333333333333E-32};
  double eta = 0.1*std::numeric_limits<double>::epsilon();
  int nbif = Initds( bifcs, 13, eta );
  int nbig = Initds( bigcs, 13, eta );
  int nbif2 = Initds( bif2cs, 15, eta );
  int nbig2 = Initds( big2cs, 15, eta );
  double x3sml = pow(eta,0.3333);
  double xmax = pow(1.5*log(std::numeric_limits<double>::max()),0.6666);
  if( x < -1.0 )
  {
    double xm, theta;
    AiryModulusPhase( x, xm, theta );
    return xm*sin(theta);
  }
  if( x <= 1.0 )
  {
    double z = 0.0;
    if( fabs(x) > x3sml )z = x*x*x;
    return 0.625+NTermChebyshevSeries(z,bifcs,nbif)+
        x*(0.4375+NTermChebyshevSeries(z,bigcs,nbig));
  }
  if( x <= 2.0 )
  {
    double z = (2.0*x*x*x-9.0)/7.0;
    return 1.125+NTermChebyshevSeries(z,bif2cs,nbif2)+
        x*(0.625+NTermChebyshevSeries(z,big2cs,nbig2));
  }
  if( x <= xmax )
    return DPBairy(x)*exp(2.0*x*sqrt(x)/3.0);
  throw EExpressionError(wxT("Airy2: argument is too large"));
}

double DPBairy( double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5I,B5L
  // dbie
  //
  //  CALCULATE THE D.P. BAIRY FUNCTION FOR NEGATIVE X AND AN EXPONENTIALLY
  //  SCALED BAIRY FUNCTION FOR POSITIVE X. i.e.,
  //  EVALUATE BI(X) FOR X .LE. 0.0  AND  BI(X)*EXP(-ZETA)  WHERE
  //  ZETA = 2/3 * X**(3/2)  FOR X .GE. 0.0
  //
  // SERIES FOR BIF        ON THE INTERVAL -1.00000E+00 TO  1.00000E+00
  //                                  WITH WEIGHTED ERROR   1.45E-32
  //                                   LOG WEIGHTED ERROR  31.84
  //                         SIGNIFICANT FIGURES REQUIRED  30.85
  //                              DECIMAL PLACES REQUIRED  32.40
  //
  // SERIES FOR BIG        ON THE INTERVAL -1.00000E+00 TO  1.00000E+00
  //                                  WITH WEIGHTED ERROR   1.29E-33
  //                                   LOG WEIGHTED ERROR  32.89
  //                         SIGNIFICANT FIGURES REQUIRED  31.48
  //                              DECIMAL PLACES REQUIRED  33.45
  //
  // SERIES FOR BIF2       ON THE INTERVAL  1.00000E+00 TO  8.00000E+00
  //                                  WITH WEIGHTED ERROR   6.08E-32
  //                                   LOG WEIGHTED ERROR  31.22
  //                  APPROX SIGNIFICANT FIGURES REQUIRED  30.8
  //                              DECIMAL PLACES REQUIRED  31.80
  //
  // SERIES FOR BIG2       ON THE INTERVAL  1.00000E+00 TO  8.00000E+00
  //                                  WITH WEIGHTED ERROR   4.91E-33
  //                                   LOG WEIGHTED ERROR  32.31
  //                  APPROX SIGNIFICANT FIGURES REQUIRED  31.6
  //                              DECIMAL PLACES REQUIRED  32.90
  //
  // SERIES FOR BIP1       ON THE INTERVAL  1.25000E-01 TO  3.53553E-01
  //                                  WITH WEIGHTED ERROR   1.06E-32
  //                                   LOG WEIGHTED ERROR  31.98
  //                         SIGNIFICANT FIGURES REQUIRED  30.61
  //                              DECIMAL PLACES REQUIRED  32.81
  //
  // SERIES FOR BIP2       ON THE INTERVAL  0.          TO  1.25000E-01
  //                                  WITH WEIGHTED ERROR   4.04E-33
  //                                   LOG WEIGHTED ERROR  32.39
  //                         SIGNIFICANT FIGURES REQUIRED  31.15
  //                              DECIMAL PLACES REQUIRED  33.37
  //
  // ATR = 16.0/(SQRT(8.)-1.)  AND  BTR = -(SQRT(8.)+1.)/(SQRT(8.)-1.)
  //
  double const bifcs[13] = {
    -.16730216471986649483537423928176E-1,
    +.10252335834249445611426362777757E+0,
    +.17083092507381516539429650242013E-2,
    +.11862545467744681179216459210040E-4,
    +.44932907017792133694531887927242E-7,
    +.10698207143387889067567767663628E-9,
    +.17480643399771824706010517628573E-12,
    +.20810231071761711025881891834399E-15,
    +.18849814695665416509927971733333E-18,
    +.13425779173097804625882666666666E-21,
    +.77159593429658887893333333333333E-25,
    +.36533879617478566399999999999999E-28,
    +.14497565927953066666666666666666E-31};
  double const bigcs[13] = {
    +.22466223248574522283468220139024E-1,
    +.37364775453019545441727561666752E-1,
    +.44476218957212285696215294326639E-3,
    +.24708075636329384245494591948882E-5,
    +.79191353395149635134862426285596E-8,
    +.16498079851827779880887872402706E-10,
    +.24119906664835455909247501122841E-13,
    +.26103736236091436985184781269333E-16,
    +.21753082977160323853123792000000E-19,
    +.14386946400390433219483733333333E-22,
    +.77349125612083468629333333333333E-26,
    +.34469292033849002666666666666666E-29,
    +.12938919273216000000000000000000E-32};
  double const bif2cs[15] = {
    +.0998457269381604104468284257993E+0,
    +.47862497786300553772211467318231E+0,
    +.25155211960433011771324415436675E-1,
    +.58206938852326456396515697872216E-3,
    +.74997659644377865943861457378217E-5,
    +.61346028703493836681403010356474E-7,
    +.34627538851480632900434268733359E-9,
    +.14288910080270254287770846748931E-11,
    +.44962704298334641895056472179200E-14,
    +.11142323065833011708428300106666E-16,
    +.22304791066175002081517866666666E-19,
    +.36815778736393142842922666666666E-22,
    +.50960868449338261333333333333333E-25,
    +.60003386926288554666666666666666E-28,
    +.60827497446570666666666666666666E-31};
  double const big2cs[15] = {
    +.033305662145514340465176188111647E+0,
    +.161309215123197067613287532084943E+0,
    +.631900730961342869121615634921173E-2,
    +.118790456816251736389780192304567E-3,
    +.130453458862002656147116485012843E-5,
    +.937412599553521729546809615508936E-8,
    +.474580188674725153788510169834595E-10,
    +.178310726509481399800065667560946E-12,
    +.516759192784958180374276356640000E-15,
    +.119004508386827125129496251733333E-17,
    +.222982880666403517277063466666666E-20,
    +.346551923027689419722666666666666E-23,
    +.453926336320504514133333333333333E-26,
    +.507884996513522346666666666666666E-29,
    +.491020674696533333333333333333333E-32};
  double const bip1cs[47] = {
    -.83220474779434474687471864707973E-1,
    +.11461189273711742889920226128031E-1,
    +.42896440718911509494134472566635E-3,
    -.14906639379950514017847677732954E-3,
    -.13076597267876290663136340998881E-4,
    +.63275983961030344754535716032494E-5,
    -.42226696982681924884778515889433E-6,
    -.19147186298654689632835494181277E-6,
    +.64531062845583173611038157880934E-7,
    -.78448546771397719289748310448628E-8,
    -.96077216623785085879198533565432E-9,
    +.70004713316443966339006074402068E-9,
    -.17731789132814932022083128056698E-9,
    +.22720894783465236347282126389311E-10,
    +.16540456313972049847032860681891E-11,
    -.18517125559292316390755369896693E-11,
    +.59576312477117290165680715534277E-12,
    -.12194348147346564781055769498986E-12,
    +.13347869253513048815386347813597E-13,
    +.17278311524339746664384792889731E-14,
    -.14590732013016720735268871713166E-14,
    +.49010319927115819978994989520104E-15,
    -.11556545519261548129262972762521E-15,
    +.19098807367072411430671732441524E-16,
    -.11768966854492179886913995957862E-17,
    -.63271925149530064474537459677047E-18,
    +.33861838880715361614130191322316E-18,
    -.10725825321758625254992162219622E-18,
    +.25995709605617169284786933115562E-19,
    -.48477583571081193660962309494101E-20,
    +.55298913982121625361505513198933E-21,
    +.49421660826069471371748197444266E-22,
    -.55162121924145707458069720814933E-22,
    +.21437560417632550086631884499626E-22,
    -.61910313387655605798785061137066E-23,
    +.14629362707391245659830967336959E-23,
    -.27918484471059005576177866069333E-24,
    +.36455703168570246150906795349333E-25,
    +.58511821906188711839382459733333E-27,
    -.24946950487566510969745047551999E-26,
    +.10979323980338380977919579477333E-26,
    -.34743388345961115015034088106666E-27,
    +.91373402635349697363171082240000E-28,
    -.20510352728210629186247720959999E-28,
    +.37976985698546461748651622399999E-29,
    -.48479458497755565887848448000000E-30,
    -.10558306941230714314205866666666E-31};
  double const bip2cs[88] = {
    -.11359673758598867913797310895527E+0,
    +.41381473947881595760052081171444E-2,
    +.13534706221193329857696921727508E-3,
    +.10427316653015353405887183456780E-4,
    +.13474954767849907889589911958925E-5,
    +.16965374054383983356062511163756E-6,
    -.10096500865641624301366228396373E-7,
    -.16729119493778475127836973095943E-7,
    -.45815364485068383217152795613391E-8,
    +.37366813665655477274064749384284E-9,
    +.57669303201452448119584643502111E-9,
    +.62181265087850324095393408792371E-10,
    -.63294120282743068241589177281354E-10,
    -.14915047908598767633999091989487E-10,
    +.78896213942486771938172394294891E-11,
    +.24960513721857797984888064000127E-11,
    -.12130075287291659477746664734814E-11,
    -.37404939108727277887343460402716E-12,
    +.22377278140321476798783446931091E-12,
    +.47490296312192466341986077472514E-13,
    -.45261607991821224810605655831294E-13,
    -.30172271841986072645112245876020E-14,
    +.91058603558754058327592683478908E-14,
    -.98149238033807062926643864207709E-15,
    -.16429400647889465253601245251589E-14,
    +.55334834214274215451182114635164E-15,
    +.21750479864482655984374381998156E-15,
    -.17379236200220656971287029558087E-15,
    -.10470023471443714959283909313604E-17,
    +.39219145986056386925441403311462E-16,
    -.11621293686345196925824005665910E-16,
    -.54027474491754245533735411307773E-17,
    +.45441582123884610882675428553304E-17,
    -.28775599625221075729427585480086E-18,
    -.10017340927225341243596162960440E-17,
    +.44823931215068369856332561906313E-18,
    +.76135968654908942328948982366775E-19,
    -.14448324094881347238956060145422E-18,
    +.40460859449205362251624847392112E-19,
    +.20321085700338446891325190707277E-19,
    -.19602795471446798718272758041962E-19,
    +.34273038443944824263518958211738E-20,
    +.37023705853905135480024651593154E-20,
    -.26879595172041591131400332966712E-20,
    +.28121678463531712209714454683364E-21,
    +.60933963636177797173271119680329E-21,
    -.38666621897150844994172977893413E-21,
    +.25989331253566943450895651927228E-22,
    +.97194393622938503767281175216084E-22,
    -.59392817834375098415630478204591E-22,
    +.38864949977113015409591960439444E-23,
    +.15334307393617272869721512868769E-22,
    -.97513555209762624036336521409724E-23,
    +.96340644440489471424741339383726E-24,
    +.23841999400208880109946748792454E-23,
    -.16896986315019706184848044205207E-23,
    +.27352715888928361222578444801478E-24,
    +.35660016185409578960111685025730E-24,
    -.30234026608258827249534280666954E-24,
    +.75002041605973930653144204823232E-25,
    +.48403287575851388827455319838748E-25,
    -.54364137654447888432698010297766E-25,
    +.19281214470820962653345978809756E-25,
    +.50116355020532656659611814172172E-26,
    -.95040744582693253786034620869972E-26,
    +.46372646157101975948696332245611E-26,
    +.21177170704466954163768170577046E-28,
    -.15404850268168594303692204548726E-26,
    +.10387944293201213662047889194441E-26,
    -.19890078156915416751316728235153E-27,
    -.21022173878658495471177044522532E-27,
    +.21353099724525793150633356670491E-27,
    -.79040810747961342319023537632627E-28,
    -.16575359960435585049973741763592E-28,
    +.38868342850124112587625586496537E-28,
    -.22309237330896866182621562424717E-28,
    +.27777244420176260265625977404382E-29,
    +.57078543472657725368712433782772E-29,
    -.51743084445303852800173371555280E-29,
    +.18413280751095837198450927071569E-29,
    +.44422562390957094598544071068647E-30,
    -.98504142639629801547464958226943E-30,
    +.58857201353585104884754198881995E-30,
    -.97636075440429787961402312628595E-31,
    -.13581011996074695047063597884122E-30,
    +.13999743518492413270568048380345E-30,
    -.59754904545248477620884562981118E-31,
    -.40391653875428313641045327529856E-32};
  double const atr = 8.75069057084843450880771988210148;
  double const btr = -2.09383632135605431360096498526268;
  double const eta = 0.1*std::numeric_limits<double>::epsilon();
  int const nbif = Initds(bifcs,13,eta);
  int const nbig = Initds(bigcs,13,eta);
  int const nbif2 = Initds(bif2cs,15,eta);
  int const nbig2 = Initds(big2cs,15,eta);
  int const nbip1 = Initds(bip1cs,47,eta);
  int const nbip2 = Initds(bip2cs,88,eta);
  //
  double const x3sml = pow(eta,0.3333);
  double const x32sml = 1.3104*x3sml*x3sml;
  double const xbig = pow(std::numeric_limits<double>::max(),0.6666);
  //
  if( x < -1.0 )
  {
    double xm, theta;
    AiryModulusPhase( x, xm, theta );
    return xm*sin(theta);
  }
  if( x <= 1.0 )
  {
    double z = 0.0;
    if( fabs(x) > x3sml )z = x*x*x;
    double result = 0.625+NTermChebyshevSeries(z,bifcs,nbif) +
        x*(0.4375+NTermChebyshevSeries(z,bigcs,nbig));
    if( x > x32sml )result *= exp(-2.0*x*sqrt(x)/3.0);
    return result;
  }
  if( x <= 2.0 )
  {
    double z = (2.0*x*x*x-9.0)/7.0;
    return exp(-2.0*x*sqrt(x)/3.0) *
        (1.125 + NTermChebyshevSeries(z,bif2cs,nbif2) +
         x*(0.625+NTermChebyshevSeries(z,big2cs,nbig2)) );
  }
  if( x <= 4.0 )
  {
    double sqrtx = sqrt(x);
    double z = atr/(x*sqrtx) + btr;
    return (0.625+NTermChebyshevSeries(z,bip1cs,nbip1))/sqrt(sqrtx);
  }
  double sqrtx = sqrt(x);
  double z = -1.0;
  if( x < xbig )z = 16.0/(x*sqrtx) - 1.0;
  return (0.625+NTermChebyshevSeries(z,bip2cs,nbip2))/sqrt(sqrtx);
}

double DiLog( double x )
{
  // THIS ROUTINE CALCULATES THE DILOGARITHM FUNCTION
  // dilog
  // DILOG(X)= -INTEGRAL(log(abs(1-t))/t)dt, limits 0 to X
  // DILOGARITHM FUNCTION IS ALSO KNOWN AS THE SPENCE'S INTEGRAL
  // REFERENCE: CERN library routine C304
  // N.B. A MORE EFFICIENT ROUTINE CAN BE FOUND IN CACM ALGORITHM #490
  //
  // ACCURACY: 10 SIGNIFICANT FIGURES HAVE BEEN CHECKED TO BE
  //           CORRECT WHERE 0.5 <= X <= 1.0
  // NB. ACCURACY MAY BE LOST NEAR ZERO OF DILOG AT X=12.595170369845018...
  //
  if( x == 1.0 )return 1.644934066848226;
  double t, s, z;
  if( x < -1.0 )
  {
    t = 1.0/x;
    s = -0.5;
    z = -M_PI*M_PI/6.0 - 0.5*log(fabs(x))*log(fabs(x));
  }
  else if( x <= 0.5 )
  {
    t = x;
    s = 0.5;
    z = 0.0;
  }
  else if( x <= 2.0 )
  {
    t = 1.0-x;
    s = -0.5;
    z = 1.644934066848226 - log(x)*log(fabs(t));
  }
  else
  {
    t = 1.0/x;
    s = -0.5;
    z = M_PI*M_PI/3.0 - 0.5*log(fabs(x))*log(fabs(x));
  }
  double y = 2.666666666666667*t + 0.666666666666667;
  double b = 0.000000000000001;
  double a = y*b + 0.000000000000004;
  b = y*a - b + 0.000000000000011;
  a = y*b - a + 0.000000000000037;
  b = y*a - b + 0.000000000000121;
  a = y*b - a + 0.000000000000398;
  b = y*a - b + 0.000000000001312;
  a = y*b - a + 0.000000000004342;
  b = y*a - b + 0.000000000014437;
  a = y*b - a + 0.000000000048274;
  b = y*a - b + 0.000000000162421;
  a = y*b - a + 0.000000000550291;
  b = y*a - b + 0.000000001879117;
  a = y*b - a + 0.000000006474338;
  b = y*a - b + 0.000000022536705;
  a = y*b - a + 0.000000079387055;
  b = y*a - b + 0.000000283575385;
  a = y*b - a + 0.000001029904264;
  b = y*a - b + 0.000003816329463;
  a = y*b - a + 0.000014496300557;
  b = y*a - b + 0.000056817822718;
  a = y*b - a + 0.000232002196094;
  b = y*a - b + 0.001001627496164;
  a = y*b - a + 0.004686361959447;
  b = y*a - b + 0.024879322924228;
  a = y*b - a + 0.166073032927855;
  a = y*a - b + 1.935064300869969;
  return s*t*(a-b) + z;
}

double IncompleteEllipticIntegral1( double x, double phi )
{
  // CALCULATES THE INCOMPLETE ELLIPTIC INTEGRALS OF THE FIRST KIND,
  // F(X,PHI) where X = modulus,    PHI = amplitude in radians
  // finell
  //
  // DEFINITION:
  // F(X,PHI)=INTEGRAL from 0 to PHI of (1-(X*sin(t))^2)^-0.5 dt
  // NB. If PHI=PI/2 then F is the COMPLETE ELLIPTIC FUNCTION ELLICK.
  //
  // REFERENCE: CACM ALGORITHM # 73
  // RESTRICTIONS: |X| <= 1   and   |PHI| <= pi/2
  //               otherwise integral will be infinite
  // ACCURACY: TO AT LEAST 1.0E-8 where 0 <= X <=sin(88 degrees),
  //           and  0 <= PHI <= 90 (degree)
  //
  if( fabs(x)>1.0 || fabs(phi)>M_PI/2.0 )
    throw EExpressionError(wxT("IncompleteEllipticIntegral1: integral is infinite"));
  double h1 = 1.0;
  double sigma[4] = { 0.0, 0.0, 0.0, 0.0 };
  int bn = 0;
  double sinphi = sin(phi);
  double cosphi = cos(phi);
  if( fabs(x*sinphi) <= tanh(1.0) )
  {
    double a1 = phi;
    double a2, del1;
    do
    {
      ++bn;
      double eine = (2.0*bn-1.0)/(2.0*bn);
      double h2 = eine*x*x*h1;
      a2 = eine*a1 - pow(sinphi,static_cast<double>(2*bn-1))*cosphi/(2.0*bn);
      del1 = h2*a2;
      sigma[0] += del1;
      h1 = h2;
      a1 = a2;
    }
    while( fabs(del1)>0.0 && fabs(phi*pow(sinphi,static_cast<double>(2*bn)))>=fabs(a2) );
    return phi+sigma[0];
  }
  double px = sqrt(1-x*x);
  double a1 = 1.0;
  double al1 = 0.0;
  double am1 = 0.0;
  double an1 = 0.0;
  double del1, result;
  do
  {
    ++bn;
    double eine = (2.0*bn-1.0)/(2.0*bn);
    result = (fabs(x)*sqrt(1-sinphi*sinphi)*pow(1-x*x*sinphi*sinphi,bn-0.5))/(2.0*bn);
    double h2 = eine*h1;
    double a2 = eine*eine*px*px*a1;
    double al2 = al1 + 1/(bn*(2.0*bn-1));
    double am2 = (am1-result*h2)*((2.0*bn+1)/(2.0*bn+2))*((2.0*bn+1)/(2.0*bn+2))*px*px;
    double an2 = (an1-result*h1)*eine*(2.0*bn+1)*px*px/(2.0*bn+2);
    del1 = am2 - a2*al2;
    double del3 = a2;
    sigma[0] += del1;
    sigma[2] += del3;
    h1 = h2;
    a1 = a2;
    al1 = al2;
    am1 = am2;
    an1 = an2;
  }
  while( fabs(del1) > 0.0 );
  double xsin = sqrt(1-x*x*sinphi*sinphi);
  double t1 = log(4/(xsin+fabs(x)*cosphi));
  double t2 = fabs(x)*cosphi/xsin;
  result = t1*(1+sigma[2]) + t2*log(0.5+0.5*fabs(x*sinphi)) + sigma[0];
  if( phi < 0.0 )result *= -1.0;
  return result;
}

double CompleteEllipticIntegral1( double x )
{
  // the complete elliptic integral of the first kind
  // E(X)= INTEGRAL(1/sqrt(1-(X*sin(t))**2))dt, limits 0 to pi/2
  // ellick
  //
  // a special case of the INCOMPLETE ELLIPTIC INTEGRAL E(X) = F(X,pi/2)
  // REFERENCE: CERN library routine C308
  //
  // RESTRICITION ON ARGUMENT X: |X| < 1,
  // ACCURACY: IN GENERAL TO AT LEAST 1.0E-6
  // REGION TESTED : 0.0 <= X < 1.00
  //
  if( fabs(x) >= 1.0 )
    throw EExpressionError(wxT("CompleteEllipticIntegral1: |argument| must be < 1.0"));
  double const c = 1.3862944;
  double const a[4] = {1.4513386e-2, 3.7425396e-2, 3.5899801e-2, 9.6663384e-2};
  double const b[4] = {4.4183982e-3, 3.3285210e-2, 6.8802955e-2, 1.2498595e-1};
  double const eta = 1.0 - x*x;
  double pa = a[0];
  double pb = b[0];
  for( int j=1; j<4; ++j )
  {
    pa *= eta;
    pa += a[j];
    pb *= eta;
    pb += b[j];
  }
  return c+pa*eta-log(eta)*(0.5+pb*eta);
}

double IncompleteEllipticIntegral2( double x, double phi )
{
  // CALCULATES THE INCOMPLETE ELLIPTIC INTEGRALS OF THE SECOND KIND,
  // E(X,PHI) where X = modulus,    PHI = amplitude in radians
  // einell
  //
  // DEFINITION:
  // E(X,PHI)=INTEGRAL from 0 to PHI of (1-(X*sin(t))^2)^0.5 dt
  // NB. If PHI=PI/2 then E is the COMPLETE ELLIPTIC FUNCTION
  // REFERENCE: CACM ALGORITHM # 73
  // RESTRICTIONS: |X| <= 1   and   |PHI| <= pi/2
  //               otherwise integral will be infinite
  // ACCURACY: TO AT LEAST 1.00E-8
  //
  if( fabs(x)>1.0 || fabs(phi)>M_PI/2.0 )
    throw EExpressionError(wxT("IncompleteEllipticIntegral2: integral is infinite"));
  double h1 = 1.0;
  double sigma[4] = {0.0, 0.0, 0.0, 0.0};
  int bn = 0;
  double sinphi = sin(phi);
  double cosphi = cos(phi);
  double tmp, del1, a1, a2;
  if( fabs(x*sinphi) <= tanh(1.0) )
  {
    a1 = phi;
    do
    {
      ++bn;
      tmp = (2.0*bn-1.04)/(2.0*bn);
      double h2 = tmp*x*x*h1;
      a2 = tmp*a1 - pow(sinphi,static_cast<double>(2*bn-1))*cosphi/(2*bn);
      del1 = h2*a2;
      double del2 = -x*x*h1*a2/(2*bn);
      sigma[0] += del1;
      sigma[1] += del2;
      h1 = h2;
      a1 = a2;
    }
    while( fabs(del1)>0.0 && fabs(phi*pow(sinphi,static_cast<double>(2*bn)))>=fabs(a2) );
    return phi+sigma[1];
  }
  double px = sqrt(1-x*x);
  a1 = 1.0;
  double al1 = 0.0;
  double am1 = 0.0;
  double an1 = 0.0;
  do
  {
    ++bn;
    tmp = (bn*2.0-1.0)/(bn*2.0);
    double fine = (fabs(x)*sqrt(1-sinphi*sinphi)*pow(1-x*x*sinphi*sinphi,bn-0.5))/(bn*2.);
    double h2 = tmp*h1;
    a2 = tmp*tmp*px*px*a1;
    double al2 = al1 + 1.0/(bn*(bn*2.0-1.0));
    double am2 = (am1-fine*h2)*((bn*2.0+1.0)/(bn*2.0+2.0))*((bn*2.0+1.0)/(bn*2.0+2.0))*px*px;
    double an2 = (an1-fine*h1)*tmp*(bn*2+1)*px*px/(bn*2.0+2.0);
    del1 = am2 - a2*al2;
    double del2 = an2 - tmp*px*px*a1*al2 + px*px*a1/(bn*4.0*bn);
    double del3 = a2;
    double del4 = (bn*2+1)*a2/(bn*2.0+2.0);
    sigma[0] += del1;
    sigma[1] += del2;
    sigma[2] += del3;
    sigma[3] += del4;
    h1 = h2;
    a1 = a2;
    al1 = al2;
    am1 = am2;
    an1 = an2;
  }
  while( fabs(del1) > 0.0 );
  double xsin = sqrt(1-x*x*sinphi*sinphi);
  double t1 = log(4.0/(xsin+fabs(x)*cosphi));
  double t2 = fabs(x)*cosphi/xsin;
  tmp = (0.5+sigma[3])*px*px*t1 + 1.0 - t2*(1-fabs(x*sinphi)) + sigma[1];
  if( phi < 0.0 )tmp *= -1.0;
  return tmp;
}

double CompleteEllipticIntegral2( double x )
{
  // COMPLETE ELLIPTIC INTEGRAL OF THE SECOND KIND
  // E(X)= INTEGRAL(sqrt(1-(X*sin(t))**2))dt, limits 0 to pi/2
  // ellice
  //
  // a special case of the INCOMPLETE ELLIPTIC INTEGRAL ELLICE(X) = EINELL(X,pi/2)
  // REFERENCE: CERN library routine C308
  // RESTRICTION ON ARGUMENT X:  |X| <= 1
  // ACCURACY: IN GENERAL TO AT LEAST 1.0E-6
  // REGION TESTED: 0.0 <= X <= 1.00
  //
  if( fabs(x) > 1.0 )
    throw EExpressionError(wxT("CompleteEllipticIntegral2: |argument| must be <= 1"));
  if( fabs(x) == 1.0 )return 1.0;
  //
  double const a[4] = {1.7363149E-2, 4.7574044E-2, 6.2607619E-2, 4.4325151E-1};
  double const b[4] = {5.2637893E-3, 4.0694684E-2, 9.2001094E-2, 2.4998366E-1};
  double eta = 1.0 - x*x;
  double pa = a[0];
  double pb = b[0];
  for( int i=1; i<4; ++i )
  {
    pa *= eta;
    pa += a[i];
    pb *= eta;
    pb += b[i];
  }
  return 1.0+(pa-log(eta)*pb)*eta;
}

double ExponentialIntegral( double x )
{
  // REFERENCE: SLATEC LIBRARY; CATEGORY B5E
  // expint
  //
  // computes the  exponential integral e1(x).
  // series on the interval -0.03125 to 0
  //                           with weighted error   4.62e-32
  //                             log weighted error  31.34
  //                   significant figures required  29.70
  //                        decimal places required  32.18
  // series on the interval -0.125 to -0.03125
  //                           with weighted error   2.22e-32
  //                             log weighted error  31.65
  //                   significant figures required  30.75
  //                        decimal places required  32.54
  // series on the interval -0.25 to -0.125
  //                           with weighted error   5.19e-32
  //                             log weighted error  31.28
  //                   significant figures required  30.82
  //                        decimal places required  32.09
  // series on the interval -4 to -1
  //                           with weighted error   8.49e-34
  //                             log weighted error  33.07
  //                   significant figures required  34.13
  //                        decimal places required  33.80
  // series on the interval -1 to 1
  //                           with weighted error   8.08e-33
  //                             log weighted error  32.09
  //            approx significant figures required  30.4
  //                        decimal places required  32.79
  // series on the interval  0.25 to 1
  //                           with weighted error   6.65e-32
  //                             log weighted error  31.18
  //                   significant figures required  30.69
  //                        decimal places required  32.03
  // series on the interval  0 to 0.25
  //                           with weighted error   5.07e-32
  //                             log weighted error  31.30
  //                   significant figures required  30.40
  //                        decimal places required  32.20
  //
  if( x == 0.0 )throw EExpressionError(wxT("ExponentialIntegral: argument = 0") );
  double xmax = log(-log(std::numeric_limits<double>::min()));
  if( x > xmax )throw EExpressionError(wxT("ExponentialIntegral: argument is too large") );
  double eta = 0.1*std::numeric_limits<double>::epsilon();
  //
  if( x > -1.0 )
  {
    if( x <= 1.0 )
    {
      double const a[25] = {
          -.3739021479220279511668698204827e-1, +.4272398606220957726049179176528e-1,
          -.130318207984970054415392055219726e+0, +.144191240246988907341095893982137e-1,
          -.134617078051068022116121527983553e-2, +.107310292530637799976115850970073e-3,
          -.742999951611943649610283062223163e-5, +.453773256907537139386383211511827e-6,
          -.247641721139060131846547423802912e-7, +.122076581374590953700228167846102e-8,
          -.548514148064092393821357398028261e-10, +.226362142130078799293688162377002e-11,
          -.863589727169800979404172916282240e-13, +.306291553669332997581032894881279e-14,
          -.101485718855944147557128906734933e-15, +.315482174034069877546855328426666e-17,
          -.923604240769240954484015923200000e-19, +.255504267970814002440435029333333e-20,
          -.669912805684566847217882453333333e-22, +.166925405435387319431987199999999e-23,
          -.396254925184379641856000000000000e-25, +.898135896598511332010666666666666e-27,
          -.194763366993016433322666666666666e-28, +.404836019024630033066666666666666e-30,
          -.807981567699845120000000000000000e-32};
      int n = Initds( a, 25, eta );
      return -log(fabs(x)) - 0.6875 + x + NTermChebyshevSeries(x,a,n);
    }
    else if( x <= 4.0 )
    {
      double const a[50] = {
          -.60577324664060345999319382737747e+0, -.11253524348366090030649768852718e+0,
          +.13432266247902779492487859329414e-1, -.19268451873811457249246838991303e-2,
          +.30911833772060318335586737475368e-3, -.53564132129618418776393559795147e-4,
          +.98278128802474923952491882717237e-5, -.18853689849165182826902891938910e-5,
          +.37494319356894735406964042190531e-6, -.76823455870552639273733465680556e-7,
          +.16143270567198777552956300060868e-7, -.34668022114907354566309060226027e-8,
          +.75875420919036277572889747054114e-9, -.16886433329881412573514526636703e-9,
          +.38145706749552265682804250927272e-10, -.87330266324446292706851718272334e-11,
          +.20236728645867960961794311064330e-11, -.47413283039555834655210340820160e-12,
          +.11221172048389864324731799928920e-12, -.26804225434840309912826809093395e-13,
          +.64578514417716530343580369067212e-14, -.15682760501666478830305702849194e-14,
          +.38367865399315404861821516441408e-15, -.94517173027579130478871048932556e-16,
          +.23434812288949573293896666439133e-16, -.58458661580214714576123194419882e-17,
          +.14666229867947778605873617419195e-17, -.36993923476444472706592538274474e-18,
          +.93790159936721242136014291817813e-19, -.23893673221937873136308224087381e-19,
          +.61150624629497608051934223837866e-20, -.15718585327554025507719853288106e-20,
          +.40572387285585397769519294491306e-21, -.10514026554738034990566367122773e-21,
          +.27349664930638667785806003131733e-22, -.71401604080205796099355574271999e-23,
          +.18705552432235079986756924211199e-23, -.49167468166870480520478020949333e-24,
          +.12964988119684031730916087125333e-24, -.34292515688362864461623940437333e-25,
          +.90972241643887034329104820906666e-26, -.24202112314316856489934847999999e-26,
          +.64563612934639510757670475093333e-27, -.17269132735340541122315987626666e-27,
          +.46308611659151500715194231466666e-28, -.12448703637214131241755170133333e-28,
          +.33544574090520678532907007999999e-29, -.90598868521070774437543935999999e-30,
          +.24524147051474238587273216000000e-30, -.66528178733552062817107967999999e-31};
      int n = Initds( a, 50, eta );
      return exp(-x)/x * (1.0 + NTermChebyshevSeries((8.0/x-5.0)/3.0,a,n));
    }
    else if( x <= xmax )
    {
      double const a[64] = {
          -.1892918000753016825495679942820e+0, -.8648117855259871489968817056824e-1,
          +.7224101543746594747021514839184e-2, -.8097559457557386197159655610181e-3,
          +.1099913443266138867179251157002e-3, -.1717332998937767371495358814487e-4,
          +.2985627514479283322825342495003e-5, -.5659649145771930056560167267155e-6,
          +.1152680839714140019226583501663e-6, -.2495030440269338228842128765065e-7,
          +.5692324201833754367039370368140e-8, -.1359957664805600338490030939176e-8,
          +.3384662888760884590184512925859e-9, -.8737853904474681952350849316580e-10,
          +.2331588663222659718612613400470e-10, -.6411481049213785969753165196326e-11,
          +.1812246980204816433384359484682e-11, -.5253831761558460688819403840466e-12,
          +.1559218272591925698855028609825e-12, -.4729168297080398718476429369466e-13,
          +.1463761864393243502076199493808e-13, -.4617388988712924102232173623604e-14,
          +.1482710348289369323789239660371e-14, -.4841672496239229146973165734417e-15,
          +.1606215575700290408116571966188e-15, -.5408917538957170947895023784252e-16,
          +.1847470159346897881370231402310e-16, -.6395830792759094470500610425050e-17,
          +.2242780721699759457250233276170e-17, -.7961369173983947552744555308646e-18,
          +.2859308111540197459808619929272e-18, -.1038450244701137145900697137446e-18,
          +.3812040607097975780866841008319e-19, -.1413795417717200768717562723696e-19,
          +.5295367865182740958305442594815e-20, -.2002264245026825902137211131439e-20,
          +.7640262751275196014736848610918e-21, -.2941119006868787883311263523362e-21,
          +.1141823539078927193037691483586e-21, -.4469308475955298425247020718489e-22,
          +.1763262410571750770630491408520e-22, -.7009968187925902356351518262340e-23,
          +.2807573556558378922287757507515e-23, -.1132560944981086432141888891562e-23,
          +.4600574684375017946156764233727e-24, -.1881448598976133459864609148108e-24,
          +.7744916111507730845444328478037e-25, -.3208512760585368926702703826261e-25,
          +.1337445542910839760619930421384e-25, -.5608671881802217048894771735210e-26,
          +.2365839716528537483710069473279e-26, -.1003656195025305334065834526856e-26,
          +.4281490878094161131286642556927e-27, -.1836345261815318199691326958250e-27,
          +.7917798231349540000097468678144e-28, -.3431542358742220361025015775231e-28,
          +.1494705493897103237475066008917e-28, -.6542620279865705439739042420053e-29,
          +.2877581395199171114340487353685e-29, -.1271557211796024711027981200042e-29,
          +.5644615555648722522388044622506e-30, -.2516994994284095106080616830293e-30,
          +.1127259818927510206370368804181e-30, -.5069814875800460855562584719360e-31};
      int n = Initds( a, 64, eta );
      return exp(-x)/x * (1.0 + NTermChebyshevSeries(8.0/x-1.0,a,n));
    }
  }
  if( x <= -32.0 )
  {
    double const a[50] = {
        +.3284394579616699087873844201881e-1, -.1669920452031362851476184343387e-1,
        +.2845284724361346807424899853252e-3, -.7563944358516206489487866938533e-5,
        +.2798971289450859157504843180879e-6, -.1357901828534531069525563926255e-7,
        +.8343596202040469255856102904906e-9, -.6370971727640248438275242988532e-10,
        +.6007247608811861235760831561584e-11, -.7022876174679773590750626150088e-12,
        +.1018302673703687693096652346883e-12, -.1761812903430880040406309966422e-13,
        +.3250828614235360694244030353877e-14, -.5071770025505818678824872259044e-15,
        +.1665177387043294298172486084156e-16, +.3166753890797514400677003536555e-16,
        -.1588403763664141515133118343538e-16, +.4175513256138018833003034618484e-17,
        -.2892347749707141906710714478852e-18, -.2800625903396608103506340589669e-18,
        +.1322938639539270903707580023781e-18, -.1804447444177301627283887833557e-19,
        -.7905384086522616076291644817604e-20, +.4435711366369570103946235838027e-20,
        -.4264103994978120868865309206555e-21, -.3920101766937117541553713162048e-21,
        +.1527378051343994266343752326971e-21, +.1024849527049372339310308783117e-22,
        -.2134907874771433576262711405882e-22, +.3239139475160028267061694700366e-23,
        +.2142183762299889954762643168296e-23, -.8234609419601018414700348082312e-24,
        -.1524652829645809479613694401140e-24, +.1378208282460639134668480364325e-24,
        +.2131311202833947879523224999253e-26, -.2012649651526484121817466763127e-25,
        +.1995535662263358016106311782673e-26, +.2798995808984003464948686520319e-26,
        -.5534511845389626637640819277823e-27, -.3884995396159968861682544026146e-27,
        +.1121304434507359382850680354679e-27, +.5566568152423740948256563833514e-28,
        -.2045482929810499700448533938176e-28, -.8453813992712336233411457493674e-29,
        +.3565758433431291562816111116287e-29, +.1383653872125634705539949098871e-29,
        -.6062167864451372436584533764778e-30, -.2447198043989313267437655119189e-30,
        +.1006850640933998348011548180480e-30, +.4623685555014869015664341461674e-31};
    int n = Initds( a, 50, eta );
    return exp(-x)/x * (1.0 + NTermChebyshevSeries(64.0/x+1.0,a,n));
  }
  if( x <= -8.0 )
  {
    double const a[60] = {
        +.20263150647078889499401236517381e+0, -.73655140991203130439536898728034e-1,
        +.63909349118361915862753283840020e-2, -.60797252705247911780653153363999e-3,
        -.73706498620176629330681411493484e-4, +.48732857449450183453464992488076e-4,
        -.23837064840448290766588489460235e-5, -.30518612628561521027027332246121e-5,
        +.17050331572564559009688032992907e-6, +.23834204527487747258601598136403e-6,
        +.10781772556163166562596872364020e-7, -.17955692847399102653642691446599e-7,
        -.41284072341950457727912394640436e-8, +.68622148588631968618346844526664e-9,
        +.53130183120506356147602009675961e-9, +.78796880261490694831305022893515e-10,
        -.26261762329356522290341675271232e-10, -.15483687636308261963125756294100e-10,
        -.25818962377261390492802405122591e-11, +.59542879191591072658903529959352e-12,
        +.46451400387681525833784919321405e-12, +.11557855023255861496288006203731e-12,
        -.10475236870835799012317547189670e-14, -.11896653502709004368104489260929e-13,
        -.47749077490261778752643019349950e-14, -.81077649615772777976249734754135e-15,
        +.13435569250031554199376987998178e-15, +.14134530022913106260248873881287e-15,
        +.49451592573953173115520663232883e-16, +.79884048480080665648858587399367e-17,
        -.14008632188089809829248711935393e-17, -.14814246958417372107722804001680e-17,
        -.55826173646025601904010693937113e-18, -.11442074542191647264783072544598e-18,
        +.25371823879566853500524018479923e-20, +.13205328154805359813278863389097e-19,
        +.62930261081586809166287426789485e-20, +.17688270424882713734999261332548e-20,
        +.23266187985146045209674296887432e-21, -.67803060811125233043773831844113e-22,
        -.59440876959676373802874150531891e-22, -.23618214531184415968532592503466e-22,
        -.60214499724601478214168478744576e-23, -.65517906474348299071370444144639e-24,
        +.29388755297497724587042038699349e-24, +.22601606200642115173215728758510e-24,
        +.89534369245958628745091206873087e-25, +.24015923471098457555772067457706e-25,
        +.34118376888907172955666423043413e-26, -.71617071694630342052355013345279e-27,
        -.75620390659281725157928651980799e-27, -.33774612157467324637952920780800e-27,
        -.10479325703300941711526430332245e-27, -.21654550252170342240854880201386e-28,
        -.75297125745288269994689298432000e-30, +.19103179392798935768638084000426e-29,
        +.11492104966530338547790728833706e-29, +.43896970582661751514410359193600e-30,
        +.12320883239205686471647157725866e-30, +.22220174457553175317538581162666e-31};
    int n = Initds( a, 60, eta );
    return exp(-x)/x * (1.0 + NTermChebyshevSeries((64.0/x+5.0)/3.0,a,n));
  }
  if( x <= -4.0 )
  {
    double const a[41] = {
        +.63629589796747038767129887806803e+0, -.13081168675067634385812671121135e+0,
        -.84367410213053930014487662129752e-2, +.26568491531006685413029428068906e-2,
        +.32822721781658133778792170142517e-3, -.23783447771430248269579807851050e-4,
        -.11439804308100055514447076797047e-4, -.14405943433238338455239717699323e-5,
        +.52415956651148829963772818061664e-8, +.38407306407844323480979203059716e-7,
        +.85880244860267195879660515759344e-8, +.10219226625855003286339969553911e-8,
        +.21749132323289724542821339805992e-10, -.22090238142623144809523503811741e-10,
        -.63457533544928753294383622208801e-11, -.10837746566857661115340539732919e-11,
        -.11909822872222586730262200440277e-12, -.28438682389265590299508766008661e-14,
        +.25080327026686769668587195487546e-14, +.78729641528559842431597726421265e-15,
        +.15475066347785217148484334637329e-15, +.22575322831665075055272608197290e-16,
        +.22233352867266608760281380836693e-17, +.16967819563544153513464194662399e-19,
        -.57608316255947682105310087304533e-19, -.17591235774646878055625369408853e-19,
        -.36286056375103174394755328682666e-20, -.59235569797328991652558143488000e-21,
        -.76030380926310191114429136895999e-22, -.62547843521711763842641428479999e-23,
        +.25483360759307648606037606400000e-24, +.25598615731739857020168874666666e-24,
        +.71376239357899318800207052800000e-25, +.14703759939567568181578956800000e-25,
        +.25105524765386733555198634666666e-26, +.35886666387790890886583637333333e-27,
        +.39886035156771301763317759999999e-28, +.21763676947356220478805333333333e-29,
        -.46146998487618942367607466666666e-30, -.20713517877481987707153066666666e-30,
        -.51890378563534371596970666666666e-31};
    int n = Initds( a, 41, eta );
    return exp(-x)/x * (1.0 + NTermChebyshevSeries(16.0/x+3.0,a,n));
  }
  else
  {
    double const a[29] = {
        -.16113461655571494025720663927566180e+2, +.77940727787426802769272245891741497e+1,
        -.19554058188631419507127283812814491e+1, +.37337293866277945611517190865690209e+0,
        -.56925031910929019385263892220051166e-1, +.72110777696600918537847724812635813e-2,
        -.78104901449841593997715184089064148e-3, +.73880933562621681878974881366177858e-4,
        -.62028618758082045134358133607909712e-5, +.46816002303176735524405823868362657e-6,
        -.32092888533298649524072553027228719e-7, +.20151997487404533394826262213019548e-8,
        -.11673686816697793105356271695015419e-9, +.62762706672039943397788748379615573e-11,
        -.31481541672275441045246781802393600e-12, +.14799041744493474210894472251733333e-13,
        -.65457091583979673774263401588053333e-15, +.27336872223137291142508012748799999e-16,
        -.10813524349754406876721727624533333e-17, +.40628328040434303295300348586666666e-19,
        -.14535539358960455858914372266666666e-20, +.49632746181648636830198442666666666e-22,
        -.16208612696636044604866560000000000e-23, +.50721448038607422226431999999999999e-25,
        -.15235811133372207813973333333333333e-26, +.44001511256103618696533333333333333e-28,
        -.12236141945416231594666666666666666e-29, +.32809216661066001066666666666666666e-31,
        -.84933452268306432000000000000000000e-33};
    int n = Initds( a, 29, eta );
    return -log(-x) + NTermChebyshevSeries((2.0*x+5.0)/3.0,a,n);
  }
}

double ExponentialIntegralN( double x, int n )
{
  // THE GENERALIZED EXPONENTIAL INTEGRAL,
  // EXPN(X,N)=E<N>(X) = INTEGRAL from 1 to infinity of
  //                 exp(-X*t)*t**(-N) dt;  X>0,  N=integer
  // expn
  //
  // RESTRICTIONS: X > 0,  N = POSITIVE INTEGER <= 20 {The upper limit
  //               of 20 may be increased by increasing dimension of FN }
  // REFERENCE: CACM ALGORITHM #471
  //
  if( x <= 0.0 )throw EExpressionError(wxT("ExponentialIntegralN: first argument <= 0") );
  if( n <= 0 )throw EExpressionError(wxT("ExponentialIntegralN: second argument <= 0") );
  if( n > 20 )throw EExpressionError(wxT("ExponentialIntegralN: second argument > 20") );
  //
  int const nacc  = 15; // accuracy parameter, no.of sig fig desired
  double fn[20];
  if( x <= 1.0 )
  {
    fn[0] = Fn1(x,nacc);
    for( int i=1; i<=n-1; ++i )fn[i] = (1.0-x*fn[i-1])/i;
    return fn[n-1]*exp(-x);
  }
  double eps = pow(10.0,static_cast<double>(-nacc));
  int n1 = static_cast<int>(x+0.5);
  double ue = 1.0;
  double ve = 1.0/(x+n1);
  double we = ve;
  double we1 = 0.0;
  double uo = 1.0;
  double vo = -n1/(x*(x+n1+1));
  double wo1 = 1.0/x;
  double wo = vo+wo1;
  double w = (we+wo)/2;
  for( int k=1;; ++k )
  {
    if( wo-we<=eps*w || we<=we1 || wo>=wo1 )break;
    we1 = we;
    wo1 = wo;
    ue = 1.0/(1-k*(n1+k-1)*ue/((n1+k*2+x-2)*(n1+k*2+x)));
    uo = 1.0/(1-k*(n1+k)*uo/((n1+k*2+x)*(n1+k*2+x)-1));
    ve *= ue-1;
    vo *= uo-1;
    we += ve;
    wo += vo;
    w = (we+wo)/2.0;
  }
  if( n1 <= n )fn[n1-1] = w;
  for( int i=n1-1; i>=1; --i )
  {
    w = (1-i*w)/x;
    if( i <= n )fn[i-1] = w;
  }
  for( int i=n1; i<=n-1; ++i )fn[i] = (1-x*fn[i-1])/i;
  return fn[n-1]*exp(-x);
}

double Fn1( double x, int n )
{
  double const gam = 0.577215664901532860606512; // Euler's constant
  double eps = 1.e-15;
  double se = 0.0;
  double se1 = -1.0;
  double so = x;
  double to = x;
  double so1 = 2.0*x;
  double s = x/2.0;
  for( int k=1;; ++k )
  {
    if( so-se<=eps*s || se<=se1 || so>=so1 )break;
    se1 = se;
    so1 = so;
    double te = (k*2-1)*x*to/(k*2*k*2);
    se += to-te;
    to = k*2*x*te/((k*2+1)*(k*2+1));
    so += to-te;
    s = (se+so)/2.0;
  }
  return (s-gam-log(x))*exp(x);
}

double FermiDirac( double x, double pp )
{
  // CALCULATES THE FERMI-DIRAC FUNCTION
  // FERDIR(X,pp)= INTEGRAL(t**pp/(exp(t-X)+1))dt, limits 0 to infinity
  // ferdir
  //
  // REFERENCE: CERN library routine C338
  // ARGUMENT RESTRICTION :
  // pp ----- ONE OF 3 VALUES ONLY: -1/2, 1/2, 3/2
  //
  if( pp!=-0.5 && pp!=0.5 && pp!=1.5 )
    throw EExpressionError(wxT("FermiDirac: second argument must be -1/2, 1/2, or 1.5") );
  //
  double xs, y;
  if( pp == 1.5 )
  {
    if( x > 4.0 )
    {
      double const p[] = {+2.46740023684e+0, +2.19167582368e+2, +1.23829379075e+4,
                          +2.20667724968e+5, +8.49442920034e+5};
      double const q[] = {+1.00000000000e+0, +8.91125140619e+1, +5.04575669667e+3,
                          +9.09075946304e+4, +3.89960915641e+5};
      xs = x*x;
      y = 1.0/xs;
      return xs*sqrt(x)*(0.4+y*(p[0]+y*(p[1]+y*(p[2]+y*(p[3]+y*p[4]))))/
             (q[0]+y*(q[1]+y*(q[2]+y*(q[3]+y*q[4])))));
    }
    else if( x > 1.0 )
    {
      double const p[] = {+1.1530213402e+0, +1.0591558972e+0, +4.6898803095e-1,
                          +1.1882908784e-1, +1.9438755787e-2};
      double const q[] = {+1.0000000000e+0, +3.7348953841e-2, +2.3248458137e-2,
                          -1.3766770874e-3, +4.6466392781e-5};
      return (p[0]+x*(p[1]+x*(p[2]+x*(p[3]+x*p[4]))))/
             (q[0]+x*(q[1]+x*(q[2]+x*(q[3]+x*q[4]))));
    }
    else
    {
      double const p[] = {-2.349963985406e-1, -2.927373637547e-1, -9.883097588738e-2,
                          -8.251386379551e-3, -1.874384153223e-5};
      double const q[] = {+1.000000000000e+0, +1.608597109146e+0, +8.275289530880e-1,
                          +1.522322382850e-1, +7.695120475064e-3};
      y = exp(x);
      return y*(1.329340388179137+y*(p[0]+y*(p[1]+y*(p[2]+y*(p[3]+y*p[4]))))/
             (q[0]+y*(q[1]+y*(q[2]+y*(q[3]+y*q[4])))));
    }
  }
  else if( pp == 0.5 )
  {
    if( x > 4.0 )
    {
      double const p[] = {+8.2244997626e-1, +2.0046303393e+1, +1.8268093446e+3,
                          +1.2226530374e+4, +1.4040750092e+5};
      double const q[] = {+1.0000000000e+0, +2.3486207659e+1, +2.2013483743e+3,
                          +1.1442673596e+4, +1.6584715900e+5};
      y = 1.0/x/x;
      return x*sqrt(x)*(0.666666666666667+y*(p[0]+y*(p[1]+y*(p[2]+y*(p[3]+y*p[4]))))/
             (q[0]+y*(q[1]+y*(q[2]+y*(q[3]+y*q[4])))));
    }
    else if( x > 1.0 )
    {
      double const p[] = {+6.78176626660e-1, +6.33124017910e-1, +2.94479651772e-1,
                          +8.01320711419e-2, +1.33918212940e-2};
      double const q[] = {+1.00000000000e+0, +1.43740400397e-1, +7.08662148450e-2,
                          +2.34579494735e-3, -1.29449928835e-5};
      return (p[0]+x*(p[1]+x*(p[2]+x*(p[3]+x*p[4]))))/
             (q[0]+x*(q[1]+x*(q[2]+x*(q[3]+x*q[4]))));
    }
    else
    {
      double const p[] = {-3.133285305570e-1, -4.161873852293e-1, -1.502208400588e-1,
                          -1.339579375173e-2, -1.513350700138e-5};
      double const q[] = {+1.000000000000e+0, +1.872608675902e+0, +1.145204446578e+0,
                          +2.570225587573e-1, +1.639902543568e-2};
      y = exp(x);
      return y*(0.886226925452758+y*(p[0]+y*(p[1]+y*(p[2]+y*(p[3]+y*p[4]))))/
             (q[0]+y*(q[1]+y*(q[2]+y*(q[3]+y*q[4])))));
    }
  }
  else
  {
    if( x > 4.0 )
    {
      double const p[] = {-8.222559330e-1, -3.620369345e+1, -3.015385410e+3,
                          -7.049871579e+4, -5.698145924e+4};
      double const q[] = {+1.000000000e+0, +3.935689841e+1, +3.568756266e+3,
                          +4.181893625e+4, +3.385138907e+5};
      y = 1.0/x/x;
      return sqrt(x)*(2.0+y*(p[0]+y*(p[1]+y*(p[2]+y*(p[3]+y*p[4]))))/
             (q[0]+y*(q[1]+y*(q[2]+y*(q[3]+y*q[4])))));
    }
    else if( x > 1.0 )
    {
      double const p[] = {+1.0738127694e+0, +5.6003303660e+0, +3.6882211270e+0,
                          +1.1743392816e+0, +2.3641935527e-1};
      double const q[] = {+1.0000000000e+0, +4.6031840667e+0, +4.3075910674e-1,
                          +4.2151132145e-1, +1.1832601601e-2};
      return (p[0]+x*(p[1]+x*(p[2]+x*(p[3]+x*p[4]))))/
             (q[0]+x*(q[1]+x*(q[2]+x*(q[3]+x*q[4]))));
    }
    else
    {
      double const p[] = {-1.253314128820E+0, -1.723663557701E+0, -6.559045729258E-1,
                          -6.342283197682E-2, -1.488383106116E-5};
      double const q[] = {+1.000000000000e+0, +2.191780925980e+0, +1.605812955406e+0,
                          +4.443669527481e-1, +3.624232288112e-2};
      y = exp(x);
      return y*(1.772453850905516+y*(p[0]+y*(p[1]+y*(p[2]+y*(p[3]+y*p[4]))))/
             (q[0]+y*(q[1]+y*(q[2]+y*(q[3]+y*q[4])))));
    }
  }
}

double FisherDistribution( int m, int n, double x )
{
  // COMPUTES FISHER'S F-DISTRIBUTION WITH M AND N DEGREES OF FREEDOM, i.e.
  // THE PROBILITY FISHER(M,N,X) = GAMMA((M+N)/2) / (GAMMA(M/2)*GAMMA(N/2)) *
  //   INTEGRAL from 0 to w of t**(M/2-1)/(t+1)**((M+N)/2) dt where w=(M/N)*X
  // fisher
  //
  // THE F-DISTRIBUTION IS ALSO KNOWN AS THE VARIANCE-RATIO DISTRIBUTION
  // OR SNEDECOR'S DISTRIBUTION
  // REFERENCE: CACM ALGORITHM #322
  // ACCURACY: TO AT LEAST 1.0E-3 for 1<=M<=60, 1<=N<=30
  //
  int a = 2*(m/2)-m+2;
  int b = 2*(n/2)-n+2;
  double w = x*m/n;
  double z = 1.0/(1.0+w);
  double y, p, d;
  if( a != 1 )
  {
    if( b != 1 )
    {
      d = z*z;
      p = w*z;
    }
    else
    {
      p = sqrt(z);
      d = 0.5*z*p;
      p = 1.0-p;
    }
  }
  else
  {
    if( b != 1 )
    {
      p = sqrt(w*z);
      d = p*z/w*0.5;
    }
    else
    {
      p = sqrt(w);
      y = 1.0/M_PI;
      d = y*z/p;
      p = 2.0*y*atan(p);
    }
  }
  y = 2.0*w/z;
  if( a != 1 )
  {
    double zk = pow(z,(n-1.0)/2.0);
    d *= zk*n/b;
    p *= zk;
    p += w*z*(zk-1.0)/(z-1.0);
  }
  else
  {
    for( int j=b+2; j<=n; j+=2 )
    {
      d = (1.0+a/(j-2.0))*d*z;
      p += d*y/(j-1);
    }
  }
  y = w*z;
  z = 2.0/z;
  b = n-2;
  for( int i=a+2; i<=m; i+=2 )
  {
    d *= y*(i+b)/(i-2.0);
    p -= z*d/(i+b);
  }
  return p;
}

double FresnelC( double x )
{
  // FRESNEL INTEGRAL C(X)
  // C(X) = INTEGRAL(cos(pi*t*t/2))dt, limits 0 to X
  // frec1
  //
  // REFERENCE : CERN library routine C309
  // ACCURACY: TO AT LEAST 1.0E-6 IN TESTED REGION
  // TESTED REGION :  0.00 <= X <= 5.0
  //
  double t = x*x;
  double z, y, a, b, result;
  if( t < 9.0 )
  {
    z = t/9.0;
    y = 4.0*z*z-2.0;
    a =      -0.000000000000003;
    b = y*a  +0.000000000000076;
    a = y*b-a-0.000000000001875;
    b = y*a-b+0.000000000041213;
    a = y*b-a-0.000000000795550;
    b = y*a-b+0.000000013357248;
    a = y*b-a-0.000000192850090;
    b = y*a-b+0.000002361906788;
    a = y*b-a-0.000024136195791;
    b = y*a-b+0.000201616882443;
    a = y*b-a-0.001341081352431;
    b = y*a-b+0.006861115379812;
    a = y*b-a-0.025736708168279;
    b = y*a-b+0.066060370528389;
    a = y*b-a-0.105139484620109;
    b = y*a-b+0.097246391833287;
    a = y*b-a-0.092641852979503;
    b = y*a-b+0.147534155215236;
    a = y*b-a-0.174163078153421;
    a = y*a-b+0.566094879476909;
    result = x*0.5*(a-b);
  }
  else
  {
    z = 9.0/t;
    y = 4.0*z*z-2.0;
    a =      -0.000000000000001;
    b = y*a  +0.000000000000008;
    a = y*b-a-0.000000000000053;
    b = y*a-b+0.000000000000381;
    a = y*b-a-0.000000000003145;
    b = y*a-b+0.000000000030716;
    a = y*b-a-0.000000000370990;
    b = y*a-b+0.000000005908966;
    a = y*b-a-0.000000137398906;
    b = y*a-b+0.000005571891859;
    a = y*b-a-0.000573621372272;
    a = y*a-b+0.635461098412986;
    double aa = 0.5*(a-b)/x;
    a =      -0.000000000000002;
    b = y*a  +0.000000000000008;
    a = y*b-a-0.000000000000050;
    b = y*a-b+0.000000000000330;
    a = y*b-a-0.000000000002478;
    b = y*a-b+0.000000000021678;
    a = y*b-a-0.000000000228879;
    b = y*a-b+0.000000003074598;
    a = y*b-a-0.000000056963997;
    b = y*a-b+0.000001663169852;
    a = y*b-a-0.000098395902454;
    a = y*a-b+0.022315579858535;
    double bb = 0.5*(a-b)*z/x;
    z = 1.570796326794897*t;
    t = sin(z);
    z = cos(z);
    x < 0.0 ? a=-0.5 : a=0.5;
    result = a+t*aa-z*bb;
  }
  return result;
}

double FresnelCAssociated( double xx )
{
  // THE ASSOCIATED FRESNEL INTEGRAL C2(X)
  // C2(X) = 1/sqrt(2*pi)*INTEGRAL(cos(t)/sqrt(t))dt, limits 0 to X
  // frec2
  //
  // REFERENCE : CERN library routine C309
  // ACCURACY: TO AT LEAST 1.0E-6 IN TESTED REGION
  // TESTED REGION : 0.00 < =  X < =  6.28 (2*PI)
  //
  double x = 0.797884560802865*sqrt(fabs(xx));
  if( xx < 0.0 )x *= -1.0;
  double t = x*x;
  double z, y, a, b, result;
  if( t < 9.0 )
  {
    z = t/9.0;
    y = 4.0*z*z-2.0;
    a =      -0.000000000000003;
    b = y*a  +0.000000000000076;
    a = y*b-a-0.000000000001875;
    b = y*a-b+0.000000000041213;
    a = y*b-a-0.000000000795550;
    b = y*a-b+0.000000013357248;
    a = y*b-a-0.000000192850090;
    b = y*a-b+0.000002361906788;
    a = y*b-a-0.000024136195791;
    b = y*a-b+0.000201616882443;
    a = y*b-a-0.001341081352431;
    b = y*a-b+0.006861115379812;
    a = y*b-a-0.025736708168279;
    b = y*a-b+0.066060370528389;
    a = y*b-a-0.105139484620109;
    b = y*a-b+0.097246391833287;
    a = y*b-a-0.092641852979503;
    b = y*a-b+0.147534155215236;
    a = y*b-a-0.174163078153421;
    a = y*a-b+0.566094879476909;
    result = x*0.5*(a-b);
  }
  else
  {
    z = 9.0/t;
    y = 4.0*z*z-2.0;
    a =      -0.000000000000001;
    b = y*a  +0.000000000000008;
    a = y*b-a-0.000000000000053;
    b = y*a-b+0.000000000000381;
    a = y*b-a-0.000000000003145;
    b = y*a-b+0.000000000030716;
    a = y*b-a-0.000000000370990;
    b = y*a-b+0.000000005908966;
    a = y*b-a-0.000000137398906;
    b = y*a-b+0.000005571891859;
    a = y*b-a-0.000573621372272;
    a = y*a-b+0.635461098412986;
    double aa = 0.5*(a-b)/x;
    a =      -0.000000000000002;
    b = y*a  +0.000000000000008;
    a = y*b-a-0.000000000000050;
    b = y*a-b+0.000000000000330;
    a = y*b-a-0.000000000002478;
    b = y*a-b+0.000000000021678;
    a = y*b-a-0.000000000228879;
    b = y*a-b+0.000000003074598;
    a = y*b-a-0.000000056963997;
    b = y*a-b+0.000001663169852;
    a = y*b-a-0.000098395902454;
    a = y*a-b+0.022315579858535;
    double bb = 0.5*(a-b)*z/x;
    z = 1.570796326794897*t;
    t = sin(z);
    z = cos(z);
    x < 0.0 ? a=-0.5 : a=0.5;
    result = a+t*aa-z*bb;
  }
  return result;
}

double FresnelS( double x )
{
  // THE FRESNEL INTEGRAL S(X)
  // S(X) = INTEGRAL(sin(pi*(t**2)/2))dt, limits 0 to X
  // fres1
  // REFERENCE: CERN library routine C309
  // ACCURACY: TO AT LEAST 1.0E-6 IN TESTED REGION
  // REGION TESTED: 0.0 <= XX <= 5.00
  //
  double t = x*x;
  double z, y, a, b, result;
  if( t < 9.0 )
  {
    z = t/9.0;
    y = 4.0*z*z-2.0;
    a =      -0.000000000000001;
    b = y*a  +0.000000000000030;
    a = y*b-a-0.000000000000794;
    b = y*a-b+0.000000000018654;
    a = y*b-a-0.000000000386931;
    b = y*a-b+0.000000007025677;
    a = y*b-a-0.000000110548467;
    b = y*a-b+0.000001489719660;
    a = y*b-a-0.000016954178157;
    b = y*a-b+0.000160243443651;
    a = y*b-a-0.001232314420465;
    b = y*a-b+0.007517763479240;
    a = y*b-a-0.035248288029314;
    b = y*a-b+0.122191066602012;
    a = y*b-a-0.300398786877130;
    b = y*a-b+0.515461606559411;
    a = y*b-a-0.688881695298469;
    b = y*a-b+0.926493976989515;
    a = y*b-a-1.247697507291387;
    a = y*a-b+1.734174339031447;
    result = x*z*0.5*(a-b);
  }
  else
  {
    z = 9.0/t;
    y = 4.0*z*z-2.0;
    a =      -0.000000000000001;
    b = y*a  +0.000000000000008;
    a = y*b-a-0.000000000000053;
    b = y*a-b+0.000000000000381;
    a = y*b-a-0.000000000003145;
    b = y*a-b+0.000000000030716;
    a = y*b-a-0.000000000370990;
    b = y*a-b+0.000000005908966;
    a = y*b-a-0.000000137398906;
    b = y*a-b+0.000005571891859;
    a = y*b-a-0.000573621372272;
    a = y*a-b+0.635461098412986;
    double aa = 0.5*(a-b)/x;
    a =      -0.000000000000002;
    b = y*a  +0.000000000000008;
    a = y*b-a-0.000000000000050;
    b = y*a-b+0.000000000000330;
    a = y*b-a-0.000000000002478;
    b = y*a-b+0.000000000021678;
    a = y*b-a-0.000000000228879;
    b = y*a-b+0.000000003074598;
    a = y*b-a-0.000000056963997;
    b = y*a-b+0.000001663169852;
    a = y*b-a-0.000098395902454;
    a = y*a-b+0.022315579858535;
    double bb = 0.5*(a-b)*z/x;
    z = 1.570796326794897*t;
    t = sin(z);
    z = cos(z);
    x < 0.0 ? a=-0.5 : a=0.5;
    result = a-t*bb-z*aa;
  }
  return result;
}

double FresnelSAssociated( double xx )
{
  // THE ASSOCIATED FRESNEL INTEGRAL S2(X)
  // S2(X)= 1/sqrt(2*pi)*INTEGRAL(sin(t)/sqrt(t))dt, limits 0 to X
  // fres2
  //
  // REFERENCE: CERN library routine C309
  // ACCURACY: TO AT LEAST 1.0E-6 IN TESTED REGION
  // REGION TESTED: 0.0 <= X <= 6.28 (2* pi)
  //
  double x = 0.797884560802865*sqrt(fabs(xx));
  if( xx < 0.0 )x *= -1.0;
  double t = x*x;
  double z, y, a, b, result;
  if( t < 9.0 )
  {
    z = t/9.0;
    y = 4.0*z*z-2.0;
    a =      -0.000000000000001;
    b = y*a  +0.000000000000030;
    a = y*b-a-0.000000000000794;
    b = y*a-b+0.000000000018654;
    a = y*b-a-0.000000000386931;
    b = y*a-b+0.000000007025677;
    a = y*b-a-0.000000110548467;
    b = y*a-b+0.000001489719660;
    a = y*b-a-0.000016954178157;
    b = y*a-b+0.000160243443651;
    a = y*b-a-0.001232314420465;
    b = y*a-b+0.007517763479240;
    a = y*b-a-0.035248288029314;
    b = y*a-b+0.122191066602012;
    a = y*b-a-0.300398786877130;
    b = y*a-b+0.515461606559411;
    a = y*b-a-0.688881695298469;
    b = y*a-b+0.926493976989515;
    a = y*b-a-1.247697507291387;
    a = y*a-b+1.734174339031447;
    result = x*z*0.5*(a-b);
  }
  else
  {
    z = 9.0/t;
    y = 4.0*z*z-2.0;
    a =      -0.000000000000001;
    b = y*a  +0.000000000000008;
    a = y*b-a-0.000000000000053;
    b = y*a-b+0.000000000000381;
    a = y*b-a-0.000000000003145;
    b = y*a-b+0.000000000030716;
    a = y*b-a-0.000000000370990;
    b = y*a-b+0.000000005908966;
    a = y*b-a-0.000000137398906;
    b = y*a-b+0.000005571891859;
    a = y*b-a-0.000573621372272;
    a = y*a-b+0.635461098412986;
    double aa = 0.5*(a-b)/x;
    a =      -0.000000000000002;
    b = y*a  +0.000000000000008;
    a = y*b-a-0.000000000000050;
    b = y*a-b+0.000000000000330;
    a = y*b-a-0.000000000002478;
    b = y*a-b+0.000000000021678;
    a = y*b-a-0.000000000228879;
    b = y*a-b+0.000000003074598;
    a = y*b-a-0.000000056963997;
    b = y*a-b+0.000001663169852;
    a = y*b-a-0.000098395902454;
    a = y*a-b+0.022315579858535;
    double bb = 0.5*(a-b)*z/x;
    z = 1.570796326794897*t;
    t = sin(z);
    z = cos(z);
    x < 0.0 ? a=-0.5 : a=0.5;
    result = a-t*bb-z*aa;
  }
  return result;
}

double Ber( double x )
{
  // COMPUTES THE KELVIN FUNCTION: ber<v>x
  // where  ber<v>x + i bei<v>x = J<v>(x*exp(i*3*pi/4))
  // J<v>(x) = BESSEL FUNCTION OF THE FIRST KIND OF ORDER v
  // IN THIS CASE v = 0.
  // kelvin
  //
  // REFERENCE: CACM ALGORITHM #57
  // ACCURACY: AT LEAST 10 SIGNIFICANT FIGURES ARE CORRECT
  //           WHEN 0.0 <= X <= 5.0
  //           FOR LARGE VALUES OF X, ACCURACY WILL BE LOST
  //
  if( x <= 0.0 )throw EExpressionError(wxT("Ber: argument must be positive"));
  double s = 1.0;
  double a = 1.0;
  for( int i=2; i<=100; ++(++i) )
  {
    if( a == 0.0 )break;
    a *= -0.0625*x*x*x*x/(i*(i-1)*i*(i-1));
    s += a;
  }
  return s;
}

double Bei( double x )
{
  // COMPUTES THE KELVIN FUNCTION: bei<v>x
  // where  ber<v>x + i bei<v>x = J<v>(x*exp(i*3*pi/4))
  // J<v>(x) = BESSEL FUNCTION OF THE FIRST KIND OF ORDER v
  // IN THIS CASE v = 0.
  // kelvin
  //
  // REFERENCE: CACM ALGORITHM #57
  // ACCURACY: AT LEAST 10 SIGNIFICANT FIGURES ARE CORRECT
  //           WHEN 0.0 <= X <= 5.0
  //           FOR LARGE VALUES OF X, ACCURACY WILL BE LOST
  //
  if( x <= 0.0 )throw EExpressionError(wxT("Bei: argument must be positive"));
  double s = x*x/4.0;
  double a = s;
  for( int i=2; i<=100; ++(++i) )
  {
    if( a == 0.0 )break;
    a *= -0.0625*x*x*x*x/((i+1)*i*(i+1)*i);
    s += a;
  }
  return s;
}

double Ker( double x )
{
  // REAL PART OF THE KELVIN FUNCTION OF THE SECOND KIND, OF ORDER ZERO
  // kel2
  //
  // ARCUMENT RESTRICTION: X >= 0
  // REFERENCE: IMSL LIBRARY ROUTINE: MMKEL0
  //
  double const xinf = std::numeric_limits<double>::max(); // 1.700d+38 == infinity
  if( x == 0.0 )return xinf;
  if( x < 0.0 )throw EExpressionError(wxT("Ker: argument must be non-negative"));
  if( x > 119.0 )throw EExpressionError(wxT("Ker: argument must be <= 119"));
  //
  if( x <= 10.0 )
  {
    double const c1[9] = {5.16070465e-5,     -4.8987125727e-3,    0.25977730007,
                         -7.2422567278207,   93.8596692971726, -470.9502795889968,
                        678.1684027663091, -156.2499999995701,    0.9999999999974};
    double const c2[9] = {4.491300e-6,       -5.444243175e-4,     3.84288282734e-2,
                         -1.4963342749742,   28.9690338786499, -240.2807549442574,
                        678.1684027769807, -434.0277777777479,   24.9999999999998};
    double const c3[9] = {1.54363047e-5,    -1.806477786e-3,    0.1222087382192,
                         -4.5187459132639,  81.952477160620, -623.0136717405201,
                       1548.484519673099, -795.7175925924866,  24.9999999999993};
    double const c4[9] = {1.2161109e-6,      -1.797627986e-4,     1.59380149705e-2,
                         -0.8061529027876,   21.2123451660231, -255.0971742710479,
                       1153.828185281456, -1412.850839120364,   234.375};
    double const euler = 0.5772156649015329; // euler's constant
    x /= 10.0;
    double z4 = x*x*x*x;
    double b1 = c1[0];
    double b2 = c2[0];
    double b3 = c3[0];
    double b4 = c4[0];
    for( int i=1; i<9; ++i )
    {
      b1 = b1*z4 + c1[i];
      b2 = b2*z4 + c2[i];
      b3 = b3*z4 + c3[i];
      b4 = b4*z4 + c4[i];
    }
    return M_PI/2*x*x*b2/2-(z4*b4+b1*(log(x/2)+euler));
  }
  else
  {
    double const e1[9] = {4.92e-8, 1.452e-7, 1.35e-8, -1.6192e-6, -1.12207e-5,
                         -5.17869e-5, 7e-10, 8.8388346e-3, 1.0};
    double const e2[9] = {-2.43e-8, 7.5e-8, 5.929e-7, 1.6431e-6, -7.2e-9,
                          -5.18006e-5, -7.031241e-4, -8.838834e-3, 0.0};
    double zi = 10.0/x;
    double zim = -zi;
    double s = e1[0];
    double sm = s;
    double t = e2[0];
    double tm = t;
    for( int i=1; i<9; ++i )
    {
      s = s*zi + e1[i];
      t = t*zi + e2[i];
      sm = sm*zim + e1[i];
      tm = tm*zim + e2[i];
    }
    double arg = x*sqrt(2.);
    return M_PI*(sm*cos(arg+M_PI/8)+tm*sin(arg+M_PI/8))/(exp(arg)*sqrt(M_PI*2*x));
  }
}

double Kei( double x )
{
  // IMAGINARY PART OF THE KELVIN FUNCTION OF THE SECOND KIND, OF ORDER ZERO
  // kel2
  //
  // ARCUMENT RESTRICTION: X >= 0
  // REFERENCE: IMSL LIBRARY ROUTINE: MMKEL0
  //
  double const xinf = std::numeric_limits<double>::max(); // 1.700d+38 == infinity
  if( x == 0.0 )return xinf;
  if( x < 0.0 )throw EExpressionError(wxT("Kei: argument must be non-negative"));
  if( x > 119.0 )throw EExpressionError(wxT("Kei: argument must be <= 119"));
  //
  if( x <= 10.0 )
  {
    double const c1[9] = {5.16070465e-5,     -4.8987125727e-3,    0.25977730007,
                         -7.2422567278207,   93.8596692971726, -470.9502795889968,
                        678.1684027663091, -156.2499999995701,    0.9999999999974};
    double const c2[9] = {4.491300e-6,       -5.444243175e-4,     3.84288282734e-2,
                         -1.4963342749742,   28.9690338786499, -240.2807549442574,
                        678.1684027769807, -434.0277777777479,   24.9999999999998};
    double const c3[9] = {1.54363047e-5,    -1.806477786e-3,    0.1222087382192,
                         -4.5187459132639,  81.952477160620, -623.0136717405201,
                       1548.484519673099, -795.7175925924866,  24.9999999999993};
    double const c4[9] = {1.2161109e-6,      -1.797627986e-4,     1.59380149705e-2,
                         -0.8061529027876,   21.2123451660231, -255.0971742710479,
                       1153.828185281456, -1412.850839120364,   234.375};
    double const euler = 0.5772156649015329; // euler's constant
    x /= 10.0;
    double z4 = x*x*x*x;
    double b1 = c1[0];
    double b2 = c2[0];
    double b3 = c3[0];
    double b4 = c4[0];
    for( int i=1; i<9; ++i )
    {
      b1 = b1*z4 + c1[i];
      b2 = b2*z4 + c2[i];
      b3 = b3*z4 + c3[i];
      b4 = b4*z4 + c4[i];
    }
    return -M_PI*b1/4+(x*x*b3-x*x*b2*(log(x/2)+euler));
  }
  else
  {
    double const e1[9] = {4.92e-8, 1.452e-7, 1.35e-8, -1.6192e-6, -1.12207e-5,
                         -5.17869e-5, 7e-10, 8.8388346e-3, 1.0};
    double const e2[9] = {-2.43e-8, 7.5e-8, 5.929e-7, 1.6431e-6, -7.2e-9,
                          -5.18006e-5, -7.031241e-4, -8.838834e-3, 0.0};
    double zi = 10.0/x;
    double zim = -zi;
    double s = e1[0];
    double sm = s;
    double t = e2[0];
    double tm = t;
    for( int i=1; i<9; ++i )
    {
      s = s*zi + e1[i];
      t = t*zi + e2[i];
      sm = sm*zim + e1[i];
      tm = tm*zim + e2[i];
    }
    double arg = x*sqrt(2.);
    return M_PI*(tm*cos(arg+M_PI/8)-sm*sin(arg+M_PI/8))/(exp(arg)*sqrt(M_PI*2*x));
  }
}

double ClebschGordanCoefficient( double a, double b, double c,
                                                  double xx, double yy, double zz )
{
  int k1 = static_cast<int>(2*a+Sign(0.10,a));
  int k2 = static_cast<int>(2*b+Sign(0.10,b));
  int k3 = static_cast<int>(2*c+Sign(0.10,c));
  int k4 = static_cast<int>(2*xx+Sign(0.10,xx));
  int k5 = static_cast<int>(2*yy+Sign(0.10,yy));
  int k6 = static_cast<int>(2*zz+Sign(0.10,zz));
  //
  double h[101];
  int j[101];
  h[0] = 1.0;
  j[0] = 0;
  double x = 0.0;
  for( int i=1; i<101; ++i )
  {
    x += 1.0;
    h[i] = h[i-1]*x;
    j[i] = j[i-1];
    while( h[i] >= 10.0 )
    {
      h[i] *= 0.01;
      j[i] += 2;
    }
  }
  double result = 0.0;
  if( k4+k5-k6 != 0 )return result;
  int m1 = k1+k2-k3;
  int m2 = k2+k3-k1;
  int m3 = k3+k1-k2;
  int m4 = k1+k4;
  int m5 = k1-k4;
  int m6 = k2+k5;
  int m7 = k2-k5;
  int m8 = k3+k6;
  int m9 = k3-k6;
  int m10 = k1+k2+k3+2;
  //
  if( m1<0 || m2<0 || m3<0 || m4<0 || m5<0 || m6<0 || m7<0 || m8<0 || m9<0 ||
      m4-(m4/2)-(m4/2)!=0 || m4-(m4/2)-(m4/2)!=0 || m4-(m4/2)-(m4/2)!=0 ||
      m10-(m10/2)-(m10/2)!=0 )return result;
  //
  double y = k3+1;
  m1 = m1/2+1;
  m2 = m2/2+1;
  m3 = m3/2+1;
  m4 = m4/2+1;
  m5 = m5/2+1;
  m6 = m6/2+1;
  m7 = m7/2+1;
  m8 = m8/2+1;
  m9 = m9/2+1;
  m10 = m10/2+1;
  //
  y = sqrt(y*h[m1-1]*h[m2-1]*h[m3-1]*h[m4-1]*h[m5-1]*h[m6-1]*h[m7-1]*
           h[m8-1]*h[m9-1]/h[m10-1]);
  int iy = (j[m1-1]+j[m2-1]+j[m3-1]+j[m4-1]+j[m5-1]+j[m6-1]+j[m7-1]+
            j[m8-1]+j[m9-1]-j[m10-1])/2;
  //
  int n4 = m1;
  if( n4 > m5 )n4 = m5;
  if( n4 > m6 )n4 = m6;
  --n4;
  m2 = k2-k3-k4;
  m3 = k1+k5-k3;
  int n5 = 0;
  if( n5 < m2 )n5 = m2;
  if( n5 < m3 )n5 = m3;
  int n5par = 4*(n5/4)-n5+1;
  n5 /= 2;
  double z = 0.0;
  while( n5 <= n4 )
  {
    int mm1 = m1-n5;
    int mm2 = m5-n5;
    int mm3 = m6-n5;
    int mm4 = n5-(m2/2)+1;
    int mm5 = n5-(m3/2)+1;
    //
    double x = 1.0/(h[mm1-1]*h[mm2-1]*h[mm3-1]*h[mm4-1]*h[mm5-1]*h[n5]);
    int ix = -j[mm1-1]-j[mm2-1]-j[mm3-1]-j[mm4-1]-j[mm5-1]-j[n5];
    //
    while( ix+iy < 0 )
    {
      x *= 0.1;
      ++ix;
    }
    while( ix+iy > 0 )
    {
      x *= 10.0;
      --ix;
    }
    if( n5par < 0 )x *= -1.;
    z += x;
    n5par *= -1;
    ++n5;
  }
  return z*y;
}

double Wigner3JSymbol( double a, double b, double c,
                                        double xx, double yy, double zz )
{
  zz *= -1.;
  int k1 = static_cast<int>(2*a+Sign(0.10,a));
  int k2 = static_cast<int>(2*b+Sign(0.10,b));
  int k3 = static_cast<int>(2*c+Sign(0.10,c));
  int k4 = static_cast<int>(2*xx+Sign(0.10,xx));
  int k5 = static_cast<int>(2*yy+Sign(0.10,yy));
  int k6 = static_cast<int>(2*zz+Sign(0.10,zz));
  //
  double h[101];
  int j[101];
  h[0] = 1.0;
  j[0] = 0;
  double x = 0.0;
  for( int i=1; i<101; ++i )
  {
    x += 1.0;
    h[i] = h[i-1]*x;
    j[i] = j[i-1];
    while( h[i] >= 10.0 )
    {
      h[i] *= 0.01;
      j[i] += 2;
    }
  }
  double result = 0.0;
  if( k4+k5-k6 != 0 )return result;
  int m1 = k1+k2-k3;
  int m2 = k2+k3-k1;
  int m3 = k3+k1-k2;
  int m4 = k1+k4;
  int m5 = k1-k4;
  int m6 = k2+k5;
  int m7 = k2-k5;
  int m8 = k3+k6;
  int m9 = k3-k6;
  int m10 = k1+k2+k3+2;
  //
  if( m1<0 || m2<0 || m3<0 || m4<0 || m5<0 || m6<0 || m7<0 || m8<0 || m9<0 ||
      m4-(m4/2)-(m4/2)!=0 || m4-(m4/2)-(m4/2)!=0 || m4-(m4/2)-(m4/2)!=0 ||
      m10-(m10/2)-(m10/2)!=0 )return result;
  //
  double y = k3+1;
  m1 = m1/2+1;
  m2 = m2/2+1;
  m3 = m3/2+1;
  m4 = m4/2+1;
  m5 = m5/2+1;
  m6 = m6/2+1;
  m7 = m7/2+1;
  m8 = m8/2+1;
  m9 = m9/2+1;
  m10 = m10/2+1;
  //
  y = sqrt(y*h[m1-1]*h[m2-1]*h[m3-1]*h[m4-1]*h[m5-1]*h[m6-1]*h[m7-1]*
           h[m8-1]*h[m9-1]/h[m10-1]);
  int iy = (j[m1-1]+j[m2-1]+j[m3-1]+j[m4-1]+j[m5-1]+j[m6-1]+j[m7-1]+
            j[m8-1]+j[m9-1]-j[m10-1])/2;
  //
  int n4 = m1;
  if( n4 > m5 )n4 = m5;
  if( n4 > m6 )n4 = m6;
  --n4;
  m2 = k2-k3-k4;
  m3 = k1+k5-k3;
  int n5 = 0;
  if( n5 < m2 )n5 = m2;
  if( n5 < m3 )n5 = m3;
  int n5par = 4*(n5/4)-n5+1;
  n5 /= 2;
  double z = 0.0;
  while( n5 <= n4 )
  {
    int mm1 = m1-n5;
    int mm2 = m5-n5;
    int mm3 = m6-n5;
    int mm4 = n5-(m2/2)+1;
    int mm5 = n5-(m3/2)+1;
    //
    double x = 1.0/(h[mm1-1]*h[mm2-1]*h[mm3-1]*h[mm4-1]*h[mm5-1]*h[n5]);
    int ix = -j[mm1-1]-j[mm2-1]-j[mm3-1]-j[mm4-1]-j[mm5-1]-j[n5];
    //
    while( ix+iy < 0 )
    {
      x *= 0.1;
      ++ix;
    }
    while( ix+iy > 0 )
    {
      x *= 10.0;
      --ix;
    }
    if( n5par < 0 )x *= -1.;
    z += x;
    n5par *= -1;
    ++n5;
  }
  int js = k1-k2+k6;
  if( js < 0 )js *= -1;
  return (4*(js/4)-js+1)*z*y/sqrt(k3+1.0);
}

double Wigner6JSymbol( double a, double b, double c,
                       double xx, double yy, double zz )
{
  double h[101];
  int j[101];
  h[0] = 1.0;
  j[0] = 0;
  double x = 0.0;
  for( int i=1; i<101; ++i )
  {
    x += 1.0;
    h[i] = h[i-1]*x;
    j[i] = j[i-1];
    while( h[i] >= 10.0 )
    {
      h[i] *= 0.01;
      j[i] += 2;
    }
  }
  int k1 = static_cast<int>(2*a+Sign(0.10,a));
  int k2 = static_cast<int>(2*b+Sign(0.10,b));
  int k3 = static_cast<int>(2*c+Sign(0.10,c));
  int k4 = static_cast<int>(2*xx+Sign(0.10,xx));
  int k5 = static_cast<int>(2*yy+Sign(0.10,yy));
  int k6 = static_cast<int>(2*zz+Sign(0.10,zz));
  int ka = k1;
  int kb = k2;
  int kc = k3;
  //
  // triangle function
  //
  double ay[4];
  int iay[4];
  for( int i=1; i<=4; ++i )
  {
    int ma = ka+kb-kc;
    int mb = ka-kb+kc;
    int mc = -ka+kb+kc;
    int md = ka+kb+kc+2;
    if( ma<0 || mb<0 || mc<0 || md-(md/2)-(md/2)!=0 )return 0.0;
    ma = ma/2+1;
    mb = mb/2+1;
    mc = mc/2+1;
    md = md/2+1;
    ay[i-1] = sqrt(h[ma-1]*h[mb-1]*h[mc-1]/h[md-1]);
    iay[i-1] = (j[ma-1]+j[mb-1]+j[mc-1]-j[md-1])/2;
    switch (i)
    {
      case 1:
        ka = k4;
        kb = k5;
        break;
      case 2:
        kb = k2;
        kc = k6;
        break;
      case 3:
        ka = k1;
        kb = k5;
        break;
    }
  }
  yy = ay[0]*ay[1]*ay[2]*ay[3];
  int iyy = iay[0]+iay[1]+iay[2]+iay[3];
  int m1 = (k1+k2+k4+k5)/2+2;
  int m2 = (k1+k2-k3)/2+1;
  int m3 = (k4+k5-k3)/2+1;
  int m4 = (k1+k5-k6)/2+1;
  int m5 = (k2+k4-k6)/2+1;
  int m6 = k1+k4-k3-k6;
  int m7 = k2+k5-k3-k6;
  int n4 = m1;
  if( n4 > m2 )n4 = m2;
  if( n4 > m3 )n4 = m3;
  if( n4 > m4 )n4 = m4;
  if( n4 > m5 )n4 = m5;
  --n4;
  int n5 = 0;
  if( n5 < m6 )n5 = m6;
  if( n5 < m7 )n5 = m7;
  int n5par = 4*(n5/4)-n5+1;
  n5 /= 2;
  m6 = m6/2-1;
  m7 = m7/2-1;
  double z = 0.0;
  while( n5 <= n4 )
  {
    x = h[m1-n5-1]/(h[n5]*h[m2-n5-1]*h[m3-n5-1]*h[m4-n5-1]*h[m5-n5-1]*h[n5-m6-1]*h[n5-m7-1]);
    int ix = j[m1-n5-1]-j[n5]-j[m2-n5-1]-j[m3-n5-1]-j[m4-n5-1]-
        j[m5-n5-1]-j[n5-m6-1]-j[n5-m7-1];
    while( ix+iyy < 0 )
    {
      x *= 0.1;
      ++ix;
    }
    while( ix+iyy > 0 )
    {
      x *= 10.0;
      --ix;
    }
    if( n5par < 0 )x *= -1.;
    z += x;
    n5par *= -1;
    ++n5;
  }
  if( 4*((k1+k2+k4+k5)/4) < k1+k2+k4+k5-1 )return -z*yy;
  return z*yy;
}

double Wigner9JSymbol( double a, double b, double c,
                                        double xx, double yy, double zz,
                                        double gg, double hh, double pp )
{
  int kk1 = static_cast<int>(2*a+Sign(0.10,a));
  int kk2 = static_cast<int>(2*b+Sign(0.10,b));
  int kk3 = static_cast<int>(2*c+Sign(0.10,c));
  int kk4 = static_cast<int>(2*xx+Sign(0.10,xx));
  int kk5 = static_cast<int>(2*yy+Sign(0.10,yy));
  int kk6 = static_cast<int>(2*zz+Sign(0.10,zz));
  int kk7 = static_cast<int>(2*gg+Sign(0.10,gg));
  int kk8 = static_cast<int>(2*hh+Sign(0.10,hh));
  int kk9 = static_cast<int>(2*pp+Sign(0.10,pp));
  int kup = kk1+kk9;
  int m1 = kk4+kk8;
  int m2 = kk2+kk6;
  if( kup > m1 )kup = m1;
  if( kup > m2 )kup = m2;
  int k = kk1-kk9;
  if( k < 0 )k *= -1;
  m1 = kk4-kk8;
  if( m1 < 0 )m1 *= -1;
  m2 = kk2-kk6;
  if( m2 < 0 )m2 *= -1;
  if( k < m1 )k = m1;
  if( k < m2 )k = m2;
  double anine = 0.0;
  bool doneAlready = false;
TOP:                              // 660
  if( k > kup )return anine;
  int k1 = kk1;
  int k2 = kk4;
  int k3 = kk7;
  int k4 = kk8;
  int k5 = kk9;
  int k6 = k;
  int keyrac = 1;
  //
  double h[101];
  int j[101];
  if( !doneAlready )
  {
    h[0] = 1.0;
    j[0] = 0;
    double x = 0.0;
    for( int i=1; i<101; ++i )
    {
      x += 1.0;
      h[i] = h[i-1]*x;
      j[i] = j[i-1];
      while( h[i] >= 10.0 )
      {
        h[i] *= 0.01;
        j[i] += 2;
      }
    }
    doneAlready = true;
  }
NEXT:
  int ka = k1;
  int kb = k2;
  int kc = k3;
  //
  // triangle function
  //
  double ay[4];
  int iay[4];
  bool flag = false;
  int iyy, m3, m4, m5, m6, m7, n4, n5, n5par;
  double result, z;
  for( int i=1; i<=4; ++i )
  {
    int ma = ka+kb-kc;
    int mb = ka-kb+kc;
    int mc = -ka+kb+kc;
    int md = ka+kb+kc+2;
    if( ma<0 || mb<0 || mc<0 || md-(md/2)-(md/2)!=0 )
    {
      flag = true;
      break;
    }
    ma = ma/2+1;
    mb = mb/2+1;
    mc = mc/2+1;
    md = md/2+1;
    ay[i-1] = sqrt(h[ma-1]*h[mb-1]*h[mc-1]/h[md-1]);
    iay[i-1] = (j[ma-1]+j[mb-1]+j[mc-1]-j[md-1])/2;
    switch (i)
    {
      case 1:
        ka = k4;
        kb = k5;
        break;
      case 2:
        kb = k2;
        kc = k6;
        break;
      case 3:
        ka = k1;
        kb = k5;
        break;
    }
  }
  if( flag )goto BOTTOM;
  yy = ay[0]*ay[1]*ay[2]*ay[3];
  iyy = iay[0]+iay[1]+iay[2]+iay[3];
  m1 = (k1+k2+k4+k5)/2+2;
  m2 = (k1+k2-k3)/2+1;
  m3 = (k4+k5-k3)/2+1;
  m4 = (k1+k5-k6)/2+1;
  m5 = (k2+k4-k6)/2+1;
  m6 = k1+k4-k3-k6;
  m7 = k2+k5-k3-k6;
  n4 = m1;
  if( n4 > m2 )n4 = m2;
  if( n4 > m3 )n4 = m3;
  if( n4 > m4 )n4 = m4;
  if( n4 > m5 )n4 = m5;
  --n4;
  n5 = 0;
  if( n5 < m6 )n5 = m6;
  if( n5 < m7 )n5 = m7;
  n5par = 4*(n5/4)-n5+1;
  n5 /= 2;
  m6 = m6/2-1;
  m7 = m7/2-1;
  z = 0.0;
  while( n5 <= n4 )
  {
    double x = h[m1-n5-1]/(h[n5]*h[m2-n5-1]*h[m3-n5-1]*h[m4-n5-1]*
                           h[m5-n5-1]*h[n5-m6-1]*h[n5-m7-1]);
    int ix = j[m1-n5-1]-j[n5]-j[m2-n5-1]-j[m3-n5-1]-j[m4-n5-1]-
        j[m5-n5-1]-j[n5-m6-1]-j[n5-m7-1];
    while( ix+iyy < 0 )
    {
      x *= 0.1;
      ++ix;
    }
    while( ix+iyy > 0 )
    {
      x *= 10.0;
      --ix;
    }
    if( n5par < 0 )x *= -1.;
    z += x;
    n5par *= -1;
    ++n5;
  }
  result = z*yy;
BOTTOM:
  double ra, rb;
  if( keyrac == 1 )
  {
    ra = result;
    k1 = kk2;
    k2 = kk8;
    k3 = kk5;
    k4 = kk4;
    k5 = kk6;
    keyrac = 2;
    goto NEXT;
  }
  else if( keyrac == 2 )
  {
    rb = result;
    k1 = kk9;
    k2 = kk6;
    k3 = kk3;
    k4 = kk2;
    k5 = kk1;
    keyrac = 3;
    goto NEXT;
  }
  else
  {
    anine += ra*rb*result*(k+1);
    k += 2;
    goto TOP;
  }
}

double RacahCoefficient( double a, double b, double c,
                                          double xx, double yy, double zz )
{
  double h[101];
  int j[101];
  h[0] = 1.0;
  j[0] = 0;
  double x = 0.0;
  for( int i=1; i<101; ++i )
  {
    x += 1.0;
    h[i] = h[i-1]*x;
    j[i] = j[i-1];
    while( h[i] >= 10.0 )
    {
      h[i] *= 0.01;
      j[i] += 2;
    }
  }
  int k1 = static_cast<int>(2*a+Sign(0.10,a));
  int k2 = static_cast<int>(2*b+Sign(0.10,b));
  int k3 = static_cast<int>(2*yy+Sign(0.10,yy));
  int k4 = static_cast<int>(2*xx+Sign(0.10,xx));
  int k5 = static_cast<int>(2*c+Sign(0.10,c));
  int k6 = static_cast<int>(2*zz+Sign(0.10,zz));
  int ka = k1;
  int kb = k2;
  int kc = k3;
  //
  // triangle function
  //
  double ay[4];
  int iay[4];
  for( int i=1; i<=4; ++i )
  {
    int ma = ka+kb-kc;
    int mb = ka-kb+kc;
    int mc = -ka+kb+kc;
    int md = ka+kb+kc+2;
    if( ma<0 || mb<0 || mc<0 || md-(md/2)-(md/2)!=0 )return 0.0;
    ma = ma/2+1;
    mb = mb/2+1;
    mc = mc/2+1;
    md = md/2+1;
    ay[i-1] = sqrt(h[ma-1]*h[mb-1]*h[mc-1]/h[md-1]);
    iay[i-1] = (j[ma-1]+j[mb-1]+j[mc-1]-j[md-1])/2;
    switch (i)
    {
      case 1:
        ka = k4;
        kb = k5;
        break;
      case 2:
        kb = k2;
        kc = k6;
        break;
      case 3:
        ka = k1;
        kb = k5;
        break;
    }
  }
  yy = ay[0]*ay[1]*ay[2]*ay[3];
  int iyy = iay[0]+iay[1]+iay[2]+iay[3];
  int m1 = (k1+k2+k4+k5)/2+2;
  int m2 = (k1+k2-k3)/2+1;
  int m3 = (k4+k5-k3)/2+1;
  int m4 = (k1+k5-k6)/2+1;
  int m5 = (k2+k4-k6)/2+1;
  int m6 = k1+k4-k3-k6;
  int m7 = k2+k5-k3-k6;
  int n4 = m1;
  if( n4 > m2 )n4 = m2;
  if( n4 > m3 )n4 = m3;
  if( n4 > m4 )n4 = m4;
  if( n4 > m5 )n4 = m5;
  --n4;
  int n5 = 0;
  if( n5 < m6 )n5 = m6;
  if( n5 < m7 )n5 = m7;
  int n5par = 4*(n5/4)-n5+1;
  n5 /= 2;
  m6 = m6/2-1;
  m7 = m7/2-1;
  double z = 0.0;
  while( n5 <= n4 )
  {
    x = h[m1-n5-1]/(h[n5]*h[m2-n5-1]*h[m3-n5-1]*h[m4-n5-1]*
                    h[m5-n5-1]*h[n5-m6-1]*h[n5-m7-1]);
    int ix = j[m1-n5-1]-j[n5]-j[m2-n5-1]-j[m3-n5-1]-j[m4-n5-1]-
        j[m5-n5-1]-j[n5-m6-1]-j[n5-m7-1];
    while( ix+iyy < 0 )
    {
      x *= 0.1;
      ++ix;
    }
    while( ix+iyy > 0 )
    {
      x *= 10.0;
      --ix;
    }
    if( n5par < 0 )x *= -1.;
    z += x;
    n5par *= -1;
    ++n5;
  }
  return z*yy;
}

double JahnUFunction( double a, double b, double c,
                                       double xx, double yy, double zz )
{
  double h[101];
  int j[101];
  h[0] = 1.0;
  j[0] = 0;
  double x = 0.0;
  for( int i=1; i<101; ++i )
  {
    x += 1.0;
    h[i] = h[i-1]*x;
    j[i] = j[i-1];
    while( h[i] >= 10.0 )
    {
      h[i] *= 0.01;
      j[i] += 2;
    }
  }
  int k1 = static_cast<int>(2*a+Sign(0.10,a));
  int k2 = static_cast<int>(2*b+Sign(0.10,b));
  int k3 = static_cast<int>(2*yy+Sign(0.10,yy));
  int k4 = static_cast<int>(2*xx+Sign(0.10,xx));
  int k5 = static_cast<int>(2*c+Sign(0.10,c));
  int k6 = static_cast<int>(2*zz+Sign(0.10,zz));
  int ka = k1;
  int kb = k2;
  int kc = k3;
  //
  // triangle function
  //
  double ay[4];
  int iay[4];
  for( int i=1; i<=4; ++i )
  {
    int ma = ka+kb-kc;
    int mb = ka-kb+kc;
    int mc = -ka+kb+kc;
    int md = ka+kb+kc+2;
    if( ma<0 || mb<0 || mc<0 || md-(md/2)-(md/2)!=0 )return 0.0;
    ma = ma/2+1;
    mb = mb/2+1;
    mc = mc/2+1;
    md = md/2+1;
    ay[i-1] = sqrt(h[ma-1]*h[mb-1]*h[mc-1]/h[md-1]);
    iay[i-1] = (j[ma-1]+j[mb-1]+j[mc-1]-j[md-1])/2;
    switch (i)
    {
      case 1:
        ka = k4;
        kb = k5;
        break;
      case 2:
        kb = k2;
        kc = k6;
        break;
      case 3:
        ka = k1;
        kb = k5;
        break;
    }
  }
  yy = ay[0]*ay[1]*ay[2]*ay[3];
  int iyy = iay[0]+iay[1]+iay[2]+iay[3];
  int m1 = (k1+k2+k4+k5)/2+2;
  int m2 = (k1+k2-k3)/2+1;
  int m3 = (k4+k5-k3)/2+1;
  int m4 = (k1+k5-k6)/2+1;
  int m5 = (k2+k4-k6)/2+1;
  int m6 = k1+k4-k3-k6;
  int m7 = k2+k5-k3-k6;
  int n4 = m1;
  if( n4 > m2 )n4 = m2;
  if( n4 > m3 )n4 = m3;
  if( n4 > m4 )n4 = m4;
  if( n4 > m5 )n4 = m5;
  --n4;
  int n5 = 0;
  if( n5 < m6 )n5 = m6;
  if( n5 < m7 )n5 = m7;
  int n5par = 4*(n5/4)-n5+1;
  n5 /= 2;
  m6 = m6/2-1;
  m7 = m7/2-1;
  double z = 0.0;
  while( n5 <= n4 )
  {
    x = h[m1-n5-1]/(h[n5]*h[m2-n5-1]*h[m3-n5-1]*h[m4-n5-1]*h[m5-n5-1]*
                    h[n5-m6-1]*h[n5-m7-1]);
    int ix = j[m1-n5-1]-j[n5]-j[m2-n5-1]-j[m3-n5-1]-j[m4-n5-1]-
        j[m5-n5-1]-j[n5-m6-1]-j[n5-m7-1];
    while( ix+iyy < 0 )
    {
      x *= 0.1;
      ++ix;
    }
    while( ix+iyy > 0 )
    {
      x *= 10.0;
      --ix;
    }
    if( n5par < 0 )x *= -1.;
    z += x;
    n5par *= -1;
    ++n5;
  }
  return sqrt(static_cast<double>((k3+1)*(k6+1)))*z*yy;
}

int LUDecomposition( std::vector<double> &a, std::vector<int> &indx )
{
  // Given an nxn matrix a, this routine replaces it by the LU decomposition
  // of a rowwise permutation of itself. indx (output) records the row
  // permutation effected by the partial pivoting.  +/-1 is returned depending
  // on whether the number of row interchanges was even or odd, respectively.
  //
  double const tiny = std::numeric_limits<double>::min();
  int d = 1;
  int const n = static_cast<int>(sqrt(static_cast<double>(a.size())));
  std::vector<double> temp( n, 0.0 );
  for( int i=0; i<n; ++i )
  {
    double aamax = 0.0;
    for( int j=0; j<n; ++j )
    {
      if( fabs(a[i+j*n]) > aamax )aamax = fabs(a[i+j*n]);
    }
    if( aamax == 0.0 )throw EExpressionError(wxT("LUDecomposition: matrix is singular"));
    temp[i] = 1.0/aamax;
  }
  for( int j=1; j<=n; ++j )
  {
    double sum;
    if( j > 1 )
    {
      for( int i=1; i<=j-1; ++i )
      {
        sum = a[i-1+(j-1)*n];
        if( i > 1 )
        {
          for( int k=1; k<=i-1; ++k )sum -= a[i-1+(k-1)*n]*a[k-1+(j-1)*n];
          a[i-1+(j-1)*n] = sum;
        }
      }
    }
    double aamax = 0.0;
    int imax;
    for( int i=j; i<=n; ++i )
    {
      sum = a[i-1+(j-1)*n];
      if( j > 1 )
      {
        for( int k=1; k<=j-1; ++k )sum -= a[i-1+(k-1)*n]*a[k-1+(j-1)*n];
        a[i-1+(j-1)*n] = sum;
      }
      double dum = temp[i-1]*fabs(sum);
      if( dum >= aamax )
      {
        imax = i;
        aamax = dum;
      }
    }
    if( j != imax )
    {
      for( int k=1; k<=n; ++k )
      {
        double dum = a[imax-1+(k-1)*n];
        a[imax-1+(k-1)*n] = a[j-1+(k-1)*n];
        a[j-1+(k-1)*n] = dum;
      }
      d *= -1;
      temp[imax-1] = temp[j-1];
    }
    indx[j-1] = imax;
    if( j != n )
    {
      if( a[j-1+(j-1)*n] == 0.0 )a[j-1+(j-1)*n] = tiny;
      double dum = 1.0/a[j-1+(j-1)*n];
      for( int i=j+1; i<=n; ++i )a[i-1+(j-1)*n] *= dum;
    }
  }
  if( a[n*n-1] == 0.0 )a[n*n-1] = tiny;
  return d;
}

void LUSubstitution( std::vector<double> const &a,
                                      std::vector<int> const &indx,
                                      std::vector<double> &b )
{
  // This routine solves the set of n linear equations, ax=b.  The matrix a
  // is input as its LU decomposition.  indx is input as the permutation
  // vector, and b is the right-hand side vector and return with the solution
  // vector x.  This routine takes into account the possibility that b will
  // begin with many zero elements, so it is efficient for use in matrix
  // inversion.
  //
  int const n = static_cast<int>(sqrt(static_cast<double>(a.size())));
  int m = 0;
  for( int i=1; i<=n; ++i )
  {
    int k = indx[i-1];
    double sum = b[k-1];
    b[k-1] = b[i-1];
    if( m != 0 )
    {
      for( int j=m; j<=i-1; ++j )sum -= a[i-1+(j-1)*n]*b[j-1];
    }
    else if( sum != 0.0 )m = i;
    b[i-1] = sum;
  }
  for( int i=n; i>=1; --i )
  {
    double sum = b[i-1];
    if( i < n )
    {
      for( int j=i+1; j<=n; ++j )sum -= a[i-1+(j-1)*n]*b[j-1];
    }
    b[i-1] = sum/a[i-1+(i-1)*n];
  }
}

void Eigen( std::vector<double> &z )
{
  // This subroutine finds the eigenvalues and eigenvectors of a LOWER
  // TRIANGULAR matrix, using the recommended method from the eigensystem
  // package (EISPACK)
  //
  // Input:
  //         z[1:N,1:N]  the matrix assumed to be lower triangular
  // Output:
  //         z[1:N,1:N]  the eigenvectors are in the columns
  //         z[1:N,N+1]  the eigenvalues in ascending order in the last column
  //
  // REFERENCES:
  //    Questions and comments should be directed to B. S. Garbow,
  //    APPLIED MATHEMATICS DIVISION, ARGONNE NATIONAL LABORATORY
  //    B. T. SMITH, J. M. BOYLE, J. J. DONGARRA, B. S. GARBOW,
  //    Y. IKEBE, V. C. KLEMA, C. B. MOLER, *MATRIX EIGEN-
  //    SYSTEM ROUTINES - EISPACK GUIDE*, SPRINGER-VERLAG, 1976
  //    NUM. MATH. 11, 181-195(1968) by Martin, Reinsch, and Wilkinson.
  //    HANDBOOK FOR AUTO. COMP., VOL.II-LINEAR ALGEBRA, 212-226 (1971)
  //    NUM. MATH. 11, 293-306(1968) by Bowdler, Martin, Reinsch, and Wilkinson
  //    HANDBOOK FOR AUTO. COMP., VOL.II-LINEAR ALGEBRA, 227-240(1971).
  //
  int n = static_cast<int>(sqrt(1.0+4.0*z.size())-1)/2;
  std::vector<double> work(n,0.0);
  //
  // Reduce a LOWER TRIANGULAR matrix to a SYMMETRIC TRIDIAGONAL matrix
  // using and accumulating orthogonal similarity transformations
  //
  for( int ii=2; ii<=n; ++ii )
  {
    int i = n + 2 - ii;
    int l = i - 1;
    double h = 0.0;
    double scale = 0.0;
    if( l > 1 )
    {
      for( int k=1; k<=l; ++k )scale += fabs(z[i-1+(k-1)*n]); // scale row
      if( scale != 0.0 )
      {
        for( int k=1; k<=l; ++k )
        {
          z[i-1+(k-1)*n] /= scale;
          h += z[i-1+(k-1)*n]*z[i-1+(k-1)*n];
        }
        double f = z[i-1+(l-1)*n];
        double g = -Sign(sqrt(h),f);
        work[i-1] = scale * g;
        h -= f*g;
        z[i-1+(l-1)*n] = f-g;
        f = 0.0;
        for( int j=1; j<=l; ++j )
        {
          z[j-1+(i-1)*n] = z[i-1+(j-1)*n]/h;
          g = 0.0;
          for( int k=1; k<=j; ++k )g += z[j-1+(k-1)*n]*z[i-1+(k-1)*n]; // form element of a*u
          if( l >= j+1 )
          {
            for( int k=j+1; k<=l; ++k )g += z[k-1+(j-1)*n]*z[i-1+(k-1)*n];
          }
          work[j-1] = g/h;       // form element of p
          f += work[j-1]*z[i-1+(j-1)*n];
        }
        double hh = f/(h+h);
        for( int j=1; j<=l; ++j )  // form reduced a
        {
          f = z[i-1+(j-1)*n];
          g = work[j-1] - hh*f;
          work[j-1] = g;
          for( int k=1; k<=j; ++k )z[j-1+(k-1)*n] -= f*work[k-1]+g*z[i-1+(k-1)*n];
        }
        z[i-1+n*n] = h;
        continue;
      }
    }
    work[i-1] = z[i-1+(l-1)*n];
    z[i-1+n*n] = h;
  }
  z[n*n] = 0.0;
  work[0] = 0.0;
  for( int i=1; i<=n; ++i )  // accumulation of transformation matrices
  {
    int l = i - 1;
    if( z[i-1+n*n] != 0.0 )
    {
      for( int j=1; j<=l; ++j )
      {
        double g = 0.0;
        for( int k=1; k<=l; ++k )g += z[i-1+(k-1)*n]*z[k-1+(j-1)*n];
        for( int k=1; k<=l; ++k )z[k-1+(j-1)*n] -= g*z[k-1+(i-1)*n];
      }
    }
    z[i-1+n*n] = z[i-1+(i-1)*n];
    z[i-1+(i-1)*n] = 1.0;
    if( l > 0 )
    {
      for( int j=1; j<=l; ++j )
      {
        z[i-1+(j-1)*n] = 0.0;
        z[j-1+(i-1)*n] = 0.0;
      }
    }
  }
  //
  // find the eigenvalues/vectors of symmetric tridiagonal matrix by ql method
  //
  for( int i=2; i<=n; ++i )work[i-2] = work[i-1];
  double f = 0.0;
  double b = 0.0;
  work[n-1] = 0.0;
  for( int l=1; l<=n; ++l )
  {
    int j = 0;
    double h = fabs(z[l-1+n*n]) + fabs(work[l-1]);
    if( b < h )b = h;
    //
    // look for small sub-diagonal element
    //
    int m;
    for( m=l; m<=n; ++m )if( b+fabs(work[m-1]) == b )break;
    if( m == l )
    {
      z[l-1+n*n] += f;
      continue;
    }
    do
    {
      if( j == 30 )
       throw EExpressionError(wxT("Eigen function: eigenvalue has not been determined after 30 iterations"));
      ++j;
      int l1 = l+1;  // form shift
      int l2 = l1+1;
      double g = z[l-1+n*n];
      double p = (z[l1-1+n*n]-g)/(2.0*work[l-1]);
      //
      // find sqrt(p*p+1) without overflow or destructive underflow
      //
      double pp = std::max(fabs(p),1.0);
      double q = std::min(fabs(p),1.0);
      if( q != 0.0 )
      {
 L50:
        double r = (q/pp)*(q/pp);
        double t = 4.0+r;
        if( t != 4.0 )
        {
          double s = r/t;
          pp += 2.0*pp*s;
          q *= s;
          goto L50;
        }
      }
      z[l-1+n*n] = work[l-1]/(p+Sign(pp,p));
      z[l1-1+n*n] = work[l-1]*(p+Sign(pp,p));
      double dl1 = z[l1-1+n*n];
      h = g - z[l-1+n*n];
      if( l2 <= n )
      {
        for( int i=l2; i<=n; ++i )z[i-1+n*n] -= h;
      }
      f += h;
      p = z[m-1+n*n];  // ql transformation
      double c = 1.0;
      double c2 = c, c3;
      double el1 = work[l1-1];
      double s = 0.0, s2;
      for( int ii=1; ii<=m-l; ++ii )
      {
        c3 = c2;
        c2 = c;
        s2 = s;
        int i = m-ii;
        g = c*work[i-1];
        h = c*p;
        double r;
        if( fabs(p) >= fabs(work[i-1]) )
        {
          c = work[i-1]/p;
          r = sqrt(c*c+1.0);
          work[i] = s*p*r;
          s = c/r;
          c = 1.0/r;
        }
        else
        {
          c = p/work[i-1];
          r = sqrt(c*c+1.0);
          work[i] = s*work[i-1]*r;
          s = 1.0/r;
          c = c*s;
        }
        p = c*z[i-1+n*n] - s*g;
        z[i+n*n] = h+s*(c*g+s*z[i-1+n*n]);
        for( int k=1; k<=n; ++k )          // form vector
        {
          h = z[k-1+i*n];
          z[k-1+i*n] = s*z[k-1+(i-1)*n] + c*h;
          z[k-1+(i-1)*n] = c*z[k-1+(i-1)*n] - s*h;
        }
      }
      p = -s*s2*c3*el1*work[l-1]/dl1;
      work[l-1] = s*p;
      z[l-1+n*n] = c*p;
    }
    while ( b+fabs(work[l-1]) > b );
    z[l-1+n*n] += f;
  }
  for( int ii=2; ii<=n; ++ii )  // order eigenvalues and eigenvectors
  {
    int i = ii - 1;
    int k = i;
    double p = z[i-1+n*n];
    for( int j=ii; j<=n; ++j )
    {
      if( z[j-1+n*n] < p )
      {
        k = j;
        p = z[j-1+n*n];
      }
    }
    if( k != i )
    {
      z[k-1+n*n] = z[i-1+n*n];
      z[i-1+n*n] = p;
      for( int j=1; j<=n; ++j )
      {
        p = z[j-1+(i-1)*n];
        z[j-1+(i-1)*n] = z[j-1+(k-1)*n];
        z[j-1+(k-1)*n] = p;
      }
    }
  }
}

int NextPrime( int n )
{
  // Returns next prime number >= N
  //
  // For N >= 1,018,801 returns next number relatively prime to all
  // primes below 1009.
  // Note: 1009 is next prime after 997, and 1,018,801 = 1009^2.
  //       The database may be extended if larger primes are desired.
  //
  // Outline:
  //    1.  If number .LE. 3 then return number.
  //        Number is prime, zero, or negative.
  //    2.  If number between 4 and 997 then search PRIME array
  //        for next prime >= number.  An approximating polynomial
  //        is used to give us a good initial guess for the index I
  //        so that PRIME(I) is very close to the prime we seek.
  //        The approximating polynomial is:
  //
  //        I = -0.3034277E-04*X^2 + 0.1918667*X + 8.0918350
  //
  //        This function is right on the money 452 times, is low by one
  //        224 times, is low by two 40 times, and is low by three 1 time.
  //        It's high by one 185 times, high by two 68 times, high by three
  //        12 times, high by four 7 times, high by five 4 times, and high
  //        by 6 one time.
  //
  //    3.  Else number is greater than 997.  Do normal search for next prime.
  //        3a.  Increment NUMBER to an odd number.  Since even numbers are
  //             not prime we skip over even numbers.
  //        3b.  Test if NUMBER is evenly divisible by any prime in the
  //             PRIME array less than SQRT(NUMBER).
  //             If NUMBER MOD PRIME(I) = 0, then NUMBER is not prime.
  //
  int const numprimes = 168;
  int prime[] = {
      2,   3,   5,   7,  11,  13,  17,  19,  23,  29,  31,  37,  41,
     43,  47,  53,  59,  61,  67,  71,  73,  79,  83,  89,  97, 101,
    103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
    173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239,
    241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313,
    317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397,
    401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467,
    479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569,
    571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643,
    647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733,
    739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823,
    827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911,
    919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997 };
  //
  if( n <= 3 )return n;
  if( n <= prime[numprimes-1] )  // search thru prime array
  {
    int i = static_cast<int>((-0.3034277e-04*n+0.1918667)*n+8.0918350);
    i = std::min(i,numprimes)-1;   // don't let i go above size of prime array
    while ( prime[i] < n )++i;     // search upward for first prime greater than number
    while ( prime[i] >= n )--i;    // search downward for first prime less than number,
    return prime[i+1];	     	     // then take next prime.
  }
  else					                 // else normal search for next prime
  {
    if( n%2 == 0 )++n;        // rule out even numbers.  they are not prime
    int i = 1;                // start with second prime since we only test odd numbers.
    while ( prime[i]*prime[i] <= n ) // test for all primes <= sqrt(number)
    {
      if( n%prime[i] == 0 )   // it's not prime
      {
        n += 2;	              // even numbers aren't prime
        i = 1;                // start over again with new number
      }
      else
      {
        ++i;                  // test with next prime
        if( i >= numprimes )break; // exit loop
                                   // number is relatively prime to first 168 primes.
      }
    }
    return n;
  }
}

void ComplexFourierTransform( std::vector<double> &a,
                                              std::vector<double> &b, int n, int nspan,
                                              bool inverse )
{
  // Multivariate complex fourier transform, computed in place using mixed-radix
  // fast fourier transform algorithm.
  //     by R. C. SINGLETON, STANFORD RESEARCH INSTITUTE, OCT. 1968
  //
  // Arrays A and B originally hold the real and imaginary components of the
  // data, and return the real and imaginary components of the resulting Fourier
  // coefficients.  Multivariate data is indexed according to the fortran array
  // element successor function, without limit on the number of implied multiple
  // subscripts. The routine is called once for each variate. The calls for a
  // multivariate transform may be in any order.
  //
  //       N is the dimension of the current variable
  // NSPAN/N is the spacing of consecutive data values
  //         while indexing the current variable
  // INVERSE determines the sign of the complex exponential
  //
  // for a single-variate transform,
  //     a.size() = N = NSPAN = (number of complex data values), e.g.
  //     ComplexFourierTransform(A,B,N,N,false)
  //
  // a tri-variate transform with A(N1,N2,N3), B(N1,N2,N3) is computed by
  //     ComplexFourierTransform(A,B,N1,N1,false)
  //     ComplexFourierTransform(A,B,N2,N1*N2,false)
  //     ComplexFourierTransform(A,B,N3,N1*N2*N3,false)
  //
  if( n < 2 )return;
  //
  int const maxd = 11;      // should agree with the array dimensions
  int maxf = 23;
  int const maxp = 209;
  //
  int ntot = a.size();
  int inc = 1;
  double rad = 2*M_PI;
  double s72 = sin(0.4*M_PI);
  double c72 = cos(0.4*M_PI);
  double s120 = sqrt(0.75);
  if( inverse )
  {
    s72 *= -1;
    s120 *= -1;
    rad *= -1;
  }
  int nt = inc*ntot;
  int ks = inc*nspan;
  int kspan = ks;
  int nn = nt-inc;
  int jc = ks/n;
  double radf = rad*jc*0.5;
  int i = 0;
  int jf = 0;
  //
  // determine the factors of n
  //
  int m = 0;
  int k = n;
  int nfac[11], np[209];
  double at[23], ck[23], bt[23], sk[23];
  while ( k%16 == 0 )
  {
    if( ++m > maxd )
     throw EExpressionError(wxT("ComplexFourierTransform: product of square-free factors > 210"));
    nfac[m-1] = 4;
    k /= 16;
  }
  int j = 3;
  do
  {
    while ( k%(j*j) == 0 )
    {
      if( ++m > maxd )
       throw EExpressionError(wxT("ComplexFourierTransform: product of square-free factors > 210"));
      nfac[m-1] = j;
      k /= j*j;
    }
    j += 2;
  } while ( j*j <= k );
  //
  int kt;
  if( k <= 4 )
  {
    kt = m;
    if( m+1 > maxd )
     throw EExpressionError(wxT("ComplexFourierTransform: product of square-free factors > 210"));
    nfac[m] = k;
    if( k != 1 )++m;
  }
  else
  {
    if( k%4 == 0 )
    {
      if( ++m > maxd )
       throw EExpressionError(wxT("ComplexFourierTransform: product of square-free factors > 210"));
      nfac[m-1] = 2;
      k /= 4;
    }
    kt = m;
    j = 2;
    do
    {
      if( k%j == 0 )
      {
        if( ++m > maxd )
         throw EExpressionError(wxT("ComplexFourierTransform: product of square-free factors > 210"));
        nfac[m-1] = j;
        k /= j;
      }
      j = ((j+1)/2)*2+1;
    } while ( j <= k );
  }
  if( kt != 0 )
  {
    j = kt;
    do
    {
      if( ++m > maxd )
       throw EExpressionError(wxT("ComplexFourierTransform: product of square-free factors > 210"));
      nfac[m-1] = nfac[j-1];
    } while( --j != 0 );
  }
  for( int dum=1; dum<=m; ++dum )
  {
    if( nfac[dum-1] > maxf )
     throw EExpressionError(wxT("ComplexFourierTransform: largest prime factor > 23"));
  }
  //
  // compute fourier transform
  //
L100:
  double sd = radf/kspan;
  double cd = 2.0*sin(sd)*sin(sd);
  sd = sin(sd+sd);
  int kk = 1;
  ++i;
  int k1, k2, k3, k4, kspnn, jj;
  double ak, bk, c1, s1, aj, bj, c2=0, s2=0, c3=0, s3=0, aa, bb, akp, akm, ajp, ajm,
      bkp, bkm, bjp, bjm;
  if( nfac[i-1] != 2 )goto L400;
  //
  // transform for factor of 2 (including rotation factor)
  //
  kspan /= 2;
  k1 = kspan+2;
L210:
  k2 = kk+kspan;
  ak = a[k2-1];
  bk = b[k2-1];
  a[k2-1] = a[kk-1]-ak;
  b[k2-1] = b[kk-1]-bk;
  a[kk-1] += ak;
  b[kk-1] += bk;
  kk = k2+kspan;
  if( kk <= nn )goto L210;
  kk -= nn;
  if( kk <= jc )goto L210;
  if( kk > kspan )goto L800;
L220:
  c1 = 1.0-cd;
  s1 = sd;
L230:
  k2 = kk+kspan;
  ak = a[kk-1]-a[k2-1];
  bk = b[kk-1]-b[k2-1];
  a[kk-1] += a[k2-1];
  b[kk-1] += b[k2-1];
  a[k2-1] = c1*ak-s1*bk;
  b[k2-1] = s1*ak+c1*bk;
  kk = k2+kspan;
  if( kk < nt )goto L230;
  k2 = kk-nt;
  c1 *= -1;
  kk = k1-k2;
  if( kk > k2 )goto L230;
  ak = c1-(cd*c1+sd*s1);
  s1 = (sd*c1-cd*s1)+s1;
  //
  // the following three statements compensate for truncation error
  // if rounded arithmetic is used, they may be deleted
  //     c1 = 0.5/(ak*ak+s1*s1)+0.5
  //     s1 = c1*s1
  //     c1 = c1*ak
  // next statement should be deleted if non-rounded arithmetic is used
  //
  c1 = ak;
  kk += jc;
  if( kk < k2 )goto L230;
  k1 += inc+inc;
  kk = (k1-kspan)/2+jc;
  if( kk <= jc+jc )goto L220;
  goto L100;
  //
  // transform for factor of 3 (optional code)
  //
L320:
  k1 = kk+kspan;
  k2 = k1+kspan;
  ak = a[kk-1];
  bk = b[kk-1];
  aj = a[k1-1]+a[k2-1];
  bj = b[k1-1]+b[k2-1];
  a[kk-1] = ak+aj;
  b[kk-1] = bk+bj;
  ak -= 0.5*aj;
  bk -= 0.5*bj;
  aj = (a[k1-1]-a[k2-1])*s120;
  bj = (b[k1-1]-b[k2-1])*s120;
  a[k1-1] = ak-bj;
  b[k1-1] = bk+aj;
  a[k2-1] = ak+bj;
  b[k2-1] = bk-aj;
  kk = k2+kspan;
  if( kk < nn )goto L320;
  kk -= nn;
  if( kk <= kspan )goto L320;
  goto L700;
  //
  // transform for factor of 4
  //
L400:
  if( nfac[i-1] != 4 )goto L600;
  kspnn = kspan;
  kspan /= 4;
L410:
  c1 = 1.0;
  s1 = 0;
L420:
  k1 = kk+kspan;
  k2 = k1+kspan;
  k3 = k2+kspan;
  akp = a[kk-1]+a[k2-1];
  akm = a[kk-1]-a[k2-1];
  ajp = a[k1-1]+a[k3-1];
  ajm = a[k1-1]-a[k3-1];
  a[kk-1] = akp+ajp;
  ajp = akp-ajp;
  bkp = b[kk-1]+b[k2-1];
  bkm = b[kk-1]-b[k2-1];
  bjp = b[k1-1]+b[k3-1];
  bjm = b[k1-1]-b[k3-1];
  b[kk-1] = bkp+bjp;
  bjp = bkp-bjp;
  if( inverse )goto L450;
  akp = akm-bjm;
  akm += bjm;
  bkp = bkm+ajm;
  bkm -= ajm;
  if( s1 == 0.0 )goto L460;
L430:
  a[k1-1] = akp*c1-bkp*s1;
  b[k1-1] = akp*s1+bkp*c1;
  a[k2-1] = ajp*c2-bjp*s2;
  b[k2-1] = ajp*s2+bjp*c2;
  a[k3-1] = akm*c3-bkm*s3;
  b[k3-1] = akm*s3+bkm*c3;
  kk = k3+kspan;
  if( kk <= nt )goto L420;
L440:
  c2 = c1-(cd*c1+sd*s1);
  s1 = (sd*c1-cd*s1)+s1;
  //
  // the following three statements compensate for truncation error
  // if rounded arithmetic is used, they may be deleted
  //     c1 = 0.5/(c2*c2+s1*s1)+0.5
  //     s1 = c1*s1
  //     c1 = c1*c2
  // next statement should be deleted if non-rounded arithmetic is used
  //
  c1 = c2;
  c2 = c1*c1-s1*s1;
  s2 = 2.0*c1*s1;
  c3 = c2*c1-s2*s1;
  s3 = c2*s1+s2*c1;
  kk -= nt-jc;
  if( kk <= kspan )goto L420;
  kk -= kspan-inc;
  if( kk <= jc )goto L410;
  if( kspan == jc )goto L800;
  goto L100;
L450:
  akp = akm+bjm;
  akm -= bjm;
  bkp = bkm-ajm;
  bkm += ajm;
  if( s1 != 0.0 )goto L430;
L460:
  a[k1-1] = akp;
  b[k1-1] = bkp;
  a[k2-1] = ajp;
  b[k2-1] = bjp;
  a[k3-1] = akm;
  b[k3-1] = bkm;
  kk = k3+kspan;
  if( kk <= nt )goto L420;
  goto L440;
  //
  // transform for factor of 5 (optional code)
  //
L510:
  c2 = c72*c72-s72*s72;
  s2 = 2.0*c72*s72;
L520:
  k1 = kk+kspan;
  k2 = k1+kspan;
  k3 = k2+kspan;
  k4 = k3+kspan;
  akp = a[k1-1]+a[k4-1];
  akm = a[k1-1]-a[k4-1];
  bkp = b[k1-1]+b[k4-1];
  bkm = b[k1-1]-b[k4-1];
  ajp = a[k2-1]+a[k3-1];
  ajm = a[k2-1]-a[k3-1];
  bjp = b[k2-1]+b[k3-1];
  bjm = b[k2-1]-b[k3-1];
  aa = a[kk-1];
  bb = b[kk-1];
  a[kk-1] = aa+akp+ajp;
  b[kk-1] = bb+bkp+bjp;
  ak = akp*c72+ajp*c2+aa;
  bk = bkp*c72+bjp*c2+bb;
  aj = akm*s72+ajm*s2;
  bj = bkm*s72+bjm*s2;
  a[k1-1] = ak-bj;
  a[k4-1] = ak+bj;
  b[k1-1] = bk+aj;
  b[k4-1] = bk-aj;
  ak = akp*c2+ajp*c72+aa;
  bk = bkp*c2+bjp*c72+bb;
  aj = akm*s2-ajm*s72;
  bj = bkm*s2-bjm*s72;
  a[k2-1] = ak-bj;
  a[k3-1] = ak+bj;
  b[k2-1] = bk+aj;
  b[k3-1] = bk-aj;
  kk = k4+kspan;
  if( kk < nn )goto L520;
  kk -= nn;
  if( kk <= kspan )goto L520;
  goto L700;
  //
  // transform for odd factors
  //
L600:
  k = nfac[i-1];
  kspnn = kspan;
  kspan /= k;
  if( k == 3 )goto L320;
  if( k == 5 )goto L510;
  if( k == jf )goto L640;
  jf = k;
  s1 = rad/k;
  c1 = cos(s1);
  s1 = sin(s1);
  ck[jf-1] = 1.0;
  sk[jf-1] = 0.0;
  j = 1;
L630:
  ck[j-1] = ck[k-1]*c1+sk[k-1]*s1;
  sk[j-1] = ck[k-1]*s1-sk[k-1]*c1;
  --k;
  ck[k-1] = ck[j-1];
  sk[k-1] = -sk[j-1];
  if( ++j < k )goto L630;
L640:
  k1 = kk;
  k2 = kk+kspnn;
  aa = a[kk-1];
  bb = b[kk-1];
  ak = aa;
  bk = bb;
  j = 1;
  k1 += kspan;
L650:
  k2 -= kspan;
  ++j;
  at[j-1] = a[k1-1]+a[k2-1];
  ak += at[j-1];
  bt[j-1] = b[k1-1]+b[k2-1];
  bk += bt[j-1];
  ++j;
  at[j-1] = a[k1-1]-a[k2-1];
  bt[j-1] = b[k1-1]-b[k2-1];
  k1 += kspan;
  if( k1 < k2 )goto L650;
  a[kk-1] = ak;
  b[kk-1] = bk;
  k1 = kk;
  k2 = kk+kspnn;
  j = 1;
L660:
  k1 += kspan;
  k2 -= kspan;
  jj = j;
  ak = aa;
  bk = bb;
  aj = 0.0;
  bj = 0.0;
  k = 1;
L670:
  ++k;
  ak += at[k-1]*ck[jj-1];
  bk += bt[k-1]*ck[jj-1];
  ++k;
  aj += at[k-1]*sk[jj-1];
  bj += bt[k-1]*sk[jj-1];
  jj += j;
  if( jj > jf )jj -= jf;
  if( k < jf )goto L670;
  k = jf-j;
  a[k1-1] = ak-bj;
  b[k1-1] = bk+aj;
  a[k2-1] = ak+bj;
  b[k2-1] = bk-aj;
  if( ++j < k )goto L660;
  kk += kspnn;
  if( kk <= nn )goto L640;
  kk -= nn;
  if( kk <= kspan )goto L640;
  //
  // multiply by rotation factor (except for factors of 2 and 4)
  //
L700:
  if( i == m )goto L800;
  kk = jc+1;
L710:
  c2 = 1.0-cd;
  s1 = sd;
L720:
  c1 = c2;
  s2 = s1;
  kk += kspan;
L730:
  ak = a[kk-1];
  a[kk-1] = c2*ak-s2*b[kk-1];
  b[kk-1] = s2*ak+c2*b[kk-1];
  kk += kspnn;
  if( kk <= nt )goto L730;
  ak = s1*s2;
  s2 = s1*c2+c1*s2;
  c2 = c1*c2-ak;
  kk -= nt-kspan;
  if( kk <= kspnn )goto L730;
  c2 = c1-(cd*c1+sd*s1);
  s1 = s1+(sd*c1-cd*s1);
  //
  // the following three statements compensate for truncation error
  // if rounded arithmetic is used, they may be deleted
  //     c1 = 0.5/(c2*c2+s1*s1)+0.5
  //     s1 = c1*s1
  //     c2 = c1*c2
  //
  kk -= kspnn-jc;
  if( kk <= kspan )goto L720;
  kk -= kspan-jc-inc;
  if( kk <= jc+jc )goto L710;
  goto L100;
  //
  // permute the results to normal order---done in two stages
  // permutation for square factors of n
  //
L800:
  np[0] = ks;
  if( kt == 0 )goto L890;
  k = kt+kt+1;
  if( m < k )--k;
  j = 1;
  np[k] = jc;
L810:
  np[j] = np[j-1]/nfac[j-1];
  np[k-1] = np[k]*nfac[j-1];
  if( ++j < --k )goto L810;
  k3 = np[k];
  kspan = np[1];
  kk = jc+1;
  k2 = kspan+1;
  j = 1;
  if( n != ntot )goto L850;
  //
  // permutation for single-variate transform (optional code)
  //
L820:
  ak = a[kk-1];
  a[kk-1] = a[k2-1];
  a[k2-1] = ak;
  bk = b[kk-1];
  b[kk-1] = b[k2-1];
  b[k2-1] = bk;
  kk += inc;
  k2 += kspan;
  if( k2 < ks )goto L820;
L830:
  k2 -= np[j-1];
  ++j;
  k2 = np[j]+k2;
  if( k2 > np[j-1] )goto L830;
  j = 1;
L840:
  if( kk < k2 )goto L820;
  kk += inc;
  k2 += kspan;
  if( k2 < ks )goto L840;
  if( kk < ks )goto L830;
  jc = k3;
  goto L890;
  //
  // permutation for multivariate transform
  //
L850:
  k = kk+jc;
L860:
  ak = a[kk-1];
  a[kk-1] = a[k2-1];
  a[k2-1] = ak;
  bk = b[kk-1];
  b[kk-1] = b[k2-1];
  b[k2-1] = bk;
  kk += inc;
  k2 += inc;
  if( kk < k )goto L860;
  kk += ks-jc;
  k2 += ks-jc;
  if( kk < nt )goto L850;
  k2 -= nt-kspan;
  kk -= nt-jc;
  if( k2 < ks )goto L850;
L870:
  k2 -= np[j-1];
  ++j;
  k2 += np[j];
  if( k2 > np[j-1] )goto L870;
  j = 1;
L880:
  if( kk < k2 )goto L850;
  kk += jc;
  k2 += kspan;
  if( k2 < ks )goto L880;
  if( kk < ks )goto L870;
  jc = k3;
L890:
  if( 2*kt+1 >= m )return;
  kspnn = np[kt];
  //
  // permutation for square-free factors of n
  //
  j = m-kt;
  nfac[j] = 1;
L900:
  nfac[j-1] = nfac[j-1]*nfac[j];
  if( --j != kt )goto L900;
  ++kt;
  if( nfac[kt-1] > maxp+1 )
   throw EExpressionError(wxT("ComplexFourierTransform: error NFAC[KT-1] > 210"));
  nn = nfac[kt-1]-1;
  jj = 0;
  j = 0;
  goto L906;
L902:
  jj -= k2;
  k2 = kk;
  ++k;
  kk = nfac[k-1];
L904:
  jj += kk;
  if( jj >= k2 )goto L902;
  np[j-1] = jj;
L906:
  k2 = nfac[kt-1];
  k = kt+1;
  kk = nfac[k-1];
  ++j;
  if( j <= nn )goto L904;
  //
  // determine the permutation cycles of length greater than 1
  //
  j = 0;
  goto L914;
L910:
  k = kk;
  kk = np[k-1];
  np[k-1] = -kk;
  if( kk != j )goto L910;
  k3 = kk;
L914:
  ++j;
  kk = np[j-1];
  if( kk < 0 )goto L914;
  if( kk != j )goto L910;
  np[j-1] = -j;
  if( j != nn )goto L914;
  maxf *= inc;
  //
  // reorder a and b, following the permutation cycles
  //
  goto L950;
L924:
  --j;
  if( np[j-1] < 0 )goto L924;
  jj = jc;
L926:
  kspan = jj;
  if( jj > maxf )kspan = maxf;
  jj -= kspan;
  k = np[j-1];
  kk = jc*k+i+jj;
  k1 = kk+kspan;
  k2 = 0;
L928:
  ++k2;
  at[k2-1] = a[k1-1];
  bt[k2-1] = b[k1-1];
  k1 -= inc;
  if( k1 != kk )goto L928;
L932:
  k1 = kk+kspan;
  k2 = k1-jc*(k+np[k-1]);
  k = -np[k-1];
L936:
  a[k1-1] = a[k2-1];
  b[k1-1] = b[k2-1];
  k1 -= inc;
  k2 -= inc;
  if( k1 != kk )goto L936;
  kk = k2;
  if( k != j )goto L932;
  k1 = kk+kspan;
  k2 = 0;
L940:
  ++k2;
  a[k1-1] = at[k2-1];
  b[k1-1] = bt[k2-1];
  k1 -= inc;
  if( k1 != kk )goto L940;
  if( jj != 0 )goto L926;
  if( j != 1 )goto L924;
L950:
  j = k3+1;
  nt -= kspnn;
  i = nt-inc+1;
  if( nt >= 0 )goto L924;
  return;
}

void SavitzkyGolay( std::vector<double> &yin, std::vector<double> &yout,
                                     std::size_t const filterWidth,
                                     std::size_t const filterOrder )
{
  // driver routine to call SavitzkyGolay for an array of points
  // filterOrder=2 or 4
  //
  std::size_t const npts = yin.size();
  //
  if( npts < filterWidth )throw EExpressionError(wxT("SavitzkyGolay: not enough input data"));
  //
  std::vector<double> c( filterWidth );
  //
  // Handle lower points of array YIN
  //
  std::size_t nl = 0;
  for( std::size_t i=0; i<filterWidth/2; ++i, ++nl )
  {
    yout[i] = 0.0;
    try
    {
      SavGol( c, filterWidth, nl, filterWidth-nl-1, filterOrder );
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    //
    // Smooth the point
    //
    std::size_t index = nl+1;
    for( std::size_t j=0; j<filterWidth; ++j )
    {
      yout[i] += c[index-1]*yin[j];
      if( --index == 0 )index = filterWidth;
    }
  }
  //
  // Handle non-end cases of XIN
  //
  nl = filterWidth/2;
  try
  {
    SavGol( c, filterWidth, nl, filterWidth-nl-1, filterOrder );
  }
  catch (EExpressionError &e)
  {
    throw;
  }
  for( std::size_t i=filterWidth/2+1; i<=npts-(filterWidth-1)/2; ++i )
  {
    //
    // Smooth the point
    //
    yout[i-1] = 0.0;
    std::size_t index = nl+1;
    for( std::size_t j=1; j<=filterWidth; ++j )
    {
      yout[i-1] += c[index-1]*yin[i-nl+j-2];
      if( --index == 0 )index = filterWidth;
    }
  }
  //
  // Handle right-end case of XIN
  //
  std::size_t nr = (filterWidth-1)/2;
  for( std::size_t i=npts-(filterWidth-1)/2+1; i<=npts; ++i )
  {
    --nr;
    nl = filterWidth-nr-1;
    try
    {
      SavGol( c, filterWidth, nl, nr, filterOrder );
    }
    catch (EExpressionError &e)
    {
      throw;
    }
    //
    // Smooth the point
    //
    yout[i-1] = 0.0;
    std::size_t index = nl+1;
    for( std::size_t j=1; j<=filterWidth; ++j )
    {
      yout[i-1] += c[index-1]*yin[npts-filterWidth+j-1];
      if( --index == 0 )index = filterWidth;
    }
  }
}

void SavGol( std::vector<double> &c, std::size_t np,
                              std::size_t nl, std::size_t nr,
                              std::size_t filterOrder )
{
  // Returns in c in wraparound order (i.e., c0 is in c[0],
  // c-1 is in C[1] and so on for negative indices while c+1 is
  // stored in c[np-1], c+2 in c[np-2] etc.), consistent with the
  // argument RESPNS in routine CONVLV, a set of Savitzky-Golay
  // coefficients. NL is the number of leftward (past) data points
  // used, while NR is the number of rightward (future) data points
  // making the total number of data pouints used NL+NR+1. LD is
  // the order of the derivative desired (eg. here LD=0 for
  // smoothed function). filterOrder is the order of the smoothing polynomial,
  // also equal to the highest conserved moment; usual values are
  // 2 or 4.  See Computers in Physics Nov/Dec 1990 pages 669ff.
  //
  std::size_t const nMax = 6;
  if( np<nl+nr+1 || nl+nr<filterOrder )
   throw EExpressionError(wxT("SavitzkyGolay: not enough input data"));
  //
  // set up the normal equations of the desired fit.
  //
  double a[(nMax+1)*(nMax+1)];
  for( std::size_t i=0; i<=2*filterOrder; ++i )
  {
    double sum = 0.0;
    if( i == 0 )sum = 1.0;
    for( std::size_t k=1; k<=nr; ++k )
      sum += pow(static_cast<double>(k),static_cast<double>(i));
    for( std::size_t k=1; k<=nl; ++k )
      sum += pow(-1.0*static_cast<double>(k),static_cast<double>(i));
    int mm = static_cast<int>(std::min(i,2*filterOrder-i));
    for( int j=-mm; j<=mm; j+=2 )a[(i+j)/2+(i-j)/2*(nMax+1)] = sum;
  }
  //
  // solve them: lu decomposition.
  //
  int indx[nMax+1];
  LudcmpSvg( a, nMax+1, filterOrder+1, indx );
  //
  // right-hand side vector is unit vector, depending on which derivative we want
  //
  double b[nMax+1];
  b[0] = 1.0;
  for( std::size_t i=1; i<filterOrder+1; ++i )b[i] = 0.0;
  //
  // backsubstitute, giving one row of the inverse matrix
  //
  LubksbSvg( a, nMax+1, filterOrder+1, indx, b );
  //
  // zero the output array (it may be bigger than number of coefficients)
  //
  for( std::size_t j=0; j<np; ++j )c[j] = 0.0;
  //
  // each savitzky-golay coefficient is the dot product
  // of powers of an integer with the inverse matrix row.
  //
  for( int i=-1*nl; i<=static_cast<int>(nr); ++i )
  {
    double sum = b[0];
    int fac = 1;
    for( std::size_t j=1; j<=filterOrder; ++j )
    {
      fac *= i;
      sum += b[j]*fac;
    }
    //
    // store in wraparound order.
    //
    c[(np-i)%np] = sum;
  }
}

void LudcmpSvg( double *a, std::size_t np, std::size_t n, int *indx )
{
  std::size_t const nMax = 100;
  double const tiny = std::numeric_limits<double>::min();
  double vv[nMax];
  for( std::size_t i=0; i<n; ++i )
  {
    double aamax = 0.0;
    for( std::size_t j=0; j<n; ++j )
    {
      if( fabs(a[i+j*np]) > aamax )aamax = fabs(a[i+j*np]);
    }
    if( aamax == 0.0 )throw EExpressionError(wxT("SavitzkyGolay: singular matrix"));
    vv[i] = 1.0/aamax;
  }
  for( std::size_t j=1; j<=n; ++j )
  {
    if( j > 1 )
    {
      for( std::size_t i=1; i<=j-1; ++i )
      {
        double sum = a[i-1+(j-1)*np];
        if( i > 1 )
        {
          for( std::size_t k=1; k<=i-1; ++k )sum -= a[i-1+(k-1)*np]*a[k-1+(j-1)*np];
          a[i-1+(j-1)*np] = sum;
        }
      }
    }
    double aamax = 0.0;
    std::size_t imax;
    for( std::size_t i=j; i<=n; ++i )
    {
      double sum = a[i-1+(j-1)*np];
      if( j > 1 )
      {
        for( std::size_t k=1; k<=j-1; ++k )sum -= a[i-1+(k-1)*np]*a[k-1+(j-1)*np];
        a[i-1+(j-1)*np] = sum;
      }
      if( vv[i-1]*fabs(sum) >= aamax )
      {
        imax = i;
        aamax = vv[i-1]*fabs(sum);
      }
    }
    if( j != imax )
    {
      for( std::size_t k=1; k<=n; ++k )
      {
        double tmp = a[imax-1+(k-1)*np];
        a[imax-1+(k-1)*np] = a[j-1+(k-1)*np];
        a[j-1+(k-1)*np] = tmp;
      }
      vv[imax-1] = vv[j-1];
    }
    indx[j-1] = imax;
    if( j != n )
    {
      if( a[j-1+(j-1)*np] == 0.0 )a[j-1+(j-1)*np] = tiny;
      double tmp = 1.0/a[j-1+(j-1)*np];
      for( std::size_t i=j+1; i<=n; ++i )a[i-1+(j-1)*np] *= tmp;
    }
  }
  if( a[n-1+(n-1)*np] == 0.0 )a[n-1+(n-1)*np] = tiny;
}

void LubksbSvg( double *a, std::size_t np, std::size_t n,
                                 int *indx, double *b )
{
  std::size_t ii = 0;
  for( std::size_t i=1; i<=n; ++i )
  {
    int ll = indx[i-1];
    double sum = b[ll-1];
    b[ll-1] = b[i-1];
    if( ii != 0 )
    {
      for( std::size_t j=ii; j<=i-1; ++j )sum -= a[i-1+(j-1)*np]*b[j-1];
    }
    else if( sum != 0.0 )
    {
      ii = i;
    }
    b[i-1] = sum;
  }
  for( std::size_t i=n; i>=1; --i )
  {
    double sum = b[i-1];
    if( i < n )
    {
      for( std::size_t j=i+1; j<=n; ++j )sum -= a[i-1+(j-1)*np]*b[j-1];
    }
    b[i-1] = sum/a[i-1+(i-1)*np];
  }
}

double Poisson( double x, int n )
{
  double y;
  try
  {
    y = pow(x,static_cast<double>(n))*exp(-x)/Factorial(n);
  }
  catch (...)
  {
    throw EExpressionError(wxT("Poisson: over/underflow error"));
  }
  return y;
}

void WindowClip( double xp, double yp, double x, double y,
                 double xmin, double xmax, double ymin, double ymax,
                 double *xdraw, double *ydraw, int &ndraw )
{
  // WindowClip clips the line segment (xp,yp)-->(x,y), using the rectangular plot
  // window range: xmin <= x <= xmax; ymin <= y <= ymax. If the line segment
  // (xp,yp)-->(x,y) is totally outside the window then ndraw is set to 0 and
  // xdraw & ydraw are undefined. if (xp,yp) & (x,y) are both outside the
  // window, but the segment (xp,yp)-->(x,y) crosses the window, then ndraw
  // is set to 2 and the clipped line segment inside the window is given by:
  // (xdraw[0],ydraw[0]-->(xdraw[1],ydraw[1]). if (xp,yp) & (x,y) are both
  // inside the window, then there is no clipping and ndraw is set to 1 and
  // (xdraw[0],ydraw[0]) is set to (x,y). if (xp,yp) is inside and (x,y) is
  // outside the window then ndraw is set to 1 and the clipped line segment
  // inside the window is given by: (xp,yp)-->(xdraw[0],ydraw[0]). if (xp,yp)
  // is outside and (x,y) is inside the window then ndraw is set to 3 and the
  // clipped line segment is given by: (xdraw[0],ydraw[0] )-->(xdraw[1],ydraw[1]),
  // where (xdraw[1],ydraw[1]) equals (x,y).
  //
  double xminl = std::min(xmin,xmax);
  double xmaxl = std::max(xmin,xmax);
  double yminl = std::min(ymin,ymax);
  double ymaxl = std::max(ymin,ymax);
  bool xyout = (x<xminl || x>xmaxl || y<yminl || y>ymaxl);
  bool xypout = (xp<xminl || xp>xmaxl || yp<yminl || yp>ymaxl);
  //
  if( !xyout && !xypout ) // (x,y) and (xp,yp) are in the plot window
  {                       // Therefore there is no need for clipping
    ndraw = 1;
    xdraw[0] = x;
    ydraw[0] = y;
    return;
  }
  //
  // Determine which segments or sides of the window perimeter are crossed
  // by the line segment (xp,yp)-->(x,y).
  // cross(i) = true if the ith segment is crossed
  // (xc(i),yc(i)) is the point at which the ith segment is crossed
  // sc(i)         is the relative distance along the segment
  //               (xp,yp)-->(x,y) at which the ith window segment
  //               crosses it, i.e. the window segment crosses the
  //               segment (xp,yp)-->(x,y) at the point given by:
  //               (xc(i),yc(i)) = (xp,yp) + sc(i) * (x-xp,y-yp)
  //
  ndraw = 0;
  if( (x<xminl && xp<xminl) || (y<yminl && yp<yminl) ||
      (x>xmaxl && xp>xmaxl) || (y>ymaxl && yp>ymaxl) )return;
  bool cross[4];
  double xc[4], yc[4], sc[4];
  cross[0] = SegmentCross(xp,yp,x,y,xmin,ymin,xmax,ymin,xc[0],yc[0],sc[0]);
  cross[1] = SegmentCross(xp,yp,x,y,xmax,ymin,xmax,ymax,xc[1],yc[1],sc[1]);
  cross[2] = SegmentCross(xp,yp,x,y,xmax,ymax,xmin,ymax,xc[2],yc[2],sc[2]);
  cross[3] = SegmentCross(xp,yp,x,y,xmin,ymax,xmin,ymin,xc[3],yc[3],sc[3]);
  //
  // Store the crossing points in xdraw and ydraw
  //
  double denom = fabs(xmax-xmin) + fabs(ymax-ymin);
  if( denom == 0.0 )denom = 1.0;
  double distanceRatio = (fabs(x-xp)+fabs(y-yp))/denom;
  double sdraw[2];
  for( int i = 0; i < 4; ++i )
  {
    if( !cross[i] )continue;
    if( ndraw == 0 )
    {
      xdraw[ndraw] = xc[i];
      ydraw[ndraw] = yc[i];
      sdraw[ndraw] = sc[i];
      if( ++ndraw == 2 )break;
    }
    else
    {
      if( fabs(sdraw[ndraw-1]-sc[i])*distanceRatio > 0.0001 )
      {
        xdraw[ndraw] = xc[i];
        ydraw[ndraw] = yc[i];
        sdraw[ndraw] = sc[i];
        if( ++ndraw == 2 )break;
      }
    }
  }
  //
  // ndraw = 0 implies that the line segment (xp,yp)-->(x,y) is
  // totally outside the plot window
  //
  if( ndraw == 0 )return;
  if( ndraw == 2 )       // There are 2 crossing points
  {
    if( sdraw[0] > sdraw[1] )
    {
      double xd1 = xdraw[0];
      double yd1 = ydraw[0];
      xdraw[0] = xdraw[1];
      ydraw[0] = ydraw[1];
      xdraw[1] = xd1;
      ydraw[1] = yd1;
    }
    if( !xypout )
    {
      ndraw = 1;
      xdraw[0] = xdraw[1];
      ydraw[0] = ydraw[1];
    }
    return;
  }
  //
  // ndraw = 1 implies only one crossing of the line segment
  //
  if( xyout && xypout )
  {
    // ndraw = 1: Both (x,y) and (xp,yp) are outside the window.
    // Therefore since there is only one crossing point, it must
    // be at a corner of the plot window.
    //
    ndraw = 2;
    xdraw[1] = xdraw[0];
    ydraw[1] = ydraw[0];
    sdraw[1] = sdraw[0];
    return;
  }
  //
  // ndraw = 1, and one of (x,y) or (xp,yp) is inside the plot window
  // If (x,y) is outside then (xp,yp) is inside; so return
  //
  if( !xyout )  // (x,y) is inside and (xp,yp) is outside the plot window
  {
    ndraw = 3;
    xdraw[1] = x;
    ydraw[1] = y;
  }
  return;
}
 
bool SegmentCross( double x1, double y1, double x2, double y2,
                                    double x1p, double y1p, double x2p, double y2p,
                                    double &xc, double &yc, double &sc )
{
  // SegmentCross returns the value true/false if the line segment (x1,y1)-->(x2,y2)
  // crosses/(does not cross) the line segment (x1p,y1p)-->(x2p,y2p). if the line
  // segments cross then it also returns the point (xc,yc) at which they cross, as
  // well as the relative distance "sc" along the segment (x1,y1)-->(x2,y2) at
  // which they cross, i.e. (xc,yc) = (x1,y1) + sc * (x2-x1,y2-y1)
  //
  double dx1 = x1-x1p;
  double dy1 = y1-y1p;
  double a11 = x2p-x1p;
  double a12 = x1-x2;
  double a21 = y2p-y1p;
  double a22 = y1-y2;
  double deta = a11*a22-a12*a21;
  bool result = false;
  if( fabs(deta) <= std::numeric_limits<double>::epsilon() )return result;
  double t = (a22*dx1-a12*dy1)/deta;
  double s = (a11*dy1-a21*dx1)/deta;
  if( s < 0.0 || s > 1.0 || t < 0.0 || t > 1.0 )return result;
  result = true;
  sc = s;
  xc = x1 + s*(x2-x1);
  yc = y1 + s*(y2-y1);
  return result;
}

void BivariateInterpolation( std::vector<double> const &x, std::vector<double> const &y,
                             std::vector<double> const &z, std::vector<double> const &u,
                             std::vector<double> const &v, std::vector<double> &w )
{
  /*
    This routine interpolates, from values of the function given at input grid points
    in an x-y plane and for a given set of points in the plane, the values of a
    single-valued bivariate function z = z(x,y).
    The method is based on a piece-wise function composed of a set of bicubic
    polynomials in x and y.  Each polynomial is applicable to a rectangle of the
    input grid in the x-y plane.  Each polynomial is determined locally.

  input:
    x = x-coordinates of input grid points (in ascending order)
    y = y-coordinates of input grid points (in ascending order)
    z = values of the function at input grid points
    u = x-coordinates of the desired points
    v = y-coordinates of the desired points

  output:
    w = the interpolated z values at the desired points
  */
  if( x.size() < 2 )throw std::runtime_error( "x must have at least 2 elements" );
  if( y.size() < 2 )throw std::runtime_error( "y must have at least 2 elements" );
  if( u.size() < 1 )throw std::runtime_error( "u must have at least 1 element" );
  if( v.size() != u.size() )throw std::runtime_error( "u and v must have the same number of elements" );
  int xLen = x.size();
  int yLen = y.size();
  int uLen = u.size();
  //
  // za  = divided difference of z with respect to x
  // zb  = divided difference of z with respect to y
  // zab = second order divided difference of z with respect to x and y
  // zx  = partial derivative of z with respect to x
  // zy  = partial derivative of z with respect to y
  // zxy = second order partial derivative of z with respect to x and y
  //
  std::vector<double> zx(16), zy(16), zxy(16), zab(9), zb(10), za(10);
  //
  for( int i=1; i<xLen; ++i )
  {
    if( x[i-1] >= x[i] )throw std::runtime_error( "x must be monotonically increasing" );
  }
  for( int i=1; i<yLen; ++i )
  {
    if( y[i-1] >= y[i] )throw std::runtime_error( "y must be monotonically increasing" );
  }
  //
  // initial setting of previous values of ixpv and iypv
  //
  int ixPrevious = 0;
  int iyPrevious = 0;
  //
  // main loop
  //
  for( int k=1; k<=uLen; ++k )
  {
    // find the ix value for which x[ix-1] <= u[k-1] < x[ix]
    //
    int ix;
    if( xLen == 2 )               ix = 2;
    else if( u[k-1] >= x[xLen-1] )ix = xLen+1;
    else if( u[k-1] < x[0] )      ix = 1;
    else
    {
      int j1 = 2;
      int j0 = xLen;
      do
      {
        ix = (j1+j0)/2;
        u[k-1] >= x[ix-1] ? j1=ix+1 : j0=ix;
      } while ( j0 > j1 );
      ix = j0;
    }
    //
    // find the iy value for which y[iy-1] <= v[k-1] < y[iy]
    //
    int iy;
    if( yLen == 2 )               iy = 2;
    else if( v[k-1] >= y[yLen-1] )iy = yLen+1;
    else if( v[k-1] < y[0] )      iy = 1;
    else
    {
      int j1 = 2;
      int j0 = yLen;
      do
      {
        iy = (j1+j0)/2;
        v[k-1] >= y[iy-1] ? j1=iy+1 : j0=iy;
      } while ( j0 > j1 );
      iy = j0;
    }
    //
    // check if the desired point is in the same rectangle as the previous point
    // if yes, skip to the computation of the polynomial
    //
    double y3, z33, x3;
    if( ix!=ixPrevious || iy!=iyPrevious )
    {
      ixPrevious = ix;
      iyPrevious = iy;
      //
      // routines to pick up necessary x, y, and z values, to compute the za, zb,
      // and zab values, and to estimate them when necessary
      //
      int jx = ix;
      if( jx == 1 )jx = 2;
      if( jx == xLen+1 )jx = xLen;
      int jy = iy;
      if( jy == 1 )jy = 2;
      if( jy == yLen+1 )jy = yLen;
      //
      // in the rectangle that contains the desired point
      //
      x3 = x[jx-2];
      zx[8] = x[jx-1];
      double a3 = 1.0/(zx[8]-x3);
      y3 = y[jy-2];
      zy[2] = y[jy-1];
      double b3 = 1.0/(zy[2]-y3);
      z33 = z[jx-2+(jy-2)*yLen];
      zxy[4] = z[jx-1+(jy-2)*yLen];
      zy[11] = z[jx-2+(jy-1)*yLen];
      zxy[2] = z[jx-1+(jy-1)*yLen];
      za[2] = (zxy[4]-z33)*a3;
      za[7] = (zxy[2]-zy[11])*a3;
      zb[4] = (zy[11]-z33)*b3;
      zb[5] = (zxy[2]-zxy[4])*b3;
      zab[4] = (zb[5]-zb[4])*a3;
      //
      // in the x direction
      //
      if( xLen == 2 )
      {
        za[1] = za[2];
        za[6] = za[7];
        za[3] = 2*za[2] - za[1];
        za[8] = 2*za[7] - za[6];
      }
      else
      {
        if( jx == 2 )
        {
          zx[11] = x[jx];
          zx[7] = 1.0/(zx[11]-zx[8]);
          zxy[8] = z[jx+(jy-2)*yLen];
          zxy[11] = z[jx+(jy-1)*yLen];
          za[3] = (zxy[8]-zxy[4])*zx[7];
          za[8] = (zxy[11]-zxy[2])*zx[7];
          za[1] = 2*za[2] - za[3];
          za[6] = 2*za[7] - za[8];
        }
        else
        {
          zx[2] = x[jx-3];
          zx[4] = 1.0/(x3-zx[2]);
          zy[4] = z[jx-3+(jy-2)*yLen];
          zy[7] = z[jx-3+(jy-1)*yLen];
          za[1] = (z33-zy[4])*zx[4];
          za[6] = (zy[11]-zy[7])*zx[4];
          if( jx == xLen )
          {
            za[3] = 2*za[2] - za[1];
            za[8] = 2*za[7] - za[6];
          }
          else
          {
            zx[11] = x[jx];
            zx[7] = 1.0/(zx[11]-zx[8]);
            zxy[8] = z[jx+(jy-2)*yLen];
            zxy[11] = z[jx+(jy-1)*yLen];
            za[3] = (zxy[8]-zxy[4])*zx[7];
            za[8] = (zxy[11]-zxy[2])*zx[7];
          }
        }
      }
      zab[3] = (za[6]-za[1])*b3;
      zab[5] = (za[8]-za[3])*b3;
      if( jx <= 3 )
      {
        za[0] = 2*za[1] - za[2];
        za[5] = 2*za[6] - za[7];
      }
      else
      {
        za[0] = (zy[4]-z[jx-4+(jy-2)*yLen])/(zx[2]-x[jx-4]);
        za[5] = (zy[7]-z[jx-4+(jy-1)*yLen])/(zx[2]-x[jx-4]);
      }
      if( jx >= xLen-1 )
      {
        za[4] = 2*za[3] - za[2];
        za[9] = 2*za[8] - za[7];
      }
      else
      {
        za[4] = (z[jx+1+(jy-2)*yLen]-zxy[8])/(x[jx+1]-zx[11]);
        za[9] = (z[jx+1+(jy-1)*yLen]-zxy[11])/(x[jx+1]-zx[11]);
      }
      //
      // in the y direction
      //
      if( yLen == 2 )
      {
        zb[2] = zb[4];
        zb[3] = zb[5];
        zb[6] = 2*zb[4] - zb[2];
        zb[7] = 2*zb[5] - zb[3];
      }
      else
      {
        if( jy == 2 )
        {
          zx[14] = y[jy];
          zy[13] = 1.0/(zx[14]-zy[2]);
          zy[14] = z[jx-2+(jy)*yLen];
          zxy[7] = z[jx-1+(jy)*yLen];
          zb[6] = (zy[14]-zy[11])*zy[13];
          zb[7] = (zxy[7]-zxy[2])*zy[13];
          zb[2] = 2*zb[4] - zb[6];
          zb[3] = 2*zb[5] - zb[7];
        }
        else
        {
          zx[13] = y[jy-3];
          zy[1] = 1.0/(y3-zx[13]);
          zy[8] = z[jx-2+(jy-3)*yLen];
          zxy[1] = z[jx-1+(jy-3)*yLen];
          zb[2] = (z33-zy[8])*zy[1];
          zb[3] = (zxy[4]-zxy[1])*zy[1];
          if( jy == yLen )
          {
            zb[6] = 2*zb[4] - zb[2];
            zb[7] = 2*zb[5] - zb[3];
          }
          else
          {
            zx[14] = y[jy];
            zy[13] = 1.0/(zx[14]-zy[2]);
            zy[14] = z[jx-2+(jy)*yLen];
            zxy[7] = z[jx-1+(jy)*yLen];
            zb[6] = (zy[14]-zy[11])*zy[13];
            zb[7] = (zxy[7]-zxy[2])*zy[13];
          }
        }
      }
      zab[1] = (zb[3]-zb[2])*a3;
      zab[7] = (zb[7]-zb[6])*a3;
      if( jy <= 3 )
      {
        zb[0] = 2*zb[2] - zb[4];
        zb[1] = 2*zb[3] - zb[5];
      }
      else
      {
        zb[0] = (zy[8]-z[jx-2+(jy-4)*yLen])/(zx[13]-y[jy-4]);
        zb[1] = (zxy[1]-z[jx-1+(jy-4)*yLen])/(zx[13]-y[jy-4]);
      }
      if( jy >= yLen-1 )
      {
        zb[8] = 2*zb[6] - zb[4];
        zb[9] = 2*zb[7] - zb[5];
      }
      else
      {
        zb[8] = (z[jx-2+(jy+1)*yLen]-zy[14])/(y[jy+1]-zx[14]);
        zb[9] = (z[jx-1+(jy+1)*yLen]-zxy[7])/(y[jy+1]-zx[14]);
      }
      //
      // in the diagonal directions
      //
      if( xLen == 2 )
      {
        zab[0] = zab[1];
        zab[2] = zab[1];
        zab[6] = zab[7];
        zab[8] = zab[7];
      }
      else if( yLen == 2 )
      {
        zab[0] = zab[3];
        zab[6] = zab[3];
        zab[2] = zab[5];
        zab[8] = zab[5];
      }
      else if( jx == xLen )
      {
        if( jy == 2 )
        {
          zab[6] = (zb[6]-(z[jx-3+(jy)*yLen]-zy[7])*zy[13])*zx[4];
          zab[0] = 2*zab[3] - zab[6];
          zab[2] = 2*zab[1] - zab[0];
          zab[8] = 2*zab[7] - zab[6];
        }
        else
        {
          zab[0] = (zb[2]-(zy[4]-z[jx-3+(jy-3)*yLen])*zy[1])*zx[4];
          zab[6] = (jy==yLen ? 2*zab[3]-zab[0] :
                              (zb[6]-(z[jx-3+(jy)*yLen]-zy[7])*zy[13])*zx[4]);
          zab[2] = 2*zab[1] - zab[0];
          zab[8] = 2*zab[7] - zab[6];
        }
      }
      else if( jy == 2 )
      {
        zab[8] = ((z[jx+(jy)*yLen]-zxy[11])*zy[13]-zb[7])*zx[7];
        zab[2] = 2*zab[5] - zab[8];
        if( jx != 2 )
        {
          zab[6] = (zb[6]-(z[jx-3+(jy)*yLen]-zy[7])*zy[13])*zx[4];
          zab[0] = 2*zab[3] - zab[6];
        }
        else
        {
          zab[0] = 2*zab[1] - zab[2];
          zab[6] = 2*zab[7] - zab[8];
        }
      }
      else
      {
        zab[2] = ((zxy[8]-z[jx+(jy-3)*yLen])*zy[1]-zb[3])*zx[7];
        if( jy == yLen )
        {
          zab[8] = 2*zab[5] - zab[2];
          if( jx != 2 )
          {
            zab[0] = (zb[2]-(zy[4]-z[jx-3+(jy-3)*yLen])*zy[1])*zx[4];
            zab[6] = 2*zab[3] - zab[0];
          }
          else
          {
            zab[0] = 2*zab[1] - zab[2];
            zab[6] = 2*zab[7] - zab[8];
          }
        }
        else
        {
          zab[8] = ((z[jx+(jy)*yLen]-zxy[11])*zy[13]-zb[7])*zx[7];
          if( jy != 2 )
          {
            if( jx != 2 )
            {
              zab[0] = (zb[2]-(zy[4]-z[jx-3+(jy-3)*yLen])*zy[1])*zx[4];
              zab[6] = (zb[6]-(z[jx-3+(jy)*yLen]-zy[7])*zy[13])*zx[4];
            }
            else
            {
              zab[0] = 2*zab[1] - zab[2];
              zab[6] = 2*zab[7] - zab[8];
            }
          }
          else
          {
            zab[2] = 2*zab[5] - zab[8];
            if( jx != 2 )
            {
              zab[6] = (zb[6]-(z[jx-3+(jy)*yLen]-zy[7])*zy[13])*zx[4];
              zab[0] = 2*zab[3] - zab[6];
            }
            else
            {
              zab[0] = 2*zab[1] - zab[2];
              zab[6] = 2*zab[7] - zab[8];
            }
          }
        }
      }
      //
      // numerical differentiation   ---   to determine partial derivatives zx, zy, and zxy
      // as weighted means of divided differences za, zb, and zab, respectively
      //
      double w1, w2, w3, w4, w5;
      double wx2, wx3, wy2, wy3;
      for( jy=2; jy<=3; ++jy )
      {
        for( jx=2; jx<=3; ++jx )
        {
          w2 = fabs(za[jx+1+(jy-2)*5]-za[jx+(jy-2)*5]);
          w3 = fabs(za[jx-1+(jy-2)*5]-za[jx-2+(jy-2)*5]);
          double sw = w2 + w3;
          if( sw == 0.0 )
          {
            wx2 = 0.5;
            wx3 = 0.5;
          }
          else
          {
            wx2 = w2/sw;
            wx3 = w3/sw;
          }
          zx[jx-1+(jy-1)*4] = wx2*za[jx-1+(jy-2)*5] + wx3*za[jx+(jy-2)*5];
          w2 = fabs(zb[jx-2+(jy+1)*2]-zb[jx-2+(jy)*2]);
          w3 = fabs(zb[jx-2+(jy-1)*2]-zb[jx-2+(jy-2)*2]);
          sw = w2 + w3;
          if( sw == 0.0 )
          {
            wy2 = 0.5;
            wy3 = 0.5;
          }
          else
          {
            wy2 = w2/sw;
            wy3 = w3/sw;
          }
          zy[jx-1+(jy-1)*4] = wy2*zb[jx-2+(jy-1)*2] + wy3*zb[jx-2+(jy)*2];
          zxy[jx-1+(jy-1)*4] =
              wy2*(wx2*zab[jx-2+(jy-2)*3]+wx3*zab[jx-1+(jy-2)*3]) +
              wy3*(wx2*zab[jx-2+(jy-1)*3]+wx3*zab[jx-1+(jy-1)*3]);
        }
      }
      //
      // when (u[k-1]<x[0]) || (u[k-1]>x[lx-1])
      //
      if( ix == xLen+1 )
      {
        w4 = zx[4]*(3.0*a3+zx[4]);
        w5 = 2.0*a3*(a3-zx[4]) + w4;
        for( int jy=2; jy<=3; ++jy )
        {
          zx[3+(jy-1)*4] = (w4*za[3+(jy-2)*5]+w5*za[4+(jy-2)*5])/(w4+w5);
          zy[3+(jy-1)*4] = zy[2+(jy-1)*4] + zy[2+(jy-1)*4] - zy[1+(jy-1)*4];
          zxy[3+(jy-1)*4] = zxy[2+(jy-1)*4] + zxy[2+(jy-1)*4] - zxy[1+(jy-1)*4];
          for( int jx=2; jx<=3; ++jx )
          {
            zx[jx-1+(jy-1)*4] = zx[jx+(jy-1)*4];
            zy[jx-1+(jy-1)*4] = zy[jx+(jy-1)*4];
            zxy[jx-1+(jy-1)*4] = zxy[jx+(jy-1)*4];
          }
        }
        x3 = zx[8];
        z33 = zxy[4];
        for( int jy=1; jy<=5; ++jy )zb[0+(jy-1)*2] = zb[1+(jy-1)*2];
        a3 = zx[4];
        jx = 3;
        za[2] = za[jx];
        for( int jy=1; jy<=3; ++jy )zab[1+(jy-1)*3] = zab[jx-1+(jy-1)*3];
      }
      else if( ix == 1 )
      {
        w2 = zx[7]*(3.0*a3+zx[7]);
        w1 = 2.0*a3*(a3-zx[7]) + w2;
        for( int jy=2; jy<=3; ++jy )
        {
          zx[0+(jy-1)*4] = (w1*za[0+(jy-2)*5]+w2*za[1+(jy-2)*5])/(w1+w2);
          zy[0+(jy-1)*4] = 2*zy[1+(jy-1)*4] - zy[2+(jy-1)*4];
          zxy[0+(jy-1)*4] = 2*zxy[1+(jy-1)*4] - zxy[2+(jy-1)*4];
          for( int jx1=2; jx1<=3; ++jx1 )
          {
            jx = 5 - jx1;
            zx[jx-1+(jy-1)*4] = zx[jx-2+(jy-1)*4];
            zy[jx-1+(jy-1)*4] = zy[jx-2+(jy-1)*4];
            zxy[jx-1+(jy-1)*4] = zxy[jx-2+(jy-1)*4];
          }
        }
        x3 -= 1.0/zx[7];
        z33 -= za[1]/zx[7];
        for( int jy=1; jy<=5; ++jy )zb[1+(jy-1)*2] = zb[0+(jy-1)*2];
        for( int jy=2; jy<=4; ++jy )zb[0+(jy-1)*2] = zb[0+(jy-1)*2] - zab[0+(jy-2)*3]/zx[7];
        a3 = zx[7];
        jx = 1;
        za[2] = za[jx];
        for( int jy=1; jy<=3; ++jy )zab[1+(jy-1)*3] = zab[jx-1+(jy-1)*3];
      }
      //
      // when v[k-1] < y[0]  or  v[k-1] > y[yLen-1]
      //
      if( iy == yLen+1 )
      {
        w4 = zy[1]*(3.0*b3+zy[1]);
        w5 = 2.0*b3*(b3-zy[1]) + w4;
        for( jx=2; jx<=3; ++jx )
        {
          if( !(jx==3 && ix==xLen+1) && !(jx==2 && ix==1) )
          {
            zy[jx-1+(3)*4] = (w4*zb[jx-2+(3)*2]+w5*zb[jx-2+(4)*2])/(w4+w5);
            zx[jx-1+(3)*4] = 2*zx[jx-1+(2)*4] - zx[jx-1+(1)*4];
            zxy[jx-1+(3)*4] = 2*zxy[jx-1+(2)*4] - zxy[jx-1+(1)*4];
          }
          for( int jy=2; jy<=3; ++jy )
          {
            zy[jx-1+(jy-1)*4] = zy[jx-1+(jy)*4];
            zx[jx-1+(jy-1)*4] = zx[jx-1+(jy)*4];
            zxy[jx-1+(jy-1)*4] = zxy[jx-1+(jy)*4];
          }
        }
        y3 = zy[2];
        z33 += zb[4]/b3;
        za[2] += zab[4]/b3;
        zb[4] = zb[6];
        zab[4] = zab[7];
        b3 = zy[1];
        if( ix==1 || ix==xLen+1 )
        {
          jx = ix/(xLen+1) + 2;
          int jx1 = 5 - jx;
          jy = iy/(yLen+1) + 2;
          int jy1 = 5 - jy;
          zx[jx-1+(jy-1)*4] = zx[jx1-1+(jy-1)*4] + zx[jx-1+(jy1-1)*4] - zx[jx1-1+(jy1-1)*4];
          zy[jx-1+(jy-1)*4] = zy[jx1-1+(jy-1)*4] + zy[jx-1+(jy1-1)*4] - zy[jx1-1+(jy1-1)*4];
          zxy[jx-1+(jy-1)*4] = zxy[jx1-1+(jy-1)*4] + zxy[jx-1+(jy1-1)*4] - zxy[jx1-1+(jy1-1)*4];
        }
      }
      else if( iy == 1 )
      {
        w2 = zy[13]*(3.0*b3+zy[13]);
        w1 = 2.0*b3*(b3-zy[13]) + w2;
        for( int jx=2; jx<=3; ++jx )
        {
          if( !(jx==3 && ix==xLen+1) && !(jx==2 && ix==1) )
          {
            zy[jx-1] = (w1*zb[jx-2]+w2*zb[jx-2+(1)*2])/(w1+w2);
            zx[jx-1] = 2*zx[jx-1+(1)*4] - zx[jx-1+(2)*4];
            zxy[jx-1] = 2*zxy[jx-1+(1)*4] - zxy[jx-1+(2)*4];
          }
          for( int jy1=2; jy1<=3; ++jy1 )
          {
            jy = 5 - jy1;
            zy[jx-1+(jy-1)*4] = zy[jx-1+(jy-2)*4];
            zx[jx-1+(jy-1)*4] = zx[jx-1+(jy-2)*4];
            zxy[jx-1+(jy-1)*4] = zxy[jx-1+(jy-2)*4];
          }
        }
        y3 -= 1.0/zy[13];
        z33 -= zb[2]/zy[13];
        za[2] -= zab[1]/zy[13];
        zb[4] = zb[2];
        zab[4] = zab[1];
        b3 = zy[13];
        if( ix==1 || ix==xLen+1 )
        {
          int jx = ix/(xLen+1) + 2;
          int jx1 = 5 - jx;
          int jy = iy/(yLen+1) + 2;
          int jy1 = 5 - jy;
          zx[jx-1+(jy-1)*4] = zx[jx1-1+(jy-1)*4] + zx[jx-1+(jy1-1)*4] - zx[jx1-1+(jy1-1)*4];
          zy[jx-1+(jy-1)*4] = zy[jx1-1+(jy-1)*4] + zy[jx-1+(jy1-1)*4] - zy[jx1-1+(jy1-1)*4];
          zxy[jx-1+(jy-1)*4] = zxy[jx1-1+(jy-1)*4] + zxy[jx-1+(jy1-1)*4] - zxy[jx1-1+(jy1-1)*4];
        }
      }
      //
      // determination of the coefficients of the polynomial
      //
      double zx3b3 = (zx[9]-zx[5])*b3;
      double zx4b3 = (zx[10]-zx[6])*b3;
      double zy3a3 = (zy[6]-zy[5])*a3;
      double zy4a3 = (zy[10]-zy[9])*a3;
      zx[1] = zab[4] - zx3b3 - zy3a3 + zxy[5];
      zx[4] = zx4b3 - zx3b3 - zxy[6] + zxy[5];
      zx[7] = zy4a3 - zy3a3 - zxy[9] + zxy[5];
      zy[1] = zxy[10] - zxy[6] - zxy[9] + zxy[5];
      zy[13] = 2*zx[1] - zx[4] - zx[7];
      zx[2] = a3*a3;
      zy[2] = b3*b3;
      zx[14] = (2.0*(zb[4]-zy[5])+zb[4]-zy[9])*b3;
      zy[4] = (-2.0*zb[4]+zy[9]+zy[5])*zy[2];
      zy[7] = (2.0*(zx3b3-zxy[5])+zx3b3-zxy[9])*b3;
      zy[8] = (-2.0*zx3b3+zxy[9]+zxy[5])*zy[2];
      zy[11] = (2.0*(za[2]-zx[5])+za[2]-zx[6])*a3;
      zy[14] = (2.0*(zy3a3-zxy[5])+zy3a3-zxy[6])*a3;
      zxy[1] = (3.0*(zx[1]+zy[13])+zy[1])*a3*b3;
      zxy[4] = (-3.0*zy[13]-zx[4]-zy[1])*a3*zy[2];
      zxy[2] = (-2.0*za[2]+zx[6]+zx[5])*zx[2];
      zxy[7] = (-2.0*zy3a3+zxy[6]+zxy[5])*zx[2];
      zxy[8] = (-3.0*zy[13]-zx[7]-zy[1])*b3*zx[2];
      zxy[11] = (zy[1]+zy[13]+zy[13])*zx[2]*zy[2];
    }
    //
    // computation of the polynomial
    //
    double vk = v[k-1] - y3;
    zx[1] = z33 + vk*(zy[5]+vk*(zx[14]+vk*zy[4]));
    zx[4] = zx[5] + vk*(zxy[5]+vk*(zy[7]+vk*zy[8]));
    zx[7] = zy[11] + vk*(zy[14]+vk*(zxy[1]+vk*zxy[4]));
    zy[1] = zxy[2] + vk*(zxy[7]+vk*(zxy[8]+vk*zxy[11]));
    double uk = u[k-1] - x3;
    w[k-1] = zx[1] + uk*(zx[4]+uk*(zx[7]+uk*zy[1]));
  }
}

void BivariateSmoothing( std::vector<double> const &x, std::vector<double> const &y,
                         std::vector<double> const &z, int mx, int my,
                         std::vector<double> &u, std::vector<double> &v,
                         std::vector<double> &w )
{
  /*
    smooth surface fitting
    This routine fits a smooth surface of a single-valued bivariate function z = z(x,y) to a set
    of input data points given at input grid points in an x-y plane.  It generates a set of output
    grid points by equally dividing the x and y coordinates in each interval between a pair of
    input grid points, interpolates the z value for the x and y values of each output grid point,
    and generates a set of output points consisting of input data points and the interpolated points.
    The method is based on a piece-wise function composed of a set of bicubic polynomials in x and y.
    Each polynomial is applicable to a rectangle of the input grid in the x-y plane.  Each polynomial
    is determined locally.

    input:
    x   = the x coordinates of input grid points (in ascending or descending order)
    y   = the y coordinates of input grid points (in ascending or descending order)
    z   = the values of the function at input grid points
    mx  = number of subintervals between each pair of input grid points in the x coordinate
          must be >= 2
    my  = number of subintervals between each pair of input grid points in the y coordinate
          must be >= 2

    output: ( where  xLen = x.size() and yLen = y.size() )
    u   = array of dimension mx*(xLen-1)+1 containing the x coordinates of output points
    v   = array of dimension my*(yLen-1)+1 containing the y coordinates of output points
    w   = doubly-dimensioned array of dimension ( mx*(xLen-1)+1, my*(yLen-1)+1 )
          containing the z coordinates of output points
  */
  // error check
  //
  if( x.size() < 2 )throw std::runtime_error( "x must have at least 2 elements" );
  if( y.size() < 2 )throw std::runtime_error( "y must have at least 2 elements" );
  if( mx <= 1 )throw std::runtime_error( "nu must be > the number of elements in x" );
  if( my <= 1 )throw std::runtime_error( "nv must be > the number of elements in y" );
  //
  int xLen = x.size();
  int yLen = y.size();
  //
  int nu = mx*(xLen-1)+1;
  int nv = my*(yLen-1)+1;
  //
  if( x[0] == x[1] )throw std::runtime_error( "x must be monotonic" );
  if( x[0] < x[1] )
  {
    for( int ix=2; ix<=xLen; ++ix )
    {
      if( x[ix-2] >= x[ix-1] )throw std::runtime_error( "x must be monotonic" );
    }
  }
  else
  {
    for( int ix=2; ix<=xLen; ++ix )
    {
      if( x[ix-2] <= x[ix-1] )throw std::runtime_error( "x must be monotonic" );
    }
  }
  if( y[0] == y[1] )throw std::runtime_error( "y must be monotonic" );
  if( y[0] < y[1] )
  {
    for( int iy=2; iy<=yLen; ++iy )
    {
      if( y[iy-2] >= y[iy-1] )throw std::runtime_error( "y must be monotonic" );
    }
  }
  else
  {
    for( int iy=2; iy<=yLen; ++iy )
    {
      if( y[iy-2] <= y[iy-1] )throw std::runtime_error( "y must be monotonic" );
    }
  }
  //
  // computation of the u array
  //
  int k = 0;
  double x3, x4=x[0];
  u[0] = x4;
  for( int ix=1; ix<xLen; ++ix )
  {
    x3 = x4;
    x4 = x[ix];
    for( int jx=0; jx<mx-1; ++jx )
    {
      ++k;
      u[k] = u[k-1] + (x4-x3)/mx;
    }
    u[++k] = x4;
  }
  //
  // computation of the v array
  //
  k = 0;
  double y3, y4=y[0];
  v[0] = y4;
  for( int iy=1; iy<yLen; ++iy )
  {
    y3 = y4;
    y4 = y[iy];
    for( int jy=0; jy<my-1; ++jy )
    {
      ++k;
      v[k] = v[k-1] + (y4-y3)/my;
    }
    v[++k] = y4;
  }
  //
  // main loops
  //
  std::vector<double> za(8), zb(5), zab(6), zx(2), zy(2), zxy(2);
  int jymx = my;
  int kv0 = 0;
  for( int iy=2; iy<=yLen; ++iy )
  {
    int ix6 = 0;
    if( iy == yLen )jymx = my+1;
    int jxmx = mx;
    int ku0 = 0;
    for( int ix=1; ix<=xLen; ++ix )
    {
      double a3, a4, a5, b1, b2, b3, b4, b5, x5, x6, z33, z43, z53, z54, z62, z63, z64, z65;
      double z3b3, z5b1, z5b2, z5b3, z5b4, z5b5, z6b1, z6b2, z6b3, z6b4, z6b5;
      double zx33, zx34, zy33, zy34, zxy33, zxy34, za5b2, za5b3, za5b4;

      if( ix == xLen )jxmx = mx+1;
      //
      // pick up necessary x, y, and z values
      // compute the za, zb, and zab values, and estimate them when necessary
      //
      if( ix == 1 )
      {
        y3 = y[iy-2];
        y4 = y[iy-1];
        b3 = 1.0/(y4-y3);
        if( iy > 2 )b2 = 1.0/(y3-y[iy-3]);
        if( iy > 3 )b1 = 1.0/(y[iy-3]-y[iy-4]);
        if( iy < yLen )b4 = 1.0/(y[iy]-y4);
        if( iy+1 < yLen )b5 = 1.0/(y[iy+1]-y[iy]);
      }
      else // save the old values
      {
        za[0] = za[1];
        za[4] = za[5];
        x3 = x4;
        z33 = z43;
        z3b3 = zb[2];
        a3 = a4;
        za[1] = za[2];
        za[5] = za[6];
        zab[0] = zab[1];
        zab[2] = zab[3];
        zab[4] = zab[5];
        x4 = x5;
        z43 = z53;
        zb[0] = z5b1;
        zb[1] = z5b2;
        zb[2] = z5b3;
        zb[3] = z5b4;
        zb[4] = z5b5;
        a4 = a5;
        za[2] = za[3];
        za[6] = za[7];
        zab[1] = za5b2;
        zab[3] = za5b3;
        zab[5] = za5b4;
        x5 = x6;
        z53 = z63;
        z54 = z64;
        z5b1 = z6b1;
        z5b2 = z6b2;
        z5b3 = z6b3;
        z5b4 = z6b4;
        z5b5 = z6b5;
      }
      //
      // compute the za, zb, and zab values and estimate the zb values
      // when  iy <= 3   or   iy >= ly-1
      //
L180:
      ++ix6;
      if( ix6 > xLen ) // estimate the za and zab values when  ix >= xLen-1   and   xLen > 2
      {
        if( xLen == 2 ) // estimate the za and zab values when   ix >= xLen-1   and   xLen == 2
        {
          za[3] = za[2];
          za[7] = za[6];
          if( ix == xLen ) // save the old values when ix != 1
          {
            zx33 = zx[0];
            zx34 = zx[1];
            zy33 = zy[0];
            zy34 = zy[1];
            zxy33 = zxy[0];
            zxy34 = zxy[1];
          }
          else
          {
            za5b2 = zab[1];
            za5b3 = zab[3];
            za5b4 = zab[5];
            if( ix == 1 ) // estimate the za and zab values when  ix == 1
            {
              za[1] = 2*za[2] - za[3];
              za[0] = 2*za[1] - za[2];
              za[5] = 2*za[6] - za[7];
              za[4] = 2*za[5] - za[6];
              zab[0] = 2*zab[1] - za5b2;
              zab[2] = 2*zab[3] - za5b3;
              zab[4] = 2*zab[5] - za5b4;
            }
            else
            {
              zx33 = zx[0];
              zx34 = zx[1];
              zy33 = zy[0];
              zy34 = zy[1];
              zxy33 = zxy[0];
              zxy34 = zxy[1];
            }
          }
        }
        else
        {
          za[3] = 2*za[2] - za[1];
          za[7] = 2*za[6] - za[5];
          if( ix != xLen )
          {
            za5b2 = 2*zab[1] - zab[0];
            za5b3 = 2*zab[3] - zab[2];
            za5b4 = 2*zab[5] - zab[4];
          }
          zx33 = zx[0];
          zx34 = zx[1];
          zy33 = zy[0];
          zy34 = zy[1];
          zxy33 = zxy[0];
          zxy34 = zxy[1];
        }
        goto L300;
      }
      x6 = x[ix6-1];
      z63 = z[ix6-1+(iy-2)*yLen];
      z64 = z[ix6-1+(iy-1)*yLen];
      z6b3 = (z64-z63)*b3;
      if( yLen == 2 )
      {
        z6b2 = z6b3;
        z6b4 = z6b3;
      }
      else
      {
        if( iy != 2 )
        {
          z62 = z[ix6-1+(iy-3)*yLen];
          z6b2 = (z63-z62)*b2;
          if( iy == yLen )
          {
            z6b4 = 2*z6b3 - z6b2;
            goto L210;
          }
        }
        z65 = z[ix6-1+(iy)*yLen];
        z6b4 = (z65-z64)*b4;
        if( iy == 2 )z6b2 = 2*z6b3 - z6b4;
      }
L210:
      z6b1 = ( iy <= 3 ? 2*z6b2-z6b3 : (z62-z[ix6-1+(iy-4)*yLen])*b1 );
      z6b5 = ( iy >= yLen-1 ? 2*z6b4-z6b3 : (z[ix6-1+(iy+1)*yLen]-z65)*b5 );
      if( ix6 == 1 )
      {
        x5 = x6;
        z53 = z63;
        z54 = z64;
        z5b1 = z6b1;
        z5b2 = z6b2;
        z5b3 = z6b3;
        z5b4 = z6b4;
        z5b5 = z6b5;
        goto L180;
      }
      a5 = 1.0/(x6-x5);
      za[3] = (z63-z53)*a5;
      za[7] = (z64-z54)*a5;
      za5b2 = (z6b2-z5b2)*a5;
      za5b3 = (z6b3-z5b3)*a5;
      za5b4 = (z6b4-z5b4)*a5;
      if( ix6 == 2 )
      {
        x4 = x5;
        z43 = z53;
        zb[0] = z5b1;
        zb[1] = z5b2;
        zb[2] = z5b3;
        zb[3] = z5b4;
        zb[4] = z5b5;
        a4 = a5;
        za[2] = za[3];
        za[6] = za[7];
        zab[1] = za5b2;
        zab[3] = za5b3;
        zab[5] = za5b4;
        x5 = x6;
        z53 = z63;
        z54 = z64;
        z5b1 = z6b1;
        z5b2 = z6b2;
        z5b3 = z6b3;
        z5b4 = z6b4;
        z5b5 = z6b5;
        goto L180;
      }
      if( ix == 1 ) // estimate the za and zab values when  ix == 1
      {
        za[1] = 2*za[2] - za[3];
        za[0] = 2*za[1] - za[2];
        za[5] = 2*za[6] - za[7];
        za[4] = 2*za[5] - za[6];
        zab[0] = 2*zab[1] - za5b2;
        zab[2] = 2*zab[3] - za5b3;
        zab[4] = 2*zab[5] - za5b4;
      }
      else
      {
        zx33 = zx[0];
        zx34 = zx[1];
        zy33 = zy[0];
        zy34 = zy[1];
        zxy33 = zxy[0];
        zxy34 = zxy[1];
      }
      //
      // numerical differentiation
      // determine partial derivatives zx, zy, and zxy as weighted means of divided
      // differences za, zb, and zab, respectively
      // save the old values when ix != 1
      //
      // new computation
      //
L300:
      for( int jy=1; jy<=2; ++jy )
      {
        double wx2, wx3;
        double w2 = fabs(za[3+(jy-1)*4]-za[2+(jy-1)*4]);
        double w3 = fabs(za[1+(jy-1)*4]-za[(jy-1)*4]);
        if( w2 + w3 == 0.0 )
        {
          wx2 = 0.5;
          wx3 = 0.5;
        }
        else
        {
          wx2 = w2/(w2 + w3);
          wx3 = w3/(w2 + w3);
        }
        zx[jy-1] = wx2*za[1+(jy-1)*4] + wx3*za[2+(jy-1)*4];
        double wy2, wy3;
        w2 = fabs(zb[jy+2]-zb[jy+1]);
        w3 = fabs(zb[jy]-zb[jy-1]);
        if( w2 + w3 == 0.0 )
        {
          wy2 = 0.5;
          wy3 = 0.5;
        }
        else
        {
          wy2 = w2/(w2 + w3);
          wy3 = w3/(w2 + w3);
        }
        zy[jy-1] = wy2*zb[jy] + wy3*zb[jy+1];
        zxy[jy-1] = wy2*(wx2*zab[(jy-1)*2]+wx3*zab[1+(jy-1)*2]) +
            wy3*(wx2*zab[(jy)*2]+wx3*zab[1+(jy)*2]);
      }
      if( ix != 1 )
      {
        //
        // determination of the coefficients of the polynomial
        //
        double zx3b3 = (zx34-zx33)*b3;
        double zx4b3 = (zx[1]-zx[0])*b3;
        double zy3a3 = (zy[0]-zy33)*a3;
        double zy4a3 = (zy[1]-zy34)*a3;
        double a = zab[2] - zx3b3 - zy3a3 + zxy33;
        double b = zx4b3 - zx3b3 - zxy[0] + zxy33;
        double c = zy4a3 - zy3a3 - zxy34 + zxy33;
        double d = zxy[1] - zxy[0] - zxy34 + zxy33;
        double e = 2*a - b - c;
        double p02 = (2.0*(z3b3-zy33)+z3b3-zy34)*b3;
        double p03 = (-2.0*z3b3+zy34+zy33)*b3*b3;
        double p12 = (2.0*(zx3b3-zxy33)+zx3b3-zxy34)*b3;
        double p13 = (-2.0*zx3b3+zxy34+zxy33)*b3*b3;
        double p20 = (2.0*(za[1]-zx33)+za[1]-zx[0])*a3;
        double p21 = (2.0*(zy3a3-zxy33)+zy3a3-zxy[0])*a3;
        double p22 = (3.0*(a+e)+d)*a3*b3;
        double p23 = (-3.0*e-b-d)*a3*b3*b3;
        double p30 = (-2.0*za[1]+zx[0]+zx33)*a3*a3;
        double p31 = (-2.0*zy3a3+zxy[0]+zxy33)*a3*a3;
        double p32 = (-3.0*e-c-d)*b3*a3*a3;
        double p33 = (d+e+e)*a3*a3*b3*b3;
        //
        // computation of the polynomial
        //
        for( int jy=1; jy<=jymx; ++jy )
        {
          int kv = kv0 + jy;
          double dy = v[kv-1] - y3;
          double q0 = z33 + dy*(zy33+dy*(p02+dy*p03));
          double q1 = zx33 + dy*(zxy33+dy*(p12+dy*p13));
          double q2 = p20 + dy*(p21+dy*(p22+dy*p23));
          double q3 = p30 + dy*(p31+dy*(p32+dy*p33));
          for( int jx=1; jx<=jxmx; ++jx )
          {
            int ku = ku0 + jx;
            double dx = u[ku-1] - x3;
            w[kv-1+(ku-1)*nv] = q0 + dx*(q1+dx*(q2+dx*q3));
          }
        }
        ku0 += mx;
      }
    }
    kv0 += my;
  }
}

} // end of namespace

// end of file

