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
#ifndef EX_COMMANDTABLE
#define EX_COMMANDTABLE

#include <iostream>
#include <map>

#include "wx/wx.h"

#include "Command.h"

class CommandTable
{
  // CommandTable is the table of pointers to Commands
  // CommandTable is a "singleton" (only one static object)
  // In CommandTable, each Command pointer is stored
  // with its name as a key to itself, so each Command
  // object must have a unique name.

protected:
  typedef std::map< wxString const, Command* > commandMap;
  typedef commandMap::value_type entryType;
  
  CommandTable();
  CommandTable( CommandTable const & );

public:
  static CommandTable *GetTable();

  ~CommandTable();
  
  void ClearTable();

  bool Contains( Command const *c ) const
  { return Contains( c->Name() ); }
  
  bool Contains( wxString const &name ) const
  { return ( cMap_->find(name) != cMap_->end() ); }
  
  int Entries() const
  { return cMap_->size(); }

  Command *Get( wxString const & ) const;
  void AddEntry( Command * );

  void CreateCommands();

private:
  commandMap *cMap_;
  static CommandTable *commandTable_;
};

#endif
