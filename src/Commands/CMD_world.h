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
#ifndef CMD_WORLD
#define CMD_WORLD

#include "Command.h"

class ParseLine;

class CMD_world : public Command
{
public:
  static CMD_world *Instance()
  {
    if( !cmd_world_ )cmd_world_ = new CMD_world();
    return cmd_world_;
  }
  
  void Execute( ParseLine const * );

private:
  CMD_world();
  static CMD_world *cmd_world_;
};

#endif
