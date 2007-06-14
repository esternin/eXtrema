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
#include <stdexcept>

#include "wx/wx.h"

#include "CMD_pause.h"
#include "ECommandError.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "TextVariable.h"

CMD_pause *CMD_pause::cmd_pause_ = 0;

CMD_pause::CMD_pause() : Command( wxT("PAUSE") )
{ AddName( wxT("TERMINAL") ); }

void CMD_pause::Execute( ParseLine const *p )
{
  SetUp( p );
  //
  wxString command( wxT("PAUSE") );
  if( ExGlobals::NotInaScript() )return;
  wxString message( wxT("Hit the <ENTER> key to continue the script") );
  if( p->GetNumberOfTokens() == 2 )
  {
    try
    {
      TextVariable::GetVariable( p->GetString(1), true, message );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
  if( !message.empty() )
  {
    ExGlobals::WriteOutput( message );
    AddToStackLine( message );
  }
  ExGlobals::PauseScripts();
}

// end of file
