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

#include "CMD_inquire.h"
#include "ExGlobals.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EExpressionError.h"
#include "EVariableError.h"
#include "ESyntaxError.h"
#include "Expression.h"
#include "TextVariable.h"
#include "NumericVariable.h"
#include "Script.h"
#include "Inquire.h"
#include "InquireYN.h"
#include "extremaMain.h"
#include "MainGraphicsWindow.h"

CMD_inquire CMD_inquire::cmd_inquire_;

CMD_inquire::CMD_inquire() : Command( "INQUIRE" )
{
  AddQualifier( "YESNO", false );
}

void CMD_inquire::Execute( ParseLine const *p )
{
  //
  // script macro command     INQUIRE string v1 { ... v2 }
  //
  if( ExGlobals::NotInaScript() )
    throw ECommandError( "INQUIRE", "INQUIRE is only allowed in executable script files" );
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  if( qualifiers["YESNO"] )
  {
    if( !p->IsString(1) )
      throw ECommandError( "INQUIRE\\YESNO","correct syntax: INQUIRE\\YESNO string v" );
    std::auto_ptr<TInquireYNForm> iq(new TInquireYNForm(dynamic_cast<TComponent*>(MainForm)));
    ExString s;
    TextVariable::GetVariable( p->GetString(1), false, s );
    iq->SetLabel( s );
    iq->ShowModal();
  }
  else
  {
    if( !p->IsString(1) )
     throw ECommandError( "INQUIRE","correct syntax: INQUIRE string v1 { v2 ... }" );
    std::auto_ptr<TInquireForm> iq(new TInquireForm(dynamic_cast<TComponent*>(MainForm)));
    ExString s;
    TextVariable::GetVariable( p->GetString(1), false, s );
    iq->SetLabel( s );
    iq->ShowModal();
  }
}

void CMD_inquire::ProcessYN( ExString const &ans )
{
  ParseLine const *op = GetParseLine();
  if( op->GetNumberOfTokens() > 3 )
    throw ECommandError( "INQUIRE\\YESNO", "correct syntax: INQUIRE\\YESNO string v" );
  if( !op->IsString(2) )
    throw ECommandError( "INQUIRE\\YESNO", "correct syntax: INQUIRE\\YESNO string v" );
  ExString name( op->GetString(2) );
  ExString simpleName;
  try
  {
    simpleName = Variable::SimpleName( name );
  }
  catch ( Exception &e )
  {
    throw ECommandError( "INQUIRE\\YESNO", e.Message.c_str() );
  }
  TextVariable::PutVariable( name, ans, op->GetInputLine() );
}

void CMD_inquire::ProcessString( ExString const &s )
{
  if( s.empty() || s==" " )return;
  ParseLine p( s );
  try
  {
    p.ParseIt();
  }
  catch( ESyntaxError &e )
  {
    throw ECommandError( "INQUIRE", e.what() );
  }
  ParseLine const *op = GetParseLine();
  std::size_t pEnd = p.GetNumberOfTokens();
  std::size_t opEnd = op->GetNumberOfTokens();
  for( std::size_t i=0; i<pEnd && i+2<opEnd; ++i )
  {
    if( !op->IsString(i+2) )
      throw ECommandError( "INQUIRE", "correct syntax: INQUIRE string v1 { v2 ... }" );
    ExString name( op->GetString(i+2) );
    ExString simpleName;
    try
    {
      simpleName = Variable::SimpleName( name );
    }
    catch ( Exception &e )
    {
      throw ECommandError( "INQUIRE", e.Message.c_str() );
    }
    ExString value;
    if( p.IsString(i) )
    {
      value = p.GetString(i);
    }
    else if( p.IsNumeric(i) )
    {
      value = ExString( p.GetNumeric(i) );
    }
    else if( p.IsEqualSign(i) )
    {
      value = ExString("=");
    }
    else if( p.IsNull(i) )
    {
      value = ExString(" ");
    }
    Expression expr( value );
    ExString finalString;
    bool characterExpression = false;
    try
    {
      expr.Evaluate();
      if( expr.IsCharacter() )
      {
        characterExpression = true;
        finalString = expr.GetFinalString();
      }
    }
    catch ( EExpressionError &e )
    {
      characterExpression = true;
      finalString = value;
    }
    if( characterExpression )
    {
      TextVariable::PutVariable( name, finalString, op->GetInputLine() );
    }
    else // not character, must be numeric
    {
      NumericData nd( expr.GetFinalAnswer() );
      switch ( nd.GetNumberOfDimensions() )
      {
        case 0:
          NumericVariable::PutVariable( name, nd.GetScalarValue(), op->GetInputLine() );
          break;
        case 1:
          NumericVariable::PutVariable( name, nd.GetData(), 0, op->GetInputLine() );
          break;
        case 2:
          NumericVariable::PutVariable( name, nd.GetData(), nd.GetDimMag(0), nd.GetDimMag(1),
                                        op->GetInputLine() );
      }
    }
  }
}

// end of file
