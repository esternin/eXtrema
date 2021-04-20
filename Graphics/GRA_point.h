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
#ifndef GRA_POINT
#define GRA_POINT

#include <ostream>

#include "GRA_color.h"
#include "GRA_outputType.h"
#include "GRA_drawableObject.h"

class GRA_point : public GRA_drawableObject
{
public:
  GRA_point( double x, double y, GRA_color *color )
      : GRA_drawableObject("POINT"), x_(x), y_(y), color_(color)
  {}

  ~GRA_point()
  {}

  GRA_point( GRA_point const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff( rhs ); }

  GRA_point &operator=( GRA_point const &rhs )
  {
    if( this != &rhs )
    {
      type_ = rhs.type_;
      CopyStuff( rhs );
    }
    return *this;
  }

  void GetXY( double &x, double &y ) const
  { x=x_; y=y_; }

  GRA_color *GetColor() const
  { return color_; }

  void GetLimits( double &xll, double &yll, double &xur, double &yur ) const
  { xll=x_; yll=y_; xur=x_; yur=y_; }

  void Draw( GRA_outputType *graphicsOutput )
  { graphicsOutput->Draw( this ); }

  friend std::ostream &operator<<( std::ostream &, GRA_point const & );

private:
  void CopyStuff( GRA_point const & );
  //
  double x_, y_;
  GRA_color *color_;
};

#endif
