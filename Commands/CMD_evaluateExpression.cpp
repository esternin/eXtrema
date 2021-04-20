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

#include "CMD_evaluateExpression.h"
#include "ExGlobals.h"
#include "ParseLine.h"
#include "ParseToken.h"
#include "ECommandError.h"
#include "Expression.h"
#include "EExpressionError.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "NumericData.h"
#include "NumericVariable.h"

CMD_evaluateExpression CMD_evaluateExpression::cmd_evaluateExpression_;

CMD_evaluateExpression::~CMD_evaluateExpression()
{
  std::vector<ExString*>::const_iterator LhsEnd = lhs_.end();
  for( std::vector<ExString*>::const_iterator i=lhs_.begin(); i!=LhsEnd; ++i )delete *i;
  std::vector<ExString*>::const_iterator RhsEnd = rhs_.end();
  for( std::vector<ExString*>::const_iterator i=rhs_.begin(); i!=RhsEnd; ++i )delete *i;
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
  ExString lhs, simpleName;
  std::size_t j = 0;
  bool nameNotFound = true;
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
      throw EExpressionError( e.what() );
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
  ExString rhs;
  if( p->IsNull(2) )  // no right hand side was entered
  {
    if( !leftside )throw EExpressionError( "no left or right hand sides" );
    if( nameNotFound )
    {
      ExString s( "no saved expression for " );
      s += simpleName;
      throw EExpressionError( s );
    }
    rhs = *rhs_[j];
  }
  else
  {
    if( p->IsNumeric(2) )
    {
      rhs = ExString( p->GetNumeric(2) );
    }
    else if( p->IsString(2) )
    {
      rhs = p->GetString(2);
    }
    else
    {
      throw EExpressionError( "right hand side of expression is invalid" );
    }
    if( leftside )
    {
      if( nameNotFound )
      {
        ExString *tmp1 = new ExString(simpleName);
        lhs_.push_back( tmp1 );
        ExString *tmp2 = new ExString(rhs);
        rhs_.push_back( tmp2 );
      }
      else
      {
        delete rhs_[j];
        rhs_[j] = new ExString(rhs);
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
    throw;
  }
  if( expr.IsCharacter() )
  {
    ExString sFinal( expr.GetFinalString() );
    if( leftside )
    {
      try
      {
        TextVariable::PutVariable( lhs, sFinal, p->GetInputLine() );
      }
      catch ( EVariableError &e )
      {
        throw EExpressionError( e.what() );
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
        throw EExpressionError( e.what() );
      }
    }
    else
    {
      char c[50];
      switch ( nd.GetNumberOfDimensions() )
      {
        case 0:
          ExGlobals::WriteOutput( nd.GetScalarValue() );
          break;
        case 1:
        {
          std::size_t dim1 = nd.GetDimMag(0);
          if( dim1 == 0 )ExGlobals::WarningMessage( "vector result is null" );
          for( std::size_t i=0; i<dim1; ++i )ExGlobals::WriteOutput( nd.GetData(i) );
          break;
        }
        case 2:
        {
          std::size_t dim1 = nd.GetDimMag(0);
          std::size_t dim2 = nd.GetDimMag(1);
          if( dim1*dim2 == 0 )ExGlobals::WarningMessage( "matrix result is null" );
          ExString s;
          for( std::size_t i=0; i<dim1; ++i )
          {
            s.clear();
            for( std::size_t j=0; j<dim2; ++j )
            {
              sprintf( c, " % 5g", nd.GetData(i,j) );
              s += ExString(c);
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
          if( dim1*dim2*dim3 == 0 )ExGlobals::WarningMessage( "tensor result is null" );
          ExString s;
          for( std::size_t i=0; i<dim1; ++i )
          {
            for( std::size_t j=0; j<dim2; ++j )
            {
              s.clear();
              for( std::size_t k = 0; k < dim3; ++k )
              {
                sprintf( c, "% g", nd.GetData(i,j,k) );
                s += std::string(c);
              }
              ExGlobals::WriteOutput( s );
            }
            ExGlobals::WriteOutput( " " );
          }
        }
      }
    }
  }
}

// end of file
