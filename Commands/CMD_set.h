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

#include "ExString.h"
#include "Command.h"

#define COMMANDS_LINKAGE_
/*
#ifdef BUILDING_COMMANDS_DLL_
#define COMMANDS_LINKAGE_ __declspec(dllexport)
#else
#define COMMANDS_LINKAGE_ __declspec(dllimport)
#endif
*/

class ParseLine;
class GRA_color;
class GRA_font;

class COMMANDS_LINKAGE_ CMD_set : public Command
{
public:
  static CMD_set *Definition()
  { return &cmd_set_; }
  
  void Execute( ParseLine const * );
  
  ExString const &GetPCHARsymbol() const
  { return pcharSymbol_; }
  
  ExString const &GetPCHARsize() const
  { return pcharSize_; }
  
  ExString const &GetPCHARcolor() const
  { return pcharColor_; }
  
  ExString const &GetPCHARangle() const
  { return pcharAngle_; }

private:
  CMD_set() : Command( "SET" )
  {}

  static CMD_set cmd_set_;
  
  double GetValue( ExString const & );
  GRA_color *GetColor( ExString const & );
  GRA_font *GetFont( ExString const & );
  ExString GetString( ExString const & );
  //
  int valueIndex_;
  //
  ExString pcharSymbol_, pcharSize_, pcharColor_, pcharAngle_;
};

#endif
