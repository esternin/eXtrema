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
#ifndef GRA_CONTOURLINE
#define GRA_CONTOURLINE

#include <vector>
#include <ostream>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_color;
class GRA_wxWidgets;
class GRA_drawableText;

class GRA_contourLine : public GRA_drawableObject
{
public:
  GRA_contourLine() : GRA_drawableObject(wxT("CONTOURLINE"))
  {}

  GRA_contourLine( double level,
                   std::vector<double> const &x, std::vector<double> const &y,
                   GRA_color *color, int ltype, int lwidth )
      : GRA_drawableObject(wxT("CONTOURLINE")),
        level_(level), color_(color), lineType_(ltype), lineWidth_(lwidth)
  {
    xData_.assign( x.begin(), x.end() );
    yData_.assign( y.begin(), y.end() );
  }

  ~GRA_contourLine()
  {}

  GRA_contourLine( GRA_contourLine const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff(rhs); }

  GRA_contourLine &operator=( GRA_contourLine const &rhs )
  {
    if( this != &rhs )
    {
      type_ = rhs.type_;
      CopyStuff(rhs);
    }
    return *this;
  }

  void Make();
  void Draw( GRA_wxWidgets *, wxDC & );

  double const GetLevel()
  { return level_; }

  std::vector<double> const &GetXData()
  { return xData_; }

  std::vector<double> const &GetYData()
  { return yData_; }

  friend std::ostream &operator<<( std::ostream &, GRA_contourLine const & );

  void SetValues( int, int, GRA_color*, double,
                  std::vector< std::vector<double> > &,
                  std::vector< std::vector<double> > & );

  GRA_color *GetColor() const
  { return color_; }

  void SetColor( GRA_color *c )
  { color_ = c; }

  int GetLineType() const
  { return lineType_; }

  void SetLineType( int lt )
  { lineType_ = lt; }

  int GetLineWidth() const
  { return lineWidth_; }

  void SetLineWidth( int lw )
  { lineWidth_ = lw; }

private:
  void CopyStuff( GRA_contourLine const & );
  //
  double level_;
  int lineType_, lineWidth_;
  GRA_color *color_;
  //
  std::vector<double> xData_, yData_;
  std::vector< std::vector<double> > xCurve_, yCurve_;
};

#endif

