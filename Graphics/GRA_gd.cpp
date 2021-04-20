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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_point.h"
#include "GRA_polyline.h"
#include "GRA_polygon.h"
#include "GRA_ellipse.h"
#include "GRA_multiLineFigure.h"
#include "GRA_plotSymbol.h"
#include "GRA_bitmap.h"
#include "GRA_font.h"
#include "GRA_simpleText.h"
#include "GRA_drawableText.h"
#include "ExGlobals.h"

#include "GRA_gd.h"

GRA_gd::GRA_gd( ExString const &filename, int width, int height )
    : GRA_outputType(), filename_(filename), heightCorrection_(0.75),
      color_(0), lineWidth_(0)
{
  int xmin, ymin, xmax, ymax;
  if( width==0 || height==0 )ExGlobals::GetMonitorLimits( xmin, ymin, xmax, ymax );
  else
  {
    xmin = 0;
    xmax = width-1;
    ymin = 0;
    ymax = height-1;
  }
  SetUpTransformationMatrices( static_cast<double>(xmin), static_cast<double>(ymin),
                               static_cast<double>(xmax), static_cast<double>(ymax) );
  Initialize();
}

GRA_gd::~GRA_gd()
{
  if( outFile_ )fclose( outFile_ );
  if( gd_ )gdImageDestroy( gd_ );
}

void GRA_gd::Initialize()
{
  ExString extension( filename_.ExtractFileExt() );
  if( extension.empty() )filename_ += ExString(".png");
  outFile_ = fopen( filename_.c_str(), "wb" );
  if( !outFile_ )
    throw EGraphicsError( ExString("GRA_gd: unable to open ")+filename_ );
  //
  // determine the bounding box dimensions
  //
  gd_ = gdImageCreateTrueColor( static_cast<int>(xMax_-xMin_)+1, static_cast<int>(yMax_-yMin_)+1 );
  //
  // make white the background color
  //
  int background = gdImageColorAllocate(gd_,255,255,255);
  gdImageFilledRectangle( gd_, 0, 0, static_cast<int>(xMax_-xMin_),
                          static_cast<int>(yMax_-yMin_), background );
  xPrevious_ = 0;
  yPrevious_ = 0;
}

void GRA_gd::Draw()
{
  try
  {
    ExGlobals::DrawGraphWindows( this );
  }
  catch (EGraphicsError &e)
  {
    throw EGraphicsError( ExString("GRA_gd: unable to create gd file ")+filename_+"\n"+e.what() );
  }
  gdImagePng(gd_,outFile_);
}

void GRA_gd::WorldToOutputType( double xw, double yw, double &xo, double &yo ) const
{
  GRA_outputType::WorldToOutputType( xw, yw, xo, yo );
  yo = yMax_ - yo;
}

void GRA_gd::SetColor( GRA_color *c )
{
  if( !c )return;
  color_ = c;
  int r, g, b;
  color_->GetRGB( r, g, b );
  gdColor_ = gdImageColorExact( gd_, r, g, b );
  if( gdColor_ < 0 )gdColor_ = gdImageColorAllocate( gd_, r, g, b );
  if( gdColor_ < 0 )gdColor_ = gdImageColorClosest( gd_, r, g, b );
}

void GRA_gd::SetLineWidth( int i )
{
  lineWidth_ = i;
  gdImageSetThickness( gd_, i );
}

void GRA_gd::GenerateOutput( double x, double y, int pen )
{
  // Generates graphics output for the printer:
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
    case 2:
      gdImageLine( gd_, xPrevious_, yPrevious_, ix, iy, gdColor_ );
      break;
    case 20:
      gdImageSetPixel( gd_, ix, iy, gdColor_ );
  }
  xPrevious_ = ix;
  yPrevious_ = iy;
}

void GRA_gd::Draw( GRA_point *p )
{
  SetColor( p->GetColor() );
  double x, y;
  p->GetXY( x, y );
  PlotPoint( x, y );
}

void GRA_gd::Draw( GRA_polyline *pl )
{
  SetColor( pl->GetColor() );
  SetLineWidth( pl->GetLineWidth() );
  SetLineType( pl->GetLineType() );
  //
  std::vector<double> x( pl->GetX() );
  std::vector<double> y( pl->GetY() );
  std::vector<int> pen( pl->GetPen() );
  std::size_t size = x.size();
  //
  StartLine( x[0], y[0] );
  for( std::size_t i=1; i<size; ++i )
  {
    pen[i]==3 ? StartLine(x[i],y[i]) : ContinueLine(x[i],y[i]);
  }
}

void GRA_gd::Draw( GRA_polygon *p )
{
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  //
  std::vector<double> x, y;
  p->DoClipping( xmin, ymin, xmax, ymax, x, y );
  //
  std::size_t const size = x.size();
  gdPoint *points = new gdPoint[size];
  for( std::size_t i=0; i<size; ++i )
  {
    double ixtmp, iytmp;
    WorldToOutputType( x[i], y[i], ixtmp, iytmp );
    points[i].x = static_cast<int>(ixtmp+0.5);
    points[i].y = static_cast<int>(iytmp+0.5);
  }
  GRA_color *fillColor = p->GetFillColor();
  if( fillColor )
  {
    SetColor( fillColor );
    gdImageFilledPolygon( gd_, points, static_cast<int>(size), gdColor_ );
  }
  SetColor( p->GetLineColor() );
  SetLineWidth( p->GetLineWidth() );
  gdImagePolygon( gd_, points, static_cast<int>(size), gdColor_ );
}

void GRA_gd::Draw( GRA_multiLineFigure *m )
{
  SetColor( m->GetLineColor() );
  SetLineWidth( m->GetLineWidth() );
  std::vector<double> x, y;
  std::vector<int> pen;
  m->GetCoordinates( x, y, pen );
  std::size_t size = x.size();
  for( std::size_t i=0; i<size; ++i )
  {
    pen[i]==3 ? PenUp(x[i],y[i]) : PenDown(x[i],y[i]);
  }
}

void GRA_gd::Draw( GRA_ellipse *ellipse )
{
  GRA_color *fillColor = ellipse->GetFillColor();
  if( ellipse->IsaCircle() )
  {
    double xp1, yp1, xp2, yp2;
    GRA_outputType::WorldToOutputType( 0.0, 0.0, xp1, yp1 );
    GRA_outputType::WorldToOutputType( 0.5*ellipse->GetMajorAxis(), 0.0, xp2, yp2 );
    int iradius = static_cast<int>(sqrt((xp1-xp2)*(xp1-xp2)+(yp1-yp2)*(yp1-yp2))+0.5);
    double xc, yc;
    ellipse->GetCenter( xc, yc );
    double cx, cy;
    WorldToOutputType( xc, yc, cx, cy );
    //
    if( fillColor )
    {
      SetColor( fillColor );
      gdImageFilledEllipse( gd_, static_cast<int>(cx+0.5), static_cast<int>(cy+0.5),
                            iradius, iradius, gdColor_ );
    }
    SetColor( ellipse->GetLineColor() );
    gdImageArc( gd_, static_cast<int>(cx+0.5), static_cast<int>(cy+0.5),
                iradius, iradius, 0, 360, gdColor_ );
  }
  else
  {
    double xc, yc;
    ellipse->GetCenter( xc, yc );
    double cx, cy;
    WorldToOutputType( xc, yc, cx, cy );
    double xlo, ylo, xup, yup;
    ellipse->GetLimits( xlo, ylo, xup, yup );
    double width, height;
    GRA_outputType::WorldToOutputType( fabs(xup-xlo), fabs(yup-ylo), width, height );
    //
    if( fillColor )
    {
      SetColor( fillColor );
      gdImageFilledEllipse( gd_, static_cast<int>(cx+0.5), static_cast<int>(cy+0.5),
                            static_cast<int>(width+0.5), static_cast<int>(height+0.5),
                            gdColor_ );
    }
    SetColor( ellipse->GetLineColor() );
    gdImageArc( gd_, static_cast<int>(cx+0.5), static_cast<int>(cy+0.5),
                static_cast<int>(width+0.5), static_cast<int>(height+0.5),
                0, 360, gdColor_ );
  }
}

void GRA_gd::Draw( GRA_plotSymbol *p )
{ p->Draw( this ); }

void GRA_gd::Draw( GRA_bitmap *b )
{
  int xlo, ylo, xhi, yhi;
  b->GetLimits( xlo, ylo, xhi, yhi );
  std::vector<GRA_color*> bitmap( b->GetBitmap() );
  int width = b->GetWidth();
  int height = b->GetHeight();
  for( int i=0; i<width; ++i )
  {
    for( int j=0; j<height; ++j )
    {
      SetColor( bitmap[i+j*width] );
      gdImageSetPixel( gd_, i+xlo-1, static_cast<int>(yMax_)-ylo-j+1, gdColor_ );
    }
  }
}

void GRA_gd::GetTextWidthHeight( GRA_simpleText *st, double xConvert, double yConvert,
                                 double &width, double &height )
{
  int mHeight = static_cast<int>(st->GetHeight()*yConvert*heightCorrection_+0.5);
  int brect[8];
  ExString fontName;
  try
  {
    fontName = GRA_fontControl::GetFontPath( st->GetFont()->GetName() );
  }
  catch (EGraphicsError const &e)
  {
    throw;
  }
  char *gdError = gdImageStringFT( 0, &brect[0], 0, const_cast<char*>(fontName.c_str()),
                                   mHeight, 0.0, 0, 0,
                                   const_cast<char*>(st->GetString().c_str()) );
  if( gdError )throw EGraphicsError(
    ExString("GRA_gd: unable to create string ")+st->GetString()+", error: "+gdError );
  width = (brect[2]-brect[6])/xConvert + st->GetXShift();
  height = (brect[3]-brect[7])/yConvert;
}

void GRA_gd::Draw( GRA_drawableText *dt )
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
    double width, height;
    try
    {
      GetTextWidthHeight( *i, xConvert, yConvert, width, height );
    }
    catch (EGraphicsError &e)
    {
      throw;
    }
    length += width;
    maxHeight = std::max( maxHeight, (*i)->GetHeight() );
  }
  double angle, xLoc, yLoc, sinx, cosx;
  AlignXY( dt, length, maxHeight, angle, xLoc, yLoc, sinx, cosx );
  //
  angle *= M_PI/180.; // convert to radians
  length = 0.0;
  for( std::vector<GRA_simpleText*>::const_iterator i=strings.begin(); i!=end; ++i )
  {
    xLoc += length*cosx - (*i)->GetYShift()*sinx;
    yLoc += length*sinx + (*i)->GetYShift()*cosx;
    double height;
    GetTextWidthHeight( *i, xConvert, yConvert, length, height );
    double x, y;
    WorldToOutputType( xLoc, yLoc, x, y );
    //
    SetColor( (*i)->GetColor() );
    //
    int brect[8];
    int mHeight = static_cast<int>(heightCorrection_*(*i)->GetHeight()*yConvert+0.5);
    ExString fontName;
    try
    {
      fontName = GRA_fontControl::GetFontPath( (*i)->GetFont()->GetName() );
    }
    catch (EGraphicsError const &e)
    {
      throw;
    }
    char *gdError = gdImageStringFT( gd_, &brect[0], gdColor_,
                                     const_cast<char*>(fontName.c_str()),
                                     mHeight, angle, static_cast<int>(x), static_cast<int>(y),
                                     const_cast<char*>((*i)->GetString().c_str()) );
    if( gdError )
      throw EGraphicsError( ExString("GRA_gd: unable to create string ")+(*i)->GetString() );
  }
}

// end of file
