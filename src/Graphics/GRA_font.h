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
#ifndef GRA_FONT
#define GRA_FONT

#include <ostream>

#include "wx/wx.h"

class GRA_font
{
public:
  // how to pick a font:
  //   -- choose a family (DECORATIVE,ROMAN,SCRIPT,SWISS,MODERN,TELETYPE)
  //   -- or open the font dialog and pick a specific font

  GRA_font( wxString const & );
  
  GRA_font( wxFont &font )
  { wxfont_ = font; }

  ~GRA_font()
  {}
  
  GRA_font( GRA_font const &rhs )
  {
    wxfont_ = rhs.wxfont_;
    name_ = rhs.name_;
  }

  GRA_font &operator=( GRA_font const &rhs )
  {
    if( &rhs != this )
    {
      wxfont_ = rhs.wxfont_;
      name_ = rhs.name_;
    }
    return *this;
  }

  bool operator==( GRA_font const &rhs ) const
  { return (wxfont_==rhs.wxfont_&&name_==rhs.name_); }
  
  bool operator!=( GRA_font const &rhs ) const
  { return !(wxfont_==rhs.wxfont_)||!(name_==rhs.name_); }

  wxString GetFontName() const
  { return name_; }

  wxFont &GetwxFont()
  { return wxfont_; }

  friend std::ostream &operator<<( std::ostream &, GRA_font const & );

private:
  wxFont wxfont_;
  wxString name_;
};

#endif
