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
#ifndef GRA_PLOTSYMBOL
#define GRA_PLOTSYMBOL

#include <ostream>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_color;
class GRA_wxWidgets;

class GRA_plotSymbol : public GRA_drawableObject
{
public:
  GRA_plotSymbol( int, double, double =0.0, GRA_color * =0, int =1 );
  
  ~GRA_plotSymbol()
  {}
  
  GRA_plotSymbol( GRA_plotSymbol const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff( rhs ); }
  
  GRA_plotSymbol &operator=( GRA_plotSymbol const &rhs )
  {
    if( this != &rhs )
    {
      type_ = rhs.type_;
      CopyStuff( rhs );
    }
    return *this;
  }

  void SetLocation( double x, double y )
  {
    x_ = x;
    y_ = y;
  }
  
  void GetLocation( double &x, double &y ) const
  {
    x = x_;
    y = y_;
  }
  
  void SetShapeCode( int c )
  {
    connectToPrevious_ = (c>=0);
    shapeCode_ = (abs(c)-1)%numberOfShapes_ + 1;
  }
  
  int GetShapeCode() const
  { return shapeCode_; }
  
  void SetSize( double size )
  { size_ = size; }
  
  double GetSize() const
  { return size_; }
  
  void SetAngle( double angle )
  { angle_ = angle; }
  
  double GetAngle() const
  { return angle_; }
  
  void SetColor( GRA_color *c )
  { color_ = c; }
  
  GRA_color *GetColor() const
  { return color_; }
  
  void SetLineWidth( int w )
  { lineWidth_ = w; }
  
  int GetLineWidth() const
  { return lineWidth_; }
  
  void SetConnectToPrevious( bool b )
  { connectToPrevious_ = b; }
  
  bool GetConnectToPrevious() const
  { return connectToPrevious_; }

  void Draw( GRA_wxWidgets *, wxDC & );

  friend std::ostream &operator<<( std::ostream &, GRA_plotSymbol const & );

private:
  void CopyStuff( GRA_plotSymbol const & );
  //
  static int numberOfShapes_;
  //
  double x_, y_;  // location in world units
  int shapeCode_;
  double angle_;
  double size_;   // size in world units
  GRA_color *color_;
  int lineWidth_;
  bool connectToPrevious_;
};
 
#endif
 
