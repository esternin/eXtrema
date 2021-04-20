/*
Copyright (C) 2005,...,2009 Joseph L. Chuma

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
#ifndef GRA_OUTPUTTYPE
#define GRA_OUTPUTTYPE

#include "ExString.h"

class GRA_color;
class GRA_point;
class GRA_polyline;
class GRA_polygon;
class GRA_multiLineFigure;
class GRA_ellipse;
class GRA_plotSymbol;
class GRA_bitmap;
class GRA_drawableText;

class GRA_outputType
{
public:
  GRA_outputType() : xMin_(0.0), yMin_(0.0), xMax_(1.0), yMax_(1.0), isPrinter_(false)
  { SetDefaultDrawLineTable(); }

  virtual ~GRA_outputType()
  {}

  void Plot( double, double, int );
  void PlotPoint( double, double );
  void PenUp( double, double );
  void PenDown( double, double );
  void StartLine( double, double );
  void ContinueLine( double, double );

  void Draw(); // draw all graph window objects
  //
  // all graphics output types need to be able to draw the primitive types
  //
  virtual void Draw( GRA_point * ) =0;
  virtual void Draw( GRA_polyline * ) =0;
  virtual void Draw( GRA_polygon * ) =0;
  virtual void Draw( GRA_multiLineFigure * ) =0;
  virtual void Draw( GRA_ellipse * ) =0;
  virtual void Draw( GRA_plotSymbol * ) =0;
  virtual void Draw( GRA_bitmap * ) =0;
  virtual void Draw( GRA_drawableText * ) =0;
  virtual void Erase( GRA_drawableText * )
  {}

  virtual void ClearGraphicsCanvas()
  {}

  virtual void Erase( double, double, double, double )
  {}

  virtual void GetLimits( double &xmin, double &ymin, double &xmax, double &ymax ) const
  { xmin=xMin_; ymin=yMin_; xmax=xMax_; ymax=yMax_; }

  virtual void OutputTypeToWorld( int, int, double &, double & ) const;
  virtual void WorldToOutputType( double, double, double &, double & ) const;

  virtual void SetColor( GRA_color * ) =0;
  virtual void SetColor( int ) =0;
  virtual void SetColor( int, int, int ) =0;
  virtual GRA_color *GetColor() const =0;

  virtual void SetLineWidth( int ) =0;
  virtual int GetLineWidth() const =0;

  int GetLineType() const
  { return currentLineType_; }

  void SetLineType( int i )
  { currentLineType_ = std::max(1,std::min(10,i)); } // illegal type defaults to solid line

  void DrawLineSet( int, double, double, double );
  void DrawLineGet( int, double &, double &, double & );
  void SetDefaultDrawLineTable();
  void DrawLineScale( int, double );
  void DrawLineScale( double );

  bool IsPrinter() const
  { return isPrinter_; }

protected:
  void SetUpTransformationMatrices( double, double, double, double );

  virtual void GenerateOutput( double, double, int ) =0;
  virtual void AlignXY( GRA_drawableText *, double, double, double &, double &,
                        double &, double &, double & );

  bool isPrinter_;
  double xMin_, yMin_, xMax_, yMax_;
  double w2oMatrix_[2][2], w2oShift_[2]; // world to outputType transformation matrix
  double o2wMatrix_[2][2], o2wShift_[2]; // outputType to world transformation matrix
  //
  double xPrevious_, yPrevious_; // world units
  int penPrevious_;
  //
  int currentLineType_;
  //
  bool newLine_;
  double drawLineTable_[3][10];
  double distanceTraveled_, xOld_, yOld_;
  int op_;
};

#endif
