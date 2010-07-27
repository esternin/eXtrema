/*
Copyright (C) 2005,,2010 Joseph L. Chuma

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
#ifndef CMD_CONTOUR
#define CMD_CONTOUR

#include "Command.h"

class ParseLine;

class CMD_contour : public Command
{
public:
  static CMD_contour *Instance()
  {
    if( !cmd_contour_ )cmd_contour_ = new CMD_contour();
    return cmd_contour_;
  }
  
  void Execute( ParseLine const * );

private:
  CMD_contour();
  static CMD_contour *cmd_contour_;
  
  void GetIntParameter( std::vector<int> &, ParseLine const *, int &, wxString const & );
  void GetDataParameters( std::vector<double> &, std::vector<double> &, std::vector<double> &,
                          ParseLine const *, int & );
};

#endif
