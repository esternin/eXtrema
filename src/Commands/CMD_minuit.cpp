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
#include "NVariableTable.h"

CMD_minuit *CMD_minuit::cmd_minuit_ = 0;

CMD_minuit::CMD_minuit() : Command( wxT("MINUIT") )
{
  AddQualifier( wxT("WEIGHTS"),      false );
  AddQualifier( wxT("MESSAGES"),     true  );
  AddQualifier( wxT("CHISQ"),        false );
  AddQualifier( wxT("VARNAMES"),     false );
  AddQualifier( wxT("UPDATE"),       false );
  AddQualifier( wxT("MINIMIZE"),     true  );
  AddQualifier( wxT("MIGRAD"),       false );
  AddQualifier( wxT("SIMPLEX"),      false );
}

void CMD_minuit::Execute( ParseLine const *p )
{
  wxString command( Name()+wxT(": ") );

#ifndef HAVE_MINUIT
  throw ECommandError(
    wxString(wxT("MINUIT:  the MINUIT minimization package is not available")) );
#else
  // minuit           y=expression
  // minuit           chisq_expression
  // minuit/weights w y=expression
  //
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
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
  int nTokens = p->GetNumberOfTokens();
  int icnt = 1;
  std::vector<double> weightVec;
  if( qualifiers[wxT("WEIGHTS")] )
  {
    if( icnt >= nTokens )throw ECommandError( command+wxT("expecting weight vector") );
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
  // extract the expression
  //
  wxString exprString;
  bool chisqDirect = false;
  std::vector<double> yVec;
  if( !p->IsEqualSign(icnt+1) ) // not y=expression, instead it is chisq_expression
  {
    chisqDirect = true;
    if( (icnt>=nTokens) || !p->IsString(icnt) )throw ECommandError( command+wxT("expecting expression") );
    exprString = p->GetString(icnt);
  }
  else
  {
    if( (icnt+2>=nTokens) || !p->IsString(icnt+2) )throw ECommandError( command+wxT("expecting expression") );
    exprString = p->GetString(icnt+2);
    rhs_ = p->GetString(icnt+2);
    //
    // extract the vector from the left hand side of the wxT("=")
    //
    wxString lhs( p->GetString(icnt) );
    try
    {
      NumericVariable::GetVector( lhs, wxT("left hand side of equation"), yVec );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( lhs+wxT("=") );
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
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( expr.IsCharacter() )throw ECommandError( command+wxT("expecting a numeric expression") );
  //
  std::size_t const nparam = expr.GetNumberOfFitParameters();
  if( nparam == 0 )
    throw ECommandError( command+wxT("no fitting parameters in the expression") );
  if( !chisqDirect && (yVec.size()<nparam) )
    throw ECommandError( command+wxT("number of data points < number of parameters") );
  Extrema_fcn fcn( yVec, weightVec );
  fcn.SetExpression( &expr );

  MnUserParameters upar;
  for( std::size_t i=0; i<nparam; ++i )
  {
    wxString name( expr.GetFitParameterName(i) );
    upar.add( name.mb_str(wxConvUTF8), expr.GetFitParameterValue(i), 1.0 );
    NumericData &pvd( NVariableTable::GetTable()->GetVariable(name)->GetData() );
    if( pvd.LowLimitSet() )
    {
      if( pvd.HighLimitSet() )
        upar.setLimits( name.mb_str(wxConvUTF8), pvd.GetLowLimit(), pvd.GetHighLimit() );
      else
        upar.setLowerLimit( name.mb_str(wxConvUTF8), pvd.GetLowLimit() );
    }
    else
    {
      if( pvd.HighLimitSet() )
        upar.setUpperLimit( name.mb_str(wxConvUTF8), pvd.GetHighLimit() );
    }
  }
  if( qualifiers[wxT("SIMPLEX")] )
  {
    MnSimplex simplex( fcn, upar );
    FunctionMinimum fmin = simplex();
    if( !fmin.isValid() )throw ECommandError( command+wxT("Minuit simplex failed") );
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
    if( output )
    {
      std::stringstream ss;
      ss << fmin << std::endl;
      std::string s;
      while( !std::getline(ss,s,'\n').fail() )
      {
        ExGlobals::WriteOutput( wxString(s.c_str(),wxConvUTF8) );
        s.clear();
      }
    }
  }
  else if( qualifiers[wxT("MIGRAD")] )
  {
    MnMigrad migrad( fcn, upar );
    FunctionMinimum fmin = migrad();
    if( !fmin.isValid() )throw ECommandError( command+wxT("Minuit migrad failed") );
    if( output )std::cout << fmin << "\n";
    for( std::size_t i=0; i<nparam; ++i )
      expr.SetFitParameterValue( i, fmin.userState().value(i), GetStackLine() );
    if( output )
    {
      std::stringstream ss;
      ss << fmin << std::endl;
      std::string s;
      while( !std::getline(ss,s,'\n').fail() )
      {
        ExGlobals::WriteOutput( wxString(s.c_str(),wxConvUTF8) );
        s.clear();
      }
    }
  }
  else
  {
    MnMinimize minimize( fcn, upar );
    FunctionMinimum fmin = minimize();
    if( !fmin.isValid() )throw ECommandError( command+wxT("Minuit minimize failed") );
    if( output )std::cout << fmin << "\n";
    for( std::size_t i=0; i<nparam; ++i )
      expr.SetFitParameterValue( i, fmin.userState().value(i), GetStackLine() );
    if( output )
    {
      std::stringstream ss;
      ss << fmin << std::endl;
      std::string s;
      while( !std::getline(ss,s,'\n').fail() )
      {
        ExGlobals::WriteOutput( wxString(s.c_str(),wxConvUTF8) );
        s.clear();
      }
    }
  }
  if( qualifiers[wxT("VARNAMES")] )
  {
    try
    {
      TextVariable::PutVariable( wxT("FIT$VAR"), expr.GetFitParameterNames(), GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )ExGlobals::WriteOutput( wxT("creating new array text variable: FIT$VAR") );
  }
  //
  // output chisq if chisq modifier specified
  //
  if( qualifiers[wxT("CHISQ")] )
  {
    expr.FinalPass();
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
      NumericVariable::PutVariable( wxT("FIT$CHISQ"), chi2, GetStackLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( output )ExGlobals::WriteOutput( wxT("creating new chisq scalar: FIT$CHISQ") );
  }
#endif
}

// end of file
