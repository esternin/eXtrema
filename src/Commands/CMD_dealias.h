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
#ifndef CMD_DEALIAS
#define CMD_DEALIAS

#include "Command.h"

class ParseLine;

class CMD_dealias : public Command
{
public:
  static CMD_dealias *Instance()
  {
    if( !cmd_dealias_ )cmd_dealias_ = new CMD_dealias();
    return cmd_dealias_;
  }

  void Execute( ParseLine const * );

private:
  CMD_dealias();
  static CMD_dealias *cmd_dealias_;
};

#endif
