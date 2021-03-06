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
#include "wx/wx.h"

#include "CMD_dealias.h"
#include "ExGlobals.h"
#include "ParseLine.h"
#include "ECommandError.h"

CMD_dealias *CMD_dealias::cmd_dealias_ = 0;

CMD_dealias::CMD_dealias() : Command( wxT("DEALIAS") )
{}

void CMD_dealias::Execute( ParseLine const *p )
{
  SetUp( p );
  wxString command( Name()+wxT(": ") );
  int nTokens = p->GetNumberOfTokens();
  if( nTokens==1 || !p->IsString(1) )throw ECommandError( command+wxT("expecting an alias name") );
  
  if( p->GetString(1).Upper() == wxT("ALL") )
  {
    ExGlobals::RemoveAliases();
    ExGlobals::WriteOutput( wxT("All aliases are now gone") );
  }
  else
  {
    if( ExGlobals::RemoveAlias(p->GetString(1).Upper()) )
      ExGlobals::WriteOutput( wxString()<<wxT("Alias ")<<p->GetString(1)<<wxT(" is now gone") );
    else
      ExGlobals::WriteOutput( p->GetString(1)+wxT(" is not an alias") );
  }
  AddToStackLine( p->GetString(1) );
}

// end of file
