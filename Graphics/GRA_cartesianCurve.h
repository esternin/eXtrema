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

#include "GRA_drawableObject.h"

class GRA_plotSymbol;
class GRA_errorBar;
class GRA_color;
class GRA_outputType;

class GRA_cartesianCurve : public GRA_drawableObject
{
public:
  GRA_cartesianCurve()
   : GRA_drawableObject("CARTESIANCURVE"), smooth_(false), popup_(false)
  {}

  GRA_cartesianCurve( std::vector<double> const &x, std::vector<double> const &y,
                      std::vector<double> const &xE1, std::vector<double> const &yE1,
                      std::vector<double> const &xE2, std::vector<double> const &yE2,
                      bool smooth =false )
      : GRA_drawableObject("CARTESIANCURVE"), smooth_(smooth), popup_(false)
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

  ~GRA_cartesianCurve();

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
  void Draw( GRA_outputType * );
  bool Inside( double, double );
  
  std::vector<double> const &GetXData()
  { return xData_; }

  std::vector<double> const &GetYData()
  { return yData_; }

  void GetXYMinMax( double &, double &, double &, double & );
  void GetXMinMax( double const, double const, double &, double & );
  void GetYMinMax( double const, double const, double &, double & );

  friend std::ostream &operator<<( std::ostream &, GRA_cartesianCurve const & );

  void SetValues( int, int, int, GRA_color*, GRA_color*, double, double, double,
                  double, double, double, double, double, std::vector<GRA_color*> &,
                  std::vector<double> &, std::vector<double> &, std::vector<int> &,
                  std::vector<GRA_plotSymbol*> &, std::vector<GRA_errorBar*> & );

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
  void DrawHistogramNoTails( GRA_outputType * );
  void DrawHistogramWithTails( GRA_outputType * );
  void DrawNonHistogram( GRA_outputType * );
  void DrawErrorBars( GRA_outputType * );
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
  //
  bool popup_;
};

#endif

