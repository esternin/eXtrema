/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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

#include "GRA_polarCurve.h"
#include "ExGlobals.h"
#include "EGraphicsError.h"
#include "UsefulFunctions.h"
#include "GRA_wxWidgets.h"
#include "GRA_polyline.h"
#include "GRA_plotSymbol.h"
#include "GRA_polygon.h"
#include "GRA_errorBar.h"
#include "GRA_color.h"
#include "GRA_intCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_window.h"
#include "GRA_colorControl.h"

GRA_polarCurve::~GRA_polarCurve()
{
  DeleteStuff();
  if( popup_ )ExGlobals::DisconnectCurvePopup();
}

void GRA_polarCurve::SetUp()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *curveC = gw->GetDataCurveCharacteristics();
  //
  std::size_t rSize = r_.size();
  //
  GRA_intCharacteristic *psCharacteristic =
    static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("PLOTSYMBOL")));
  std::vector<int> symbols;
  if( psCharacteristic->IsVector() )
  {
    symbols.assign( psCharacteristic->Gets().begin(), psCharacteristic->Gets().end() );
    std::size_t psSize = symbols.size();
    for( std::size_t i=psSize; i<rSize; ++i )symbols.push_back(11);
  }
  else
  {
    int s = psCharacteristic->Get();
    symbols.assign( rSize, s );
  }
  GRA_sizeCharacteristic *pssizeCharacteristic =
    static_cast<GRA_sizeCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLSIZE")));
  std::vector<double> sizes;
  if( pssizeCharacteristic->IsVector() )
  {
    sizes = pssizeCharacteristic->GetAsWorlds();
    std::size_t pssSize = sizes.size();
    for( std::size_t i=pssSize; i<rSize; ++i )sizes.push_back(sizes[pssSize-1]);
  }
  else sizes.assign( rSize, pssizeCharacteristic->GetAsWorld() );
  GRA_angleCharacteristic *psangleCharacteristic =
    static_cast<GRA_angleCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLANGLE")));
  std::vector<double> angles;
  if( psangleCharacteristic->IsVector() )
  {
    angles.assign( psangleCharacteristic->Gets().begin(), psangleCharacteristic->Gets().end() );
    std::size_t psaSize = angles.size();
    for( std::size_t i=psaSize; i<rSize; ++i )angles.push_back(angles[psaSize-1]);
  }
  else angles.assign( rSize, psangleCharacteristic->Get() );
  GRA_colorCharacteristic *pscolorCharacteristic =
    static_cast<GRA_colorCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLCOLOR")));
  std::vector<GRA_color*> colors;
  if( pscolorCharacteristic->IsVector() )
  {
    colors.assign( pscolorCharacteristic->Gets().begin(), pscolorCharacteristic->Gets().end() );
    std::size_t pscSize = colors.size();
    for( std::size_t i=pscSize; i<rSize; ++i )colors.push_back(colors[pscSize-1]);
  }
  else colors.assign( rSize, pscolorCharacteristic->Get() );
  GRA_intCharacteristic *pslwCharacteristic =
    static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("PLOTSYMBOLLINEWIDTH")));
  std::vector<int> lws;
  if( pslwCharacteristic->IsVector() )
  {
    lws.assign( pslwCharacteristic->Gets().begin(), pslwCharacteristic->Gets().end() );
    std::size_t pslwSize = lws.size();
    for( std::size_t i=pslwSize; i<rSize; ++i )lws.push_back(lws[pslwSize-1]);
  }
  else lws.assign( rSize, pslwCharacteristic->Get() );
  //
  for( std::size_t i=0; i<rSize; ++i )plotsymbols_.push_back(
    new GRA_plotSymbol(symbols[i],sizes[i],angles[i],colors[i],lws[i]) );
  //
  histogramType_ = 0;
  // histogramType_ = static_cast<GRA_intCharacteristic*>(curveC->Get(wxT("HISTOGRAMTYPE")))->Get();
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
    areaFillColors_.assign( areaFillColorCharacteristic->Gets().begin(),
                            areaFillColorCharacteristic->Gets().end() );
    std::size_t afcSize = areaFillColors_.size();
    for( std::size_t i=afcSize; i<rSize; ++i )areaFillColors_.push_back(areaFillColors_[afcSize-1]);
  }
  else areaFillColor_ = areaFillColorCharacteristic->Get();
  //
  GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
  xmax_ = static_cast<GRA_doubleCharacteristic*>(polarC->Get(wxT("MAX")))->Get();
}

void GRA_polarCurve::DeleteStuff()
{
  while( !plotsymbols_.empty() )
  {
    delete plotsymbols_.back();
    plotsymbols_.pop_back();
  }
}

void GRA_polarCurve::CopyStuff( GRA_polarCurve const &rhs )
{
  histogramType_ = rhs.histogramType_;
  lineType_ = rhs.lineType_;
  lineWidth_ = rhs.lineWidth_;
  color_ = rhs.color_;
  areaFillColor_ = rhs.areaFillColor_;
  areaFillColors_.assign( rhs.areaFillColors_.begin(), rhs.areaFillColors_.end() );
  //
  r_.assign( rhs.r_.begin(), rhs.r_.end() );
  theta_.assign( rhs.theta_.begin(), rhs.theta_.end() );
  xCurve_.assign( rhs.xCurve_.begin(), rhs.xCurve_.end() );
  yCurve_.assign( rhs.yCurve_.begin(), rhs.yCurve_.end() );
  pen_.assign( rhs.pen_.begin(), rhs.pen_.end() );
  std::vector<GRA_plotSymbol*>::const_iterator sEnd = rhs.plotsymbols_.end();
  for( std::vector<GRA_plotSymbol*>::const_iterator i=rhs.plotsymbols_.begin(); i!=sEnd; ++i )
    plotsymbols_.push_back( new GRA_plotSymbol(**i) );
  //
  xmax_ = rhs.xmax_;
  popup_ = rhs.popup_;
}

void GRA_polarCurve::Make()
{
  MakeNonHistogram();
}

void GRA_polarCurve::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  DrawNonHistogram( graphicsOutput, dc );
}

bool GRA_polarCurve::Inside( double x, double y )
{
  std::vector<double> xp(4,0.0), yp(4,0.0);
  double const eps = 0.05;
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

void GRA_polarCurve::MakeNonHistogram()
{
  std::size_t size = r_.size();
  pen_.assign( size, 3 );
  for( std::size_t i=0; i<size; ++i )
  {
    if( plotsymbols_[i]->GetConnectToPrevious() )pen_[i] = 2;
  }
  pen_[0] = 3;
  ExGlobals::GetGraphWindow()->PolarToWorld( r_, theta_, xCurve_, yCurve_ );
}

void GRA_polarCurve::DrawNonHistogram( GRA_wxWidgets *graphicsOutput, wxDC &dc )
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
  // draw the data curve itself
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

std::ostream &operator<<( std::ostream &out, GRA_polarCurve const &cc )
{
  out << "<polarcurve histype=\"" << cc.histogramType_ << "\" linetype=\""
      << cc.lineType_ << "\" linewidth=\"" << cc.lineWidth_ << "\" linecolor=\""
      << GRA_colorControl::GetColorCode(cc.color_) << "\" areafillcolor=\""
      << GRA_colorControl::GetColorCode(cc.areaFillColor_) << "\" xmax=\""
      << cc.xmax_ << "\">\n";
  std::size_t size = cc.areaFillColors_.size();
  out << "<areafillcolors size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << GRA_colorControl::GetColorCode(cc.areaFillColors_[i]) << " ";
  out << "</areafillcolors>\n";
  size = cc.r_.size();
  out << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << cc.r_[i] << " " << cc.theta_[i] << " ";
  out << "</data>\n";
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
  return out << "</polarcurve>\n";
}

void GRA_polarCurve::SetValues( int histype, int linetype, int linewidth, double xmax,
                                GRA_color* linecolor, GRA_color *areafillcolor,
                                std::vector<GRA_color*> &areaFillColors,
                                std::vector<double> &xcurve, std::vector<double> &ycurve,
                                std::vector<int> &pen, std::vector<GRA_plotSymbol*> &symbols )
{
  histogramType_ = histype;
  lineType_ = linetype;
  lineWidth_ = linewidth;
  xmax_ = xmax;
  color_ = linecolor;
  areaFillColor_ = areafillcolor;
  areaFillColors_.assign( areaFillColors.begin(), areaFillColors.end() );
  xCurve_.assign( xcurve.begin(), xcurve.end() );
  yCurve_.assign( ycurve.begin(), ycurve.end() );
  pen_.assign( pen.begin(), pen.end() );
  plotsymbols_.assign( symbols.begin(), symbols.end() );
}

int GRA_polarCurve::GetPlotsymbolCode() const
{ return plotsymbols_[0]->GetShapeCode(); }

void GRA_polarCurve::SetPlotsymbolCode( int ps )
{
  int size = plotsymbols_.size();
  for( int i=0; i<size; ++i )plotsymbols_[i]->SetShapeCode( ps );
}

GRA_color *GRA_polarCurve::GetPlotsymbolColor() const
{ return plotsymbols_[0]->GetColor(); }

void GRA_polarCurve::SetPlotsymbolColor( GRA_color *c )
{
  int size = plotsymbols_.size();
  for( int i=0; i<size; ++i )plotsymbols_[i]->SetColor( c );
}

double GRA_polarCurve::GetPlotsymbolSize() const
{ return plotsymbols_[0]->GetSize(); }

void GRA_polarCurve::SetPlotsymbolSize( double s )
{
  int size = plotsymbols_.size();
  for( int i=0; i<size; ++i )plotsymbols_[i]->SetSize( s );
}

double GRA_polarCurve::GetPlotsymbolAngle() const
{ return plotsymbols_[0]->GetAngle(); }

void GRA_polarCurve::SetPlotsymbolAngle( double a )
{
  int size = plotsymbols_.size();
  for( int i=0; i<size; ++i )plotsymbols_[i]->SetAngle( a );
}

// end of file
