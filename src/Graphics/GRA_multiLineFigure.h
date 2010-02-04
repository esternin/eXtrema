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
#ifndef GRA_MULTILINEFIGURE
#define GRA_MULTILINEFIGURE

#include <vector>
#include <ostream>

#include "wx/wx.h"

#include "GRA_shape.h"

class GRA_color;
class GRA_wxWidgets;

class GRA_multiLineFigure : public GRA_shape
{
protected:
  enum MultiType { UNKNOWN, ARROW3 };

public:
  GRA_multiLineFigure() : GRA_shape(0,0,0,1,wxT("MULTILINEFIGURE"))
  {}

  GRA_multiLineFigure( std::vector<double> const &,
                       std::vector<double> const &,
                       std::vector<int> const &, GRA_color * =0, int =1, int =1 );

  ~GRA_multiLineFigure()
  {}

  GRA_multiLineFigure( GRA_multiLineFigure const &rhs ) : GRA_shape(rhs)
  { CopyStuff(rhs); }
  
  GRA_multiLineFigure &operator=( GRA_multiLineFigure const &rhs )
  {
    if( this != &rhs )
    {
      GRA_shape::CopyStuff(rhs);
      CopyStuff(rhs);
    }
    return *this;
  }
  
  void GetCoordinates( std::vector<double> &x, std::vector<double> &y, std::vector<int> &pen )
  {
    x.assign( x_.begin(), x_.end() );
    y.assign( y_.begin(), y_.end() );
    pen.assign( pen_.begin(), pen_.end() );
  }
  
  void Draw( GRA_wxWidgets *, wxDC & );

  bool IsaArrow3() const
  { return (type_ == ARROW3); }
  
  void SetArrow3()
  { type_ = ARROW3; }
  
  friend std::ostream &operator<<( std::ostream &, GRA_multiLineFigure const & );

protected:
  void CopyStuff( GRA_multiLineFigure const & );
  //
  std::vector<double> x_, y_;
  std::vector<int> pen_;
  MultiType type_;
};
#endif
 
