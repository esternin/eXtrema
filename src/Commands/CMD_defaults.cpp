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

#include "CMD_defaults.h"
#include "ExGlobals.h"
#include "ParseLine.h"
#include "GRA_window.h"

CMD_defaults *CMD_defaults::cmd_defaults_ = 0;

CMD_defaults::CMD_defaults() : Command( wxT("DEFAULTS") )
{}

void CMD_defaults::Execute( ParseLine const *p )
{
  // cannot set the window to zero since then the
  // defaults always only apply to window 0
  //
  SetUp( p );
  //
  ExGlobals::GetGraphWindow()->SetDefaults();
}

// end of file
