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
#ifndef CMD_WRITE
#define CMD_WRITE

#include <fstream>

#include "wx/wx.h"

#include "Command.h"

class ParseLine;

class CMD_write : public Command
{
public:
  static CMD_write *Instance()
  {
    if( !cmd_write_ )cmd_write_ = new CMD_write();
    return cmd_write_;
  }

  void Execute( ParseLine const * );
  
  void SetFilename( wxString const &s )
  { filename_ = s; }
  
  wxString &GetFilename()
  { return filename_; }
  
  std::ofstream *GetStream()
  { return &ofStream_; }

private:
  CMD_write();
  static CMD_write *cmd_write_;

  std::ofstream ofStream_;
  wxString filename_;
};

#endif
