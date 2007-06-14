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
#ifndef GRA_POLYGON
#define GRA_POLYGON

#include <vector>
#include <ostream>

#include "wx/wx.h"

#include "GRA_shape.h"

class GRA_wxWidgets;
class GRA_color;

class GRA_polygon : public GRA_shape
{
public:
  GRA_polygon( GRA_color *lineColor =0, GRA_color *fillColor =0,
               int lineWidth =1, int lineType =1 )
      : GRA_shape(lineColor,fillColor,lineWidth,lineType,wxT("POLYGON"))
  {}

  GRA_polygon(  std::vector<double> const &x, std::vector<double> const &y,
                GRA_color *lineColor =0, GRA_color *fillColor =0,
                int lineWidth =1, int lineType =1 )
      : GRA_shape(lineColor,fillColor,lineWidth,lineType,wxT("POLYGON"))
  { SetUp( x, y ); }

  virtual ~GRA_polygon()
  {}
  
  GRA_polygon( GRA_polygon const &rhs ) : GRA_shape(rhs)
  { CopyStuff( rhs ); }
  
  GRA_polygon &operator=( GRA_polygon const &rhs )
  {
    if( this != &rhs )
    {
      GRA_shape::CopyStuff(rhs);
      CopyStuff(rhs);
    }
    return *this;
  }

  void SetUp( std::vector<double> const &, std::vector<double> const & );
  
  void GetCoordinates( std::vector<double> &x, std::vector<double> &y ) const
  {
    x.assign( x_.begin(), x_.end() );
    y.assign( y_.begin(), y_.end() );
  }

  void DoClipping( double, double, double, double, std::vector<double> &, std::vector<double> & );

  virtual void Draw( GRA_wxWidgets *, wxDC & );

  friend std::ostream &operator<<( std::ostream &, GRA_polygon const & );

protected:
  void CopyStuff( GRA_polygon const &rhs );
  //
  int GetQuadrant( double, double, double, double, double, double );
  int GetEdge( double, double, double, double, double, double );
  //
  std::vector<double> x_, y_;
};
#endif
