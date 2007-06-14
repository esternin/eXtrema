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
#ifndef GRA_COLOR
#define GRA_COLOR

#include <ostream>

#include "wx/wx.h"

class GRA_color
{
public:
  GRA_color( int r, int g, int b ) : r_(r), g_(g), b_(b)
  {}
  
  GRA_color( wxString &name, int r, int g, int b )
      : name_(name), r_(r), g_(g), b_(b)
  {}
  
  GRA_color( wxChar const *name, int r, int g, int b )
      : name_(name), r_(r), g_(g), b_(b)
  {}

  virtual ~GRA_color()
  {}
  
  GRA_color( GRA_color const &rhs )
  { CopyStuff( rhs ); }
  
  GRA_color &operator=( GRA_color const &rhs )
  {
    if( &rhs != this )CopyStuff( rhs );
    return *this;
  }
  
  bool operator==( GRA_color const &rhs ) const
  { return ( r_==rhs.r_ && g_==rhs.g_ && b_==rhs.b_ ); }

  void GetRGB( int &r, int &g, int &b ) const
  {
    r = r_;
    g = g_;
    b = b_;
  }

  wxString GetName() const
  { return name_; }

  bool IsNamed() const
  { return !name_.empty(); }

  friend std::ostream &operator<<( std::ostream &, GRA_color const & );

protected:
  int r_, g_, b_;
  wxString name_;

  void CopyStuff( GRA_color const &rhs )
  {
    r_ = rhs.r_;
    g_ = rhs.g_;
    b_ = rhs.b_;
    name_ = rhs.name_;
  }
};

#endif
