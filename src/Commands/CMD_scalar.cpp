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
#include "wx/wx.h"

#include "CMD_scalar.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "NumericData.h"
#include "TextVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "Expression.h"
#include "Variable.h"
#include "EExpressionError.h"

CMD_scalar *CMD_scalar::cmd_scalar_ = 0;

CMD_scalar::CMD_scalar() : Command( wxT("SCALAR") )
{
  AddQualifier( wxT("FIT"), false );
  AddQualifier( wxT("VARY"), false );
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
  wxString command( Name()+wxT(": ") );
  std::vector<wxString> names;
  std::vector<double> lowLimits, highLimits;
  std::vector<bool> lowLimitSet, highLimitSet;
  for( int counter=1; counter<p->GetNumberOfTokens(); ++counter )
  {
    if( !p->IsString(counter) )throw ECommandError( command+wxT("expecting variable name(s)") );
    wxString name( p->GetString(counter) );
    AddToStackLine( name );
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
    if( names.back() == name.Upper() ) // no limits attached
    {
      lowLimits.push_back( 0.0 );
      lowLimitSet.push_back( false );
      highLimits.push_back( 0.0 );
      highLimitSet.push_back( false );
    }
    else // assume there are limits attached, e.g., [low:high]
    {
      wxString limits( name.substr(names.back().size(),name.size()-names.back().size()) );
      if( limits[0]!='(' && limits[0]!='{' && limits[0]!='[' )
        throw ECommandError( command+wxT("variable limits must begin with an opening bracket") );
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
            Expression expr( wxString()<<'['<<limits.substr(start,j-start)<<']' );
            try
            {
              expr.Evaluate();
            }
            catch ( EExpressionError &e )
            {
              throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
            }
            if( expr.IsCharacter() )
              throw ECommandError( command+wxT("character expression in variable low limit") );
            NumericData nd( expr.GetFinalAnswer() );
            if( nd.GetNumberOfDimensions() > 0 )
              throw ECommandError( command+wxT("variable low limit must be a scalar value") );
            lowLimits.push_back( nd.GetScalarValue() );
            lowLimitSet.push_back( true );
          }
          start = j+1;
        }
        else if( (limits[j]==')'||limits[j]=='}'||limits[j]==']') && (cntr--==1) )
        {
          if( j = start )
          {
            highLimits.push_back( 0.0 );
            highLimits.push_back( false );
          }
          else
          {
            Expression expr( wxString()<<'['<<limits.substr(start,j-start)<<']' );
            try
            {
              expr.Evaluate();
            }
            catch ( EExpressionError &e )
            {
              throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
            }
            if( expr.IsCharacter() )
              throw ECommandError( command+wxT("character expression in variable high limit") );
            NumericData nd( expr.GetFinalAnswer() );
            if( nd.GetNumberOfDimensions() > 0 )
              throw ECommandError( command+wxT("variable high limit must be a scalar value") );
            highLimits.push_back( nd.GetScalarValue() );
            if( lowLimitSet.back() && (lowLimits.back()>=highLimits.back()) )
              throw ECommandError( command+wxT("variable low limit >= high limit") );
            highLimitSet.push_back( true );
          }
          if( j != limits.size()-1 )
            throw ECommandError(
              command+wxT("variable limits syntax: [low:high] or [low:] or [:high]") );
        }
      }
      if( cntr != 0 )throw ECommandError( command+wxT("unmatched brackets in variable limits") );
    }
  }
  if( names.empty() )throw ECommandError( command+wxT("expecting some output scalar names") );
  std::size_t size = names.size();
  for( std::size_t j=0; j<size; ++j )
  {
    bool doesNotExistAlready = true;
    NumericVariable *nv = NVariableTable::GetTable()->GetVariable( names[j] );
    if( nv )
    {
      NumericData &data( nv->GetData() );
      if( data.GetNumberOfDimensions() == 0 ) // it is a scalar
      {
        if( qualifiers[wxT("FIT")] || qualifiers[wxT("VARY")] )
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
        doesNotExistAlready = false;
      }
      else
      {
        NVariableTable::GetTable()->RemoveEntry( nv );
        nv = 0;
      }
    }
    else
      TVariableTable::GetTable()->RemoveEntry( names[j] );
    if( doesNotExistAlready )
    {
      NumericVariable *nv = NumericVariable::PutVariable( names[j], 0.0, p->GetInputLine() );
      NumericData &data( nv->GetData() );
      if( qualifiers[wxT("FIT")] || qualifiers[wxT("VARY")] )
      {
        data.SetFit();
        if( lowLimitSet[j] )data.SetLowLimit( lowLimits[j] );
        if( highLimitSet[j] )data.SetHighLimit( highLimits[j] );
      }
    }
  }
}
 
// end of file
