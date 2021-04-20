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

#include "CMD_scalar.h"

#include "ExGlobals.h"
#include "ExString.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "EExpressionError.h"
#include "EVariableError.h"
#include "ECommandError.h"
#include "NumericVariable.h"
#include "NumericData.h"
#include "TextVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "Expression.h"
#include "Variable.h"

CMD_scalar CMD_scalar::cmd_scalar_;

CMD_scalar::CMD_scalar() : Command( "SCALAR" )
{
  AddQualifier( "FIT", false );
  AddQualifier( "VARY", false );
}

void CMD_scalar::Execute( ParseLine const *p )
{
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  std::vector<ExString> names;
  std::vector<double> lowLimits, highLimits;
  std::vector<bool> lowLimitSet, highLimitSet;
  for( int i=1; i<p->GetNumberOfTokens(); ++i )
  {
    if( !p->IsString(i) )throw ECommandError( "SCALAR", "expecting variable name(s)" );
    ExString name( p->GetString(i) );
    AddToStackLine( name );
    //
    // see if the name needs to be constructed first,
    // e.g.,  SCALAR\FIT 'a'//rchar(i)
    //
    Expression expr( name );
    try
    {
      expr.Evaluate();
      if( expr.IsCharacter() )name = expr.GetFinalString();
    }
    catch ( EExpressionError &e )
    {
    }
    names.push_back( Variable::SimpleName(name) );
    if( names.back() == name.UpperCase() )  // no limits attached
    {
      lowLimits.push_back( 0.0 );
      lowLimitSet.push_back( false );
      highLimits.push_back( 0.0 );
      highLimitSet.push_back( false );
    }
    else // assume there are limits attached, e.g., [low:high]
    {
      ExString limits( name.substr(names.back().size(),name.size()-names.back().size()) );
      //
      if( limits[0] != '(' && limits[0] != '{' && limits[0] != '[' )
        throw ECommandError( "SCALAR", "variable limits must begin with an opening bracket" );
      //
      int cntr = 0;
      std::size_t start = 1;
      for( std::size_t j=0; j<limits.size(); ++j )
      {
        if( limits[j]=='(' || limits[j]=='{' || limits[j]=='[' )++cntr;
        else if( limits[j]==':' && cntr==1 )
        {
          if( j == start )
          {
            lowLimits.push_back( 0.0 );
            lowLimitSet.push_back( false );
          }
          else
          {
            Expression expr( ExString("[")+limits.substr(start,j-start)+ExString("]") );
            try
            {
              expr.Evaluate();
            }
            catch ( EExpressionError &e )
            {
              throw ECommandError( "SCALAR", e.what() );
            }
            if( expr.IsCharacter() )
              throw ECommandError( "SCALAR", "character expression in variable low limit" );
            //
            NumericData nd( expr.GetFinalAnswer() );
            if( nd.GetNumberOfDimensions() > 0 )
              throw ECommandError( "SCALAR", "variable low limit must be a scalar value" );
            lowLimits.push_back( nd.GetScalarValue() );
            lowLimitSet.push_back( true );
          }
          start = j+1;
        }
        else if( (limits[j]==')'||limits[j]=='}'||limits[j]==']') && (cntr--==1) )
        {
          if( j == start )
          {
            highLimits.push_back( 0.0 );
            highLimitSet.push_back( false );
          }
          else
          {
            Expression expr( ExString("[")+limits.substr(start,j-start)+ExString("]") );
            try
            {
              expr.Evaluate();
            }
            catch ( EExpressionError &e )
            {
              throw ECommandError( "SCALAR", e.what() );
            }
            if( expr.IsCharacter() )
              throw ECommandError( "SCALAR", "character expression in variable high limit" );
            //
            NumericData nd( expr.GetFinalAnswer() );
            if( nd.GetNumberOfDimensions() > 0 )
              throw ECommandError( "SCALAR", "variable high limit must be a scalar value" );
            highLimits.push_back( nd.GetScalarValue() );
            if( lowLimitSet.back() && (lowLimits.back()>=highLimits.back()) )
              throw ECommandError( "SCALAR", "variable low limit >= high limit" );
            highLimitSet.push_back( true );
          }
          if( j != limits.size()-1 )
            throw ECommandError( "SCALAR", "variable limits syntax: [low:high] or [low:] or [:high]" );
        }
      }
      if( cntr != 0 )throw EVariableError(ExString("unmatched brackets in variable limits: ")+name);
    }
  }
  if( names.empty() )throw ECommandError( "SCALAR", "expecting some output scalar names" );
  std::size_t size = names.size();
  for( std::size_t j=0; j<size; ++j )
  {
    NumericVariable *nv = NVariableTable::GetTable()->GetVariable( names[j] );
    if( nv ) // variable exists already
    {
      NumericData &data( nv->GetData() );
      if( data.GetNumberOfDimensions() == 0 ) // it is a scalar
      {
        if( qualifiers["FIT"] || qualifiers["VARY"] )
        {
          data.SetFit();
          data.ReleaseLowLimit();
          data.ReleaseHighLimit();
          if( lowLimitSet[j] )data.SetLowLimit( lowLimits[j] );
          if( highLimitSet[j] )data.SetHighLimit( highLimits[j] );
        }
        else
        {
          data.SetFixed();
        }
        nv->AddToHistory( p->GetInputLine() );
      }
      else
      {
        NVariableTable::GetTable()->RemoveEntry( nv );
        nv = 0;
      }
    }
    else
    {
      TVariableTable::GetTable()->RemoveEntry( names[j] );
    }
    if( nv == 0 )
    {
      nv = NumericVariable::PutVariable( names[j], 0.0, p->GetInputLine() );
      if( qualifiers["FIT"] || qualifiers["VARY"] )
      {
        NumericData &data( nv->GetData() );
        data.SetFit();
        if( lowLimitSet[j] )data.SetLowLimit( lowLimits[j] );
        if( highLimitSet[j] )data.SetHighLimit( highLimits[j] );
      }
    }
  }
}

// end of file
