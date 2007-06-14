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
#ifndef GRA_CARTESIANCURVE
#define GRA_CARTESIANCURVE

#include <vector>
#include <ostream>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_plotSymbol;
class GRA_errorBar;
class GRA_color;
class GRA_wxWidgets;

class GRA_cartesianCurve : public GRA_drawableObject
{
public:
  GRA_cartesianCurve() : GRA_drawableObject(wxT("CARTESIANCURVE")), smooth_(false)
  {}

  GRA_cartesianCurve( std::vector<double> const &x, std::vector<double> const &y,
                      std::vector<double> const &xE1, std::vector<double> const &yE1,
                      std::vector<double> const &xE2, std::vector<double> const &yE2,
                      bool smooth =false )
      : GRA_drawableObject(wxT("CARTESIANCURVE")), smooth_(smooth)
  {
    xData_.assign( x.begin(), x.end() );
    yData_.assign( y.begin(), y.end() );
    xE1_.assign( xE1.begin(), xE1.end() );
    yE1_.assign( yE1.begin(), yE1.end() );
    xE2_.assign( xE2.begin(), xE2.end() );
    yE2_.assign( yE2.begin(), yE2.end() );
    if( !xE1_.empty() && xE2_.empty() )xE2_.assign( xE1_.begin(), xE1_.end() );
    if( !yE1_.empty() && yE2_.empty() )yE2_.assign( yE1_.begin(), yE1_.end() );
    SetUp();
  }

  ~GRA_cartesianCurve()
  { DeleteStuff(); }
  
  GRA_cartesianCurve( GRA_cartesianCurve const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff(rhs); }

  GRA_cartesianCurve &operator=( GRA_cartesianCurve const &rhs )
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
  
  std::vector<double> const &GetXData()
  { return xData_; }

  std::vector<double> const &GetYData()
  { return yData_; }

  void GetXYMinMax( double &, double &, double &, double & );
  void GetXMinMax( double const, double const, double &, double & );
  void GetYMinMax( double const, double const, double &, double & );

  friend std::ostream &operator<<( std::ostream &, GRA_cartesianCurve const & );

  void SetCharacteristic( wxString &, wxString & );
  void SetCharacteristic( wxString &, wxString &, wxString & );
  void SetValues( int, int, int, GRA_color*, GRA_color*, double, double, double,
                  double, double, double, double, double, std::vector<GRA_color*> &,
                  std::vector<double> &, std::vector<double> &, std::vector<int> &,
                  std::vector<GRA_plotSymbol*> &, std::vector<GRA_errorBar*> & );

  void GetClippingBoundary( double &xlaxis, double &ylaxis,
                            double &xuaxis, double &yuaxis )
  {
    xlaxis = xlaxis_;
    ylaxis = ylaxis_;
    xuaxis = xuaxis_;
    yuaxis = yuaxis_;
  }
  
  int GetHistogramType() const
  { return histogramType_; }

  std::vector<GRA_color*> &GetAreaFillColors()
  { return areaFillColors_; }

  GRA_color *GetAreaFillColor()
  { return areaFillColor_; }

  void GetXYcurve( std::vector<double> &x, std::vector<double> &y )
  {
    x.assign( xCurve_.begin(), xCurve_.end() );
    y.assign( yCurve_.begin(), yCurve_.end() );
  }

  GRA_color *GetColor()
  { return color_; }

  int GetLineWidth() const
  { return lineWidth_; }

  int GetLineType() const
  { return lineType_; }
  
  std::vector<int> &GetPen()
  { return pen_; }

  std::vector<GRA_plotSymbol*> &GetPlotSymbols()
  { return plotsymbols_; }

  std::vector<GRA_errorBar*> &GetErrorBars()
  { return errorBars_; }
    
private:
  void SetUp();
  void DeleteStuff();
  void CopyStuff( GRA_cartesianCurve const & );
  //
  void MakeHorizontalHistogramNoTails();
  void MakeHorizontalHistogramWithTails();
  void MakeVerticalHistogramNoTails();
  void MakeVerticalHistogramWithTails();
  void MakeNonHistogram();
  void MakeErrorBars();
  //
  void DrawHistogramNoTails( GRA_wxWidgets *, wxDC & );
  void DrawHistogramWithTails( GRA_wxWidgets *, wxDC & );
  void DrawNonHistogram( GRA_wxWidgets *, wxDC & );
  void DrawErrorBars( GRA_wxWidgets *, wxDC & );
  //
  int histogramType_, lineType_, lineWidth_;
  bool smooth_;
  GRA_color *color_, *areaFillColor_;
  std::vector<GRA_color*> areaFillColors_;
  //
  std::vector<double> xData_, yData_, xE1_, xE2_, yE1_, yE2_, xCurve_, yCurve_;
  std::vector<int> pen_;
  std::vector<GRA_plotSymbol*> plotsymbols_;
  std::vector<GRA_errorBar*> errorBars_;
  double xlaxis_, ylaxis_, xuaxis_, yuaxis_;
  double xmin_, xmax_, ymin_, ymax_;
};

#endif
