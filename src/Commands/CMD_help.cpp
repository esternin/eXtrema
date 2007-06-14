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

#include "CMD_help.h"
#include "ECommandError.h"
#include "ParseLine.h"
#include "ParseToken.h"
#include "TextVariable.h"
#include "ExGlobals.h"

CMD_help *CMD_help::cmd_help_ = 0;

CMD_help::CMD_help() : Command( wxT("HELP") )
{}

void CMD_help::Execute( ParseLine const *p )
{
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  ExGlobals::StartHelp();
}

// end of file
