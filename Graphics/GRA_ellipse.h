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
#ifndef GRA_ELLIPSE
#define GRA_ELLIPSE

#include <ostream>

#include "GRA_outputType.h"
#include "GRA_color.h"
#include "ExString.h"
#include "GRA_shape.h"

class GRA_ellipse : public GRA_shape
{
public:
  GRA_ellipse( double, double, double, double, bool, GRA_color * =0, GRA_color * =0, int =1, int =1 );

  GRA_ellipse( GRA_ellipse const &rhs ) : GRA_shape(rhs)
  { CopyStuff(rhs); }
  
  GRA_ellipse &operator=( GRA_ellipse const &rhs )
  {
    if( this != &rhs )
    {
      GRA_shape::CopyStuff(rhs);
      CopyStuff(rhs);
    }
    return *this;
  }

  ~GRA_ellipse()
  {}
  
  void SetCircle( bool b )
  { circle_ = b; }
  
  bool IsaCircle() const
  { return circle_; }
  
  double GetMajorAxis() const
  { return majorAxis_; }
  
  double GetMinorAxis() const
  { return minorAxis_; }
  
  void Draw( GRA_outputType *graphicsOutput )
  { graphicsOutput->Draw( this ); }

  friend std::ostream &operator<<( std::ostream &, GRA_ellipse const & );

protected:
  void CopyStuff( GRA_ellipse const & );
  //
  bool circle_;
  double minorAxis_, majorAxis_;
};
#endif
