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
#ifndef CMD_SAVE
#define CMD_SAVE

#include "wx/wx.h"

#include "Command.h"

class ParseLine;

class CMD_save : public Command
{
public:
  static CMD_save *Instance()
  {
    if( !cmd_save_ )cmd_save_ = new CMD_save();
    return cmd_save_;
  }
  
  void Execute( ParseLine const * );

private:
  CMD_save();
  static CMD_save *cmd_save_;

  std::ofstream ofStream_;
  wxString filename_;
};

#endif
