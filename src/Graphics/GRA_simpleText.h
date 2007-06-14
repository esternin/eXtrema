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
#ifndef GRA_SIMPLETEXT
#define GRA_SIMPLETEXT

#include <ostream>

#include "wx/wx.h"

class GRA_color;
class GRA_font;

class GRA_simpleText
{
  // this class encompasses simple strings (without embedded commands)
  // and is only used by GRA_drawableText
  //
public:
  GRA_simpleText( wxString string, double height, GRA_color *color, GRA_font *font,
                  double xShift, double yShift )
      : string_(string), height_(height), color_(color), font_(font),
        xShift_(xShift), yShift_(yShift)
  {}
  
  GRA_simpleText( wxString, double, GRA_color *, wxChar const *, double, double );

  ~GRA_simpleText()
  {}
  
  GRA_simpleText( GRA_simpleText const &rhs )
  { CopyStuff(rhs); }

  GRA_simpleText &operator=( GRA_simpleText const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }
  
  bool operator==( GRA_simpleText const & ) const;
  
  wxString GetString() const
  { return string_; }
  
  GRA_color *GetColor() const
  { return color_; }
  
  GRA_font *GetFont() const
  { return font_; }
  
  double GetHeight() const
  { return height_; }

  double GetXShift() const
  { return xShift_; }

  double GetYShift() const
  { return yShift_; }

  friend std::ostream &operator<<( std::ostream &, GRA_simpleText const & );

private:
  void CopyStuff( GRA_simpleText const & );
  //
  double height_;
  wxString string_;
  GRA_color *color_;
  GRA_font *font_;
  //
  double xShift_, yShift_;
};

#endif
