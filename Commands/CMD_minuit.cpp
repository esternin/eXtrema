/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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

#include <cmath>
#include <sstream>

#ifdef HAVE_MINUIT
#include "Minuit/MnUserParameters.h"
#include "Minuit/MnApplication.h"
#include "Minuit/MnMigrad.h"
#include "Minuit/MnMinimize.h"
#include "Minuit/MnSimplex.h"
#include "Minuit/FunctionMinimum.h"
#include "Minuit/MnPrint.h"
#endif

#include "CMD_minuit.h"
#include "Extrema_fcn.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EExpressionError.h"
#include "Expression.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "NumericData.h"
#include "TextVariable.h"

CMD_minuit CMD_minuit::cmd_minuit_;

CMD_minuit::CMD_minuit() : Command("MINUIT")
{
  AddQualifier( "WEIGHTS",      false );
  AddQualifier( "MESSAGES",     true  );
  AddQualifier( "CHISQ",        false );
  AddQualifier( "VARNAMES",     false );
  AddQualifier( "UPDATE",       false );
  AddQualifier( "MINIMIZE",     true  );
  AddQualifier( "MIGRAD",       false );
  AddQualifier( "SIMPLEX",      false );
}

void CMD_minuit::Execute( ParseLine const *p )
{
#ifndef HAVE_MINUIT
  throw ECommandError(
    ExString("MINUIT:  the MINUIT minimization package is not available") );
#else
  ExString command( Name()+": " );
  //
  // minuit           y=expression
  // minuit           chisq_expression
  // minuit/weights w y=expression
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
    if( rhs_.length() == 0 )throw ECommandError( command, "no previous fit" );
    //
    if( !p->IsString(1) )throw ECommandError( command, "no output variable" );
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
      throw ECommandError( command, e.what() );
    }
    if( expr.IsCharacter() )throw ECommandError( command, "expecting numeric expression" );
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
      throw ECommandError( command, e.what() );
    }
    return;
  }
  int nTokens = p->GetNumberOfTokens();
  int icnt = 1;
  std::vector<double> weightVec;
  if( qualifiers["WEIGHTS"] )
  {
    if( icnt >= nTokens )throw ECommandError( command, "expecting weight vector" );
    try
    {
      NumericVariable::GetVector( p->GetString(icnt), "weight vector", weightVec );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
    AddToStackLine( p->GetString(icnt++) );
  }
  // extract the expression
  //
  ExString exprString;
  bool chisqDirect = false;
  std::vector<double> yVec;
  if( !p->IsEqualSign(icnt+1) ) // not y=expression, instead it is chisq_expression
  {
    chisqDirect = true;
    if( (icnt>=nTokens) || !p->IsString(icnt) )throw ECommandError( command, "expecting expression" );
    exprString = p->GetString(icnt);
  }
  else
  {
    if( (icnt+2>=nTokens) || !p->IsString(icnt+2) )throw ECommandError( command, "expecting expression" );
    exprString = p->GetString(icnt+2);
    rhs_ = p->GetString(icnt+2);
    //
    // extract the vector from the left hand side of the "="
    //
    ExString lhs( p->GetString(icnt) );
    try
    {
      NumericVariable::GetVector( lhs, "left hand side of equation", yVec );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
    AddToStackLine( lhs+"=" );
    //
    if( qualifiers["WEIGHTS"] )
    {
      if( weightVec.size() != yVec.size() )
        throw ECommandError( command, "weight vector has different length than dependent variable" );
    }
    else
    {
      weightVec.assign( yVec.size(), 1.0 );
    }
  }
  AddToStackLine( exprString );
  //
  // decode the expression
  //
  Expression expr( exprString );
  expr.SetIsaFit();
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    throw ECommandError( command, e.what() );
  }
  if( expr.IsCharacter() )throw ECommandError( command, "expecting a numeric expression" );
  //
  std::size_t const nparam = expr.GetNumberOfFitParameters();
  if( nparam == 0 )
    throw ECommandError( command, "no fitting parameters in the expression" );
  if( !chisqDirect && (yVec.size()<nparam) )
    throw ECommandError( command, "number of data points < number of parameters" );
  Extrema_fcn fcn( yVec, weightVec );
  fcn.SetExpression( &expr );

  MnUserParameters upar;
  for( std::size_t i=0; i<nparam; ++i )
  {
    ExString name( expr.GetFitParameterName(i) );
    upar.add( name.c_str(), expr.GetFitParameterValue(i), 1.0 );
    NumericData &pvd( NVariableTable::GetTable()->GetVariable(name)->GetData() );
    if( pvd.LowLimitSet() )
    {
      if( pvd.HighLimitSet() )upar.setLimits( name.c_str(), pvd.GetLowLimit(), pvd.GetHighLimit() );
      else                    upar.setLowerLimit( name.c_str(), pvd.GetLowLimit() );
    }
    else
    {
      if( pvd.HighLimitSet() )upar.setUpperLimit( name.c_str(), pvd.GetHighLimit() );
    }
  }
  if( qualifiers["SIMPLEX"] )
  {
    MnSimplex simplex( fcn, upar );
    FunctionMinimum fmin = simplex();
    if( !fmin.isValid() )throw ECommandError( command, "Minuit simplex failed" );
    //
    // make sure this is done before new variables are created
    // since making new variables changes the variable indices
    //
    // update the parameters and definition table after a successful fit
    //
    // also update variable history
    //
    for( std::size_t i=0; i<nparam; ++i )
      expr.SetFitParameterValue( i, fmin.userState().value(i), GetStackLine() );
    //
    std::stringstream ss;
    ss << fmin << std::endl;
    std::string s;
    while( !std::getline(ss,s,'\n').fail() )
    {
      ExGlobals::WriteOutput( s );
      s.clear();
    }
  }
  else if( qualifiers["MIGRAD"] )
  {
    MnMigrad migrad( fcn, upar );
    FunctionMinimum fmin = migrad();
    if( !fmin.isValid() )throw ECommandError( command, "Minuit migrad failed" );
    for( std::size_t i=0; i<nparam; ++i )
      expr.SetFitParameterValue( i, fmin.userState().value(i), GetStackLine() );
    //
    std::stringstream ss;
    ss << fmin << std::endl;
    std::string s;
    while( !std::getline(ss,s,'\n').fail() )
    {
      ExGlobals::WriteOutput( s );
      s.clear();
    }
  }
  else
  {
    MnMinimize minimize( fcn, upar );
    FunctionMinimum fmin = minimize();
    if( !fmin.isValid() )throw ECommandError( command, "Minuit minimize failed" );
    for( std::size_t i=0; i<nparam; ++i )
      expr.SetFitParameterValue( i, fmin.userState().value(i), GetStackLine() );
    //
    std::stringstream ss;
    ss << fmin << std::endl;
    std::string s;
    while( !std::getline(ss,s,'\n').fail() )
    {
      ExGlobals::WriteOutput( s );
      s.clear();
    }
  }
  if( qualifiers["VARNAMES"] )
  {
    try
    {
      TextVariable::PutVariable( "FIT$VAR", expr.GetFitParameterNames(), GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
    if( output )ExGlobals::WriteOutput( "creating new array text variable: FIT$VAR" );
  }
  //
  // output chisq if chisq modifier specified
  //
  if( qualifiers["CHISQ"] )
  {
    try
    {
      expr.FinalPass();
    }
    catch ( EExpressionError &e )
    {
      throw;
    }
    NumericData result( expr.GetFinalAnswer() );
    std::vector<double> temp( result.GetData() );
    std::size_t N = yVec.size();
    double chi2 = 0.0;
    if( temp.size() == 1 )
    {
      for( std::size_t i=0; i<N; ++i )
        chi2 += weightVec[i]*(yVec[i]-temp[0])*(yVec[i]-temp[0]);
    }
    else
    {
      for( std::size_t i=0; i<N; ++i )
        chi2 += weightVec[i]*(yVec[i]-temp[i])*(yVec[i]-temp[i]);
    }
    try
    {
      NumericVariable::PutVariable( "FIT$CHISQ", chi2, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command, e.what() );
    }
    if( output )ExGlobals::WriteOutput( "creating new chisq scalar: FIT$CHISQ" );
  }
#endif
}

// end of file
