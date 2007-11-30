/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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
#ifndef GRA_POLARCURVE
#define GRA_POLARCURVE

#include <vector>
#include <ostream>

#include "GRA_drawableObject.h"

class GRA_plotSymbol;
class GRA_errorBar;
class GRA_color;
class GRA_wxWidgets;

class GRA_polarCurve : public GRA_drawableObject
{
public:
  GRA_polarCurve()
   : GRA_drawableObject(wxT("POLARCURVE")), popup_(false)
  {}

  GRA_polarCurve( std::vector<double> const &r, std::vector<double> const &theta )
      : GRA_drawableObject(wxT("POLARCURVE")), popup_(false)
  {
    r_.assign( r.begin(), r.end() );
    theta_.assign( theta.begin(), theta.end() );
    SetUp();
  }

  ~GRA_polarCurve();

  GRA_polarCurve( GRA_polarCurve const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff(rhs); }

  GRA_polarCurve &operator=( GRA_polarCurve const &rhs )
  {
    if( this != &rhs )
    {
      DeleteStuff();
      type_ = rhs.type_;
      CopyStuff(rhs);
    }
    return *this;
  }

  void Make();
  void Draw( GRA_wxWidgets *, wxDC & );
  bool Inside( double, double );
  
  std::vector<double> const &GetR()
  { return r_; }

  std::vector<double> const &GetTheta()
  { return theta_; }

  void GetXYcurve( std::vector<double> &x, std::vector<double> &y )
  {
    x.assign( xCurve_.begin(), xCurve_.end() );
    y.assign( yCurve_.begin(), yCurve_.end() );
  }

  friend std::ostream &operator<<( std::ostream &, GRA_polarCurve const & );

  void SetValues( int, int, int, double, GRA_color*, GRA_color*,
                  std::vector<GRA_color*> &, std::vector<double> &, std::vector<double> &,
                  std::vector<int> &, std::vector<GRA_plotSymbol*> & );

  GRA_color *GetColor() const
  { return color_; }

  void SetColor( GRA_color *c )
  { color_ = c; }

  GRA_color *GetAreaFillColor() const
  { return areaFillColor_; }

  void SetAreaFillColor( GRA_color *c )
  { areaFillColor_ = c; }

  int GetHistogramType() const
  { return histogramType_; }

  void SetHistogramType( int ht )
  { histogramType_ = ht; }

  int GetLineType() const
  { return lineType_; }

  void SetLineType( int lt )
  { lineType_ = lt; }

  int GetLineWidth() const
  { return lineWidth_; }

  void SetLineWidth( int lw )
  { lineWidth_ = lw; }

  int GetPlotsymbolCode() const;
  void SetPlotsymbolCode( int );

  GRA_color *GetPlotsymbolColor() const;
  void SetPlotsymbolColor( GRA_color * );

  double GetPlotsymbolSize() const;
  void SetPlotsymbolSize( double );

  double GetPlotsymbolAngle() const;
  void SetPlotsymbolAngle( double );

  void SetPopup()
  { popup_ = true; }

  void Disconnect()
  { popup_ = false; }
  
  std::vector<int> &GetPen()
  { return pen_; }

  std::vector<GRA_plotSymbol*> &GetPlotSymbols()
  { return plotsymbols_; }

private:
  void SetUp();
  void DeleteStuff();
  void CopyStuff( GRA_polarCurve const & );
  //
  void MakeNonHistogram();
  //
  void DrawNonHistogram( GRA_wxWidgets *, wxDC & );
  //
  int histogramType_, lineType_, lineWidth_;
  GRA_color *color_, *areaFillColor_;
  std::vector<GRA_color*> areaFillColors_;
  //
  std::vector<double> r_, theta_, xCurve_, yCurve_;
  std::vector<int> pen_;
  std::vector<GRA_plotSymbol*> plotsymbols_;
  //
  double xmax_;
  bool popup_;
};

#endif

