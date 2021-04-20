/*
Copyright (C) 2005,...,2008 Joseph L. Chuma, TRIUMF

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
#ifndef GRA_POLYLINE
#define GRA_POLYLINE

#include <ostream>
#include <vector>

#include "GRA_color.h"
#include "GRA_outputType.h"

#include "GRA_drawableObject.h"

class GRA_polyline : public GRA_drawableObject
{
public:
  GRA_polyline( double x, double y, int lw, int lt, GRA_color *color )
      : GRA_drawableObject("POLYLINE"), lineWidth_(lw), lineType_(lt), color_(color)
  {
    x_.push_back( x );
    y_.push_back( y );
    pen_.push_back( 3 );
  }

  GRA_polyline( std::vector<double> const &x, std::vector<double> const &y,
                std::vector<int> const &pen, int lw, int lt, GRA_color *color )
      : GRA_drawableObject("POLYLINE"), lineWidth_(lw), lineType_(lt), color_(color)
  {
    x_.assign(x.begin(),x.end());
    y_.assign(y.begin(),y.end());
    pen_.assign( pen.begin(), pen.end() );
  }

  ~GRA_polyline()
  {}

  GRA_polyline( GRA_polyline const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff( rhs ); }

  GRA_polyline &operator=( GRA_polyline const &rhs )
  {
    if( this != &rhs )
    {
      type_ = rhs.type_;
      CopyStuff( rhs );
    }
    return *this;
  }

  std::vector<double> const &GetX() const
  { return x_; }

  std::vector<double> const &GetY() const
  { return y_; }

  std::vector<int> const &GetPen() const
  { return pen_; }

  int GetLineWidth() const
  { return lineWidth_; }

  int GetLineType() const
  { return lineType_; }

  GRA_color *GetColor() const
  { return color_; }

  void SetColor( GRA_color *c )
  { color_ = c; }
  
  void Add( double x, double y, int pen )
  {
    x_.push_back( x );
    y_.push_back( y );
    pen_.push_back( pen );
  }

  void Draw( GRA_outputType *graphicsOutput )
  { graphicsOutput->Draw( this ); }

  friend std::ostream &operator<<( std::ostream &, GRA_polyline const & );

private:
  void CopyStuff( GRA_polyline const & );
  //
  std::vector<double> x_, y_;
  std::vector<int> pen_;
  int lineWidth_, lineType_;
  GRA_color *color_;
};

#endif
