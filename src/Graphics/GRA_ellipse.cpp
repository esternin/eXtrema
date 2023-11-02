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
#include "GRA_ellipse.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_wxWidgets.h"
#include "ExGlobals.h"

GRA_ellipse::GRA_ellipse( double xmin, double ymin, double xmax, double ymax, bool circle,
                          GRA_color *lineColor, GRA_color *fillColor, int lineWidth, int lineType )
    : GRA_shape(lineColor,fillColor,lineWidth,lineType,wxT("ELLIPSE"))
{
  xmin_ = xmin;
  ymin_ = ymin;
  xmax_ = xmax;
  ymax_ = ymax;
  circle_ = circle;
  if( xmin > xmax )
  {
    xmin_ = xmax;
    xmax_ = xmin;
  }
  if( ymin > ymax )
  {
    ymax_ = ymin;
    ymin_ = ymax;
  }
  double dx = xmax_ - xmin_;
  double dy = ymax_ - ymin_;
  if( circle_ )
  {
    if( dy > dx )
    {
      ymax_ = ymin_ + dx;
      dy = dx;
    }
    else if( dy < dx )
    {
      xmax_ = xmin_ + dy;
      dx = dy;
    }
    majorAxis_ = dx;
    minorAxis_ = dx;
  }
  else
  {
    majorAxis_ = std::max(dx,dy);
    minorAxis_ = std::min(dx,dy);
  }
  xc_ = 0.5*(xmax_+xmin_);
  yc_ = 0.5*(ymax_+ymin_);
}

void GRA_ellipse::CopyStuff( GRA_ellipse const &rhs )
{
  xmin_ = rhs.xmin_;
  ymin_ = rhs.ymin_;
  xmax_ = rhs.xmax_;
  ymax_ = rhs.ymax_;
  circle_ = rhs.circle_;
  minorAxis_ = rhs.minorAxis_;
  majorAxis_ = rhs.majorAxis_;
}

void GRA_ellipse::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  int xll, yll, xur, yur;
  graphicsOutput->WorldToOutputType( xmin_, ymin_, xll, yll );
  graphicsOutput->WorldToOutputType( xmax_, ymax_, xur, yur );

  // With wxWidgets 3.0 we need to ensure that width and height are positive
  // and that we really pass the coordinates of the upper left corner to
  // DrawEllipse(), so do it as long as we support it, even if it's not
  // necessary with the later versions.
  if ( xll > xur )
    std::swap(xll, xur);
  if ( yll > yur )
    std::swap(yll, yur);

  int width = xur - xll;
  int height = yur - yll;
  // wxBrush controls must come before wxPen controls
  wxBrush wxbrush( dc.GetBrush() );
  if( fillColor_ )
  {
    wxbrush.SetColour( ExGlobals::GetwxColor(fillColor_) );
    wxbrush.SetStyle( wxBRUSHSTYLE_SOLID );
  } else {
    wxbrush.SetColour( wxTransparentColour );
    wxbrush.SetStyle( wxBRUSHSTYLE_TRANSPARENT );
  }
  dc.SetBrush( wxbrush );
  //
  wxPen wxpen( dc.GetPen() );
  wxpen.SetWidth( std::max(1,lineWidth_) );
  wxpen.SetColour( ExGlobals::GetwxColor(lineColor_) );
  dc.SetPen( wxpen );
  //
  dc.DrawEllipse( static_cast<int>(xll+0.5), static_cast<int>(yll+0.5), width, height );
}

std::ostream &operator<<( std::ostream &out, GRA_ellipse const &ellipse )
{
  return out << "<ellipse xmin=\"" << ellipse.xmin_ << "\" ymin=\"" << ellipse.ymin_
             << "\" xmax=\"" << ellipse.xmax_ << "\" ymax=\"" << ellipse.ymax_
             << "\" circle=\"" << ellipse.circle_
             << "\" linecolor=\"" << GRA_colorControl::GetColorCode(ellipse.lineColor_)
             << "\" fillcolor=\"" << GRA_colorControl::GetColorCode(ellipse.fillColor_)
             << "\" linewidth=\"" << ellipse.lineWidth_
             << "\" linetype=\"" << ellipse.lineType_ << "\"/>\n";
}

// end of file
