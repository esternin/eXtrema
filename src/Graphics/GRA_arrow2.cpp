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
#include <cmath>

#include "GRA_arrow2.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"

GRA_arrow2::GRA_arrow2( double x1, double y1, double x2, double y2, bool heads,
                        GRA_color *lineColor, GRA_color *fillColor, int lineWidth,
			double widthFactor, double lengthFactor )
  : GRA_polygon(lineColor,fillColor,lineWidth), headsBothEnds_(heads)
{
  double const dx = x2 - x1;
  double const dy = y2 - y1;
  double const angle = atan2(dy,dx)+0.5*M_PI;
  double const cosang = cos(angle);
  double const sinang = sin(angle);
  double const length = sqrt(dx*dx+dy*dy);
  double const stemWidth = 0.01;
  //double const headWidth = 0.3;
  //double const headLength = 0.3;
  double const temp = length - 15.;
  double headLength = 0.2+(temp/5.)*(temp/5.)*exp(-temp*temp/25.);
  double headWidth = 0.75*headLength;
  headWidth *= widthFactor;
  headLength *= lengthFactor;
  double const xc = 0.5*(x1+x2);
  double const yc = 0.5*(y1+y2);
  double const tan30 = tan(30.*M_PI/180.);
  std::vector<double> x, y;
  if( headsBothEnds_ )
  {
    x.push_back(xc+RotX(0.,-0.5*length,sinang,cosang));
    y.push_back(yc+RotY(0.,-0.5*length,sinang,cosang));
    x.push_back(xc+RotX(-headWidth*tan30,-0.5*length+headLength,sinang,cosang));
    y.push_back(yc+RotY(-headWidth*tan30,-0.5*length+headLength,sinang,cosang));
    x.push_back(xc+RotX(-stemWidth,-0.5*length+headLength,sinang,cosang));
    y.push_back(yc+RotY(-stemWidth,-0.5*length+headLength,sinang,cosang));
    x.push_back(xc+RotX(-stemWidth,0.5*length-headLength,sinang,cosang));
    y.push_back(yc+RotY(-stemWidth,0.5*length-headLength,sinang,cosang));
    x.push_back(xc+RotX(-headWidth*tan30,0.5*length-headLength,sinang,cosang));
    y.push_back(yc+RotY(-headWidth*tan30,0.5*length-headLength,sinang,cosang));
    x.push_back(xc+RotX(0.,0.5*length,sinang,cosang));
    y.push_back(yc+RotY(0.,0.5*length,sinang,cosang));
    x.push_back(xc+RotX(headWidth*tan30,0.5*length-headLength,sinang,cosang));
    y.push_back(yc+RotY(headWidth*tan30,0.5*length-headLength,sinang,cosang));
    x.push_back(xc+RotX(stemWidth,0.5*length-headLength,sinang,cosang));
    y.push_back(yc+RotY(stemWidth,0.5*length-headLength,sinang,cosang));
    x.push_back(xc+RotX(stemWidth,-0.5*length+headLength,sinang,cosang));
    y.push_back(yc+RotY(stemWidth,-0.5*length+headLength,sinang,cosang));
    x.push_back(xc+RotX(headWidth*tan30,-0.5*length+headLength,sinang,cosang));
    y.push_back(yc+RotY(headWidth*tan30,-0.5*length+headLength,sinang,cosang));
  }
  else
  {
    x.push_back(xc+RotX(-stemWidth,-0.5*length,sinang,cosang));
    y.push_back(yc+RotY(-stemWidth,-0.5*length,sinang,cosang));
    x.push_back(xc+RotX(-stemWidth,0.5*length-headLength,sinang,cosang));
    y.push_back(yc+RotY(-stemWidth,0.5*length-headLength,sinang,cosang));
    x.push_back(xc+RotX(-headWidth*tan30,0.5*length-headLength,sinang,cosang));
    y.push_back(yc+RotY(-headWidth*tan30,0.5*length-headLength,sinang,cosang));
    x.push_back(xc+RotX(0.,0.5*length,sinang,cosang));
    y.push_back(yc+RotY(0.,0.5*length,sinang,cosang));
    x.push_back(xc+RotX(headWidth*tan30,0.5*length-headLength,sinang,cosang));
    y.push_back(yc+RotY(headWidth*tan30,0.5*length-headLength,sinang,cosang));
    x.push_back(xc+RotX(stemWidth,0.5*length-headLength,sinang,cosang));
    y.push_back(yc+RotY(stemWidth,0.5*length-headLength,sinang,cosang));
    x.push_back(xc+RotX(stemWidth,-0.5*length,sinang,cosang));
    y.push_back(yc+RotY(stemWidth,-0.5*length,sinang,cosang));
  }
  SetUp( x, y );
}
/*
std::ostream &operator<<( std::ostream &out, GRA_arrow2 const &p )
{
  out << "<arrow2 width=\"" << p.lineWidth_ << "\" color=\""
      << GRA_colorControl::GetColorCode(p.lineColor_) << "\" fillcolor=\""
      << GRA_colorControl::GetColorCode(p.fillColor_) << "\">\n";
  std::size_t size = p.x_.size();
  out << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size-1; ++i )out << p.x_[i] << " " << p.y_[i] << " ";
  out << p.x_[size-1] << " " << p.y_[size-1] << "</data>\n";
  return out << "</arrow2>\n";
}
*/
// end of file

