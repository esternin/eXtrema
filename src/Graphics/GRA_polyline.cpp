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
#include "GRA_polyline.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_wxWidgets.h"
#include "ExGlobals.h"

void GRA_polyline::CopyStuff( GRA_polyline const &rhs )
{
  x_.assign( rhs.x_.begin(), rhs.x_.end() );
  y_.assign( rhs.y_.begin(), rhs.y_.end() );
  pen_.assign( rhs.pen_.begin(), rhs.pen_.end() );
  lineWidth_ = rhs.lineWidth_;
  lineType_ = rhs.lineType_;
  color_ = rhs.color_;
}

void GRA_polyline::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  wxPen wxpen( dc.GetPen() );
  wxpen.SetColour( ExGlobals::GetwxColor(color_) );
  wxpen.SetWidth( lineWidth_ );
  dc.SetPen( wxpen );
  graphicsOutput->SetLineType( lineType_ );
  //
  std::size_t size = x_.size();
  graphicsOutput->StartLine( x_[0], y_[0] );
  for( std::size_t i=1; i<size; ++i )
  {
    pen_[i]==3 ? graphicsOutput->StartLine(x_[i],y_[i]) :
                 graphicsOutput->ContinueLine(x_[i],y_[i],dc);
  }
}

std::ostream &operator<<( std::ostream &out, GRA_polyline const &p )
{
  std::size_t size = p.x_.size();
  out << "<polyline "
      << "linewidth=\"" << p.lineWidth_ << "\" linetype=\"" << p.lineType_
      << "\" linecolor=\"" << GRA_colorControl::GetColorCode(p.color_) << "\">\n"
      << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size-1; ++i )
    out << p.x_[i] << " " << p.y_[i] << " " << p.pen_[i] << " ";
  out << p.x_[size-1] << " " << p.y_[size-1] << " "
      << p.pen_[size-1] << "</data>\n";
  return out << "</polyline>\n";
}

// end of file
