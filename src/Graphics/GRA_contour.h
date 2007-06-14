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
#ifndef GRA_CONTOUR
#define GRA_CONTOUR

#include <vector>
#include <ostream>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_wxWidgets;
class GRA_color;
class GRA_font;
class GRA_drawableText;

class GRA_contour : public GRA_drawableObject
{
public:
  GRA_contour() : GRA_drawableObject(wxT("CONTOUR")), contourValue_(0.0), color_(0), polar_(false)
  {}

  GRA_contour( double, double, GRA_color *, std::vector<double> &, std::vector<double> &,
               std::vector< std::vector<double> > &, bool );

  ~GRA_contour();

  GRA_contour( GRA_contour const &rhs ) : GRA_drawableObject(rhs)
  {}

  GRA_contour &operator=( GRA_contour const & )
  { return *this; }

  void Make();
  void Draw( GRA_wxWidgets *, wxDC & );

  friend std::ostream &operator<<( std::ostream &, GRA_contour const & );

  void SetColor( GRA_color *c )
  { color_ = c; }

  GRA_color *GetColor() const
  { return color_; }

  void SetCoords( std::vector<double> const &x, std::vector<double> const &y,
                  std::vector<bool> const &connect )
  {
    xPlot_.assign( x.begin(), x.end() );
    yPlot_.assign( y.begin(), y.end() );
    connect_.assign( connect.begin(), connect.end() );
  }

  void GetCoords( std::vector<double> &x, std::vector<double> &y, std::vector<bool> &connect )
  {
    x.assign( xPlot_.begin(), xPlot_.end() );
    y.assign( yPlot_.begin(), yPlot_.end() );
    connect.assign( connect_.begin(), connect_.end() );
  }
  
  void SetDrawableText( std::vector<GRA_drawableText*> const &textVec )
  { textVec_.assign( textVec.begin(), textVec.end() ); }

  void GetDrawableText( std::vector<GRA_drawableText*> &text )
  { text.assign( textVec_.begin(), textVec_.end() ); }

  void GetXYPlot( std::vector<double> &x, std::vector<double> &y, std::vector<bool> &connect )
  {
    x = xPlot_;
    y = yPlot_;
    connect = connect_;
  }
  
  std::vector<GRA_drawableText*> *GetTextVec()
  { return &textVec_; }
  
private:
  void ClearAllSwitches();
  bool Search( int, int, bool );
  void ContourA();
  void ContourB( double, double, double, double, double, double );
  //
  void SetGraphPlotFirst();
  void GraphPlot( double, double, int );
  //
  double contourValue_, drawnValue_;
  int xRef_, yRef_, xNext_, yNext_;
  std::vector<double> xCoords_, yCoords_;
  int nArray_, *array_;
  std::vector<double> xData_, yData_;
  std::vector< std::vector<double> > zData_;
  bool polar_, numin_;
  double labelSeparation_, labelSize_, xSep_;
  int pen_;
  GRA_color *color_;
  GRA_font *font_;
  bool graphPlotFirst_;
  std::vector<double> xPlot_, yPlot_;
  std::vector<bool> connect_;
  std::vector<GRA_drawableText*> textVec_;
};
#endif

