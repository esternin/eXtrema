/*
Copyright (C) 2005,...,2010 Joseph L. Chuma

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
#ifndef GRA_POSTSCRIPT
#define GRA_POSTSCRIPT

#include <fstream>

#include "wx/wx.h"

class GRA_color;
class GRA_font;
class GRA_point;
class GRA_polyline;
class GRA_polygon;
class GRA_multiLineFigure;
class GRA_ellipse;
class GRA_plotSymbol;
class GRA_bitmap;
class GRA_drawableText;
class GRA_drawableObject;
class GRA_axis;
class GRA_cartesianAxes;
class GRA_cartesianCurve;
class GRA_polarAxes;
class GRA_polarCurve;
class GRA_legend;
class GRA_contourLine;
class GRA_densityPlot;
class GRA_boxPlot;
class GRA_diffusionPlot;
class GRA_ditheringPlot;
class GRA_gradientPlot;
class GRA_window;

class GRA_postscript
{
public:
  GRA_postscript()
  {}
  
  ~GRA_postscript();

  void Initialize( wxString const & );

  GRA_color *GetColor() const;
  void SetColor( GRA_color * );
  void SetColor( int );
  void SetColor( int, int, int );
  
  void SetLineType( int );
  int GetLineType() const;

  void SetLineWidth( int );
  int GetLineWidth() const;

  void GetLimits( double &, double &, double &, double & ) const;

  void OutputTypeToWorld( int, int, double &, double & ) const;
  void WorldToOutputType( double, double, double &, double & ) const;
  
  void Plot( double, double, int );
  void PlotPoint( double, double );
  void PenUp( double, double );
  void PenDown( double, double );
  void StartLine( double, double );
  void ContinueLine( double, double );
  
  void DrawLineSet( int, double, double, double );
  void DrawLineGet( int, double &, double &, double & );
  void SetDefaultDrawLineTable();
  void DrawLineScale( int, double );
  void DrawLineScale( double );

  void DisplayBackground( GRA_window * );

  void Draw( GRA_point * );
  void Draw( GRA_polyline * );
  void Draw( GRA_polygon * );
  void Draw( GRA_multiLineFigure * );
  void Draw( GRA_ellipse * );
  void Draw( GRA_plotSymbol * );
  void Draw( GRA_bitmap * );
  void Draw( GRA_drawableText * );
  void Draw( GRA_axis * );
  void Draw( GRA_cartesianAxes * );
  void Draw( GRA_cartesianCurve * );
  void Draw( GRA_polarAxes * );
  void Draw( GRA_polarCurve * );
  void Draw( GRA_legend * );
  void Draw( GRA_contourLine * );
  void Draw( GRA_boxPlot * );
  void Draw( GRA_diffusionPlot * );
  void Draw( GRA_ditheringPlot * );
  void Draw( GRA_gradientPlot * );

  void EndDoc();

private:
  void SetUpTransformationMatrices( double, double, double, double );
  void GenerateOutput( double, double, int );
  void DrawProfiles( GRA_densityPlot * );
  void DrawAngle( wxString &, double, double, double, double, GRA_color *, GRA_font * );
  
  std::ofstream outFile_;
  wxString filename_;
  int r_, g_, b_;
  int lineWidth_, lineType_;
  double xMin_, yMin_, xMax_, yMax_;
  double w2oMatrix_[2][2], w2oShift_[2]; // world to output transformation matrix
  double o2wMatrix_[2][2], o2wShift_[2]; // output to world transformation matrix
  double xPrevious_, yPrevious_; // world units
  int penPrevious_;
  bool newLine_;
  double drawLineTable_[3][10];
  static double dotsPerInch_;
  int counter_;
};

#endif
