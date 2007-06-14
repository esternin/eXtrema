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
#include "GRA_errorBar.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"

GRA_errorBar::GRA_errorBar( double xc, double yc, double bottom, double top,
                            bool vertical, double footSize, GRA_color *color,
                            int width )
    : GRA_multiLineFigure(),
      bottom_(bottom), top_(top), vertical_(vertical), footSize_(footSize)
{
  SetLineColor( color );
  SetLineWidth( width );
  xc_ = xc;
  yc_ = yc;
  if( vertical )
  {
    if( footSize > 0.0001 )
    {
      x_.push_back( xc-footSize );
      y_.push_back( yc-bottom );
      pen_.push_back( 3 );
      x_.push_back( xc+footSize );
      y_.push_back( yc-bottom );
      pen_.push_back( 2 );
      x_.push_back( xc-footSize );
      y_.push_back( yc+top );
      pen_.push_back( 3 );
      x_.push_back( xc+footSize );
      y_.push_back( yc+top );
      pen_.push_back( 2 );
    }
    x_.push_back( xc );
    y_.push_back( yc-bottom );
    pen_.push_back( 3 );
    x_.push_back( xc );
    y_.push_back( yc+top );
    pen_.push_back( 2 );
  }
  else
  {
    if( footSize > 0.0001 )
    {
      x_.push_back( xc-bottom );
      y_.push_back( yc-footSize );
      pen_.push_back( 3 );
      x_.push_back( xc-bottom );
      y_.push_back( yc+footSize );
      pen_.push_back( 2 );
      x_.push_back( xc+top );
      y_.push_back( yc-footSize );
      pen_.push_back( 3 );
      x_.push_back( xc+top );
      y_.push_back( yc+footSize );
      pen_.push_back( 2 );
    }
    x_.push_back( xc-bottom );
    y_.push_back( yc );
    pen_.push_back( 3 );
    x_.push_back( xc+top );
    y_.push_back( yc );
    pen_.push_back( 2 );
  }
}

std::ostream &operator<<( std::ostream &out, GRA_errorBar const &eb )
{
  return out << "<errorbar x=\"" << eb.xc_ << "\" y=\"" << eb.yc_
             << "\" linewidth=\"" << eb.lineWidth_
             << "\" linecolor=\"" << GRA_colorControl::GetColorCode(eb.lineColor_)
             << "\" bottom=\"" << eb.bottom_ << "\" top=\"" << eb.top_
             << "\" vertical=\"" << eb.vertical_ << "\" footsize=\"" << eb.footSize_
             << "\"/>\n";
}

// end of file

