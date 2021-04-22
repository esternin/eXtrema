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
#include "CMD_define.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "ECommandError.h"

CMD_define *CMD_define::cmd_define_ = 0;

CMD_define::CMD_define() : Command( wxT("DEFINE") )
{
  AddQualifier( wxT("CONSTANTS"), true );
}

void CMD_define::Execute( ParseLine const *p )
{
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
  if( qualifiers[wxT("CONSTANTS")] )ExGlobals::DefineConstants();
}

// end of file
