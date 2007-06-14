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
#include "wx/wx.h"

#include "CMD_stack.h"
#include "ExGlobals.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "NumericVariable.h"

CMD_stack *CMD_stack::cmd_stack_ = 0;

CMD_stack::CMD_stack() : Command( wxT("STACK") )
{
  AddQualifier( wxT("APPEND"), false );
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
  wxString command( Name()+wxT(": ") );
  if( p->GetNumberOfTokens()<2 || !p->IsString(1) )throw ECommandError( command+wxT("no filename was entered") );
  wxString filename;
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, filename );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( filename.Upper() == wxT("OFF") )
  {
    ExGlobals::SetStackOff();
  }
  else if( filename.Upper() == wxT("ON") )
  {
    filename = ExGlobals::GetStackFile();
    if( filename.empty() )throw ECommandError( command+wxT("no previous stack file") );
    if( !ExGlobals::SetStackOn(filename,true) )throw ECommandError( command+wxT("could not open ")+filename );
  }
  else
  {
    // get the file extension, if there was one given
    wxString extension( ExGlobals::GetFileExtension(filename) );
    if( extension.empty() )filename += wxT(".stk");
    if( !ExGlobals::SetStackOn(filename,qualifiers[wxT("APPEND")]) )
      throw ECommandError( command+wxT("could not open ")+filename );
  }
}

// end of file
