/*
Copyright (C) 2010 Joseph L. Chuma

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

#include "GRA_polygon.h"
#include "GRA_colorControl.h"
#include "GRA_wxWidgets.h"
#include "UsefulFunctions.h"
#include "ExGlobals.h"

void GRA_polygon::SetUp( std::vector<double> const &x, std::vector<double> const &y )
{
  std::size_t xSize = x.size();
  std::size_t ySize = y.size();
  std::size_t size = std::min( xSize, ySize );
  x_.assign( x.begin(), x.end() );
  y_.assign( y.begin(), y.end() );
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
  xc_ = (xmax_+xmin_)*0.5;
  yc_ = (ymax_+ymin_)*0.5;
}

void GRA_polygon::SetUp( double xc, double yc, double angle, double radius, int vertices )
{
  xc_ = xc;
  yc_ = yc;
  double angleInc = 360./vertices;
  for( int i=0; i<vertices; ++i )
  {
    x_.push_back( radius*cos(M_PI/180.*(angle-i*angleInc)) + xc_ );
    y_.push_back( radius*sin(M_PI/180.*(angle-i*angleInc)) + yc_ );
  }
  xmin_ = x_[0];
  xmax_ = x_[0];
  ymin_ = y_[0];
  ymax_ = y_[0];
  for( std::size_t i=1; i<vertices; ++i )
  {
    if( x_[i] < xmin_ )xmin_ = x_[i];
    if( x_[i] > xmax_ )xmax_ = x_[i];
    if( y_[i] < ymin_ )ymin_ = y_[i];
    if( y_[i] > ymax_ )ymax_ = y_[i];
  }
}

void GRA_polygon::CopyStuff( GRA_polygon const &rhs )
{
  x_.assign( rhs.x_.begin(), rhs.x_.end() );
  y_.assign( rhs.y_.begin(), rhs.y_.end() );
  xmin_ = rhs.xmin_;
  xmax_ = rhs.xmax_;
  ymin_ = rhs.ymin_;
  ymax_ = rhs.ymax_;
  xc_ = rhs.xc_;
  yc_ = rhs.yc_;
  type_ = rhs.type_;
}

int GRA_polygon::GetQuadrant( double x, double y,
                              double xmin, double ymin, double xmax, double ymax )
{
  //         |         |
  //      3  |    4    |   5
  //         |         |
  //   ------+---------+------- ymax
  //         |         |
  //      2  |    0    |   6
  //         |         |
  //   ------+---------+------- ymin
  //         |         |
  //      1  |    8    |   7
  //         |         |
  //       xmin      xmax

  int quadrant = 8;
  if( x>=xmin && x<=xmax && y>=ymin && y<=ymax )quadrant=0;
  else if( x<xmin && y<ymin )quadrant = 1;
  else if( x<xmin && y>=ymin && y<=ymax )quadrant = 2;
  else if( x<xmin && y>ymax )quadrant = 3;
  else if( x>=xmin && x<=xmax && y>ymax )quadrant = 4;
  else if( x>xmax && y>ymax )quadrant = 5;
  else if( x>xmax && y>=ymin && y<=ymax )quadrant = 6;
  else if( x>xmax && y<ymin )quadrant = 7;
  return quadrant;
}

int GRA_polygon::GetEdge( double x, double y,
                          double xmin, double ymin, double xmax, double ymax )
{
  //       |         |
  //       |         |
  //   ----+----3----+---- ymax
  //       |         |
  //       4         2
  //       |         |
  //   ----+----1----+---- ymin
  //       |         |
  //       |         |
  //     xmin      xmax

  double const eps = 0.0001;
  int edge = 1;
  if( x>=xmin && x<=xmax && fabs(y-ymin)<eps )edge = 1;
  else if( fabs(xmax-x)<eps && y>=ymin && y<=ymax )edge = 2;
  else if( x>=xmin && x<=xmax && fabs(ymax-y)<eps )edge = 3;
  else if( fabs(x-xmin)<eps && y>=ymin && y<=ymax )edge = 4;
  return edge;
}

void GRA_polygon::DoClipping( double xmin, double ymin, double xmax, double ymax,
                              std::vector<double> &x, std::vector<double> &y )
{
  std::size_t size = x_.size();
  x.reserve( size );
  y.reserve( size );
  int edgeOut = 0;
  int quadrant = GetQuadrant( x_[0], y_[0], xmin, ymin, xmax, ymax );
  if( quadrant == 0 )
  {
    x.push_back( x_[0] );
    y.push_back( y_[0] );
  }
  else if( quadrant==1 || quadrant==8 || quadrant==7 )edgeOut = 1;
  else if( quadrant == 6 )edgeOut = 2;
  else if( quadrant==3 || quadrant==4 || quadrant==5 )edgeOut = 3;
  else if( quadrant == 2 )edgeOut = 4;
  int edgeInit = edgeOut;
  for( std::size_t i=1; i<size; ++i )
  {
    int previousQuadrant = quadrant;
    quadrant = GetQuadrant( x_[i], y_[i], xmin, ymin, xmax, ymax );
    if( quadrant==0 && previousQuadrant==0 )
    {
      // (x[i],y[i]) and (x[i-1],y[i-1]) are both inside the clipping boundary
      //
      x.push_back( x_[i] );
      y.push_back( y_[i] );
    }
    else
    {
      int ndraw = 1;
      double xdraw[2], ydraw[2];
      xdraw[0] = x_[i];
      ydraw[0] = y_[i];
      UsefulFunctions::WindowClip( x_[i-1], y_[i-1], x_[i], y_[i],
                                   xmin, xmax, ymin, ymax,
                                   xdraw, ydraw, ndraw );
      switch (ndraw)
      {
        case 0:
        {
          if( i == size-1 ) // first and last points both outside
          {
            if( (edgeOut==2 && edgeInit==3) || (edgeOut==3 && edgeInit==2) )
            {
              x.push_back( xmax );
              y.push_back( ymax );
            }
            else if( (edgeOut==2 && edgeInit==1) || (edgeOut==1 && edgeInit==2) )
            {
              x.push_back( xmax );
              y.push_back( ymin );
            }
            else if( (edgeOut==3 && edgeInit==4) || (edgeOut==4 && edgeInit==3) )
            {
              x.push_back( xmin );
              y.push_back( ymax );
            }
            else if( (edgeOut==1 && edgeInit==4) || (edgeOut==4 && edgeInit==1) )
            {
              x.push_back( xmin );
              y.push_back( ymin );
            }
          }
          break;
        }
        case 1:   //(x[i],y[i]) is outside but (x[i-1],y[i-1]) is inside
        {
          edgeOut = GetEdge( xdraw[0], ydraw[0], xmin, ymin, xmax, ymax );
          x.push_back( xdraw[0] );
          y.push_back( ydraw[0] );
          break;
        }
        case 2:   // (x[i],y[i]) and (x[i-1],y[i-1]) are both outside
        {
          int edgeIn = GetEdge( xdraw[0], ydraw[0], xmin, ymin, xmax, ymax );
          if( (edgeOut==2 && edgeIn==3) || (edgeOut==3 && edgeIn==2) )
          {
            x.push_back( xmax );
            y.push_back( ymax );
          }
          else if( (edgeOut==2 && edgeIn==1) || (edgeOut==1 && edgeIn==2) )
          {
            x.push_back( xmax );
            y.push_back( ymin );
          }
          else if( (edgeOut==3 && edgeIn==4) || (edgeOut==4 && edgeIn==3) )
          {
            x.push_back( xmin );
            y.push_back( ymax );
          }
          else if( (edgeOut==1 && edgeIn==4) || (edgeOut==4 && edgeIn==1) )
          {
            x.push_back( xmin );
            y.push_back( ymin );
          }
          x.push_back( xdraw[0] );
          y.push_back( ydraw[0] );
          x.push_back( xdraw[1] );
          y.push_back( ydraw[1] );
          edgeOut = GetEdge( xdraw[1], ydraw[1], xmin, ymin, xmax, ymax );
          break;
        }
        case 3:   // (x[i-1],y[i-1]) is outside but (x[i],y[i]) is inside
        {
          int edgeIn = GetEdge( xdraw[0], ydraw[0], xmin, ymin, xmax, ymax );
          if( (edgeOut==2 && edgeIn==3) || (edgeOut==3 && edgeIn==2) )
          {
            x.push_back( xmax );
            y.push_back( ymax );
          }
          else if( (edgeOut==2 && edgeIn==1) || (edgeOut==1 && edgeIn==2) )
          {
            x.push_back( xmax );
            y.push_back( ymin );
          }
          else if( (edgeOut==3 && edgeIn==4) || (edgeOut==4 && edgeIn==3) )
          {
            x.push_back( xmin );
            y.push_back( ymax );
          }
          else if( (edgeOut==1 && edgeIn==4) || (edgeOut==4 && edgeIn==1) )
          {
            x.push_back( xmin );
            y.push_back( ymin );
          }
          x.push_back( xdraw[0] );
          y.push_back( ydraw[0] );
          x.push_back( x_[i] );
          y.push_back( y_[i] );
        }
      }
    }
  }
}

void GRA_polygon::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  //
  std::vector<double> x, y;
  DoClipping( xmin, ymin, xmax, ymax, x, y );
  //
  std::size_t const size = x.size();
  wxPoint *pArray = new wxPoint[size];
  for( std::size_t i=0; i<size; ++i )
  {
    int ix, iy;
    graphicsOutput->WorldToOutputType( x[i], y[i], ix, iy );
    pArray[i] = wxPoint( ix, iy );
  }
  wxPen wxpen( dc.GetPen() );
  wxpen.SetWidth( std::max(1,lineWidth_) );
  wxpen.SetColour( ExGlobals::GetwxColor(lineColor_) );
  dc.SetPen( wxpen );
  wxBrush wxbrush( dc.GetBrush() );
  if( fillColor_ )
  {
    wxbrush.SetColour( ExGlobals::GetwxColor(fillColor_) );
    wxbrush.SetStyle( wxSOLID );
    dc.SetBrush( wxbrush );
    dc.DrawPolygon( static_cast<int>(size), pArray );
  }
  wxbrush.SetStyle( wxTRANSPARENT );
  dc.SetBrush( wxbrush );
  dc.DrawPolygon( static_cast<int>(size), pArray );
  delete [] pArray;
}

std::ostream &operator<<( std::ostream &out, GRA_polygon const &p )
{
  out << "<polygon linewidth=\"" << p.lineWidth_ << "\" linetype=\""
      << p.lineType_ << "\" linecolor=\""
      << GRA_colorControl::GetColorCode(p.lineColor_) << "\" fillcolor=\""
      << GRA_colorControl::GetColorCode(p.fillColor_) << "\">\n";
  std::size_t size = p.x_.size();
  out << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << p.x_[i] << " " << p.y_[i] << " ";
  return out << "</data>\n</polygon>\n";
}

// end of file

