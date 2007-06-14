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
#ifndef CMD_DISPLAY
#define CMD_DISPLAY

#include "Command.h"

class ParseLine;

class CMD_display : public Command
{
public:
  static CMD_display *Instance()
  {
    if( !cmd_display_ )cmd_display_ = new CMD_display();
    return cmd_display_;
  }

  void Execute( ParseLine const *p );

private:
  CMD_display();
  static CMD_display *cmd_display_;
};

#endif
