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
#ifndef CMD_DENSITY
#define CMD_DENSITY

#include "Command.h"

class ParseLine;

class CMD_density : public Command
{
public:
  static CMD_density *Instance()
  {
    if( !cmd_density_ )cmd_density_ = new CMD_density();
    return cmd_density_;
  }

  void Execute( ParseLine const * );

private:
  CMD_density();
  static CMD_density *cmd_density_;
};

#endif
