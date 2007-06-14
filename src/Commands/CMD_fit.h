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
#ifndef CMD_FITCOMMAND
#define CMD_FITCOMMAND

#include "wx/wx.h"

#include "Command.h"

class ParseLine;

class CMD_fit : public Command
{
public:
  static CMD_fit *Instance()
  {
    if( !cmd_fit_ )cmd_fit_ = new CMD_fit();
    return cmd_fit_;
  }

  void Execute( ParseLine const * );

private:
  CMD_fit();
  static CMD_fit *cmd_fit_;

  wxString rhs_;
  std::vector<wxString> lhsSave_;
  std::vector<wxString> rhsSave_;
};

#endif
