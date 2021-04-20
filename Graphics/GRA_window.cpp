/*
Copyright (C) 2005,...,2010 Joseph L. Chuma

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

#include <algorithm>

#include "GRA_window.h"

extern "C"
{
#include "gd.h"
}

#include "EGraphicsError.h"
#include "GRA_outputType.h"
#include "GRA_drawableText.h"
#include "UsefulFunctions.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "ExGlobals.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_cartesianCurve.h"
#include "GRA_cartesianAxes.h"
#include "GRA_point.h"
#include "GRA_polyline.h"
#include "GRA_polygon.h"
#include "GRA_rectangle.h"
#include "GRA_ellipse.h"
#include "GRA_plotSymbol.h"
#include "GRA_axis.h"
#include "GRA_legend.h"
#include "GRA_contourLine.h"
#include "GRA_multiLineFigure.h"
#include "GRA_boxPlot.h"
#include "GRA_ditheringPlot.h"
#include "GRA_diffusionPlot.h"
#include "GRA_gradientPlot.h"
#include "GRA_polarAxes.h"
#include "GRA_polarCurve.h"
#include "MainGraphicsWindow.h"
#include "ThreeDPlot.h"
#include "GRA_threeDPlot.h"
//#include "GRA_threeDFigure.h"

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
  CreatePolarCharacteristics( xl, yl, xu, yu );
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
  delete polarCharacteristics_;
  xAxisCharacteristics_ = 0;
  yAxisCharacteristics_ = 0;
  generalCharacteristics_ = 0;
  textCharacteristics_ = 0;
  graphLegendCharacteristics_ = 0;
  dataCurveCharacteristics_ = 0;
  polarCharacteristics_ = 0;
}

void GRA_window::CopyStuff( GRA_window const &rhs )
{
  xLoP_ = rhs.xLoP_;
  yLoP_ = rhs.yLoP_;
  xHiP_ = rhs.xHiP_;
  yHiP_ = rhs.yHiP_;
  DeleteCharacteristics();
  SetUp();
  xPrevious_ = rhs.xPrevious_;
  yPrevious_ = rhs.yPrevious_;
  Clear();
  DisplayBackground( ExGlobals::GetScreenOutput() );
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
      p = new GRA_contourLine(*(static_cast<GRA_contourLine*>(*i)));
    else if( (*i)->IsaPolarAxes() )
      p = new GRA_polarAxes(*(static_cast<GRA_polarAxes*>(*i)));
    else if( (*i)->IsaPolarCurve() )
      p = new GRA_polarCurve(*(static_cast<GRA_polarCurve*>(*i)));
    drawableObjects_.push_back( p );
  }
  *xAxisCharacteristics_ = *rhs.xAxisCharacteristics_;
  *yAxisCharacteristics_ = *rhs.yAxisCharacteristics_;
  *textCharacteristics_ = *rhs.textCharacteristics_;
  *generalCharacteristics_ = *rhs.generalCharacteristics_;
  *graphLegendCharacteristics_ = *rhs.graphLegendCharacteristics_;
  *dataCurveCharacteristics_ = *rhs.dataCurveCharacteristics_;
  *polarCharacteristics_ = *rhs.polarCharacteristics_;
}

void GRA_window::Reset()
{
  Clear();
  DeleteCharacteristics();
  SetUp();
  DisplayBackground( ExGlobals::GetScreenOutput() );
}

void GRA_window::AddDrawableObject( GRA_drawableObject *object )
{ drawableObjects_.push_back( object ); }

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
  static_cast<GRA_stringCharacteristic*>(generalCharacteristics_->Get("AUTOSCALE"))->Set(
   static_cast<GRA_stringCharacteristic*>(w->generalCharacteristics_->Get("AUTOSCALE"))->Get() );
  static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get("AREAFILLCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->generalCharacteristics_->Get("AREAFILLCOLOR"))->Get() );
  static_cast<GRA_boolCharacteristic*>(generalCharacteristics_->Get("GRAPHBOX"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->generalCharacteristics_->Get("GRAPHBOX"))->Get() );
  static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get("COLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->generalCharacteristics_->Get("COLOR"))->Get() );
  static_cast<GRA_fontCharacteristic*>(generalCharacteristics_->Get("FONT"))->Set(
   static_cast<GRA_fontCharacteristic*>(w->generalCharacteristics_->Get("FONT"))->Get() );
  static_cast<GRA_intCharacteristic*>(generalCharacteristics_->Get("LINEWIDTH"))->Set(
   static_cast<GRA_intCharacteristic*>(w->generalCharacteristics_->Get("LINEWIDTH"))->Get() );
  static_cast<GRA_intCharacteristic*>(generalCharacteristics_->Get("GRIDLINETYPE"))->Set(
   static_cast<GRA_intCharacteristic*>(w->generalCharacteristics_->Get("GRIDLINETYPE"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get("LEGENDSIZE"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->generalCharacteristics_->Get("LEGENDSIZE"))->GetAsPercent() );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get("CONTOURLABELHEIGHT"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->generalCharacteristics_->Get("CONTOURLABELHEIGHT"))->GetAsPercent() );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get("CONTOURLABELSEPARATION"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->generalCharacteristics_->Get("CONTOURLABELSEPARATION"))->GetAsPercent() );
  static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get("BACKGROUNDCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->generalCharacteristics_->Get("COLOR"))->Get() );
  static_cast<GRA_stringCharacteristic*>(generalCharacteristics_->Get("BACKGROUNDFILE"))->Set(
   static_cast<GRA_stringCharacteristic*>(w->generalCharacteristics_->Get("BACKGROUNDFILE"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(generalCharacteristics_->Get("ARROWHEADWIDTH"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->generalCharacteristics_->Get("ARROWHEADWIDTH"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(generalCharacteristics_->Get("ARROWHEADLENGTH"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->generalCharacteristics_->Get("ARROWHEADLENGTH"))->Get() );
  //
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("AXISON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("AXISON"))->Get() );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get("AXISCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->xAxisCharacteristics_->Get("AXISCOLOR"))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("LINEWIDTH"))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get("LINEWIDTH"))->Get() );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get("AXISANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->xAxisCharacteristics_->Get("AXISANGLE"))->Get() );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get("LOWERAXIS"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->xAxisCharacteristics_->Get("LOWERAXIS"))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get("UPPERAXIS"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->xAxisCharacteristics_->Get("UPPERAXIS"))->GetAsPercent() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MIN"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get("MIN"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MAX"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get("MAX"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("VIRTUALMIN"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get("VIRTUALMIN"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("VIRTUALMAX"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get("VIRTUALMAX"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("LOGBASE"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get("LOGBASE"))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("LOGSTYLE"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("LOGSTYLE"))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NLINCS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get("NLINCS"))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NSINCS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get("NSINCS"))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("GRID"))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get("GRID"))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("FORCECROSS"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("FORCECROSS"))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("ZERO"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("ZERO"))->Get() );
  static_cast<GRA_stringCharacteristic*>(xAxisCharacteristics_->Get("LABEL"))->Set(
   static_cast<GRA_stringCharacteristic*>(w->xAxisCharacteristics_->Get("LABEL"))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("LABELON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("LABELON"))->Get() );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get("LABELCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->xAxisCharacteristics_->Get("LABELCOLOR"))->Get() );
  static_cast<GRA_fontCharacteristic*>(xAxisCharacteristics_->Get("LABELFONT"))->Set(
   static_cast<GRA_fontCharacteristic*>(w->xAxisCharacteristics_->Get("LABELFONT"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get("LABELHEIGHT"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->xAxisCharacteristics_->Get("LABELHEIGHT"))->GetAsPercent() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("NUMBERSON"))->Get() );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->xAxisCharacteristics_->Get("NUMBERSCOLOR"))->Get() );
  static_cast<GRA_fontCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSFONT"))->Set(
   static_cast<GRA_fontCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSFONT"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSHEIGHT"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->xAxisCharacteristics_->Get("NUMBERSHEIGHT"))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->xAxisCharacteristics_->Get("NUMBERSANGLE"))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NUMBEROFDIGITS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get("NUMBEROFDIGITS"))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NUMBEROFDECIMALS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get("NUMBEROFDECIMALS"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MOD"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get("MOD"))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("LEADINGZEROS"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("LEADINGZEROS"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("OFFSET"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get("OFFSET"))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("DROPFIRSTNUMBER"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("DROPFIRSTNUMBER"))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("DROPLASTNUMBER"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("DROPLASTNUMBER"))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("TICSON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("TICSON"))->Get() );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("TICSBOTHSIDES"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->xAxisCharacteristics_->Get("TICSBOTHSIDES"))->Get() );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get("TICANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->xAxisCharacteristics_->Get("TICANGLE"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get("LARGETICLENGTH"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->xAxisCharacteristics_->Get("LARGETICLENGTH"))->GetAsPercent() );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get("SMALLTICLENGTH"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->xAxisCharacteristics_->Get("SMALLTICLENGTH"))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get("IMAGTICANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->xAxisCharacteristics_->Get("IMAGTICANGLE"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get("IMAGTICLENGTH"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->xAxisCharacteristics_->Get("IMAGTICLENGTH"))->GetAsPercent() );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("POWER"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->xAxisCharacteristics_->Get("POWER"))->Get() );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("POWERAUTO"))->Set(
   static_cast<GRA_intCharacteristic*>(w->xAxisCharacteristics_->Get("POWERAUTO"))->Get() );
  //
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("AXISON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("AXISON"))->Get() );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get("AXISCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->yAxisCharacteristics_->Get("AXISCOLOR"))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("LINEWIDTH"))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get("LINEWIDTH"))->Get() );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get("AXISANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->yAxisCharacteristics_->Get("AXISANGLE"))->Get() );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get("LOWERAXIS"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->yAxisCharacteristics_->Get("LOWERAXIS"))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get("UPPERAXIS"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->yAxisCharacteristics_->Get("UPPERAXIS"))->GetAsPercent() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MIN"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get("MIN"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MAX"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get("MAX"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("VIRTUALMIN"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get("VIRTUALMIN"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("VIRTUALMAX"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get("VIRTUALMAX"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("LOGBASE"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get("LOGBASE"))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("LOGSTYLE"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("LOGSTYLE"))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NLINCS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get("NLINCS"))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NSINCS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get("NSINCS"))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("GRID"))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get("GRID"))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("FORCECROSS"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("FORCECROSS"))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("ZERO"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("ZERO"))->Get() );
  static_cast<GRA_stringCharacteristic*>(yAxisCharacteristics_->Get("LABEL"))->Set(
   static_cast<GRA_stringCharacteristic*>(w->yAxisCharacteristics_->Get("LABEL"))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("LABELON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("LABELON"))->Get() );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get("LABELCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->yAxisCharacteristics_->Get("LABELCOLOR"))->Get() );
  static_cast<GRA_fontCharacteristic*>(yAxisCharacteristics_->Get("LABELFONT"))->Set(
   static_cast<GRA_fontCharacteristic*>(w->yAxisCharacteristics_->Get("LABELFONT"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get("LABELHEIGHT"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->yAxisCharacteristics_->Get("LABELHEIGHT"))->GetAsPercent() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("NUMBERSON"))->Get() );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->yAxisCharacteristics_->Get("NUMBERSCOLOR"))->Get() );
  static_cast<GRA_fontCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSFONT"))->Set(
   static_cast<GRA_fontCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSFONT"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSHEIGHT"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->yAxisCharacteristics_->Get("NUMBERSHEIGHT"))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->yAxisCharacteristics_->Get("NUMBERSANGLE"))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NUMBEROFDIGITS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get("NUMBEROFDIGITS"))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NUMBEROFDECIMALS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get("NUMBEROFDECIMALS"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MOD"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get("MOD"))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("LEADINGZEROS"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("LEADINGZEROS"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("OFFSET"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get("OFFSET"))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("DROPFIRSTNUMBER"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("DROPFIRSTNUMBER"))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("DROPLASTNUMBER"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("DROPLASTNUMBER"))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("TICSON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("TICSON"))->Get() );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("TICSBOTHSIDES"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->yAxisCharacteristics_->Get("TICSBOTHSIDES"))->Get() );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get("TICANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->yAxisCharacteristics_->Get("TICANGLE"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get("LARGETICLENGTH"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->yAxisCharacteristics_->Get("LARGETICLENGTH"))->GetAsPercent() );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get("SMALLTICLENGTH"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->yAxisCharacteristics_->Get("SMALLTICLENGTH"))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get("IMAGTICANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->yAxisCharacteristics_->Get("IMAGTICANGLE"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get("IMAGTICLENGTH"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->yAxisCharacteristics_->Get("IMAGTICLENGTH"))->GetAsPercent() );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("POWER"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->yAxisCharacteristics_->Get("POWER"))->Get() );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("POWERAUTO"))->Set(
   static_cast<GRA_intCharacteristic*>(w->yAxisCharacteristics_->Get("POWERAUTO"))->Get() );
  //
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get("ON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->graphLegendCharacteristics_->Get("ON"))->Get() );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get("ENTRYLINEON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->graphLegendCharacteristics_->Get("ENTRYLINEON"))->Get() );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get("FRAMEON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->graphLegendCharacteristics_->Get("FRAMEON"))->Get() );

  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get("FRAMEXLO"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->graphLegendCharacteristics_->Get("FRAMEXLO"))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get("FRAMEYLO"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->graphLegendCharacteristics_->Get("FRAMEYLO"))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get("FRAMEXHI"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->graphLegendCharacteristics_->Get("FRAMEXHI"))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get("FRAMEYHI"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->graphLegendCharacteristics_->Get("FRAMEYHI"))->GetAsPercent() );

  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get("TRANSPARENCY"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->graphLegendCharacteristics_->Get("TRANSPARENCY"))->Get() );
  static_cast<GRA_intCharacteristic*>(graphLegendCharacteristics_->Get("SYMBOLS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->graphLegendCharacteristics_->Get("SYMBOLS"))->Get() );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get("TITLEON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->graphLegendCharacteristics_->Get("TITLEON"))->Get() );
  static_cast<GRA_stringCharacteristic*>(graphLegendCharacteristics_->Get("TITLE"))->Set(
   static_cast<GRA_stringCharacteristic*>(w->graphLegendCharacteristics_->Get("TITLE"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(graphLegendCharacteristics_->Get("TITLEHEIGHT"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->graphLegendCharacteristics_->Get("TITLEHEIGHT"))->GetAsPercent() );
  static_cast<GRA_colorCharacteristic*>(graphLegendCharacteristics_->Get("TITLECOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->graphLegendCharacteristics_->Get("TITLECOLOR"))->Get() );
  static_cast<GRA_fontCharacteristic*>(graphLegendCharacteristics_->Get("TITLEFONT"))->Set(
   static_cast<GRA_fontCharacteristic*>(graphLegendCharacteristics_->Get("TITLEFONT"))->Get() );
  //
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get("HISTOGRAMTYPE"))->Set(
   static_cast<GRA_intCharacteristic*>(w->dataCurveCharacteristics_->Get("HISTOGRAMTYPE"))->Get() );
  static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("CURVECOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->dataCurveCharacteristics_->Get("CURVECOLOR"))->Get() );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get("CURVELINEWIDTH"))->Set(
   static_cast<GRA_intCharacteristic*>(w->dataCurveCharacteristics_->Get("CURVELINEWIDTH"))->Get() );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get("CURVELINETYPE"))->Set(
   static_cast<GRA_intCharacteristic*>(w->dataCurveCharacteristics_->Get("CURVELINETYPE"))->Get() );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get("PLOTSYMBOL"))->Set(
   static_cast<GRA_intCharacteristic*>(w->dataCurveCharacteristics_->Get("PLOTSYMBOL"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(dataCurveCharacteristics_->Get("PLOTSYMBOLSIZE"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->dataCurveCharacteristics_->Get("PLOTSYMBOLSIZE"))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(dataCurveCharacteristics_->Get("PLOTSYMBOLANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->dataCurveCharacteristics_->Get("PLOTSYMBOLANGLE"))->Get() );
  static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("PLOTSYMBOLCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->dataCurveCharacteristics_->Get("PLOTSYMBOLCOLOR"))->Get() );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get("PLOTSYMBOLLINEWIDTH"))->Set(
   static_cast<GRA_intCharacteristic*>(w->dataCurveCharacteristics_->Get("PLOTSYMBOLLINEWIDTH"))->Get() );
  static_cast<GRA_boolCharacteristic*>(dataCurveCharacteristics_->Get("CLIP"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->dataCurveCharacteristics_->Get("CLIP"))->Get() );
  //
  static_cast<GRA_intCharacteristic*>(textCharacteristics_->Get("ALIGNMENT"))->Set(
   static_cast<GRA_intCharacteristic*>(w->textCharacteristics_->Get("ALIGNMENT"))->Get() );
  static_cast<GRA_angleCharacteristic*>(textCharacteristics_->Get("ANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->textCharacteristics_->Get("ANGLE"))->Get() );
  static_cast<GRA_colorCharacteristic*>(textCharacteristics_->Get("COLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->textCharacteristics_->Get("COLOR"))->Get() );
  static_cast<GRA_fontCharacteristic*>(textCharacteristics_->Get("FONT"))->Set(
   static_cast<GRA_fontCharacteristic*>(w->textCharacteristics_->Get("FONT"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(textCharacteristics_->Get("HEIGHT"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->textCharacteristics_->Get("HEIGHT"))->GetAsPercent() );
  static_cast<GRA_boolCharacteristic*>(textCharacteristics_->Get("INTERACTIVE"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->textCharacteristics_->Get("INTERACTIVE"))->Get() );
  static_cast<GRA_distanceCharacteristic*>(textCharacteristics_->Get("XLOCATION"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->textCharacteristics_->Get("XLOCATION"))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(textCharacteristics_->Get("YLOCATION"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->textCharacteristics_->Get("YLOCATION"))->GetAsPercent() );
  //
  static_cast<GRA_distanceCharacteristic*>(polarCharacteristics_->Get("XORIGIN"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->polarCharacteristics_->Get("XORIGIN"))->GetAsPercent() );
  static_cast<GRA_distanceCharacteristic*>(polarCharacteristics_->Get("YORIGIN"))->SetAsPercent(
   static_cast<GRA_distanceCharacteristic*>(w->polarCharacteristics_->Get("YORIGIN"))->GetAsPercent() );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("AXISLENGTH"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->polarCharacteristics_->Get("AXISLENGTH"))->GetAsPercent() );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("NAXES"))->Set(
   static_cast<GRA_intCharacteristic*>(w->polarCharacteristics_->Get("NAXES"))->Get() );

  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("AXISON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("AXISON"))->Get() );
  static_cast<GRA_colorCharacteristic*>(polarCharacteristics_->Get("AXISCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->polarCharacteristics_->Get("AXISCOLOR"))->Get() );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("LINEWIDTH"))->Set(
   static_cast<GRA_intCharacteristic*>(w->polarCharacteristics_->Get("LINEWIDTH"))->Get() );
  static_cast<GRA_angleCharacteristic*>(polarCharacteristics_->Get("AXISANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->polarCharacteristics_->Get("AXISANGLE"))->Get() );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("COMPASSLABELS"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("COMPASSLABELS"))->Get() );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("CLOCKWISE"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("CLOCKWISE"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("MIN"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->polarCharacteristics_->Get("MIN"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("MAX"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->polarCharacteristics_->Get("MAX"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("VIRTUALMIN"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->polarCharacteristics_->Get("VIRTUALMIN"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("VIRTUALMAX"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->polarCharacteristics_->Get("VIRTUALMAX"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("LOGBASE"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->polarCharacteristics_->Get("LOGBASE"))->Get() );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("LOGSTYLE"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("LOGSTYLE"))->Get() );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("NLINCS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->polarCharacteristics_->Get("NLINCS"))->Get() );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("NSINCS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->polarCharacteristics_->Get("NSINCS"))->Get() );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("GRID"))->Set(
   static_cast<GRA_intCharacteristic*>(w->polarCharacteristics_->Get("GRID"))->Get() );
  static_cast<GRA_stringCharacteristic*>(polarCharacteristics_->Get("LABEL"))->Set(
   static_cast<GRA_stringCharacteristic*>(w->polarCharacteristics_->Get("LABEL"))->Get() );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("LABELON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("LABELON"))->Get() );
  static_cast<GRA_colorCharacteristic*>(polarCharacteristics_->Get("LABELCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->polarCharacteristics_->Get("LABELCOLOR"))->Get() );
  static_cast<GRA_fontCharacteristic*>(polarCharacteristics_->Get("LABELFONT"))->Set(
   static_cast<GRA_fontCharacteristic*>(w->polarCharacteristics_->Get("LABELFONT"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("LABELHEIGHT"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->polarCharacteristics_->Get("LABELHEIGHT"))->GetAsPercent() );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("NUMBERSON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("NUMBERSON"))->Get() );
  static_cast<GRA_colorCharacteristic*>(polarCharacteristics_->Get("NUMBERSCOLOR"))->Set(
   static_cast<GRA_colorCharacteristic*>(w->polarCharacteristics_->Get("NUMBERSCOLOR"))->Get() );
  static_cast<GRA_fontCharacteristic*>(polarCharacteristics_->Get("NUMBERSFONT"))->Set(
   static_cast<GRA_fontCharacteristic*>(polarCharacteristics_->Get("NUMBERSFONT"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("NUMBERSHEIGHT"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->polarCharacteristics_->Get("NUMBERSHEIGHT"))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(polarCharacteristics_->Get("NUMBERSANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->polarCharacteristics_->Get("NUMBERSANGLE"))->Get() );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("NUMBEROFDIGITS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->polarCharacteristics_->Get("NUMBEROFDIGITS"))->Get() );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("NUMBEROFDECIMALS"))->Set(
   static_cast<GRA_intCharacteristic*>(w->polarCharacteristics_->Get("NUMBEROFDECIMALS"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("MOD"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->polarCharacteristics_->Get("MOD"))->Get() );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("LEADINGZEROS"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("LEADINGZEROS"))->Get() );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("OFFSET"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->polarCharacteristics_->Get("OFFSET"))->Get() );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("DROPFIRSTNUMBER"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("DROPFIRSTNUMBER"))->Get() );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("DROPLASTNUMBER"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("DROPLASTNUMBER"))->Get() );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("TICSON"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("TICSON"))->Get() );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("TICSBOTHSIDES"))->Set(
   static_cast<GRA_boolCharacteristic*>(w->polarCharacteristics_->Get("TICSBOTHSIDES"))->Get() );
  static_cast<GRA_angleCharacteristic*>(polarCharacteristics_->Get("TICANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->polarCharacteristics_->Get("TICANGLE"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("LARGETICLENGTH"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->polarCharacteristics_->Get("LARGETICLENGTH"))->GetAsPercent() );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("SMALLTICLENGTH"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->polarCharacteristics_->Get("SMALLTICLENGTH"))->GetAsPercent() );
  static_cast<GRA_angleCharacteristic*>(polarCharacteristics_->Get("IMAGTICANGLE"))->Set(
   static_cast<GRA_angleCharacteristic*>(w->polarCharacteristics_->Get("IMAGTICANGLE"))->Get() );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("IMAGTICLENGTH"))->SetAsPercent(
   static_cast<GRA_sizeCharacteristic*>(w->polarCharacteristics_->Get("IMAGTICLENGTH"))->GetAsPercent() );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("POWER"))->Set(
   static_cast<GRA_doubleCharacteristic*>(w->polarCharacteristics_->Get("POWER"))->Get() );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("POWERAUTO"))->Set(
   static_cast<GRA_intCharacteristic*>(w->polarCharacteristics_->Get("POWERAUTO"))->Get() );
}

void GRA_window::Clear()
{
  // destroy all the drawableObjects
  // the monitor screen is cleared by ExGlobals::ClearGraphicsMonitor()
  //
  while ( !drawableObjects_.empty() )
  {
    GRA_drawableObject *obj = drawableObjects_.back();
    if( obj->IsaThreeDPlot() )
    {
      TThreeDPlotForm *form = dynamic_cast<GRA_threeDPlot*>(obj)->GetForm();
      if( form )form->ZeroPlot();
    }
    delete obj;
    drawableObjects_.pop_back();
  }
}

void GRA_window::Erase()
{
  double xlw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("XLOWERWINDOW"))->GetAsWorld();
  double ylw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("YLOWERWINDOW"))->GetAsWorld();
  double xuw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("XUPPERWINDOW"))->GetAsWorld();
  double yuw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("YUPPERWINDOW"))->GetAsWorld();
  ExGlobals::GetScreenOutput()->Erase( xlw, ylw, xuw, yuw );
}

void GRA_window::Replot()
{
  if( drawableObjects_.empty() )return;

  double xmin = std::numeric_limits<double>::max();
  double xmax = -xmin;
  double ymin = xmin;
  double ymax = xmax;
  double xlog = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("LOGBASE"))->Get();
  double ylog = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("LOGBASE"))->Get();
  ExString autoScale( static_cast<GRA_stringCharacteristic*>(generalCharacteristics_->Get("AUTOSCALE"))->Get() );
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
      if( autoScale=="ON" || autoScale=="VIRTUAL" || autoScale=="COMMENSURATE" )
      {
        double xmn, ymn, xmx, ymx;
        cc->GetXYMinMax( xmn, xmx, ymn, ymx );
        xmin = std::min( xmin, xmn );
        xmax = std::max( xmax, xmx );
        ymin = std::min( ymin, ymn );
        ymax = std::max( ymax, ymx );
      }
      else if( autoScale=="Y" || autoScale=="YVIRTUAL" )
      {
        xmin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MIN"))->Get();
        xmax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MAX"))->Get();
        if( static_cast<int>(xlog) > 1 )
        {
          xmin = pow(xlog,xmin);
          xmax = pow(xlog,xmax);
        }
        double ymn, ymx;
        cc->GetYMinMax( xmin, xmax, ymn, ymx );
        ymin = std::min( ymin, ymn );
        ymax = std::max( ymax, ymx );
      }
      else if( autoScale=="X" || autoScale=="XVIRTUAL" )
      {
        double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MIN"))->Get();
        double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MAX"))->Get();
        if( static_cast<int>(ylog) > 1 )
        {
          ymin = pow(ylog,ymin);
          ymax = pow(ylog,ymax);
        }
        double xmn, xmx;
        cc->GetXMinMax( ymin, ymax, xmn, xmx );
        xmin = std::min( xmin, xmn );
        xmax = std::max( xmax, xmx );
      }
      else if( autoScale=="OFF" )
      {
        xmin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MIN"))->Get();
        xmax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MAX"))->Get();
        if( static_cast<int>(xlog) > 1 )
        {
          xmin = pow(xlog,xmin);
          xmax = pow(xlog,xmax);
        }
        ymin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MIN"))->Get();
        ymax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MAX"))->Get();
        if( static_cast<int>(ylog) > 1 )
        {
          ymin = pow(ylog,ymin);
          ymax = pow(ylog,ymax);
        }
      }
      if( xmin > xmax ) // no data points within range
      {                 // can't allow min > max
        xmin = 0.0;
        xmax = 1.0;
      }
      else if( xmin == xmax )
      {
        xmax = xmin==0.0 ? 1.0 : xmin+0.01*fabs(xmin);
      }
      if( ymin > ymax )
      {
        ymin = 0.0;
        ymax = 1.0;
      }
      else if( ymin == ymax )
      {
        ymax = ymin==0.0 ? 1.0 : ymin+0.01*fabs(ymin);
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
    if( autoScale == "COMMENSURATE" )
    {
      double xlwind =
       static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("XLOWERWINDOW"))->
        GetAsWorld();
      double ylwind =
       static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("YLOWERWINDOW"))->
        GetAsWorld();
      double xuwind =
       static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("XUPPERWINDOW"))->
        GetAsWorld();
      double yuwind =
       static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("YUPPERWINDOW"))->
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
      catch ( runtime_error &e )
      {
        throw EGraphicsError( e.what() );
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
      static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get("LOWERAXIS"))->SetAsPercent( xlp );
      static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get("UPPERAXIS"))->SetAsPercent( xup );
      static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NLINCS"))->Set( nlxinc );
      static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NUMBEROFDIGITS"))->Set( 7 );
      static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NUMBEROFDECIMALS"))->Set( -1 );
      //
      static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get("LOWERAXIS"))->SetAsPercent( ylp );
      static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get("UPPERAXIS"))->SetAsPercent( yup );
      static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NLINCS"))->Set( nlyinc );
      static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NUMBEROFDIGITS"))->Set( 7 );
      static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NUMBEROFDECIMALS"))->Set( -1 );
    }
    if( static_cast<int>(xlog) > 1 )
    {
      xmin = xmin<=0.0 ? std::numeric_limits<double>::min() : log(xmin)/log(xlog);
      xmax = xmax<=0.0 ? std::numeric_limits<double>::min() : log(xmax)/log(xlog);
    }
    if( static_cast<int>(ylog) > 1 )
    {
      ymin = ymin<=0.0 ? std::numeric_limits<double>::min() : log(ymin)/log(ylog);
      ymax = ymax<=0.0 ? std::numeric_limits<double>::min() : log(ymax)/log(ylog);
    }
    static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MAX"))->Set( xmax );
    static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MIN"))->Set( xmin );
    static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("VIRTUALMAX"))->Set( xmax );
    static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("VIRTUALMIN"))->Set( xmin );
    static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MAX"))->Set( ymax );
    static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MIN"))->Set( ymin );
    static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("VIRTUALMAX"))->Set( ymax );
    static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("VIRTUALMIN"))->Set( ymin );
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
  for( drawableVecIter i=drawableObjects_.begin(); i!=end; ++i )
  {
    (*i)->Make();
    (*i)->Draw( ExGlobals::GetScreenOutput() );
  }
}

void GRA_window::CreateGeneralCharacteristics( double xl, double yl, double xu, double yu )
{
  double xminw, yminw, xmaxw, ymaxw;
  ExGlobals::GetWorldLimits( xminw, yminw, xmaxw, ymaxw );
  //
  generalCharacteristics_ = new GRA_setOfCharacteristics();
  //
  generalCharacteristics_->AddDistance( "XLOWERWINDOW", xl, false, xminw, xmaxw );
  generalCharacteristics_->AddDistance( "YLOWERWINDOW", yl, false, yminw, ymaxw );
  generalCharacteristics_->AddDistance( "XUPPERWINDOW", xu, false, xminw, xmaxw );
  generalCharacteristics_->AddDistance( "YUPPERWINDOW", yu, false, yminw, ymaxw );
  generalCharacteristics_->AddString( "AUTOSCALE", ExString("ON") );
  generalCharacteristics_->AddColor( "AREAFILLCOLOR", reinterpret_cast<GRA_color*>(0) ); // zero means no color
  generalCharacteristics_->AddBool( "GRAPHBOX", true );
  generalCharacteristics_->AddColor( "COLOR", GRA_colorControl::GetColor("BLACK") );
  generalCharacteristics_->AddFont( "FONT", GRA_fontControl::GetFont("ARIAL") );
  generalCharacteristics_->AddNumber( "LINEWIDTH", 1 );
  generalCharacteristics_->AddNumber( "GRIDLINETYPE", 7 );
  generalCharacteristics_->AddSize( "LEGENDSIZE", 3.0, true, yl, yu );
  generalCharacteristics_->AddSize( "CONTOURLABELHEIGHT", 2.0, true, yl, yu );
  generalCharacteristics_->AddSize( "CONTOURLABELSEPARATION", 50.0, true, yl, yu );
  generalCharacteristics_->AddColor( "BACKGROUNDCOLOR", reinterpret_cast<GRA_color*>(0) );
  generalCharacteristics_->AddString( "BACKGROUNDFILE", ExString("") );
  generalCharacteristics_->AddNumber( "ARROWHEADWIDTH", 1.0 );
  generalCharacteristics_->AddNumber( "ARROWHEADLENGTH", 1.0 );
}

void GRA_window::SetGeneralDefaults()
{
  static_cast<GRA_stringCharacteristic*>(generalCharacteristics_->Get("AUTOSCALE"))->
      Set( ExString("ON") );
  static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get("AREAFILLCOLOR"))->
      Set( reinterpret_cast<GRA_color*>(0) );
  static_cast<GRA_boolCharacteristic*>(generalCharacteristics_->Get("GRAPHBOX"))->
      Set( true );
  static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get("COLOR"))->
      Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(generalCharacteristics_->Get("FONT"))->
      Set( GRA_fontControl::GetFont("ARIAL") );
  static_cast<GRA_intCharacteristic*>(generalCharacteristics_->Get("LINEWIDTH"))->
      Set( 1 );
  static_cast<GRA_intCharacteristic*>(generalCharacteristics_->Get("GRIDLINETYPE"))->
      Set( 7 );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get("LEGENDSIZE"))->
      SetAsPercent( 3.0 );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get("CONTOURLABELHEIGHT"))->
      SetAsPercent( 2.0 );
  static_cast<GRA_sizeCharacteristic*>(generalCharacteristics_->Get("CONTOURLABELSEPARATION"))->
      SetAsPercent( 50.0 );
  static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get("BACKGROUNDCOLOR"))->
      Set( reinterpret_cast<GRA_color*>(0) );
  static_cast<GRA_stringCharacteristic*>(generalCharacteristics_->Get("BACKGROUNDFILE"))->
      Set( ExString("") );
  static_cast<GRA_doubleCharacteristic*>(generalCharacteristics_->Get("ARROWHEADWIDTH"))->
      Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(generalCharacteristics_->Get("ARROWHEADLENGTH"))->
      Set( 1.0 );
}

void GRA_window::CreateXAxisCharacteristics( double xl, double yl, double xu, double yu )
{
  xAxisCharacteristics_ = new GRA_setOfCharacteristics();
  //
  xAxisCharacteristics_->AddBool( "AXISON", true );
  xAxisCharacteristics_->AddColor( "AXISCOLOR", GRA_colorControl::GetColor("BLACK") );
  xAxisCharacteristics_->AddNumber( "LINEWIDTH", 1 );
  xAxisCharacteristics_->AddAngle( "AXISANGLE", 0.0 );
  xAxisCharacteristics_->AddDistance( "LOWERAXIS", 10.0, true, xl, xu );
  xAxisCharacteristics_->AddDistance( "UPPERAXIS", 90.0, true, xl, xu );
  xAxisCharacteristics_->AddNumber( "MIN", 0.0 );
  xAxisCharacteristics_->AddNumber( "MAX", 1.0 );
  xAxisCharacteristics_->AddNumber( "VIRTUALMIN", 0.0 );
  xAxisCharacteristics_->AddNumber( "VIRTUALMAX", 1.0 );
  xAxisCharacteristics_->AddNumber( "LOGBASE", 0.0 );
  xAxisCharacteristics_->AddBool( "LOGSTYLE", true );
  xAxisCharacteristics_->AddNumber( "NLINCS", 0 );
  xAxisCharacteristics_->AddNumber( "NSINCS", 0 );
  xAxisCharacteristics_->AddNumber( "GRID", 0 );
  xAxisCharacteristics_->AddBool( "FORCECROSS", false );
  xAxisCharacteristics_->AddBool( "ZERO", false );
  xAxisCharacteristics_->AddString( "LABEL", ExString("") );
  xAxisCharacteristics_->AddBool( "LABELON", false );
  xAxisCharacteristics_->AddColor( "LABELCOLOR", GRA_colorControl::GetColor("BLACK") );
  xAxisCharacteristics_->AddFont( "LABELFONT", GRA_fontControl::GetFont("ARIAL") );
  xAxisCharacteristics_->AddSize( "LABELHEIGHT", 3.0, true, yl, yu );
  xAxisCharacteristics_->AddBool( "NUMBERSON", true );
  xAxisCharacteristics_->AddColor( "NUMBERSCOLOR", GRA_colorControl::GetColor("BLACK") );
  xAxisCharacteristics_->AddFont( "NUMBERSFONT", GRA_fontControl::GetFont("ARIAL") );
  xAxisCharacteristics_->AddSize( "NUMBERSHEIGHT", 3.0, true, yl, yu );
  xAxisCharacteristics_->AddAngle( "NUMBERSANGLE", 0.0 );
  xAxisCharacteristics_->AddNumber( "NUMBEROFDIGITS", 7 );
  xAxisCharacteristics_->AddNumber( "NUMBEROFDECIMALS", -1 );
  xAxisCharacteristics_->AddNumber( "MOD", 0.0 );
  xAxisCharacteristics_->AddBool( "LEADINGZEROS", false );
  xAxisCharacteristics_->AddNumber( "OFFSET", 0.0 );
  xAxisCharacteristics_->AddBool( "DROPFIRSTNUMBER", false );
  xAxisCharacteristics_->AddBool( "DROPLASTNUMBER", false );
  xAxisCharacteristics_->AddBool( "TICSON", true );
  xAxisCharacteristics_->AddBool( "TICSBOTHSIDES", false );
  xAxisCharacteristics_->AddAngle( "TICANGLE", 90.0 );
  xAxisCharacteristics_->AddSize( "LARGETICLENGTH", 2.0, true, yl, yu );
  xAxisCharacteristics_->AddSize( "SMALLTICLENGTH", 1.0, true, yl, yu );
  xAxisCharacteristics_->AddAngle( "IMAGTICANGLE", 270.0 );
  xAxisCharacteristics_->AddSize( "IMAGTICLENGTH", 1.0, true, yl, yu );
  xAxisCharacteristics_->AddNumber( "POWER", 0.0 );
  xAxisCharacteristics_->AddNumber( "POWERAUTO", 1 );
}

void GRA_window::SetXAxisDefaults()
{
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("AXISON"))->Set( true );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get("AXISCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("LINEWIDTH"))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get("AXISANGLE"))->Set( 0.0 );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get("LOWERAXIS"))->SetAsPercent( 10.0 );
  static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get("UPPERAXIS"))->SetAsPercent( 90.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MIN"))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MAX"))->Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("VIRTUALMIN"))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("VIRTUALMAX"))->Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("LOGBASE"))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("LOGSTYLE"))->Set( true );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NLINCS"))->Set( 0 );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NSINCS"))->Set( 0 );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("GRID"))->Set( 0 );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("FORCECROSS"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("ZERO"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("LABELON"))->Set( false );
  static_cast<GRA_stringCharacteristic*>(xAxisCharacteristics_->Get("LABEL"))->Set( ExString("") );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get("LABELCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(xAxisCharacteristics_->Get("LABELFONT"))->Set( GRA_fontControl::GetFont("ARIAL") );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get("LABELHEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSON"))->Set( true );
  static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSFONT"))->Set( GRA_fontControl::GetFont("ARIAL") );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSHEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get("NUMBERSANGLE"))->Set( 0.0 );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NUMBEROFDIGITS"))->Set( 7 );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("NUMBEROFDECIMALS"))->Set( -1 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MOD"))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("LEADINGZEROS"))->Set( false );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("OFFSET"))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("DROPFIRSTNUMBER"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("DROPLASTNUMBER"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("TICSON"))->Set( true );
  static_cast<GRA_boolCharacteristic*>(xAxisCharacteristics_->Get("TICSBOTHSIDES"))->Set( false );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get("TICANGLE"))->Set( 90.0 );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get("LARGETICLENGTH"))->SetAsPercent( 2.0 );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get("SMALLTICLENGTH"))->SetAsPercent( 1.0 );
  static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get("IMAGTICANGLE"))->Set( 270.0 );
  static_cast<GRA_sizeCharacteristic*>(xAxisCharacteristics_->Get("IMAGTICLENGTH"))->SetAsPercent( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("POWER"))->Set( 0.0 );
  static_cast<GRA_intCharacteristic*>(xAxisCharacteristics_->Get("POWERAUTO"))->Set( 1 );
}

void GRA_window::CreateYAxisCharacteristics( double xl, double yl, double xu, double yu )
{
  yAxisCharacteristics_ = new GRA_setOfCharacteristics();
  //
  yAxisCharacteristics_->AddBool( "AXISON", true );
  yAxisCharacteristics_->AddColor( "AXISCOLOR", GRA_colorControl::GetColor("BLACK") );
  yAxisCharacteristics_->AddNumber( "LINEWIDTH", 1 );
  yAxisCharacteristics_->AddAngle( "AXISANGLE", 90.0 );
  yAxisCharacteristics_->AddDistance( "LOWERAXIS", 10.0, true, yl, yu );
  yAxisCharacteristics_->AddDistance( "UPPERAXIS", 90.0, true, yl, yu );
  yAxisCharacteristics_->AddNumber( "MIN", 0.0 );
  yAxisCharacteristics_->AddNumber( "MAX", 1.0 );
  yAxisCharacteristics_->AddNumber( "VIRTUALMIN", 0.0 );
  yAxisCharacteristics_->AddNumber( "VIRTUALMAX", 1.0 );
  yAxisCharacteristics_->AddNumber( "LOGBASE", 0.0 );
  yAxisCharacteristics_->AddBool( "LOGSTYLE", true );
  yAxisCharacteristics_->AddNumber( "NLINCS", 0 );
  yAxisCharacteristics_->AddNumber( "NSINCS", 0 );
  yAxisCharacteristics_->AddNumber( "GRID", 0 );
  yAxisCharacteristics_->AddBool( "FORCECROSS", false );
  yAxisCharacteristics_->AddBool( "ZERO", false );
  yAxisCharacteristics_->AddString( "LABEL", ExString("") );
  yAxisCharacteristics_->AddBool( "LABELON", false );
  yAxisCharacteristics_->AddColor( "LABELCOLOR", GRA_colorControl::GetColor("BLACK") );
  yAxisCharacteristics_->AddFont( "LABELFONT", GRA_fontControl::GetFont("ARIAL") );
  yAxisCharacteristics_->AddSize( "LABELHEIGHT", 3.0, true, yl, yu );
  yAxisCharacteristics_->AddBool( "NUMBERSON", true );
  yAxisCharacteristics_->AddColor( "NUMBERSCOLOR", GRA_colorControl::GetColor("BLACK") );
  yAxisCharacteristics_->AddFont( "NUMBERSFONT", GRA_fontControl::GetFont("ARIAL") );
  yAxisCharacteristics_->AddSize( "NUMBERSHEIGHT", 3.0, true, yl, yu );
  yAxisCharacteristics_->AddAngle( "NUMBERSANGLE", 0.0 );
  yAxisCharacteristics_->AddNumber( "NUMBEROFDIGITS", 7 );
  yAxisCharacteristics_->AddNumber( "NUMBEROFDECIMALS", -1 );
  yAxisCharacteristics_->AddNumber( "MOD", 0.0 );
  yAxisCharacteristics_->AddBool( "LEADINGZEROS", false );
  yAxisCharacteristics_->AddNumber( "OFFSET", 0.0 );
  yAxisCharacteristics_->AddBool( "DROPFIRSTNUMBER", false );
  yAxisCharacteristics_->AddBool( "DROPLASTNUMBER", false );
  yAxisCharacteristics_->AddBool( "TICSON", true );
  yAxisCharacteristics_->AddBool( "TICSBOTHSIDES", false );
  yAxisCharacteristics_->AddAngle( "TICANGLE", 270.0 );
  yAxisCharacteristics_->AddSize( "LARGETICLENGTH", 2.0, true, xl, xu );
  yAxisCharacteristics_->AddSize( "SMALLTICLENGTH", 1.0, true, xl, xu );
  yAxisCharacteristics_->AddAngle( "IMAGTICANGLE", 90.0 );
  yAxisCharacteristics_->AddSize( "IMAGTICLENGTH", 1.0, true, xl, xu );
  yAxisCharacteristics_->AddNumber( "POWER", 0.0 );
  yAxisCharacteristics_->AddNumber( "POWERAUTO", 1 );
}

void GRA_window::SetYAxisDefaults()
{
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("AXISON"))->Set( true );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get("AXISCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("LINEWIDTH"))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get("AXISANGLE"))->Set( 90.0 );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get("LOWERAXIS"))->SetAsPercent( 10.0 );
  static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get("UPPERAXIS"))->SetAsPercent( 90.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MIN"))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MAX"))->Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("VIRTUALMIN"))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("VIRTUALMAX"))->Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("LOGBASE"))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("LOGSTYLE"))->Set( true );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NLINCS"))->Set( 0 );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NSINCS"))->Set( 0 );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("GRID"))->Set( 0 );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("FORCECROSS"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("ZERO"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("LABELON"))->Set( false );
  static_cast<GRA_stringCharacteristic*>(yAxisCharacteristics_->Get("LABEL"))->Set( ExString("") );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get("LABELCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(yAxisCharacteristics_->Get("LABELFONT"))->Set( GRA_fontControl::GetFont("ARIAL") );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get("LABELHEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSON"))->Set( true );
  static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSFONT"))->Set( GRA_fontControl::GetFont("ARIAL") );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSHEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get("NUMBERSANGLE"))->Set( 0.0 );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NUMBEROFDIGITS"))->Set( 7 );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("NUMBEROFDECIMALS"))->Set( -1 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MOD"))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("LEADINGZEROS"))->Set( false );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("OFFSET"))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("DROPFIRSTNUMBER"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("DROPLASTNUMBER"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("TICSON"))->Set( true );
  static_cast<GRA_boolCharacteristic*>(yAxisCharacteristics_->Get("TICSBOTHSIDES"))->Set( false );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get("TICANGLE"))->Set( 270.0 );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get("LARGETICLENGTH"))->SetAsPercent( 2.0 );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get("SMALLTICLENGTH"))->SetAsPercent( 1.0 );
  static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get("IMAGTICANGLE"))->Set( 90.0 );
  static_cast<GRA_sizeCharacteristic*>(yAxisCharacteristics_->Get("IMAGTICLENGTH"))->SetAsPercent( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("POWER"))->Set( 0.0 );
  static_cast<GRA_intCharacteristic*>(yAxisCharacteristics_->Get("POWERAUTO"))->Set( 1 );
}

void GRA_window::CreateGraphLegendCharacteristics( double xl, double yl, double xu, double yu )
{
  graphLegendCharacteristics_ = new GRA_setOfCharacteristics();
  //
  graphLegendCharacteristics_->AddBool( "ON", false );
  graphLegendCharacteristics_->AddBool( "ENTRYLINEON", true );
  graphLegendCharacteristics_->AddBool( "FRAMEON", true );
  graphLegendCharacteristics_->AddDistance( "FRAMEXLO", 15.0, true, xl, xu );
  graphLegendCharacteristics_->AddDistance( "FRAMEYLO", 70.0, true, yl, yu );
  graphLegendCharacteristics_->AddDistance( "FRAMEXHI", 50.0, true, xl, xu );
  graphLegendCharacteristics_->AddDistance( "FRAMEYHI", 90.0, true, yl, yu );
  graphLegendCharacteristics_->AddColor( "FRAMECOLOR", GRA_colorControl::GetColor("BLACK") );
  graphLegendCharacteristics_->AddBool( "TRANSPARENCY", true );
  graphLegendCharacteristics_->AddNumber( "SYMBOLS", 1 );
  graphLegendCharacteristics_->AddBool( "TITLEON", true );
  graphLegendCharacteristics_->AddString( "TITLE", ExString("") );
  graphLegendCharacteristics_->AddSize( "TITLEHEIGHT", 3.0, true, yl, yu );
  graphLegendCharacteristics_->AddColor( "TITLECOLOR", GRA_colorControl::GetColor("BLACK") );
  graphLegendCharacteristics_->AddFont( "TITLEFONT", GRA_fontControl::GetFont("ARIAL") );
}

void GRA_window::SetGraphLegendDefaults()
{
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get("ON"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get("ENTRYLINEON"))->Set( true );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get("FRAMEON"))->Set( true );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get("FRAMEXLO"))->SetAsPercent( 15.0 );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get("FRAMEYLO"))->SetAsPercent( 70.0 );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get("FRAMEXHI"))->SetAsPercent( 50.0 );
  static_cast<GRA_distanceCharacteristic*>(graphLegendCharacteristics_->Get("FRAMEYHI"))->SetAsPercent( 90.0 );
  static_cast<GRA_colorCharacteristic*>(graphLegendCharacteristics_->Get("FRAMECOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get("TRANSPARENCY"))->Set( true );
  static_cast<GRA_intCharacteristic*>(graphLegendCharacteristics_->Get("SYMBOLS"))->Set( 1 );
  static_cast<GRA_boolCharacteristic*>(graphLegendCharacteristics_->Get("TITLEON"))->Set( true );
  static_cast<GRA_stringCharacteristic*>(graphLegendCharacteristics_->Get("TITLE"))->Set( ExString("") );
  static_cast<GRA_sizeCharacteristic*>(graphLegendCharacteristics_->Get("TITLEHEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_colorCharacteristic*>(graphLegendCharacteristics_->Get("TITLECOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(graphLegendCharacteristics_->Get("TITLEFONT"))->Set( GRA_fontControl::GetFont("ARIAL") );
}

void GRA_window::CreateDataCurveCharacteristics( double yl, double yu )
{
  dataCurveCharacteristics_ = new GRA_setOfCharacteristics();
  //
  dataCurveCharacteristics_->AddNumber( "HISTOGRAMTYPE", 0 );
  dataCurveCharacteristics_->AddColor( "CURVECOLOR", GRA_colorControl::GetColor("BLACK") );
  dataCurveCharacteristics_->AddNumber( "CURVELINEWIDTH", 1 );
  dataCurveCharacteristics_->AddNumber( "CURVELINETYPE", 1 );
  dataCurveCharacteristics_->AddSize( "PLOTSYMBOLSIZE", 2.0, true, yl, yu );
  dataCurveCharacteristics_->AddAngle( "PLOTSYMBOLANGLE", 0.0 );
  dataCurveCharacteristics_->AddColor( "PLOTSYMBOLCOLOR", GRA_colorControl::GetColor("BLACK") );
  dataCurveCharacteristics_->AddNumber( "PLOTSYMBOLLINEWIDTH", 1 );
  dataCurveCharacteristics_->AddNumber( "PLOTSYMBOL", 0 );
  dataCurveCharacteristics_->AddBool( "CLIP", true );
}

void GRA_window::SetDataCurveDefaults()
{
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get("HISTOGRAMTYPE"))->Set( 0 );
  static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("CURVECOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get("CURVELINEWIDTH"))->Set( 1 );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get("CURVELINETYPE"))->Set( 1 );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get("PLOTSYMBOL"))->Set( 0 );
  static_cast<GRA_sizeCharacteristic*>(dataCurveCharacteristics_->Get("PLOTSYMBOLSIZE"))->SetAsPercent( 2.0 );
  static_cast<GRA_angleCharacteristic*>(dataCurveCharacteristics_->Get("PLOTSYMBOLANGLE"))->Set( 0.0 );
  static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("PLOTSYMBOLCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_intCharacteristic*>(dataCurveCharacteristics_->Get("PLOTSYMBOLLINEWIDTH"))->Set( 1 );
  static_cast<GRA_boolCharacteristic*>(dataCurveCharacteristics_->Get("CLIP"))->Set( true );
}

void GRA_window::CreateTextCharacteristics( double xl, double yl, double xu, double yu )
{
  textCharacteristics_ = new GRA_setOfCharacteristics();
  //
  textCharacteristics_->AddNumber( "ALIGNMENT", 1 );
  textCharacteristics_->AddAngle( "ANGLE", 0.0 );
  textCharacteristics_->AddColor( "COLOR", GRA_colorControl::GetColor("BLACK") );
  textCharacteristics_->AddFont( "FONT", GRA_fontControl::GetFont("ARIAL") );
  textCharacteristics_->AddSize( "HEIGHT", 3.0, true, yl, yu );
  textCharacteristics_->AddBool( "INTERACTIVE", true );
  textCharacteristics_->AddDistance( "XLOCATION", 50.0, true, xl, xu );
  textCharacteristics_->AddDistance( "YLOCATION", 50.0, true, yl, yu );
}

void GRA_window::SetTextDefaults()
{
  static_cast<GRA_intCharacteristic*>(textCharacteristics_->Get("ALIGNMENT"))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(textCharacteristics_->Get("ANGLE"))->Set( 0.0 );
  static_cast<GRA_colorCharacteristic*>(textCharacteristics_->Get("COLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(textCharacteristics_->Get("FONT"))->Set( GRA_fontControl::GetFont("ARIAL") );
  static_cast<GRA_sizeCharacteristic*>(textCharacteristics_->Get("HEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_boolCharacteristic*>(textCharacteristics_->Get("INTERACTIVE"))->Set( true );
  static_cast<GRA_distanceCharacteristic*>(textCharacteristics_->Get("XLOCATION"))->SetAsPercent( 50.0 );
  static_cast<GRA_distanceCharacteristic*>(textCharacteristics_->Get("YLOCATION"))->SetAsPercent( 50.0 );
}

void GRA_window::CreatePolarCharacteristics( double xl, double yl, double xu, double yu )
{
  polarCharacteristics_ = new GRA_setOfCharacteristics();
  //
  polarCharacteristics_->AddDistance( "XORIGIN", 50.0, true, xl, xu );
  polarCharacteristics_->AddDistance( "YORIGIN", 50.0, true, yl, yu );
  polarCharacteristics_->AddNumber( "NAXES", 4 );
  polarCharacteristics_->AddSize( "AXISLENGTH", 30.0, true, xl, xu );
  polarCharacteristics_->AddBool( "AXISON", true );
  polarCharacteristics_->AddColor( "AXISCOLOR", GRA_colorControl::GetColor("BLACK") );
  polarCharacteristics_->AddBool( "CLOCKWISE", false );
  polarCharacteristics_->AddNumber( "LINEWIDTH", 1 );
  polarCharacteristics_->AddAngle( "AXISANGLE", 0.0 );
  polarCharacteristics_->AddBool( "COMPASSLABELS", false );
  polarCharacteristics_->AddNumber( "MIN", 0.0 );
  polarCharacteristics_->AddNumber( "MAX", 1.0 );
  polarCharacteristics_->AddNumber( "VIRTUALMIN", 0.0 );
  polarCharacteristics_->AddNumber( "VIRTUALMAX", 1.0 );
  polarCharacteristics_->AddNumber( "LOGBASE", 0.0 );
  polarCharacteristics_->AddBool( "LOGSTYLE", true );
  polarCharacteristics_->AddNumber( "NLINCS", 0 );
  polarCharacteristics_->AddNumber( "NSINCS", 0 );
  polarCharacteristics_->AddNumber( "GRID", 0 );
  polarCharacteristics_->AddString( "LABEL", ExString("") );
  polarCharacteristics_->AddBool( "LABELON", false );
  polarCharacteristics_->AddColor( "LABELCOLOR", GRA_colorControl::GetColor("BLACK") );
  polarCharacteristics_->AddFont( "LABELFONT", GRA_fontControl::GetFont("ARIAL") );
  polarCharacteristics_->AddSize( "LABELHEIGHT", 3.0, true, yl, yu );
  polarCharacteristics_->AddBool( "NUMBERSON", true );
  polarCharacteristics_->AddColor( "NUMBERSCOLOR", GRA_colorControl::GetColor("BLACK") );
  polarCharacteristics_->AddFont( "NUMBERSFONT", GRA_fontControl::GetFont("ARIAL") );
  polarCharacteristics_->AddSize( "NUMBERSHEIGHT", 3.0, true, yl, yu );
  polarCharacteristics_->AddAngle( "NUMBERSANGLE", 0.0 );
  polarCharacteristics_->AddNumber( "NUMBEROFDIGITS", 7 );
  polarCharacteristics_->AddNumber( "NUMBEROFDECIMALS", -1 );
  polarCharacteristics_->AddNumber( "MOD", 0.0 );
  polarCharacteristics_->AddBool( "LEADINGZEROS", false );
  polarCharacteristics_->AddNumber( "OFFSET", 0.0 );
  polarCharacteristics_->AddBool( "DROPFIRSTNUMBER", false );
  polarCharacteristics_->AddBool( "DROPLASTNUMBER", false );
  polarCharacteristics_->AddBool( "TICSON", true );
  polarCharacteristics_->AddBool( "TICSBOTHSIDES", false );
  polarCharacteristics_->AddAngle( "TICANGLE", 90.0 );
  polarCharacteristics_->AddSize( "LARGETICLENGTH", 2.0, true, yl, yu );
  polarCharacteristics_->AddSize( "SMALLTICLENGTH", 1.0, true, yl, yu );
  polarCharacteristics_->AddAngle( "IMAGTICANGLE", 270.0 );
  polarCharacteristics_->AddSize( "IMAGTICLENGTH", 1.0, true, yl, yu );
  polarCharacteristics_->AddNumber( "POWER", 0.0 );
  polarCharacteristics_->AddNumber( "POWERAUTO", 1 );
}

void GRA_window::SetPolarDefaults()
{
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("NAXES"))->Set( 4 );
  static_cast<GRA_distanceCharacteristic*>(polarCharacteristics_->Get("XORIGIN"))->SetAsPercent( 50.0 );
  static_cast<GRA_distanceCharacteristic*>(polarCharacteristics_->Get("YORIGIN"))->SetAsPercent( 50.0 );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("AXISLENGTH"))->SetAsPercent( 30.0 );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("AXISON"))->Set( true );
  static_cast<GRA_colorCharacteristic*>(polarCharacteristics_->Get("AXISCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("CLOCKWISE"))->Set( false );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("LINEWIDTH"))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(polarCharacteristics_->Get("AXISANGLE"))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("COMPASSLABELS"))->Set( false );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("MIN"))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("MAX"))->Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("VIRTUALMIN"))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("VIRTUALMAX"))->Set( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("LOGBASE"))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("LOGSTYLE"))->Set( true );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("NLINCS"))->Set( 0 );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("NSINCS"))->Set( 0 );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("GRID"))->Set( 0 );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("LABELON"))->Set( false );
  static_cast<GRA_stringCharacteristic*>(polarCharacteristics_->Get("LABEL"))->Set( ExString("") );
  static_cast<GRA_colorCharacteristic*>(polarCharacteristics_->Get("LABELCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(polarCharacteristics_->Get("LABELFONT"))->Set( GRA_fontControl::GetFont("ARIAL") );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("LABELHEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("NUMBERSON"))->Set( true );
  static_cast<GRA_colorCharacteristic*>(polarCharacteristics_->Get("NUMBERSCOLOR"))->Set( GRA_colorControl::GetColor("BLACK") );
  static_cast<GRA_fontCharacteristic*>(polarCharacteristics_->Get("NUMBERSFONT"))->Set( GRA_fontControl::GetFont("ARIAL") );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("NUMBERSHEIGHT"))->SetAsPercent( 3.0 );
  static_cast<GRA_angleCharacteristic*>(polarCharacteristics_->Get("NUMBERSANGLE"))->Set( 0.0 );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("NUMBEROFDIGITS"))->Set( 7 );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("NUMBEROFDECIMALS"))->Set( -1 );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("MOD"))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("LEADINGZEROS"))->Set( false );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("OFFSET"))->Set( 0.0 );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("DROPFIRSTNUMBER"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("DROPLASTNUMBER"))->Set( false );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("TICSON"))->Set( true );
  static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("TICSBOTHSIDES"))->Set( false );
  static_cast<GRA_angleCharacteristic*>(polarCharacteristics_->Get("TICANGLE"))->Set( 90.0 );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("LARGETICLENGTH"))->SetAsPercent( 2.0 );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("SMALLTICLENGTH"))->SetAsPercent( 1.0 );
  static_cast<GRA_angleCharacteristic*>(polarCharacteristics_->Get("IMAGTICANGLE"))->Set( 270.0 );
  static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("IMAGTICLENGTH"))->SetAsPercent( 1.0 );
  static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("POWER"))->Set( 0.0 );
  static_cast<GRA_intCharacteristic*>(polarCharacteristics_->Get("POWERAUTO"))->Set( 1 );
}

void GRA_window::WorldToPercent( double x, double y, double &xout, double &yout ) const
{
  // convert x and y from world units to percentages of the window
  //
  double xlwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("XLOWERWINDOW"))->GetAsWorld();
  double xuwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("XUPPERWINDOW"))->GetAsWorld();
  double ylwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("YLOWERWINDOW"))->GetAsWorld();
  double yuwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("YUPPERWINDOW"))->GetAsWorld();
  xout = 100.*(x-xlwind)/(xuwind-xlwind);
  yout = 100.*(y-ylwind)/(yuwind-ylwind);
}

void GRA_window::PercentToWorld( double x, double y, double &xout, double &yout ) const
{
  // convert x and y from percentages of the window to world
  //
  double xlwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("XLOWERWINDOW"))->GetAsWorld();
  double xuwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("XUPPERWINDOW"))->GetAsWorld();
  double ylwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("YLOWERWINDOW"))->GetAsWorld();
  double yuwind = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("YUPPERWINDOW"))->GetAsWorld();
  xout = x*(xuwind-xlwind)/100. + xlwind;
  yout = y*(yuwind-ylwind)/100. + ylwind;
}

void GRA_window::GraphToWorld( double xg, double yg, double &xw, double &yw, bool reset ) const
{
  // convert from graph units to world units
  //
  static double xlog, ylog, xlaxis, ylaxis, cosx, sinx, cosy, siny, ax, bx, ay, by, xmin, ymin;
  if( reset )
    GetValues( xlog, ylog, xlaxis, ylaxis, cosx, sinx, cosy, siny, ax, bx, ay, by, xmin, ymin );
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

void GRA_window::PolarToWorld( double r, double theta, double &xw, double &yw, bool reset ) const
{
  // convert from polar graph units to world units
  //
  static double xorigin, yorigin, axisa, a;
  static bool clockwise;
  if( reset )
  {
    axisa = static_cast<GRA_angleCharacteristic*>(polarCharacteristics_->Get("AXISANGLE"))->Get();
    xorigin = static_cast<GRA_distanceCharacteristic*>(polarCharacteristics_->Get("XORIGIN"))->GetAsWorld();
    yorigin = static_cast<GRA_distanceCharacteristic*>(polarCharacteristics_->Get("YORIGIN"))->GetAsWorld();
    double const length =
      static_cast<GRA_sizeCharacteristic*>(polarCharacteristics_->Get("AXISLENGTH"))->GetAsWorld();
    double const max = static_cast<GRA_doubleCharacteristic*>(polarCharacteristics_->Get("MAX"))->Get();
    a = max!=0.0 ? length/max : 1.0;
    clockwise = static_cast<GRA_boolCharacteristic*>(polarCharacteristics_->Get("CLOCKWISE"))->Get();
  }
  if( clockwise )theta *= -1.0;
  double angle = (theta+axisa)*M_PI/180.0;
  xw = xorigin + a*r*cos(angle);
  yw = yorigin + a*r*sin(angle);
}

void GRA_window::PolarToWorld( std::vector<double> const &r, std::vector<double> const &theta,
                               std::vector<double> &xw, std::vector<double> &yw ) const
{
  std::size_t const size = r.size();
  xw.resize( size );
  yw.resize( size );
  for( std::size_t i=0; i<size; ++i )PolarToWorld( r[i], theta[i], xw[i], yw[i], false );
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
      throw EGraphicsError("problem with x-axis log scales");
    xg = exp(xg*log(xlog));
  }
  if( static_cast<int>(ylog) > 1 )
  {
    if( yg*log(ylog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError("problem with y-axis log scales");
    yg = exp(yg*log(ylog));
  }
}

void GRA_window::GetValues( double &xlog, double &ylog, double &xlaxis, double &ylaxis,
                            double &cosx, double &sinx, double &cosy, double &siny,
                            double &ax, double &bx, double &ay, double &by,
                            double &xmin, double &ymin ) const
{
  xlog = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("LOGBASE"))->Get();
  ylog = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("LOGBASE"))->Get();
  xmin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MIN"))->Get();
  double const xmax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MAX"))->Get();
  ymin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MIN"))->Get();
  double const ymax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MAX"))->Get();
  double const xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get("UPPERAXIS"))->GetAsWorld();
  xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisCharacteristics_->Get("LOWERAXIS"))->GetAsWorld();
  double const yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get("UPPERAXIS"))->GetAsWorld();
  ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisCharacteristics_->Get("LOWERAXIS"))->GetAsWorld();
  double const degreesToRadians = M_PI/180.;
  double const xaxisa = static_cast<GRA_angleCharacteristic*>(xAxisCharacteristics_->Get("AXISANGLE"))->Get();
  double const yaxisa = static_cast<GRA_angleCharacteristic*>(yAxisCharacteristics_->Get("AXISANGLE"))->Get();
  cosx = cos( xaxisa*degreesToRadians );
  cosy = cos( yaxisa*degreesToRadians );
  sinx = sin( xaxisa*degreesToRadians );
  siny = sin( yaxisa*degreesToRadians );
  double const eps = 0.000001;
  if( fabs(cosx) < eps )cosx = 0.0;
  if( fabs(cosy) < eps )cosy = 0.0;
  if( fabs(sinx) < eps )sinx = 0.0;
  if( fabs(siny) < eps )siny = 0.0;
  ax = xmax!=xmin ? (xuaxis-xlaxis)/(xmax-xmin) : 1.0;
  ay = ymax!=ymin ? (yuaxis-ylaxis)/(ymax-ymin) : 1.0;
  bx = -ax*xmin;
  by = -ay*ymin;
}

bool GRA_window::InsideWindow( double x, double y ) const
{
  if( !drawableObjects_.empty() )
  {
    double xlw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("XLOWERWINDOW"))->GetAsWorld();
    double ylw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("YLOWERWINDOW"))->GetAsWorld();
    double xuw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("XUPPERWINDOW"))->GetAsWorld();
    double yuw = static_cast<GRA_distanceCharacteristic*>(generalCharacteristics_->Get("YUPPERWINDOW"))->GetAsWorld();
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
  double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MIN"))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MAX"))->Get();
  double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MIN"))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MAX"))->Get();
  bool clip = static_cast<GRA_boolCharacteristic*>(dataCurveCharacteristics_->Get("CLIP"))->Get();
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
  double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MIN"))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MAX"))->Get();
  double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MIN"))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MAX"))->Get();
  bool clip = static_cast<GRA_boolCharacteristic*>(dataCurveCharacteristics_->Get("CLIP"))->Get();
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

void GRA_window::DisplayBackground( GRA_outputType *graphicsOutput )
{
  double xminw, yminw, xmaxw, ymaxw;
  ExGlobals::GetWorldLimits( xminw, yminw, xmaxw, ymaxw );
  double xl = xminw+xLoP_*(xmaxw-xminw)/100.0;
  double yl = yminw+yLoP_*(ymaxw-yminw)/100.0;
  double xu = xminw+xHiP_*(xmaxw-xminw)/100.0;
  double yu = yminw+yHiP_*(ymaxw-yminw)/100.0;
  GRA_color *bgcolor =
   static_cast<GRA_colorCharacteristic*>(generalCharacteristics_->Get("BACKGROUNDCOLOR"))->Get();
  ExString bgfile(
   static_cast<GRA_stringCharacteristic*>(generalCharacteristics_->Get("BACKGROUNDFILE"))->Get() );
  ExGlobals::SetClippingBoundary( xl, yl, xu, yu );
  if( bgcolor )
  {
    std::auto_ptr<GRA_rectangle> rect( new GRA_rectangle(xl,yl,xu,yu,0.0,false,bgcolor,bgcolor,1) );
    rect->Draw( graphicsOutput );
  }
  else if( !bgfile.empty() )
  {
    FILE *f = fopen(bgfile.c_str(),"rb");
    gdImagePtr im_in = gdImageCreateFromPng( f );
    fclose(f);
    //
    int h = gdImageSY(im_in);
    int w = gdImageSX(im_in);
    double xl1, yl1, xu1, yu1;
    graphicsOutput->WorldToOutputType( xl, yl, xl1, yl1 );
    graphicsOutput->WorldToOutputType( xu, yu, xu1, yu1 );
    //
    int dx = abs(int(xu1-xl1));
    int dy = abs(int(yu1-yl1));
    gdImagePtr im_out = gdImageCreateTrueColor( dx, dy );
    gdImageCopyResized( im_out, im_in, 0, 0, 0, 0, dx, dy, w, h );
    //
    GRA_bitmap *bitmap =
      new GRA_bitmap(xl,yl,xu,yu,GRA_colorControl::GetColor("WHITE"),graphicsOutput);
    std::vector<GRA_color *> tempColors;
    for( int j=0; j<dy; ++j )
    {
      for( int i=0; i<dx; ++i )
      {
        int c = gdImageGetPixel( im_out, i, dy-1-j );
        int r = gdImageRed(im_out,c);
        int g = gdImageGreen(im_out,c);
        int b = gdImageBlue(im_out,c);
        GRA_color *color = new GRA_color(r,g,b);
        bitmap->SetPixelColor( i, j, color );
        tempColors.push_back( color );
      }
    }
    gdImageDestroy( im_in );
    gdImageDestroy( im_out );
    graphicsOutput->Draw( bitmap );
    for( int i=0; i<dx*dy; ++i )delete tempColors[i];
    delete bitmap;
  }
  ExGlobals::ResetClippingBoundary();
}

void GRA_window::Draw( GRA_outputType *graphicsOutput )
{
  DisplayBackground( graphicsOutput );
  try
  {
    drawableVecIter end = drawableObjects_.end();
    for( drawableVecIter i=drawableObjects_.begin(); i!=end; ++i )
      (*i)->Draw( graphicsOutput );
  }
  catch ( EGraphicsError &e )
  {
    throw;
  }
}

void GRA_window::DrawLastTextString( GRA_outputType *outputType )
{
  drawableVecRIter rend = drawableObjects_.rend();
  for( drawableVecRIter i=drawableObjects_.rbegin(); i!=rend; ++i )
  {
    if( (*i)->IsaDrawableText() )
    {
      (*i)->Draw( outputType );
      break;
    }
  }
}

void GRA_window::RemoveLastTextString()
{
  for( int i=static_cast<int>(drawableObjects_.size())-1; i>=0; --i )
  {
    if( drawableObjects_[i]->IsaDrawableText() )
    {
      ExGlobals::GetScreenOutput()->Erase( static_cast<GRA_drawableText*>(drawableObjects_[i]) );
      drawableObjects_.erase( drawableObjects_.begin()+i );
      break;
    }
  }
} 

GRA_drawableText *GRA_window::GetLastDrawableText()
{
  GRA_drawableText *result = 0;
  for( int i=static_cast<int>(drawableObjects_.size())-1; i>=0; --i )
  {
    if( drawableObjects_[i]->IsaDrawableText() )
    {
      result = static_cast<GRA_drawableText*>(drawableObjects_[i]);
      break;
    }
  }
  return result;
}

void GRA_window::DrawZerolines( bool horizontal, bool vertical )
{
  GRA_outputType *monitor = ExGlobals::GetScreenOutput();
  double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MIN"))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("MAX"))->Get();
  double xlog = static_cast<GRA_doubleCharacteristic*>(xAxisCharacteristics_->Get("LOGBASE"))->Get();
  double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MIN"))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("MAX"))->Get();
  double ylog = static_cast<GRA_doubleCharacteristic*>(yAxisCharacteristics_->Get("LOGBASE"))->Get();
  //
  if( xlog > 1.0 )
  {
    if( xmin*log(xlog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError( "problem with x-axis log scales min" );
    if( xmax*log(xlog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError( "problem with x-axis log scales max");
    xmin = pow(xlog,xmin);
    xmax = pow(xlog,xmax);
  }
  if( ylog > 1.0 )
  {
    if( ymin*log(ylog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError( "problem with y-axis log scales min");
    if( ymax*log(ylog) > log(std::numeric_limits<double>::max()) )
      throw EGraphicsError( "problem with y-axis log scales max");
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
  if( horizontal )
  {
    GRA_color *axisColor =
     static_cast<GRA_colorCharacteristic*>(xAxisCharacteristics_->Get("AXISCOLOR"))->Get();
    GRA_color *curveColorSave =
     static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("CURVECOLOR"))->Get();
    static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("CURVECOLOR"))->Set( axisColor );
    GRA_cartesianCurve *cartesianCurve =
        new GRA_cartesianCurve(xTmp,zTmp,xe1,ye1,xe2,ye2);
    if( !cartesianCurve )throw EGraphicsError( "could not make new horizontal zero line" );
    try
    {
      cartesianCurve->Make();
    }
    catch (EGraphicsError &e)
    {
      static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("CURVECOLOR"))->Set( curveColorSave );
      delete cartesianCurve;
      throw;
    }
    cartesianCurve->Draw( monitor );
    AddDrawableObject( cartesianCurve );
    static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("CURVECOLOR"))->Set( curveColorSave );
  }
  if( vertical )
  {
    GRA_color *axisColor =
     static_cast<GRA_colorCharacteristic*>(yAxisCharacteristics_->Get("AXISCOLOR"))->Get();
    GRA_color *curveColorSave =
     static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("CURVECOLOR"))->Get();
    static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("CURVECOLOR"))->Set( axisColor );
    GRA_cartesianCurve *cartesianCurve =
        new GRA_cartesianCurve(zTmp,yTmp,xe1,ye1,xe2,ye2);
    if( !cartesianCurve )throw EGraphicsError( "could not make new vertical zero line" );
    try
    {
      cartesianCurve->Make();
    }
    catch (EGraphicsError &e)
    {
      static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("CURVECOLOR"))->Set( curveColorSave );
      delete cartesianCurve;
      throw;
    }
    cartesianCurve->Draw( monitor );
    AddDrawableObject( cartesianCurve );
    static_cast<GRA_colorCharacteristic*>(dataCurveCharacteristics_->Get("CURVECOLOR"))->Set( curveColorSave );
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
      << "<polarc>\n" << *gw.polarCharacteristics_ << "</polarc>\n"
      << "<drawableobjects size=\"" << gw.drawableObjects_.size() << "\">\n";
  GRA_window::drawableVecIter end = gw.drawableObjects_.end();
  for( GRA_window::drawableVecIter i=gw.drawableObjects_.begin(); i!=end; ++i )
  {
    if( (*i)->IsaPolyline() )out << *(static_cast<GRA_polyline*>(*i));
    else if( (*i)->IsaDrawableText() )out << *(static_cast<GRA_drawableText*>(*i));
    else if( (*i)->IsaCartesianAxes() )out << *(static_cast<GRA_cartesianAxes*>(*i));
    else if( (*i)->IsaCartesianCurve() )out << *(static_cast<GRA_cartesianCurve*>(*i));
    else if( (*i)->IsaGraphLegend() )out << *(static_cast<GRA_legend*>(*i));
    else if( (*i)->IsaPoint() )out << *(static_cast<GRA_point*>(*i));
    else if( (*i)->IsaPolyline() )out << *(static_cast<GRA_polyline*>(*i));
    else if( (*i)->IsaEllipse() )out << *(static_cast<GRA_ellipse*>(*i));
    else if( (*i)->IsaPolygon() )out << *(static_cast<GRA_polygon*>(*i));
    else if( (*i)->IsaMultilineFigure() )out << *(static_cast<GRA_multiLineFigure*>(*i));
    else if( (*i)->IsaPlotsymbol() )out << *(static_cast<GRA_plotSymbol*>(*i));
    else if( (*i)->IsaContour() )out << *(static_cast<GRA_contourLine*>(*i));
    else if( (*i)->IsaBoxPlot() )out << *(static_cast<GRA_boxPlot*>(*i));
    else if( (*i)->IsaDitheringPlot() )out << *(static_cast<GRA_ditheringPlot*>(*i));
    else if( (*i)->IsaDiffusionPlot() )out << *(static_cast<GRA_diffusionPlot*>(*i));
    else if( (*i)->IsaGradientPlot() )out << *(static_cast<GRA_gradientPlot*>(*i));
    //else if( (*i)->IsaThreeDFigure() )out << *(static_cast<GRA_threeDFigure*>(*i));
  }
  return out << "</drawableobjects>\n</graphwindow>\n";
}

// end of file

