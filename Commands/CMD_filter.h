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
#ifndef CMD_FILTERCOMMAND
#define CMD_FILTERCOMMAND

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

class COMMANDS_LINKAGE_ CMD_filter : public Command
{
public:
  static CMD_filter *Definition()
  { return &cmd_filter_; }

  void Execute( ParseLine const *p );

private:
  CMD_filter();
  static CMD_filter cmd_filter_;
  
  double Median( std::vector<double> &, int );
};

#endif
