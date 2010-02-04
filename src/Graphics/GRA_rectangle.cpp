/*
Copyright (C) 2005,...,2010 Joseph L. Chuma

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
#include <cmath>

#include "GRA_rectangle.h"
#include "GRA_color.h"

GRA_rectangle::GRA_rectangle( double xc, double yc, double xSide, double ySide,
                              double angle, GRA_color *lineColor, GRA_color *fillColor,
                              int lineWidth )
  : GRA_polygon(lineColor,fillColor,lineWidth,1)
{
  SetRectangle();
  double const cosang = cos(angle*M_PI/180.);
  double const sinang = sin(angle*M_PI/180.);
  double const xs = xSide*0.5;
  double const ys = ySide*0.5;
  std::vector<double> x, y;
  x.push_back(xc+RotX( xs,-ys,sinang,cosang));
  y.push_back(yc+RotY( xs,-ys,sinang,cosang));
  x.push_back(xc+RotX( xs, ys,sinang,cosang));
  y.push_back(yc+RotY( xs, ys,sinang,cosang));
  x.push_back(xc+RotX(-xs, ys,sinang,cosang));
  y.push_back(yc+RotY(-xs, ys,sinang,cosang));
  x.push_back(xc+RotX(-xs,-ys,sinang,cosang));
  y.push_back(yc+RotY(-xs,-ys,sinang,cosang));
  x.push_back(xc+RotX( xs,-ys,sinang,cosang));
  y.push_back(yc+RotY( xs,-ys,sinang,cosang));
  //
  SetUp( x, y );
}

GRA_rectangle::GRA_rectangle( double x1, double y1, double x2, double y2, double angle,
                              bool square, GRA_color *lineColor, GRA_color *fillColor,
                              int lineWidth )
  : GRA_polygon(lineColor,fillColor,lineWidth,1)
{
  SetRectangle();
  double xul=x1, yul=y1, xlr=x2, ylr=y2;
  if( xul > xlr )
  {
    xul = x2;
    xlr = x1;
  }
  if( ylr > yul )
  {
    ylr = y1;
    yul = y2;
  }
  double dx = xlr - xul;
  double dy = yul - ylr;
  if( square )
  {
    if( dx < dy )
    {
      ylr = yul - dx;
      dy = dx;
    }
    else if( dy < dx )
    {
      xlr = xul + dy;
      dx = dy;
    }
  }
  double const xc = (xlr+xul)*0.5;
  double const yc = (ylr+yul)*0.5;
  double const cosang = cos(angle*M_PI/180.);
  double const sinang = sin(angle*M_PI/180.);
  double const xs = dx*0.5;
  double const ys = dy*0.5;
  std::vector<double> x, y;
  x.push_back(xc+RotX( xs,-ys,sinang,cosang));
  y.push_back(yc+RotY( xs,-ys,sinang,cosang));
  x.push_back(xc+RotX( xs, ys,sinang,cosang));
  y.push_back(yc+RotY( xs, ys,sinang,cosang));
  x.push_back(xc+RotX(-xs, ys,sinang,cosang));
  y.push_back(yc+RotY(-xs, ys,sinang,cosang));
  x.push_back(xc+RotX(-xs,-ys,sinang,cosang));
  y.push_back(yc+RotY(-xs,-ys,sinang,cosang));
  x.push_back(xc+RotX( xs,-ys,sinang,cosang));
  y.push_back(yc+RotY( xs,-ys,sinang,cosang));
  //
  SetUp( x, y );
}

// end of file
