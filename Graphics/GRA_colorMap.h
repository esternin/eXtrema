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
#ifndef GRA_COLORMAP
#define GRA_COLORMAP

#include <vector>

#include "EGraphicsError.h"
#include "ExString.h"
#include "GRA_color.h"

class GRA_colorMap
{
private:
  typedef std::vector<GRA_color*> colorVec;

public:
  GRA_colorMap( ExString const &name ) : name_(name)
  {}

  GRA_colorMap( ExString const &name, colorVec const &v ) : name_(name)
  { colors_.assign( v.begin(), v.end() ); }

  virtual ~GRA_colorMap()
  { DeleteStuff(); }

  GRA_colorMap( GRA_colorMap const &rhs )
  { CopyStuff(rhs); }

  GRA_colorMap &operator=( GRA_colorMap const & rhs )
  {
    if( &rhs != this )
    {
      DeleteStuff();
      CopyStuff(rhs);
    }
    return *this;
  }

  ExString GetName() const
  { return name_; }

  void AddColor( GRA_color *color )
  { colors_.push_back(color); }
  
  GRA_color *GetColor( int i ) const
  {
    GRA_color *color = i>=0 ? colors_[(i-1)%colors_.size()] : 0;
    return color;
  }

  int GetCode( GRA_color * ) const;
  int GetCode( int, int, int ) const;

  int GetSize() const
  { return colors_.size(); }

  friend std::ostream &operator<<( std::ostream &, GRA_colorMap const & );

private:
  void CopyStuff( GRA_colorMap const &rhs )
  {
    name_ = rhs.name_;
    colors_.assign( rhs.colors_.begin(), rhs.colors_.end() );
  }

  // The GRA_colors passed in the constructor are allocated elsewhere,
  // but are deleted by the GRA_colormap destructor

  void DeleteStuff();

  colorVec colors_;
  ExString name_;
};

#endif
