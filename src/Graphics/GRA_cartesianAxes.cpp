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
#include <stdexcept>

#include "GRA_cartesianAxes.h"
#include "EGraphicsError.h"
#include "GRA_axis.h"
#include "GRA_window.h"
#include "GRA_wxWidgets.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"

GRA_cartesianAxes::GRA_cartesianAxes( std::vector<double> const &x, std::vector<double> const &y,
                                      bool xOnTop, bool yOnRight )
  : GRA_drawableObject(wxT("CARTESIANAXES")), xOnTop_(xOnTop), yOnRight_(yOnRight)
{
  try
  {
    AutoScale( x, y );
  }
  catch ( EGraphicsError &e )
  {
    throw;
  }
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  //
  double xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  double ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  double xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  double yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  bool graphBox = static_cast<GRA_boolCharacteristic*>(generalC->Get(wxT("GRAPHBOX")))->Get();
  //
  GRA_setOfCharacteristics xCharacteristics( *xAxisC );
  GRA_setOfCharacteristics yCharacteristics( *yAxisC );
  double xlwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
  double xuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
  double ylwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("YLOWERWINDOW")))->GetAsWorld();
  double yuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("YUPPERWINDOW")))->GetAsWorld();
  double xminw, yminw, xmaxw, ymaxw;
  ExGlobals::GetWorldLimits( xminw, yminw, xmaxw, ymaxw );
  xCharacteristics.AddDistance( wxT("XLOWERWINDOW"), xlwind, false, xminw, xmaxw );
  xCharacteristics.AddDistance( wxT("YLOWERWINDOW"), ylwind, false, yminw, ymaxw );
  xCharacteristics.AddDistance( wxT("XUPPERWINDOW"), xuwind, false, xminw, xmaxw );
  xCharacteristics.AddDistance( wxT("YUPPERWINDOW"), yuwind, false, yminw, ymaxw );
  yCharacteristics.AddDistance( wxT("XLOWERWINDOW"), xlwind, false, xminw, xmaxw );
  yCharacteristics.AddDistance( wxT("YLOWERWINDOW"), ylwind, false, yminw, ymaxw );
  yCharacteristics.AddDistance( wxT("XUPPERWINDOW"), xuwind, false, xminw, xmaxw );
  yCharacteristics.AddDistance( wxT("YUPPERWINDOW"), yuwind, false, yminw, ymaxw );
  xCharacteristics.AddDistance(wxT("OLAXIS"),ylaxis,false,ylwind,yuwind);
  xCharacteristics.AddDistance(wxT("OUAXIS"),yuaxis,false,ylwind,yuwind);
  yCharacteristics.AddDistance(wxT("OLAXIS"),xlaxis,false,xlwind,xuwind);
  yCharacteristics.AddDistance(wxT("OUAXIS"),xuaxis,false,xlwind,xuwind);
  int gridLineType = static_cast<GRA_intCharacteristic*>(generalC->Get(wxT("GRIDLINETYPE")))->Get();
  xCharacteristics.AddNumber(wxT("GRIDLINETYPE"),gridLineType);
  yCharacteristics.AddNumber(wxT("GRIDLINETYPE"),gridLineType);
  //
  boxXAxis_ = 0;
  boxYAxis_ = 0;
  if( graphBox )
  {
    GRA_setOfCharacteristics boxSettings( xCharacteristics );
    static_cast<GRA_intCharacteristic*>(boxSettings.Get(wxT("GRID")))->Set(0);
    static_cast<GRA_boolCharacteristic*>(boxSettings.Get(wxT("LABELON")))->Set(false);
    static_cast<GRA_boolCharacteristic*>(boxSettings.Get(wxT("NUMBERSON")))->Set(false);
    //
    if( xOnTop )boxXAxis_ = new GRA_axis( xlaxis, ylaxis, &boxSettings );
    else
    {
      double angle = static_cast<GRA_angleCharacteristic*>(xAxisC->Get(wxT("TICANGLE")))->Get();
      static_cast<GRA_angleCharacteristic*>(boxSettings.Get(wxT("TICANGLE")))->Set(-angle);
      boxXAxis_ = new GRA_axis( xlaxis, yuaxis, &boxSettings );
    }
    boxSettings = yCharacteristics;
    static_cast<GRA_intCharacteristic*>(boxSettings.Get(wxT("GRID")))->Set(0);
    static_cast<GRA_boolCharacteristic*>(boxSettings.Get(wxT("LABELON")))->Set(false);
    static_cast<GRA_boolCharacteristic*>(boxSettings.Get(wxT("NUMBERSON")))->Set(false);
    if( yOnRight )boxYAxis_ = new GRA_axis( xlaxis, ylaxis, &boxSettings );
    else
    {
      double angle = static_cast<GRA_angleCharacteristic*>(yAxisC->Get(wxT("TICANGLE")))->Get();
      static_cast<GRA_angleCharacteristic*>(boxSettings.Get(wxT("TICANGLE")))->Set(-angle);
      boxYAxis_ = new GRA_axis( xuaxis, ylaxis, &boxSettings );
    }
  }
  if( yOnRight )
  {
    double ytica = static_cast<GRA_angleCharacteristic*>(yCharacteristics.Get(wxT("TICANGLE")))->Get();
    double yitica = static_cast<GRA_angleCharacteristic*>(yCharacteristics.Get(wxT("IMAGTICANGLE")))->Get();
    static_cast<GRA_angleCharacteristic*>(yCharacteristics.Get(wxT("TICANGLE")))->Set(-ytica);
    static_cast<GRA_angleCharacteristic*>(yCharacteristics.Get(wxT("IMAGTICANGLE")))->Set(-yitica);
    yAxis_ = new GRA_axis( xuaxis, ylaxis, &yCharacteristics );
  }
  else
  {
    yAxis_ = new GRA_axis( xlaxis, ylaxis, &yCharacteristics );
  }
  if( xOnTop )
  {
    double xtica = static_cast<GRA_angleCharacteristic*>(xCharacteristics.Get(wxT("TICANGLE")))->Get();
    double xitica = static_cast<GRA_angleCharacteristic*>(xCharacteristics.Get(wxT("IMAGTICANGLE")))->Get();
    static_cast<GRA_angleCharacteristic*>(xCharacteristics.Get(wxT("TICANGLE")))->Set(-xtica);
    static_cast<GRA_angleCharacteristic*>(xCharacteristics.Get(wxT("IMAGTICANGLE")))->Set(-xitica);
    xAxis_ = new GRA_axis( xlaxis, yuaxis, &xCharacteristics );
  }
  else
  {
    xAxis_ = new GRA_axis( xlaxis, ylaxis, &xCharacteristics );
  }
}

void GRA_cartesianAxes::DeleteStuff()
{
  delete xAxis_; xAxis_=0;
  delete yAxis_; yAxis_=0;
  delete boxXAxis_; boxXAxis_=0;
  delete boxYAxis_; boxYAxis_=0;
}

void GRA_cartesianAxes::CopyStuff( GRA_cartesianAxes const &rhs )
{
  xOnTop_ = rhs.xOnTop_;
  yOnRight_ = rhs.yOnRight_;
  //
  if( rhs.xAxis_ )xAxis_ = new GRA_axis( *rhs.xAxis_ );
  if( rhs.yAxis_ )yAxis_ = new GRA_axis( *rhs.yAxis_ );
  if( rhs.boxXAxis_ )boxXAxis_ = new GRA_axis( *rhs.boxXAxis_ );
  if( rhs.boxYAxis_ )boxYAxis_ = new GRA_axis( *rhs.boxYAxis_ );
}

void GRA_cartesianAxes::Make()
{
  xAxis_->Make();
  yAxis_->Make();
  if( boxXAxis_ )boxXAxis_->Make();
  if( boxYAxis_ )boxYAxis_->Make();
}

void GRA_cartesianAxes::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  xAxis_->Draw( graphicsOutput, dc );
  yAxis_->Draw( graphicsOutput, dc );
  if( boxXAxis_ )boxXAxis_->Draw( graphicsOutput, dc );
  if( boxYAxis_ )boxYAxis_->Draw( graphicsOutput, dc );
}

void GRA_cartesianAxes::AutoScale( std::vector<double> const &x, std::vector<double> const &y )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  wxString autoScale(
   static_cast<GRA_stringCharacteristic*>(generalC->Get(wxT("AUTOSCALE")))->Get() );
  GRA_setOfCharacteristics *xCharacteristics = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yCharacteristics = gw->GetYAxisCharacteristics();
  int autoX, autoY;
  if( autoScale == wxT("OFF") )
  {
    autoX = 0;
    autoY = 0;
  }
  else if( autoScale == wxT("ON") )
  {
    autoX = 2;
    autoY = 2;
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set(7);
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDECIMALS")))->Set(-1);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set(7);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDECIMALS")))->Set(-1);
  }
  else if( autoScale == wxT("X") )
  {
    autoX = 2;
    autoY = 0;
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set(7);
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDECIMALS")))->Set(-1);
  }
  else if( autoScale == wxT("Y") )
  {
    autoX = 0;
    autoY = 2;
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set(7);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDECIMALS")))->Set(-1);
  }
  else if( autoScale == wxT("VIRTUAL") )
  {
    autoX = 3;
    autoY = 3;
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set(7);
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDECIMALS")))->Set(-1);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set(7);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDECIMALS")))->Set(-1);
  }
  else if( autoScale == wxT("XVIRTUAL") )
  {
    autoX = 3;
    autoY = 0;
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set(7);
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDECIMALS")))->Set(-1);
  }
  else if( autoScale == wxT("YVIRTUAL") )
  {
    autoX = 0;
    autoY = 3;
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set(7);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDECIMALS")))->Set(-1);
  }
  else if( autoScale == wxT("COMMENSURATE") )
  {
    double xlwind =
     static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XLOWERWINDOW")))->
      GetAsWorld();
    double ylwind =
     static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("YLOWERWINDOW")))->
      GetAsWorld();
    double xuwind =
     static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XUPPERWINDOW")))->
      GetAsWorld();
    double yuwind =
     static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("YUPPERWINDOW")))->
      GetAsWorld();
    autoX = 0;
    autoY = 0;
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
    double xmins, xmaxs, ymins, ymaxs;
    UsefulFunctions::MinMax( x, 0, x.size(), xmins, xmaxs );
    UsefulFunctions::MinMax( y, 0, y.size(), ymins, ymaxs );
    //
    double xmin, xmax, xinc, ymin, ymax, yinc;
    try
    {
      UsefulFunctions::Scale1( xmin, xmax, xinc, 5, xmins, xmaxs );
      UsefulFunctions::Scale1( ymin, ymax, yinc, 5, ymins, ymaxs );
    }
    catch ( std::runtime_error &e )
    {
      throw EGraphicsError( wxString(e.what(),wxConvUTF8) );
    }
    double xd = xmax - xmin;
    double yd = ymax - ymin;
    int nlxinc = static_cast<int>( xd/xinc + 0.5 );
    int nlyinc = static_cast<int>( yd/yinc + 0.5 );
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
    static_cast<GRA_distanceCharacteristic*>(xCharacteristics->Get(wxT("LOWERAXIS")))->SetAsPercent( xlp );
    static_cast<GRA_distanceCharacteristic*>(xCharacteristics->Get(wxT("UPPERAXIS")))->SetAsPercent( xup );
    static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("MAX")))->Set( xmax );
    static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("MIN")))->Set( xmin );
    static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("VIRTUALMAX")))->Set( xmax );
    static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("VIRTUALMIN")))->Set( xmin );
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NLINCS")))->Set( nlxinc );
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set( 7 );
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDECIMALS")))->Set( -1 );
    //
    static_cast<GRA_distanceCharacteristic*>(yCharacteristics->Get(wxT("LOWERAXIS")))->SetAsPercent( ylp );
    static_cast<GRA_distanceCharacteristic*>(yCharacteristics->Get(wxT("UPPERAXIS")))->SetAsPercent( yup );
    static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("MAX")))->Set( ymax );
    static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("MIN")))->Set( ymin );
    static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("VIRTUALMAX")))->Set( ymax );
    static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("VIRTUALMIN")))->Set( ymin );
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NLINCS")))->Set( nlyinc );
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set( 7 );
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDECIMALS")))->Set( -1 );
    //
    return;
  }
  int xnlinc = abs(static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NLINCS")))->Get());
  double xmin = static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("MIN")))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("MAX")))->Get();
  double xmins = static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("VIRTUALMIN")))->Get();
  double xmaxs = static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("VIRTUALMAX")))->Get();
  bool xzeroForce = static_cast<GRA_boolCharacteristic*>(xCharacteristics->Get(wxT("ZERO")))->Get();
  double xlogBase = static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("LOGBASE")))->Get();
  bool xlogStyle = static_cast<GRA_boolCharacteristic*>(xCharacteristics->Get(wxT("LOGSTYLE")))->Get();
  bool xAxisLinear = (static_cast<int>(xlogBase) <= 1);
  if( autoX==2 || autoX==3 )xnlinc = 0;
  if( xmin==xmax && autoX==0 )autoX = 2;
  bool xAxisScaleFixed = (autoX == 0);
  double xminValue = xmins;
  double xmaxValue = xmaxs;
  try
  {
    if( !xAxisLinear )GetActualValues( xminValue, xmaxValue, xlogBase );
  }
  catch ( EGraphicsError &e )
  {
    throw;
  }
  //
  int ynlinc = abs(static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NLINCS")))->Get());
  double ymin = static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("MIN")))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("MAX")))->Get();
  double ymins = static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("VIRTUALMIN")))->Get();
  double ymaxs = static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("VIRTUALMAX")))->Get();
  bool yzeroForce = static_cast<GRA_boolCharacteristic*>(yCharacteristics->Get(wxT("ZERO")))->Get();
  double ylogBase = static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("LOGBASE")))->Get();
  bool ylogStyle = static_cast<GRA_boolCharacteristic*>(yCharacteristics->Get(wxT("LOGSTYLE")))->Get();
  bool yAxisLinear = (static_cast<int>(ylogBase) <= 1);
  if( autoY==2 || autoY==3 )ynlinc = 0;
  if( ymin==ymax && autoY==0 )autoY = 2;
  bool yAxisScaleFixed = (autoY == 0);
  double yminValue = ymins;
  double ymaxValue = ymaxs;
  try
  {
    if( !yAxisLinear )GetActualValues( yminValue, ymaxValue, ylogBase );
  }
  catch ( EGraphicsError &e )
  {
    throw;
  }
  //
  std::size_t npt = x.size();
  double xinc, yinc;
  if( xAxisScaleFixed )
  {
    if( yAxisScaleFixed )
    {
      try
      {
        if( xAxisLinear && xnlinc==0 )
          SetNumberOfIncrements( xnlinc, xminValue, xmaxValue, xCharacteristics );
        if( yAxisLinear && ynlinc==0 )
          SetNumberOfIncrements( ynlinc, yminValue, ymaxValue, yCharacteristics );
      }
      catch ( EGraphicsError &e )
      {
        throw;
      }
    }
    else // x-axis is fixed, autoscale y-axis
    {
      try
      {
        if( xAxisLinear && xnlinc==0 )
          SetNumberOfIncrements( xnlinc, xminValue, xmaxValue, xCharacteristics );
      }
      catch ( EGraphicsError &e )
      {
        throw;
      }
      //
      // only consider data within the x-axis range
      //
      bool pointFound = false;
      for( std::size_t i=0; i<npt; ++i )
      {
        if( x[i]>=xmin && x[i]<=xmax )
        {
          pointFound = true;
          break;
        }
      }
      if( !pointFound ) // no data values within the x-axis range, cannot autoscale the y-axis
      {
        static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NLINCS")))->Set( 1 );
        static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("MIN")))->Set( 0.0 );
        static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("MAX")))->Set( 1.0 );
        static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("VIRTUALMIN")))->Set( 0.0 );
        static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get(wxT("VIRTUALMAX")))->Set( 1.0 );
        return;
      }
      yminValue = std::numeric_limits<double>::max();
      ymaxValue = -yminValue;
      for( std::size_t i=0; i<npt; ++i )
      {
        if( (x[i]-xmin)*(xmax-x[i]) >= 0.0 )
        {
          if( y[i] < yminValue )yminValue = y[i];
          if( y[i] > ymaxValue )ymaxValue = y[i];
        }
      }
      bool yVirtualMinMax = (autoY==3);
      FixupScales( yAxisLinear, yzeroForce, yminValue, ymaxValue, ylogBase,
                   ynlinc, ymins, ymaxs, yinc, yCharacteristics, yVirtualMinMax );
    }
  }
  else // autoscale x-axis
  {
    if( yAxisScaleFixed ) // autoscale x-axis, y-axis is fixed
    {
      try
      {
        if( yAxisLinear && ynlinc==0 )
          SetNumberOfIncrements( ynlinc, yminValue, ymaxValue, yCharacteristics );
      }
      catch ( EGraphicsError &e )
      {
        throw;
      }
      //
      // only consider data within the y-axis range
      //
      bool pointFound = false;
      for( std::size_t i=0; i<npt; ++i )
      {
        if( (y[i]-ymin)*(ymax-y[i]) >= 0.0 )
        {
          pointFound = true;
          break;
        }
      }
      if( !pointFound ) // no data values within the y-axis range
      {
        static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NLINCS")))->Set( 1 );
        static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("MIN")))->Set( 0.0 );
        static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("MAX")))->Set( 1.0 );
        static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("VIRTUALMIN")))->Set( 0.0 );
        static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get(wxT("VIRTUALMAX")))->Set( 1.0 );
        return;
      }
      xminValue = std::numeric_limits<double>::max();
      xmaxValue = -xminValue;
      for( std::size_t i=0; i<npt; ++i )
      {
        if( (y[i]-ymin)*(ymax-y[i]) >= 0.0 )
        {
          if( x[i] < xminValue )xminValue = x[i];
          if( x[i] > xmaxValue )xmaxValue = x[i];
        }
      }
      bool xVirtualMinMax = (autoX==3);
      FixupScales( xAxisLinear, xzeroForce, xminValue, xmaxValue, xlogBase,
                   xnlinc, xmins, xmaxs, xinc, xCharacteristics, xVirtualMinMax );
    }
    else // autoscale x-axis, autoscale y-axis
    {
      UsefulFunctions::MinMax( x, 0, npt, xminValue, xmaxValue );
      bool xVirtualMinMax = (autoX==3);
      FixupScales( xAxisLinear, xzeroForce, xminValue, xmaxValue, xlogBase,
                   xnlinc, xmins, xmaxs, xinc, xCharacteristics, xVirtualMinMax );
      UsefulFunctions::MinMax( y, 0, npt, yminValue, ymaxValue );
      bool yVirtualMinMax = (autoY==3);
      FixupScales( yAxisLinear, yzeroForce, yminValue, ymaxValue, ylogBase,
                   ynlinc, ymins, ymaxs, yinc, yCharacteristics, yVirtualMinMax );
    }
  }
  if( !xAxisLinear && xlogStyle )
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set(
      NumberOfDigits(xmins,xmaxs,xlogBase) );
  if( !yAxisLinear && ylogStyle )
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get(wxT("NUMBEROFDIGITS")))->Set(
      NumberOfDigits(ymins,ymaxs,ylogBase) );
}

void GRA_cartesianAxes::GetActualValues( double &minValue, double &maxValue, double logBase )
{
  try
  {
    ResetLogValue( minValue, logBase );
  }
  catch ( EGraphicsError &e )
  {
    throw EGraphicsError(wxT("problem with axis log scales minimum value"));
  }
  try
  {
    ResetLogValue( maxValue, logBase );
  }
  catch ( EGraphicsError &e )
  {
    throw EGraphicsError(wxT("problem with axis log scales maximum value"));
  }
}

void GRA_cartesianAxes::SetNumberOfIncrements( int &nlinc, double minValue, double maxValue,
                                               GRA_setOfCharacteristics *characteristics )
{
  nlinc = 5;
  double inc, mins, maxs;
  try
  {
    UsefulFunctions::Scale1( mins, maxs, inc, nlinc, minValue, maxValue );
  }
  catch ( std::runtime_error &e )
  {
    throw EGraphicsError( wxString(e.what(),wxConvUTF8) );
  }
  nlinc = static_cast<int>( (maxs-mins)/inc + 0.5 );
  static_cast<GRA_intCharacteristic*>(characteristics->Get(wxT("NLINCS")))->Set( nlinc );
  static_cast<GRA_doubleCharacteristic*>(characteristics->Get(wxT("VIRTUALMIN")))->Set( mins );
  static_cast<GRA_doubleCharacteristic*>(characteristics->Get(wxT("VIRTUALMAX")))->Set( maxs );
}

int GRA_cartesianAxes::NumberOfDigits( double mins, double maxs, double logBase )
{
  int ndig1p = 1;
  if( fabs(mins) > 0.0 )ndig1p = static_cast<int>(log10(fabs(mins)))+1;
  if( mins < 0.0 )++ndig1p;
  int ndig1b = static_cast<int>(log10(logBase))+1;
  int ndig1  = ndig1p + ndig1b;
  int ndig2p = 1;
  if( fabs(maxs) > 0.0 )ndig2p = static_cast<int>(log10(fabs(maxs)))+1;
  if( maxs < 0.0 )++ndig2p;
  int ndig2 = ndig2p + ndig1b;
  return std::max(ndig1,ndig2);
}

void GRA_cartesianAxes::ResetLogValue( double &value, double logBase )
{
  if( value*log(logBase) > log(std::numeric_limits<double>::max()) )throw EGraphicsError(wxT(" "));
  try
  {
    value = exp(value*log(logBase));
  }
  catch (...)
  {
    throw EGraphicsError(wxT(" "));
  }
}

void GRA_cartesianAxes::FixupScales( bool linear, bool zeroForce, double minValue, double maxValue,
                                     double logBase, int &nlinc, double &mins, double &maxs, double &inc,
                                     GRA_setOfCharacteristics *characteristics, bool virtualMinMax )
{
  if( linear ) // axis is linear
  {
    if( minValue == 0.0 )minValue = -0.01;
    else minValue -= 0.01*fabs(minValue);
    if( maxValue == 0.0 )maxValue = 0.01;
    else maxValue += 0.01*fabs(maxValue);
    if( zeroForce ) // force zero to be displayed on a linear axis
    {
      if( minValue > 0.0 )minValue = 0.0;
      if( maxValue < 0.0 )maxValue = 0.0;
    }
    if( nlinc == 0 )
    {
      nlinc = 5;
      try
      {
        UsefulFunctions::Scale1( mins, maxs, inc, nlinc, minValue, maxValue );
      }
      catch ( std::runtime_error &e )
      {
        throw EGraphicsError( wxString(e.what(),wxConvUTF8) );
      }
      nlinc = static_cast<int>( (maxs-mins)/inc + 0.5 );
      static_cast<GRA_intCharacteristic*>(characteristics->Get(wxT("NLINCS")))->Set( nlinc );
    }
    else
    {
      nlinc = std::max(nlinc,2);
      try
      {
        UsefulFunctions::Scale2( mins, maxs, inc, nlinc, minValue, maxValue );
      }
      catch ( std::runtime_error &e)
      {
        throw EGraphicsError( wxString(e.what(),wxConvUTF8) );
      }
    }
  }
  else // axis is logarithmic
  {
    if( minValue <= 0.0 )minValue = std::numeric_limits<double>::min();
    mins = log(minValue)/log(logBase);  // calculate the power
    mins = static_cast<double>(static_cast<int>(mins)) - 1;
    if( maxValue <= 0.0 )maxValue = std::numeric_limits<double>::min();
    maxs = log(maxValue)/log(logBase);
    maxs = static_cast<double>(static_cast<int>(maxs)) + 1;
    if( nlinc==0 || static_cast<int>(maxs-mins)/nlinc*nlinc!=static_cast<int>(maxs-mins) )
    {
      nlinc = std::max(5,nlinc);
      double incp, minp, maxp;
      try
      {
        UsefulFunctions::Scale1( minp, maxp, incp, nlinc, mins, maxs );
      }
      catch ( std::runtime_error &e )
      {
        throw EGraphicsError( wxString(e.what(),wxConvUTF8) );
      }
      nlinc = static_cast<int>( (maxp-minp)/incp + 0.5 );
      static_cast<GRA_intCharacteristic*>(characteristics->Get(wxT("NLINCS")))->Set( nlinc );
      mins = static_cast<double>(static_cast<int>(minp));
      maxs = static_cast<double>(static_cast<int>(maxp));
    }
  }
  if( virtualMinMax )
  {
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get(wxT("MIN")))->Set( minValue );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get(wxT("MAX")))->Set( maxValue );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get(wxT("VIRTUALMIN")))->Set( mins );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get(wxT("VIRTUALMAX")))->Set( maxs );
  }
  else
  {
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get(wxT("MIN")))->Set( mins );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get(wxT("MAX")))->Set( maxs );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get(wxT("VIRTUALMIN")))->Set( mins );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get(wxT("VIRTUALMAX")))->Set( maxs );
  }
}

std::ostream &operator<<( std::ostream &out, GRA_cartesianAxes const &ca )
{
  int size = 2;
  if( ca.boxXAxis_ )++size;
  if( ca.boxYAxis_ )++size;
  out << "<cartesianaxes size=\"" << size << "\">\n";
  out << *ca.xAxis_ << *ca.yAxis_;
  if( ca.boxXAxis_ )out << *ca.boxXAxis_;
  if( ca.boxYAxis_ )out << *ca.boxYAxis_;
  return out << "</cartesianaxes>\n";
}

// end of file
