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
#include "CMD_evaluateExpression.h"

#include "ECommandError.h"
#include "ParseLine.h"
#include "ParseToken.h"
#include "EExpressionError.h"
#include "Expression.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "NumericData.h"
#include "NumericVariable.h"
#include "ExGlobals.h"

CMD_evaluateExpression *CMD_evaluateExpression::cmd_evaluateExpression_ = 0;

CMD_evaluateExpression::CMD_evaluateExpression() : Command( wxT("EVALUATEEXPRESSION") )
{}


CMD_evaluateExpression::~CMD_evaluateExpression()
{
  std::vector<wxString*>::const_iterator LhsEnd = lhs_.end();
  for( std::vector<wxString*>::const_iterator i=lhs_.begin(); i!=LhsEnd; ++i )delete *i;
  std::vector<wxString*>::const_iterator RhsEnd = rhs_.end();
  for( std::vector<wxString*>::const_iterator i=rhs_.begin(); i!=RhsEnd; ++i )delete *i;
}

void CMD_evaluateExpression::Execute( ParseLine const *p )
{
  // First, determine the nature of the input expression. This may be
  // either an assignment or a prompt (an isolated expression to be
  // evaluated and discarded). These cases are differentiated by the
  // presence of a variable name (string) to the left of the "=" in the
  // assignment statement. For an assignment, the first token is a string,
  // while for a prompt, it is a null.
  //
  // If the line is an assignment statement, and an isolated "=" follows the
  // variable name, the previously assigned expression is re-evaluated and
  // assigned to the left hand side.  Check the left hand side for a match
  // against the table of variable names. If no match is found, it is
  // assumed a new variable is to be defined by means of this assignment.
  //
  SetUp( p );
  EXStackLine(); // stack line = the input line (special for this class)
  //
  // extract the variable from the left of the "="
  //
  bool leftside = p->IsString(0);
  wxString lhs, simpleName;
  std::size_t j = 0;
  bool nameNotFound = true;
  wxString command( Name()+wxT(": ") );
  if( leftside )
  {
    // get the uppercase name without indices
    //
    lhs = p->GetString(0);
    try
    {
      simpleName = Variable::SimpleName( lhs );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    while( j < lhs_.size() )
    {
      if( simpleName == *lhs_[j] )
      {
        nameNotFound = false;
        break;
      }
      ++j;
    }
  }
  //
  // Extract the expression from the right of the "=".  If no expression
  // has been entered, restore the previous assignment of this variable
  // and use this expression.
  //
  wxString rhs;
  if( p->IsNull(2) )  // no right hand side was entered
  {
    if( !leftside )
      throw ECommandError( command+wxT("no left or right hand sides") );
    if( nameNotFound )
      throw ECommandError( command+wxT("no saved expression for ")+simpleName );
    rhs = *rhs_[j];
  }
  else
  {
    if( p->IsNumeric(2) )    rhs << p->GetNumeric(2);
    else if( p->IsString(2) )rhs = p->GetString(2);
    else throw ECommandError( command+wxT("right hand side of expression is invalid") );
    if( leftside )
    {
      if( nameNotFound )
      {
        wxString *tmp1 = new wxString(simpleName);
        lhs_.push_back( tmp1 );
        wxString *tmp2 = new wxString(rhs);
        rhs_.push_back( tmp2 );
      }
      else
      {
        delete rhs_[j];
        rhs_[j] = new wxString(rhs);
      }
    }
  }
  //
  // evaluate the expression
  //
  Expression expr( rhs );
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( expr.IsCharacter() )
  {
    wxString sFinal( expr.GetFinalString() );
    if( leftside )
    {
      try
      {
        TextVariable::PutVariable( lhs, sFinal, p->GetInputLine() );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
    }
    else
    {
      ExGlobals::WriteOutput( sFinal );
    }
  }
  else // not character, must be numeric
  {
    NumericData nd( expr.GetFinalAnswer() );
    if( leftside )
    {
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
    }
    else
    {
      wxChar c[50];
      switch ( nd.GetNumberOfDimensions() )
      {
        case 0:
        {
          wxString s;
          ExGlobals::WriteOutput( s << nd.GetScalarValue() );
          break;
        }
        case 1:
        {
          std::size_t dim1 = nd.GetDimMag(0);
          if( dim1 == 0 )ExGlobals::WarningMessage( wxT("vector result is null") );
          wxString s;
          for( std::size_t i=0; i<dim1; ++i )
          {
            ExGlobals::WriteOutput( s << nd.GetData(i) );
            s.clear();
          }
          break;
        }
        case 2:
        {
          std::size_t dim1 = nd.GetDimMag(0);
          std::size_t dim2 = nd.GetDimMag(1);
          if( dim1*dim2 == 0 )ExGlobals::WarningMessage( wxT("matrix result is null") );
          wxString s;
          for( std::size_t i=0; i<dim1; ++i )
          {
            s.clear();
            for( std::size_t j=0; j<dim2; ++j )
            {
              ::wxSnprintf( c, 50, wxT(" % 5g"), nd.GetData(i,j) );
              s += c;
            }
            ExGlobals::WriteOutput( s );
          }
          break;
        }
        case 3:
        {
          std::size_t dim1 = nd.GetDimMag(0);
          std::size_t dim2 = nd.GetDimMag(1);
          std::size_t dim3 = nd.GetDimMag(2);
          if( dim1*dim2*dim3 == 0 )ExGlobals::WarningMessage( wxT("tensor result is null") );
          wxString s;
          for( std::size_t i=0; i<dim1; ++i )
          {
            for( std::size_t j=0; j<dim2; ++j )
            {
              s.clear();
              for( std::size_t k = 0; k < dim3; ++k )
              {
                ::wxSnprintf( c, 50, wxT("% g"), nd.GetData(i,j,k) );
                s += c;
              }
              ExGlobals::WriteOutput( s );
            }
            ExGlobals::WriteOutput( wxT(" ") );
          }
        }
      }
    }
  }
}

// end of file
