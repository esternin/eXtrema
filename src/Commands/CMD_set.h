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
#ifndef CMD_SET
#define CMD_SET

#include "wx/wx.h"

#include "Command.h"

class ParseLine;
class GRA_color;
class GRA_font;

class CMD_set : public Command
{
public:
  static CMD_set *Instance()
  {
    if( !cmd_set_ )cmd_set_ = new CMD_set();
    return cmd_set_;
  }
  
  void Execute( ParseLine const * );
  
  wxString const &GetPCHARsymbol() const
  { return pcharSymbol_; }
  
  wxString const &GetPCHARsize() const
  { return pcharSize_; }
  
  wxString const &GetPCHARcolor() const
  { return pcharColor_; }
  
  wxString const &GetPCHARangle() const
  { return pcharAngle_; }

private:
  CMD_set();
  static CMD_set *cmd_set_;
  
  double GetValue( wxString const & );
  GRA_color *GetColor( wxString const & );
  GRA_font *GetFont( wxString const & );
  wxString GetString( wxString const & );
  //
  int valueIndex_;
  //
  wxString pcharSymbol_, pcharSize_, pcharColor_, pcharAngle_;
};

#endif
