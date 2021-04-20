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
#ifndef GRA_GD
#define GRA_GD

#include <cstdio>

#include "gd.h"

#include "ExString.h"
#include "GRA_outputType.h"

class GRA_color;
class GRA_point;
class GRA_polyline;
class GRA_polygon;
class GRA_multiLineFigure;
class GRA_ellipse;
class GRA_plotSymbol;
class GRA_bitmap;
class GRA_drawableText;
class GRA_simpleText;

class GRA_gd : public GRA_outputType
{
public:
  GRA_gd( ExString const &, int =0, int =0 );
  ~GRA_gd();

  GRA_color *GetColor() const
  { return color_; }

  void SetColor( GRA_color * );

  void SetColor( int )
  {}

  void SetColor( int, int, int )
  {}

  void SetLineWidth( int );

  int GetLineWidth() const
  { return lineWidth_; }

  void Draw();
  void Draw( GRA_point * );
  void Draw( GRA_polyline * );
  void Draw( GRA_polygon * );
  void Draw( GRA_multiLineFigure * );
  void Draw( GRA_ellipse * );
  void Draw( GRA_plotSymbol * );
  void Draw( GRA_bitmap * );
  void Draw( GRA_drawableText * );

  void GetTextWidthHeight( GRA_simpleText *, double, double, double &, double & );
  void WorldToOutputType( double, double, double &, double & ) const;

private:
  void Initialize();
  void GenerateOutput( double, double, int );

  ExString filename_;
  FILE *outFile_;
  gdImagePtr gd_;
  int gdColor_, xPrevious_, yPrevious_;
  GRA_color *color_;
  int lineWidth_;

  double heightCorrection_;
};

#endif
