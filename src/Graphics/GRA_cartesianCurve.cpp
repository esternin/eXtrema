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
#include <limits>
#include <cmath>

#include "GRA_cartesianCurve.h"
#include "EGraphicsError.h"
#include "GRA_wxWidgets.h"
#include "GRA_plotSymbol.h"
#include "GRA_polygon.h"
#include "GRA_errorBar.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_window.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"

GRA_cartesianCurve::~GRA_cartesianCurve()
{
  DeleteStuff();
  if( popup_ )ExGlobals::DisconnectCurvePopup();
}
  
void GRA_cartesianCurve::SetUp()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *curveC = gw->GetDataCurveCharacteristics();
  //
  std::size_t xSize = xData_.size();
  //
  GRA_intCharacteristic *psCharacteristic =
    static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("PLOTSYMBOL")));
  std::vector<int> symbols;
  if( psCharacteristic->IsVector() )
  {
    symbols.assign( psCharacteristic->Gets().begin(), psCharacteristic->Gets().end() );
    std::size_t psSize = symbols.size();
    if( smooth_ )
    {
      // make sure plot symbols are disconnected when plotting smooth data curve
      for( std::size_t i=0; i<psSize; ++i )
        symbols[i] = symbols[i]==0 ? -11 : -1*abs(symbols[i]); // 11 is a dot
      for( std::size_t i=psSize; i<xSize; ++i )symbols.push_back(-11);
    }
    else for( std::size_t i=psSize; i<xSize; ++i )symbols.push_back(11);
  }
  else
  {
    int s = psCharacteristic->Get();
    if( smooth_ )s = s==0 ? -11 : -1*abs(s);
    symbols.assign( xSize, s );
  }
  GRA_sizeCharacteristic *pssizeCharacteristic =
    static_cast<GRA_sizeCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLSIZE")));
  std::vector<double> sizes;
  if( pssizeCharacteristic->IsVector() )
  {
    sizes = pssizeCharacteristic->GetAsWorlds();
    std::size_t pssSize = sizes.size();
    for( std::size_t i=pssSize; i<xSize; ++i )sizes.push_back(sizes[pssSize-1]);
  }
  else sizes.assign( xSize, pssizeCharacteristic->GetAsWorld() );
  GRA_angleCharacteristic *psangleCharacteristic =
    static_cast<GRA_angleCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLANGLE")));
  std::vector<double> angles;
  if( psangleCharacteristic->IsVector() )
  {
    angles.assign( psangleCharacteristic->Gets().begin(), psangleCharacteristic->Gets().end() );
    std::size_t psaSize = angles.size();
    for( std::size_t i=psaSize; i<xSize; ++i )angles.push_back(angles[psaSize-1]);
  }
  else angles.assign( xSize, psangleCharacteristic->Get() );
  GRA_colorCharacteristic *pscolorCharacteristic =
    static_cast<GRA_colorCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLCOLOR")));
  std::vector<GRA_color*> colors;
  if( pscolorCharacteristic->IsVector() )
  {
    colors.assign( pscolorCharacteristic->Gets().begin(), pscolorCharacteristic->Gets().end() );
    std::size_t pscSize = colors.size();
    for( std::size_t i=pscSize; i<xSize; ++i )colors.push_back(colors[pscSize-1]);
  }
  else colors.assign( xSize, pscolorCharacteristic->Get() );
  GRA_intCharacteristic *pslwCharacteristic =
    static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLLINEWIDTH")));
  std::vector<int> lws;
  if( pslwCharacteristic->IsVector() )
  {
    lws.assign( pslwCharacteristic->Gets().begin(), pslwCharacteristic->Gets().end() );
    std::size_t pslwSize = lws.size();
    for( std::size_t i=pslwSize; i<xSize; ++i )lws.push_back(lws[pslwSize-1]);
  }
  else lws.assign( xSize, pslwCharacteristic->Get() );
  //
  for( std::size_t i=0; i<xSize; ++i )
    plotsymbols_.push_back( new GRA_plotSymbol(symbols[i],sizes[i],angles[i],colors[i],lws[i]) );
  //
  histogramType_ =
    static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("HISTOGRAMTYPE")))->Get();
  lineType_ =
    static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("CURVELINETYPE")))->Get();
  lineWidth_ =
    static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("CURVELINEWIDTH")))->Get();
  color_ =
    static_cast<GRA_colorCharacteristic*>(curveC->Get(wxT("CURVECOLOR")))->Get();
  GRA_colorCharacteristic *areaFillColorCharacteristic =
    static_cast<GRA_colorCharacteristic*>(generalC->Get(wxT("AREAFILLCOLOR")));
  if( areaFillColorCharacteristic->IsVector() )
  {
    areaFillColor_ = 0;
    areaFillColors_.assign( areaFillColorCharacteristic->Gets().begin(), areaFillColorCharacteristic->Gets().end() );
    std::size_t afcSize = areaFillColors_.size();
    for( std::size_t i=afcSize; i<xSize; ++i )areaFillColors_.push_back(areaFillColors_[afcSize-1]);
  }
  else areaFillColor_ = areaFillColorCharacteristic->Get();
  //
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  //
  xuaxis_ = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  xlaxis_ = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  yuaxis_ = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  ylaxis_ = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  xmin_ = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MIN")))->Get();
  xmax_ = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get(wxT("MAX")))->Get();
  ymin_ = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MIN")))->Get();
  ymax_ = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MAX")))->Get();
}

void GRA_cartesianCurve::DeleteStuff()
{
  while( !plotsymbols_.empty() )
  {
    delete plotsymbols_.back();
    plotsymbols_.pop_back();
  }
  while( !errorBars_.empty() )
  {
    delete errorBars_.back();
    errorBars_.pop_back();
  }
}

void GRA_cartesianCurve::CopyStuff( GRA_cartesianCurve const &rhs )
{
  histogramType_ = rhs.histogramType_;
  lineType_ = rhs.lineType_;
  lineWidth_ = rhs.lineWidth_;
  smooth_ = rhs.smooth_;
  color_ = rhs.color_;
  areaFillColor_ = rhs.areaFillColor_;
  areaFillColors_.assign( rhs.areaFillColors_.begin(), rhs.areaFillColors_.end() );
  //
  xlaxis_ = rhs.xlaxis_;
  ylaxis_ = rhs.ylaxis_;
  xuaxis_ = rhs.xuaxis_;
  yuaxis_ = rhs.yuaxis_;
  xmin_ = rhs.xmin_;
  ymin_ = rhs.ymin_;
  xmax_ = rhs.xmax_;
  ymax_ = rhs.ymax_;
  xData_.assign( rhs.xData_.begin(), rhs.xData_.end() );
  yData_.assign( rhs.yData_.begin(), rhs.yData_.end() );
  xE1_.assign( rhs.xE1_.begin(), rhs.xE1_.end() );
  xE2_.assign( rhs.xE2_.begin(), rhs.xE2_.end() );
  yE1_.assign( rhs.yE1_.begin(), rhs.yE1_.end() );
  yE2_.assign( rhs.yE2_.begin(), rhs.yE2_.end() );
  xCurve_.assign( rhs.xCurve_.begin(), rhs.xCurve_.end() );
  yCurve_.assign( rhs.yCurve_.begin(), rhs.yCurve_.end() );
  pen_.assign( rhs.pen_.begin(), rhs.pen_.end() );
  DeleteStuff();
  std::vector<GRA_plotSymbol*>::const_iterator sEnd = rhs.plotsymbols_.end();
  for( std::vector<GRA_plotSymbol*>::const_iterator i=rhs.plotsymbols_.begin(); i!=sEnd; ++i )
    plotsymbols_.push_back( new GRA_plotSymbol(**i) );
  std::vector<GRA_errorBar*>::const_iterator eEnd = rhs.errorBars_.end();
  for( std::vector<GRA_errorBar*>::const_iterator i=rhs.errorBars_.begin(); i!=eEnd; ++i )
    errorBars_.push_back( new GRA_errorBar(**i) );
  //
  popup_ = rhs.popup_;
}

void GRA_cartesianCurve::Make()
{
  switch ( histogramType_ )
  {
    case 1:
      MakeHorizontalHistogramNoTails();   // profile parallel to x-axis
      break;
    case 2:
      MakeHorizontalHistogramWithTails(); // profile parallel to x-axis
      break;
    case 3:
      MakeVerticalHistogramNoTails();     // profile parallel to y-axis
      break;
    case 4:
      MakeVerticalHistogramWithTails();   // profile parallel to y-axis
      break;
    default:
      MakeNonHistogram();
  }
  MakeErrorBars();
}

void GRA_cartesianCurve::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  ExGlobals::SetClippingBoundary( xlaxis_, ylaxis_, xuaxis_, yuaxis_ );
  switch ( histogramType_ )
  {
    case 1:
    case 3:
      DrawHistogramNoTails( graphicsOutput, dc );
      break;
    case 2:
    case 4:
      DrawHistogramWithTails( graphicsOutput, dc );
      break;
    default:
      DrawNonHistogram( graphicsOutput, dc );
  }
  DrawErrorBars( graphicsOutput, dc );
  ExGlobals::ResetClippingBoundary();
}

bool GRA_cartesianCurve::Inside( double x, double y )
{
  double const eps = 0.05;
  std::vector<double> xp(4,0.0), yp(4,0.0);
  if( histogramType_ == 0 )
  {
    int size = xCurve_.size();
    for( int i=1; i<size; ++i )
    {
      double L = sqrt((xCurve_[i]-xCurve_[i-1])*(xCurve_[i]-xCurve_[i-1])+
                      (yCurve_[i]-yCurve_[i-1])*(yCurve_[i]-yCurve_[i-1]));
      double cost = (xCurve_[i]-xCurve_[i-1])/L;
      double sint = (yCurve_[i]-yCurve_[i-1])/L;
      xp[0] = xCurve_[i-1] - eps*sint;
      yp[0] = yCurve_[i-1] + eps*cost;
      xp[1] = xCurve_[i-1] + eps*sint;
      yp[1] = yCurve_[i-1] - eps*cost;
      xp[2] = xCurve_[i] + (xCurve_[i-1]-xp[0]);
      yp[2] = yCurve_[i] - (yp[0]-yCurve_[i-1]);
      xp[3] = xCurve_[i] - (xCurve_[i-1]-xp[0]);
      yp[3] = yCurve_[i] + (yp[0]-yCurve_[i-1]);
      if( UsefulFunctions::InsidePolygon(x,y,xp,yp) )return true;
    }
    return false;
  }
  else if( histogramType_ == 1 )
  {
    int size = xCurve_.size();
    for( int i=0; i<size; i+=2 )
    {
      xp[0] = xCurve_[i] - eps;
      yp[0] = yCurve_[i];
      xp[1] = xCurve_[i] + eps;
      yp[1] = yCurve_[i];
      xp[2] = xCurve_[i+1] + eps;
      yp[2] = yCurve_[i+1];
      xp[3] = xCurve_[i+1] - eps;
      yp[3] = yCurve_[i+1];
      if( UsefulFunctions::InsidePolygon(x,y,xp,yp) )return true;
      if( i+2 >= size )break;
      xp[0] = xCurve_[i+1];
      yp[0] = yCurve_[i+1] - eps;
      xp[1] = xCurve_[i+1];
      yp[1] = yCurve_[i+1] + eps;
      xp[2] = xCurve_[i+2];
      yp[2] = yCurve_[i+2] + eps;
      xp[3] = xCurve_[i+2];
      yp[3] = yCurve_[i+2] - eps;
      if( UsefulFunctions::InsidePolygon(x,y,xp,yp) )return true;
    }
    return false;
  }
  else if( histogramType_ == 2 )
  {
    int size = xCurve_.size();
    for( int i=0; i<size; i+=4 )
    {
      xp[0] = xCurve_[i] - eps;
      yp[0] = yCurve_[i];
      xp[1] = xCurve_[i] + eps;
      yp[1] = yCurve_[i];
      xp[2] = xCurve_[i+1] + eps;
      yp[2] = yCurve_[i+1];
      xp[3] = xCurve_[i+1] - eps;
      yp[3] = yCurve_[i+1];
      if( UsefulFunctions::InsidePolygon(x,y,xp,yp) )return true;
      if( i+2 >= size )break;
      xp[0] = xCurve_[i+1];
      yp[0] = yCurve_[i+1] - eps;
      xp[1] = xCurve_[i+1];
      yp[1] = yCurve_[i+1] + eps;
      xp[2] = xCurve_[i+2];
      yp[2] = yCurve_[i+2] + eps;
      xp[3] = xCurve_[i+2];
      yp[3] = yCurve_[i+2] - eps;
      if( UsefulFunctions::InsidePolygon(x,y,xp,yp) )return true;
      if( i+3 >= size )break;
      xp[0] = xCurve_[i+2] - eps;
      yp[0] = yCurve_[i+2];
      xp[1] = xCurve_[i+2] + eps;
      yp[1] = yCurve_[i+2];
      xp[2] = xCurve_[i+3] + eps;
      yp[2] = yCurve_[i+3];
      xp[3] = xCurve_[i+3] - eps;
      yp[3] = yCurve_[i+3];
      if( UsefulFunctions::InsidePolygon(x,y,xp,yp) )return true;
    }
    return false;
  }
  else
    return false;
}

void GRA_cartesianCurve::MakeNonHistogram()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  int const xlog = static_cast<int>(
   static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get(wxT("LOGBASE")))->Get());
  int const ylog = static_cast<int>(
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get(wxT("LOGBASE")))->Get());
  std::vector<double> x, y;
  if( smooth_ )
  {
    std::size_t const nsmooth = 100;
    x.resize( nsmooth );
    y.resize( nsmooth );
    pen_.assign( nsmooth, 2 );
    pen_[0] = 3;
    UsefulFunctions::Splinterp( xData_, yData_, x, y );
    for( int i=0; i<nsmooth; ++i )
    {
      if( ylog>1 && y[i]<=0.0 )y[i] = std::numeric_limits<double>::min();
      if( xlog>1 && x[i]<=0.0 )x[i] = std::numeric_limits<double>::min();
    }
  }
  else
  {
    x.assign( xData_.begin(), xData_.end() );
    y.assign( yData_.begin(), yData_.end() );
    std::size_t size = xData_.size();
    pen_.assign( size, 3 );
    for( std::size_t i=0; i<size; ++i )
    {
      if( ylog>1 && y[i]<=0.0 )y[i] = std::numeric_limits<double>::min();
      if( xlog>1 && x[i]<=0.0 )x[i] = std::numeric_limits<double>::min();
      if( plotsymbols_[i]->GetConnectToPrevious() )pen_[i] = 2;
    }
    pen_[0] = 3;
  }
  gw->GraphToWorld( x, y, xCurve_, yCurve_ );
}

void GRA_cartesianCurve::MakeHorizontalHistogramNoTails()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  std::vector<double> x, y;
  std::size_t const npt = xData_.size();
  if( npt == 1 )
  {
    x.push_back( xData_[0] );
    y.push_back( 0.0 );
    x.push_back( xData_[0] );
    y.push_back( yData_[0] );
  }
  else
  {
    double dx = 0.5*fabs(xData_[1]-xData_[0]);
    x.push_back( xData_[0]-dx );
    y.push_back( 0.0 );
    //
    x.push_back( xData_[0]-dx );
    y.push_back( yData_[0] );
    //
    x.push_back( xData_[0]+dx );
    y.push_back( yData_[0] );
    //
    double const eps = 0.01;
    for( std::size_t i=1; i<npt-1; ++i )
    {
      double dx2 = std::min(0.5*fabs(xData_[i]-xData_[i-1]),0.5*fabs(xData_[i+1]-xData_[i]));
      if( eps < (xData_[i]-xData_[i-1])-(dx+dx2) )
      {
        x.push_back( xData_[i-1]+dx );
        y.push_back( 0.0 );
        x.push_back( xData_[i]-dx2 );
        y.push_back( 0.0 );
      }
      x.push_back( xData_[i]-dx2 );
      y.push_back( yData_[i] );
      //
      x.push_back( xData_[i]+dx2 );
      y.push_back( yData_[i] );
      //
      dx = dx2;
    }
    double dx2 = 0.5*fabs(xData_[npt-1]-xData_[npt-2]);
    if( eps < (xData_[npt-1]-xData_[npt-2])-(dx+dx2) )
    {
      x.push_back( xData_[npt-2]+dx );
      y.push_back( 0.0 );
      x.push_back( xData_[npt-1]-dx2 );
      y.push_back( 0.0 );
    }
    x.push_back( xData_[npt-1]-dx2 );
    y.push_back( yData_[npt-1] );
    //
    x.push_back( xData_[npt-1]+dx2 );
    y.push_back( yData_[npt-1] );
    //
    x.push_back( xData_[npt-1]+dx2 );
    y.push_back( 0.0 );
  }
  gw->GraphToWorld( x, y, xCurve_, yCurve_ );
}

void GRA_cartesianCurve::MakeHorizontalHistogramWithTails()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  std::vector<double> x, y;
  std::size_t npt = xData_.size();
  if( npt == 1 )
  {
    x.push_back( xData_[0] );
    y.push_back( 0.0 );
    //
    x.push_back( xData_[0] );
    y.push_back( yData_[0] );
  }
  else
  {
    double dx = 0.5*fabs(xData_[1]-xData_[0]);
    x.push_back( xData_[0]-dx );
    y.push_back( 0.0 );
    //
    x.push_back( xData_[0]-dx );
    y.push_back( yData_[0] );
    //
    x.push_back( xData_[0]+dx );
    y.push_back( yData_[0] );
    //
    x.push_back( xData_[0]+dx );
    y.push_back( 0.0 );
    //
    for( std::size_t i=1; i<npt-1; ++i )
    {
      dx = std::min(0.5*fabs(xData_[i]-xData_[i-1]),0.5*fabs(xData_[i+1]-xData_[i]));
      x.push_back( xData_[i]-dx );
      y.push_back( 0.0 );
      //
      x.push_back( xData_[i]-dx );
      y.push_back( yData_[i] );
      //
      x.push_back( xData_[i]+dx );
      y.push_back( yData_[i] );
      //
      x.push_back( xData_[i]+dx );
      y.push_back( 0.0 );
    }
    dx = 0.5*fabs(xData_[npt-1]-xData_[npt-2]);
    x.push_back( xData_[npt-1]-dx );
    y.push_back( 0.0 );
    //
    x.push_back( xData_[npt-1]-dx );
    y.push_back( yData_[npt-1] );
    //
    x.push_back( xData_[npt-1]+dx );
    y.push_back( yData_[npt-1] );
    //
    x.push_back( xData_[npt-1]+dx );
    y.push_back( 0.0 );
  }
  gw->GraphToWorld( x, y, xCurve_, yCurve_ );
}

void GRA_cartesianCurve::MakeVerticalHistogramNoTails()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  std::vector<double> x, y;
  std::size_t npt = xData_.size();
  if( npt == 1 )
  {
    x.push_back( 0.0 );
    y.push_back( yData_[0] );
    x.push_back( xData_[0] );
    y.push_back( yData_[0] );
  }
  else
  {
    double dy = 0.5*fabs(yData_[1]-yData_[0]);
    x.push_back( 0.0 );
    y.push_back( yData_[0]-dy );
    //
    x.push_back( xData_[0] );
    y.push_back( yData_[0]-dy );
    //
    x.push_back( xData_[0] );
    y.push_back( yData_[0]+dy );
    //
    double const eps = 0.01;
    for( std::size_t i=1; i<npt-1; ++i )
    {
      double dy2 = std::min(0.5*fabs(yData_[i]-yData_[i-1]),0.5*fabs(yData_[i+1]-yData_[i]));
      if( eps < (yData_[i]-yData_[i-1])-(dy+dy2) )
      {
        x.push_back( 0.0 );
        y.push_back( yData_[i-1]+dy );
        x.push_back( 0.0 );
        y.push_back( yData_[i]-dy2 );
      }
      x.push_back( xData_[i] );
      y.push_back( yData_[i]-dy2 );
      //
      x.push_back( xData_[i] );
      y.push_back( yData_[i]+dy2 );
      //
      dy = dy2;
    }
    double dy2 = 0.5*fabs(yData_[npt-1]-yData_[npt-2]);
    if( eps < (yData_[npt-1]-yData_[npt-2])-(dy+dy2) )
    {
      x.push_back( 0.0 );
      y.push_back( yData_[npt-2]+dy );
      x.push_back( 0.0 );
      y.push_back( yData_[npt-1]-dy2 );
    }
    x.push_back( xData_[npt-1] );
    y.push_back( yData_[npt-1]-dy2 );
    //
    x.push_back( xData_[npt-1] );
    y.push_back( yData_[npt-1]+dy2 );
    //
    x.push_back( 0.0 );
    y.push_back( yData_[npt-1]+dy2 );
  }
  gw->GraphToWorld( x, y, xCurve_, yCurve_ );
}

void GRA_cartesianCurve::MakeVerticalHistogramWithTails()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  std::vector<double> x, y;
  std::size_t npt = xData_.size();
  if( npt == 1 )
  {
    x.push_back( 0.0 );
    y.push_back( yData_[0] );
    x.push_back( xData_[0] );
    y.push_back( yData_[0] );
  }
  else
  {
    double dy = 0.5*fabs(yData_[1]-yData_[0]);
    x.push_back( 0.0 );
    y.push_back( yData_[0]-dy );
    //
    x.push_back( xData_[0] );
    y.push_back( yData_[0]-dy );
    //
    x.push_back( xData_[0] );
    y.push_back( yData_[0]+dy );
    //
    x.push_back( 0.0 );
    y.push_back( yData_[0]+dy );
    //
    for( std::size_t i=1; i<npt-1; ++i )
    {
      dy = std::min(0.5*fabs(yData_[i]-yData_[i-1]),0.5*fabs(yData_[i+1]-yData_[i]));
      x.push_back( 0.0 );
      y.push_back( yData_[i]-dy );
      //
      x.push_back( xData_[i] );
      y.push_back( yData_[i]-dy );
      //
      x.push_back( xData_[i] );
      y.push_back( yData_[i]+dy );
      //
      x.push_back( 0.0 );
      y.push_back( yData_[i]+dy );
    }
    dy = 0.5*fabs(yData_[npt-1]-yData_[npt-2]);
    x.push_back( 0.0 );
    y.push_back( yData_[npt-1]-dy );
    //
    x.push_back( xData_[npt-1] );
    y.push_back( yData_[npt-1]-dy );
    //
    x.push_back( xData_[npt-1] );
    y.push_back( yData_[npt-1]+dy );
    //
    x.push_back( 0.0 );
    y.push_back( yData_[npt-1]+dy );
  }
  gw->GraphToWorld( x, y, xCurve_, yCurve_ );
}

void GRA_cartesianCurve::DrawNonHistogram( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  if( !areaFillColors_.empty() )
  {
    areaFillColor_ = areaFillColors_[0];
    std::vector<GRA_color*>().swap( areaFillColors_ );
  }
  if( areaFillColor_ )
  {
    // draw the filled region
    // make sure polygon is closed, may have to add last point set to first point
    //
    bool test = false;
    if( xCurve_.front()!=xCurve_.back() || yCurve_.front()!=yCurve_.back() )
    {
      test = true;
      xCurve_.push_back( xCurve_.front() );
      yCurve_.push_back( yCurve_.front() );
    }
    GRA_polygon p( xCurve_, yCurve_, color_, areaFillColor_, lineWidth_ );
    p.Draw( graphicsOutput, dc );
    if( test )
    {
      xCurve_.erase( xCurve_.end()-1 );
      yCurve_.erase( yCurve_.end()-1 );
    }
  }
  // draw the data curve itself (which may be smoothed curve)
  //
  int lineTypeSave = graphicsOutput->GetLineType();
  graphicsOutput->SetLineType( lineType_ );
  wxPen wxpen( dc.GetPen() );
  wxpen.SetColour( ExGlobals::GetwxColor(color_) );
  wxpen.SetWidth( lineWidth_ );
  dc.SetPen( wxpen );
  std::size_t size = xCurve_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    pen_[i]==2 ? graphicsOutput->ContinueLine( xCurve_[i], yCurve_[i], dc ) :
                 graphicsOutput->StartLine( xCurve_[i], yCurve_[i] );
  }
  //
  // draw the plot symbols at the data coordinates
  //
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  for( std::size_t i=0; i<size; ++i )
  {
    if( xmin<=xCurve_[i] && xCurve_[i]<=xmax && ymin<=yCurve_[i] && yCurve_[i]<=ymax )
    {
      plotsymbols_[i]->SetLocation( xCurve_[i], yCurve_[i] );
      plotsymbols_[i]->Draw( graphicsOutput, dc );
    }
  }
  graphicsOutput->SetLineType( lineTypeSave );
}

void GRA_cartesianCurve::DrawHistogramNoTails( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  if( !areaFillColors_.empty() )
  {
    areaFillColor_ = areaFillColors_[0];
    std::vector<GRA_color*>().swap( areaFillColors_ );
  }
  if( areaFillColor_ && xCurve_.size()>1 )
  {
    // draw the filled region
    // make sure polygon is closed, add last point set to first point
    //
    xCurve_.push_back( xCurve_.front() );
    yCurve_.push_back( yCurve_.front() );
    GRA_polygon p( xCurve_, yCurve_, color_, areaFillColor_, lineWidth_ );
    p.Draw( graphicsOutput, dc );
    xCurve_.erase( xCurve_.end()-1 );
    yCurve_.erase( yCurve_.end()-1 );
  }
  wxPen wxpen( dc.GetPen() );
  wxpen.SetColour( ExGlobals::GetwxColor(color_) );
  wxpen.SetWidth( lineWidth_ );
  dc.SetPen( wxpen );
  std::size_t size = xCurve_.size();
  graphicsOutput->PenUp( xCurve_[0], yCurve_[0] );
  for( std::size_t i=1; i<size; ++i )graphicsOutput->PenDown( xCurve_[i], yCurve_[i], dc );
}

void GRA_cartesianCurve::DrawHistogramWithTails( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  std::size_t npt = xData_.size();
  if( npt > 1 )
  {
    std::vector<double> xp, yp;
    bool areaFillColorVector = !areaFillColors_.empty();
    for( std::size_t i=0; i<npt; ++i )
    {
      GRA_color *fillColor;
      areaFillColorVector ? fillColor=areaFillColors_[i%areaFillColors_.size()] :
                            fillColor=areaFillColor_;
      if( fillColor )
      {
        std::size_t j = 4*i;
        xp.push_back( xCurve_[j] );
        yp.push_back( yCurve_[j] );
        xp.push_back( xCurve_[j+1] );
        yp.push_back( yCurve_[j+1] );
        xp.push_back( xCurve_[j+2] );
        yp.push_back( yCurve_[j+2] );
        xp.push_back( xCurve_[j+3] );
        yp.push_back( yCurve_[j+3] );
        xp.push_back( xCurve_[j] );
        yp.push_back( yCurve_[j] );
        GRA_polygon p( xp, yp, color_, fillColor, lineWidth_ );
        p.Draw( graphicsOutput, dc );
        std::vector<double>().swap( xp );
        std::vector<double>().swap( yp );
      }
    }
  }
  wxPen wxpen( dc.GetPen() );
  wxpen.SetColour( ExGlobals::GetwxColor(color_) );
  wxpen.SetWidth( lineWidth_ );
  dc.SetPen( wxpen );
  if( npt == 1 )
  {
    graphicsOutput->PenUp( xCurve_[0], yCurve_[0] );
    graphicsOutput->PenDown( xCurve_[1], yCurve_[1], dc );
  }
  else
  {
    for( std::size_t i=0; i<npt; ++i )
    {
      std::size_t j = 4*i;
      graphicsOutput->PenUp( xCurve_[j], yCurve_[j] );
      graphicsOutput->PenDown( xCurve_[j+1], yCurve_[j+1], dc );
      graphicsOutput->PenDown( xCurve_[j+2], yCurve_[j+2], dc );
      graphicsOutput->PenDown( xCurve_[j+3], yCurve_[j+3], dc );
      graphicsOutput->PenDown( xCurve_[j],   yCurve_[j], dc );
    }
  }
}

void GRA_cartesianCurve::MakeErrorBars()
{
  while( !errorBars_.empty() )
  {
    delete errorBars_.back();
    errorBars_.pop_back();
  }
  if( xE1_.empty() && yE1_.empty() )return;
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  double xl=0.0, yl=0.0, xu=0.0, yu=0.0;
  int npt = xData_.size();
  for( int i=0; i<npt; ++i )
  {
    std::vector<double> xLoc, yLoc;
    std::vector<int> pen;
    //
    if( !xE1_.empty() )
    {
      if( xmax_ > xmin_ )
      {
        xl = xData_[i] - xE1_[i];
        xu = xData_[i] + xE2_[i];
      }
      else
      {
        xl = xData_[i] + xE1_[i];
        xu = xData_[i] - xE2_[i];
      }
    }
    if( !yE1_.empty() )
    {
      if( ymax_ > ymin_ )
      {
        yl = yData_[i] - yE1_[i];
        yu = yData_[i] + yE2_[i];
      }
      else
      {
        yl = yData_[i] + yE1_[i];
        yu = yData_[i] - yE2_[i];
      }
    }
    double footSize = 0.0;
    if( (histogramType_==0) && plotsymbols_[i] )footSize = plotsymbols_[i]->GetSize()*0.5;
    //
    double x, y, xLeft, xRight, yLower, yUpper;
    double xc, yc;
    gw->GraphToWorld( xData_[i], yData_[i], xc, yc );
    GRA_color *color = plotsymbols_[i]->GetColor();
    if( !xE1_.empty() )
    {
      gw->GraphToWorld( xl, yData_[i], xLeft, y );
      gw->GraphToWorld( xu, yData_[i], xRight, y );
      errorBars_.push_back( new GRA_errorBar(xc,yc,xc-xLeft,xRight-xc,false,footSize,color,lineWidth_) );
    }
    if( !yE1_.empty() )
    {
      gw->GraphToWorld( xData_[i], yl, x, yLower );
      gw->GraphToWorld( xData_[i], yu, x, yUpper );
      errorBars_.push_back( new GRA_errorBar(xc,yc,yc-yLower,yUpper-yc,true,footSize,color,lineWidth_) );
    }
  }
}

void GRA_cartesianCurve::DrawErrorBars( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  if( errorBars_.empty() )return;
  //
  std::vector<GRA_errorBar*>::const_iterator end = errorBars_.end();
  for( std::vector<GRA_errorBar*>::const_iterator i=errorBars_.begin(); i!=end; ++i )
    (*i)->Draw( graphicsOutput, dc );
}

std::ostream &operator<<( std::ostream &out, GRA_cartesianCurve const &cc )
{
  out << "<cartesiancurve histype=\"" << cc.histogramType_ << "\" linetype=\""
      << cc.lineType_ << "\" linewidth=\"" << cc.lineWidth_ << "\" smooth=\""
      << cc.smooth_ << "\" linecolor=\"" << GRA_colorControl::GetColorCode(cc.color_) << "\" areafillcolor=\""
      << GRA_colorControl::GetColorCode(cc.areaFillColor_) << "\" xlaxis=\""
      << cc.xlaxis_ << "\" ylaxis=\"" << cc.ylaxis_ << "\" xuaxis=\"" << cc.xuaxis_
      << "\" yuaxis=\"" << cc.yuaxis_ << "\" xmin=\"" << cc.xmin_ << "\" xmax=\""
      << cc.xmax_ << "\" ymin=\"" << cc.ymin_ << "\" ymax=\"" << cc.ymax_ << "\">\n";
  std::size_t size = cc.areaFillColors_.size();
  out << "<areafillcolors size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )
    out << GRA_colorControl::GetColorCode(cc.areaFillColors_[i]) << " ";
  out << "</areafillcolors>\n";
  size = cc.xData_.size();
  out << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << cc.xData_[i] << " " << cc.yData_[i] << " ";
  out << "</data>\n";
  size = cc.xE1_.size();
  out << "<xe1 size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << cc.xE1_[i] << " ";
  out << "</xe1>\n";
  size = cc.xE2_.size();
  out << "<xe2 size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << cc.xE2_[i] << " ";
  out << "</xe2>\n";
  size = cc.yE1_.size();
  out << "<ye1 size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << cc.yE1_[i] << " ";
  out << "</ye1>\n";
  size = cc.yE2_.size();
  out << "<ye2 size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << cc.yE2_[i] << " ";
  out << "</ye2>\n";
  size = cc.xCurve_.size();
  out << "<xycurve size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << cc.xCurve_[i] << " " << cc.yCurve_[i] << " ";
  out << "</xycurve>\n";
  size = cc.pen_.size();
  out << "<pen size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << cc.pen_[i] << " ";
  out << "</pen>\n";
  size = cc.plotsymbols_.size();
  out << "<plotsymbols size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << *cc.plotsymbols_[i];
  out << "</plotsymbols>\n";
  size = cc.errorBars_.size();
  out << "<errorbars size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << *cc.errorBars_[i];
  return out << "</errorbars>\n</cartesiancurve>\n";
}

void GRA_cartesianCurve::SetValues( int histype, int linetype, int linewidth,
                                    GRA_color* linecolor, GRA_color *areafillcolor,
                                    double xlaxis, double ylaxis, double xuaxis,
                                    double yuaxis, double xmin, double ymin, double xmax,
                                    double ymax, std::vector<GRA_color*> &areaFillColors,
                                    std::vector<double> &xcurve,
                                    std::vector<double> &ycurve,
                                    std::vector<int> &pen,
                                    std::vector<GRA_plotSymbol*> &symbols,
                                    std::vector<GRA_errorBar*> &errorbars )
{
  histogramType_ = histype;
  lineType_ = linetype;
  lineWidth_ = linewidth;
  color_ = linecolor;
  areaFillColor_ = areafillcolor;
  xlaxis_ = xlaxis;
  ylaxis_ = ylaxis;
  xuaxis_ = xuaxis;
  yuaxis_ = yuaxis;
  xmin_ = xmin;
  ymin_ = ymin;
  xmax_ = xmax;
  ymax_ = ymax;
  areaFillColors_.assign( areaFillColors.begin(), areaFillColors.end() );
  xCurve_.assign( xcurve.begin(), xcurve.end() );
  yCurve_.assign( ycurve.begin(), ycurve.end() );
  pen_.assign( pen.begin(), pen.end() );
  plotsymbols_.assign( symbols.begin(), symbols.end() );
  errorBars_.assign( errorbars.begin(), errorbars.end() );
}

void GRA_cartesianCurve::GetXYMinMax( double &xmin, double &xmax, double &ymin, double &ymax )
{
  std::size_t num = xData_.size();
  xmin = std::numeric_limits<double>::max();
  xmax = -xmin;
  ymin = xmin;
  ymax = xmax;
  if( !yE1_.empty() )
  {
    if( !yE2_.empty() )
    {
      for( std::size_t i=0; i<num; ++i )
      {
        if( ymax < yData_[i]+yE2_[i] )ymax = yData_[i]+yE2_[i];
        if( ymin > yData_[i]-yE1_[i] )ymin = yData_[i]-yE1_[i];
      }
    }
    else
    {
      for( std::size_t i=0; i<num; ++i )
      {
        if( ymax < yData_[i]+yE1_[i] )ymax = yData_[i]+yE1_[i];
        if( ymin > yData_[i]-yE1_[i] )ymin = yData_[i]-yE1_[i];
      }
    }
  }
  else
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( ymax < yData_[i] )ymax = yData_[i];
      if( ymin > yData_[i] )ymin = yData_[i];
    }
  }
  if( !xE1_.empty() )
  {
    if( !xE2_.empty() )
    {
      for( std::size_t i=0; i<num; ++i )
      {
        if( xmax < xData_[i]+xE2_[i] )xmax = xData_[i]+xE2_[i];
        if( xmin > xData_[i]-xE1_[i] )xmin = xData_[i]-xE1_[i];
      }
    }
    else
    {
      for( std::size_t i=0; i<num; ++i )
      {
        if( xmax < xData_[i]+xE1_[i] )xmax = xData_[i]+xE1_[i];
        if( xmin > xData_[i]-xE1_[i] )xmin = xData_[i]-xE1_[i];
      }
    }
  }
  else
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( xmax < xData_[i] )xmax = xData_[i];
      if( xmin > xData_[i] )xmin = xData_[i];
    }
  }
}

void GRA_cartesianCurve::GetYMinMax( double const xmin, double const xmax,
                                     double &ymin, double &ymax )
{
  std::size_t num = xData_.size();
  ymin = std::numeric_limits<double>::max();
  ymax = -ymin;
  bool flag = true;
  for( std::size_t j=0; j<num; ++j )
  {
    if( xmin>xData_[j] || xData_[j]>xmax )continue;
    if( flag )
    {
      ymin = yData_[j];
      ymax = yData_[j];
      flag = false;
      if( !yE1_.empty() )
      {
        if( !yE2_.empty() )
        {
          ymax += yE2_[j];
          ymin -= yE1_[j];
        }
        else
        {
          ymax += yE1_[j];
          ymin -= yE1_[j];
        }
      }
    }
    else
    {
      if( !yE1_.empty() )
      {
        if( !yE2_.empty() )
        {
          if( ymax < yData_[j]+yE2_[j] )ymax = yData_[j]+yE2_[j];
          if( ymin > yData_[j]-yE1_[j] )ymin = yData_[j]-yE1_[j];
        }
        else
        {
          if( ymax < yData_[j]+yE1_[j] )ymax = yData_[j]+yE1_[j];
          if( ymin > yData_[j]-yE1_[j] )ymin = yData_[j]-yE1_[j];
        }
      }
      else
      {
        if( ymin > yData_[j] )ymin = yData_[j];
        if( ymax < yData_[j] )ymax = yData_[j];
      }
    }
  }
}

void GRA_cartesianCurve::GetXMinMax( double const ymin, double const ymax,
                                     double &xmin, double &xmax )
{
  std::size_t num = xData_.size();
  xmin = std::numeric_limits<double>::max();
  xmax = -xmin;
  bool flag = true;
  for( std::size_t j=0; j<num; ++j )
  {
    if( ymin>yData_[j] || yData_[j]>ymax )continue;
    if( flag )
    {
      xmin = xData_[j];
      xmax = xData_[j];
      flag = false;
      if( !xE1_.empty() )
      {
        if( !xE2_.empty() )
        {
          xmax += xE2_[j];
          xmin -= xE1_[j];
        }
        else
        {
          xmax += xE1_[j];
          xmin -= xE1_[j];
        }
      }
    }
    else
    {
      if( !xE1_.empty() )
      {
        if( !xE2_.empty() )
        {
          if( xmax < xData_[j]+xE2_[j] )xmax = xData_[j]+xE2_[j];
          if( xmin > xData_[j]-xE1_[j] )xmin = xData_[j]-xE1_[j];
        }
        else
        {
          if( xmax < xData_[j]+xE1_[j] )xmax = xData_[j]+xE1_[j];
          if( xmin > xData_[j]-xE1_[j] )xmin = xData_[j]-xE1_[j];
        }
      }
      else
      {
        if( xmin > xData_[j] )xmin = xData_[j];
        if( xmax < xData_[j] )xmax = xData_[j];
      }
    }
  }
}

int GRA_cartesianCurve::GetPlotsymbolCode() const
{ return plotsymbols_[0]->GetShapeCode(); }

void GRA_cartesianCurve::SetPlotsymbolCode( int ps )
{
  int size = plotsymbols_.size();
  for( int i=0; i<size; ++i )plotsymbols_[i]->SetShapeCode( ps );
}

GRA_color *GRA_cartesianCurve::GetPlotsymbolColor() const
{ return plotsymbols_[0]->GetColor(); }

void GRA_cartesianCurve::SetPlotsymbolColor( GRA_color *c )
{
  int size = plotsymbols_.size();
  for( int i=0; i<size; ++i )plotsymbols_[i]->SetColor( c );
}

double GRA_cartesianCurve::GetPlotsymbolSize() const
{ return plotsymbols_[0]->GetSize(); }

void GRA_cartesianCurve::SetPlotsymbolSize( double s )
{
  int size = plotsymbols_.size();
  for( int i=0; i<size; ++i )plotsymbols_[i]->SetSize( s );
}

double GRA_cartesianCurve::GetPlotsymbolAngle() const
{ return plotsymbols_[0]->GetAngle(); }

void GRA_cartesianCurve::SetPlotsymbolAngle( double a )
{
  int size = plotsymbols_.size();
  for( int i=0; i<size; ++i )plotsymbols_[i]->SetAngle( a );
}

GRA_color *GRA_cartesianCurve::GetAreaFillColor()
{
  GRA_colorCharacteristic *areaFillColorCharacteristic =
      static_cast<GRA_colorCharacteristic*>(ExGlobals::GetGraphWindow()->GetGeneralCharacteristics()->
                                            Get(wxT("AREAFILLCOLOR")));
  if( areaFillColorCharacteristic->IsVector() )return areaFillColors_[0];
  else                                         return areaFillColor_;
}

// end of file
