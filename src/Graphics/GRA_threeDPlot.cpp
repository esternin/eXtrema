/*
Copyright (C) 2010 Joseph L. Chuma

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
#include "GRA_threeDPlot.h"
#include "ExGlobals.h"
#include "EGraphicsError.h"
#include "UsefulFunctions.h"
#include "GRA_wxWidgets.h"
#include "GRA_plotSymbol.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_intCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_drawableText.h"
#include "GRA_window.h"

void GRA_threeDPlot::DeleteStuff()
{
  while( !plotSymbols_.empty() )
  {
    delete plotSymbols_.back();
    plotSymbols_.pop_back();
  }
}

void GRA_threeDPlot::CopyStuff( GRA_threeDPlot const &rhs )
{
  threeDForm_ = rhs.threeDForm_;
  x_.assign( rhs.x_.begin(), rhs.x_.end() );
  y_.assign( rhs.y_.begin(), rhs.y_.end() );
  z_.assign( rhs.z_.begin(), rhs.z_.end() );
  xmin_ = rhs.xmin_;
  xmax_ = rhs.xmax_;
  ymin_ = rhs.ymin_;
  ymax_ = rhs.ymax_;
  zmin_ = rhs.zmin_;
  zmax_ = rhs.zmax_;
  xmn_ = rhs.xmn_;
  xmx_ = rhs.xmx_;
  xinc_ = rhs.xinc_;
  ymn_ = rhs.ymn_;
  ymx_ = rhs.ymx_;
  yinc_ = rhs.yinc_;
  zmn_ = rhs.zmn_;
  zmx_ = rhs.zmx_;
  zinc_ = rhs.zinc_;
  nlxinc_ = rhs.nlxinc_;
  nlyinc_ = rhs.nlyinc_;
  nlzinc_ = rhs.nlzinc_;
  xc_ = rhs.xc_;
  yc_ = rhs.yc_;
  zc_ = rhs.zc_;

  std::vector<GRA_plotSymbol*>::const_iterator sEnd = rhs.plotSymbols_.end();
  for( std::vector<GRA_plotSymbol*>::const_iterator i=rhs.plotSymbols_.begin(); i!=sEnd; ++i )
    plotSymbols_.push_back( new GRA_plotSymbol(**i) );

  x2_.assign( rhs.x2_.begin(), rhs.x2_.end() );
  y2_.assign( rhs.y2_.begin(), rhs.y2_.end() );
  z2_.assign( rhs.z2_.begin(), rhs.z2_.end() );
  xn_.assign( rhs.xn_.begin(), rhs.xn_.end() );
  yn_.assign( rhs.yn_.begin(), rhs.yn_.end() );
  zn_.assign( rhs.zn_.begin(), rhs.zn_.end() );
  p2_.assign( rhs.p2_.begin(), rhs.p2_.end() );
  eye2object_ = rhs.eye2object_;
  screenHalfSize_ = rhs.screenHalfSize_;
  ds_ = rhs.ds_;
  angleIncrement_ = rhs.angleIncrement_;
  xAxisAngle_ = rhs.xAxisAngle_;
  yAxisAngle_ = rhs.yAxisAngle_;
}

void GRA_threeDPlot::SetupData()
{
  std::size_t npt = x_.size();
  UsefulFunctions::MinMax( x_, 0, npt, xmin_, xmax_ );
  UsefulFunctions::MinMax( y_, 0, npt, ymin_, ymax_ );
  UsefulFunctions::MinMax( z_, 0, npt, zmin_, zmax_ );

  UsefulFunctions::Scale1( xmn_, xmx_, xinc_, 5, xmin_, xmax_ );
  nlxinc_ = static_cast<int>( (xmx_-xmn_)/xinc_ + 0.5 );
  xmin_ = xmn_;
  xmax_ = xmx_;
  xmn_ -= 0.1*(xmax_-xmin_);
  xmx_ += 0.1*(xmax_-xmin_);

  UsefulFunctions::Scale1( ymn_, ymx_, yinc_, 5, ymin_, ymax_ );
  nlyinc_ = static_cast<int>( (ymx_-ymn_)/yinc_ + 0.5 );
  ymin_ = ymn_;
  ymax_ = ymx_;
  ymn_ -= 0.1*(ymax_-ymin_);
  ymx_ += 0.1*(ymax_-ymin_);

  UsefulFunctions::Scale1( zmn_, zmx_, zinc_, 5, zmin_, zmax_ );
  nlzinc_ = static_cast<int>( (zmx_-zmn_)/zinc_ + 0.5 );
  zmin_ = zmn_;
  zmax_ = zmx_;
  zmn_ -= 0.1*(zmax_-zmin_);
  zmx_ += 0.1*(zmax_-zmin_);

  xc_ = (xmn_+xmx_)/2;  // centre of object = (xc_,yc_,zc_)
  yc_ = (ymn_+ymx_)/2;
  zc_ = (zmn_+zmx_)/2;
}

void GRA_threeDPlot::SetupPlotVectors()
{
  // z becomes 2*d+z in eye coordinates
  //
  std::size_t npt = x_.size();
  int nxyz = 2*(nlxinc_+nlyinc_+nlzinc_) + 12;
  x2_.resize( nxyz );
  y2_.resize( nxyz );
  z2_.resize( nxyz );
  p2_.resize( nxyz );
  xn_.resize( 6 );
  yn_.resize( 6 );
  zn_.resize( 6 );

  xn_[0] = xmn_-0.05*(xmx_-xmn_);    // zmax_
  yn_[0] = ymn_;
  zn_[0] = zmax_;
  xn_[1] = xmn_-0.05*(xmx_-xmn_);    // zmin_
  yn_[1] = ymn_;
  zn_[1] = zmin_;
  xn_[2] = xmn_-0.05*(xmx_-xmn_);    // ymin_
  yn_[2] = ymin_;
  zn_[2] = zmn_;
  xn_[3] = xmn_-0.05*(xmx_-xmn_);    // ymax_
  yn_[3] = ymax_;
  zn_[3] = zmn_;
  xn_[4] = xmin_;                  // xmin_
  yn_[4] = ymx_+0.05*(ymx_-ymn_);
  zn_[4] = zmn_;
  xn_[5] = xmax_;                  // xmax_
  yn_[5] = ymx_+0.05*(ymx_-ymn_);
  zn_[5] = zmn_;

  x2_[0] = xmn_;                   // z axis
  y2_[0] = ymn_;
  z2_[0] = zmx_;
  p2_[0] = 3;
  x2_[1] = xmn_;
  y2_[1] = ymn_;
  z2_[1] = zmn_;
  p2_[1] = 2;
  for( int i=1; i<=nlzinc_+1; ++i )
  {
    x2_[2*i] = xmn_-0.05*(xmx_-xmn_);
    y2_[2*i] = ymn_;
    z2_[2*i] = zmax_-(i-1)*zinc_;
    p2_[2*i] = 3;
    x2_[2*i+1] = xmn_;
    y2_[2*i+1] = ymn_;
    z2_[2*i+1] = zmax_-(i-1)*zinc_;
    p2_[2*i+1] = 2;
  }
  int nn = 2*nlzinc_+4;
  x2_[nn] = xmn_;                  // y axis
  y2_[nn] = ymn_;
  z2_[nn] = zmn_;
  p2_[nn] = 3;
  x2_[nn+1] = xmn_;
  y2_[nn+1] = ymx_;
  z2_[nn+1] = zmn_;
  p2_[nn+1] = 2;
  nn += 2;
  for( int i=1; i<=nlyinc_+1; ++i )
  {
    x2_[nn+2*(i-1)] = xmn_-0.05*(xmx_-xmn_);
    y2_[nn+2*(i-1)] = ymin_+(i-1)*yinc_;
    z2_[nn+2*(i-1)] = zmn_;
    p2_[nn+2*(i-1)] = 3;
    x2_[nn+2*(i-1)+1] = xmn_;
    y2_[nn+2*(i-1)+1] = ymin_+(i-1)*yinc_;
    z2_[nn+2*(i-1)+1] = zmn_;
    p2_[nn+2*(i-1)+1] = 2;
  }
  nn += 2*nlyinc_+2;
  x2_[nn] = xmn_;                  // x axis
  y2_[nn] = ymx_;
  z2_[nn] = zmn_;
  p2_[nn] = 3;
  x2_[nn+1] = xmx_;
  y2_[nn+1] = ymx_;
  z2_[nn+1] = zmn_;
  p2_[nn+1] = 2;
  nn += 2;
  for( int i=1; i<=nlxinc_+1; ++i )
  {
    x2_[nn+2*(i-1)] = xmin_+(i-1)*xinc_;
    y2_[nn+2*(i-1)] = ymx_+0.05*(ymx_-ymn_);
    z2_[nn+2*(i-1)] = zmn_;
    p2_[nn+2*(i-1)] = 3;
    x2_[nn+2*(i-1)+1] = xmin_+(i-1)*xinc_;
    y2_[nn+2*(i-1)+1] = ymx_;
    z2_[nn+2*(i-1)+1] = zmn_;
    p2_[nn+2*(i-1)+1] = 2;
  }
  nn += 2*nlxinc_+2;
  for( std::size_t i=0; i<npt; ++i )   // normalize with respect to the centre of the object
  {
    x_[i] -= xc_;
    y_[i] -= yc_;
    z_[i] -= zc_;
  }
  for( int i=0; i<6; ++i )
  {
    xn_[i] -= xc_;
    yn_[i] -= yc_;
    zn_[i] -= zc_;
  }
  for( int i=0; i<nxyz; ++i )
  {
    x2_[i] -= xc_;
    y2_[i] -= yc_;
    z2_[i] -= zc_;
  }
  double sinth = sin(M_PI/180.*xAxisAngle_);
  double costh = cos(M_PI/180.*xAxisAngle_);
  for( std::size_t i=0; i<npt; ++i )
  {
    double yi = y_[i];
    double zi = z_[i];
    y_[i] =  yi*costh + zi*sinth;
    z_[i] = -yi*sinth + zi*costh;
  }
  for( int i=0; i<6; ++i )
  {
    double yi = yn_[i];
    double zi = zn_[i];
    yn_[i] =  yi*costh + zi*sinth;
    zn_[i] = -yi*sinth + zi*costh;
  }
  for( int i=0; i<nxyz; ++i )
  {
    double yi = y2_[i];
    double zi = z2_[i];
    y2_[i] =  yi*costh + zi*sinth;
    z2_[i] = -yi*sinth + zi*costh;
  }
  sinth = sin(M_PI/180.*yAxisAngle_);
  costh = cos(M_PI/180.*yAxisAngle_);
  for( std::size_t i=0; i<npt; ++i )
  {
    double xi = x_[i];
    double zi = z_[i];
    x_[i] = xi*costh - zi*sinth;
    z_[i] = xi*sinth + zi*costh;
  }
  for( int i=0; i<6; ++i )
  {
    double xi = xn_[i];
    double zi = zn_[i];
    xn_[i] = xi*costh - zi*sinth;
    zn_[i] = xi*sinth + zi*costh;
  }
  for( int i=0; i<nxyz; ++i )
  {
    double xi = x2_[i];
    double zi = z2_[i];
    x2_[i] = xi*costh - zi*sinth;
    z2_[i] = xi*sinth + zi*costh;
  }
}

void GRA_threeDPlot::SetupPlotSymbols()
{
  std::size_t npt = x_.size();
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *curveC = gw->GetDataCurveCharacteristics();
  GRA_intCharacteristic *psCharacteristic =
      static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("PLOTSYMBOL")));
  std::vector<int> symbols;
  if( psCharacteristic->IsVector() )
  {
    symbols.assign( psCharacteristic->Gets().begin(), psCharacteristic->Gets().end() );
    for( std::size_t i=symbols.size(); i<npt; ++i )symbols.push_back(11);
  }
  else
  {
    symbols.assign( npt, psCharacteristic->Get() );
  }
  GRA_sizeCharacteristic *pssizeCharacteristic =
      static_cast<GRA_sizeCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLSIZE")));
  std::vector<double> sizes;
  if( pssizeCharacteristic->IsVector() )
  {
    sizes.assign( pssizeCharacteristic->GetAsWorlds().begin(),
                  pssizeCharacteristic->GetAsWorlds().end() );
    for( std::size_t i=sizes.size(); i<npt; ++i )sizes.push_back(sizes.back());
  }
  else
  {
    sizes.assign( npt, pssizeCharacteristic->GetAsWorld() );
  }
  GRA_angleCharacteristic *psangleCharacteristic =
      static_cast<GRA_angleCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLANGLE")));
  std::vector<double> angles;
  if( psangleCharacteristic->IsVector() )
  {
    angles.assign( psangleCharacteristic->Gets().begin(),
                   psangleCharacteristic->Gets().end() );
    for( std::size_t i=angles.size(); i<npt; ++i )angles.push_back(angles.back());
  }
  else
  {
    angles.assign( npt, psangleCharacteristic->Get() );
  }
  GRA_colorCharacteristic *pscolorCharacteristic =
      static_cast<GRA_colorCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLCOLOR")));
  std::vector<GRA_color *> colors;
  if( pscolorCharacteristic->IsVector() )
  {
    colors.assign( pscolorCharacteristic->Gets().begin(),
                   pscolorCharacteristic->Gets().end() );
    for( std::size_t i=colors.size(); i<npt; ++i )colors.push_back(colors.back());
  }
  else
  {
    colors.assign( npt, pscolorCharacteristic->Get() );
  }
  GRA_intCharacteristic *pslwCharacteristic =
      static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLLINEWIDTH")));
  std::vector<int> lws;
  if( pslwCharacteristic->IsVector() )
  {
    lws.assign( pslwCharacteristic->Gets().begin(),
                pslwCharacteristic->Gets().end() );
    for( std::size_t i=lws.size(); i<npt; ++i )lws.push_back(lws.back());
  }
  else
  {
    lws.assign( npt, pslwCharacteristic->Get() );
  }
  for( std::size_t i=0; i<npt; ++i )plotSymbols_.push_back(
    new GRA_plotSymbol(symbols[i],sizes[i],angles[i],colors[i],lws[i]) );
}

void GRA_threeDPlot::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  DrawCurve( graphicsOutput, dc );
  DrawSymbols( graphicsOutput, dc );
  DrawAxes( graphicsOutput, dc );
}

void GRA_threeDPlot::ScaleXY( double const x, double const y, double const z,
                              double &scx, double &scy )
{
  double xminW, yminW, xmaxW, ymaxW;
  ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
  scx = ((xmaxW-xminW)*ds_/screenHalfSize_*x/(eye2object_+z)+(xminW+xmaxW)*screenHalfSize_)/
      (2*screenHalfSize_);
  scy = ((ymaxW-yminW)*ds_/screenHalfSize_*y/(eye2object_+z)+(yminW+ymaxW)*screenHalfSize_)/
      (2*screenHalfSize_);
}

void GRA_threeDPlot::DrawCurve( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *curveC = gw->GetDataCurveCharacteristics();
  
  int lineTypeSave = graphicsOutput->GetLineType();
  graphicsOutput->SetLineType(
    static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("CURVELINETYPE")))->Get() );
  wxPen wxpen( dc.GetPen() );
  wxpen.SetColour( ExGlobals::GetwxColor(
                   static_cast<GRA_colorCharacteristic*>(curveC->Get(wxT("CURVECOLOR")))->Get() ));
  wxpen.SetWidth( static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("CURVELINEWIDTH")))->Get() );
  dc.SetPen( wxpen );

  double xs, ys;
  ScaleXY( x_[0], y_[0], z_[0], xs, ys );
  graphicsOutput->StartLine( xs, ys );
  std::size_t npt = x_.size();
  for( std::size_t i=1; i<npt; ++i )
  {
    ScaleXY( x_[i], y_[i], z_[i], xs, ys );
    plotSymbols_[i]->GetConnectToPrevious() ? graphicsOutput->ContinueLine( xs, ys, dc ) :
                                              graphicsOutput->StartLine( xs, ys );
  }
  wxpen.SetColour( ExGlobals::GetwxColor( GRA_colorControl::GetColor(wxT("BLACK")) ) );
  dc.SetPen( wxpen );
}

void GRA_threeDPlot::DrawSymbols( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  std::size_t npt = x_.size();
  for( std::size_t i=0; i<npt; ++i )
  {
    double xs, ys;
    ScaleXY( x_[i], y_[i], z_[i], xs, ys );
    plotSymbols_[i]->SetLocation( xs, ys );
    plotSymbols_[i]->Draw( graphicsOutput, dc );
  }
}

void GRA_threeDPlot::DrawAxes( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *textC = gw->GetTextCharacteristics();
  GRA_setOfCharacteristics textCsave( *textC );
  static_cast<GRA_boolCharacteristic*>(textC->Get(wxT("INTERACTIVE")))->Set( false );
  int nxyz = 2*(nlxinc_+nlyinc_+nlzinc_) + 12;
  for( int i=0; i<nxyz; ++i )
  {
    double xs, ys;
    ScaleXY( x2_[i], y2_[i], z2_[i], xs, ys );
    p2_[i] == 2 ? graphicsOutput->ContinueLine( xs, ys, dc ) : graphicsOutput->StartLine( xs, ys );
  }
  double xs1, ys1;
  ScaleXY( xn_[0], yn_[0], zn_[0], xs1, ys1 );
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 6 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsWorld( xs1 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsWorld( ys1 );
  GRA_drawableText *dt = new GRA_drawableText( wxString()<<zmax_ );
  dt->Parse();
  dt->Draw( graphicsOutput, dc );

  double xs2, ys2;
  ScaleXY( xn_[1], yn_[1], zn_[1], xs2, ys2 );
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 6 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsWorld( xs2 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsWorld( ys2 );
  dt = new GRA_drawableText( wxString()<<zmin_ );
  dt->Parse();
  dt->Draw( graphicsOutput, dc );

  double xc = (xs1+xs2)/2.;
  double yc = (ys1+ys2)/2.;
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 6 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsWorld( xc );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsWorld( yc );
  dt = new GRA_drawableText( wxT("Z") );
  dt->Parse();
  dt->Draw( graphicsOutput, dc );

  ScaleXY( xn_[2], yn_[2], zn_[2], xs1, ys1 );
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 6 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsWorld( xs1 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsWorld( ys1 );
  dt = new GRA_drawableText( wxString()<<xmin_ );
  dt->Parse();
  dt->Draw( graphicsOutput, dc );

  ScaleXY( xn_[3], yn_[3], zn_[3], xs2, ys2 );
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 6 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsWorld( xs2 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsWorld( ys2 );
  dt = new GRA_drawableText( wxString()<<xmax_ );
  dt->Parse();
  dt->Draw( graphicsOutput, dc );

  xc = (xs1+xs2)/2.;
  yc = (ys1+ys2)/2.;
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 6 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsWorld( xc );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsWorld( yc );
  dt = new GRA_drawableText( wxT("X") );
  dt->Parse();
  dt->Draw( graphicsOutput, dc );

  ScaleXY( xn_[4], yn_[4], zn_[4], xs1, ys1 );
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 4 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsWorld( xs1 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsWorld( ys1 );
  dt = new GRA_drawableText( wxString()<<ymin_ );
  dt->Parse();
  dt->Draw( graphicsOutput, dc );

  ScaleXY( xn_[5], yn_[5], zn_[5], xs2, ys2 );
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 4 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsWorld( xs2 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsWorld( ys2 );
  dt = new GRA_drawableText( wxString()<<ymax_ );
  dt->Parse();
  dt->Draw( graphicsOutput, dc );

  xc = (xs1+xs2)/2.;
  yc = (ys1+ys2)/2.;
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 4 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsWorld( xc );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsWorld( yc );
  dt = new GRA_drawableText( wxT("Y") );
  dt->Parse();
  dt->Draw( graphicsOutput, dc );

  *textC = textCsave;
}

void GRA_threeDPlot::RotateX( double const angle )
{
  double sinth = sin(M_PI/180.*angle);
  double costh = cos(M_PI/180.*angle);
  std::size_t npt = x_.size();
  for( std::size_t i=0; i<npt; ++i )
  {
    double yi = y_[i];
    double zi = z_[i];
    y_[i] = yi*costh + zi*sinth;
    z_[i] = -yi*sinth + zi*costh;
  }
  for( int i=0; i<6; ++i )
  {
    double yi = yn_[i];
    double zi = zn_[i];
    yn_[i] = yi*costh + zi*sinth;
    zn_[i] = -yi*sinth + zi*costh;
  }
  int nxyz = 2*(nlxinc_+nlyinc_+nlzinc_) + 12;
  for( int i=0; i<nxyz; ++i )
  {
    double yi = y2_[i];
    double zi = z2_[i];
    y2_[i] = yi*costh + zi*sinth;
    z2_[i] = -yi*sinth + zi*costh;
  }
}

void GRA_threeDPlot::RotateY( double const angle )
{
  double sinth = sin(M_PI/180.*angle);
  double costh = cos(M_PI/180.*angle);
  std::size_t npt = x_.size();
  for( std::size_t i=0; i<npt; ++i )
  {
    double xi = x_[i];
    double zi = z_[i];
    x_[i] = xi*costh - zi*sinth;
    z_[i] = xi*sinth + zi*costh;
  }
  for( int i=0; i<6; ++i )
  {
    double xi = xn_[i];
    double zi = zn_[i];
    xn_[i] = xi*costh - zi*sinth;
    zn_[i] = xi*sinth + zi*costh;
  }
  int nxyz = 2*(nlxinc_+nlyinc_+nlzinc_) + 12;
  for( int i=0; i<nxyz; ++i )
  {
    double xi = x2_[i];
    double zi = z2_[i];
    x2_[i] = xi*costh - zi*sinth;
    z2_[i] = xi*sinth + zi*costh;
  }
}

void GRA_threeDPlot::ScaleDS( double const c )
{
  ds_ *= c;
}

void GRA_threeDPlot::SetEye2Object( double const e )
{
  eye2object_ = e;
}
