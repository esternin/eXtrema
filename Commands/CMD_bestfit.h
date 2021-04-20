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
#ifndef CMD_BESTFIT
#define CMD_BESTFIT

#include "Command.h"

#define COMMANDS_LINKAGE_

class ParseLine;

class COMMANDS_LINKAGE_ CMD_bestfit : public Command
{
public:
  static CMD_bestfit *Definition()
  { return &cmd_bestfit_; }

  void Execute( ParseLine const * );

private:
  CMD_bestfit();
  static CMD_bestfit cmd_bestfit_;

  void FitData( int, int, int,
                std::vector<double> const &,
                std::vector<double> const &,
                std::vector<double> const &,
                std::vector<double> const &,
                std::vector<double> const &,
                std::vector<double> const &,
                std::vector<double> & );

  void FitDataInfo( int, int, int,
                    std::vector<double> &,
                    std::vector<double> const &,
                    std::vector<double> const &,
                    std::vector<double> const & );

  bool Simq( std::vector<double> &, std::vector<double> & );
};

#endif
