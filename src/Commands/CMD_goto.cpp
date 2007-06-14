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

#include "CMD_goto.h"
#include "ECommandError.h"
#include "ESyntaxError.h"
#include "ParseLine.h"
#include "Script.h"
#include "ExGlobals.h"

CMD_goto *CMD_goto::cmd_goto_ = 0;

CMD_goto::CMD_goto() : Command( wxT("GOTO") )
{}

void CMD_goto::Execute( ParseLine const *p )
{
  SetUp( p );
  wxString command( Name()+wxT(": ") );
  if( !p->IsString(1) )throw ECommandError( command+wxT("invalid GOTO label") );
  Script *script = ExGlobals::GetScript();
  if( !script )throw ECommandError( command+wxT("GOTO is only valid in a script") );
  try
  {
    script->ProcessGoTo( p->GetString(1).Upper() );
  }
  catch (ESyntaxError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
}

// end of file
