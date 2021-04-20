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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "CommandTable.h"

CommandTable *CommandTable::commandTable_ = 0;

Command *CommandTable::Get( ExString const &name ) const
{
  Command *c = 0;
  commandMap::const_iterator cEnd = cMap_->end();
  for( commandMap::const_iterator i=cMap_->begin(); i!=cEnd; ++i )
  {
    if( (*i).second->IsCommand(name) )
    {
      c = (*i).second;
      break;
    }
  }
  return c;
}

void CommandTable::AddEntry( Command *c )
{
  ExString name = c->Name();
  if( Contains( name ) )
  {
    ExString s = ExString("The command ")+name+ExString(" is already in the table");
    Application->MessageBox( const_cast<char*>(s.c_str()), "CommandTable::AddEntry", MB_OK );
    return;
  }
  cMap_->insert(entryType(name,c));
}

// end of file

