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

#include "CMD_bestfit.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "EExpressionError.h"
#include "UsefulFunctions.h"
#include "NumericVariable.h"
#include "ExGlobals.h"

CMD_bestfit *CMD_bestfit::cmd_bestfit_ = 0;

CMD_bestfit::CMD_bestfit() : Command( wxT("BESTFIT") )
{
  AddQualifier( wxT("WEIGHTS"), false );
  AddQualifier( wxT("CYCLES"), false );
}

void CMD_bestfit::Execute( ParseLine const *p )
{
  // BESTFIT                    pmin pmax penalty error parm pout
  // BESTFIT\CYCLES           n pmin pmax penalty error parm pout
  // BESTFIT\WEIGHTS        w   pmin pmax penalty error parm pout
  // BESTFIT\CYCLES\WEIGHTS w n pmin pmax penalty error parm pout
  //
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
  wxString command( Name()+wxT(": ") );
  int counter = 1;
  std::vector<double> weights;
  int nwt;
  if( qualifiers[wxT("WEIGHTS")] )
  {
    //
    // BESTFIT\WEIGHTS        w   pmin pmax penalty error parm pout
    // BESTFIT\CYCLES\WEIGHTS w n pmin pmax penalty error parm pout
    //
    if( p->GetNumberOfTokens() < counter+1 )
      throw ECommandError( command+wxT("expecting weight vector") );
    try
    {
      NumericVariable::GetVector( p->GetString(counter), wxT("weight vector"), weights );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    nwt = weights.size();
    AddToStackLine( p->GetString(counter++) );
  }
  int ncyc = 10;
  if( qualifiers[wxT("CYCLES")] )
  {
    // BESTFIT\CYCLES           n pmin pmax penalty error parm pout
    // BESTFIT\CYCLES\WEIGHTS w n pmin pmax penalty error parm pout
    //
    double v;
    try
    {
      NumericVariable::GetScalar( p->GetString(counter), wxT("number of cycles"), v );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    ncyc = static_cast<int>(v);
    if( ncyc <= 0 )
      throw ECommandError( command+wxT("number of cycles must be > 0") );
    AddToStackLine( p->GetString(counter++) );
  }
  std::vector<double> pmin;
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("parameter minima"), pmin );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  int npmin = pmin.size();
  AddToStackLine( p->GetString(counter++) );
  std::vector<double> pmax;
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("parameter maxima"), pmax );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  int npmax = pmax.size();
  AddToStackLine( p->GetString(counter++) );
  std::vector<double> penalties;
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("parameter penalties"), penalties );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  int npenalties = penalties.size();
  AddToStackLine( p->GetString(counter++) );
  std::vector<double> errors;
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("error vector"), errors );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  int nerror = errors.size();
  AddToStackLine( p->GetString(counter++) );
  if( !qualifiers[wxT("WEIGHTS")] )
  {
    nwt = nerror;
    weights.assign( nwt, 1.0 );
  }
  if( nwt != nerror )throw ECommandError( command+wxT("weight vector length must equal the error vector length") );
  std::vector<double> pcm;
  std::size_t nr, np;
  try
  {
    NumericVariable::GetMatrix( p->GetString(counter), wxT("parameter characteristic matrix"),
                                pcm, nr, np );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(counter++) );
  if( nerror != static_cast<int>(nr) )throw ECommandError(
    command+wxT("error vector length must equal the number of rows of the matrix") );
  if( npmin != static_cast<int>(np) )throw ECommandError(
    command+wxT("parameter minimum vector length must equal the number of columns of the matrix") );
  if( npmax != static_cast<int>(np) )throw ECommandError(
    command+wxT("parameter maximum vector length must equal the number of columns of the matrix") );
  if( npenalties != static_cast<int>(np) )throw ECommandError(
    command+wxT("parameter penalty vector length must equal the number of columns of the matrix") );
  if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
      throw ECommandError( command+wxT("expecting output parameter vector name") );
  //
  std::vector<double> pout(np);
  FitData( static_cast<int>(np), static_cast<int>(nr), ncyc, pmin, pmax, penalties,
           errors, weights, pcm, pout );
  wxString poutName( p->GetString(counter) );
  try
  {
    NumericVariable::PutVariable( poutName, pout, 0, p->GetInputLine() );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( poutName );
}

void CMD_bestfit::FitData( int np, int nr, int ncyc,
                           std::vector<double> const &pmin,
                           std::vector<double> const &pmax,
                           std::vector<double> const &penalties,
                           std::vector<double> const &errors,
                           std::vector<double> const &weights,
                           std::vector<double> const &pcm,
                           std::vector<double> &p )
{
  // original fortran code by C.J. Kost, 29 July, 1980
  // See TRIUMF design note TRI-DN-69-31
  //
  // Calculate parameters for least-squares fit to an error vector using np
  // adjustable parameters
  //
  // Given an error vector "errors" (error array at nr locations).
  // Given the influence function (that is the effect at the nr locations of a
  // unit change) of np (<nr) variables (parameters).
  // Given a weight array length nr corresponding to the "importance" of reducing
  // the initial error vector to zero at these locations.
  // Given an array "penalties" of length "nr" which represent penalty funtions to
  // changes of the "nr" parameters (that is the larger penalties[i] the smaller
  // will be the the adjustment of this parameter).
  // The routine determines the optimal set of changes of the "np" parameters
  // within their allowed range "pmin" to "pmax" in the least squares sense.
  //
  //  np         :number of parameters
  //  nr         :number of locations to be fitted (length of errors array)
  //  ncyc       :limit to number of cycles to meet constraints
  //  pmin       :array of np lower parameter limits
  //  pmax       :array of np upper parameter limits
  //  penalties  :array of penalty weights for the np parameters
  //  errors     :array of nr errors to be least-square fitted
  //  weights    :array of nr weights for fit to errors array
  //  pcm        :array(nr,np) giving parameter characteristics/unit change
  //  p          :output array of np parameter changes giving best fit
  //
  // Fill the arrays a and ip
  //
  std::vector<double> a(nr*np,0.0);
  std::vector<int> ip(np,0);
  int nc = np;
  for( int n=1; n<=nc; ++n )
  {
    ip[n-1] = n;
    for( int m=n; m<=nc; ++m )
    {
      int nm = n+(m-1)*nc-1;
      int mn = m+(n-1)*nc-1;
      a[nm] = 0.0;
      for( int ir=0; ir<nr; ++ir )a[nm] += pcm[ir+(n-1)*nr]*pcm[ir+(m-1)*nr]*weights[ir];
      a[mn] = a[nm];
    }
    a[n+(n-1)*nc-1] += penalties[n-1];
  }
  //
  // Form weighted error vector
  //
  std::vector<double> weightedErrors(nr,0.0);
  for( int ir=0; ir<nr; ++ir )weightedErrors[ir] = -weights[ir]*errors[ir];
  int cyclesUsed = 1;
  //
  // Recycle point for constraints
  // Form vector d, saving a version as dt
  //
  for( ;; )
  {
    std::vector<double> d(nc);
    std::vector<double> dt(nc);
    std::vector<double> ainv(nc*nc);
    for( int n=0; n<nc; ++n )
    {
      p[ip[n]-1] = 0.0;
      d[n] = 0.0;
      for( int ir=0; ir<nr; ++ir )d[n] += weightedErrors[ir]*pcm[ir+(ip[n]-1)*nr];
      dt[n] = d[n];
      for( int m=0; m<nc; ++m )ainv[m+n*nc] = a[m+n*nc];
    }
    if( !Simq(ainv,dt) )
      throw ECommandError( wxT("BESTFIT: the matrix is singular") );
    for( int n=0; n<nc; ++n )p[ip[n]-1] = dt[n];
    //
    // Constraints section follows
    //
    if( ncyc <= 0 )
    {
      FitDataInfo( cyclesUsed, np, nr, p, weights, errors, pcm );
      return;
    }
    //
    // Find largest violation of maximum constraints
    //
    int kill = 0;
    double cm = 0.0;
    int next = nc-1;
    for( int n=1; n<=nc; ++n )
    {
      double cd = p[ip[n-1]-1] - pmax[ip[n-1]-1];
      if( cd > cm )
      {
        cm = cd;
        kill = n;
      }
    }
    if( kill == 0 ) // no violation of maximum constraints
    {
      // determine largest violation of minimum constraints
      //
      for( int n=1; n<=nc; ++n )
      {
        double cd = pmin[ip[n-1]-1] - p[ip[n-1]-1];
        if( cd > cm )
        {
          cm = cd;
          kill = n;
        }
      }
      if( kill == 0 )
      {
        FitDataInfo( cyclesUsed, np, nr, p, weights, errors, pcm );
        return;
      }
      //
      // Parameter ikill is largest violator of minimum constraint
      //
      int ikill = ip[kill-1];
      if( cyclesUsed > ncyc )
      {
        ExGlobals::WriteOutput( wxString(wxT("limit on cycles reached, with parameter "))
                                << ip[kill]+1 << wxT(" still outside") );
        FitDataInfo( cyclesUsed, np, nr, p, weights, errors, pcm );
        return;
      }
      p[ikill-1] = pmin[ikill-1];
      ExGlobals::WriteOutput( wxString(wxT("parameter ")) << ikill
                              << wxT(" < min ") << pmin[ikill-1] << wxT(" so set to min") );
      //
      // Modify  contribution to vector D due to parameter ip[kill]
      //
      for( int ir=0; ir<nr; ++ir )
        weightedErrors[ir] -= p[ikill-1]*pcm[ir+(ikill-1)*np]*weights[ir];
    }
    else
    {
      // Parameter ikill is largest violator of maximum constraint
      //
      int ikill = ip[kill-1];
      if( cyclesUsed > ncyc )
      {
        ExGlobals::WriteOutput( wxString(wxT("limit on cycles reached, with parameter "))
                                << ip[kill]+1 << wxT(" still outside") );
        FitDataInfo( cyclesUsed, np, nr, p, weights, errors, pcm );
        return;
      }
      p[ikill-1] = pmax[ikill-1];
      ExGlobals::WriteOutput( wxString(wxT("parameter ")) << ikill-1 << wxT(" > max ")
                              << pmax[ikill-1] << wxT(" so set to max") );
      //
      // Modify  contribution to vector D due to parameter ip[kill]
      // Check if next < 0 since we return if have null matrix
      //
      if( next == 0 )return;
      for( int ir=0; ir<nr; ++ir )
        weightedErrors[ir] -= p[ikill-1]*pcm[ir+(ikill-1)*np]*weights[ir];
    }
    // Relabel index ip[n]
    // Now eliminate row and column ip[kill] from matrix a
    //
    for( int n=kill; n<=next; ++n )
    {
      ip[n-1] = ip[n];
      for( int m=0; m<nc; ++m )a[n+m*next-1] = a[n+m*next];
    }
    for( int n=kill; n<=next; ++n )
    {
      for( int m=0; m<next; ++m )a[m+(n-1)*next] = a[m+n*next];
    }
    nc = next;
    ++cyclesUsed;
  } // return to least square fitting with p[ip[kill]] fixed at extreme
}

void CMD_bestfit::FitDataInfo( int cyclesUsed, int np, int nr,
                               std::vector<double> &p,
                               std::vector<double> const &weights,
                               std::vector<double> const &errors,
                               std::vector<double> const &pcm )
{
  //
  // Write out RMS param changes, and residuals before and after fit
  //
  ExGlobals::WriteOutput( wxString(wxT("number of cycles used: ")) << cyclesUsed );
  double sumpar = 0.0;
  for( int i=0; i<np; ++i )
  {
    sumpar += p[i]*p[i];
    ExGlobals::WriteOutput( wxString(wxT("variable ")) << i+1 << wxT(" change = ") << p[i] );
  }
  double resi = 0.0;
  double resf = 0.0;
  for( int j=0; j<nr; ++j )
  {
    resi += weights[j]*errors[j]*weights[j]*errors[j];
    double errf = errors[j];
    for( int i=0; i<np; ++i )errf += pcm[j+i*nr]*p[i];
    resf += weights[j]*errf*weights[j]*errf;
  }
  double sparn = sqrt(sumpar/np);
  ExGlobals::WriteOutput( wxString(wxT("rms parameter change = ")) << sparn );
  ExGlobals::WriteOutput( wxString(wxT("    initial residual = ")) << resi );
  ExGlobals::WriteOutput( wxString(wxT("      final residual = ")) << resf );
}

bool CMD_bestfit::Simq( std::vector<double> &a, std::vector<double> &b )
{
  // Original fortran by C.J. Kost, 29 July, 1980
  //
  //  Purpose: solution of simultaneous linear equations by
  //           Gaussian elimination using largest pivital divisors
  //
  //  description of parameters
  //  a - matrix of coefficients stored columnwise
  //      these are destroyed in the computation
  //      the size of matrix a is n by n
  //  b - vector of original constrants (length n)
  //      these are replaced by final solution values, vector x
  //  return true  for a normal solution
  //  return false for a singular set of equations
  //
  // Forward solution
  //
  double tol = 0.0; // set initial tolerance to zero
  int n = b.size();
  //
  // the following for statement defines one stage of the forward
  // solution to be executed n times
  //
  int jj = -n;
  for( int j=1; j<=n; ++j )
  {
    int jy = j+1;
    jj += n+1;
    double biga = 0.0;
    int it = jj-j;
    int imax;
    for( int i=j; i<=n; ++i )
    {
      if( fabs(biga) < fabs(a[it+i-1]) ) // search for maximum coefficient in column
      {
        biga = a[it+i-1];
        imax = i;
      }
    }
    // test for singular matrix
    // if no pivot exceeds the value tol, the matrix is considered
    // singular and return is made to the calling program with ks=1
    //
    if( fabs(biga) <= tol )return false;
    //
    // interchange rows if necessary
    //
    // the following for statement defines the operation which
    // divides the equation by the leading coefficient
    //
    int i1 = j+n*(j-2);
    it = imax-j;
    for( int k=j; k<=n; ++k )
    {
      i1 += n;
      int i2 = i1+it;
      double save = a[i1-1];
      a[i1-1] = a[i2-1];
      a[i2-1] = save;
      a[i1-1] /= biga; // divide equation by leading coefficient
    }
    double save = b[imax-1];
    b[imax-1] = b[j-1];
    b[j-1] = save/biga;
    //
    // eliminate next variable
    // successive variables are eliminated until n_th equation
    // takes the form x = const
    //
    if( j == n )break;
    int iqs = n*(j-1);
    for( int ix=jy; ix<=n; ++ix )
    {
      int ixj = iqs+ix;
      it = j-ix;
      for( int jx=jy; jx<=n; ++jx )
      {
        int ixjx = n*(jx-1)+ix;
        int jjx = ixjx+it;
        a[ixjx-1] -= a[ixj-1]*a[jjx-1];
      }
      b[ix-1] -= b[j-1]*a[ixj-1];
    }
  }
  //
  // back solution
  //
  // the following for statement defines the back-substitution procedure
  //
  int ny = n-1;
  int it = n*n;
  for( int j=1; j<=ny; ++j )
  {
    int ia = it-j;
    int ib = n-j;
    int ic = n;
    for( int k=0; k<j; ++k )
    {
      b[ib-1] -= a[ia-1]*b[ic-1];
      ia -= n;
      --ic;
    }
  }
  return true;
}

// end of file
