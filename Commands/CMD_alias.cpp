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

#include "CMD_alias.h"
#include "ExGlobals.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "TextVariable.h"

CMD_alias CMD_alias::cmd_alias_;

CMD_alias::CMD_alias() : Command( "ALIAS" )
{
  AddQualifier( "EXPAND", false );
}

void CMD_alias::Execute( ParseLine const *p )
{
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch( ECommandError const &e )
  {
    throw;
  }
  int nTokens = p->GetNumberOfTokens();
  if( nTokens == 1 )
  {
    ExGlobals::DisplayAliases();
  }
  else if( nTokens == 2 )
  {
    if( !p->IsString(1) )throw ECommandError( "ALIAS", "expecting an alias name" );
    ExGlobals::DisplayAlias( p->GetString(1) );
    AddToStackLine( p->GetString(1) );
  }
  else
  {
    if( !p->IsString(1) )throw ECommandError( "ALIAS", "expecting an alias name" );
    if( !p->IsString(2) )throw ECommandError( "ALIAS", "expecting name 'value'" );
    ExString value;
    if( qualifiers["EXPAND"] )
    {
      try
      {
        TextVariable::GetVariable( p->GetString(2), true, value );
      }
      catch( EVariableError const &e )
      {
        throw ECommandError( "ALIAS", e.what() );
      }
    }
    else
    {
      value = p->GetInputLine().substr( 1+p->GetInputLine().find(ExString(" ")+p->GetString(2)) );
    }
    ExGlobals::AddAlias( p->GetString(1).UpperCase(), value );
    AddToStackLine( p->GetString(1) );
    AddToStackLine( p->GetString(2) );
  }
}

// end of file
