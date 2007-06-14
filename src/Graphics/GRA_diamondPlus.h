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
#ifndef GRA_DIAMONDPLUS
#define GRA_DIAMONDPLUS

#include <ostream>

#include "GRA_multiLineFigure.h"

class GRA_color;

class GRA_diamondPlus : public GRA_multiLineFigure
{
public:
  GRA_diamondPlus( double, double, double, double, GRA_color * =0, int =1 );
  virtual ~GRA_diamondPlus();

  friend std::ostream &operator<<( std::ostream &, GRA_diamondPlus const & );
};
#endif
 
