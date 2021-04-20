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

#include "ExGlobals.h"
#include "UsefulFunctions.h"
#include "Expression.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "EExpressionError.h"
#include "ECommandError.h"
#include "EVariableError.h"

#include "CMD_fit.h"

CMD_fit CMD_fit::cmd_fit_;

CMD_fit::CMD_fit() : Command( "FIT" )
{
  AddQualifier( "WEIGHTS",      false );
  AddQualifier( "ZEROS",        true  );
  AddQualifier( "TOLERANCE",    false );
  AddQualifier( "POISSON",      false );
  AddQualifier( "MARQUARDT",    false );
  AddQualifier( "RESET",        false );
  AddQualifier( "ITMAX",        false );
  AddQualifier( "CORRMAT",      false );
  AddQualifier( "COVMAT",       false );
  AddQualifier( "E1",           false );
  AddQualifier( "E2",           false );
  AddQualifier( "CHISQ",        false );
  AddQualifier( "CL",           false );
  AddQualifier( "VARNAMES",     false );
  AddQualifier( "UPDATE",       false );
  AddQualifier( "MESSAGES",     true  );
  AddQualifier( "FREE",         false );
}

void CMD_fit::Execute( ParseLine const *p )
{
  // fit          y = expression
  // fit/weight w y = expression
  // fit/itmax  n y = expression
  // fit/weight/itmax w n y = expression
  // fit/poisson y = expression
  // fit/poisson/itmax n y = expression
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
  //
  if( qualifiers["UPDATE"] )
  {
    if( rhs_.length() == 0 )throw ECommandError( "FIT\\UPDATE", "no previous fit" );
    //
    if( !p->IsString(1) )throw ECommandError( "FIT\\UPDATE", "no output variable" );
    //
    ExString lhs( p->GetString(1) );
    AddToStackLine( lhs+ExString(" ! ( = ")+rhs_+ExString(" )") );
    //
    // re-evaluate the last expression
    //
    Expression expr( rhs_ );
    try
    {
      expr.Evaluate();
    }
    catch ( EExpressionError &e )
    {
      throw ECommandError( "FIT\\UPDATE", e.what() );
    }
    if( expr.IsCharacter() )throw ECommandError( "FIT\\UPDATE", "expecting numeric expression" );
    //
    NumericData nd( expr.GetFinalAnswer() );
    try
    {
      switch ( nd.GetNumberOfDimensions() )
      {
        case 0:
          NumericVariable::PutVariable( lhs, nd.GetScalarValue(), p->GetInputLine() );
          break;
        case 1:
          NumericVariable::PutVariable( lhs, nd.GetData(), 0, p->GetInputLine() );
          break;
        case 2:
          NumericVariable::PutVariable( lhs, nd.GetData(), nd.GetDimMag(0), nd.GetDimMag(1),
                                        p->GetInputLine() );
      }
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT\\UPDATE", e.what() );
    }
    return;
  }
  int icnt = 1;
  std::vector<double> weightVec;
  if( qualifiers["WEIGHTS"] )
  {
    try
    {
      NumericVariable::GetVector( p->GetString(icnt), "weight vector", weightVec );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    AddToStackLine( p->GetString(icnt++) );
  }
  else
  {
    qualifiers["ZEROS"] = false;
  }
  int ntmax;
  if( qualifiers["ITMAX"] )
  {
    double dntmax;
    try
    {
      NumericVariable::GetScalar( p->GetString(icnt), "iteration maximum", dntmax );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    ntmax = static_cast<int>(dntmax);
    if( ntmax < 0 )
    {
      throw ECommandError( "FIT", "maximum iteration number < 0" );
    }
    else if( ntmax == 0 )
    {
      throw ECommandError( "FIT", "maximum iteration number = 0" );
    }
    AddToStackLine( p->GetString(icnt++) );
  }
  else
  {
    ntmax = 51;
  }
  double tolerance = 0.00001;
  if( qualifiers["TOLERANCE"] )
  {
    try
    {
      NumericVariable::GetScalar( p->GetString(icnt), "iteration tolerance", tolerance );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    if( tolerance < 0.0 )
    {
      throw ECommandError( "FIT", "tolerance < 0" );
    }
    else if( tolerance == 0.0 )
    {
      throw ECommandError( "FIT", "tolerance = 0" );
    }
    AddToStackLine( p->GetString(icnt++) );
  }
  //
  // extract the vector from the left hand side of the "="
  //
  ExString lhs( p->GetString(icnt) );
  std::vector<double> yVec;
  try
  {
    NumericVariable::GetVector( lhs, "left hand side of equation", yVec );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "FIT", e.what() );
  }
  AddToStackLine( lhs );
  //
  if( qualifiers["WEIGHTS"] )
  {
    if( weightVec.size() != yVec.size() )
      throw ECommandError( "FIT", "weight vector has different length than dependent variable" );
  }
  else
  {
    weightVec.assign( yVec.size(), 1.0 );
  }
  int ilf = -1;
  std::size_t const lhsEnd = lhsSave_.size();
  for( std::size_t i=0; i<lhsEnd; ++i )
  {
    if( lhs == lhsSave_[i] )
    {
      ilf = i;
      break;
    }
  }
  if( ilf == -1 )
  {
    lhsSave_.push_back( lhs );
    rhsSave_.push_back( ExString(" ") );
    ilf = lhsSave_.size()-1;
  }
  //
  // extract the expression from the right of the "=".  if no expression
  // has been entered, restore the previous assignment of this variable
  // and use this expression.
  //
  AddToStackLine( "=" );
  icnt += 2;
  if( p->GetNumberOfTokens()-1 < icnt )
  {
    if( lhsSave_[ilf].length() == 0 )
    {
      throw ECommandError( "FIT",
                           "no previous expression has been entered for this variable" );
    }
    rhs_ = rhsSave_[ilf];
  }
  else if( p->IsNumeric(icnt) )
  {
    throw ECommandError( "FIT", "attempt to fit a constant" );
  }
  else if( p->IsString(icnt) )
  {
    rhs_ = p->GetString(icnt);
    rhsSave_[ilf] = p->GetString(icnt);
    AddToStackLine( rhsSave_[ilf] );
  }
  else if( p->IsEqualSign(icnt) )
  {
    throw ECommandError( "FIT", "equal sign encountered, expecting right hand side of equation" );
  }
  //
  // decode the expression
  //
  Expression expr( rhs_ );
  expr.SetIsaFit();
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    throw ECommandError( "FIT", e.what() );
  }
  if( expr.IsCharacter() )throw ECommandError( "FIT", "expecting a numeric expression" );
  //
  std::size_t const nparam = expr.GetNumberOfFitParameters();
  if( nparam == 0 )
    throw ECommandError( "FIT", "no fitting parameters in the expression" );
  if( yVec.size() < nparam )
    throw ECommandError( "FIT", "number of data points < number of parameters" );
  //
  // do the least squares fit
  //
  std::vector<double> pp( nparam, 0.0 );
  for( std::size_t i=0; i<nparam; ++i )
    pp[i] = expr.GetFitParameterValue( i );  // initial parameter values
  std::vector<double> p1( pp );                   // save original parameter values
  std::vector<double> pSave( pp );
  std::vector<double> e1( nparam ), e2( nparam );
  std::vector< std::vector<double> > errmat( nparam );
  for( std::size_t i=0; i<nparam; ++i )errmat[i].resize( nparam );
  double confidenceLevel=0.0, chisq=0.0;
  std::size_t nfree=0;
  try
  {
    UsefulFunctions::LeastSquaresFit( &expr, lhs, yVec, weightVec, e1, e2, pp, pSave,
                                      ntmax, tolerance, chisq, confidenceLevel,
                                      errmat, output, qualifiers["ZEROS"],
                                      qualifiers["POISSON"], qualifiers["MARQUARDT"], nfree );
  }
  catch ( EExpressionError &e )
  {
    if( qualifiers["RESET"] ) // reset original parameter values
    {
      for( std::size_t i=0; i<nparam; ++i )expr.SetFitParameterValue( i, p1[i] );
    }
    else        // update the parameters after an unsuccessful fit
    {
      for( std::size_t i=0; i<nparam; ++i )expr.SetFitParameterValue( i, pSave[i], GetStackLine() );
    }
    throw ECommandError( "FIT", e.what() );
  }
  //
  // make sure iparam is used before new variables are created
  // since making new variables changes the variable indices
  //
  // update the parameters and definition table after a successful fit
  //
  // also update variable history
  for( std::size_t i=0; i<nparam; ++i )expr.SetFitParameterValue( i, pp[i], GetStackLine() );
  if( qualifiers["VARNAMES"] )
  {
    try
    {
      TextVariable::PutVariable( "FIT$VAR", expr.GetFitParameterNames(), GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    if( output )ExGlobals::WriteOutput( "creating new array text variable: FIT$VAR" );
  }
  //
  // write out the table of new parameter values and chi-square
  // after a successful fit
  //
  if( output )
  {
    ExGlobals::WriteOutput(" ");
    ExGlobals::WriteOutput("   parameter            value               e1             e2");
    for( std::size_t i=0; i<nparam; ++i )
    {
      char c[100];
      sprintf( c, "%12s    %15.5lg      %15.5lg%15.5lg",
               expr.GetFitParameterName(i).c_str(), pp[i], e1[i], e2[i] );
      ExGlobals::WriteOutput( c );
    }
  }
  //
  // output correlation matrix if corrmat modifier specified
  //
  if( qualifiers["CORRMAT"] )
  {
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
    try
    {
      NumericVariable::PutVariable( "FIT$CORR", corr, nparam, nparam, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    if( output )ExGlobals::WriteOutput( "creating new correlation matrix: FIT$CORR" );
  }
  //
  // output covariance matrix if covmat modifier specified
  //
  if( qualifiers["COVMAT"] )
  {
    std::vector<double> cov( nparam*nparam, 0.0 );
    for( std::size_t i=0; i<nparam; ++i )
    {
      for( std::size_t j=0; j<nparam; ++j )cov[j+i*nparam] = errmat[j][i];
    }
    try
    {
      NumericVariable::PutVariable( "FIT$COVM", cov, nparam, nparam, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    if( output )ExGlobals::WriteOutput( "creating new covariance matrix: FIT$COVM" );
  }
  //
  // output e1 error vector if e1 modifier specified
  //
  if( qualifiers["E1"] )
  {
    try
    {
      NumericVariable::PutVariable( "FIT$E1", e1, 0, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    if( output )ExGlobals::WriteOutput( "creating new error vector: FIT$E1" );
  }
  //
  // output e2 error vector if e2 modifier specified
  //
  if( qualifiers["E2"] )
  {
    try
    {
      NumericVariable::PutVariable( "FIT$E2", e2, 0, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    if( output )ExGlobals::WriteOutput( "creating new error vector: FIT$E2" );
  }
  //
  // output chisq if chisq modifier specified
  //
  if( qualifiers["CHISQ"] )
  {
    try
    {
      NumericVariable::PutVariable( "FIT$CHISQ", chisq, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    if( output )ExGlobals::WriteOutput( "creating new chisq scalar: FIT$CHISQ" );
  }
  //
  // output confidenceLevel if CONFIDENCELEVEL qualifier specified
  //
  if( qualifiers["CL"] )
  {
    try
    {
      NumericVariable::PutVariable( "FIT$CL", confidenceLevel, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    if( output )ExGlobals::WriteOutput( "creating new confidence level scalar: FIT$CL" );
  }
  //
  // output degrees of freedom if free modifier specified
  //
  if( qualifiers["FREE"] )
  {
    try
    {
      NumericVariable::PutVariable( "FIT$FREE", static_cast<double>(nfree), GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FIT", e.what() );
    }
    if( output )ExGlobals::WriteOutput( "creating new degrees of freedom scalar: FIT$FREE" );
  }
}

// end of file
