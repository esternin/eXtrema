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
#include <cmath>
#include <algorithm>

#include "wx/wx.h"

#include "GRA_window.h"
#include "EGraphicsError.h"
#include "GRA_wxWidgets.h"
#include "GRA_drawableObject.h"
#include "GRA_drawableText.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_cartesianCurve.h"
#include "GRA_cartesianAxes.h"
#include "GRA_point.h"
#include "GRA_polyline.h"
#include "GRA_polygon.h"
#include "GRA_ellipse.h"
#include "GRA_plotSymbol.h"
#include "GRA_axis.h"
#include "GRA_legend.h"
#include "GRA_contour.h"
#include "GRA_multiLineFigure.h"
#include "GRA_boxPlot.h"
#include "GRA_ditheringPlot.h"
#include "GRA_diffusionPlot.h"
#include "GRA_gradientPlot.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"

void GRA_window::SetUp()
{
  double xminw, yminw, xmaxw, ymaxw;
  ExGlobals::GetWorldLimits( xminw, yminw, xmaxw, ymaxw );
  //
  double xl = xminw+xLoP_*(xmaxw-xminw)/100.0;
  double yl = yminw+yLoP_*(ymaxw-yminw)/100.0;
  double xu = xminw+xHiP_*(xmaxw-xminw)/100.0;
  double yu = yminw+yHiP_*(ymaxw-yminw)/100.0;
  //
  CreateGeneralCharacteristics( xl, yl, xu, yu );
  CreateXAxisCharacteristics( xl, yl, xu, yu );
  CreateYAxisCharacteristics( xl, yl, xu, yu );
  CreateDataCurveCharacteristics( yl, yu );
  CreateGraphLegendCharacteristics( xl, yl, xu, yu );
  CreateTextCharacteristics( xl, yl, xu, yu );
  //
  xPrevious_ = yPrevious_ = 0.0;
}

void GRA_window::DeleteCharacteristics()
{
  delete xAxisCharacteristics_;
  delete yAxisCharacteristics_;
  delete generalCharacteristics_;
  delete textCharacteristics_;
  delete graphLegendCharacteristics_;
  delete dataCurveCharacteristics_;
  xAxisCharacteristics_ = 0;
  yAxisCharacteristics_ = 0;
  generalCharacteristics_ = 0;
  textCharacteristics_ = 0;
  graphLegendCharacteristics_ = 0;
  dataCurveCharacteristics_ = 0;
}

GRA_setOfCharacteristics *GRA_window::GetXAxisCharacteristics()
{ return xAxisCharacteristics_; }

GRA_setOfCharacteristics *GRA_window::GetYAxisCharacteristics()
{ return yAxisCharacteristics_; }

GRA_setOfCharacteristics *GRA_window::GetGeneralCharacteristics()
{ return generalCharacteristics_; }

GRA_setOfCharacteristics *GRA_window::GetTextCharacteristics()
{ return textCharacteristics_; }

GRA_setOfCharacteristics *GRA_window::GetGraphLegendCharacteristics()
{ return graphLegendCharacteristics_; }

GRA_setOfCharacteristics *GRA_window::GetDataCurveCharacteristics()
{ return dataCurveCharacteristics_; }

void GRA_window::SetXAxisCharacteristics( GRA_setOfCharacteristics const &c )
{ *xAxisCharacteristics_ = c; }

void GRA_window::SetYAxisCharacteristics( GRA_setOfCharacteristics const &c )
{ *yAxisCharacteristics_ = c; }

void GRA_window::CopyStuff( GRA_window const &rhs )
{
  xLoP_ = rhs.xLoP_;
  yLoP_ = rhs.yLoP_;
  xHiP_ = rhs.xHiP_;
  yHiP_ = rhs.yHiP_;
  xPrevious_ = rhs.xPrevious_;
  yPrevious_ = rhs.yPrevious_;
  Clear();
  drawableVecIter end = rhs.drawableObjects_.end();
  GRA_drawableObject *p = 0;
  for( drawableVecIter i=rhs.drawableObjects_.begin(); i!=end; ++i )
  {
    if( (*i)->IsaPoint() )
      p = new GRA_point(*(static_cast<GRA_point*>(*i)));
    else if( (*i)->IsaPolyline() )
      p = new GRA_polyline(*(static_cast<GRA_polyline*>(*i)));
    else if( (*i)->IsaPlotsymbol() )
      p = new GRA_plotSymbol(*(static_cast<GRA_plotSymbol*>(*i)));
    else if( (*i)->IsaDrawableText() )
      p = new GRA_drawableText(*(static_cast<GRA_drawableText*>(*i)));
    else if( (*i)->IsaAxis() )
      p = new GRA_axis(*(static_cast<GRA_axis*>(*i)));
    else if( (*i)->IsaCartesianAxes() )
      p = new GRA_cartesianAxes(*(static_cast<GRA_cartesianAxes*>(*i)));
    else if( (*i)->IsaCartesianCurve() )
      p = new GRA_cartesianCurve(*(static_cast<GRA_cartesianCurve*>(*i)));
    else if( (*i)->IsaGraphLegend() )
      p = new GRA_legend(*(static_cast<GRA_legend*>(*i)));
    else if( (*i)->IsaContour() )
      p = new GRA_contour(*(static_cast<GRA_contour*>(*i)));
    drawableObjects_.push_back( p );
  }
  *xAxisCharacteristics_ = *rhs.xAxisCharacteristics_;
  *yAxisCharacteristics_ = *rhs.yAxisCharacteristics_;
  *textCharacteristics_ = *rhs.textCharacteristics_;
  *generalCharacteristics_ = *rhs.generalCharacteristics_;
  *graphLegendCharacteristics_ = *rhs.graphLegendCharacteristics_;
  *dataCurveCharacteristics_ = *rhs.dataCurveCharacteristics_;
}

void GRA_window::Reset()
{
  Clear();
  DeleteCharacteristics();
  SetUp();
}

void GRA_window::RemoveDrawableObject( GRA_drawableObject *object )
{
  std::vector<GRA_drawableObject*>::iterator i = find( drawableObjects_.begin(), drawableObjects_.end(), object );
  if( i != drawableObjects_.end() )drawableObjects_.erase( i );
}

GRA_legend *GRA_window::GetGraphLegend()
{
  drawableVecIter end = drawableObjects_.end();
  GRA_legend *legend = 0;
  for( drawableVecIter i=drawableObjects_.begin(); i!=end; ++i )
  {
    if( (*i)->IsaGraphLegend() )
    {
      legend = static_cast<GRA_legend*>(*i);
      break;
    }
  }
  if( !legend )
  {
    legend = new GRA_legend( this );
    AddDrawableObject( legend );
  }
  return legend;
}

void GRA_window::InheritFrom( GRA_window const *w )
{
  static_cast<GRA_stringCharacteristic*>(generalCharacteristics_->Get(wxT("AUTOSCALE")))->Set(
   static_cast<GRA_stringCharacteristic*>(w->generalCharacteristics_->Get(wxT("AUTOSCALE")))->Get() );
  static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get(wxT("AREAFILLCOLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->generalCharacteristics_->Get(wxT("AREAFILLCOLOR")))->Get() );
  static_cast<GRA_boolCharacteristic*>(generalCharacteristics_->Get(wxT("GRAPHBOX")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->generalCharacteristics_->Get(wxT("GRAPHBOX")))->Get() );
  static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get(wxT("COLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->generalCharacteristics_->Get(wxT("COLOR")))->Get() );
  static_cast<GRA_fontCharacteristic*>(generalCharacteristics_->Get(wxT("FONT")))->Set(
   static_cast<GRA_fontCharacteristic*>(w->generalCharacteristics_->Get(wxT("FONT")))->Get() );
  static_cast<GRA_intCharacteristic*>(generalCharacteristics_->Get(wxT("LINEWIDTH")))->Set(
   static_cast<GRA_intCharacteristic*>(w->generalCharacteristics_->Get(wxT("LINEWIDTH")))->Get() );
  static_cast<GRA_intCharacteristic*>(generalCharacteristics_->Get(wxT("GRIDLINETYPE")))->Set(
   static_cast<GRA_intCharacteristic*>(w->generalCharacteristics_->Get(wxT("GRIDLINETYPE")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get(wxT("LEGENDSIZE")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->generalCharacteristics_->Get(wxT("LEGENDSIZE")))->GetAsPercent() );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get(wxT("CONTOURLABELHEIGHT")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->generalCharacteristics_->Get(wxT("CONTOURLABELHEIGHT")))->GetAsPercent() );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get(wxT("CONTOURLABELSEPARATION")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->generalCharacteristics_->Get(wxT("CONTOURLABELSEPARATION")))->GetAsPercent() );
  //
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("AXISON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("AXISON")))->Get() );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get(wxT("AXISCOLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("AXISCOLOR")))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("LINEWIDTH")))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LINEWIDTH")))->Get() );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get(wxT("AXISANGLE")))->Set(
   static_cast<GRA_angleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("AXISANGLE")))->Get() );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOWERAXIS")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LOWERAXIS")))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("UPPERAXIS")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("UPPERAXIS")))->GetAsPercent() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MIN")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("MIN")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MAX")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("MAX")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("VIRTUALMIN")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("VIRTUALMIN")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("VIRTUALMAX")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("VIRTUALMAX")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOGBASE")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LOGBASE")))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOGSTYLE")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LOGSTYLE")))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NLINCS")))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("NLINCS")))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NSINCS")))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("NSINCS")))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("GRID")))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("GRID")))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("FORCECROSS")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("FORCECROSS")))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("ZERO")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("ZERO")))->Get() );
  static_cast<GRA_stringCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABEL")))->Set(
   static_cast<GRA_stringCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LABEL")))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LABELON")))->Get() );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELCOLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LABELCOLOR")))->Get() );
  static_cast<GRA_fontCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELFONT")))->Set(
   static_cast<GRA_fontCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LABELFONT")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELHEIGHT")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LABELHEIGHT")))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELX")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LABELX")))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELY")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LABELY")))->GetAsPercent() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("NUMBERSON")))->Get() );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSCOLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("NUMBERSCOLOR")))->Get() );
  static_cast<GRA_fontCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSFONT")))->Set(
   static_cast<GRA_fontCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSFONT")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSHEIGHT")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("NUMBERSHEIGHT")))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSANGLE")))->Set(
   static_cast<GRA_angleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("NUMBERSANGLE")))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBEROFDIGITS")))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("NUMBEROFDIGITS")))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBEROFDECIMALS")))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("NUMBEROFDECIMALS")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MOD")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("MOD")))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("LEADINGZEROS")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LEADINGZEROS")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("OFFSET")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("OFFSET")))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("DROPFIRSTNUMBER")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("DROPFIRSTNUMBER")))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("DROPLASTNUMBER")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("DROPLASTNUMBER")))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("TICSON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("TICSON")))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("TICSBOTHSIDES")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("TICSBOTHSIDES")))->Get() );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get(wxT("TICANGLE")))->Set(
   static_cast<GRA_angleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("TICANGLE")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get(wxT("LARGETICLENGTH")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("LARGETICLENGTH")))->GetAsPercent() );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get(wxT("SMALLTICLENGTH")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("SMALLTICLENGTH")))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get(wxT("IMAGTICANGLE")))->Set(
   static_cast<GRA_angleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("IMAGTICANGLE")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get(wxT("IMAGTICLENGTH")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("IMAGTICLENGTH")))->GetAsPercent() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("POWER")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("POWER")))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("POWERAUTO")))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get(wxT("POWERAUTO")))->Get() );
  //
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("AXISON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("AXISON")))->Get() );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get(wxT("AXISCOLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("AXISCOLOR")))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("LINEWIDTH")))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LINEWIDTH")))->Get() );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get(wxT("AXISANGLE")))->Set(
   static_cast<GRA_angleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("AXISANGLE")))->Get() );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOWERAXIS")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LOWERAXIS")))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("UPPERAXIS")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("UPPERAXIS")))->GetAsPercent() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MIN")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("MIN")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MAX")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("MAX")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("VIRTUALMIN")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("VIRTUALMIN")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("VIRTUALMAX")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("VIRTUALMAX")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOGBASE")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LOGBASE")))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOGSTYLE")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LOGSTYLE")))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NLINCS")))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("NLINCS")))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NSINCS")))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("NSINCS")))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("GRID")))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("GRID")))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("FORCECROSS")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("FORCECROSS")))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("ZERO")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("ZERO")))->Get() );
  static_cast<GRA_stringCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABEL")))->Set(
   static_cast<GRA_stringCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LABEL")))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LABELON")))->Get() );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELCOLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LABELCOLOR")))->Get() );
  static_cast<GRA_fontCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELFONT")))->Set(
   static_cast<GRA_fontCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LABELFONT")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELHEIGHT")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LABELHEIGHT")))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELX")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LABELX")))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELY")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LABELY")))->GetAsPercent() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("NUMBERSON")))->Get() );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSCOLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("NUMBERSCOLOR")))->Get() );
  static_cast<GRA_fontCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSFONT")))->Set(
   static_cast<GRA_fontCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSFONT")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSHEIGHT")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("NUMBERSHEIGHT")))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSANGLE")))->Set(
   static_cast<GRA_angleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("NUMBERSANGLE")))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBEROFDIGITS")))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("NUMBEROFDIGITS")))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBEROFDECIMALS")))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("NUMBEROFDECIMALS")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MOD")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("MOD")))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("LEADINGZEROS")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LEADINGZEROS")))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("OFFSET")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("OFFSET")))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("DROPFIRSTNUMBER")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("DROPFIRSTNUMBER")))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("DROPLASTNUMBER")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("DROPLASTNUMBER")))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("TICSON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("TICSON")))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("TICSBOTHSIDES")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("TICSBOTHSIDES")))->Get() );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get(wxT("TICANGLE")))->Set(
   static_cast<GRA_angleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("TICANGLE")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get(wxT("LARGETICLENGTH")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("LARGETICLENGTH")))->GetAsPercent() );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get(wxT("SMALLTICLENGTH")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("SMALLTICLENGTH")))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get(wxT("IMAGTICANGLE")))->Set(
   static_cast<GRA_angleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("IMAGTICANGLE")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get(wxT("IMAGTICLENGTH")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("IMAGTICLENGTH")))->GetAsPercent() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("POWER")))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("POWER")))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("POWERAUTO")))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get(wxT("POWERAUTO")))->Get() );
  //
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get(wxT("ON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("ON")))->Get() );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get(wxT("ENTRYLINEON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("ENTRYLINEON")))->Get() );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMEON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("FRAMEON")))->Get() );

  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMEXLO")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("FRAMEXLO")))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMEYLO")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("FRAMEYLO")))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMEXHI")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("FRAMEXHI")))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMEYHI")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("FRAMEYHI")))->GetAsPercent() );

  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TRANSPARENCY")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("TRANSPARENCY")))->Get() );
  static_cast<GRA_intCharacteristic*>(graphLegendCharacteristics_->Get(wxT("SYMBOLS")))->Set(
   static_cast<GRA_intCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("SYMBOLS")))->Get() );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLEON")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("TITLEON")))->Get() );
  static_cast<GRA_stringCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLE")))->Set(
   static_cast<GRA_stringCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("TITLE")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLEHEIGHT")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("TITLEHEIGHT")))->GetAsPercent() );
  static_cast<GRA_colorCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLECOLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->graphLegendCharacteristics_->Get(wxT("TITLECOLOR")))->Get() );
  static_cast<GRA_fontCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLEFONT")))->Set(
   static_cast<GRA_fontCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLEFONT")))->Get() );
  //
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("HISTOGRAMTYPE")))->Set(
   static_cast<GRA_intCharacteristic*>(w->dataCurveCharacteristics_->Get(wxT("HISTOGRAMTYPE")))->Get() );
  static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVECOLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->dataCurveCharacteristics_->Get(wxT("CURVECOLOR")))->Get() );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVELINEWIDTH")))->Set(
   static_cast<GRA_intCharacteristic*>(w->dataCurveCharacteristics_->Get(wxT("CURVELINEWIDTH")))->Get() );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVELINETYPE")))->Set(
   static_cast<GRA_intCharacteristic*>(w->dataCurveCharacteristics_->Get(wxT("CURVELINETYPE")))->Get() );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL")))->Set(
   static_cast<GRA_intCharacteristic*>(w->dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLSIZE")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLSIZE")))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLANGLE")))->Set(
   static_cast<GRA_angleCharacteristic*>(w->dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLANGLE")))->Get() );
  static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLCOLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLCOLOR")))->Get() );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLLINEWIDTH")))->Set(
   static_cast<GRA_intCharacteristic*>(w->dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLLINEWIDTH")))->Get() );
  static_cast<GRA_boolCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CLIP")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->dataCurveCharacteristics_->Get(wxT("CLIP")))->Get() );
  //
  static_cast<GRA_intCharacteristic*>(textCharacteristics_->Get(wxT("ALIGNMENT")))->Set(
   static_cast<GRA_intCharacteristic*>(w->textCharacteristics_->Get(wxT("ALIGNMENT")))->Get() );
  static_cast<GRA_angleCharacteristic*>(textCharacteristics_->Get(wxT("ANGLE")))->Set(
   static_cast<GRA_angleCharacteristic*>(w->textCharacteristics_->Get(wxT("ANGLE")))->Get() );
  static_cast<GRA_colorCharacteristic*>(textCharacteristics_->Get(wxT("COLOR")))->Set(
   static_cast<GRA_colorCharacteristic*>(w->textCharacteristics_->Get(wxT("COLOR")))->Get() );
  static_cast<GRA_fontCharacteristic*>(textCharacteristics_->Get(wxT("FONT")))->Set(
   static_cast<GRA_fontCharacteristic*>(w->textCharacteristics_->Get(wxT("FONT")))->Get() );
  static_cast<GRA_sizeCharacteristic*>(textCharacteristics_->Get(wxT("HEIGHT")))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->textCharacteristics_->Get(wxT("HEIGHT")))->GetAsPercent() );
  static_cast<GRA_boolCharacteristic*>(textCharacteristics_->Get(wxT("INTERACTIVE")))->Set(
   static_cast<GRA_boolCharacteristic*>(w->textCharacteristics_->Get(wxT("INTERACTIVE")))->Get() );
  static_cast<GRA_distanceCharacteristic*>(textCharacteristics_->Get(wxT("XLOCATION")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->textCharacteristics_->Get(wxT("XLOCATION")))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(textCharacteristics_->Get(wxT("YLOCATION")))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->textCharacteristics_->Get(wxT("YLOCATION")))->GetAsPercent() );
}

void GRA_window::Clear()
{
  // destroy all the drawableObjects
  // the monitor screen is cleared by ExGlobals::ClearGraphicsMonitor()
  //
  while ( !drawableObjects_.empty() )
  {
    delete drawableObjects_.back();
    drawableObjects_.pop_back();
  }
}

void GRA_window::Erase()
{
  double xlw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->
                                                        Get(wxT("XLOWERWINDOW")))->GetAsWorld();
  double ylw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->
                                                        Get(wxT("YLOWERWINDOW")))->GetAsWorld();
  double xuw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->
                                                        Get(wxT("XUPPERWINDOW")))->GetAsWorld();
  double yuw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->
                                                        Get(wxT("YUPPERWINDOW")))->GetAsWorld();
  wxClientDC dc( ExGlobals::GetwxWindow() );
  int xl, yl, xu, yu;
  ExGlobals::GetGraphicsOutput()->WorldToOutputType( xlw, ylw, xl, yl );
  ExGlobals::GetGraphicsOutput()->WorldToOutputType( xuw, yuw, xu, yu );
  dc.SetBrush( *wxWHITE_BRUSH );
  dc.SetPen( *wxWHITE_PEN );
  dc.DrawRectangle( xl, yu, xu-xl+1, yu-yl+1 );
}

void GRA_window::Replot()
{
  if( drawableObjects_.empty() )return;
  double xmin = std::numeric_limits<double>::max();
  double xmax = -xmin;
  double ymin = xmin;
  double ymax = xmax;
  double xlog = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOGBASE")))->Get();
  double ylog = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOGBASE")))->Get();
  wxString autoScale( static_cast<GRA_stringCharacteristic*>(generalCharacteristics_->Get(wxT("AUTOSCALE")))->Get() );
  bool curveFound = false;
  int size = static_cast<int>(drawableObjects_.size());
  for( int i=0; i<size; ++i )
  {
    if( drawableObjects_[i]->IsaCartesianAxes() )
    {
      delete drawableObjects_[i];
      drawableObjects_.erase( drawableObjects_.begin()+i );
      --size;
      --i;
    }
    else if( drawableObjects_[i]->IsaCartesianCurve() )
    {
      curveFound = true;
      GRA_cartesianCurve *cc = static_cast<GRA_cartesianCurve*>(drawableObjects_[i]);
      if( autoScale==wxT("ON") || autoScale==wxT("VIRTUAL") || autoScale==wxT("COMMENSURATE") )
      {
        double xmn, ymn, xmx, ymx;
        cc->GetXYMinMax( xmn, xmx, ymn, ymx );
        xmin = std::min( xmin, xmn );
        xmax = std::max( xmax, xmx );
        ymin = std::min( ymin, ymn );
        ymax = std::max( ymax, ymx );
      }
      else if( autoScale==wxT("Y") || autoScale==wxT("YVIRTUAL") )
      {
        double xMin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MIN")))->Get();
        double xMax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MAX")))->Get();
        double xmn, xmx, ymn, ymx;
        if( static_cast<int>(xlog) > 1 )
        {
          xmn = pow(xlog,xMin);
          xmx = pow(xlog,xMax);
        }
        else
        {
          xmn = xMin;
          xmx = xMax;
        }
        cc->GetYMinMax( xmn, xmx, ymn, ymx );
        ymin = std::min( ymin, ymn );
        ymax = std::max( ymax, ymx );
      }
      else if( autoScale==wxT("X") || autoScale==wxT("XVIRTUAL") )
      {
        double yMin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MIN")))->Get();
        double yMax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MAX")))->Get();
        double xmn, ymn, xmx, ymx;
        if( static_cast<int>(ylog) > 1 )
        {
          ymn = pow(ylog,yMin);
          ymx = pow(ylog,yMax);
        }
        else
        {
          ymn = yMin;
          ymx = yMax;
        }
        cc->GetXMinMax( ymn, ymx, xmn, xmx );
        xmin = std::min( xmin, xmn );
        xmax = std::max( xmax, xmx );
      }
      if( xmin > xmax ) // no data points within range
      {                 // can't allow min > max
        xmin = 0.0;
        xmax = 1.0;
      }
      else if( xmin == xmax )
      {
        xmin==0.0 ? xmax=1.0 : xmax=xmin+0.01*fabs(xmin);
      }
      if( ymin > ymax )
      {
        ymin = 0.0;
        ymax = 1.0;
      }
      else if( ymin == ymax )
      {
        ymin==0.0 ? ymax=1.0 : ymax=ymin+0.01*fabs(ymin);
      }
    }
  }
  if( curveFound )
  {
    std::vector<double> xTmp, yTmp;
    xTmp.push_back( xmin );
    xTmp.push_back( xmax );
    yTmp.push_back( ymin );
    yTmp.push_back( ymax );
    if( autoScale == wxT("COMMENSURATE") )
    {
      double xlwind =
          static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("XLOWERWINDOW")))->
          GetAsWorld();
      double ylwind =
          static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("YLOWERWINDOW")))->
          GetAsWorld();
      double xuwind =
          static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("XUPPERWINDOW")))->
          GetAsWorld();
      double yuwind =
          static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("YUPPERWINDOW")))->
          GetAsWorld();
      //
      double delxwind = fabs(xuwind - xlwind);
      double delywind = fabs(yuwind - ylwind);
      //
      double xlaxis = 0.15 * delxwind + xlwind;
      double xuaxis = 0.90 * delxwind + xlwind;
      double ylaxis = 0.15 * delywind + ylwind;
      double yuaxis = 0.90 * delywind + ylwind;
      double xmid = 0.5 * (xlwind + xuwind);
      double ymid = 0.5 * (ylwind + yuwind);
      //
      double xmins, xmaxs, xincs, ymins, ymaxs, yincs;
      try
      {
        UsefulFunctions::Scale1( xmins, xmaxs, xincs, 5, xmin, xmax );
        UsefulFunctions::Scale1( ymins, ymaxs, yincs, 5, ymin, ymax );
      }
      catch ( std::runtime_error &e )
      {
        throw EGraphicsError( wxString(e.what(),wxConvUTF8) );
      }
      double xd = xmaxs - xmins;
      double yd = ymaxs - ymins;
      int nlxinc = static_cast<int>( xd/xincs + 0.5 );
      int nlyinc = static_cast<int>( yd/yincs + 0.5 );
      //
      double delx, dely;
      if( xd >= yd )
      {
        delx = xuaxis - xlaxis;
        dely  = delx * yd / xd;
        ylaxis = ymid - 0.5*dely;
        yuaxis = ymid + 0.5*dely;
      }
      else
      {
        dely = yuaxis - ylaxis;
        delx = dely * xd / yd;
        xlaxis = xmid - 0.5*delx;
        xuaxis = xmid + 0.5*delx;
      }
      if( (ylaxis-ylwind)/delywind < 0.15 )
      {
        dely = 0.15*delywind + ylwind - ylaxis;
        ylaxis = 0.15*delywind + ylwind;
        yuaxis = yuaxis - dely;
        xlaxis = xlaxis + dely;
        xuaxis = xuaxis - dely;
      }
      else if( (xlaxis-xlwind)/delxwind < 0.15 )
      {
        delx = 0.15*delxwind + xlwind - xlaxis;
        xlaxis = 0.15*delxwind + xlwind;
        xuaxis = xuaxis - delx;
        ylaxis = ylaxis + delx;
        yuaxis = yuaxis - delx;
      }
      double xlp = (xlaxis-xlwind)/delxwind*100.;
      double xup = (xuaxis-xlwind)/delxwind*100.;
      double ylp = (ylaxis-ylwind)/delywind*100.;
      double yup = (yuaxis-ylwind)/delywind*100.;
      //
      static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOWERAXIS")))->SetAsPercent( xlp );
      static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("UPPERAXIS")))->SetAsPercent( xup );
      static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MAX")))->Set( xmax );
      static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MIN")))->Set( xmin );
      static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("VIRTUALMAX")))->Set( xmax );
      static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("VIRTUALMIN")))->Set( xmin );
      static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NLINCS")))->Set( nlxinc );
      static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBEROFDIGITS")))->Set( 7 );
      static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBEROFDECIMALS")))->Set( -1 );
      //
      static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOWERAXIS")))->SetAsPercent( ylp );
      static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("UPPERAXIS")))->SetAsPercent( yup );
      static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MAX")))->Set( ymax );
      static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MIN")))->Set( ymin );
      static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("VIRTUALMAX")))->Set( ymax );
      static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("VIRTUALMIN")))->Set( ymin );
      static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NLINCS")))->Set( nlyinc );
      static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBEROFDIGITS")))->Set( 7 );
      static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBEROFDECIMALS")))->Set( -1 );
    }
    GRA_cartesianAxes *cartesianAxes = 0;
    try
    {
      cartesianAxes = new GRA_cartesianAxes( xTmp, yTmp, false, false );
      cartesianAxes->Make();
      AddDrawableObject( cartesianAxes );
    }
    catch ( EGraphicsError &e )
    {
      delete cartesianAxes;
      throw;
    }
  }
  drawableVecIter end = drawableObjects_.end();
  wxClientDC dc( ExGlobals::GetwxWindow() );
  for( drawableVecIter i=drawableObjects_.begin(); i!=end; ++i )
  {
    (*i)->Make();
    (*i)->Draw( ExGlobals::GetGraphicsOutput(), dc );
  }
}

void GRA_window::CreateGeneralCharacteristics( double xl, double yl, double xu, double yu )
{
  double xminw, yminw, xmaxw, ymaxw;
  ExGlobals::GetWorldLimits( xminw, yminw, xmaxw, ymaxw );
  //
  generalCharacteristics_ = new GRA_setOfCharacteristics();
  //
  generalCharacteristics_->AddDistance( wxT("XLOWERWINDOW"), xl, false, xminw, xmaxw );
  generalCharacteristics_->AddDistance( wxT("YLOWERWINDOW"), yl, false, yminw, ymaxw );
  generalCharacteristics_->AddDistance( wxT("XUPPERWINDOW"), xu, false, xminw, xmaxw );
  generalCharacteristics_->AddDistance( wxT("YUPPERWINDOW"), yu, false, yminw, ymaxw );
  generalCharacteristics_->AddString( wxT("AUTOSCALE"), wxString(wxT("ON")) );
  generalCharacteristics_->AddColor( wxT("AREAFILLCOLOR"), 0 );
  generalCharacteristics_->AddBool( wxT("GRAPHBOX"), true );
  generalCharacteristics_->AddColor( wxT("COLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  generalCharacteristics_->AddFont( wxT("FONT"), GRA_fontControl::GetFont(wxT("SWISS")) );
  generalCharacteristics_->AddNumber( wxT("LINEWIDTH"), 1 );
  generalCharacteristics_->AddNumber( wxT("GRIDLINETYPE"), 7 );
  generalCharacteristics_->AddSize( wxT("LEGENDSIZE"), 3.0, true, yl, yu );
  generalCharacteristics_->AddSize( wxT("CONTOURLABELHEIGHT"), 2.0, true, yl, yu );
  generalCharacteristics_->AddSize( wxT("CONTOURLABELSEPARATION"), 50.0, true, yl, yu );
}

void GRA_window::SetGeneralDefaults()
{
  static_cast<GRA_stringCharacteristic*>(generalCharacteristics_->Get(wxT("AUTOSCALE")))->
      Set( wxString(wxT("ON")) );
  static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get(wxT("AREAFILLCOLOR")))->
      Set( reinterpret_cast<GRA_color*>(0) );
  static_cast<GRA_boolCharacteristic*>(generalCharacteristics_->Get(wxT("GRAPHBOX")))->
      Set( true );
  static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get(wxT("COLOR")))->
      Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_fontCharacteristic*>(generalCharacteristics_->Get(wxT("FONT")))->
      Set( GRA_fontControl::GetFont(wxT("SWISS")) );
  static_cast<GRA_intCharacteristic*>(generalCharacteristics_->Get(wxT("LINEWIDTH")))->
      Set( 1 );
  static_cast<GRA_intCharacteristic*>(generalCharacteristics_->Get(wxT("GRIDLINETYPE")))->
      Set( 7 );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get(wxT("LEGENDSIZE")))->
      SetAsPercent( 3.0 );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get(wxT("CONTOURLABELHEIGHT")))->
      SetAsPercent( 2.0 );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get(wxT("CONTOURLABELSEPARATION")))->
      SetAsPercent( 50.0 );
}
  
void GRA_window::CreateXAxisCharacteristics( double xl, double yl, double xu, double yu )
{
  xAxisCharacteristics_ = new GRA_setOfCharacteristics();
  //
  xAxisCharacteristics_->AddBool( wxT("AXISON"), true );
  xAxisCharacteristics_->AddColor( wxT("AXISCOLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  xAxisCharacteristics_->AddNumber( wxT("LINEWIDTH"), 1 );
  xAxisCharacteristics_->AddAngle( wxT("AXISANGLE"), 0.0 );
  xAxisCharacteristics_->AddDistance( wxT("LOWERAXIS"), 10.0, true, xl, xu );
  xAxisCharacteristics_->AddDistance( wxT("UPPERAXIS"), 90.0, true, xl, xu );
  xAxisCharacteristics_->AddNumber( wxT("MIN"), 0.0 );
  xAxisCharacteristics_->AddNumber( wxT("MAX"), 1.0 );
  xAxisCharacteristics_->AddNumber( wxT("VIRTUALMIN"), 0.0 );
  xAxisCharacteristics_->AddNumber( wxT("VIRTUALMAX"), 1.0 );
  xAxisCharacteristics_->AddNumber( wxT("LOGBASE"), 0.0 );
  xAxisCharacteristics_->AddBool( wxT("LOGSTYLE"), true );
  xAxisCharacteristics_->AddNumber( wxT("NLINCS"), 0 );
  xAxisCharacteristics_->AddNumber( wxT("NSINCS"), 0 );
  xAxisCharacteristics_->AddNumber( wxT("GRID"), 0 );
  xAxisCharacteristics_->AddBool( wxT("FORCECROSS"), false );
  xAxisCharacteristics_->AddBool( wxT("ZERO"), false );
  xAxisCharacteristics_->AddString( wxT("LABEL"), wxString(wxT("")) );
  xAxisCharacteristics_->AddBool( wxT("LABELON"), false );
  xAxisCharacteristics_->AddColor( wxT("LABELCOLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  xAxisCharacteristics_->AddFont( wxT("LABELFONT"), GRA_fontControl::GetFont(wxT("SWISS")) );
  xAxisCharacteristics_->AddSize( wxT("LABELHEIGHT"), 3.0, true, yl, yu );
  xAxisCharacteristics_->AddDistance( wxT("LABELX"), 50.0, true, xl, xu );
  xAxisCharacteristics_->AddDistance( wxT("LABELY"), 0.5, true, yl, yu );
  xAxisCharacteristics_->AddBool( wxT("NUMBERSON"), true );
  xAxisCharacteristics_->AddColor( wxT("NUMBERSCOLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  xAxisCharacteristics_->AddFont( wxT("NUMBERSFONT"), GRA_fontControl::GetFont(wxT("SWISS")) );
  xAxisCharacteristics_->AddSize( wxT("NUMBERSHEIGHT"), 3.0, true, yl, yu );
  xAxisCharacteristics_->AddAngle( wxT("NUMBERSANGLE"), 0.0 );
  xAxisCharacteristics_->AddNumber( wxT("NUMBEROFDIGITS"), 5 );
  xAxisCharacteristics_->AddNumber( wxT("NUMBEROFDECIMALS"), -1 );
  xAxisCharacteristics_->AddNumber( wxT("MOD"), 0.0 );
  xAxisCharacteristics_->AddBool( wxT("LEADINGZEROS"), false );
  xAxisCharacteristics_->AddNumber( wxT("OFFSET"), 0.0 );
  xAxisCharacteristics_->AddBool( wxT("DROPFIRSTNUMBER"), false );
  xAxisCharacteristics_->AddBool( wxT("DROPLASTNUMBER"), false );
  xAxisCharacteristics_->AddBool( wxT("TICSON"), true );
  xAxisCharacteristics_->AddBool( wxT("TICSBOTHSIDES"), false );
  xAxisCharacteristics_->AddAngle( wxT("TICANGLE"), 90.0 );
  xAxisCharacteristics_->AddSize( wxT("LARGETICLENGTH"), 2.0, true, yl, yu );
  xAxisCharacteristics_->AddSize( wxT("SMALLTICLENGTH"), 1.0, true, yl, yu );
  xAxisCharacteristics_->AddAngle( wxT("IMAGTICANGLE"), 270.0 );
  xAxisCharacteristics_->AddSize( wxT("IMAGTICLENGTH"), 1.0, true, yl, yu );
  xAxisCharacteristics_->AddNumber( wxT("POWER"), 0.0 );
  xAxisCharacteristics_->AddNumber( wxT("POWERAUTO"), 1 );
}

void GRA_window::SetXAxisDefaults()
{
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("AXISON")))->Set( true );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get(wxT("AXISCOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("LINEWIDTH")))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get(wxT("AXISANGLE")))->Set( 0.0 );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOWERAXIS")))->SetAsPercent( 10.0 );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("UPPERAXIS")))->SetAsPercent( 90.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MIN")))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MAX")))->Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("VIRTUALMIN")))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("VIRTUALMAX")))->Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOGBASE")))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOGSTYLE")))->Set( true );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NLINCS")))->Set( 0 );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NSINCS")))->Set( 0 );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("GRID")))->Set( 0 );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("FORCECROSS")))->Set( false );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("ZERO")))->Set( false );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELON")))->Set( false );
  static_cast<GRA_stringCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABEL")))->Set( wxString(wxT("")) );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELCOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_fontCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELFONT")))->Set( GRA_fontControl::GetFont(wxT("SWISS")) );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELHEIGHT")))->SetAsPercent( 3.0 );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELX")))->SetAsPercent( 50.0 );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("LABELY")))->SetAsPercent( 0.5 );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSON")))->Set( true );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSCOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_fontCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSFONT")))->Set( GRA_fontControl::GetFont(wxT("SWISS")) );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSHEIGHT")))->SetAsPercent( 3.0 );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBERSANGLE")))->Set( 0.0 );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBEROFDIGITS")))->Set( 5 );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("NUMBEROFDECIMALS")))->Set( -1 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MOD")))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("LEADINGZEROS")))->Set( false );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("OFFSET")))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("DROPFIRSTNUMBER")))->Set( false );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("DROPLASTNUMBER")))->Set( false );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("TICSON")))->Set( true );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get(wxT("TICSBOTHSIDES")))->Set( false );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get(wxT("TICANGLE")))->Set( 90.0 );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get(wxT("LARGETICLENGTH")))->SetAsPercent( 2.0 );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get(wxT("SMALLTICLENGTH")))->SetAsPercent( 1.0 );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get(wxT("IMAGTICANGLE")))->Set( 270.0 );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get(wxT("IMAGTICLENGTH")))->SetAsPercent( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("POWER")))->Set( 0.0 );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get(wxT("POWERAUTO")))->Set( 1 );
}

void GRA_window::CreateYAxisCharacteristics( double xl, double yl, double xu, double yu )
{
  yAxisCharacteristics_ = new GRA_setOfCharacteristics();
  //
  yAxisCharacteristics_->AddBool( wxT("AXISON"), true );
  yAxisCharacteristics_->AddColor( wxT("AXISCOLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  yAxisCharacteristics_->AddNumber( wxT("LINEWIDTH"), 1 );
  yAxisCharacteristics_->AddAngle( wxT("AXISANGLE"), 90.0 );
  yAxisCharacteristics_->AddDistance( wxT("LOWERAXIS"), 10.0, true, yl, yu );
  yAxisCharacteristics_->AddDistance( wxT("UPPERAXIS"), 90.0, true, yl, yu );
  yAxisCharacteristics_->AddNumber( wxT("MIN"), 0.0 );
  yAxisCharacteristics_->AddNumber( wxT("MAX"), 1.0 );
  yAxisCharacteristics_->AddNumber( wxT("VIRTUALMIN"), 0.0 );
  yAxisCharacteristics_->AddNumber( wxT("VIRTUALMAX"), 1.0 );
  yAxisCharacteristics_->AddNumber( wxT("LOGBASE"), 0.0 );
  yAxisCharacteristics_->AddBool( wxT("LOGSTYLE"), true );
  yAxisCharacteristics_->AddNumber( wxT("NLINCS"), 0 );
  yAxisCharacteristics_->AddNumber( wxT("NSINCS"), 0 );
  yAxisCharacteristics_->AddNumber( wxT("GRID"), 0 );
  yAxisCharacteristics_->AddBool( wxT("FORCECROSS"), false );
  yAxisCharacteristics_->AddBool( wxT("ZERO"), false );
  yAxisCharacteristics_->AddString( wxT("LABEL"), wxString(wxT("")) );
  yAxisCharacteristics_->AddBool( wxT("LABELON"), false );
  yAxisCharacteristics_->AddColor( wxT("LABELCOLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  yAxisCharacteristics_->AddFont( wxT("LABELFONT"), GRA_fontControl::GetFont(wxT("SWISS")) );
  yAxisCharacteristics_->AddSize( wxT("LABELHEIGHT"), 3.0, true, yl, yu );
  yAxisCharacteristics_->AddDistance( wxT("LABELX"), 0.5, true, xl, xu );
  yAxisCharacteristics_->AddDistance( wxT("LABELY"), 50.0, true, yl, yu );
  yAxisCharacteristics_->AddBool( wxT("NUMBERSON"), true );
  yAxisCharacteristics_->AddColor( wxT("NUMBERSCOLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  yAxisCharacteristics_->AddFont( wxT("NUMBERSFONT"), GRA_fontControl::GetFont(wxT("SWISS")) );
  yAxisCharacteristics_->AddSize( wxT("NUMBERSHEIGHT"), 3.0, true, yl, yu );
  yAxisCharacteristics_->AddAngle( wxT("NUMBERSANGLE"), 0.0 );
  yAxisCharacteristics_->AddNumber( wxT("NUMBEROFDIGITS"), 5 );
  yAxisCharacteristics_->AddNumber( wxT("NUMBEROFDECIMALS"), -1 );
  yAxisCharacteristics_->AddNumber( wxT("MOD"), 0.0 );
  yAxisCharacteristics_->AddBool( wxT("LEADINGZEROS"), false );
  yAxisCharacteristics_->AddNumber( wxT("OFFSET"), 0.0 );
  yAxisCharacteristics_->AddBool( wxT("DROPFIRSTNUMBER"), false );
  yAxisCharacteristics_->AddBool( wxT("DROPLASTNUMBER"), false );
  yAxisCharacteristics_->AddBool( wxT("TICSON"), true );
  yAxisCharacteristics_->AddBool( wxT("TICSBOTHSIDES"), false );
  yAxisCharacteristics_->AddAngle( wxT("TICANGLE"), 270.0 );
  yAxisCharacteristics_->AddSize( wxT("LARGETICLENGTH"), 2.0, true, xl, xu );
  yAxisCharacteristics_->AddSize( wxT("SMALLTICLENGTH"), 1.0, true, xl, xu );
  yAxisCharacteristics_->AddAngle( wxT("IMAGTICANGLE"), 90.0 );
  yAxisCharacteristics_->AddSize( wxT("IMAGTICLENGTH"), 1.0, true, xl, xu );
  yAxisCharacteristics_->AddNumber( wxT("POWER"), 0.0 );
  yAxisCharacteristics_->AddNumber( wxT("POWERAUTO"), 1 );
}

void GRA_window::SetYAxisDefaults()
{
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("AXISON")))->Set( true );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get(wxT("AXISCOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("LINEWIDTH")))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get(wxT("AXISANGLE")))->Set( 90.0 );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOWERAXIS")))->SetAsPercent( 10.0 );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("UPPERAXIS")))->SetAsPercent( 90.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MIN")))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MAX")))->Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("VIRTUALMIN")))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("VIRTUALMAX")))->Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOGBASE")))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOGSTYLE")))->Set( true );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NLINCS")))->Set( 0 );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NSINCS")))->Set( 0 );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("GRID")))->Set( 0 );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("FORCECROSS")))->Set( false );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("ZERO")))->Set( false );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELON")))->Set( false );
  static_cast<GRA_stringCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABEL")))->Set( wxString(wxT("")) );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELCOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_fontCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELFONT")))->Set( GRA_fontControl::GetFont(wxT("SWISS")) );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELHEIGHT")))->SetAsPercent( 3.0 );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELX")))->SetAsPercent( 0.5 );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("LABELY")))->SetAsPercent( 50.0 );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSON")))->Set( true );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSCOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_fontCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSFONT")))->Set( GRA_fontControl::GetFont(wxT("SWISS")) );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSHEIGHT")))->SetAsPercent( 3.0 );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBERSANGLE")))->Set( 0.0 );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBEROFDIGITS")))->Set( 5 );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("NUMBEROFDECIMALS")))->Set( -1 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MOD")))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("LEADINGZEROS")))->Set( false );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("OFFSET")))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("DROPFIRSTNUMBER")))->Set( false );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("DROPLASTNUMBER")))->Set( false );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("TICSON")))->Set( true );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get(wxT("TICSBOTHSIDES")))->Set( false );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get(wxT("TICANGLE")))->Set( 270.0 );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get(wxT("LARGETICLENGTH")))->SetAsPercent( 2.0 );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get(wxT("SMALLTICLENGTH")))->SetAsPercent( 1.0 );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get(wxT("IMAGTICANGLE")))->Set( 90.0 );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get(wxT("IMAGTICLENGTH")))->SetAsPercent( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("POWER")))->Set( 0.0 );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get(wxT("POWERAUTO")))->Set( 1 );
}

void GRA_window::CreateGraphLegendCharacteristics( double xl, double yl, double xu, double yu )
{
  graphLegendCharacteristics_ = new GRA_setOfCharacteristics();
  //
  graphLegendCharacteristics_->AddBool( wxT("ON"), false );
  graphLegendCharacteristics_->AddBool( wxT("ENTRYLINEON"), true );
  graphLegendCharacteristics_->AddBool( wxT("FRAMEON"), true );
  graphLegendCharacteristics_->AddDistance( wxT("FRAMEXLO"), 15.0, true, xl, xu );
  graphLegendCharacteristics_->AddDistance( wxT("FRAMEYLO"), 70.0, true, yl, yu );
  graphLegendCharacteristics_->AddDistance( wxT("FRAMEXHI"), 50.0, true, xl, xu );
  graphLegendCharacteristics_->AddDistance( wxT("FRAMEYHI"), 90.0, true, yl, yu );
  graphLegendCharacteristics_->AddColor( wxT("FRAMECOLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  graphLegendCharacteristics_->AddBool( wxT("TRANSPARENCY"), true );
  graphLegendCharacteristics_->AddNumber( wxT("SYMBOLS"), 1 );
  graphLegendCharacteristics_->AddBool( wxT("TITLEON"), true );
  graphLegendCharacteristics_->AddString( wxT("TITLE"), wxString(wxT("")) );
  graphLegendCharacteristics_->AddSize( wxT("TITLEHEIGHT"), 3.0, true, yl, yu );
  graphLegendCharacteristics_->AddColor( wxT("TITLECOLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  graphLegendCharacteristics_->AddFont( wxT("TITLEFONT"), GRA_fontControl::GetFont(wxT("SWISS")) );
}

void GRA_window::SetGraphLegendDefaults()
{
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get(wxT("ON")))->Set( false );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get(wxT("ENTRYLINEON")))->Set( true );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMEON")))->Set( true );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMEXLO")))->SetAsPercent( 15.0 );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMEYLO")))->SetAsPercent( 70.0 );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMEXHI")))->SetAsPercent( 50.0 );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMEYHI")))->SetAsPercent( 90.0 );
  static_cast<GRA_colorCharacteristic*>(graphLegendCharacteristics_->Get(wxT("FRAMECOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TRANSPARENCY")))->Set( true );
  static_cast<GRA_intCharacteristic*>(graphLegendCharacteristics_->Get(wxT("SYMBOLS")))->Set( 1 );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLEON")))->Set( true );
  static_cast<GRA_stringCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLE")))->Set( wxString(wxT("")) );
  static_cast<GRA_sizeCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLEHEIGHT")))->SetAsPercent( 3.0 );
  static_cast<GRA_colorCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLECOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_fontCharacteristic*>(graphLegendCharacteristics_->Get(wxT("TITLEFONT")))->Set( GRA_fontControl::GetFont(wxT("SWISS")) );
}

void GRA_window::CreateDataCurveCharacteristics( double yl, double yu )
{
  dataCurveCharacteristics_ = new GRA_setOfCharacteristics();
  //
  dataCurveCharacteristics_->AddNumber( wxT("HISTOGRAMTYPE"), 0 );
  dataCurveCharacteristics_->AddColor( wxT("CURVECOLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  dataCurveCharacteristics_->AddNumber( wxT("CURVELINEWIDTH"), 1 );
  dataCurveCharacteristics_->AddNumber( wxT("CURVELINETYPE"), 1 );
  dataCurveCharacteristics_->AddSize( wxT("PLOTSYMBOLSIZE"), 2.0, true, yl, yu );
  dataCurveCharacteristics_->AddAngle( wxT("PLOTSYMBOLANGLE"), 0.0 );
  dataCurveCharacteristics_->AddColor( wxT("PLOTSYMBOLCOLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  dataCurveCharacteristics_->AddNumber( wxT("PLOTSYMBOLLINEWIDTH"), 1 );
  dataCurveCharacteristics_->AddNumber( wxT("PLOTSYMBOL"), 0 );
  dataCurveCharacteristics_->AddBool( wxT("CLIP"), true );
}

void GRA_window::SetDataCurveDefaults()
{
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("HISTOGRAMTYPE")))->Set( 0 );
  static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVECOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVELINEWIDTH")))->Set( 1 );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVELINETYPE")))->Set( 1 );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL")))->Set( 0 );
  static_cast<GRA_sizeCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLSIZE")))->SetAsPercent( 2.0 );
  static_cast<GRA_angleCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLANGLE")))->Set( 0.0 );
  static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLCOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOLLINEWIDTH")))->Set( 1 );
  static_cast<GRA_boolCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CLIP")))->Set( true );
}

void GRA_window::CreateTextCharacteristics( double xl, double yl, double xu, double yu )
{
  textCharacteristics_ = new GRA_setOfCharacteristics();
  //
  textCharacteristics_->AddNumber( wxT("ALIGNMENT"), 1 );
  textCharacteristics_->AddAngle( wxT("ANGLE"), 0.0 );
  textCharacteristics_->AddColor( wxT("COLOR"), GRA_colorControl::GetColor(wxT("BLACK")) );
  textCharacteristics_->AddFont( wxT("FONT"), GRA_fontControl::GetFont(wxT("SWISS")) );
  textCharacteristics_->AddSize( wxT("HEIGHT"), 3.0, true, yl, yu );
  textCharacteristics_->AddBool( wxT("INTERACTIVE"), true );
  textCharacteristics_->AddDistance( wxT("XLOCATION"), 50.0, true, xl, xu );
  textCharacteristics_->AddDistance( wxT("YLOCATION"), 50.0, true, yl, yu );
}

void GRA_window::SetTextDefaults()
{
  static_cast<GRA_intCharacteristic*>(textCharacteristics_->Get(wxT("ALIGNMENT")))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(textCharacteristics_->Get(wxT("ANGLE")))->Set( 0.0 );
  static_cast<GRA_colorCharacteristic*>(textCharacteristics_->Get(wxT("COLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_fontCharacteristic*>(textCharacteristics_->Get(wxT("FONT")))->Set( GRA_fontControl::GetFont(wxT("SWISS")) );
  static_cast<GRA_sizeCharacteristic*>(textCharacteristics_->Get(wxT("HEIGHT")))->SetAsPercent( 3.0 );
  static_cast<GRA_boolCharacteristic*>(textCharacteristics_->Get(wxT("INTERACTIVE")))->Set( true );
  static_cast<GRA_distanceCharacteristic*>(textCharacteristics_->Get(wxT("XLOCATION")))->SetAsPercent( 50.0 );
  static_cast<GRA_distanceCharacteristic*>(textCharacteristics_->Get(wxT("YLOCATION")))->SetAsPercent( 50.0 );
}

void GRA_window::WorldToPercent( double x, double y, double &xout, double &yout ) const
{
  // convert x and y from world units to percentages of the window
  //
  double xlwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
  double xuwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
  double ylwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("YLOWERWINDOW")))->GetAsWorld();
  double yuwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("YUPPERWINDOW")))->GetAsWorld();
  xout = 100.*(x-xlwind)/(xuwind-xlwind);
  yout = 100.*(y-ylwind)/(yuwind-ylwind);
}

void GRA_window::PercentToWorld( double x, double y, double &xout, double &yout ) const
{
  // convert x and y from percentages of the window to world
  //
  double xlwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
  double xuwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
  double ylwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("YLOWERWINDOW")))->GetAsWorld();
  double yuwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("YUPPERWINDOW")))->GetAsWorld();
  xout = x*(xuwind-xlwind)/100. + xlwind;
  yout = y*(yuwind-ylwind)/100. + ylwind;
}

void GRA_window::GraphToWorld( double xg, double yg, double &xw, double &yw, bool reset ) const
{
  // convert from graph units to world units
  //
  static double xlog, ylog, xlaxis, ylaxis, cosx, sinx, cosy, siny, ax, bx, ay, by, xmin, ymin;
  if( reset )GetValues( xlog, ylog, xlaxis, ylaxis, cosx, sinx, cosy, siny, ax, bx, ay, by, xmin, ymin );
  if( static_cast<int>(xlog) > 1 )
  {
    if( xg <= 0.0 )xg = std::numeric_limits<double>::min();
    xg = log(xg)/log(xlog);  // linearize
  }
  if( static_cast<int>(ylog) > 1 )
  {
    if( yg <= 0.0 )yg = std::numeric_limits<double>::min();
    yg = log(yg)/log(ylog);  // linearize
  }
  xw = xlaxis + cosx*(ax*xg+bx) + cosy*(ay*yg+by);
  yw = ylaxis + sinx*(ax*xg+bx) + siny*(ay*yg+by);
}

void GRA_window::GraphToWorld( std::vector<double> const &xg, std::vector<double> const &yg,
                               std::vector<double> &xw, std::vector<double> &yw ) const
{
  std::size_t const size = xg.size();
  xw.resize( size );
  yw.resize( size );
  GraphToWorld( xg[0], yg[0], xw[0], yw[0], true );
  for( std::size_t i=1; i<size; ++i )GraphToWorld( xg[i], yg[i], xw[i], yw[i], false );
}

void GRA_window::WorldToGraph( double xw, double yw, double &xg, double &yg, bool reset ) const
{
  // convert from world units to graph units
  //
  static double xlog, ylog, xlaxis, ylaxis, cosx, sinx, cosy, siny, ax, bx, ay, by, xmin, ymin;
  if( reset )GetValues( xlog, ylog, xlaxis, ylaxis, cosx, sinx, cosy, siny, ax, bx, ay, by, xmin, ymin );
  xg = xmin + ((yw-ylaxis)*cosy-(xw-xlaxis)*siny)/ax/(cosy*sinx-cosx*siny);
  yg = ymin + ((xw-xlaxis)*sinx-(yw-ylaxis)*cosx)/ay/(cosy*sinx-cosx*siny);
  if( static_cast<int>(xlog) > 1 )
  {
    if( xg*log(xlog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError(wxT("problem with x-axis log scales"));
    xg = exp(xg*log(xlog));
  }
  if( static_cast<int>(ylog) > 1 )
  {
    if( yg*log(ylog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError(wxT("problem with y-axis log scales"));
    yg = exp(yg*log(ylog));
  }
}

void GRA_window::GetValues( double &xlog, double &ylog, double &xlaxis, double &ylaxis,
                            double &cosx, double &sinx, double &cosy, double &siny,
                            double &ax, double &bx, double &ay, double &by,
                            double &xmin, double &ymin ) const
{
  xlog = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOGBASE")))->Get();
  ylog = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOGBASE")))->Get();
  xmin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MIN")))->Get();
  double const xmax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MAX")))->Get();
  ymin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MIN")))->Get();
  double const ymax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MAX")))->Get();
  double const xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("UPPERAXIS")))->GetAsWorld();
  xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOWERAXIS")))->GetAsWorld();
  double const yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("UPPERAXIS")))->GetAsWorld();
  ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOWERAXIS")))->GetAsWorld();
  double const degreesToRadians = M_PI/180.;
  double const xaxisa = static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get(wxT("AXISANGLE")))->Get();
  double const yaxisa = static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get(wxT("AXISANGLE")))->Get();
  cosx = cos( xaxisa*degreesToRadians );
  cosy = cos( yaxisa*degreesToRadians );
  sinx = sin( xaxisa*degreesToRadians );
  siny = sin( yaxisa*degreesToRadians );
  double const eps = 0.000001;
  if( fabs(cosx) < eps )cosx = 0.0;
  if( fabs(cosy) < eps )cosy = 0.0;
  if( fabs(sinx) < eps )sinx = 0.0;
  if( fabs(siny) < eps )siny = 0.0;
  xmax!=xmin ? ax=(xuaxis-xlaxis)/(xmax-xmin) : ax=1.0;
  ymax!=ymin ? ay=(yuaxis-ylaxis)/(ymax-ymin) : ay=1.0;
  bx = -ax*xmin;
  by = -ay*ymin;
}

bool GRA_window::InsideWindow( double x, double y ) const
{
  if( !drawableObjects_.empty() )
  {
    double xlw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
    double ylw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("YLOWERWINDOW")))->GetAsWorld();
    double xuw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
    double yuw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get(wxT("YUPPERWINDOW")))->GetAsWorld();
    std::vector<double> xv, yv;
    xv.push_back(xlw);
    yv.push_back(ylw);
    xv.push_back(xuw);
    yv.push_back(ylw);
    xv.push_back(xuw);
    yv.push_back(yuw);
    xv.push_back(xlw);
    yv.push_back(yuw);
    if( UsefulFunctions::InsidePolygon(x,y,xv,yv) )return true;
  }
  return false;
}

void GRA_window::StartLine( double x, double y, double &xw, double &yw )
{
  // x, y are in graph units, returns xw, yw in world units
  //
  // must call StartLine before calling ContinueLine
  //
  double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MIN")))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MAX")))->Get();
  double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MIN")))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MAX")))->Get();
  bool clip = static_cast<GRA_boolCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CLIP")))->Get();
  //
  if( !clip || ((x-xmin)*(xmax-x)>=0.0 && (y-ymin)*(ymax-y)>=0.0) )GraphToWorld(x,y,xw,yw);
  xPrevious_ = x;
  yPrevious_ = y;
}

void GRA_window::ContinueLine( double x, double y, double *xw, double *yw, int &ndraw )
{
  // x, y are in graph units, xw, yw are in world units
  // xw, yw are arrays with 2 elements, ndraw returns the number of elements used
  //
  // must call StartLine before calling ContinueLine
  // since xPrevious_, yPrevious_ are assumed here to have appropriate values
  //
  double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MIN")))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MAX")))->Get();
  double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MIN")))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MAX")))->Get();
  bool clip = static_cast<GRA_boolCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CLIP")))->Get();
  //
  double xdraw[2], ydraw[2];
  if( clip )
  {
    UsefulFunctions::WindowClip( xPrevious_, yPrevious_, x, y, xmin, xmax, ymin, ymax,
                                 xdraw, ydraw, ndraw );
    if( ndraw == 1 )
    {
      GraphToWorld( xdraw[0], ydraw[0], xw[0], yw[0] );
    }
    else if( ndraw==2 || ndraw==3 )
    {
      GraphToWorld( xdraw[0], ydraw[0], xw[0], yw[0] );
      GraphToWorld( xdraw[1], ydraw[1], xw[1], yw[1] );
    }
  }
  else  // not clip
  {
    ndraw = 1;
    GraphToWorld( x, y, xw[0], yw[0] );
  }
  xPrevious_ = x;
  yPrevious_ = y;
}

void GRA_window::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  try
  {
    drawableVecIter end = drawableObjects_.end();
    for( drawableVecIter i=drawableObjects_.begin(); i!=end; ++i )
      (*i)->Draw( graphicsOutput, dc );
  }
  catch ( EGraphicsError &e )
  {
    throw;
  }
}

void GRA_window::RemoveLastTextString()
{
  wxWindow *wx = ExGlobals::GetwxWindow();
  wxClientDC dc( wx );
  wx->PrepareDC( dc );
  for( int i=static_cast<int>(drawableObjects_.size())-1; i>=0; --i )
  {
    if( drawableObjects_[i]->IsaDrawableText() )
    {
      static_cast<GRA_drawableText*>(drawableObjects_[i])->Erase( ExGlobals::GetGraphicsOutput(), dc );
      drawableObjects_.erase( drawableObjects_.begin()+i );
      break;
    }
  }
} 

void GRA_window::DrawZerolines( bool horizontal, bool vertical )
{
  GRA_wxWidgets *monitor = ExGlobals::GetGraphicsOutput();
  double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MIN")))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("MAX")))->Get();
  double xlog = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get(wxT("LOGBASE")))->Get();
  double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MIN")))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("MAX")))->Get();
  double ylog = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get(wxT("LOGBASE")))->Get();
  //
  if( xlog > 1.0 )
  {
    if( xmin*log(xlog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError( wxT("problem with x-axis log scales min") );
    if( xmax*log(xlog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError( wxT("problem with x-axis log scales max"));
    xmin = pow(xlog,xmin);
    xmax = pow(xlog,xmax);
  }
  if( ylog > 1.0 )
  {
    if( ymin*log(ylog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError( wxT("problem with y-axis log scales min"));
    if( ymax*log(ylog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError( wxT("problem with y-axis log scales max"));
    ymin = pow(ylog,ymin);
    ymax = pow(ylog,ymax);
  }
  std::vector<double> xTmp, yTmp, zTmp, xe1, ye1, xe2, ye2;
  xTmp.push_back( xmin );
  xTmp.push_back( xmax );
  yTmp.push_back( ymin );
  yTmp.push_back( ymax );
  zTmp.push_back( 0.0 );
  zTmp.push_back( 0.0 );
  //
  wxClientDC dc( ExGlobals::GetwxWindow() );
  if( horizontal )
  {
    GRA_intCharacteristic plotsymbolSave(
      *static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL"))) );
    GRA_colorCharacteristic curveColorSave(
      *static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVECOLOR"))) );
    static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL")))->Set( 0 );
    GRA_color *axisColor =
      static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get(wxT("AXISCOLOR")))->Get();
    static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVECOLOR")))->Set( axisColor );
    GRA_cartesianCurve *cartesianCurve = new GRA_cartesianCurve( xTmp, zTmp, xe1, ye1, xe2, ye2 );
    if( !cartesianCurve )throw EGraphicsError( wxT("could not make new horizontal zero line") );
    try
    {
      cartesianCurve->Make();
    }
    catch (EGraphicsError &e)
    {
      *static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL"))) = plotsymbolSave;
      *static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVECOLOR"))) = curveColorSave;
      delete cartesianCurve;
      throw;
    }
    cartesianCurve->Draw( monitor, dc );
    AddDrawableObject( cartesianCurve );
    *static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL"))) = plotsymbolSave;
    *static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVECOLOR"))) = curveColorSave;
  }
  if( vertical )
  {
    GRA_intCharacteristic plotsymbolSave(
      *static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL"))) );
    GRA_colorCharacteristic curveColorSave(
      *static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVECOLOR"))) );
    static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL")))->Set( 0 );
    GRA_color *axisColor =
      static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get(wxT("AXISCOLOR")))->Get();
    static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVECOLOR")))->Set( axisColor );
    GRA_cartesianCurve *cartesianCurve = new GRA_cartesianCurve( zTmp, yTmp, xe1, ye1, xe2, ye2 );
    if( !cartesianCurve )throw EGraphicsError( wxT("could not make new vertical zero line") );
    try
    {
      cartesianCurve->Make();
    }
    catch (EGraphicsError &e)
    {
      *static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL"))) = plotsymbolSave;
      *static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVECOLOR"))) = curveColorSave;
      delete cartesianCurve;
      throw;
    }
    cartesianCurve->Draw( monitor, dc );
    AddDrawableObject( cartesianCurve );
    *static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get(wxT("PLOTSYMBOL"))) = plotsymbolSave;
    *static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get(wxT("CURVECOLOR"))) = curveColorSave;
  }
}

std::ostream &operator<<( std::ostream &out, GRA_window const &gw )
{
  out << "<graphwindow number=\"" << gw.number_ << "\" xPrevious=\""
      << gw.xPrevious_ << "\" yPrevious=\"" << gw.yPrevious_
      << "\" xLoP=\"" << gw.xLoP_ << "\" yLoP=\"" << gw.yLoP_
      << "\" xHiP=\"" << gw.xHiP_ << "\" yHiP=\"" << gw.yHiP_ << "\">\n"
      << "<axisc>\n" << *gw.xAxisCharacteristics_ << "</axisc>\n"
      << "<axisc>\n" << *gw.yAxisCharacteristics_ << "</axisc>\n"
      << "<generalc>\n" << *gw.generalCharacteristics_ << "</generalc>\n"
      << "<textc>\n" << *gw.textCharacteristics_ << "</textc>\n"
      << "<graphlegendc>\n" << *gw.graphLegendCharacteristics_ << "</graphlegendc>\n"
      << "<datacurvec>\n" << *gw.dataCurveCharacteristics_ << "</datacurvec>\n"
      << "<drawableobjects>\n";
  GRA_window::drawableVecIter end = gw.drawableObjects_.end();
  for( GRA_window::drawableVecIter i=gw.drawableObjects_.begin(); i!=end; ++i )
  {
    if( (*i)->IsaPolyline() )out << *(static_cast<GRA_polyline*>(*i));
    else if( (*i)->IsaDrawableText() )out << *(static_cast<GRA_drawableText*>(*i));
    else if( (*i)->IsaCartesianAxes() )out << *(static_cast<GRA_cartesianAxes*>(*i));
    else if( (*i)->IsaCartesianCurve() )out << *(static_cast<GRA_cartesianCurve*>(*i));
    else if( (*i)->IsaGraphLegend() )out << *(static_cast<GRA_legend*>(*i));
    else if( (*i)->IsaPoint() )out << *(static_cast<GRA_point*>(*i));
    else if( (*i)->IsaEllipse() )out << *(static_cast<GRA_ellipse*>(*i));
    else if( (*i)->IsaPolygon() )out << *(static_cast<GRA_polygon*>(*i));
    else if( (*i)->IsaMultilineFigure() )out << *(static_cast<GRA_multiLineFigure*>(*i));
    else if( (*i)->IsaPlotsymbol() )out << *(static_cast<GRA_plotSymbol*>(*i));
    else if( (*i)->IsaContour() )out << *(static_cast<GRA_contour*>(*i));
    else if( (*i)->IsaBoxPlot() )out << *(static_cast<GRA_boxPlot*>(*i));
    else if( (*i)->IsaDitheringPlot() )out << *(static_cast<GRA_ditheringPlot*>(*i));
    else if( (*i)->IsaDiffusionPlot() )out << *(static_cast<GRA_diffusionPlot*>(*i));
    else if( (*i)->IsaGradientPlot() )out << *(static_cast<GRA_gradientPlot*>(*i));
    //else if( (*i)->IsaThreeDFigure() )out << *(static_cast<GRA_threeDFigure*>(*i));
  }
  return out << "</drawableobjects>\n</graphwindow>\n";
}

// end of file

