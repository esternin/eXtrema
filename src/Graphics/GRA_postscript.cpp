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
#include <limits>
#include <iomanip>
#include <memory>

#include "GRA_postscript.h"
#include "GRA_window.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_point.h"
#include "GRA_polyline.h"
#include "GRA_polygon.h"
#include "GRA_ellipse.h"
#include "GRA_multiLineFigure.h"
#include "GRA_rectangle.h"
#include "GRA_cross.h"
#include "GRA_crossPlus.h"
#include "GRA_plus.h"
#include "GRA_diamond.h"
#include "GRA_diamondPlus.h"
#include "GRA_asterisk.h"
#include "GRA_triangle.h"
#include "GRA_ellipse.h"
#include "GRA_star5pt.h"
#include "GRA_arrow3.h"
#include "GRA_plotSymbol.h"
#include "GRA_bitmap.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "GRA_simpleText.h"
#include "GRA_drawableText.h"
#include "GRA_axis.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "GRA_polarAxes.h"
#include "GRA_polarCurve.h"
#include "GRA_errorBar.h"
#include "GRA_characteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_legend.h"
#include "GRA_contour.h"
#include "GRA_boxPlot.h"
#include "GRA_diffusionPlot.h"
#include "GRA_ditheringPlot.h"
#include "GRA_gradientPlot.h"
#include "ExGlobals.h"
#include "EGraphicsError.h"
#include "UsefulFunctions.h"

double GRA_postscript::dotsPerInch_ = 72.0;

GRA_postscript::~GRA_postscript()
{ outFile_.close(); }

void GRA_postscript::Initialize( wxString const &filename )
{
  filename_ = filename;
  lineWidth_ = 0;
  lineType_ = 1;
  //
  double xminW, yminW, xmaxW, ymaxW;
  ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
  //
  SetDefaultDrawLineTable();
  SetUpTransformationMatrices( xminW, yminW, xmaxW, ymaxW );
  //
  wxString extension( ExGlobals::GetFileExtension(filename_) );
  if( extension.empty() )filename_ << wxT(".eps");
  outFile_.open( filename_.mb_str(wxConvUTF8), std::ios_base::out );
  if( !outFile_.is_open() )
    throw EGraphicsError( wxString()<<wxT("GRA_postscript: unable to open ")<<filename_ );
  //
  outFile_ << "%!PS-Adobe-3.0 EPSF-3.0\n"
           << "%%BoundingBox: "
           << static_cast<int>(xminW*dotsPerInch_+0.5) << " "
           << static_cast<int>(yminW*dotsPerInch_+0.5) << " "
           << static_cast<int>(xmaxW*dotsPerInch_+0.5) << " "
           << static_cast<int>(ymaxW*dotsPerInch_+0.5) << "\n"
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

void GRA_postscript::SetUpTransformationMatrices( double xmin, double ymin, double xmax, double ymax )
{
  xMin_ = xmin;
  yMin_ = ymin;
  xMax_ = xmax;
  yMax_ = ymax;
  //
  double xminW, yminW, xmaxW, ymaxW;
  ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
  //
  //  Calculate the world to outputType transformation
  //
  //            |x|                |xw|
  //            | | = w2oMatrix_ * |  | + w2oShift_
  //            |y|                |yw|
  //
  w2oMatrix_[0][0] = (xMax_-xMin_)/(xmaxW-xminW);
  w2oMatrix_[0][1] = 0.0;
  w2oMatrix_[1][0] = 0.0;
  w2oMatrix_[1][1] = (yMax_-yMin_)/(ymaxW-yminW);
  w2oShift_[0] = xMin_-xminW*w2oMatrix_[0][0];
  w2oShift_[1] = yMin_-yminW*w2oMatrix_[1][1];
  //
  //  Calculate the inverse transformation matrix o2wMatrix_ from
  //  the outputType coordinate system to the world coordinate system:
  //
  //            |xw|                |x|
  //            |  | = o2wMatrix_ * | | + o2wShift_
  //            |yw|                |y|
  //
  //  o2wMatrix_ = inv(w2oMatrix_)
  //  world <-- outputType  =  inv(outputType <-- world)
  //
  double const determinant = w2oMatrix_[0][0]*w2oMatrix_[1][1] -
                             w2oMatrix_[1][0]*w2oMatrix_[0][1];
  o2wMatrix_[0][0] =  w2oMatrix_[1][1]/determinant;
  o2wMatrix_[0][1] = -w2oMatrix_[0][1]/determinant;
  o2wMatrix_[1][0] = -w2oMatrix_[1][0]/determinant;
  o2wMatrix_[1][1] =  w2oMatrix_[0][0]/determinant;
  //
  //  Calculate the inverse translation vector o2wShift_ from the
  //  outputType coordinate system to the world coordinate system
  //
  //  o2wShift_ = -o2wMatrix_ * w2oShift_
  //
  o2wShift_[0] = -o2wMatrix_[0][0]*w2oShift_[0] - o2wMatrix_[0][1]*w2oShift_[1];
  o2wShift_[1] = -o2wMatrix_[1][0]*w2oShift_[0] - o2wMatrix_[1][1]*w2oShift_[1];
}

void GRA_postscript::EndDoc()
{
  outFile_ << "s\n" << "showpage\n" << "%%EOF" << std::endl;
}

int GRA_postscript::GetLineWidth() const
{ return lineWidth_; }

void GRA_postscript::GetLimits( double &xmin, double &ymin, double &xmax, double &ymax ) const
{
  xmin = dotsPerInch_*xMin_;
  ymin = dotsPerInch_*yMin_;
  xmax = dotsPerInch_*xMax_;
  ymax = dotsPerInch_*yMax_;
}

void GRA_postscript::OutputTypeToWorld( int xo, int yo, double &xw, double &yw ) const
{
  xw = o2wMatrix_[0][0]*xo + o2wMatrix_[0][1]*(yMax_-yo) + o2wShift_[0];
  yw = o2wMatrix_[1][0]*xo + o2wMatrix_[1][1]*(yMax_-yo) + o2wShift_[1];
}

void GRA_postscript::WorldToOutputType( double xw, double yw, double &xo, double &yo ) const
{
  xo = dotsPerInch_*(w2oMatrix_[0][0]*xw + w2oMatrix_[0][1]*yw + w2oShift_[0]);
  yo = dotsPerInch_*(w2oMatrix_[1][0]*xw + w2oMatrix_[1][1]*yw + w2oShift_[1]);
}

GRA_color *GRA_postscript::GetColor() const
{
  return GRA_colorControl::GetColor( GRA_colorControl::GetColorCode(r_,g_,b_) );
}

void GRA_postscript::SetColor( GRA_color *c )
{
  if( !c )return;
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
    outFile_ << static_cast<double>(i) << " setlinewidth\n";
  }
}

void GRA_postscript::SetLineType( int i )
{
  lineType_ = std::max(1,std::min(10,i)); // illegal type defaults to solid line
}

int GRA_postscript::GetLineType() const
{
  return lineType_;
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

void GRA_postscript::Plot( double x, double y, int pen )
{
  // (x,y) are the world coordinates to which the pen is to be moved either up
  // (pen=3) or down (pen=2). The plot coordinates (x,y) can have any value
  // positive or negative, but are clipped outside the plot windows.
  //
  switch (pen)
  {
    case 3:   // move with pen up to (x,y)
    {
      PenUp( x, y );
      break;
    }
    case 2:   // move with pen down to (x,y)
    {
      PenDown( x, y );
      break;
    }
    case 20:  // plot a point at (x,y)
    {
      PlotPoint( x, y );
      break;
    }
  }
}

void GRA_postscript::PlotPoint( double x, double y )
{
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  if( (x-xmin)*(xmax-x)>=0.0 && (y-ymin)*(ymax-y)>=0.0 )GenerateOutput( x, y, 20 );
  xPrevious_ = x;    // set previous coordinates to new coordinates
  yPrevious_ = y;
  penPrevious_ = 20;
}

void GRA_postscript::PenUp( double x, double y )
{
  // penUp moves with the pen up (not drawing) to the plot coordinates (x,y)
  //
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  if( (x-xmin)*(xmax-x) >=0.0 && (y-ymin)*(ymax-y)>=0.0 )GenerateOutput( x, y, 3 );
  xPrevious_ = x;    // set previous coordinates to new coordinates
  yPrevious_ = y;
  penPrevious_ = 3;
}

void GRA_postscript::PenDown( double x, double y )
{
  // PenDown draws a line to the world coordinates (x,y)
  //
  // clip the line segment (xPrevious_,yPrevious_)-->(x,y) within the clipping boundary
  //
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  if( x>=xmin && xPrevious_>=xmin && x<=xmax && xPrevious_<=xmax &&
      y>=ymin && yPrevious_>=ymin && y<=ymax && yPrevious_<=ymax )
  {
    // (x,y) and (xPrevious_,yPrevious_) are both inside the clipping boundary
    //
    GenerateOutput( x, y, 2 );
  }
  else
  {
    int ndraw = 1;
    double xdraw[2], ydraw[2];
    xdraw[0] = x;
    ydraw[0] = y;
    UsefulFunctions::WindowClip( xPrevious_, yPrevious_, x, y,
                                 xmin, xmax, ymin, ymax,
                                 xdraw, ydraw, ndraw );
    switch (ndraw)
    {
      case 1:   //(x,y) is outside but (xPrevious_,yPrevious_) is in
      {
        GenerateOutput( xdraw[0], ydraw[0], 2 );
        break;
      }
      case 2:   // (x,y) and (xPrevious_,yPrevious_) are both outside
      case 3:   // (xPrevious_,yPrevious_) is outside but (x,y) is inside
      {
        GenerateOutput( xdraw[0], ydraw[0], 3 );
        GenerateOutput( xdraw[1], ydraw[1], 2 );
        break;
      }
    }
  }
  xPrevious_ = x;    // set previous coordinates to new coordinates
  yPrevious_ = y;
  penPrevious_ = 2;
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
  std::vector<double> const &x = pl->GetX();
  std::vector<double> const &y = pl->GetY();
  std::vector<int> const &pen = pl->GetPen();
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
  SetLineWidth( p->GetLineWidth() );
  SetColor( p->GetLineColor() );
  outFile_ << "newpath " << ix[0] << " " << iy[0] << " m\n";
  for( std::size_t i=1; i<size; ++i )outFile_ << ix[i] << " " << iy[i] << " l\n";
  outFile_ << "closepath\n" << "s\n";
}

void GRA_postscript::Draw( GRA_multiLineFigure *m )
{
  SetColor( m->GetLineColor() );
  outFile_ << "newpath\n";
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
               << " " << iradius << " 0 360 arc fill\n";
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
    outFile_ << s1 << " " << s2 << " scale\n";
    int ir, ig, ib;
    double dr, dg, db;
    //
    GRA_color *fillColor = ellipse->GetFillColor();
    if( fillColor )
    {
      SetColor( fillColor );
      outFile_ << "newpath " << icx << " " << icy << " " << iradius
               << " 0 360 arc fill\n";
    }
    SetColor( ellipse->GetLineColor() );
    outFile_ << "newpath " << icx << " " << icy << " " << iradius
             << " 0 360 arc s\n"
             << s3 << " " << s4 << " scale\n";
  }
}

void GRA_postscript::Draw( GRA_plotSymbol *p )
{
  int shapeCode = p->GetShapeCode();
  if( shapeCode==0 || shapeCode==32 )return;
  double x, y;
  p->GetLocation( x, y );
  double size = p->GetSize();
  double angle = p->GetAngle();
  GRA_color *color = p->GetColor();
  int lineWidth = p->GetLineWidth();
  switch( shapeCode )
  {
    case 1:  // square
    {
      GRA_rectangle r(x,y,size,size,angle,color,0,lineWidth);
      Draw( static_cast<GRA_polygon*>(&r) );
      break;
    }
    case 2:  // cross
    {
      GRA_cross c(x,y,size,angle,color,lineWidth);
      Draw( static_cast<GRA_multiLineFigure*>(&c) );
      break;
    }
    case 3:  // square with cross
    {
      GRA_crossPlus cp(x,y,size,angle,color,lineWidth);
      Draw( static_cast<GRA_multiLineFigure*>(&cp) );
      break;
    }
    case 4:  // plus
    {
      GRA_plus p(x,y,size,angle,color,lineWidth);
      Draw( static_cast<GRA_multiLineFigure*>(&p) );
      break;
    }
    case 5:  // diamond
    {
      GRA_diamond d(x,y,size,angle,color,0,lineWidth);
      Draw( static_cast<GRA_polygon*>(&d) );
      break;
    }
    case 6:  // diamond +
    {
      GRA_diamondPlus dp(x,y,size,angle,color,lineWidth);
      Draw( static_cast<GRA_multiLineFigure*>(&dp) );
      break;
    }
    case 7:  // asterisk
    {
      GRA_asterisk a(x,y,size,angle,color,lineWidth);
      Draw( static_cast<GRA_multiLineFigure*>(&a) );
      break;
    }
    case 8:  // triangle
    {
      GRA_triangle t(x,y,size,angle,color,0,lineWidth);
      Draw( static_cast<GRA_polygon*>(&t) );
      break;
    }
    case 9:  // circle
    {
      double radius = size*0.5;
      GRA_ellipse e(x-radius,y-radius,x+radius,y+radius,true,color,0,lineWidth,1);
      Draw( &e );
      break;
    }
    case 10: // star
    {
      GRA_star5pt s5(x,y,size,angle,color,0,lineWidth);
      Draw( static_cast<GRA_polygon*>(&s5) );
      break;
    }
    case 11: // point
    {
      PlotPoint( x, y );
      break;
    }
    case 12: // arrow with foot at (x,y)
    {
      GRA_arrow3 a3(x,y,size,0.0,angle,color,lineWidth);
      Draw( static_cast<GRA_multiLineFigure*>(&a3) );
      break;
    }
    case 13: // arrow centred at (x,y)
    {
      GRA_arrow3 a3(x,y,size,0.5*size,angle,color,lineWidth);
      Draw( static_cast<GRA_multiLineFigure*>(&a3) );
      break;
    }
    case 14:  // filled square
    {
      GRA_rectangle r(x,y,size,size,angle,color,color,lineWidth);
      Draw( static_cast<GRA_polygon*>(&r) );
      break;
    }
    case 15:  // filled diamond
    {
      GRA_diamond d(x,y,size,angle,color,color,lineWidth);
      Draw( static_cast<GRA_polygon*>(&d) );
      break;
    }
    case 16:  // filled triangle
    {
      GRA_triangle t(x,y,size,angle,color,color,lineWidth);
      Draw( static_cast<GRA_polygon*>(&t) );
      break;
    }
    case 17:  // filled circle
    {
      double radius = size*0.5;
      GRA_ellipse e(x-radius,y-radius,x+radius,y+radius,true,color,color,lineWidth,1);
      Draw( &e );
      break;
    }
    case 18:  // filled star
    {
      GRA_star5pt s5(x,y,size,angle,color,color,lineWidth);
      Draw( static_cast<GRA_polygon*>(&s5) );
      break;
    }
  }
}

void GRA_postscript::Draw( GRA_axis *axis )
{
  std::vector<GRA_polyline*> &polylines( axis->GetPolylines() );
  std::vector<GRA_polyline*>::const_iterator pEnd = polylines.end();
  for( std::vector<GRA_polyline*>::const_iterator i=polylines.begin(); i!=pEnd; ++i )Draw( *i );
  std::vector<GRA_drawableText*> &textVec( axis->GetDrawableText() );
  std::vector<GRA_drawableText*>::const_iterator tEnd = textVec.end();
  for( std::vector<GRA_drawableText*>::const_iterator i=textVec.begin(); i!=tEnd; ++i )Draw( *i );
}

void GRA_postscript::Draw( GRA_cartesianAxes *cartesianAxes )
{
  GRA_axis *xAxis, *yAxis, *boxXAxis, *boxYAxis;
  cartesianAxes->GetAxes( xAxis, yAxis, boxXAxis, boxYAxis );
  Draw( xAxis );
  Draw( yAxis );
  if( boxXAxis )Draw( boxXAxis );
  if( boxYAxis )Draw( boxYAxis );
  //
  GRA_setOfCharacteristics *xAxisC = xAxis->GetCharacteristics();
  GRA_setOfCharacteristics *yAxisC = yAxis->GetCharacteristics();
  //
  // draw the x-axis grid lines, if requested
  //
  lineType_ = cartesianAxes->GetGridLineType();
  
  SetColor( static_cast<GRA_colorCharacteristic*>(xAxisC->Get(wxT("AXISCOLOR")))->Get() );
  
  double xlaxis, ylaxis, xuaxis, yuaxis;
  xAxis->GetOrigin( xlaxis, ylaxis );
  xAxis->GetEndPoint( xuaxis, yuaxis );
  int nGrid = xAxis->GetGrid();
  double axisAngle = xAxis->GetAngle();
  //
  std::vector<double> xTicCoordinates, yTicCoordinates;
  xAxis->GetTicCoordinates( xTicCoordinates, yTicCoordinates );
  std::size_t size = xTicCoordinates.size();
  double const eps = 0.001;
  //
  // grid lines are drawn on every nxgrid_th (sic) long tic mark on the x-axis
  //
  for( std::size_t i=0; i<size; i+=nGrid )
  {
    double x = xTicCoordinates[i];
    double y = yTicCoordinates[i];
    if( fabs(x-xuaxis)>eps && fabs(x-xlaxis)>eps ) // don't draw on top of y-axis
    {
      StartLine( x, y );
      ContinueLine( x, y+(yuaxis-ylaxis) );
    }
  }
  //
  // draw the y-axis grid lines, if requested
  //
  SetColor( static_cast<GRA_colorCharacteristic*>(yAxisC->Get(wxT("AXISCOLOR")))->Get() );
  
  yAxis->GetOrigin( xlaxis, ylaxis );
  yAxis->GetEndPoint( xuaxis, yuaxis );
  nGrid = yAxis->GetGrid();
  axisAngle = yAxis->GetAngle();
  //
  xTicCoordinates.clear();
  yTicCoordinates.clear();
  yAxis->GetTicCoordinates( xTicCoordinates, yTicCoordinates );
  size = xTicCoordinates.size();
  //
  // grid lines are drawn on every nxgrid_th (sic) long tic mark on the x-axis
  //
  for( std::size_t i=0; i<size; i+=nGrid )
  {
    double x = xTicCoordinates[i];
    double y = yTicCoordinates[i];
    if( fabs(y-yuaxis)>eps && fabs(y-ylaxis)>eps ) // don't draw on top of x-axis
    {
      StartLine( x, y );
      ContinueLine( x+(xuaxis-xlaxis), y );
    }
  }
  // draw the x-axis automatic label
  //
  wxString label;
  bool labelIsOn = static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("LABELON")))->Get();
  if( labelIsOn )label = static_cast<GRA_stringCharacteristic*>(xAxisC->Get(wxT("LABEL")))->Get();
  if( label.empty() )labelIsOn = false;
  double power = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("POWER")))->Get();
  if( static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("LOGBASE")))->Get()<=1.0 &&
      power != 0.0 &&
      static_cast<GRA_intCharacteristic*>(xAxisC->Get(wxT("POWERAUTO")))->Get()!=2 &&
      static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("NUMBERSON")))->Get() )
  {
    wxString powLabel(wxT("(x10<^><Z0.5%>"));
    powLabel << power << wxT("<_>)");
    label.empty() ? label=powLabel : label << wxT(" ") << powLabel;
    labelIsOn = true;
  }
  //
  // the axis label is composed of two parts:
  //  a) the string found in the label
  //  b) the factor by which all the numbers
  //     labelling the axis should be multiplied to get the graph axis units
  //
  // if powerAuto_ = 2, calculate the power but do not draw it
  //
  if( labelIsOn )
  {
    double sizlab = static_cast<GRA_sizeCharacteristic*>(xAxisC->Get(wxT("LABELHEIGHT")))->GetAsWorld();
    double imagTicAngle = static_cast<GRA_angleCharacteristic*>(xAxisC->Get(wxT("IMAGTICANGLE")))->Get();
    double imagTicLen = static_cast<GRA_sizeCharacteristic*>(xAxisC->Get(wxT("IMAGTICLENGTH")))->GetAsWorld();
    double numHeight = static_cast<GRA_sizeCharacteristic*>(xAxisC->Get(wxT("NUMBERSHEIGHT")))->GetAsWorld();
    GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(xAxisC->Get(wxT("LABELFONT")))->Get();
    GRA_color *labelColor = static_cast<GRA_colorCharacteristic*>(xAxisC->Get(wxT("LABELCOLOR")))->Get();
    double x1, y1, angle=0.0;
    int align = 2;
    double xOrigin, yOrigin;
    xAxis->GetOrigin( xOrigin, yOrigin );
    //
    // draw the label horizontally and centered on the x-axis
    x1 = xOrigin + 0.5*xAxis->GetLength();
    if( imagTicAngle > 180.0 )
    {
      align = 8; // 8=top centre
      y1 = yOrigin - 1.05*(imagTicLen+numHeight);
    }
    else
    {
      align = 2; // 2=bottom centre
      y1 = yOrigin + 1.05*(imagTicLen+numHeight);
    }
    GRA_drawableText *dt = new GRA_drawableText(label,sizlab,angle,x1,y1,align,labelFont,labelColor);
    try
    {
      dt->Parse();
    }
    catch( EGraphicsError const &e )
    {
      delete dt;
      throw;
    }
    Draw( dt );
  }
  //
  // draw the y-axis automatic label
  //
  labelIsOn = static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("LABELON")))->Get();
  label.clear();
  if( labelIsOn )label = static_cast<GRA_stringCharacteristic*>(yAxisC->Get(wxT("LABEL")))->Get();
  if( label.empty() )labelIsOn = false;
  power = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("POWER")))->Get();
  if( static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("LOGBASE")))->Get()<=1.0 &&
      power != 0.0 &&
      static_cast<GRA_intCharacteristic*>(yAxisC->Get(wxT("POWERAUTO")))->Get()!=2 &&
      static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("NUMBERSON")))->Get() )
  {
    wxString powLabel(wxT("(x10<^><Z0.5%>"));
    powLabel << power << wxT("<_>)");
    label.empty() ? label=powLabel : label << wxT(" ") << powLabel;
    labelIsOn = true;
  }
  //
  // the axis label is composed of two parts:
  //  a) the string found in the label
  //  b) the factor by which all the numbers
  //     labelling the axis should be multiplied to get the graph axis units
  //
  // if powerAuto_ = 2, calculate the power but do not draw it
  //
  if( labelIsOn )
  {
    double sizlab = static_cast<GRA_sizeCharacteristic*>(yAxisC->Get(wxT("LABELHEIGHT")))->GetAsWorld();
    double imagTicAngle = static_cast<GRA_angleCharacteristic*>(yAxisC->Get(wxT("IMAGTICANGLE")))->Get();
    double imagTicLen = static_cast<GRA_sizeCharacteristic*>(yAxisC->Get(wxT("IMAGTICLENGTH")))->GetAsWorld();
    GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(yAxisC->Get(wxT("LABELFONT")))->Get();
    GRA_color *labelColor = static_cast<GRA_colorCharacteristic*>(yAxisC->Get(wxT("LABELCOLOR")))->Get();
    double x1, y1, angle=0.0;
    int align = 2;
    double xOrigin, yOrigin;
    yAxis->GetOrigin( xOrigin, yOrigin );
    double numWidth = yAxis->GetMaxWidth();
    //
    // draw the label vertically and centered on the y-axis
    //
    y1 = yOrigin + 0.5*yAxis->GetLength();
    if( imagTicAngle > 180.0 )
    {
      x1 = xOrigin + 1.05*(imagTicLen+numWidth);
      angle = 270.0;
    }
    else
    {
      x1 = xOrigin - 1.05*(imagTicLen+numWidth);
      angle = 90.0;
    }
    GRA_drawableText *dt = new GRA_drawableText(label,sizlab,angle,x1,y1,align,labelFont,labelColor);
    try
    {
      dt->Parse();
    }
    catch( EGraphicsError const &e )
    {
      delete dt;
      throw;
    }
    Draw( dt );
  }
}

void GRA_postscript::Draw( GRA_cartesianCurve *cartesianCurve )
{
  double xlaxis, ylaxis, xuaxis, yuaxis;
  cartesianCurve->GetClippingBoundary( xlaxis, ylaxis, xuaxis, yuaxis );
  ExGlobals::SetClippingBoundary( xlaxis, ylaxis, xuaxis, yuaxis );
  GRA_color *areaFillColor = cartesianCurve->GetAreaFillColor();
  GRA_color *color = cartesianCurve->GetColor();
  int lineWidth = cartesianCurve->GetLineWidth();
  int lineType = cartesianCurve->GetLineType();
  std::vector<double> xCurve, yCurve;
  cartesianCurve->GetXYcurve( xCurve, yCurve );
  switch ( cartesianCurve->GetHistogramType() )
  {
    case 1:
    case 3:
    {
      // draw histogram with no tails
      //
      std::size_t size = xCurve.size();
      if( areaFillColor && size>1 )
      {
        // draw the filled region
        // make sure polygon is closed, add last point set to first point
        //
        xCurve.push_back( xCurve.front() );
        yCurve.push_back( yCurve.front() );
        GRA_polygon p( xCurve, yCurve, color, areaFillColor, lineWidth );
        Draw( &p );
        xCurve.erase( xCurve.end()-1 );
        yCurve.erase( yCurve.end()-1 );
      }
      SetColor( color );
      SetLineWidth( lineWidth );
      PenUp( xCurve[0], yCurve[0] );
      for( std::size_t i=1; i<size; ++i )PenDown( xCurve[i], yCurve[i] );
      break;
    }
    case 2:
    case 4:
    {
      // draw histogram with tails
      //
      std::size_t npt = cartesianCurve->GetXData().size();
      if( npt > 1 )
      {
        std::vector<double> xp, yp;
        std::vector<GRA_color*> areaFillColors( cartesianCurve->GetAreaFillColors() );
        bool areaFillColorVector = !areaFillColors.empty();
        for( std::size_t i=0; i<npt; ++i )
        {
          GRA_color *fillColor = areaFillColorVector ? areaFillColors[i%areaFillColors.size()] : areaFillColor;
          if( fillColor )
          {
            std::size_t j = 4*i;
            xp.push_back( xCurve[j] );
            yp.push_back( yCurve[j] );
            xp.push_back( xCurve[j+1] );
            yp.push_back( yCurve[j+1] );
            xp.push_back( xCurve[j+2] );
            yp.push_back( yCurve[j+2] );
            xp.push_back( xCurve[j+3] );
            yp.push_back( yCurve[j+3] );
            xp.push_back( xCurve[j] );
            yp.push_back( yCurve[j] );
            GRA_polygon p( xp, yp, color, fillColor, lineWidth );
            Draw( &p );
            std::vector<double>().swap( xp );
            std::vector<double>().swap( yp );
          }
        }
      }
      SetColor( color );
      SetLineWidth( lineWidth );
      if( npt == 1 )
      {
        PenUp( xCurve[0], yCurve[0] );
        PenDown( xCurve[1], yCurve[1] );
      }
      else
      {
        for( std::size_t i=0; i<npt; ++i )
        {
          std::size_t j = 4*i;
          PenUp( xCurve[j], yCurve[j] );
          PenDown( xCurve[j+1], yCurve[j+1] );
          PenDown( xCurve[j+2], yCurve[j+2] );
          PenDown( xCurve[j+3], yCurve[j+3] );
          PenDown( xCurve[j],   yCurve[j] );
        }
      }
      break;
    }
    default:
    {
      // draw non-histogram
      //
      if( areaFillColor )
      {
        // draw the filled region
        // make sure polygon is closed, may have to add last point set to first point
        //
        bool test = false;
        if( xCurve.front()!=xCurve.back() || yCurve.front()!=yCurve.back() )
        {
          test = true;
          xCurve.push_back( xCurve.front() );
          yCurve.push_back( yCurve.front() );
        }
        GRA_polygon p( xCurve, yCurve, color, areaFillColor, lineWidth );
        Draw( &p );
        if( test )
        {
          xCurve.erase( xCurve.end()-1 );
          yCurve.erase( yCurve.end()-1 );
        }
      }
      // draw the data curve itself (which may be smoothed curve)
      //
      SetLineType( lineType );
      SetColor( color );
      SetLineWidth( lineWidth );
      std::size_t size = xCurve.size();
      std::vector<int> pen( cartesianCurve->GetPen() );
      for( std::size_t i=0; i<size; ++i )
      {
        pen[i]==2 ? ContinueLine( xCurve[i], yCurve[i] ) : StartLine( xCurve[i], yCurve[i] );
      }
      //
      // draw the plot symbols at the data coordinates
      //
      double xmin, ymin, xmax, ymax;
      ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
      std::vector<GRA_plotSymbol*> plotsymbols( cartesianCurve->GetPlotSymbols() );
      for( std::size_t i=0; i<size; ++i )
      {
        if( xmin<=xCurve[i] && xCurve[i]<=xmax && ymin<=yCurve[i] && yCurve[i]<=ymax )
        {
          //plotsymbols[i]->SetLocation( xCurve[i], yCurve[i] );
          Draw( plotsymbols[i] );
        }
      }
    }
  }
  // draw error bars
  //
  std::vector<GRA_errorBar*> errorBars( cartesianCurve->GetErrorBars() );
  if( !errorBars.empty() )
  {
    std::vector<GRA_errorBar*>::const_iterator end = errorBars.end();
    for( std::vector<GRA_errorBar*>::const_iterator i=errorBars.begin(); i!=end; ++i )
      Draw( static_cast<GRA_multiLineFigure*>(*i) );
  }
  ExGlobals::ResetClippingBoundary();
}

void GRA_postscript::Draw( GRA_polarAxes *polarAxes )
{
  // draw the 0 degree radial axis
  //
  GRA_axis *axis = polarAxes->GetAxis();
  Draw( axis );
  //
  // draw the automatic axis label 
  //
  GRA_setOfCharacteristics *axisC = axis->GetCharacteristics();
  //
  wxString label;
  bool labelIsOn = static_cast<GRA_boolCharacteristic*>(axisC->Get(wxT("LABELON")))->Get();
  if( labelIsOn )label = static_cast<GRA_stringCharacteristic*>(axisC->Get(wxT("LABEL")))->Get();
  if( label.empty() )labelIsOn = false;
  //
  // if powerAuto_ = 2, calculate the power but do not draw it
  //
  double power = static_cast<GRA_doubleCharacteristic*>(axisC->Get(wxT("POWER")))->Get();
  if( static_cast<GRA_intCharacteristic*>(axisC->Get(wxT("POWERAUTO")))->Get()!=2 &&
      static_cast<GRA_boolCharacteristic*>(axisC->Get(wxT("NUMBERSON")))->Get() && power!=0.0 )
  {
    wxString powLabel(wxT("(x10<^><Z0.5%>"));
    label << power << wxT("<_>)");
    label = label.empty() ? powLabel : label+wxT(" ")+powLabel;
    labelIsOn = true;
  }
  //
  // the axis label is composed of two parts:
  //  a) the string found in the label
  //  b) the factor by which all the numbers
  //     labelling the axis should be multiplied to get the graph axis units
  //
  if( labelIsOn )
  {
    double sizlab = static_cast<GRA_sizeCharacteristic*>(axisC->Get(wxT("LABELHEIGHT")))->GetAsWorld();
    double imagTicAngle = static_cast<GRA_angleCharacteristic*>(axisC->Get(wxT("IMAGTICANGLE")))->Get();
    double imagTicLen = static_cast<GRA_sizeCharacteristic*>(axisC->Get(wxT("IMAGTICLENGTH")))->GetAsWorld();
    double numHeight = static_cast<GRA_sizeCharacteristic*>(axisC->Get(wxT("NUMBERSHEIGHT")))->GetAsWorld();
    GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(axisC->Get(wxT("LABELFONT")))->Get();
    GRA_color *labelColor = static_cast<GRA_colorCharacteristic*>(axisC->Get(wxT("LABELCOLOR")))->Get();
    double xOrigin, yOrigin;
    axis->GetOrigin( xOrigin, yOrigin );
    double length = axis->GetLength();
    double axisAngle = axis->GetAngle();
    double x1, y1, angle;
    int align;
    //
    // draw the label horizontally and centered on the axis
    if( axisAngle == 0.0 )
    {
      x1 = xOrigin + 0.5*length;
      y1 = yOrigin - 1.05*(imagTicLen+numHeight);
      angle = 0.0;
      align = 8; // 8=top centre
    }
    else
    {
      x1 = xOrigin + 1.1*(imagTicLen+axis->GetMaxWidth());
      y1 = yOrigin + 0.5*length;
      angle = 270.0;
      align = 2; // bottom centre
    }
    GRA_drawableText dt(label,sizlab,angle,x1,y1,align,labelFont,labelColor);
    try
    {
      dt.Parse();
    }
    catch( EGraphicsError const &e )
    {
      throw;
    }
    Draw( &dt );
  }
  //
  // draw the other axes
  //
  std::vector<GRA_axis*> otherAxes( polarAxes->GetOtherAxes() );
  std::size_t nAxes = otherAxes.size();
  for( std::size_t i=0; i<nAxes; ++i )Draw( otherAxes[i] );
  //
  // draw the grid lines
  // if nxgrid is zero, no tic coordinates are recorded, so no grid lines will be drawn
  //
  GRA_color *lineColor = static_cast<GRA_colorCharacteristic*>(axisC->Get(wxT("AXISCOLOR")))->Get();
  double xorigin, yorigin;
  axis->GetOrigin( xorigin, yorigin );
  int gridLineType = polarAxes->GetGridLineType();
  //
  // grid circles are drawn at tic marks on the x-axis
  //
  std::vector<double> ticX, ticY;
  axis->GetTicCoordinates( ticX, ticY );
  double axisAngle = axis->GetAngle();

  double cosAngle = cos(axisAngle*M_PI/180.0);
  double sinAngle = sin(axisAngle*M_PI/180.0);
  if( fabs(cosAngle) < 0.0001 )cosAngle = 0.0;
  if( fabs(sinAngle) < 0.0001 )sinAngle = 0.0;

  std::size_t ticSize = ticX.size();
  std::vector<double> x, y;
  std::vector<int> pen;
  int ngrid = abs(axis->GetGrid());
  for( std::size_t i=0; i<ticSize; i+=ngrid )
  {
    double radius = sqrt( (xorigin-ticX[i])*(xorigin-ticX[i]) +
                          (yorigin-ticY[i])*(yorigin-ticY[i]) );
    for( std::size_t j=0; j<101; ++j )
    {
      double angle = j*M_PI/50.;
      x.push_back( xorigin+radius*cos(angle) );
      y.push_back( yorigin+radius*sin(angle) );
      pen.push_back( 2 );
    }
    GRA_polyline p( x, y, pen, 1, gridLineType, lineColor );
    Draw( &p );
    x.clear();
    y.clear();
  }
  //
  // draw the angles (or the compass directions) at the ends of the axes
  //
  double height = static_cast<GRA_sizeCharacteristic*>(axisC->Get(wxT("LABELHEIGHT")))->GetAsWorld();
  if( height > 0.0 )
  {
    GRA_color *color = static_cast<GRA_colorCharacteristic*>(axisC->Get(wxT("LABELCOLOR")))->Get();
    GRA_font *font = static_cast<GRA_fontCharacteristic*>(axisC->Get(wxT("LABELFONT")))->Get();
    bool clockwise = polarAxes->GetClockwise();
    double xend, yend, angle;
    wxString label;
    if( polarAxes->GetCompassLabels() && (nAxes==0||nAxes==1||nAxes==3||nAxes==7||nAxes==15) )
    {
      axis->GetEndPoint( xend, yend );
      DrawAngle( label<<wxT("N"), xend, yend, axisAngle, height, color, font );
      label.clear();
      switch (nAxes)
      {
        case 1:  // N, S
        {
          otherAxes[0]->GetEndPoint( xend, yend );
          angle = otherAxes[0]->GetAngle();
          DrawAngle( label<<wxT("S"), xend, yend, angle, height, color, font );
          label.clear();
          break;
        }
        case 3:  // N, E, S, W
        {
          std::vector<wxString> labels(3);
          labels[0] = wxT("E");
          labels[1] = wxT("S");
          labels[2] = wxT("W");
          for( int i=0; i<3; ++i )
          {
            otherAxes[i]->GetEndPoint( xend, yend );
            angle = otherAxes[i]->GetAngle();
            if( clockwise )
              DrawAngle( labels[2-i], xend, yend, angle, height, color, font );
            else
              DrawAngle( labels[i], xend, yend, angle, height, color, font );
          }
          break;
        }
        case 7:  // N, NE, E, SE, S, SW, W, NW
        {
          std::vector<wxString> labels(7);
          labels[0] = wxT("NE");
          labels[1] = wxT("E");
          labels[2] = wxT("SE");
          labels[3] = wxT("S");
          labels[4] = wxT("SW");
          labels[5] = wxT("W");
          labels[6] = wxT("NW");
          for( int i=0; i<7; ++i )
          {
            otherAxes[i]->GetEndPoint( xend, yend );
            angle = otherAxes[i]->GetAngle();
            if( clockwise )
              DrawAngle( labels[6-i], xend, yend, angle, height, color, font );
            else
              DrawAngle( labels[i], xend, yend, angle, height, color, font );
          }
          break;
        }
        case 15:
        {
          std::vector<wxString> labels(15);
          labels[0] = wxT("NNE");
          labels[1] = wxT("NE");
          labels[2] = wxT("ENE");
          labels[3] = wxT("E");
          labels[4] = wxT("ESE");
          labels[5] = wxT("SE");
          labels[6] = wxT("SSE");
          labels[7] = wxT("S");
          labels[8] = wxT("SSW");
          labels[9] = wxT("SW");
          labels[10] = wxT("WSW");
          labels[11] = wxT("W");
          labels[12] = wxT("WNW");
          labels[13] = wxT("NW");
          labels[14] = wxT("NNW");
          for( int i=0; i<15; ++i )
          {
            otherAxes[i]->GetEndPoint( xend, yend );
            angle = otherAxes[i]->GetAngle();
            if( clockwise )
              DrawAngle( labels[14-i], xend, yend, angle, height, color, font );
            else
              DrawAngle( labels[i], xend, yend, angle, height, color, font );
          }
          break;
        }
      }
    }
    else
    {
      axis->GetEndPoint( xend, yend );
      DrawAngle( label<<wxT("0<degree>"), xend, yend, axisAngle, height, color, font );
      label.clear();
      for( std::size_t i=0; i<nAxes; ++i )
      {
        angle = otherAxes[i]->GetAngle();
        double drawAngle = angle - axisAngle;
        while( drawAngle < 0.0 )drawAngle += 360.0;
        if( clockwise )drawAngle = 360.0 - drawAngle;
        otherAxes[i]->GetEndPoint( xend, yend );
        DrawAngle( label<<drawAngle<<wxT("<degree>"), xend, yend, angle, height, color, font );
        label.clear();
      }
    }
  }
}

    
void GRA_postscript::DrawAngle( wxString &label, double xend, double yend, double angle, double height,
                                GRA_color *color, GRA_font *font )
{
  int align = 4;
  if( 45 < angle && angle < 135 )       align = 2;
  else if( 135 <= angle && angle < 225 )align = 6;
  else if( 225 <= angle && angle < 315 )align = 8;
  //
  double xoff = 0.2*cos(angle*M_PI/180.);
  double yoff = 0.2*sin(angle*M_PI/180.);
  GRA_drawableText dt( label, height, 0.0, xend+xoff, yend+yoff, align, font, color );
  dt.Parse();
  Draw( &dt );
}

void GRA_postscript::Draw( GRA_polarCurve *polarCurve )
{
  std::vector<double> xCurve, yCurve;
  polarCurve->GetXYcurve( xCurve, yCurve );
  GRA_color *areaFillColor = polarCurve->GetAreaFillColor();
  if( areaFillColor )
  {
    // draw the filled region
    // make sure polygon is closed, may have to add last point set to first point
    //
    bool test = false;
    if( xCurve.front()!=xCurve.back() || yCurve.front()!=yCurve.back() )
    {
      test = true;
      xCurve.push_back( xCurve.front() );
      yCurve.push_back( yCurve.front() );
    }
    GRA_polygon p( xCurve, yCurve, polarCurve->GetColor(), areaFillColor, polarCurve->GetLineWidth() );
    Draw( &p );
    if( test )
    {
      xCurve.erase( xCurve.end()-1 );
      yCurve.erase( yCurve.end()-1 );
    }
  }
  // draw the data curve itself
  //
  SetLineType( polarCurve->GetLineType() );
  SetColor( polarCurve->GetColor() );
  SetLineWidth( polarCurve->GetLineWidth() );
  std::vector<int> pen( polarCurve->GetPen() );
  std::size_t size = xCurve.size();
  for( std::size_t i=0; i<size; ++i )
  {
    pen[i]==2 ? ContinueLine( xCurve[i], yCurve[i] ) : StartLine( xCurve[i], yCurve[i] );
  }
  //
  // draw the plot symbols at the data coordinates
  //
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  std::vector<GRA_plotSymbol*> plotsymbols( polarCurve->GetPlotSymbols() );
  for( std::size_t i=0; i<size; ++i )
  {
    if( xmin<=xCurve[i] && xCurve[i]<=xmax && ymin<=yCurve[i] && yCurve[i]<=ymax )Draw( plotsymbols[i] );
  }
}

void GRA_postscript::Draw( GRA_legend *legend )
{
  GRA_window *gw = legend->GetGraphWindow();
  GRA_setOfCharacteristics *legendC = gw->GetGraphLegendCharacteristics();
  //
  if( static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("FRAMEON")))->Get() && legend->GetFrame() )
    Draw( legend->GetFrame() );
  if( static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("TITLEON")))->Get() && legend->GetTitle() )
    Draw( legend->GetTitle() );
  //
  double xStart = legend->GetLineStart();
  double xEnd = legend->GetLineEnd();
  double xLabel = legend->GetXLabel();
  std::vector<GRA_legendEntry*> entries( legend->GetEntries() );
  std::vector<GRA_legendEntry*>::const_iterator end = entries.end();
  for( std::vector<GRA_legendEntry*>::const_iterator i=entries.begin(); i!=end; ++i )
  {
    double labelHeight = (*i)->GetLabelHeight();
    double y0 = legend->GetYHi() - 1.5*labelHeight*((*i)->GetEntryNumber()+1);
    // first draw the line segment
    //
    double y = y0 + labelHeight*0.5;
    if( (*i)->GetDrawLineSegment() )
    {
      SetLineType( (*i)->GetLineType() );
      SetLineWidth( (*i)->GetLineWidth() );
      SetColor( (*i)->GetLineColor() );
      StartLine( xStart, y );
      ContinueLine( xEnd, y );
    }
    // draw the symbols on the line segment
    //
    int nSymbols = (*i)->GetNSymbols();
    GRA_plotSymbol plotSymbol( *(*i)->GetPlotSymbol() );
    if( nSymbols > 0 )
    {
      if( nSymbols > 1 )
      {
        double xinc = (xEnd-xStart)/(nSymbols-1.);
        for( int i=0; i<nSymbols-1; ++i )
        {
          plotSymbol.SetLocation( xStart+i*xinc, y );
          Draw( &plotSymbol );
        }
        plotSymbol.SetLocation( xEnd, y );
        Draw( &plotSymbol );
      }
      else
      {
        plotSymbol.SetLocation( (xStart+xEnd)/2.0, y );
        Draw( &plotSymbol );
      }
    }
    // draw the label at the end of the line segment
    //
    GRA_drawableText label( *(*i)->GetLabel() );
    label.SetX( xLabel );
    label.SetY( y0 );
    Draw( &label );
  }
}

void GRA_postscript::Draw( GRA_contour *contour )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  double const xuaxis =
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  double const xlaxis =
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  double const yuaxis =
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  double const ylaxis =
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  ExGlobals::SetClippingBoundary( xlaxis, ylaxis, xuaxis, yuaxis );
  SetColor( contour->GetColor() );
  std::vector<double> xPlot, yPlot;
  std::vector<bool> connect;
  contour->GetXYPlot( xPlot, yPlot, connect );
  std::size_t size = xPlot.size();
  for( std::size_t i=0; i<size; ++i )
  {
    connect[i] ? PenDown( xPlot[i], yPlot[i] ) : PenUp( xPlot[i], yPlot[i] );
  }
  std::vector<GRA_drawableText*> *textVec = contour->GetTextVec();
  size = textVec->size();
  for( std::size_t i=0; i<size; ++i )Draw( textVec->at(i) );
  ExGlobals::ResetClippingBoundary();
}

void GRA_postscript::Draw( GRA_boxPlot *boxPlot )
{
  double dataXmin, dataYmin, dataXmax, dataYmax;
  boxPlot->GetDataLimits( dataXmin, dataYmin, dataXmax, dataYmax );
  GRA_cartesianAxes *axes =0;
  if( boxPlot->GetDrawAxes() )
  {
    std::vector<double> xt, yt;
    xt.push_back( dataXmin );
    xt.push_back( dataXmax );
    xt.push_back( dataXmax );
    xt.push_back( dataXmin );
    xt.push_back( dataXmin );
    yt.push_back( dataYmax );
    yt.push_back( dataYmax );
    yt.push_back( dataYmin );
    yt.push_back( dataYmin );
    yt.push_back( dataYmax );
    axes = new GRA_cartesianAxes(xt,yt,false,false);
    try
    {
      axes->Make();
    }
    catch( EGraphicsError const &e )
    {
      delete axes;
      throw;
    }
    Draw( axes );
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *dataC = gw->GetDataCurveCharacteristics();
  double xtmp, ytmp;
  gw->GraphToWorld( 0.0, 0.0, xtmp, ytmp, true );
  double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MIN")))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MAX")))->Get();
  double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MIN")))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MAX")))->Get();
  GRA_color *fillColor = static_cast<GRA_colorCharacteristic*>(generalC->Get(wxT("AREAFILLCOLOR")))->Get();
  GRA_color *lineColor = static_cast<GRA_colorCharacteristic*>(dataC->Get(wxT("CURVECOLOR")))->Get();
  int mlo, nlo, mhi, nhi;
  boxPlot->GetMNLimits( mlo, nlo, mhi, nhi );
  std::vector<double> x, y, z;
  boxPlot->GetXYZ( x, y, z );
  double xside, yside;
  boxPlot->GetSides( xside, yside );
  std::size_t nrow = boxPlot->GetNRow();
  double amin = boxPlot->GetAmin();
  double da = boxPlot->GetDa();
  double fmin, fmax;
  boxPlot->GetFminmax( fmin, fmax );
  if( nrow > 0 ) // using the matrix and not x y z data
  {
    for( int i=mlo-1; i<mhi; ++i )
    {
      for( int j=nlo-1; j<nhi; ++j )
      {
        if( x[i]>xmax || x[i]<xmin || y[j]>ymax || y[j]<ymin )return;
        //
        double const frac = (z[j+i*nrow]-amin)/da;
        if( frac<fmin || frac>fmax )return;
        //
        double xw, yw;
        ExGlobals::GetGraphWindow()->GraphToWorld( x[i], y[j], xw, yw );
        //
        GRA_rectangle r( xw, yw, xside*frac, yside*frac, 0.0, lineColor, fillColor, 1 );
        Draw( &r );
      }
    }
  }
  else               // using x y z data and not the matrix
  {
    int ny = y.size();
    for( int j=0; j<ny; ++j )
    {
      if( (x[j]-xmin)*(xmax-x[j])>=0.0 && (y[j]-ymin)*(ymax-y[j])>=0.0 )
      {
        if( x[j]>xmax || x[j]<xmin || y[j]>ymax || y[j]<ymin )return;
        //
        double const frac = (z[j]-amin)/da;
        if( frac<fmin || frac>fmax )return;
        //
        double xw, yw;
        ExGlobals::GetGraphWindow()->GraphToWorld( x[j], y[j], xw, yw );
        //
        GRA_rectangle r( xw, yw, xside*frac, yside*frac, 0.0, lineColor, fillColor, 1 );
        Draw( &r );
      }
    }
  }
  if( boxPlot->GetBorder() )
  {
    GRA_color *lineColor = static_cast<GRA_colorCharacteristic*>(dataC->Get(wxT("CURVECOLOR")))->Get();
    double x1, y1, x2, y2;
    gw->GraphToWorld( dataXmin, dataYmin, x1, y1, true );
    gw->GraphToWorld( dataXmax, dataYmax, x2, y2 );
    GRA_rectangle r( x1, y1, x2, y2, 0.0, false, lineColor, 0, 1 );
    Draw( &r );
  }
  DrawProfiles( boxPlot );
  if( boxPlot->GetDrawAxes() )
  {
    Draw( axes );
    delete axes;
  }
  if( boxPlot->GetResetAxes() )
  {
    double xuaxisp, yuaxisp;
    boxPlot->GetUpperAxis( xuaxisp, yuaxisp );
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->SetAsPercent( xuaxisp );
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->SetAsPercent( yuaxisp );
  }
}

void GRA_postscript::Draw( GRA_diffusionPlot *diffPlot )
{
  double dataXmin, dataYmin, dataXmax, dataYmax;
  diffPlot->GetDataLimits( dataXmin, dataYmin, dataXmax, dataYmax );
  GRA_cartesianAxes *axes =0;
  //
  // Determine min and max values in array. The minimum value
  // will correspond to zero (blank) print density, the maximum
  // to solid black print
  //
  std::vector<double> x, y, z;
  diffPlot->GetXYZ( x, y, z );
  std::size_t nrow = diffPlot->GetNRow();
  double amin, amax;
  UsefulFunctions::MinMax( z, nrow, 0, x.size(), 0, y.size(), amin, amax );
  //
  double fmin, fmax;
  diffPlot->GetFminmax( fmin, fmax );
  //
  double alo = amin + fmin*(amax-amin);
  double ahi = amin + fmax*(amax-amin);
  if( ahi-alo < std::numeric_limits<double>::epsilon() )
    throw EGraphicsError( wxT("region is flat") );
  diffPlot->SetAlohi( alo, ahi );
  double amean = (amin + amax) / 2.0;
  if( amean == 0.0 )amean = 1.e-10;
  if( fabs((amax-amin)/amean) < 1.0e-5 )
    throw EGraphicsError( wxT("region is flat") );
  try
  {
    diffPlot->SetUp();
  }
  catch( EGraphicsError const &e )
  {
    throw;
  }
  double xlo, ylo, xhi, yhi;
  diffPlot->GetXYLimits( xlo, ylo, xhi, yhi );
  GRA_bitmap bitmap( xlo, ylo, xhi, yhi, GRA_colorControl::GetColor(wxT("WHITE")), (GRA_wxWidgets*)0 );
  diffPlot->FillBitmap( &bitmap );
  try
  {
    Draw( &bitmap );
  }
  catch( EGraphicsError const &e )
  {
    throw;
  }
  if( diffPlot->GetDrawAxes() )
  {
    std::vector<double> xt, yt;
    xt.push_back( dataXmin );
    xt.push_back( dataXmax );
    xt.push_back( dataXmax );
    xt.push_back( dataXmin );
    xt.push_back( dataXmin );
    yt.push_back( dataYmax );
    yt.push_back( dataYmax );
    yt.push_back( dataYmin );
    yt.push_back( dataYmin );
    yt.push_back( dataYmax );
    axes = new GRA_cartesianAxes(xt,yt,false,false);
    try
    {
      axes->Make();
    }
    catch( EGraphicsError const &e )
    {
      delete axes;
      throw;
    }
    Draw( axes );
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *dataC = gw->GetDataCurveCharacteristics();
  if( diffPlot->GetBorder() )
  {
    GRA_color *lineColor =
      static_cast<GRA_colorCharacteristic*>(dataC->Get(wxT("CURVECOLOR")))->Get();
    double x1, y1, x2, y2;
    gw->GraphToWorld( dataXmin, dataYmin, x1, y1, true );
    gw->GraphToWorld( dataXmax, dataYmax, x2, y2 );
    GRA_rectangle r( x1, y1, x2, y2, 0.0, false, lineColor, 0, 1 );
    Draw( &r );
  }

  DrawProfiles( diffPlot );
  if( diffPlot->GetDrawAxes() )
  {
    Draw( axes );
    delete axes;
  }
  if( diffPlot->GetResetAxes() )
  {
    double xuaxisp, yuaxisp;
    diffPlot->GetUpperAxis( xuaxisp, yuaxisp );
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->SetAsPercent( xuaxisp );
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->SetAsPercent( yuaxisp );
  }
}

void GRA_postscript::Draw( GRA_ditheringPlot *dithPlot )
{
  double dataXmin, dataYmin, dataXmax, dataYmax;
  dithPlot->GetDataLimits( dataXmin, dataYmin, dataXmax, dataYmax );
  GRA_cartesianAxes *axes =0;
  std::vector<double> x, y, z;
  dithPlot->GetXYZ( x, y, z );
  std::size_t nrow = dithPlot->GetNRow();
  double amin, amax;
  UsefulFunctions::MinMax( z, nrow, 0, x.size(), 0, y.size(), amin, amax );
  //
  double fmin, fmax;
  dithPlot->GetFminmax( fmin, fmax );
  //
  double alo = amin + fmin*(amax-amin);
  double ahi = amin + fmax*(amax-amin);
  if( ahi-alo < std::numeric_limits<double>::epsilon() )throw EGraphicsError( wxT("region is flat") );
  dithPlot->SetAlohi( alo, ahi );
  double amean = (amin + amax) / 2.0;
  if( amean == 0.0 )amean = 1.e-10;
  if( fabs((amax-amin)/amean) < 1.0e-5 )throw EGraphicsError( wxT("region is flat") );
  //
  try
  {
    dithPlot->SetUp();
  }
  catch( EGraphicsError const &e )
  {
    throw;
  }
  std::size_t const nLevels = dithPlot->GetDither().size()/2;
  if( dithPlot->GetAreas() )dithPlot->GetAreaVec().resize( nLevels, 0.0 );
  if( dithPlot->GetVolumes() )dithPlot->GetVolumeVec().resize( nLevels, 0.0 );
  //
  dithPlot->GetContourVec().resize( nLevels+1, 0.0 );
  dithPlot->GetContourVec().at(0) = alo;
  if( dithPlot->GetContourLevels().empty() )
  {
    for( std::size_t i=1; i<nLevels; ++i )dithPlot->GetContourVec().at(i) = alo+i*(ahi-alo)/nLevels ;
  }
  else
  {
    if( dithPlot->GetContourLevels().at(0) <= amin )
      throw EGraphicsError( wxT("first contour level <= data minimum") );
    if( dithPlot->GetContourLevels().at(nLevels-2) >= amax )
      throw EGraphicsError( wxT("last contour level >= data maximum") );
    for( std::size_t i=2; i<=nLevels; ++i )dithPlot->GetContourVec().at(i-1) =
                                               dithPlot->GetContourLevels().at(i-2);
  }
  dithPlot->GetContourVec().at(nLevels) = ahi;
  //
  if( dithPlot->GetLegend() )
  {
    GRA_window *gw = ExGlobals::GetGraphWindow();
    GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
    GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
    GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
    double xlwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
    double xuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
    double xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
    double ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
    double yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
    double yticl = static_cast<GRA_sizeCharacteristic*>(yAxisC->Get(wxT("LARGETICLENGTH")))->GetAsWorld();
    bool xProfile, yProfile;
    dithPlot->GetProfiles( xProfile, yProfile );
    double xlo = yProfile ? xuaxis+0.1*(xuwind-xlwind)+1.5*yticl : xuaxis+1.5*yticl;
    double xhi = xlo + (xuwind-xlo)/3.0;
    //
    double height = static_cast<GRA_sizeCharacteristic*>(generalC->Get(wxT("LEGENDSIZE")))->GetAsWorld();
    std::size_t nLevels = dithPlot->GetDither().size()/2;
    double yi = (yuaxis-ylaxis)/nLevels;
    //
    if( height > 0.0 )
    {
      for( std::size_t i=0; i<=nLevels; ++i )
      {
        double yp = ylaxis + i*yi;
        wxChar c[50];
        ::wxSnprintf( c, 50, wxT("%9.2le"), dithPlot->GetContourVec().at(i) );
        double angle = 0.0;
        GRA_font *font = static_cast<GRA_fontCharacteristic*>(generalC->Get(wxT("FONT")))->Get();
        wxString stmp(c);
        GRA_drawableText dt( stmp, height, angle, xhi+0.1, yp-height/2., 1,
                             font, GRA_colorControl::GetColor(wxT("BLACK")) );
        try
        {
          dt.Parse();
        }
        catch( EGraphicsError const &e )
        {
          throw;
        }
        Draw( &dt );
      }
    }
    GRA_bitmap bitmap( xlo, ylaxis, xhi, yuaxis,
                       GRA_colorControl::GetColor(wxT("WHITE")), (GRA_wxWidgets*)0 );
    dithPlot->FillLegendBitmap( &bitmap );
    bitmap.SetBorder( GRA_colorControl::GetColor(wxT("BLACK")) );
    Draw( &bitmap );
  }
  double xlo, ylo, xhi, yhi;
  dithPlot->GetXYLimits( xlo, ylo, xhi, yhi );
  GRA_bitmap bitmap( xlo, ylo, xhi, yhi,
                     GRA_colorControl::GetColor(wxT("WHITE")), (GRA_wxWidgets*)0 );
  dithPlot->FillBitmap( &bitmap );
  try
  {
    Draw( &bitmap );
  }
  catch( EGraphicsError const &e )
  {
    throw;
  }
  if( dithPlot->GetDrawAxes() )
  {
    std::vector<double> xt, yt;
    xt.push_back( dataXmin );
    xt.push_back( dataXmax );
    xt.push_back( dataXmax );
    xt.push_back( dataXmin );
    xt.push_back( dataXmin );
    yt.push_back( dataYmax );
    yt.push_back( dataYmax );
    yt.push_back( dataYmin );
    yt.push_back( dataYmin );
    yt.push_back( dataYmax );
    axes = new GRA_cartesianAxes(xt,yt,false,false);
    try
    {
      axes->Make();
    }
    catch( EGraphicsError const &e )
    {
      delete axes;
      throw;
    }
    Draw( axes );
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *dataC = gw->GetDataCurveCharacteristics();
  if( dithPlot->GetBorder() )
  {
    GRA_color *lineColor =
      static_cast<GRA_colorCharacteristic*>(dataC->Get(wxT("CURVECOLOR")))->Get();
    double x1, y1, x2, y2;
    gw->GraphToWorld( dataXmin, dataYmin, x1, y1, true );
    gw->GraphToWorld( dataXmax, dataYmax, x2, y2 );
    GRA_rectangle r( x1, y1, x2, y2, 0.0, false, lineColor, 0, 1 );
    Draw( &r );
  }
  DrawProfiles( dithPlot );
  if( dithPlot->GetDrawAxes() )
  {
    Draw( axes );
    delete axes;
  }
  if( dithPlot->GetResetAxes() )
  {
    double xuaxisp, yuaxisp;
    dithPlot->GetUpperAxis( xuaxisp, yuaxisp );
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->SetAsPercent( xuaxisp );
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->SetAsPercent( yuaxisp );
  }
}

void GRA_postscript::Draw( GRA_gradientPlot *gradPlot )
{
  double dataXmin, dataYmin, dataXmax, dataYmax;
  gradPlot->GetDataLimits( dataXmin, dataYmin, dataXmax, dataYmax );
  GRA_cartesianAxes *axes =0;
  GRA_colorMap *colorMapSave = GRA_colorControl::GetColorMap();
  GRA_colorControl::SetColorMap( gradPlot->GetColorMap() );

  std::vector<double> x, y, z;
  gradPlot->GetXYZ( x, y, z );
  std::size_t nrow = gradPlot->GetNRow();
  double amin, amax;
  UsefulFunctions::MinMax( z, nrow, 0, x.size(), 0, y.size(), amin, amax );
  //
  double fmin, fmax;
  gradPlot->GetFminmax( fmin, fmax );
  //
  double alo = amin + fmin*(amax-amin);
  double ahi = amin + fmax*(amax-amin);
  if( ahi-alo < std::numeric_limits<double>::epsilon() )throw EGraphicsError( wxT("region is flat") );
  gradPlot->SetAlohi( alo, ahi );
  double amean = (amin + amax) / 2.0;
  if( amean == 0.0 )amean = 1.e-10;
  if( fabs((amax-amin)/amean) < 1.0e-5 )throw EGraphicsError( wxT("region is flat") );
  try
  {
    gradPlot->SetUp();
  }
  catch( EGraphicsError const &e )
  {
    throw;
  }
  if( gradPlot->GetLegend() )
  {
    GRA_window *gw = ExGlobals::GetGraphWindow();
    GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
    GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
    GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
    double xlwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
    double xuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
    double xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
    double ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
    double yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
    double yticl = static_cast<GRA_sizeCharacteristic*>(yAxisC->Get(wxT("LARGETICLENGTH")))->GetAsWorld();
    bool xProfile, yProfile;
    gradPlot->GetProfiles( xProfile, yProfile );
    double xlo = yProfile ? xuaxis+0.1*(xuwind-xlwind)+1.5*yticl : xuaxis+1.5*yticl;
    double xhi = xlo + (xuwind-xlo)/3.0;
    //
    double height = static_cast<GRA_sizeCharacteristic*>(generalC->Get(wxT("LEGENDSIZE")))->GetAsWorld();
    //
    if( height > 0.0 )
    {
      wxChar c[10];
      ::wxSnprintf( c, 10, wxT("%9.2lg"), alo );
      double angle = 0.0;
      GRA_font *font = static_cast<GRA_fontCharacteristic*>(generalC->Get(wxT("FONT")))->Get();
      wxString stmp(c);
      GRA_drawableText dt( stmp, height, angle, xhi+0.1, ylaxis-height/2., 1,
                           font, GRA_colorControl::GetColor(wxT("BLACK")) );
      ::wxSnprintf( c, 10, wxT("%9.2lg"), ahi );
      stmp = wxString(c);
      GRA_drawableText dt2( stmp, height, angle, xhi+0.1, yuaxis-height/2., 1,
                            font, GRA_colorControl::GetColor(wxT("BLACK")) );
      try
      {
        dt.Parse();
        dt2.Parse();
      }
      catch( EGraphicsError const &e )
      {
        throw;
      }
      Draw( &dt );
      Draw( &dt2 );
    }
    GRA_bitmap bitmap( xlo, ylaxis, xhi, yuaxis,
                       GRA_colorControl::GetColor(wxT("WHITE")), (GRA_wxWidgets*)0 );
    gradPlot->FillLegendBitmap( &bitmap );
    bitmap.SetBorder( GRA_colorControl::GetColor(wxT("BLACK")) );
    Draw( &bitmap );
  }
  double xlo, ylo, xhi, yhi;
  gradPlot->GetXYLimits( xlo, ylo, xhi, yhi );
  GRA_bitmap bitmap( xlo, ylo, xhi, yhi, GRA_colorControl::GetColor(wxT("WHITE")), (GRA_wxWidgets*)0 );
  gradPlot->FillBitmap( &bitmap );
  try
  {
    Draw( &bitmap );
  }
  catch( EGraphicsError const &e )
  {
    throw;
  }
  if( gradPlot->GetDrawAxes() )
  {
    std::vector<double> xt, yt;
    xt.push_back( dataXmin );
    xt.push_back( dataXmax );
    xt.push_back( dataXmax );
    xt.push_back( dataXmin );
    xt.push_back( dataXmin );
    yt.push_back( dataYmax );
    yt.push_back( dataYmax );
    yt.push_back( dataYmin );
    yt.push_back( dataYmin );
    yt.push_back( dataYmax );
    axes = new GRA_cartesianAxes(xt,yt,false,false);
    try
    {
      axes->Make();
    }
    catch( EGraphicsError const &e )
    {
      delete axes;
      throw;
    }
    Draw( axes );
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *dataC = gw->GetDataCurveCharacteristics();
  if( gradPlot->GetBorder() )
  {
    GRA_color *lineColor =
      static_cast<GRA_colorCharacteristic*>(dataC->Get(wxT("CURVECOLOR")))->Get();
    double x1, y1, x2, y2;
    gw->GraphToWorld( dataXmin, dataYmin, x1, y1, true );
    gw->GraphToWorld( dataXmax, dataYmax, x2, y2 );
    GRA_rectangle r( x1, y1, x2, y2, 0.0, false, lineColor, 0, 1 );
    Draw( &r );
  }
  DrawProfiles( gradPlot );
  if( gradPlot->GetDrawAxes() )
  {
    Draw( axes );
    delete axes;
  }
  if( gradPlot->GetResetAxes() )
  {
    double xuaxisp, yuaxisp;
    gradPlot->GetUpperAxis( xuaxisp, yuaxisp );
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->SetAsPercent( xuaxisp );
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->SetAsPercent( yuaxisp );
  }
  GRA_colorControl::SetColorMap( colorMapSave );
}

void GRA_postscript::DrawProfiles( GRA_densityPlot *densPlot )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *dataC = gw->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  bool xProfile, yProfile;
  densPlot->GetProfiles( xProfile, yProfile );
  if( xProfile || yProfile )
  {
    bool xlabel = static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("LABELON")))->Get();
    bool ylabel = static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("LABELON")))->Get();
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("LABELON")))->Set( false );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("LABELON")))->Set( false );
    int histyp = static_cast<GRA_intCharacteristic*>(dataC->Get(wxT("HISTOGRAMTYPE")))->Get();
    bool xaxison = static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("AXISON")))->Get();
    bool yaxison = static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("AXISON")))->Get();
    int xlinc = static_cast<GRA_intCharacteristic*>(xAxisC->Get(wxT("NLINCS")))->Get();
    int ylinc = static_cast<GRA_intCharacteristic*>(yAxisC->Get(wxT("NLINCS")))->Get();
    bool box = static_cast<GRA_boolCharacteristic*>(generalC->Get(wxT("GRAPHBOX")))->Get();
    bool xnumbers = static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("NUMBERSON")))->Get();
    bool ynumbers = static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("NUMBERSON")))->Get();
    int xndig = static_cast<GRA_intCharacteristic*>(xAxisC->Get(wxT("NUMBEROFDIGITS")))->Get();
    int yndig = static_cast<GRA_intCharacteristic*>(yAxisC->Get(wxT("NUMBEROFDIGITS")))->Get();
    int xndec = static_cast<GRA_intCharacteristic*>(xAxisC->Get(wxT("NUMBEROFDECIMALS")))->Get();
    int yndec = static_cast<GRA_intCharacteristic*>(yAxisC->Get(wxT("NUMBEROFDECIMALS")))->Get();
    //
    double xlwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
    double xuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
    double ylwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("YLOWERWINDOW")))->GetAsWorld();
    double yuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("YUPPERWINDOW")))->GetAsWorld();
    double xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
    double xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
    double ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
    double yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
    double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MIN")))->Get();
    double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MAX")))->Get();
    double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MIN")))->Get();
    double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MAX")))->Get();
    double xvmin = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("VIRTUALMIN")))->Get();
    double xvmax = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("VIRTUALMAX")))->Get();
    double yvmin = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("VIRTUALMIN")))->Get();
    double yvmax = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("VIRTUALMAX")))->Get();
    bool ytics = static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("TICSON")))->Get();
    bool xtics = static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("TICSON")))->Get();
    wxString autoScale( static_cast<GRA_stringCharacteristic*>(generalC->Get(wxT("AUTOSCALE")))->Get() );
    std::vector<double> *xProfileX, *xProfileY, *yProfileX, *yProfileY;
    densPlot->GetProfileVecs( xProfileX, xProfileY, yProfileX, yProfileY );
    if( yProfile )
    {
      static_cast<GRA_intCharacteristic*>(dataC->Get(wxT("HISTOGRAMTYPE")))->Set( 3 );
      static_cast<GRA_boolCharacteristic*>(generalC->Get(wxT("GRAPHBOX")))->Set( false );
      static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("AXISON")))->Set( true );
      static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("AXISON")))->Set( true );
      static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("TICSON")))->Set( true );
      static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("TICSON")))->Set( false );
      static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MIN")))->Set( 0.0 );
      static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MAX")))->Set( 1.0 );
      static_cast<GRA_intCharacteristic*>(xAxisC->Get(wxT("NLINCS")))->Set( 1 );
      static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("NUMBERSON")))->Set( false );
      static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("NUMBERSON")))->Set( false );
      static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->SetAsWorld( xuaxis );
      static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->SetAsWorld( xuaxis+0.1*(xuwind-xlwind) );
      static_cast<GRA_stringCharacteristic*>(generalC->Get(wxT("AUTOSCALE")))->Set( wxString(wxT("OFF")) );
      std::vector<double> xe1, ye1, xe2, ye2;
      GRA_cartesianAxes *cartesianAxes = new GRA_cartesianAxes( *yProfileX, *yProfileY, false, false );
      GRA_cartesianCurve *cartesianCurve = new GRA_cartesianCurve( *yProfileX, *yProfileY, xe1,ye1,xe2,ye2, false );
      try
      {
        cartesianAxes->Make();
        cartesianCurve->Make();
      }
      catch( EGraphicsError const &e )
      {
        delete cartesianAxes;
        delete cartesianCurve;
        throw;
      }
      Draw( cartesianAxes );
      Draw( cartesianCurve );
      delete cartesianAxes;
      delete cartesianCurve;
    }
    if( xProfile )
    {
      static_cast<GRA_intCharacteristic*>(dataC->Get(wxT("HISTOGRAMTYPE")))->Set( 1 );
      static_cast<GRA_boolCharacteristic*>(generalC->Get(wxT("GRAPHBOX")))->Set( false );
      static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("AXISON")))->Set( true );
      static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("AXISON")))->Set( true );
      static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("TICSON")))->Set( false );
      static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("TICSON")))->Set( true );
      static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MIN")))->Set( xmin );
      static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MAX")))->Set( xmax );
      static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MIN")))->Set( 0.0 );
      static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MAX")))->Set( 1.0 );
      static_cast<GRA_intCharacteristic*>(yAxisC->Get(wxT("NLINCS")))->Set( 1 );
      static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("NUMBERSON")))->Set( false );
      static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("NUMBERSON")))->Set( false );
      static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->SetAsWorld( xlaxis );
      static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->SetAsWorld( xuaxis );
      static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->SetAsWorld( yuaxis );
      static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->SetAsWorld( yuaxis+0.1*(yuwind-ylwind) );
      static_cast<GRA_stringCharacteristic*>(generalC->Get(wxT("AUTOSCALE")))->Set( wxString(wxT("OFF")) );
      static_cast<GRA_stringCharacteristic*>(generalC->Get(wxT("AUTOSCALE")))->Set( wxString(wxT("OFF")) );
      std::vector<double> xe1, ye1, xe2, ye2;
      GRA_cartesianAxes *cartesianAxes = new GRA_cartesianAxes( *xProfileX, *xProfileY, false, false );
      GRA_cartesianCurve *cartesianCurve = new GRA_cartesianCurve( *xProfileX, *xProfileY, xe1,ye1,xe2,ye2, false );
      try
      {
        cartesianAxes->Make();
        cartesianCurve->Make();
      }
      catch( EGraphicsError const &e )
      {
        delete cartesianAxes;
        delete cartesianCurve;
        throw;
      }
      Draw( cartesianAxes );
      Draw( cartesianCurve );
      delete cartesianAxes;
      delete cartesianCurve;
    }
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("LABELON")))->Set( xlabel );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("LABELON")))->Set( ylabel );
    static_cast<GRA_intCharacteristic*>(dataC->Get(wxT("HISTOGRAMTYPE")))->Set( histyp );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("AXISON")))->Set( xaxison );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("AXISON")))->Set( yaxison );
    static_cast<GRA_intCharacteristic*>(xAxisC->Get(wxT("NLINCS")))->Set( xlinc );
    static_cast<GRA_intCharacteristic*>(yAxisC->Get(wxT("NLINCS")))->Set( ylinc );
    static_cast<GRA_boolCharacteristic*>(generalC->Get(wxT("GRAPHBOX")))->Set( box );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("NUMBERSON")))->Set( xnumbers );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("NUMBERSON")))->Set( ynumbers );
    static_cast<GRA_intCharacteristic*>(xAxisC->Get(wxT("NUMBEROFDIGITS")))->Set( xndig );
    static_cast<GRA_intCharacteristic*>(yAxisC->Get(wxT("NUMBEROFDIGITS")))->Set( yndig );
    static_cast<GRA_intCharacteristic*>(xAxisC->Get(wxT("NUMBEROFDECIMALS")))->Set( xndec );
    static_cast<GRA_intCharacteristic*>(yAxisC->Get(wxT("NUMBEROFDECIMALS")))->Set( yndec );
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->SetAsWorld( xlaxis );
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->SetAsWorld( xuaxis );
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->SetAsWorld( ylaxis );
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->SetAsWorld( yuaxis );
    static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MIN")))->Set( xmin );
    static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MAX")))->Set( xmax );
    static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MIN")))->Set( ymin );
    static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MAX")))->Set( ymax );
    static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("VIRTUALMIN")))->Set( xvmin );
    static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("VIRTUALMAX")))->Set( xvmax );
    static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("VIRTUALMIN")))->Set( yvmin );
    static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("VIRTUALMAX")))->Set( yvmax );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("TICSON")))->Set( xtics );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("TICSON")))->Set( ytics );
    static_cast<GRA_stringCharacteristic*>(generalC->Get(wxT("AUTOSCALE")))->Set( autoScale );  
  }
}

void GRA_postscript::DisplayBackground( GRA_window *gw )
{
  double xminw, yminw, xmaxw, ymaxw;
  ExGlobals::GetWorldLimits( xminw, yminw, xmaxw, ymaxw );
  double xlo, ylo, xhi, yhi;
  gw->GetDimensions( xlo, ylo, xhi, yhi );
  //
  // calculate window coordinates in world units
  //
  double xl = xminw+xlo*(xmaxw-xminw)/100.0;
  double yl = yminw+ylo*(ymaxw-yminw)/100.0;
  double xu = xminw+xhi*(xmaxw-xminw)/100.0;
  double yu = yminw+yhi*(ymaxw-yminw)/100.0;
  //
  GRA_color *bgcolor = static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->
							     Get(wxT("BACKGROUNDCOLOR")))->Get();
  wxString bgfile( static_cast<GRA_stringCharacteristic*>(gw->GetGeneralCharacteristics()->
							  Get(wxT("BACKGROUNDFILE")))->Get() );
  ExGlobals::SetClippingBoundary( xl, yl, xu, yu );
  if( bgcolor )
  {
    std::auto_ptr<GRA_rectangle> rect( new GRA_rectangle(xl,yl,xu,yu,0.0,false,bgcolor,bgcolor) );
    Draw( rect.get() );
  }
  else if( !bgfile.empty() )
  {
    wxImage image( bgfile, wxBITMAP_TYPE_PNG );
    //
    double xl1, yl1, xu1, yu1;
    WorldToOutputType( xl, yl, xl1, yl1 );
    WorldToOutputType( xu, yu, xu1, yu1 );
    //
    int dx = int(fabs(xu1-xl1));
    int dy = int(fabs(yu1-yl1));
    image.Rescale( dx, dy );
    //
    GRA_bitmap *bitmap = new GRA_bitmap( xl, yl, xu, yu,
					 GRA_colorControl::GetColor(wxT("WHITE")), 0 );
    int width = bitmap->GetWidth();
    int height = bitmap->GetHeight();
    std::vector<GRA_color*> tempColors( width*height );
    for( int i=0; i<width; ++i )
    {
      for( int j=0; j<height; ++j )
      {
	int r = image.GetRed(i,height-1-j);
	int g = image.GetGreen(i,height-1-j);
	int b = image.GetBlue(i,height-1-j);
	GRA_color *color = new GRA_color(r,g,b);
	bitmap->SetPixelColor( i, j, color );
	tempColors[i+j*width] = color;
      }
    }
    Draw( bitmap );
    delete bitmap;
    for( int i=0; i<width*height; ++i )delete tempColors[i];
  }
  ExGlobals::ResetClippingBoundary();
}

void GRA_postscript::Draw( GRA_bitmap *b )
{
  double xlo, ylo, xhi, yhi;
  b->GetLimits( xlo, ylo, xhi, yhi );
  double xlod, ylod, xhid, yhid;
  WorldToOutputType( xlo, ylo, xlod, ylod );
  WorldToOutputType( xhi, yhi, xhid, yhid );
  //
  int bwidth = b->GetWidth();
  int bheight = b->GetHeight();
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
  std::vector<GRA_color*> bitmap( b->GetBitmap() );
  for( int j=bheight-1; j>=0; --j )
  {
    for( int i=0; i<bwidth; ++i )
    {
      int r, g, b;
      bitmap[i+j*bwidth]->GetRGB( r, g, b );
      try
      {
        outFile_ << ExGlobals::IntToHex(r).mb_str(wxConvUTF8)
                 << ExGlobals::IntToHex(g).mb_str(wxConvUTF8)
                 << ExGlobals::IntToHex(b).mb_str(wxConvUTF8);
        //outFile_ << std::setfill('0') << std::setw(2) << std::hex << r << g << b << std::dec << std::setw(0);
      }
      catch( std::runtime_error const &e )
      {
        throw EGraphicsError( wxT("invalid RGB codes in bitmap") );
      }
      if( ++count == 12 )
      {
        outFile_ << "\n";
        count = 0;
      }
    }
  }
  if( count > 0 )outFile_ << "\n";
  outFile_ << "grestore\n";
}

void GRA_postscript::Draw( GRA_drawableText *dt )
{
  std::vector<GRA_simpleText*> textVec( dt->GetStrings() );
  outFile_ << "s\n"
           << "/ArrayLength " << textVec.size() << " def\n"
           << "/TextBuffer ArrayLength array def\n";
  double maxHeight = 0.0;
  std::size_t counter = 0;
  std::vector<GRA_simpleText*>::const_iterator textVecEnd( textVec.end() );
  for( std::vector<GRA_simpleText*>::const_iterator i=textVec.begin(); i!=textVecEnd; ++i, ++counter )
  {
    wxString text( (*i)->GetString() );
    if( text.empty() )continue;
    //
    int r, g, b;
    (*i)->GetColor()->GetRGB( r, g, b );
    double dr = static_cast<double>(r)/255.0;
    double dg = static_cast<double>(g)/255.0;
    double db = static_cast<double>(b)/255.0;
    //
    wxString fontName( (*i)->GetFont()->GetFontName() );
    wxString psFontName;
    try
    {
      psFontName = GRA_fontControl::GetPostScriptFontName( (*i)->GetFont()->GetFontName() );
    }
    catch( EGraphicsError const &e )
    {
      throw;
    }

    //std::cout << "fontName = |" << fontName.mb_str(wxConvUTF8) << "|\n";

    ExGlobals::RemoveBlanks( fontName );
    ExGlobals::ToCapitalize( fontName );
    double height = dotsPerInch_*(*i)->GetHeight();
    double xshift = dotsPerInch_*(*i)->GetXShift();
    double yshift = dotsPerInch_*(*i)->GetYShift();
    maxHeight = std::max( height, maxHeight );
    outFile_ << "TextBuffer " << counter << " [[" << dr << " " << dg << " " << db
             << "] /" << psFontName.mb_str(wxConvUTF8) << " " << height
             << " " << xshift << " " << yshift << " (";
    std::size_t tEnd = text.size();
    for( size_t i=0; i<tEnd; ++i )
    {
      int j = static_cast<int>(text[i]);
      if( j < 0 )j += 256;

      //std::cout << "j = " << j << "\n";

      outFile_ << "\\" << std::setfill('0') << std::setw(3) << std::oct << j << std::dec << std::setw(0);
    }
    outFile_ << ")] put\n";
  }
  outFile_ << "SetupText\n";
  double xLoc = dt->GetX();
  double yLoc = dt->GetY();

  // apply fudge factor
  double xminW, yminW, xmaxW, ymaxW;
  ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
  yLoc += 0.01*(ymaxW-yminW);

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
  outFile_ << "DrawText\n";
}

void GRA_postscript::StartLine( double x, double y )
{
  PenUp( x, y );
  newLine_ = true;
}

void GRA_postscript::ContinueLine( double x, double y )
{
  // This routine draws a line of specified line type from the
  // current location to location (x,y) in world coordinates
  //
  // The line types are in four different styles:
  // 1) Ordinary solid line
  // 2) Double line of specified width
  // 3) Dashed line with specified dash and space lengths
  // 4) Dashed line with two different dash lengths
  //
  // Input:
  //   x,y     point coordinates in world units
  //
  // For each line type type, the appearance of the line is determined
  // by three parameters p1,p2,p3 that are stored in an internal table
  //
  // If p1 = 0, the line is an ordinary solid line
  // If p1 > 0 and p2 = 0, the line is a double line of width p1
  // If p1 > 0 and p2 > 0, the line is a dashed line, with dash
  //  length p1 and space length p2
  // If p1 > 0 and p2 > 0 and p3 > 0, the line is a dashed line,
  //  with first dash length p1, space length p2, and second dash
  //  length p3
  //
  // NOTE:  p1, p2 and p3 should be in world units
  //
  // The user may define his own line types, or he may use the default
  // types established in this routine, which are suitable for the
  // default 640 x 480 coordinate system.  Line types are user-defined
  // by calling DrawLineSet( type, p1, p2, p3 )
  // where type is a line type number between 1 and 10 and p1-p3
  // are the desired parameters.  Alternatively, the default line types
  // can be scaled to fit a user coordinate system by calling
  // DrawLineScale( type, sf ) where sf is the desired scale factor.
  // For example, if the user coordinate system is 10 units in x by
  // 7.5 units in y, the scale factor sf=10/640 would be appropriate
  //
  // This routine always plots from the current location.
  // Thus a call to StartLine must precede a series of calls
  // to ContinueLine.  In order to plot a continuous dashed line through
  // a series of points, ContinueLine "remembers" where it's at between
  // calls.  As long as the current location is the same as that
  // where ContinueLine left off on the previous call, 
  // ContinueLine will continue dashing from where it
  // left off.  Thus the dashing may be interrupted, a symbol or other
  // items plotted, and the dashing can be continued by calling StartLine
  // to the old location before calling ContinueLine.
  //
  static int op = 0;
  static double xOld = 0.0;
  static double yOld = 0.0;
  static double distanceTraveled = 0.0;
  //
  if( x==xPrevious_ && y==yPrevious_ )return; // draw nothing if going nowhere
  //
  // get parameters
  //
  double p1 = drawLineTable_[0][lineType_-1];
  double p2 = drawLineTable_[1][lineType_-1];
  double p3 = drawLineTable_[2][lineType_-1];
  //
  if( p1 == 0. ) // draw an ordinary vector
  {
    PenDown( x, y );
  }
  else
  {
    double const deltx = x-xPrevious_;
    double const delty = y-yPrevious_;
    double const distanceToTravel = sqrt(deltx*deltx + delty*delty);
    //
    double sinth = delty/distanceToTravel; // sine of orientation angle
    double costh = deltx/distanceToTravel; // cosine of orientation angle
    //
    if( p2 == 0. ) // double line
    {
      double hwid = p1/2.0;
      double dx = hwid*sinth;
      double dy = hwid*costh;
      double xloc = xPrevious_;
      double yloc = yPrevious_;
      PenUp( xloc-dx, yloc+dy );
      PenDown( x-dx, y+dy );
      PenUp( xloc+dx, yloc-dy );
      PenDown( x+dx, y-dy );
    }
    else           // dashed line
    {
      if( p3 == 0. )p3 = p1;
      //
      double dd[4]; // distance increments for operation loop
      dd[0] = p1;
      dd[1] = p2;
      dd[2] = p3;
      dd[3] = p2;
      //
      double const x0 = xPrevious_; // start point
      double const y0 = yPrevious_;
      //
      // continuing previous line?
      //
      double const eps = 0.0001;
      if( !newLine_ && fabs(x0-xOld)<eps && fabs(y0-yOld)<eps )
      {
        if( distanceTraveled > 0. )goto L210; // continuing dash or space
        goto L200;
      }
      //
      // begin dashing sequence
      //
      op = 0;                      // operation number
      distanceTraveled = 0.0;     // distance travelled
 L200:                 // operation loop begins
      if( ++op == 5 )op = 1;
      distanceTraveled += dd[op-1];
      L210:
      double xloc = x0 + distanceTraveled*costh;
      double yloc = y0 + distanceTraveled*sinth;
      bool drawTo = (op==1||op==3);
      //
      // last operation
      //
      if( distanceTraveled >= distanceToTravel )
      {
        xloc = x;
        yloc = y;
        //drawTo = true;
      }
      drawTo ? PenDown(xloc,yloc) : PenUp(xloc,yloc);
      if( distanceTraveled < distanceToTravel )goto L200;  // do next operation
    }
    xOld = x;  // save end point
    yOld = y;
    distanceTraveled -= distanceToTravel;  // portion of dash or space to be continued
  }
  newLine_ = false;
}

void GRA_postscript::DrawLineSet( int type, double p1, double p2, double p3 )
{
  // set up line type characteristics for DrawLine
  //
  if( type>10 || type<1 )return;
  drawLineTable_[0][type-1] = p1;
  drawLineTable_[1][type-1] = p2;
  drawLineTable_[2][type-1] = p3;
}

void GRA_postscript::DrawLineGet( int type, double &p1, double &p2, double &p3 )
{
  if( type>10 || type<1 )return;
  p1 = drawLineTable_[0][type-1];
  p2 = drawLineTable_[1][type-1];
  p3 = drawLineTable_[2][type-1];
}

void GRA_postscript::SetDefaultDrawLineTable()
{
  double const defaultTable[3][10] =
  {
    {0.00,0.02,0.30,0.30,0.40,0.40,0.02,0.02,0.06,0.10},    // p1
    {0.00,0.00,0.15,0.15,0.20,0.16,0.12,0.12,0.20,0.30},    // p2
    {0.00,0.00,0.00,0.10,0.00,0.12,0.00,0.08,0.00,0.00}     // p3
  };
  for( std::size_t i=0; i<10; ++i )
  {
    for( std::size_t j=0; j<3; ++j )drawLineTable_[j][i] = defaultTable[j][i];
  }
}

void GRA_postscript::DrawLineScale( int type, double sf )
{
  // scale the line table parameters by factor sf
  // for specified type only
  //
  if( type<1 || type>10 )return;
  for( std::size_t j=0; j<3; ++j )drawLineTable_[j][type-1] *= sf;
}

void GRA_postscript::DrawLineScale( double sf )
{
  // scale all of the line table parameters by factor sf
  //
  for( std::size_t i=0; i<10; ++i )
  {
    for( std::size_t j=0; j<3; ++j )drawLineTable_[j][i] *= sf;
  }
}

// end of file
