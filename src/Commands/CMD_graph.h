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
#ifndef CMD_GRAPH
#define CMD_GRAPH

#include "Command.h"

class ParseLine;

class CMD_graph : public Command
{
public:
  static CMD_graph *Instance()
  {
    if( !cmd_graph_ )cmd_graph_ = new CMD_graph();
    return cmd_graph_;
  }

  void Execute( ParseLine const * );

private:
  CMD_graph();
  static CMD_graph *cmd_graph_;
};

#endif
