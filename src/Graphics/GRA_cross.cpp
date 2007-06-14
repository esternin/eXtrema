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
#include <cmath>

#include "GRA_cross.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"

GRA_cross::~GRA_cross()
{}

GRA_cross::GRA_cross( double xc, double yc, double side, double angle,
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
  x_.push_back(xc+RotX(temp,-temp,sinang,cosang));
  y_.push_back(yc+RotY(temp,-temp,sinang,cosang));
  pen_.push_back(3);
  x_.push_back(xc+RotX(-temp,temp,sinang,cosang));
  y_.push_back(yc+RotY(-temp,temp,sinang,cosang));
  pen_.push_back(2);
  x_.push_back(xc+RotX(-temp,-temp,sinang,cosang));
  y_.push_back(yc+RotY(-temp,-temp,sinang,cosang));
  pen_.push_back(3);
  x_.push_back(xc+RotX(temp,temp,sinang,cosang));
  y_.push_back(yc+RotY(temp,temp,sinang,cosang));
  pen_.push_back(2);
}

std::ostream &operator<<( std::ostream &out, GRA_cross const &mlf )
{
  out << wxT("<cross xc=\"") << mlf.xc_ << wxT("\" yc=\"") << mlf.yc_
      << wxT("\" xmin=\"") << mlf.xmin_ << wxT("\" ymin=\"") << mlf.ymin_
      << wxT("\" xmax=\"") << mlf.xmax_ << wxT("\" ymax=\"") << mlf.ymax_
      << wxT("\" linewidth=\"") << mlf.lineWidth_
      << wxT("\" color=\"") << GRA_colorControl::GetColorCode(mlf.lineColor_) << wxT("\">\n");
  std::size_t size = mlf.x_.size();
  out << wxT("<data size=\"") << size << wxT("\">");
  for( std::size_t i=0; i<size-1; ++i )out << mlf.x_[i] << wxT(" ") << mlf.y_[i] << wxT(" ") << mlf.pen_[i] << wxT(" ");
  out << mlf.x_[size-1] << wxT(" ") << mlf.y_[size-1] << wxT(" ") << mlf.pen_[size-1] << wxT("</data>\n");
  return out << wxT("</cross>\n");
}

// end of file

