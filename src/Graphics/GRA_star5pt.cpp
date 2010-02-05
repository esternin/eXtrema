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

#include "GRA_star5pt.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"

GRA_star5pt::GRA_star5pt( double xc, double yc, double side, double angle,
                          GRA_color *lineColor, GRA_color *fillColor, int lineWidth )
    : GRA_polygon(lineColor,fillColor,lineWidth,1)
{
  Set5PtStar();
  MakeStar( angle, side, xc, yc );
}

GRA_star5pt::GRA_star5pt( double x1, double y1, double x2, double y2, double angle,
                          GRA_color *lineColor, GRA_color *fillColor, int lineWidth )
    : GRA_polygon(lineColor,fillColor,lineWidth,1)
{
  Set5PtStar();
  MakeStar( angle, sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)), x1, y1 );
}

void GRA_star5pt::MakeStar( double const angle, double const side, double const xc, double const yc )
{
  double const cosang = cos(angle*M_PI/180.);
  double const sinang = sin(angle*M_PI/180.);
  double const sin72 = 0.95105651630;
  double const cos72 = 0.30901699437;
  double const sin54 = 0.80901699437;
  double const cos54 = 0.58778525229;
  double const tan54 = 1.37638192047;
  double const sin144 = cos54;
  double const cos144 = -sin54;
  double const cos216 = cos144;
  double const sin216 = -sin144;
  double const cos288 = cos72;
  double const sin288 = -sin72;
  double const s = 2.*side*cos54/(1.+sin54);
  double const h = 0.5*s*tan54;
  double const ap = (side-h)/3.;
  std::vector<double> x, y;
  x.push_back(xc+RotX(-0.5*s,-h,sinang,cosang));
  y.push_back(yc+RotY(-0.5*s,-h,sinang,cosang));
  x.push_back(xc+RotX(0.,-ap,sinang,cosang));
  y.push_back(yc+RotY(0.,-ap,sinang,cosang));
  x.push_back(xc+RotX(-0.5*s*cos72+h*sin72,-0.5*s*sin72-h*cos72,sinang,cosang));
  y.push_back(yc+RotY(-0.5*s*cos72+h*sin72,-0.5*s*sin72-h*cos72,sinang,cosang));
  x.push_back(xc+RotX(ap*sin72,-ap*cos72,sinang,cosang));
  y.push_back(yc+RotY(ap*sin72,-ap*cos72,sinang,cosang));
  x.push_back(xc+RotX(-0.5*s*cos144+h*sin144,-0.5*s*sin144-h*cos144,sinang,cosang));
  y.push_back(yc+RotY(-0.5*s*cos144+h*sin144,-0.5*s*sin144-h*cos144,sinang,cosang));
  x.push_back(xc+RotX(ap*sin144,-ap*cos144,sinang,cosang));
  y.push_back(yc+RotY(ap*sin144,-ap*cos144,sinang,cosang));
  x.push_back(xc+RotX(-0.5*s*cos216+h*sin216,-0.5*s*sin216-h*cos216,sinang,cosang));
  y.push_back(yc+RotY(-0.5*s*cos216+h*sin216,-0.5*s*sin216-h*cos216,sinang,cosang));
  x.push_back(xc+RotX(ap*sin216,-ap*cos216,sinang,cosang));
  y.push_back(yc+RotY(ap*sin216,-ap*cos216,sinang,cosang));
  x.push_back(xc+RotX(-0.5*s*cos288+h*sin288,-0.5*s*sin288-h*cos288,sinang,cosang));
  y.push_back(yc+RotY(-0.5*s*cos288+h*sin288,-0.5*s*sin288-h*cos288,sinang,cosang));
  x.push_back(xc+RotX(ap*sin288,-ap*cos288,sinang,cosang));
  y.push_back(yc+RotY(ap*sin288,-ap*cos288,sinang,cosang));
  //
  SetUp( x, y );
}
/*
std::ostream &operator<<( std::ostream &out, GRA_star5pt const &p )
{
  out << "<star5pt width=\"" << p.lineWidth_ << "\" color=\""
      << GRA_colorControl::GetColorCode(p.lineColor_) << "\" fillcolor=\""
      << GRA_colorControl::GetColorCode(p.fillColor_) << "\">\n";
  std::size_t size = p.x_.size();
  out << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size-1; ++i )out << p.x_[i] << " " << p.y_[i] << " ";
  out << p.x_[size-1] << " " << p.y_[size-1] << "</data>\n";
  return out << "</star5pt>\n";
}
*/
// end of file

