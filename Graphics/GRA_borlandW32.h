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
#ifndef GRA_BORLANDW32
#define GRA_BORLANDW32

#include "ExString.h"

#include "GRA_outputType.h"

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
class GRA_simpleText;

class GRA_borlandW32 : public GRA_outputType
{
public:
  GRA_borlandW32();
  GRA_borlandW32( int, int, int, int );
  
  virtual ~GRA_borlandW32()
  {}

  void SetCanvas( TCanvas *canvas )
  { canvas_ = canvas; }
  
  TCanvas *GetCanvas() const
  { return canvas_; }
  
  void ClearGraphicsCanvas();

  void Erase( double, double, double, double );

  GRA_color *GetColor() const;
  void SetColor( GRA_color * );
  
  void SetColor( int code )
  { canvas_->Pen->Color = GetTColor( code ); }
  
  void SetColor( int r, int g, int b )
  { canvas_->Pen->Color = GetTColor( r, g, b ); }
  
  void SetFillColor( int code )
  { canvas_->Brush->Color = GetTColor( code ); }
  
  void SetFillColor( int r, int g, int b )
  { canvas_->Brush->Color = GetTColor( r, g, b ); }
  
  void SetFillColor( GRA_color * );

  TColor GetTColor( GRA_color * ) const;
  TColor GetTColor( int ) const;
  TColor GetTColor( int r, int g, int b ) const
  { return StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2)); }

  virtual void SetLineWidth( int i )
  { canvas_->Pen->Width = std::max(1,i); }

  virtual int GetLineWidth() const
  { return canvas_->Pen->Width; }
  
  double GetTextWidth( GRA_simpleText *, double, double );

  static TFont *TranslateFont( GRA_font const * );

  void Draw();
  void Draw( GRA_point * );
  void Draw( GRA_polyline * );
  void Draw( GRA_polygon * );
  void Draw( GRA_multiLineFigure * );
  void Draw( GRA_ellipse * );
  void Draw( GRA_plotSymbol * );
  void Draw( GRA_bitmap * );
  void Draw( GRA_drawableText * );
  void Erase( GRA_drawableText * );

  virtual void OutputTypeToWorld( int, int, double &, double & ) const;
  virtual void WorldToOutputType( double, double, double &, double & ) const;

  static void TestEditExit( TEdit *, int, char const * );
  static void TestEditExit( TEdit *, double, double, char const * );

protected:
  TCanvas *canvas_;
  int inc_;

  virtual void GenerateOutput( double, double, int );
};

#endif
