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
#ifndef GRA_DRAWABLEOBJECT
#define GRA_DRAWABLEOBJECT

#include "ExString.h"

class GRA_outputType;

class GRA_drawableObject
{
protected:
  enum Type {UNKNOWN, POINT, POLYLINE, ELLIPSE, POLYGON, MULTILINEFIGURE,
             PLOTSYMBOL, DRAWABLETEXT, AXIS, CARTESIANCURVE, CARTESIANAXES,
             GRAPHLEGEND, CONTOUR, BOXPLOT, DIFFUSIONPLOT, DITHERINGPLOT,
             GRADIENTPLOT, THREEDFIGURE, POLARAXES, POLARCURVE, THREEDPLOT };
public:
  GRA_drawableObject()
  { type_ = UNKNOWN; }

  GRA_drawableObject( char const * );

  GRA_drawableObject( GRA_drawableObject const &rhs )
  { type_ = rhs.type_; }

  virtual ~GRA_drawableObject()
  {}

  bool IsaUnknown() const
  { return (type_ == UNKNOWN); }

  bool IsaPoint() const
  { return (type_ == POINT); }

  bool IsaPolyline() const
  { return (type_ == POLYLINE); }

  bool IsaEllipse() const
  { return (type_ == ELLIPSE); }

  bool IsaPolygon() const
  { return (type_ == POLYGON); }

  bool IsaMultilineFigure() const
  { return (type_ == MULTILINEFIGURE); }

  bool IsaPlotsymbol() const
  { return (type_ == PLOTSYMBOL); }

  bool IsaDrawableText() const
  { return (type_ == DRAWABLETEXT); }

  bool IsaAxis() const
  { return (type_ == AXIS); }

  bool IsaCartesianCurve() const
  { return (type_ == CARTESIANCURVE); }

  bool IsaCartesianAxes() const
  { return (type_ == CARTESIANAXES); }

  bool IsaGraphLegend() const
  { return (type_ == GRAPHLEGEND); }

  bool IsaContour() const
  { return (type_ == CONTOUR); }

  bool IsaBoxPlot() const
  { return (type_ == BOXPLOT); }

  bool IsaDiffusionPlot() const
  { return (type_ == DIFFUSIONPLOT); }

  bool IsaDitheringPlot() const
  { return (type_ == DITHERINGPLOT); }

  bool IsaGradientPlot() const
  { return (type_ == GRADIENTPLOT); }

  bool IsaThreeDFigure() const
  { return (type_ == THREEDFIGURE); }

  bool IsaPolarAxes() const
  { return (type_ == POLARAXES); }

  bool IsaPolarCurve() const
  { return (type_ == POLARCURVE); }

  bool IsaThreeDPlot() const
  { return (type_ == THREEDPLOT); }

  virtual void Draw( GRA_outputType * ) =0;

  virtual void Make()
  {}

  virtual bool Inside( double, double )
  { return false; }

protected:
  Type type_;
};

#endif
