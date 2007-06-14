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
#ifndef GRA_BITMAP
#define GRA_BITMAP

#include <vector>

#include "wx/wx.h"

class GRA_color;
class GRA_wxWidgets;

class GRA_bitmap
{
protected:
  typedef std::vector<GRA_color*> colorVec;

public:
  GRA_bitmap( double, double, double, double, GRA_color *, GRA_wxWidgets * );

  virtual ~GRA_bitmap()
  {}

  GRA_bitmap( GRA_bitmap const &rhs )
  { CopyStuff( rhs ); }

  GRA_bitmap &operator=( GRA_bitmap const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }

  colorVec &GetBitmap()
  { return bitmap_; }

  void SetBorder( GRA_color * );

  void SetPixelColor( int i, int j, GRA_color *color )
  { bitmap_[i+j*width_] = color; }

  void GetLimits( double &xlo, double &ylo, double &xhi, double &yhi ) const
  {
    xlo = xLo_;
    ylo = yLo_;
    xhi = xHi_;
    yhi = yHi_;
  }

  void GetLimits( int &xlo, int &ylo, int &xhi, int &yhi ) const
  {
    xlo = ixLo_;
    ylo = iyLo_;
    xhi = ixHi_;
    yhi = iyHi_;
  }

  int GetWidth() const
  { return width_; }

  int GetHeight() const
  { return height_; }

  void Draw( GRA_wxWidgets *, wxDC & );

protected:
  double xLo_, xHi_, yLo_, yHi_, xInc_, yInc_;
  int ixLo_, ixHi_, iyLo_, iyHi_, width_, height_;
  colorVec bitmap_;

  void CopyStuff( GRA_bitmap const & );
};

#endif
