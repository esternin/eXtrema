/*
Copyright (C) 2005,...,2009 Joseph L. Chuma

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
#ifndef GRA_ARROW1
#define GRA_ARROW1

#include "GRA_polygon.h"

class GRA_color;

class GRA_arrow1 : public GRA_polygon
{
public:
  GRA_arrow1( double, double, double, double, bool, GRA_color * =0,
              GRA_color * =0, int =1, double =1.0, double =1.0 );

  ~GRA_arrow1()
  {}

private:
  bool headsBothEnds_;
};

#endif
 
