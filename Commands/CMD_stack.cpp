/*
Copyright (C) 2006 Joseph L. Chuma, TRIUMF

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

#include "CMD_stack.h"
#include "ExGlobals.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "NumericVariable.h"

CMD_stack CMD_stack::cmd_stack_;

CMD_stack::CMD_stack() : Command( "STACK" )
{
  AddQualifier( "APPEND", false );
  writeStack_ = false;
}

void CMD_stack::Execute( ParseLine const *p )
{
  // STACK filename
  // STACK\APPEND filename
  // STACK OFF
  // STACK ON   <-->  STACK\APPEND sameFilename
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
  if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
    throw ECommandError( "STACK", "no filename was entered" );
  ExString filename;
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, filename );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "STACK", e.what() );
  }
  if( filename.UpperCase() == "OFF" )
  {
    ExGlobals::SetStackOff();
  }
  else if( filename.UpperCase() == "ON" )
  {
    filename = ExGlobals::GetStackFile();
    if( filename.empty() )
      throw ECommandError( "STACK", "no previous stack file" );
    if( !ExGlobals::SetStackOn(filename,true) )
    {
      ExString s = ExString("could not open ") + filename;
      throw ECommandError( "STACK", s );
    }
  }
  else
  {
    ExString extension( filename.ExtractFileExt() );
    if( extension.empty() )filename += ExString(".stk");
    if( !ExGlobals::SetStackOn(filename,qualifiers["APPEND"]) )
    {
      ExString s = ExString("could not open ") + filename;
      throw ECommandError( "STACK", s );
    }
  }
}

// end of file
