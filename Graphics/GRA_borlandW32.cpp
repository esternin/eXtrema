/*
Copyright (C) 2005,...,2008 Joseph L. Chuma, TRIUMF

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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <vector>

#include "GRA_font.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_point.h"
#include "GRA_polyline.h"
#include "GRA_polygon.h"
#include "GRA_multiLineFigure.h"
#include "GRA_ellipse.h"
#include "GRA_plotSymbol.h"
#include "GRA_bitmap.h"
#include "GRA_drawableText.h"
#include "GRA_simpleText.h"
#include "ExGlobals.h"
#include "EGraphicsError.h"

#include "GRA_borlandW32.h"

GRA_borlandW32::GRA_borlandW32()
 : GRA_outputType(), canvas_(0), inc_(1)
{
  int xmin, ymin, xmax, ymax;
  ExGlobals::GetMonitorLimits( xmin, ymin, xmax, ymax );
  SetUpTransformationMatrices( static_cast<double>(xmin), static_cast<double>(ymin),
                               static_cast<double>(xmax), static_cast<double>(ymax) );
}

GRA_borlandW32::GRA_borlandW32( int xmin, int ymin, int xmax, int ymax )
 : GRA_outputType(), canvas_(0), inc_(1)
{
  SetUpTransformationMatrices( static_cast<double>(xmin), static_cast<double>(ymin),
                               static_cast<double>(xmax), static_cast<double>(ymax) );
}

void GRA_borlandW32::WorldToOutputType( double xw, double yw, double &xo, double &yo ) const
{
  GRA_outputType::WorldToOutputType( xw, yw, xo, yo );
  yo = yMax_ - yo;
}

void GRA_borlandW32::OutputTypeToWorld( int xo, int yo, double &xw, double &yw ) const
{
  GRA_outputType::OutputTypeToWorld( xo, static_cast<int>(yMax_-yo), xw, yw );
}

void GRA_borlandW32::ClearGraphicsCanvas()
{
  // clearing graphics only applies to the screen monitor
  // all other output types are not drawn until the user asks for it,
  // so clearing is meaningless for those types
  //
  int xll = static_cast<int>(xMin_);
  int yll = static_cast<int>(yMax_);
  int xur = static_cast<int>(xMax_);
  int yur = static_cast<int>(yMin_);
  canvas_->CopyMode = cmWhiteness;
  TRect aRect( xll, yll, xur, yur );
  canvas_->CopyRect( aRect, canvas_, aRect );
  canvas_->CopyMode = cmSrcCopy;
}

void GRA_borlandW32::Erase( double xminw, double yminw, double xmaxw, double ymaxw )
{
  double xmin, ymin, xmax, ymax;
  WorldToOutputType( xminw, yminw, xmin, ymin );
  WorldToOutputType( xmaxw, ymaxw, xmax, ymax );
  int xll = static_cast<int>(xmin);
  int yll = static_cast<int>(ymin);
  int xur = static_cast<int>(xmax);
  int yur = static_cast<int>(ymax);
  canvas_->CopyMode = cmWhiteness;
  TRect aRect( xll, yll, xur, yur );
  canvas_->CopyRect( aRect, canvas_, aRect );
  canvas_->CopyMode = cmSrcCopy;
}

GRA_color *GRA_borlandW32::GetColor() const
{
  int tcolor = ColorToRGB(canvas_->Pen->Color);
  return GRA_colorControl::GetColor(
   GRA_colorControl::GetColorCode(static_cast<int>(GetRValue(tcolor)),
                                  static_cast<int>(GetGValue(tcolor)),
                                  static_cast<int>(GetBValue(tcolor))) );
}

void GRA_borlandW32::SetColor( GRA_color *color )
{
  if( color )
  {
    int r, g, b;
    color->GetRGB( r, g, b );
    canvas_->Pen->Color = GetTColor( r, g, b );
  }
}

void GRA_borlandW32::SetFillColor( GRA_color *color )
{
  if( color )
  {
    int r, g, b;
    color->GetRGB( r, g, b );
    canvas_->Brush->Color = GetTColor( r, g, b );
  }
}

TColor GRA_borlandW32::GetTColor( GRA_color *color ) const
{
  if( color )
  {
    int r, g, b;
    color->GetRGB( r, g, b );
    return GetTColor( r, g, b );
  }
  else
    return StringToColor(AnsiString("0x00000000"));
}

TColor GRA_borlandW32::GetTColor( int code ) const
{
  int r, g, b;
  GRA_colorControl::ColorCodeToRGB( code, r, g, b );
  return GetTColor( r, g, b );
}

TFont *GRA_borlandW32::TranslateFont( GRA_font const *font )
{
  // remember to delete the font returned by TranslateFont
  //
  TFont *tfont = new TFont();
  tfont->Name = AnsiString(font->GetName().c_str());
  tfont->Style = TFontStyles();
  return tfont;
}

void GRA_borlandW32::Draw()
{
  try
  {
    ExGlobals::DrawGraphWindows( this );
  }
  catch (EGraphicsError &e)
  {
    throw;
  }
}

void GRA_borlandW32::Draw( GRA_point *p )
{
  SetColor( p->GetColor() );
  double x, y;
  p->GetXY( x, y );
  PlotPoint( x, y );
}

void GRA_borlandW32::Draw( GRA_polyline *pl )
{
  SetColor( pl->GetColor() );
  SetLineWidth( pl->GetLineWidth() );
  SetLineType( pl->GetLineType() );
  //
  std::vector<double> x( pl->GetX() );
  std::vector<double> y( pl->GetY() );
  std::vector<int> pen( pl->GetPen() );
  //
  std::size_t size = x.size();
  StartLine( x[0], y[0] );
  for( std::size_t i=1; i<size; ++i )
  {
    pen[i]==3 ? StartLine(x[i],y[i]) : ContinueLine(x[i],y[i]);
  }
}

void GRA_borlandW32::Draw( GRA_polygon *p )
{
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  //
  std::vector<double> x, y;
  p->DoClipping( xmin, ymin, xmax, ymax, x, y );
  //
  std::size_t const size = x.size();
  TPoint *tp = new TPoint[size];
  for( std::size_t i=0; i<size; ++i )
  {
    double ix, iy;
    WorldToOutputType( x[i], y[i], ix, iy );
    tp[i] = Point(static_cast<int>(ix+0.5),static_cast<int>(iy+0.5));
  }
  SetLineWidth( p->GetLineWidth() );
  SetColor( p->GetLineColor() );
  GRA_color *fillColor = p->GetFillColor();
  if( fillColor )
  {
    SetFillColor( fillColor );
    canvas_->Brush->Style = bsSolid;
    canvas_->Polygon(tp,size-1);
  }
  canvas_->Brush->Style = bsClear;
  canvas_->Polygon(tp,size-1);
  delete [] tp;
}

void GRA_borlandW32::Draw( GRA_multiLineFigure *m )
{
  SetLineWidth( m->GetLineWidth() );
  SetColor( m->GetLineColor() );
  //
  std::vector<double> x, y;
  std::vector<int> pen;
  m->GetCoordinates( x, y, pen );
  //
  std::size_t size = x.size();
  for( std::size_t i=0; i<size; ++i )
  {
    pen[i]==3 ? PenUp(x[i],y[i]) : PenDown(x[i],y[i]);
  }
}

void GRA_borlandW32::Draw( GRA_ellipse *ellipse )
{
  double xulw, yulw, xlrw, ylrw;
  ellipse->GetLimits( xulw, yulw, xlrw, ylrw );
  double xul, yul, xlr, ylr;
  WorldToOutputType( xulw, yulw, xul, yul );
  WorldToOutputType( xlrw, ylrw, xlr, ylr );
  SetColor( ellipse->GetLineColor() );
  SetLineWidth( ellipse->GetLineWidth() );
  GRA_color *fillColor = ellipse->GetFillColor();
  if( fillColor )
  {
    canvas_->Brush->Style = bsSolid;
    SetFillColor( fillColor );
    canvas_->Ellipse( static_cast<int>(xul+0.5), static_cast<int>(yul+0.5),
                      static_cast<int>(xlr+0.5), static_cast<int>(ylr+0.5) );
  }
  canvas_->Brush->Style = bsClear;
  canvas_->Ellipse( static_cast<int>(xul+0.5), static_cast<int>(yul+0.5),
                    static_cast<int>(xlr+0.5), static_cast<int>(ylr+0.5) );
}

void GRA_borlandW32::Draw( GRA_plotSymbol *p )
{ p->Draw( this ); }

void GRA_borlandW32::Draw( GRA_bitmap *b )
{
  int xlo, ylo, xhi, yhi;
  b->GetLimits( xlo, ylo, xhi, yhi );
  std::vector<GRA_color*> bitmap( b->GetBitmap() );
  int bwidth = b->GetWidth();
  int bheight = b->GetHeight();
  /*
  if( isPrinter_ )
  {
    Graphics::TBitmap *tb = new Graphics::TBitmap();
    for( int i=0; i<bwidth; i+=inc_ )
    {
      for( int j=0; j<bheight; j+=inc_ )
        tb->Canvas->Pixels[i][j] = GetTColor( bitmap[i+j*bwidth] );
    }
    TRect aRect = Rect( xlo-1, static_cast<int>(yMax_)-ylo+1, bwidth+xlo-1, static_cast<int>(yMax_)-ylo+1-bheight );
    canvas_->StretchDraw( aRect, tb );
    delete tb;
  }
  else
  {
    for( int i=0; i<bwidth; i+=inc_ )
    {
      for( int j=0; j<bheight; j+=inc_ )
        canvas_->Pixels[i+xlo-1][static_cast<int>(yMax_)-ylo-j+1] = GetTColor( bitmap[i+j*bwidth] );
    }
  }
  */
  for( int i=0; i<bwidth; i+=inc_ )
  {
    for( int j=0; j<bheight; j+=inc_ )
      canvas_->Pixels[i+xlo-1][static_cast<int>(yMax_)-ylo-j+1] = GetTColor( bitmap[i+j*bwidth] );
  }
}

double GRA_borlandW32::GetTextWidth( GRA_simpleText *st, double xConvert, double yConvert )
{
  TCanvas *monitorCanvas = static_cast<GRA_borlandW32*>(ExGlobals::GetScreenOutput())->GetCanvas();
  TFont *tfontSave = new TFont();
  tfontSave->Assign( monitorCanvas->Font );
  //
  TFont *tfont = TranslateFont( st->GetFont() );
  tfont->Height = -static_cast<int>(st->GetHeight()*yConvert+0.5);
  monitorCanvas->Font->Assign( tfont );
  //
  int tmp = monitorCanvas->TextWidth(AnsiString(st->GetString().c_str()));
  monitorCanvas->Font->Assign( tfontSave );
  delete tfont;
  delete tfontSave;
  return static_cast<double>(tmp)/xConvert + st->GetXShift();
}

void GRA_borlandW32::Draw( GRA_drawableText *dt )
{
  double xminW, yminW, xmaxW, ymaxW;
  ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
  double const yConvert = (yMax_-yMin_)/(ymaxW-yminW);
  double const xConvert = (xMax_-xMin_)/(xmaxW-xminW);
  //
  double length = 0.0;
  std::vector<GRA_simpleText*> strings( dt->GetStrings() );
  std::vector<GRA_simpleText*>::const_iterator end = strings.end();
  double maxHeight = 0.0;
  for( std::vector<GRA_simpleText*>::const_iterator i=strings.begin(); i!=end; ++i )
  {
    maxHeight = std::max(maxHeight,(*i)->GetHeight());
    length += GetTextWidth( *i, xConvert, yConvert );
  }
  double angle, xLoc, yLoc, sinx, cosx;
  AlignXY( dt, length, maxHeight, angle, xLoc, yLoc, sinx, cosx );
  TFont *tfontSave = new TFont();
  tfontSave->Assign( canvas_->Font );
  length = 0.0;
  for( std::vector<GRA_simpleText*>::const_iterator i=strings.begin(); i!=end; ++i )
  {
    xLoc += length*cosx - (*i)->GetYShift()*sinx + (*i)->GetXShift()*cosx;
    yLoc += length*sinx + (*i)->GetYShift()*cosx + (*i)->GetXShift()*sinx;
    length = GetTextWidth( *i, xConvert, yConvert );
    double height = (*i)->GetHeight();
    double x, y;
    WorldToOutputType( xLoc-height*sinx, yLoc+height*cosx, x, y );
    TFont *tfont = TranslateFont( (*i)->GetFont() );
    tfont->Color = GetTColor( (*i)->GetColor() );
    int mHeight = static_cast<int>(height*yConvert+0.5); // in monitor units
    if( angle == 0.0 )
    {
      canvas_->Font->Assign( tfont );
      canvas_->Font->Height = -mHeight;
    }
    else // see the TFont rotated font example to use a non-zero angle
    {
      LOGFONT lf; // Windows native font structure
      ZeroMemory( &lf, sizeof(LOGFONT) );
      lf.lfQuality = PROOF_QUALITY;
      lf.lfHeight = -mHeight;
      lf.lfEscapement = static_cast<int>(10*angle);
      lf.lfOrientation = static_cast<int>(10*angle);
      //lf.lfCharSet = font->Charset;
      lf.lfWeight = FW_NORMAL;
      lf.lfItalic = false;
      if( tfont->Style.Contains(fsBold) )lf.lfWeight = FW_BOLD;
      if( tfont->Style.Contains(fsItalic) )lf.lfItalic = true;
      strcpy( lf.lfFaceName, tfont->Name.c_str() );
      canvas_->Font->Handle = CreateFontIndirect( &lf );
      canvas_->Font->Color = tfont->Color;
    }
    AnsiString aString( (*i)->GetString().c_str() );
    int ix = static_cast<int>(x+0.5);
    int iy = static_cast<int>(y+0.5);
    canvas_->TextOut( ix, iy, aString );
    (*i)->SetBoundary( ix, iy, canvas_->TextWidth(aString), canvas_->TextHeight(aString) );
    delete tfont;
  }
  canvas_->Font->Assign( tfontSave );
  delete tfontSave;
}

void GRA_borlandW32::Erase( GRA_drawableText *dt )
{
  double xminW, yminW, xmaxW, ymaxW;
  ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
  double const yConvert = (yMax_-yMin_)/(ymaxW-yminW);
  double const xConvert = (xMax_-xMin_)/(xmaxW-xminW);
  //
  double length = 0.0;
  std::vector<GRA_simpleText*> strings( dt->GetStrings() );
  std::vector<GRA_simpleText*>::const_iterator end = strings.end();
  double maxHeight = 0.0;
  for( std::vector<GRA_simpleText*>::const_iterator i=strings.begin(); i!=end; ++i )
  {
    maxHeight = std::max(maxHeight,(*i)->GetHeight());
    length += GetTextWidth( *i, xConvert, yConvert );
  }
  double angle, xLoc, yLoc, sinx, cosx;
  AlignXY( dt, length, maxHeight, angle, xLoc, yLoc, sinx, cosx );
  TFont *tfontSave = new TFont();
  tfontSave->Assign( canvas_->Font );
  length = 0.0;
  for( std::vector<GRA_simpleText*>::const_iterator i=strings.begin(); i!=end; ++i )
  {
    xLoc += length*cosx - (*i)->GetYShift()*sinx + (*i)->GetXShift()*cosx;
    yLoc += length*sinx + (*i)->GetYShift()*cosx + (*i)->GetXShift()*sinx;
    length = GetTextWidth( *i, xConvert, yConvert );
    double height = (*i)->GetHeight();
    double x, y;
    WorldToOutputType( xLoc-height*sinx, yLoc+height*cosx, x, y );
    TFont *tfont = TranslateFont( (*i)->GetFont() );
    tfont->Color = GetTColor( GRA_colorControl::GetColor("WHITE") );
    int mHeight = static_cast<int>(height*yConvert+0.5); // in monitor units
    if( angle == 0.0 )
    {
      canvas_->Font->Assign( tfont );
      canvas_->Font->Height = -mHeight;
    }
    else // see the TFont rotated font example to use a non-zero angle
    {
      LOGFONT lf; // Windows native font structure
      ZeroMemory( &lf, sizeof(LOGFONT) );
      lf.lfQuality = PROOF_QUALITY;
      lf.lfHeight = -mHeight;
      lf.lfEscapement = static_cast<int>(10*angle);
      lf.lfOrientation = static_cast<int>(10*angle);
      //lf.lfCharSet = font->Charset;
      lf.lfWeight = FW_NORMAL;
      lf.lfItalic = false;
      if( tfont->Style.Contains(fsBold) )lf.lfWeight = FW_BOLD;
      if( tfont->Style.Contains(fsItalic) )lf.lfItalic = true;
      strcpy( lf.lfFaceName, tfont->Name.c_str() );
      canvas_->Font->Handle = CreateFontIndirect( &lf );
      canvas_->Font->Color = tfont->Color;
    }
    canvas_->TextOut( static_cast<int>(x+0.5), static_cast<int>(y+0.5),
                      AnsiString((*i)->GetString().c_str()) );
    delete tfont;
  }
  canvas_->Font->Assign( tfontSave );
  delete tfontSave;
}

void GRA_borlandW32::GenerateOutput( double x, double y, int pen )
{
  // pen=2:   move to location (x,y)
  // pen=3:   draw to location (x,y)
  // pen=20:  plot a point at location (x,y)
  //
  double ixd, iyd;
  WorldToOutputType( x, y, ixd, iyd );
  int ix = static_cast<int>(ixd+0.5);
  int iy = static_cast<int>(iyd+0.5);
  switch (pen)
  {
    case 3:
    {
      canvas_->MoveTo( ix, iy );
      break;
    }
    case 2:
    {
      canvas_->LineTo( ix, iy );
      break;
    }
    case 20:
    {
      canvas_->Pixels[ix][iy] = canvas_->Pen->Color;
      break;
    }
  }
}

void GRA_borlandW32::TestEditExit( TEdit *edit, int minValue, char const *what )
{
  if( edit->Text.IsEmpty() )return;
  int n;
  AnsiString s( edit->Text );
  try
  {
    n = s.ToInt();
  }
  catch (...)
  {
    s += AnsiString(" is not a valid integer");
    Application->MessageBox( s.c_str(), "Error", MB_OK );
    edit->Clear();
    edit->SetFocus();
    return;
  }
  if( n < minValue )
  {
    AnsiString message( what );
    message += " must be greater than "+AnsiString(minValue);
    Application->MessageBox(message.c_str(),"Error",MB_OK);
    edit->Clear();
    edit->SetFocus();
    return;
  }
}

void GRA_borlandW32::TestEditExit( TEdit *edit, double minValue, double maxValue,
                                   char const *what )
{
  if( edit->Text.IsEmpty() )return;
  int n;
  AnsiString s( edit->Text );
  try
  {
    n = s.ToInt();
  }
  catch (...)
  {
    s += AnsiString(" is not a valid integer");
    Application->MessageBox( s.c_str(), "Error", MB_OK );
    edit->Clear();
    edit->SetFocus();
    return;
  }
  if( n < minValue )
  {
    AnsiString message( what );
    message += " must be greater than "+AnsiString(minValue);
    Application->MessageBox(message.c_str(),"Error",MB_OK);
    edit->Clear();
    edit->SetFocus();
    return;
  }
  if( n > maxValue )
  {
    AnsiString message( what );
    message += " must be less than "+AnsiString(maxValue);
    Application->MessageBox(message.c_str(),"Error",MB_OK);
    edit->Clear();
    edit->SetFocus();
    return;
  }
}

// end of file
