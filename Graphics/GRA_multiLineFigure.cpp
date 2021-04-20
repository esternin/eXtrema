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

#include "GRA_multiLineFigure.h"

GRA_multiLineFigure::GRA_multiLineFigure(  std::vector<double> const &x,
                                           std::vector<double> const &y,
                                           std::vector<int> const &pen,
                                           GRA_color *lineColor, int lineWidth, int lineType )
    : GRA_shape(lineColor,0,lineWidth,lineType,"MULTILINEFIGURE")
{
  std::size_t xSize = x.size();
  std::size_t ySize = y.size();
  std::size_t pSize = pen.size();
  std::size_t size = std::min( std::min(xSize,ySize), pSize );
  x_.assign( x.begin(), x.end() );
  y_.assign( y.begin(), y.end() );
  pen_.assign( pen.begin(), pen.end() );
  xmin_ = x_[0];
  xmax_ = x_[0];
  ymin_ = y_[0];
  ymax_ = y_[0];
  for( std::size_t i=1; i<size; ++i )
  {
    if( x_[i] < xmin_ )xmin_ = x_[i];
    if( x_[i] > xmax_ )xmax_ = x_[i];
    if( y_[i] < ymin_ )ymin_ = y_[i];
    if( y_[i] > ymax_ )ymax_ = y_[i];
  }
  xc_ = (xmax_+xmin_)/2.0;
  yc_ = (ymax_+ymin_)/2.0;
}

void GRA_multiLineFigure::CopyStuff( GRA_multiLineFigure const &rhs )
{
  x_.assign( rhs.x_.begin(), rhs.x_.end() );
  y_.assign( rhs.y_.begin(), rhs.y_.end() );
  pen_.assign( rhs.pen_.begin(), rhs.pen_.end() );
}

std::ostream &operator<<( std::ostream &out, GRA_multiLineFigure const &mlf )
{
  out << "<multilinefigure linewidth=\"" << mlf.lineWidth_ << "\" linetype=\""
      << mlf.lineType_ << "\" linecolor=\""
      << GRA_colorControl::GetColorCode(mlf.lineColor_) << "\">\n";
  std::size_t size = mlf.x_.size();
  out << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << mlf.x_[i] << " " << mlf.y_[i] << " " << mlf.pen_[i] << " ";
  return out << "</data>\n</multilinefigure>\n";
}

// end of file
