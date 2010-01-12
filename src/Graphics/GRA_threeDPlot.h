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
#ifndef GRA_THREEDPLOT
#define GRA_THREEDPLOT

#include <vector>
#include <ostream>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_plotSymbol;
class GRA_wxWidgets;
class ThreeDPlotForm;

class GRA_threeDPlot : public GRA_drawableObject
{
public:
  GRA_threeDPlot( ThreeDPlotForm *form )
      : GRA_drawableObject(wxT("THREEDPLOT")), threeDForm_(form),
        eye2object_(100.), screenHalfSize_(2.), ds_(15.), angleIncrement_(5.),
        xAxisAngle_(90.), yAxisAngle_(30.)
  {}

  GRA_threeDPlot( ThreeDPlotForm *form,
                  std::vector<double> const &x,
                  std::vector<double> const &y,
                  std::vector<double> const &z )
      : GRA_drawableObject(wxT("THREEDPLOT")), threeDForm_(form),
        eye2object_(100.), screenHalfSize_(2.), ds_(15.), angleIncrement_(5.),
        xAxisAngle_(90.), yAxisAngle_(30.)
  {
    x_.assign( x.begin(), x.end() );
    y_.assign( y.begin(), y.end() );
    z_.assign( z.begin(), z.end() );
    SetupData();
    SetupPlotVectors();
    SetupPlotSymbols();
  }

  ~GRA_threeDPlot()
  { DeleteStuff(); }

  GRA_threeDPlot( GRA_threeDPlot const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff(rhs); }

  GRA_threeDPlot &operator=( GRA_threeDPlot const &rhs )
  {
    if( this != &rhs )
    {
      DeleteStuff();
      CopyStuff(rhs);
    }
    return *this;
  }

  ThreeDPlotForm *GetForm()
  { return threeDForm_; }
  
  void Draw( GRA_wxWidgets *, wxDC & );
  void RotateX( double const );
  void RotateY( double const );
  void ScaleDS( double const );
  void SetEye2Object( double const );

private:
  void DeleteStuff();
  void CopyStuff( GRA_threeDPlot const & );

  void SetupData();
  void SetupPlotVectors();
  void SetupPlotSymbols();

  void ScaleXY( double const, double const, double const, double &, double & );
  void DrawCurve( GRA_wxWidgets *, wxDC & );
  void DrawSymbols( GRA_wxWidgets *, wxDC & );
  void DrawAxes( GRA_wxWidgets *, wxDC & );

  ThreeDPlotForm *threeDForm_;
  std::vector<double> x_, y_, z_;
  double xmin_, xmax_, ymin_, ymax_, zmin_, zmax_;
  double xmn_, xmx_, xinc_, ymn_, ymx_, yinc_, zmn_, zmx_, zinc_;
  int nlxinc_, nlyinc_, nlzinc_;
  double xc_, yc_, zc_;

  std::vector<GRA_plotSymbol*> plotSymbols_;
  std::vector<double> x2_, y2_, z2_, xn_, yn_, zn_;
  std::vector<int> p2_;

  double eye2object_;
  double screenHalfSize_;
  double ds_;
  double angleIncrement_;
  double xAxisAngle_, yAxisAngle_;
};

#endif
