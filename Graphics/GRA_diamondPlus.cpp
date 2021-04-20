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

#include "GRA_diamondPlus.h"

GRA_diamondPlus::~GRA_diamondPlus()
{}

GRA_diamondPlus::GRA_diamondPlus( double xc, double yc, double side, double angle,
                                  GRA_color *lineColor, int lineWidth )
  : GRA_multiLineFigure()
{
  SetLineColor( lineColor );
  SetLineWidth( lineWidth );
  xc_ = xc;
  yc_ = yc;
  double const cosang = cos(angle*M_PI/180.);
  double const sinang = sin(angle*M_PI/180.);
  double const temp = side*0.5;
  double const temp2 = side*0.4;
  x_.push_back(xc+RotX(temp2,0.,sinang,cosang));
  y_.push_back(yc+RotY(temp2,0.,sinang,cosang));
  pen_.push_back(3);
  x_.push_back(xc+RotX(0.,temp,sinang,cosang));
  y_.push_back(yc+RotY(0.,temp,sinang,cosang));
  pen_.push_back(2);
  x_.push_back(xc+RotX(-temp2,0.,sinang,cosang));
  y_.push_back(yc+RotY(-temp2,0.,sinang,cosang));
  pen_.push_back(2);
  x_.push_back(xc+RotX(0.,-temp,sinang,cosang));
  y_.push_back(yc+RotY(0.,-temp,sinang,cosang));
  pen_.push_back(2);
  x_.push_back(xc+RotX(temp2,0.,sinang,cosang));
  y_.push_back(yc+RotY(temp2,0.,sinang,cosang));
  pen_.push_back(2);
  x_.push_back(xc+RotX(temp2,0.,sinang,cosang));
  y_.push_back(yc+RotY(temp2,0.,sinang,cosang));
  pen_.push_back(3);
  x_.push_back(xc+RotX(-temp2,0.,sinang,cosang));
  y_.push_back(yc+RotY(-temp2,0.,sinang,cosang));
  pen_.push_back(2);
  x_.push_back(xc+RotX(0.,temp,sinang,cosang));
  y_.push_back(yc+RotY(0.,temp,sinang,cosang));
  pen_.push_back(3);
  x_.push_back(xc+RotX(0.,-temp,sinang,cosang));
  y_.push_back(yc+RotY(0.,-temp,sinang,cosang));
  pen_.push_back(2);
}

std::ostream &operator<<( std::ostream &out, GRA_diamondPlus const &mlf )
{
  out << "<diamondplus xc=\"" << mlf.xc_ << "\" yc=\"" << mlf.yc_
      << "\" xmin=\"" << mlf.xmin_ << "\" ymin=\"" << mlf.ymin_
      << "\" xmax=\"" << mlf.xmax_ << "\" ymax=\"" << mlf.ymax_
      << "\" linewidth=\"" << mlf.lineWidth_
      << "\" color=\"" << GRA_colorControl::GetColorCode(mlf.lineColor_) << "\">\n";
  std::size_t size = mlf.x_.size();
  out << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size-1; ++i )out << mlf.x_[i] << " " << mlf.y_[i] << " " << mlf.pen_[i] << " ";
  out << mlf.x_[size-1] << " " << mlf.y_[size-1] << " " << mlf.pen_[size-1] << "</data>\n";
  return out << "</diamondplus>\n";
}

// end of file

