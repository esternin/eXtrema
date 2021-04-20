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
#include "ProjectHeader.h"
#pragma hdrstop

#include "GRA_contourLine.h"
#include "ExGlobals.h"
#include "EGraphicsError.h"
#include "UsefulFunctions.h"
#include "GRA_outputType.h"
#include "GRA_color.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_window.h"
#include "GRA_drawableText.h"
#include "GRA_colorControl.h"
#include "GRA_fontControl.h"

void GRA_contourLine::CopyStuff( GRA_contourLine const &rhs )
{
  lineType_ = rhs.lineType_;
  lineWidth_ = rhs.lineWidth_;
  color_ = rhs.color_;
  //
  level_ = rhs.level_;
  xData_.assign( rhs.xData_.begin(), rhs.xData_.end() );
  yData_.assign( rhs.yData_.begin(), rhs.yData_.end() );
  xCurve_.clear();
  yCurve_.clear();
  for( std::size_t i=0; i<rhs.xCurve_.size(); ++i )
  {
    xCurve_.push_back( std::vector<double> (rhs.xCurve_[i].begin(), rhs.xCurve_[i].end()) );
    yCurve_.push_back( std::vector<double> (rhs.yCurve_[i].begin(), rhs.yCurve_[i].end()) );
  }
}

void GRA_contourLine::Make()
{
  /*
  for( std::size_t i=0; i<xData_.size(); i+=2 )
  {
    pen_.push_back(3);
    pen_.push_back(2);
  }
  ExGlobals::GetGraphWindow()->GraphToWorld( xData_, yData_, xCurve_, yCurve_ );
  */
  GRA_window *gw = ExGlobals::GetGraphWindow();
  int const xlog = static_cast<int>(
   static_cast<GRA_doubleCharacteristic*>(gw->GetXAxisCharacteristics()->Get("LOGBASE"))->Get());
  int const ylog = static_cast<int>(
    static_cast<GRA_doubleCharacteristic*>(gw->GetYAxisCharacteristics()->Get("LOGBASE"))->Get());
  //
  // turn the line segments into a polyline
  //
  std::vector<double> x, y;
  x.assign( xData_.begin(), xData_.end() );
  y.assign( yData_.begin(), yData_.end() );
  std::vector<double> xpoly, ypoly;
  std::vector<double>::iterator ix, iy;
  while( !x.empty() )
  {
    ix = x.begin();
    iy = y.begin();
    xpoly.push_back( *ix );
    ypoly.push_back( *iy );
    xpoly.push_back( *(ix+1) );
    ypoly.push_back( *(iy+1) );
    x.erase( ix, ix+2 );
    y.erase( iy, iy+2 );
    ix = x.begin();
    iy = y.begin();
    while( ix != x.end() )
    {
      if( *ix==xpoly.back() && *iy==ypoly.back() )
      {
        xpoly.push_back( *(ix+1) );
        ypoly.push_back( *(iy+1) );
        x.erase( ix, ix+2 );
        y.erase( iy, iy+2 );
        ix = x.begin();
        iy = y.begin();
      }
      else if( *(ix+1)==xpoly.back() && *(iy+1)==ypoly.back() )
      {
        xpoly.push_back( *ix );
        ypoly.push_back( *iy );
        x.erase( ix, ix+2 );
        y.erase( iy, iy+2 );
        ix = x.begin();
        iy = y.begin();
      }
      else
      {
        ix += 2;
        iy += 2;
      }
    }
    std::size_t size = xpoly.size();
    if( xlog>1 || ylog>1 )
    {
      for( std::size_t i=0; i<size; ++i )
      {
        if( ylog>1 && ypoly[i]<=0.0 )ypoly[i] = std::numeric_limits<double>::min();
        if( xlog>1 && xpoly[i]<=0.0 )xpoly[i] = std::numeric_limits<double>::min();
      }
    }
    std::vector<double> xc( size ), yc( size );
    gw->GraphToWorld( xpoly, ypoly, xc, yc );
    xCurve_.push_back( xc );
    yCurve_.push_back( yc );
    xpoly.clear();
    ypoly.clear();
  }
}

void GRA_contourLine::Draw( GRA_outputType *graphicsOutput )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  double xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->GetAsWorld();
  double xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("LOWERAXIS"))->GetAsWorld();
  double yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("UPPERAXIS"))->GetAsWorld();
  double ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("LOWERAXIS"))->GetAsWorld();
  ExGlobals::SetClippingBoundary( xlaxis, ylaxis, xuaxis, yuaxis );
  //
  // draw the contour line
  //
  int lineTypeSave = graphicsOutput->GetLineType();
  int lineWidthSave = graphicsOutput->GetLineWidth();
  GRA_color *colorSave = graphicsOutput->GetColor();
  graphicsOutput->SetLineType( lineType_ );
  graphicsOutput->SetLineWidth( lineWidth_ );
  graphicsOutput->SetColor( color_ );
  double labelHeight =
    static_cast<GRA_sizeCharacteristic*>(generalC->Get("CONTOURLABELHEIGHT"))->GetAsWorld();
  double labelAngle = 0.0;
  int labelAlignment = 5;
  GRA_font *labelFont = GRA_fontControl::GetFont("ARIAL");
  GRA_color *labelColor = color_;
  std::size_t size = xCurve_.size();
  double const pp[5] = {0.2,0.333,0.5,0.667,0.8};
  std::size_t ix = random(5);
  for( std::size_t i=0; i<size; ++i )
  {
    std::size_t np = xCurve_[i].size();
    graphicsOutput->StartLine( xCurve_[i][0], yCurve_[i][0] );
    for( std::size_t j=1; j<np; ++j )
      graphicsOutput->ContinueLine( xCurve_[i][j], yCurve_[i][j] );
    if( np > 10 )
    {
      ++ix;
      double xloc = xCurve_[i][int(pp[ix%5]*np)];
      double yloc = yCurve_[i][int(pp[ix%5]*np)];
      GRA_drawableText dt( ExString(level_), labelHeight, labelAngle, xloc, yloc,
                           labelAlignment, labelFont, labelColor );
      dt.Parse();
      graphicsOutput->Draw( &dt );
    }
  }
  graphicsOutput->SetLineType( lineTypeSave );
  graphicsOutput->SetColor( colorSave );
  graphicsOutput->SetLineWidth( lineWidthSave );
  ExGlobals::ResetClippingBoundary();
}

std::ostream &operator<<( std::ostream &out, GRA_contourLine const &cc )
{
  out << "<contourline linetype=\"" << cc.lineType_ << "\" linewidth=\"" << cc.lineWidth_
      << "\" color=\"" << GRA_colorControl::GetColorCode(cc.color_)
      << "\" level=\"" << cc.level_ << ">\n";
  std::size_t size = cc.xData_.size();
  out << "<data size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )out << cc.xData_[i] << " " << cc.yData_[i] << " ";
  out << "</data>\n";
  size = cc.xCurve_.size();
  out << "<xycurve size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )
  {
    std::size_t np = cc.xCurve_[i].size();
    out << "<segment size=\"" << np << "\">";
    for( std::size_t j=0; j<np; ++j )
      out << cc.xCurve_[i][j] << " " << cc.yCurve_[i][j] << " ";
    out << "</segment>\n";
  }
  out << "</xycurve>\n";
  return out << "</contourline>\n";
}

void GRA_contourLine::SetValues( int linetype, int linewidth,
                                 GRA_color* linecolor, double level,
                                 std::vector< std::vector<double> > &xcurve,
                                 std::vector< std::vector<double> > &ycurve )
{
  lineType_ = linetype;
  lineWidth_ = linewidth;
  color_ = linecolor;
  level_ = level;
  xCurve_.assign( xcurve.begin(), xcurve.end() );
  yCurve_.assign( ycurve.begin(), ycurve.end() );
}

// end of file
