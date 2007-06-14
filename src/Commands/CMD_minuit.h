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
#ifndef CMD_MINUITCOMMAND
#define CMD_MINUITCOMMAND

#include "wx/wx.h"

#include "Command.h"

class ParseLine;

class CMD_minuit : public Command
{
public:
  static CMD_minuit *Instance()
  {
    if( !cmd_minuit_ )cmd_minuit_ = new CMD_minuit();
    return cmd_minuit_;
  }

  void Execute( ParseLine const * );

private:
  CMD_minuit();
  static CMD_minuit *cmd_minuit_;

  wxString rhs_;
};

#endif
