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

#include "CMD_enddo.h"
#include "ECommandError.h"
#include "ESyntaxError.h"
#include "ParseLine.h"
#include "Script.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "NumericVariable.h"

CMD_enddo *CMD_enddo::cmd_enddo_ = 0;

CMD_enddo::CMD_enddo() : Command( wxT("ENDDO") )
{}

void CMD_enddo::Execute( ParseLine const *p )
{
  SetUp( p );
  //
  Script *script = ExGlobals::GetScript();
  if( !script )throw ECommandError( Name()+wxT(": only valid in a script") );
  //
  bool finishedDo;
  wxString name;
  double value;
  try
  {
    finishedDo = script->ProcessEnddo( name, value );
  }
  catch (ESyntaxError &e)
  {
    throw ECommandError( Name()+wxT(": ")+wxString(e.what(),wxConvUTF8) );
  }
  if( !finishedDo )
  {
    try
    {
      NumericVariable::PutVariable( name, value, p->GetInputLine(), false );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( Name()+wxT(": ")+wxString(e.what(),wxConvUTF8) );
    }
  }
}

// end of file
