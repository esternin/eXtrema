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

#include "GRA_arrow3.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"

GRA_arrow3::GRA_arrow3( double xc, double yc, double side, double offset, double angle,
                        GRA_color *lineColor, int lineWidth )
  : GRA_multiLineFigure()
{
  SetArrow3();
  SetLineColor( lineColor );
  SetLineWidth( lineWidth );
  xc_ = xc;
  yc_ = yc;
  double const cosang = cos(angle*M_PI/180.);
  double const sinang = sin(angle*M_PI/180.);
  double const tan20 = 0.36397023400;
  x_.push_back(xc+RotX(0.,-offset,sinang,cosang));
  y_.push_back(yc+RotY(0.,-offset,sinang,cosang));
  pen_.push_back(3);
  x_.push_back(xc+RotX(0.,side-offset,sinang,cosang));
  y_.push_back(yc+RotY(0.,side-offset,sinang,cosang));
  pen_.push_back(2);
  x_.push_back(xc+RotX(-0.2*side*tan20,0.8*(side-offset),sinang,cosang));
  y_.push_back(yc+RotY(-0.2*side*tan20,0.8*(side-offset),sinang,cosang));
  pen_.push_back(2);
  x_.push_back(xc+RotX(0.2*side*tan20,0.8*(side-offset),sinang,cosang));
  y_.push_back(yc+RotY(0.2*side*tan20,0.8*(side-offset),sinang,cosang));
  pen_.push_back(2);
  x_.push_back(xc+RotX(0.,side-offset,sinang,cosang));
  y_.push_back(yc+RotY(0.,side-offset,sinang,cosang));
  pen_.push_back(2);
}

GRA_arrow3::GRA_arrow3( double x1, double y1, double x2, double y2, bool heads,
                        GRA_color *lineColor, int lineWidth, double widthFactor, double lengthFactor )
  : GRA_multiLineFigure(), headsBothEnds_(heads)
{
  SetArrow3();
  SetLineColor( lineColor );
  SetLineWidth( lineWidth );
  double const dx = x2 - x1;
  double const dy = y2 - y1;
  double const angle = atan2(dy,dx)+0.5*M_PI;
  double const cosang = cos(angle);
  double const sinang = sin(angle);
  double const length = sqrt(dx*dx+dy*dy);
  //double const headWidth = 0.3;
  //double const headLength = 0.3;
  double const temp = length - 15.;
  double headLength = 0.2+(temp/5.)*(temp/5.)*exp(-temp*temp/25.);
  double headWidth = 0.75*headLength;
  headWidth *= widthFactor;
  headLength *= lengthFactor;
  xc_ = 0.5*(x1+x2);
  yc_ = 0.5*(y1+y2);
  double const tan30 = tan(30.*M_PI/180.);
  if( headsBothEnds_ )
  {
    x_.push_back(xc_+RotX(-headWidth*tan30,-0.5*length+headLength,sinang,cosang));
    y_.push_back(yc_+RotY(-headWidth*tan30,-0.5*length+headLength,sinang,cosang));
    pen_.push_back(3);
    x_.push_back(xc_+RotX(0.0,-0.5*length,sinang,cosang));
    y_.push_back(yc_+RotY(0.0,-0.5*length,sinang,cosang));
    pen_.push_back(2);
    x_.push_back(xc_+RotX(headWidth*tan30,-0.5*length+headLength,sinang,cosang));
    y_.push_back(yc_+RotY(headWidth*tan30,-0.5*length+headLength,sinang,cosang));
    pen_.push_back(2);
    x_.push_back(xc_+RotX(0.0,-0.5*length,sinang,cosang));
    y_.push_back(yc_+RotY(0.0,-0.5*length,sinang,cosang));
    pen_.push_back(3);
    x_.push_back(xc_+RotX(0.0,0.5*length,sinang,cosang));
    y_.push_back(yc_+RotY(0.0,0.5*length,sinang,cosang));
    pen_.push_back(2);
    x_.push_back(xc_+RotX(-headWidth*tan30,0.5*length-headLength,sinang,cosang));
    y_.push_back(yc_+RotY(-headWidth*tan30,0.5*length-headLength,sinang,cosang));
    pen_.push_back(3);
    x_.push_back(xc_+RotX(0.0,0.5*length,sinang,cosang));
    y_.push_back(yc_+RotY(0.0,0.5*length,sinang,cosang));
    pen_.push_back(2);
    x_.push_back(xc_+RotX(headWidth*tan30,0.5*length-headLength,sinang,cosang));
    y_.push_back(yc_+RotY(headWidth*tan30,0.5*length-headLength,sinang,cosang));
    pen_.push_back(2);
  }
  else
  {
    x_.push_back(xc_+RotX(0.0,-0.5*length,sinang,cosang));
    y_.push_back(yc_+RotY(0.0,-0.5*length,sinang,cosang));
    pen_.push_back(3);
    x_.push_back(xc_+RotX(0.0,0.5*length,sinang,cosang));
    y_.push_back(yc_+RotY(0.0,0.5*length,sinang,cosang));
    pen_.push_back(2);
    x_.push_back(xc_+RotX(-headWidth*tan30,0.5*length-headLength,sinang,cosang));
    y_.push_back(yc_+RotY(-headWidth*tan30,0.5*length-headLength,sinang,cosang));
    pen_.push_back(3);
    x_.push_back(xc_+RotX(0.0,0.5*length,sinang,cosang));
    y_.push_back(yc_+RotY(0.0,0.5*length,sinang,cosang));
    pen_.push_back(2);
    x_.push_back(xc_+RotX(headWidth*tan30,0.5*length-headLength,sinang,cosang));
    y_.push_back(yc_+RotY(headWidth*tan30,0.5*length-headLength,sinang,cosang));
    pen_.push_back(2);
  }
}
/*
std::ostream &operator<<( std::ostream &out, GRA_arrow3 const &mlf )
{
  out << "<arrow3 linewidth=\"" << mlf.lineWidth_
      << "\" color=\"" << GRA_colorControl::GetColorCode(mlf.lineColor_) << "\">\n";
  std::size_t size = mlf.x_.size();
  out << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size-1; ++i )out << mlf.x_[i] << " " << mlf.y_[i] << " " << mlf.pen_[i] << " ";
  out << mlf.x_[size-1] << " " << mlf.y_[size-1] << " " << mlf.pen_[size-1] << "</data>\n";
  return out << "</arrow3>\n";
}
*/
// end of file

