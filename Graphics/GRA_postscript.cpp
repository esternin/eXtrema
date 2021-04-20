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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GRA_postscript.h"
#include "GRA_window.h"
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
#include "GRA_fontControl.h"
#include "GRA_simpleText.h"
#include "GRA_drawableText.h"
#include "ExGlobals.h"

double GRA_postscript::dotsPerInch_ = 72.0;

GRA_postscript::GRA_postscript( char const *filename )
    : GRA_outputType(), filename_(filename), lineWidth_(0)
{
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetWorldLimits( xmin, ymin, xmax, ymax );
  SetUpTransformationMatrices( xmin, ymin, xmax, ymax );
  Initialize();
}

GRA_postscript::~GRA_postscript()
{ outFile_.close(); }

void GRA_postscript::Initialize()
{
  ExString extension( filename_.ExtractFileExt() );
  if( extension.empty() )filename_ += ExString(".eps");
  outFile_.open( filename_.c_str(), ios_base::out );
  if( !outFile_.is_open() )
  {
    ExString mess( "GRA_postscript: unable to open " );
    mess += filename_;
    throw EGraphicsError( mess );
  }
  //
  // determine the bounding box dimensions
  //
  int const xLowerLeft = static_cast<int>(xMin_*dotsPerInch_+0.5);
  int const yLowerLeft = static_cast<int>(yMin_*dotsPerInch_+0.5);
  int const xUpperRight = static_cast<int>(xMax_*dotsPerInch_+0.5);
  int const yUpperRight = static_cast<int>(yMax_*dotsPerInch_+0.5);
  outFile_ << "%!PS-Adobe-3.0 EPSF-3.0\n"
           << "%%BoundingBox: " << xLowerLeft << " " << yLowerLeft << " "
           << xUpperRight << " " << yUpperRight << "\n"
           << "%%Creator: Extrema\n"
           << "%%DocumentData: Clean7Bit\n"
           << "%%LanguageLevel: 2\n"
           << "%%EndComments\n"
           << "%%BeginProlog\n"
           << "1 setlinecap\n"  // round linecap
           << "1 setlinejoin\n" // round linejoin
           << "%%EndProlog\n"
           << "/SetupText {\n"
           << "  /TotalLength 0 def\n"
           << "  /Index 0 def\n"
           << "  ArrayLength {\n"
           << "    /Font TextBuffer Index get 1 get def\n"
           << "    /Height TextBuffer Index get 2 get def\n"
           << "    /XShift TextBuffer Index get 3 get def\n"
           << "    /YShift TextBuffer Index get 4 get def\n"
           << "    /String TextBuffer Index get 5 get def\n"
           << "    Font findfont Height scalefont setfont\n"
           << "    /TotalLength TotalLength String stringwidth pop XShift add add def\n"
           << "    /Index Index 1 add def } repeat\n"
           << "} def\n"
           << "/DrawText {\n"
           << "  Xloc Yloc moveto\n"
           << "  Angle rotate\n"
           << "  /Index 0 def\n"
           << "  ArrayLength {\n"
           << "    /R TextBuffer Index get 0 get 0 get def\n"
           << "    /G TextBuffer Index get 0 get 1 get def\n"
           << "    /B TextBuffer Index get 0 get 2 get def\n"
           << "    /Font TextBuffer Index get 1 get def\n"
           << "    /Height TextBuffer Index get 2 get def\n"
           << "    /XShift TextBuffer Index get 3 get def\n"
           << "    /YShift TextBuffer Index get 4 get def\n"
           << "    /String TextBuffer Index get 5 get def\n"
           << "    Font findfont Height scalefont setfont\n"
           << "    R G B setrgbcolor\n"
           << "    XShift YShift rmoveto\n"
           << "    String show\n"
           << "    /Index Index 1 add def } repeat\n"
           << "  Angle -1 mul rotate\n"
           << "} def\n"
           << "/s {stroke} def\n"
           << "/m {moveto} def\n"
           << "/l {lineto} def\n";
  counter_ = 0;
}

void GRA_postscript::Draw()
{
  try
  {
    ExGlobals::DrawGraphWindows( this );
  }
  catch (EGraphicsError const &e)
  {
    throw EGraphicsError( ExString("GRA_postscript: unable to create PostScript file ")+filename_+"\n"+e.what() );
  }
  outFile_ << "s\n" << "showpage\n" << "%%EOF" << std::endl;
}

void GRA_postscript::WorldToOutputType( double xw, double yw, double &xo, double &yo ) const
{
  GRA_outputType::WorldToOutputType( xw, yw, xo, yo );
  xo *= dotsPerInch_;
  yo *= dotsPerInch_;
}

GRA_color *GRA_postscript::GetColor() const
{
  return GRA_colorControl::GetColor( GRA_colorControl::GetColorCode(r_,g_,b_) );
}

void GRA_postscript::SetColor( GRA_color *c )
{
  if( c == NULL )return;
  int r, g, b;
  c->GetRGB( r, g, b );
  SetColor( r, g, b );
}

void GRA_postscript::SetColor( int i )
{
  int r, g, b;
  GRA_colorControl::ColorCodeToRGB( i, r, g, b );
  SetColor( r, g, b );
}

void GRA_postscript::SetColor( int r, int g, int b )
{
  r_ = r;
  g_ = g;
  b_ = b;
  double dr = static_cast<double>(r)/255.0;
  double dg = static_cast<double>(g)/255.0;
  double db = static_cast<double>(b)/255.0;
  outFile_ << "s\n" << dr << " " << dg << " " << db << " setrgbcolor\n";
}

void GRA_postscript::SetLineWidth( int i )
{
  if( lineWidth_ != i )
  {
    lineWidth_ = i;
    outFile_ << "s\n" << static_cast<double>(i)/4.0 << " setlinewidth" << std::endl;
  }
}

void GRA_postscript::Draw( GRA_point *p )
{
  SetColor( p->GetColor() );
  double x, y;
  p->GetXY( x, y );
  PlotPoint( x, y );
}

void GRA_postscript::Draw( GRA_polyline *pl )
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
  outFile_ << "s\n";
}

void GRA_postscript::Draw( GRA_polygon *p )
{
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  //
  std::vector<double> x, y;
  p->DoClipping( xmin, ymin, xmax, ymax, x, y );
  //
  if( x.empty() )return;
  std::size_t const size = x.size();
  std::vector<int> ix(size), iy(size);
  for( std::size_t i=0; i<size; ++i )
  {
    double ixtmp, iytmp;
    WorldToOutputType( x[i], y[i], ixtmp, iytmp );
    ix[i] = static_cast<int>(ixtmp+0.5);
    iy[i] = static_cast<int>(iytmp+0.5);
  }
  GRA_color *fillColor = p->GetFillColor();
  if( fillColor )
  {
    SetColor( fillColor );
    outFile_ << "newpath " << ix[0] << " " << iy[0] << " m\n";
    for( std::size_t i=1; i<size; ++i )outFile_ << ix[i] << " " << iy[i] << " l\n";
    outFile_ << "closepath fill\n";
  }
  SetLineWidth( 4 );
  SetColor( p->GetLineColor() );
  outFile_ << "newpath " << ix[0] << " " << iy[0] << " m\n";
  for( std::size_t i=1; i<size; ++i )outFile_ << ix[i] << " " << iy[i] << " l\n";
  outFile_ << "closepath\n";
  SetLineWidth( p->GetLineWidth() );
  outFile_ << "s\n";
}

void GRA_postscript::Draw( GRA_multiLineFigure *m )
{
  SetColor( m->GetLineColor() );
  outFile_ << "newpath" << std::endl;
  std::vector<double> x, y;
  std::vector<int> pen;
  m->GetCoordinates( x, y, pen );
  std::size_t size = x.size();
  for( std::size_t i=0; i<size; ++i )
  {
    pen[i]==3 ? PenUp(x[i],y[i]) : PenDown(x[i],y[i]);
  }
  SetLineWidth( m->GetLineWidth() );
  outFile_ << "s\n";
}

void GRA_postscript::Draw( GRA_ellipse *ellipse )
{
  outFile_ << "s\n";
  if( ellipse->IsaCircle() )
  {
    double xp1, yp1, xp2, yp2;
    WorldToOutputType( 0.0, 0.0, xp1, yp1 );
    WorldToOutputType( 0.5*ellipse->GetMajorAxis(), 0.0, xp2, yp2 );
    int iradius = static_cast<int>(sqrt((xp1-xp2)*(xp1-xp2)+(yp1-yp2)*(yp1-yp2))+0.5);
    double xc, yc;
    ellipse->GetCenter( xc, yc );
    double icx, icy;
    WorldToOutputType( xc, yc, icx, icy );
    int ir, ig, ib;
    double dr, dg, db;
    //
    GRA_color *fillColor = ellipse->GetFillColor();
    if( fillColor )
    {
      SetColor( fillColor );
      outFile_ << "newpath " << static_cast<int>(icx+0.5) << " " << static_cast<int>(icy+0.5)
               << " " << iradius << " 0 360 arc fill" << std::endl;
    }
    SetColor( ellipse->GetLineColor() );
    outFile_ << "newpath " << static_cast<int>(icx+0.5) << " " << static_cast<int>(icy+0.5)
             << " " << iradius << " 0 360 arc s\n";
  }
  else
  {
    double majorAxis = ellipse->GetMajorAxis();
    double frac = ellipse->GetMinorAxis()/majorAxis;
    double xp1, yp1, xp2, yp2;
    WorldToOutputType( 0.0, 0.0, xp1, yp1 );
    WorldToOutputType( 0.5*majorAxis, 0.0, xp2, yp2 );
    int iradius = static_cast<int>(sqrt((xp1-xp2)*(xp1-xp2)+(yp1-yp2)*(yp1-yp2))+0.5);
    double xc, yc;
    ellipse->GetCenter( xc, yc );
    double cxp, cyp;
    WorldToOutputType( xc, yc, cxp, cyp );
    double s1, s2, s3, s4;
    int icx, icy;
    double xul, yul, xlr, ylr;
    ellipse->GetLimits( xul, yul, xlr, ylr );
    if( xlr-xul >= yul-ylr )
    {
      s1 = frac;
      s2 = 1.0;
      s3 = 1.0/frac;
      s4 = 1.0;
      icx = static_cast<int>(cxp/frac+0.5);
      icy = static_cast<int>(dotsPerInch_*cyp+0.5);
    }
    else
    {
      s1 = 1.0;
      s2 = frac;
      s3 = 1.0;
      s4 = 1.0/frac;
      icx = static_cast<int>(dotsPerInch_*cxp+0.5);
      icy = static_cast<int>(cyp/frac+0.5);
    }
    outFile_ << s1 << " " << s2 << " scale" << std::endl;
    int ir, ig, ib;
    double dr, dg, db;
    //
    GRA_color *fillColor = ellipse->GetFillColor();
    if( fillColor )
    {
      SetColor( fillColor );
      outFile_ << "newpath " << icx << " " << icy << " " << iradius
               << " 0 360 arc fill" << std::endl;
    }
    SetColor( ellipse->GetLineColor() );
    outFile_ << "newpath " << icx << " " << icy << " " << iradius
             << " 0 360 arc s\n"
             << s3 << " " << s4 << " scale\n";
  }
}

void GRA_postscript::Draw( GRA_plotSymbol *p )
{ p->Draw( this ); }

void GRA_postscript::Draw( GRA_bitmap *bitmap )
{
  double xlo, ylo, xhi, yhi;
  bitmap->GetLimits( xlo, ylo, xhi, yhi );
  double xlod, ylod, xhid, yhid;
  WorldToOutputType( xlo, ylo, xlod, ylod );
  WorldToOutputType( xhi, yhi, xhid, yhid );
  //
  int bwidth = bitmap->GetWidth();
  int bheight = bitmap->GetHeight();
  double xscale = bwidth;
  double yscale = bheight;
  //
  outFile_ << "gsave\n"
           << static_cast<int>(xlod+0.5) << " " << static_cast<int>(ylod+0.5) << " translate\n"
           << xscale << " " << yscale << " scale\n"
           << bwidth << " " << bheight << " 8 [" << bwidth << " 0 0 -"
           << bheight << " 0 " << bheight << "]\n"
           << "{currentfile " << 3*bwidth << " string readhexstring pop} bind\n"
           << "false 3 colorimage\n";
  //
  unsigned int count = 0;
  std::vector<GRA_color*> colors( bitmap->GetBitmap() );
  for( int j=bheight-1; j>=0; --j )
  {
    for( int i=0; i<bwidth; ++i )
    {
      int r, g, b;
      colors[i+j*bwidth]->GetRGB( r, g, b );
      outFile_ << ExString::IntToHex(r).c_str() << ExString::IntToHex(g).c_str()
               << ExString::IntToHex(b).c_str();
      if( ++count == 12 )
      {
        outFile_ << "\n";
        count = 0;
      }
    }
  }
  if( count > 0 )outFile_ << "\n";
  outFile_ << "grestore" << std::endl;
}

void GRA_postscript::GenerateOutput( double x, double y, int pen )
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
    case 3:
    {
      outFile_ << "s newpath " << ix << " " << iy << " m\n";
      counter_ = 0;
      break;
    }
    case 2:
    {
      outFile_ << ix << " " << iy << " l\n";
      if( ++counter_ == 500 )
      {
        outFile_ << "s " << ix << " " << iy << " m\n";
        counter_ = 0;
      }
      break;
    }
    case 20:
    {
      outFile_ << "s newpath " << ix << " " << iy << " m "
               << ix << " " << iy << " l\n";
      counter_ = 0;
      break;
    }
  }
}

void GRA_postscript::Draw( GRA_drawableText *dt )
{
  //bool landscape = ExGlobals::IsLandscape();
  std::vector<GRA_simpleText*> textVec( dt->GetStrings() );
  outFile_ << "/ArrayLength " << textVec.size() << " def" << std::endl;
  outFile_ << "/TextBuffer ArrayLength array def" << std::endl;
  double maxHeight = 0.0;
  std::size_t counter = 0;
  std::vector<GRA_simpleText*>::const_iterator textVecEnd( textVec.end() );
  for( std::vector<GRA_simpleText*>::const_iterator i=textVec.begin(); i!=textVecEnd; ++i, ++counter )
  {
    ExString text( (*i)->GetString() );
    if( text.empty() )continue;
    //
    int r, g, b;
    (*i)->GetColor()->GetRGB( r, g, b );
    double dr = static_cast<double>(r)/255.0;
    double dg = static_cast<double>(g)/255.0;
    double db = static_cast<double>(b)/255.0;
    //
    ExString psFontName;
    try
    {
      psFontName = GRA_fontControl::GetPostScriptFontName( (*i)->GetFont()->GetName() );
    }
    catch( EGraphicsError const &e )
    {
      throw;
    }
    double height = dotsPerInch_*(*i)->GetHeight();
    double xshift = dotsPerInch_*(*i)->GetXShift();
    double yshift = dotsPerInch_*(*i)->GetYShift();
    //maxHeight = std::max( height+fabs(yshift), maxHeight );
    maxHeight = std::max( height, maxHeight );
    outFile_ << "TextBuffer " << counter << " [[" << dr << " " << dg << " " << db
             << "] /" << psFontName.c_str() << " " << height
             << " " << xshift << " " << yshift << " (";
    std::size_t tEnd = text.size();
    for( size_t i=0; i<tEnd; ++i )
    {
      int j = static_cast<int>(text[i]);
      if( j < 0 )j += 256;
      outFile_ << "\\" << setfill('0') << setw(3) << oct << j << dec << setw(0);
    }
    outFile_ << ")] put" << std::endl;
  }
  outFile_ << "SetupText" << std::endl;
  double xLoc = dt->GetX();
  double yLoc = dt->GetY();
  if( dt->GetGraphUnits() ) // convert to world units
  {
    double xg=xLoc, yg=yLoc;
    ExGlobals::GetGraphWindow()->GraphToWorld( xg, yg, xLoc, yLoc, true );
  }
  double angle = dt->GetAngle(); // this is in degrees
  double cosx = cos( angle*M_PI/180.0 );
  double sinx = sin( angle*M_PI/180.0 );
  double const eps = 0.0001;
  if( fabs(cosx) <= eps )cosx = 0.0;
  if( fabs(sinx) <= eps )sinx = 0.0;
  if( fabs(cosx-1.0) <= eps )cosx = 1.0;
  if( fabs(sinx-1.0) <= eps )sinx = 1.0;
  switch ( dt->GetAlignment() )
  {
    case 1:   // lower left
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle " << angle << " def\n"
               << "/Cosx " << cosx << " def\n"
               << "/Sinx " << sinx << " def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5) << " def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5) << " def\n";
      break;
    case 2:   // lower center
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle " << angle << " def\n"
               << "/Cosx " << cosx << " def\n"
               << "/Sinx " << sinx << " def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " TotalLength 2 div Cosx mul sub def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " TotalLength 2 div Sinx mul sub def\n";
      break;
    case 3:   // lower right
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle " << angle << " def\n"
               << "/Cosx " << cosx << " def\n"
               << "/Sinx " << sinx << " def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " TotalLength Cosx mul sub def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " TotalLength Sinx mul sub def\n";
      break;
    case 4:   // center left
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle " << angle << " def\n"
               << "/Cosx " << cosx << " def\n"
               << "/Sinx " << sinx << " def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " MaxHeight 2 div Sinx mul add def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " MaxHeight 2 div Cosx mul sub def\n";
      break;
    case 5:   // center center
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle " << angle << " def\n"
               << "/Cosx " << cosx << " def\n"
               << "/Sinx " << sinx << " def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " TotalLength 2 div Cosx mul sub MaxHeight 2 div Sinx mul add def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " TotalLength 2 div Sinx mul sub MaxHeight 2 div Cosx mul sub def\n";
      break;
    case 6:   // center right
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle " << angle << " def\n"
               << "/Cosx " << cosx << " def\n"
               << "/Sinx " << sinx << " def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " TotalLength Cosx mul sub MaxHeight 2 div Sinx mul add def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " TotalLength Sinx mul sub MaxHeight 2 div Cosx mul sub def\n";
      break;
    case 7:   // upper left
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle " << angle << " def\n"
               << "/Cosx " << cosx << " def\n"
               << "/Sinx " << sinx << " def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " MaxHeight Sinx mul add def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " MaxHeight Cosx mul sub def\n";
      break;
    case 8:   // upper center
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle " << angle << " def\n"
               << "/Cosx " << cosx << " def\n"
               << "/Sinx " << sinx << " def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " TotalLength 2 div Cosx mul sub MaxHeight Sinx mul add def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " TotalLength 2 div Sinx mul sub MaxHeight Cosx mul sub def\n";
      break;
    case 9:   // upper right
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle " << angle << " def\n"
               << "/Cosx " << cosx << " def\n"
               << "/Sinx " << sinx << " def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " TotalLength Cosx mul sub MaxHeight Sinx mul add def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " TotalLength Sinx mul sub MaxHeight Cosx mul sub def\n";
      break;
    case 10:  // lower left at 90deg
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle 90.0 def\n"
               << "/Cosx 0.0 def\n"
               << "/Sinx 1.0 def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " def\n";
      break;
    case 11:  // lower left at 270deg
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle 270.0 def\n"
               << "/Cosx 0.0 def\n"
               << "/Sinx -1.0 def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " def\n";
      break;
    case 12:  // lower center at 90deg
      outFile_ << "/MaxHeight " << static_cast<int>(maxHeight+0.5) << " def\n"
               << "/Angle 90.0 def\n"
               << "/Cosx 0.0 def\n"
               << "/Sinx 1.0 def\n"
               << "/Xloc " << static_cast<int>(xLoc*dotsPerInch_+0.5)
               << " def\n"
               << "/Yloc " << static_cast<int>(yLoc*dotsPerInch_+0.5)
               << " TotalLength 2 div sub def\n";
  }
  //if( landscape )
  //  outFile_ << (90+angle) << " rotate" << std::endl;
  //else
  //  if( angle != 0.0 )outFile_ << angle << " rotate" << std::endl;
  outFile_ << "DrawText" << std::endl;
  //if( landscape )
  //  outFile_ << (-(90+angle)) << " rotate" << std::endl;
  //else
  //  if( angle != 0.0 )outFile_ << (-angle) << " rotate" << std::endl;
}

// end of file
