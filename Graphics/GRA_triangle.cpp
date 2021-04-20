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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GRA_color.h"
#include "GRA_colorControl.h"

#include "GRA_triangle.h"

GRA_triangle::GRA_triangle( double xc, double yc, double side, double angle,
                            GRA_color *lineColor, GRA_color *fillColor, int lineWidth )
    : GRA_polygon(lineColor,fillColor,lineWidth,1)
{
  double cosang = cos(angle*M_PI/180.);
  double sinang = sin(angle*M_PI/180.);
  double const eps = 0.000001;
  if( fabs(cosang) < eps )cosang = 0.0;
  if( fabs(sinang) < eps )sinang = 0.0;
  double const xs = side*sqrt(3.)/4.;
  double const ys = side/4.0;
  std::vector<double> x, y;
  x.push_back(xc+RotX( xs,  -ys,sinang,cosang));
  y.push_back(yc+RotY( xs,  -ys,sinang,cosang));
  x.push_back(xc+RotX( 0.,2.*ys,sinang,cosang));
  y.push_back(yc+RotY( 0.,2.*ys,sinang,cosang));
  x.push_back(xc+RotX(-xs,  -ys,sinang,cosang));
  y.push_back(yc+RotY(-xs,  -ys,sinang,cosang));
  x.push_back(xc+RotX( xs,  -ys,sinang,cosang));
  y.push_back(yc+RotY( xs,  -ys,sinang,cosang));
  //
  SetUp( x, y );
}

GRA_triangle::GRA_triangle( double x1, double y1, double x2, double y2, double x3, double y3,
                            GRA_color *lineColor, GRA_color *fillColor, int lineWidth )
    : GRA_polygon(lineColor,fillColor,lineWidth,1)
{
  std::vector<double> x, y;
  x.push_back(x1);
  y.push_back(y1);
  x.push_back(x2);
  y.push_back(y2);
  x.push_back(x3);
  y.push_back(y3);
  x.push_back(x1);
  y.push_back(y1);
  //
  SetUp( x, y );
}
/*
std::ostream &operator<<( std::ostream &out, GRA_triangle const &p )
{
  out << "<triangle width=\"" << p.lineWidth_ << "\" color=\""
      << GRA_colorControl::GetColorCode(p.lineColor_) << "\" fillcolor=\""
      << GRA_colorControl::GetColorCode(p.fillColor_) << "\">\n";
  std::size_t size = p.x_.size();
  out << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size-1; ++i )out << p.x_[i] << " " << p.y_[i] << " ";
  out << p.x_[size-1] << " " << p.y_[size-1] << "</data>\n";
  return out << "</triangle>\n";
}
*/
// end of file
