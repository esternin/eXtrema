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
#ifndef GRA_DRAWABLETEXT
#define GRA_DRAWABLETEXT

#include <vector>
#include <ostream>

#include "ExString.h"
#include "ExGlobals.h"
#include "GRA_font.h"
#include "GRA_color.h"
#include "GRA_outputType.h"

#include "GRA_drawableObject.h"

class GRA_simpleText;
class GRA_window;

class GRA_drawableText : public GRA_drawableObject
{
private:
  typedef std::vector<GRA_simpleText*> textVec;
  typedef textVec::const_iterator textVecIter;

  // this class has a vector of pointers to GRA_simpleText objects (simple strings)
public:
  GRA_drawableText( ExString const &, bool =false );

  GRA_drawableText( ExString const &inputString, double height, double angle,
                    double x, double y, int alignment, GRA_font *font,
                    GRA_color *color )
    : GRA_drawableObject("DRAWABLETEXT"), inputString_(inputString), height_(height),
      angle_(angle), x_(x), y_(y), alignment_(alignment),
      color_(color), font_(new GRA_font(*font)), graphUnits_(false),
      subsupFactor_(0.6), popup_(false)
  {}

  ~GRA_drawableText();

  GRA_drawableText( GRA_drawableText const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff( rhs ); }
  
  GRA_drawableText &operator=( GRA_drawableText const &rhs )
  {
    if( this != &rhs )
    {
      DeleteStuff();
      type_ = rhs.type_;
      CopyStuff( rhs );
    }
    return *this;
  }
  
  bool operator==( GRA_drawableText const & ) const;

  void SetString( ExString const &s )
  { inputString_ = s; }

  ExString GetString() const
  { return inputString_; }
  
  textVec const &GetStrings() const
  { return strings_; }

  bool GetGraphUnits() const
  { return graphUnits_; }

  void SetX( double x )
  { x_ = x; }

  double GetX() const
  { return x_; }

  void SetY( double y )
  { y_ = y; }

  double GetY() const
  { return y_; }

  void SetHeight( double h )
  { height_ = h; }

  double GetHeight() const
  { return height_; }

  void SetAngle( double a )
  { angle_ = a; }
  
  double GetAngle() const
  {
    double result = angle_ - static_cast<int>(angle_/360.)*360.;
    if( result < 0.0 )result += 360.;
    return result;
  }

  int GetAlignment() const
  { return alignment_; }

  void SetFont( GRA_font *f )
  { font_ = f; }

  GRA_font *GetFont() const
  { return font_; }

  void SetColor( GRA_color *c )
  { color_ = c; }

  GRA_color *GetColor() const
  { return color_; }
  
  void Parse();

  void Draw( GRA_outputType *graphicsOutput )
  { graphicsOutput->Draw( this ); }

  bool Inside( double, double );

  void SetPopup()
  { popup_ = true; }

  void Disconnect()
  { popup_ = false; }

  double GetWidth() const;

  friend std::ostream &operator<<( std::ostream &, GRA_drawableText const & );
  
private:
  ExString inputString_;
  textVec strings_;
  bool graphUnits_;
  double x_, y_, height_, angle_;
  int alignment_;
  GRA_font *font_;
  GRA_color *color_;
  //
  double subsupFactor_;
  //
  bool popup_;
  //
  void CopyStuff( GRA_drawableText const & );
  void DeleteStuff();
  //
  void DetermineCommand( ExString &, double &, double &, double &,
                         bool &, bool &, int &, int & );
  char Special( ExString const &, char *& );
};

#endif
