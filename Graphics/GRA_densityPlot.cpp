/*
Copyright (C) 2005,...,2007 Joseph L. Chuma, TRIUMF

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
#pragma hdrstop

#include "GRA_densityPlot.h"
#include "ExGlobals.h"
#include "EGraphicsError.h"
#include "GRA_window.h"
#include "GRA_outputType.h"
#include "UsefulFunctions.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "GRA_rectangle.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_intCharacteristic.h"

GRA_densityPlot::GRA_densityPlot( char const *type,
                                  std::vector<double> &x, std::vector<double> &y,
                                  std::vector<double> &z,
                                  int nrow, double fmin, double fmax,
                                  double gmin, double gmax,
                                  bool xProfile, bool yProfile, bool border,
                                  bool zoom, bool axes, bool reset, bool legend )
  : GRA_drawableObject(type), nrow_(nrow), fmin_(fmin), fmax_(fmax), gmin_(gmin), gmax_(gmax),
    xProfile_(xProfile), yProfile_(yProfile), border_(border), zoom_(zoom), drawAxes_(axes),
    resetAxes_(reset), legend_(legend)
{
  x_.assign( x.begin(), x.end() );
  y_.assign( y.begin(), y.end() );
  z_.assign( z.begin(), z.end() );
  densityType_ = UNKNOWN;
  ExString stype( type );
  if( stype == "BOXPLOT" )             densityType_ = BOX;
  else if( stype == "DIFFUSIONPLOT" )  densityType_ = DIFFUSION;
  else if( stype == "DITHERINGPLOT" )  densityType_ = DITHERING;
  else if( stype == "GRADIENTPLOT" )   densityType_ = GRADIENT;
}

void GRA_densityPlot::CopyStuff( GRA_densityPlot const &rhs )
{
  densityType_ = rhs.densityType_;
  dataXmin_ = rhs.dataXmin_;
  dataYmin_ = rhs.dataYmin_;
  dataXmax_ = rhs.dataXmax_;
  dataYmax_ = rhs.dataYmax_;
  alo_ = rhs.alo_;
  ahi_ = rhs.ahi_;
  xlo_ = rhs.xlo_;
  ylo_ = rhs.ylo_;
  xhi_ = rhs.xhi_;
  yhi_ = rhs.yhi_;
  mlo_ = rhs.mlo_;
  mhi_ = rhs.mhi_;
  nlo_ = rhs.nlo_;
  nhi_ = rhs.nhi_;
  rmlo_ = rhs.rmlo_;
  rmhi_ = rhs.rmhi_;
  rnlo_ = rhs.rnlo_;
  rnhi_ = rhs.rnhi_;
  fmin_ = rhs.fmin_;
  fmax_ = rhs.fmax_;
  gmin_ = rhs.gmin_;
  gmax_ = rhs.gmax_;
  nrow_ = rhs.nrow_;
  border_ = rhs.border_;
  zoom_ = rhs.zoom_;
  drawAxes_ = rhs.drawAxes_;
  resetAxes_ = rhs.resetAxes_;
  legend_ = rhs.legend_;
  xuaxisp_ = rhs.xuaxisp_;
  yuaxisp_ = rhs.yuaxisp_;
  x_.assign( rhs.x_.begin(), rhs.x_.end() );
  y_.assign( rhs.y_.begin(), rhs.y_.end() );
  z_.assign( rhs.z_.begin(), rhs.z_.end() );
  xProfile_ = rhs.xProfile_;
  yProfile_ = rhs.yProfile_;
  xProfileX_.assign( rhs.xProfileX_.begin(), rhs.xProfileX_.end() );
  xProfileY_.assign( rhs.xProfileY_.begin(), rhs.xProfileY_.end() );
  yProfileX_.assign( rhs.yProfileX_.begin(), rhs.yProfileX_.end() );
  yProfileY_.assign( rhs.yProfileY_.begin(), rhs.yProfileY_.end() );
}

void GRA_densityPlot::SetUp()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  xuaxisp_ = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->GetAsPercent();
  yuaxisp_ = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("UPPERAXIS"))->GetAsPercent();
  if( legend_ )
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->SetAsPercent( 75.0 );
  if( yProfile_ )
  {
    double tmp = legend_ ? 65.0 : 85.0;
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->SetAsPercent( tmp );
  }
  if( xProfile_ )
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("UPPERAXIS"))->SetAsPercent( 80.0 );
  //
  UsefulFunctions::MinMax( x_, 0, x_.size(), dataXmin_, dataXmax_ );
  UsefulFunctions::MinMax( y_, 0, y_.size(), dataYmin_, dataYmax_ );
  if( drawAxes_ )
  {
    std::vector<double> xt, yt;
    xt.push_back( dataXmin_ );
    xt.push_back( dataXmax_ );
    xt.push_back( dataXmax_ );
    xt.push_back( dataXmin_ );
    xt.push_back( dataXmin_ );
    yt.push_back( dataYmax_ );
    yt.push_back( dataYmax_ );
    yt.push_back( dataYmin_ );
    yt.push_back( dataYmin_ );
    yt.push_back( dataYmax_ );
    std::auto_ptr<GRA_cartesianAxes> cartesianAxes( new GRA_cartesianAxes(xt,yt,false,false) );
    try
    {
      cartesianAxes->Make();
    }
    catch (EGraphicsError &e)
    {
      throw;
    }
  }
  double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MIN"))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MAX"))->Get();
  double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MIN"))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MAX"))->Get();
  std::size_t nx = x_.size();
  std::size_t ny = y_.size();
  //
  // use the following for zooming in
  //
  double dx = dataXmax_ - dataXmin_;
  double dy = dataYmax_ - dataYmin_;
  if( dx == 0.0 )
  {
    rmlo_ = 1.;
    rmhi_ = static_cast<double>(nx);
  }
  else
  {
    rmlo_ = std::max( 0.0, (xmin-dataXmin_)/dx*(nx-1.0) ) + 1.0;
    rmhi_ = std::min( nx-1.0, (xmax-dataXmin_)/dx*(nx-1.0) ) + 1.0;
  }
  if( dy == 0.0 )
  {
    rnlo_ = 1.0;
    rnhi_ = static_cast<double>(ny);
  }
  else
  {
    rnlo_ = std::max( 0.0, (ymin-dataYmin_)/dy*(ny-1.0) ) + 1.0;
    rnhi_ = std::min( ny-1.0, (ymax-dataYmin_)/dy*(ny-1.0) ) + 1.0;
  }
  mlo_ = static_cast<int>(rmlo_);
  mhi_ = static_cast<int>(rmhi_+0.99999);
  nlo_ = static_cast<int>(rnlo_);
  nhi_ = static_cast<int>(rnhi_+0.99999);
  //
  double xmn2 = std::max(dataXmin_,xmin);
  double xmx2 = std::min(dataXmax_,xmax);
  double ymn2 = std::max(dataYmin_,ymin);
  double ymx2 = std::min(dataYmax_,ymax);
  //
  gw->GraphToWorld( xmn2, ymn2, xlo_, ylo_, true );
  gw->GraphToWorld( xmx2, ymx2, xhi_, yhi_ );
}

void GRA_densityPlot::MakeProfiles( int i1, int i2, int j1, int j2 )
{
  std::size_t nx = x_.size();
  std::size_t ny = y_.size();
  std::vector<double> xr, xc;
  if( yProfile_ )xr.resize( ny, 0.0 );
  if( xProfile_ )xc.resize( nx, 0.0 );
  for( int i=i1; i<i2; ++i )
  {
    for( int j=j1; j<j2; ++j )
    {
      if( yProfile_ )xr[j] += z_[j+i*nrow_];
      if( xProfile_ )xc[i] += z_[j+i*nrow_];
    }
  }
  if( yProfile_ )
  {
    yProfileX_.assign( xr.begin()+j1, xr.begin()+j2 );
    yProfileY_.assign( y_.begin()+j1, y_.begin()+j2 );
    std::size_t size = yProfileX_.size();
    double xrmin, xrmax;
    UsefulFunctions::MinMax( yProfileX_, 0, size, xrmin, xrmax );
    double denom = xrmax-xrmin;
    if( denom == 0.0 )
    {
      for( std::size_t j=0; j<size; ++j )yProfileX_[j] = 1.0;
    }
    else
    {
      for( std::size_t j=0; j<size; ++j )yProfileX_[j] = (yProfileX_[j]-xrmin)/denom;
    }
  }
  if( xProfile_ )
  {
    xProfileX_.assign( x_.begin()+i1, x_.begin()+i2 );
    xProfileY_.assign( xc.begin()+i1, xc.begin()+i2 );
    std::size_t size = xProfileY_.size();
    double xcmin, xcmax;
    UsefulFunctions::MinMax( xProfileY_, 0, size, xcmin, xcmax );
    double denom = xcmax-xcmin;
    if( denom == 0.0 )
    {
      for( std::size_t i=0; i<size; ++i )xProfileY_[i] = 1.0;
    }
    else
    {
      for( std::size_t i=0; i<size; ++i )xProfileY_[i] = (xProfileY_[i]-xcmin)/denom;
    }
  }
}

void GRA_densityPlot::DrawProfiles( GRA_outputType *graphicsOutput )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *dataC = gw->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  bool xlabel = static_cast<GRA_boolCharacteristic*>(xAxisC->Get("LABELON"))->Get();
  bool ylabel = static_cast<GRA_boolCharacteristic*>(yAxisC->Get("LABELON"))->Get();
  static_cast<GRA_boolCharacteristic*>(xAxisC->Get("LABELON"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get("LABELON"))->Set( false );
  int histyp = static_cast<GRA_intCharacteristic*>(dataC->Get("HISTOGRAMTYPE"))->Get();
  bool xaxison = static_cast<GRA_boolCharacteristic*>(xAxisC->Get("AXISON"))->Get();
  bool yaxison = static_cast<GRA_boolCharacteristic*>(yAxisC->Get("AXISON"))->Get();
  int xlinc = static_cast<GRA_intCharacteristic*>(xAxisC->Get("NLINCS"))->Get();
  int ylinc = static_cast<GRA_intCharacteristic*>(yAxisC->Get("NLINCS"))->Get();
  bool box = static_cast<GRA_boolCharacteristic*>(generalC->Get("GRAPHBOX"))->Get();
  bool xnumbers = static_cast<GRA_boolCharacteristic*>(xAxisC->Get("NUMBERSON"))->Get();
  bool ynumbers = static_cast<GRA_boolCharacteristic*>(yAxisC->Get("NUMBERSON"))->Get();
  int xndig = static_cast<GRA_intCharacteristic*>(xAxisC->Get("NUMBEROFDIGITS"))->Get();
  int yndig = static_cast<GRA_intCharacteristic*>(yAxisC->Get("NUMBEROFDIGITS"))->Get();
  int xndec = static_cast<GRA_intCharacteristic*>(xAxisC->Get("NUMBEROFDECIMALS"))->Get();
  int yndec = static_cast<GRA_intCharacteristic*>(yAxisC->Get("NUMBEROFDECIMALS"))->Get();
  //
  double xlwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get("XLOWERWINDOW"))->GetAsWorld();
  double xuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get("XUPPERWINDOW"))->GetAsWorld();
  double ylwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get("YLOWERWINDOW"))->GetAsWorld();
  double yuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get("YUPPERWINDOW"))->GetAsWorld();
  double xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("LOWERAXIS"))->GetAsWorld();
  double xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->GetAsWorld();
  double ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("LOWERAXIS"))->GetAsWorld();
  double yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("UPPERAXIS"))->GetAsWorld();
  double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MIN"))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MAX"))->Get();
  double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MIN"))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MAX"))->Get();
  double xvmin = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("VIRTUALMIN"))->Get();
  double xvmax = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("VIRTUALMAX"))->Get();
  double yvmin = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("VIRTUALMIN"))->Get();
  double yvmax = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("VIRTUALMAX"))->Get();
  bool ytics = static_cast<GRA_boolCharacteristic*>(xAxisC->Get("TICSON"))->Get();
  bool xtics = static_cast<GRA_boolCharacteristic*>(yAxisC->Get("TICSON"))->Get();
  ExString autoScale( static_cast<GRA_stringCharacteristic*>(generalC->Get("AUTOSCALE"))->Get() );
  if( yProfile_ )
  {
    static_cast<GRA_intCharacteristic*>(dataC->Get("HISTOGRAMTYPE"))->Set( 3 );
    static_cast<GRA_boolCharacteristic*>(generalC->Get("GRAPHBOX"))->Set( false );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get("AXISON"))->Set( true );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get("AXISON"))->Set( true );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get("TICSON"))->Set( true );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get("TICSON"))->Set( false );
    static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MIN"))->Set( 0.0 );
    static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MAX"))->Set( 1.0 );
    static_cast<GRA_intCharacteristic*>(xAxisC->Get("NLINCS"))->Set( 1 );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get("NUMBERSON"))->Set( false );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get("NUMBERSON"))->Set( false );
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("LOWERAXIS"))->SetAsWorld( xuaxis );
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->SetAsWorld( xuaxis+0.1*(xuwind-xlwind) );
    static_cast<GRA_stringCharacteristic*>(generalC->Get("AUTOSCALE"))->Set( ExString("OFF") );
    std::vector<double> xe1, ye1, xe2, ye2;
    std::auto_ptr<GRA_cartesianAxes> cartesianAxes(
      new GRA_cartesianAxes(yProfileX_,yProfileY_,false,false) );
    std::auto_ptr<GRA_cartesianCurve> cartesianCurve(
      new GRA_cartesianCurve(yProfileX_,yProfileY_,xe1,ye1,xe2,ye2,false) );
    try
    {
      cartesianAxes->Make();
      cartesianCurve->Make();
    }
    catch ( EGraphicsError &e )
    {
      throw;
    }
    cartesianAxes->Draw( graphicsOutput );
    cartesianCurve->Draw( graphicsOutput );
  }
  if( xProfile_ )
  {
    static_cast<GRA_intCharacteristic*>(dataC->Get("HISTOGRAMTYPE"))->Set( 1 );
    static_cast<GRA_boolCharacteristic*>(generalC->Get("GRAPHBOX"))->Set( false );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get("AXISON"))->Set( true );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get("AXISON"))->Set( true );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get("TICSON"))->Set( false );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get("TICSON"))->Set( true );
    static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MIN"))->Set( xmin );
    static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MAX"))->Set( xmax );
    static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MIN"))->Set( 0.0 );
    static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MAX"))->Set( 1.0 );
    static_cast<GRA_intCharacteristic*>(yAxisC->Get("NLINCS"))->Set( 1 );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get("NUMBERSON"))->Set( false );
    static_cast<GRA_boolCharacteristic*>(yAxisC->Get("NUMBERSON"))->Set( false );
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("LOWERAXIS"))->SetAsWorld( xlaxis );
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->SetAsWorld( xuaxis );
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("LOWERAXIS"))->SetAsWorld( yuaxis );
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("UPPERAXIS"))->SetAsWorld( yuaxis+0.1*(yuwind-ylwind) );
    static_cast<GRA_stringCharacteristic*>(generalC->Get("AUTOSCALE"))->Set( ExString("OFF") );
    static_cast<GRA_stringCharacteristic*>(generalC->Get("AUTOSCALE"))->Set( ExString("OFF") );
    std::vector<double> xe1, ye1, xe2, ye2;
    std::auto_ptr<GRA_cartesianAxes> cartesianAxes(
      new GRA_cartesianAxes(xProfileX_,xProfileY_,false,false) );
    std::auto_ptr<GRA_cartesianCurve> cartesianCurve(
      new GRA_cartesianCurve(xProfileX_,xProfileY_,xe1,ye1,xe2,ye2,false) );
    try
    {
      cartesianAxes->Make();
      cartesianCurve->Make();
    }
    catch ( EGraphicsError &e )
    {
      throw;
    }
    cartesianAxes->Draw( graphicsOutput );
    cartesianCurve->Draw( graphicsOutput );
  }
  static_cast<GRA_boolCharacteristic*>(xAxisC->Get("LABELON"))->Set( xlabel );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get("LABELON"))->Set( ylabel );
  static_cast<GRA_intCharacteristic*>(dataC->Get("HISTOGRAMTYPE"))->Set( histyp );
  static_cast<GRA_boolCharacteristic*>(xAxisC->Get("AXISON"))->Set( xaxison );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get("AXISON"))->Set( yaxison );
  static_cast<GRA_intCharacteristic*>(xAxisC->Get("NLINCS"))->Set( xlinc );
  static_cast<GRA_intCharacteristic*>(yAxisC->Get("NLINCS"))->Set( ylinc );
  static_cast<GRA_boolCharacteristic*>(generalC->Get("GRAPHBOX"))->Set( box );
  static_cast<GRA_boolCharacteristic*>(xAxisC->Get("NUMBERSON"))->Set( xnumbers );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get("NUMBERSON"))->Set( ynumbers );
  static_cast<GRA_intCharacteristic*>(xAxisC->Get("NUMBEROFDIGITS"))->Set( xndig );
  static_cast<GRA_intCharacteristic*>(yAxisC->Get("NUMBEROFDIGITS"))->Set( yndig );
  static_cast<GRA_intCharacteristic*>(xAxisC->Get("NUMBEROFDECIMALS"))->Set( xndec );
  static_cast<GRA_intCharacteristic*>(yAxisC->Get("NUMBEROFDECIMALS"))->Set( yndec );
  static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("LOWERAXIS"))->SetAsWorld( xlaxis );
  static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->SetAsWorld( xuaxis );
  static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("LOWERAXIS"))->SetAsWorld( ylaxis );
  static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("UPPERAXIS"))->SetAsWorld( yuaxis );
  static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MIN"))->Set( xmin );
  static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MAX"))->Set( xmax );
  static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MIN"))->Set( ymin );
  static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MAX"))->Set( ymax );
  static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("VIRTUALMIN"))->Set( xvmin );
  static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("VIRTUALMAX"))->Set( xvmax );
  static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("VIRTUALMIN"))->Set( yvmin );
  static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("VIRTUALMAX"))->Set( yvmax );
  static_cast<GRA_boolCharacteristic*>(xAxisC->Get("TICSON"))->Set( xtics );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get("TICSON"))->Set( ytics );
  static_cast<GRA_stringCharacteristic*>(generalC->Get("AUTOSCALE"))->Set( autoScale );
}

void GRA_densityPlot::DrawAxes( GRA_outputType *graphicsOutput, GRA_cartesianAxes *&axes )
{
  if( !drawAxes_ )return;
  std::vector<double> xt, yt;
  xt.push_back( dataXmin_ );
  xt.push_back( dataXmax_ );
  xt.push_back( dataXmax_ );
  xt.push_back( dataXmin_ );
  xt.push_back( dataXmin_ );
  yt.push_back( dataYmax_ );
  yt.push_back( dataYmax_ );
  yt.push_back( dataYmin_ );
  yt.push_back( dataYmin_ );
  yt.push_back( dataYmax_ );
  axes = new GRA_cartesianAxes(xt,yt,false,false);
  try
  {
    axes->Make();
  }
  catch (EGraphicsError &e)
  {
    delete axes;
    throw;
  }
  axes->Draw( graphicsOutput );
}

void GRA_densityPlot::DrawAxes2( GRA_outputType *graphicsOutput, GRA_cartesianAxes *axes )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *dataC = gw->GetDataCurveCharacteristics();
  if( border_ )
  {
    GRA_color *lineColor = static_cast<GRA_colorCharacteristic*>(dataC->Get("CURVECOLOR"))->Get();
    double x1, y1, x2, y2;
    gw->GraphToWorld( dataXmin_, dataYmin_, x1, y1, true );
    gw->GraphToWorld( dataXmax_, dataYmax_, x2, y2 );
    std::auto_ptr<GRA_rectangle> r( new GRA_rectangle(x1,y1,x2,y2,0.0,false,lineColor,0,1) );
    graphicsOutput->Draw( r.get() );
  }
  if( xProfile_ || yProfile_ )DrawProfiles( graphicsOutput );
  if( drawAxes_ )
  {
    axes->Draw(  graphicsOutput );
    delete axes;
  }
  if( resetAxes_ )
  {
    static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->SetAsPercent( xuaxisp_ );
    static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("UPPERAXIS"))->SetAsPercent( yuaxisp_ );
  }
}
// end of file

