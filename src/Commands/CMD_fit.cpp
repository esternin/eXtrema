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

#include "CMD_fit.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"
#include "EExpressionError.h"
#include "Expression.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "TextVariable.h"

CMD_fit *CMD_fit::cmd_fit_ = 0;

CMD_fit::CMD_fit() : Command( wxT("FIT") )
{
  AddQualifier( wxT("WEIGHTS"),      false );
  AddQualifier( wxT("ZEROS"),        true  );
  AddQualifier( wxT("TOLERANCE"),    false );
  AddQualifier( wxT("POISSON"),      false );
  AddQualifier( wxT("MARQUARDT"),    false );
  AddQualifier( wxT("RESET"),        false );
  AddQualifier( wxT("ITMAX"),        false );
  AddQualifier( wxT("CORRELATIONS"), false );
  AddQualifier( wxT("COVARIANCE"),   false );
  AddQualifier( wxT("E1"),           false );
  AddQualifier( wxT("E2"),           false );
  AddQualifier( wxT("CHISQ"),        false );
  AddQualifier( wxT("CL"),           false );
  AddQualifier( wxT("VARNAMES"),     false );
  AddQualifier( wxT("UPDATE"),       false );
  AddQualifier( wxT("MESSAGES"),     true  );
  AddQualifier( wxT("FREE"),         false );
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
  SetUp( p, qualifiers );
  wxString command( Name()+wxT(": ") );
  bool output = qualifiers[wxT("MESSAGES")] || ExGlobals::NotInaScript() || ExGlobals::GetEcho();
  if( !qualifiers[wxT("MESSAGES")] )output = false;
  //
  if( qualifiers[wxT("UPDATE")] )
  {
    if( rhs_.length() == 0 )throw ECommandError( command+wxT("no previous fit") );
    //
    if( !p->IsString(1) )throw ECommandError( command+wxT("no output variable") );
    //
    wxString lhs( p->GetString(1) );
    AddToStackLine( lhs+wxString(wxT(" ! ( = "))+rhs_+wxString(wxT(" )")) );
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
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( expr.IsCharacter() )throw ECommandError( command+wxT("expecting numeric expression") );
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
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    return;
  }
  int icnt = 1;
  std::vector<double> weightVec;
  if( qualifiers[wxT("WEIGHTS")] )
  {
    try
    {
      NumericVariable::GetVector( p->GetString(icnt), wxT("weight vector"), weightVec );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(icnt++) );
  }
  else
  {
    qualifiers[wxT("ZEROS")] = false;
  }
  int ntmax;
  if( qualifiers[wxT("ITMAX")] )
  {
    double dntmax;
    try
    {
      NumericVariable::GetScalar( p->GetString(icnt), wxT("iteration maximum"), dntmax );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    ntmax = static_cast<int>(dntmax);
    if( ntmax < 0 )
    {
      throw ECommandError( command+wxT("maximum iteration number < 0") );
    }
    else if( ntmax == 0 )
    {
      throw ECommandError( command+wxT("maximum iteration number = 0") );
    }
    AddToStackLine( p->GetString(icnt++) );
  }
  else
  {
    ntmax = 51;
  }
  double tolerance = 0.00001;
  if( qualifiers[wxT("TOLERANCE")] )
  {
    try
    {
      NumericVariable::GetScalar( p->GetString(icnt), wxT("iteration tolerance"), tolerance );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( tolerance < 0.0 )
    {
      throw ECommandError( command+wxT("tolerance < 0") );
    }
    else if( tolerance == 0.0 )
    {
      throw ECommandError( command+wxT("tolerance = 0") );
    }
    AddToStackLine( p->GetString(icnt++) );
  }
  //
  // extract the vector from the left hand side of the wxT("=")
  //
  wxString lhs( p->GetString(icnt) );
  std::vector<double> yVec;
  try
  {
    NumericVariable::GetVector( lhs, wxT("left hand side of equation"), yVec );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( lhs );
  //
  if( qualifiers[wxT("WEIGHTS")] )
  {
    if( weightVec.size() != yVec.size() )
      throw ECommandError( command+wxT("weight vector has different length than dependent variable") );
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
    rhsSave_.push_back( wxString(wxT(" ")) );
    ilf = lhsSave_.size()-1;
  }
  //
  // extract the expression from the right of the "=".  if no expression
  // has been entered, restore the previous assignment of this variable
  // and use this expression.
  //
  AddToStackLine( wxT("=") );
  icnt += 2;
  if( p->GetNumberOfTokens()-1 < icnt )
  {
    if( lhsSave_[ilf].length() == 0 )
    {
      throw ECommandError( command+wxT("no previous expression has been entered for this variable") );
    }
    rhs_ = rhsSave_[ilf];
  }
  else if( p->IsNumeric(icnt) )
  {
    throw ECommandError( command+wxT("attempt to fit a constant") );
  }
  else if( p->IsString(icnt) )
  {
    rhs_ = p->GetString(icnt);
    rhsSave_[ilf] = p->GetString(icnt);
    AddToStackLine( rhsSave_[ilf] );
  }
  else if( p->IsEqualSign(icnt) )
  {
    throw ECommandError( command+wxT("equal sign encountered, expecting right hand side of equation") );
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
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( expr.IsCharacter() )throw ECommandError( command+wxT("expecting a numeric expression") );
  //
  std::size_t const nparam = expr.GetNumberOfFitParameters();
  if( nparam == 0 )
    throw ECommandError( command+wxT("no fitting parameters in the expression") );
  if( yVec.size() < nparam )
    throw ECommandError( command+wxT("number of data points < number of parameters") );
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
  unsigned int  nfree=0;
  try
  {
    UsefulFunctions::LeastSquaresFit( &expr, lhs, yVec, weightVec, e1, e2, pp, pSave,
                                      ntmax, tolerance, chisq, confidenceLevel,
                                      errmat, output, qualifiers[wxT("ZEROS")],
                                      qualifiers[wxT("POISSON")], qualifiers[wxT("MARQUARDT")], nfree );
  }
  catch ( EExpressionError &e )
  {
    if( qualifiers[wxT("RESET")] ) // reset original parameter values
    {
      for( std::size_t i=0; i<nparam; ++i )
        expr.SetFitParameterValue( i, p1[i] );
    }
    else        // update the parameters after an unsuccessful fit
    {
      for( std::size_t i=0; i<nparam; ++i )
        expr.SetFitParameterValue( i, pSave[i], GetStackLine() );
    }
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  //
  // make sure iparam is used before new variables are created
  // since making new variables changes the variable indices
  //
  // update the parameters and definition table after a successful fit
  //
  // also update variable history
  //
  wxString name;
  for( std::size_t i=0; i<nparam; ++i )expr.SetFitParameterValue( i, pp[i], GetStackLine() );
  if( qualifiers[wxT("VARNAMES")] )
  {
    try
    {
      name = wxT("FIT$VAR");
      TextVariable::PutVariable( name, expr.GetFitParameterNames(), GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )ExGlobals::WriteOutput( wxT("creating new array text variable: FIT$VAR") );
  }
  //
  // write out the table of new parameter values and chi-square
  // after a successful fit
  //
  if( output )
  {
    ExGlobals::WriteOutput(wxT(" "));
    ExGlobals::WriteOutput(wxT("   parameter            value               e1             e2"));
    for( std::size_t i=0; i<nparam; ++i )
    {
      wxChar c[100];
      ::wxSnprintf( c, 100, wxT("%12s    %15.5lg      %15.5lg%15.5lg"),
                    expr.GetFitParameterName(i).c_str(), pp[i], e1[i], e2[i] );
      ExGlobals::WriteOutput( c );
    }
  }
  //
  // output correlation matrix if corrmat modifier specified
  //
  if( qualifiers[wxT("CORRELATIONS")] )
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
      name = wxT("FIT$CORR");
      NumericVariable::PutVariable( name, corr, nparam, nparam, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )ExGlobals::WriteOutput( wxT("creating new correlation matrix: FIT$CORR") );
  }
  //
  // output covariance matrix if covmat modifier specified
  //
  if( qualifiers[wxT("COVARIANCE")] )
  {
    std::vector<double> cov( nparam*nparam, 0.0 );
    for( std::size_t i=0; i<nparam; ++i )
    {
      for( std::size_t j=0; j<nparam; ++j )cov[j+i*nparam] = errmat[j][i];
    }
    try
    {
      name = wxT("FIT$COVM");
      NumericVariable::PutVariable( name, cov, nparam, nparam, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )ExGlobals::WriteOutput( wxT("creating new covariance matrix: FIT$COVM") );
  }
  //
  // output e1 error vector if e1 modifier specified
  //
  if( qualifiers[wxT("E1")] )
  {
    try
    {
      name = wxT("FIT$E1");
      NumericVariable::PutVariable( name, e1, 0, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )ExGlobals::WriteOutput( wxT("creating new error vector: FIT$E1") );
  }
  //
  // output e2 error vector if e2 modifier specified
  //
  if( qualifiers[wxT("E2")] )
  {
    try
    {
      name = wxT("FIT$E2");
      NumericVariable::PutVariable( name, e2, 0, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )ExGlobals::WriteOutput( wxT("creating new error vector: FIT$E2") );
  }
  //
  // output chisq if chisq modifier specified
  //
  if( qualifiers[wxT("CHISQ")] )
  {
    try
    {
      name = wxT("FIT$CHISQ");
      NumericVariable::PutVariable( name, chisq, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )ExGlobals::WriteOutput( wxT("creating new chisq scalar: FIT$CHISQ") );
  }
  //
  // output confidenceLevel if CONFIDENCELEVEL qualifier specified
  //
  if( qualifiers[wxT("CL")] )
  {
    try
    {
      name = wxT("FIT$CL");
      NumericVariable::PutVariable( name, confidenceLevel, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )ExGlobals::WriteOutput( wxT("creating new confidence level scalar: FIT$CL") );
  }
  //
  // output degrees of freedom if free modifier specified
  //
  if( qualifiers[wxT("FREE")] )
  {
    try
    {
      name = wxT("FIT$FREE");
      NumericVariable::PutVariable( name, static_cast<double>(nfree), GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )ExGlobals::WriteOutput( wxT("creating new degrees of freedom scalar: FIT$FREE") );
  }
}

// end of file
