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

#define COMMANDS_LINKAGE_
/*
#ifdef BUILDING_COMMANDS_DLL_
#define COMMANDS_LINKAGE_ __declspec(dllexport)
#else
#define COMMANDS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include <map>

#include "Command.h"
#include "ExString.h"

class COMMANDS_LINKAGE_ CommandTable
{
  // CommandTable is the table of pointers to Commands
  // CommandTable is a "singleton" (only one static object)
  // In CommandTable, each Command pointer is stored
  // with its name as a key to itself, so each Command
  // object must have a unique name.

protected:
  typedef std::map< ExString const, Command* > commandMap;
  typedef commandMap::value_type entryType;
  
  CommandTable()
  { cMap_ = new commandMap; }

  CommandTable( CommandTable const &rhs )
  {
    delete cMap_;
    cMap_ = new commandMap(*(rhs.cMap_));
  }

public:
  static CommandTable *GetTable()
  {
    // CommandTable is a "singleton", and can get its pointer by this function.
    // The first time this function is called, the CommandTable object is created.
    //
    if( commandTable_ == 0 )commandTable_ = new CommandTable();
    return commandTable_;
  }

  ~CommandTable()
  { delete cMap_; }

  bool Contains( Command const *c ) const
  { return Contains( c->Name() ); }
  
  bool Contains( ExString const &name ) const
  { return ( cMap_->find(name) != cMap_->end() ); }
  
  int Entries() const
  { return cMap_->size(); }

  Command *Get( ExString const & ) const;
  void AddEntry( Command * );

private:
  commandMap *cMap_;
  static CommandTable *commandTable_;
};

#endif

 
