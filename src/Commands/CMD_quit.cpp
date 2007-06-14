/*
Copyright (C) 2005,...,2007 Joseph L. Chuma, TRIUMF

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

#include "CMD_quit.h"
#include "ParseLine.h"
#include "ExGlobals.h"

CMD_quit *CMD_quit::cmd_quit_ = 0;

CMD_quit::CMD_quit() : Command( wxT("QUIT") )
{}

void CMD_quit::Execute( ParseLine const *p )
{
  SetUp( p );
  ExGlobals::StopAllScripts();
  ExGlobals::QuitApp();
}

// end of file

