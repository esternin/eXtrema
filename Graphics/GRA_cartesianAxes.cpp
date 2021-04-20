/*
Copyright (C) 2005,...,2008 Joseph L. Chuma, TRIUMF

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

#include "GRA_cartesianAxes.h"
#include "ExGlobals.h"
#include "EGraphicsError.h"
#include "GRA_axis.h"
#include "GRA_window.h"
#include "GRA_outputType.h"
#include "GRA_setOfCharacteristics.h"
#include "UsefulFunctions.h"

GRA_cartesianAxes::GRA_cartesianAxes( std::vector<double> const &x, std::vector<double> const &y,
                                      bool xOnTop, bool yOnRight )
  : GRA_drawableObject("CARTESIANAXES"), xOnTop_(xOnTop), yOnRight_(yOnRight)
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
  gridLineType_ = static_cast<GRA_intCharacteristic*>(generalC->Get("GRIDLINETYPE"))->Get();
  //
  double xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("LOWERAXIS"))->GetAsWorld();
  double ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("LOWERAXIS"))->GetAsWorld();
  double xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get("UPPERAXIS"))->GetAsWorld();
  double yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get("UPPERAXIS"))->GetAsWorld();
  bool graphBox = static_cast<GRA_boolCharacteristic*>(generalC->Get("GRAPHBOX"))->Get();
  double const xLength = xuaxis - xlaxis;
  double const yLength = yuaxis - ylaxis;
  //
  GRA_setOfCharacteristics xCharacteristics( *xAxisC );
  GRA_setOfCharacteristics yCharacteristics( *yAxisC );
  //
  boxXAxis_ = 0;
  boxYAxis_ = 0;
  if( graphBox )
  {
    GRA_setOfCharacteristics boxSettings( xCharacteristics );
    static_cast<GRA_intCharacteristic*>(boxSettings.Get("GRID"))->Set(0);
    static_cast<GRA_boolCharacteristic*>(boxSettings.Get("LABELON"))->Set(false);
    static_cast<GRA_boolCharacteristic*>(boxSettings.Get("NUMBERSON"))->Set(false);
    //
    if( xOnTop )boxXAxis_ = new GRA_axis( xlaxis, ylaxis, xLength, &boxSettings );
    else
    {
      double angle = static_cast<GRA_angleCharacteristic*>(xAxisC->Get("TICANGLE"))->Get();
      static_cast<GRA_angleCharacteristic*>(boxSettings.Get("TICANGLE"))->Set(-angle);
      boxXAxis_ = new GRA_axis( xlaxis, yuaxis, xLength, &boxSettings );
    }
    boxSettings = yCharacteristics;
    static_cast<GRA_intCharacteristic*>(boxSettings.Get("GRID"))->Set(0);
    static_cast<GRA_boolCharacteristic*>(boxSettings.Get("LABELON"))->Set(false);
    static_cast<GRA_boolCharacteristic*>(boxSettings.Get("NUMBERSON"))->Set(false);
    if( yOnRight )boxYAxis_ = new GRA_axis( xlaxis, ylaxis, yLength, &boxSettings );
    else
    {
      double angle = static_cast<GRA_angleCharacteristic*>(yAxisC->Get("TICANGLE"))->Get();
      static_cast<GRA_angleCharacteristic*>(boxSettings.Get("TICANGLE"))->Set(-angle);
      boxYAxis_ = new GRA_axis( xuaxis, ylaxis, yLength, &boxSettings );
    }
  }
  if( yOnRight )
  {
    double ytica = static_cast<GRA_angleCharacteristic*>(yCharacteristics.Get("TICANGLE"))->Get();
    double yitica = static_cast<GRA_angleCharacteristic*>(yCharacteristics.Get("IMAGTICANGLE"))->Get();
    static_cast<GRA_angleCharacteristic*>(yCharacteristics.Get("TICANGLE"))->Set(-ytica);
    static_cast<GRA_angleCharacteristic*>(yCharacteristics.Get("IMAGTICANGLE"))->Set(-yitica);
    yAxis_ = new GRA_axis( xuaxis, ylaxis, yLength, &yCharacteristics );
  }
  else
  {
    yAxis_ = new GRA_axis( xlaxis, ylaxis, yLength, &yCharacteristics );
  }
  if( xOnTop )
  {
    double xtica = static_cast<GRA_angleCharacteristic*>(xCharacteristics.Get("TICANGLE"))->Get();
    double xitica = static_cast<GRA_angleCharacteristic*>(xCharacteristics.Get("IMAGTICANGLE"))->Get();
    static_cast<GRA_angleCharacteristic*>(xCharacteristics.Get("TICANGLE"))->Set(-xtica);
    static_cast<GRA_angleCharacteristic*>(xCharacteristics.Get("IMAGTICANGLE"))->Set(-xitica);
    xAxis_ = new GRA_axis( xlaxis, yuaxis, xLength, &xCharacteristics );
  }
  else
  {
    xAxis_ = new GRA_axis( xlaxis, ylaxis, xLength, &xCharacteristics );
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
  gridLineType_ = rhs.gridLineType_;
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

void GRA_cartesianAxes::Draw( GRA_outputType *graphicsOutput )
{
  xAxis_->Draw( graphicsOutput );
  DrawXLabel( graphicsOutput );
  yAxis_->Draw( graphicsOutput );
  DrawYLabel( graphicsOutput );
  if( boxXAxis_ )boxXAxis_->Draw( graphicsOutput );
  if( boxYAxis_ )boxYAxis_->Draw( graphicsOutput );
  //
  // draw the x-axis grid lines
  // if nxgrid is zero, no tic coordinates are recorded, so no grid lines will be drawn
  //
  GRA_setOfCharacteristics *xAxisC = xAxis_->GetCharacteristics();
  std::vector<double> ticX, ticY;
  xAxis_->GetTicCoordinates( ticX, ticY );
  std::size_t size = ticX.size();
  double const eps = 0.001;
  graphicsOutput->SetColor( static_cast<GRA_colorCharacteristic*>(xAxisC->Get("AXISCOLOR"))->Get() );
  //
  int lineTypeSave = graphicsOutput->GetLineType();
  graphicsOutput->SetLineType( gridLineType_ );
  //
  // grid lines are drawn on every nxgrid_th (sic) long tic mark on the x-axis, parallel to the y-axis
  //
  double xlaxis, ylaxis, xuaxis, yuaxis;
  xAxis_->GetOrigin( xlaxis, ylaxis );
  xAxis_->GetEndPoint( xuaxis, yuaxis );
  int ngrid = abs(xAxis_->GetGrid());
  for( std::size_t i=0; i<size; i+=ngrid )
  {
    double x = ticX[i];
    double y = ticY[i];
    if( fabs(x-xuaxis)>eps && fabs(x-xlaxis)>eps )
    {
      graphicsOutput->StartLine( x, y );
      graphicsOutput->ContinueLine( x, y+(yuaxis-ylaxis) );
    }
  }
  // draw the y-axis grid lines, parallel to the x-axis
  // if nygrid is zero, no tic coordinates are recorded, so no grid lines will be drawn
  //
  GRA_setOfCharacteristics *yAxisC = yAxis_->GetCharacteristics();
  yAxis_->GetTicCoordinates( ticX, ticY );
  size = ticX.size();
  graphicsOutput->SetColor( static_cast<GRA_colorCharacteristic*>(yAxisC->Get("AXISCOLOR"))->Get() );
  //
  // grid lines are drawn on every nxgrid_th (sic) long tic mark on the x-axis and parallel to the y-axis
  //
  ngrid = abs(yAxis_->GetGrid());
  for( std::size_t i=0; i<size; i+=ngrid )
  {
    double x = ticX[i];
    double y = ticY[i];
    if( fabs(y-yuaxis)>eps && fabs(y-ylaxis)>eps )
    {
      graphicsOutput->StartLine( x, y );
      graphicsOutput->ContinueLine( x+(xuaxis-xlaxis), y );
    }
  }
  graphicsOutput->SetLineType( lineTypeSave );
}

void GRA_cartesianAxes::DrawXLabel( GRA_outputType *graphicsOutput )
{
  GRA_setOfCharacteristics *xAxisC = xAxis_->GetCharacteristics();
  //
  ExString label;
  bool labelIsOn = static_cast<GRA_boolCharacteristic*>(xAxisC->Get("LABELON"))->Get();
  if( labelIsOn )label = static_cast<GRA_stringCharacteristic*>(xAxisC->Get("LABEL"))->Get();
  if( label.empty() )labelIsOn = false;
  //
  // if powerAuto_ = 2, calculate the power but do not draw it
  //
  double power = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("POWER"))->Get();
  if( static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("LOGBASE"))->Get()<=1.0 &&
      static_cast<GRA_intCharacteristic*>(xAxisC->Get("POWERAUTO"))->Get()!=2 &&
      static_cast<GRA_boolCharacteristic*>(xAxisC->Get("NUMBERSON"))->Get() && power != 0.0 )
  {
    ExString powLabel("(x10<^><Z0.5%>");
    powLabel += ExString(power) + ExString("<_>)");
    label = label.empty() ? powLabel : label+ExString(" ")+powLabel;
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
    double sizlab = static_cast<GRA_sizeCharacteristic*>(xAxisC->Get("LABELHEIGHT"))->GetAsWorld();
    double imagTicAngle = static_cast<GRA_angleCharacteristic*>(xAxisC->Get("IMAGTICANGLE"))->Get();
    double imagTicLen = static_cast<GRA_sizeCharacteristic*>(xAxisC->Get("IMAGTICLENGTH"))->GetAsWorld();
    double numHeight = static_cast<GRA_sizeCharacteristic*>(xAxisC->Get("NUMBERSHEIGHT"))->GetAsWorld();
    GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(xAxisC->Get("LABELFONT"))->Get();
    GRA_color *labelColor = static_cast<GRA_colorCharacteristic*>(xAxisC->Get("LABELCOLOR"))->Get();
    double x1, y1, angle=0.0;
    int align = 2;
    double xOrigin, yOrigin;
    xAxis_->GetOrigin( xOrigin, yOrigin );
    //
    // draw the label horizontally and centered on the x-axis
    x1 = xOrigin + 0.5*xAxis_->GetLength();
    if( imagTicAngle > 180.0 )
    {
      align = 8; // 8=top centre
      y1 = yOrigin - 1.10*(imagTicLen+numHeight);
    }
    else
    {
      align = 2; // 2=bottom centre
      y1 = yOrigin + 1.10*(imagTicLen+numHeight);
    }
    GRA_drawableText dt(label,sizlab,angle,x1,y1,align,labelFont,labelColor);
    try
    {
      dt.Parse();
    }
    catch ( EGraphicsError const &e )
    {
      throw;
    }
    graphicsOutput->Draw( &dt );
  }
}

void GRA_cartesianAxes::DrawYLabel( GRA_outputType *graphicsOutput )
{
  GRA_setOfCharacteristics *yAxisC = yAxis_->GetCharacteristics();
  //
  ExString label;
  bool labelIsOn = static_cast<GRA_boolCharacteristic*>(yAxisC->Get("LABELON"))->Get();
  if( labelIsOn )label = static_cast<GRA_stringCharacteristic*>(yAxisC->Get("LABEL"))->Get();
  if( label.empty() )labelIsOn = false;
  //
  // if powerAuto_ = 2, calculate the power but do not draw it
  //
  double power = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("POWER"))->Get();
  if( static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("LOGBASE"))->Get()<=1.0 &&
      static_cast<GRA_intCharacteristic*>(yAxisC->Get("POWERAUTO"))->Get()!=2 &&
      static_cast<GRA_boolCharacteristic*>(yAxisC->Get("NUMBERSON"))->Get() && power != 0.0 )
  {
    ExString powLabel("(x10<^><Z0.5%>");
    powLabel += ExString(power) + ExString("<_>)");
    label = label.empty() ? powLabel : label+ExString(" ")+powLabel;
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
    double sizlab = static_cast<GRA_sizeCharacteristic*>(yAxisC->Get("LABELHEIGHT"))->GetAsWorld();
    double imagTicAngle = static_cast<GRA_angleCharacteristic*>(yAxisC->Get("IMAGTICANGLE"))->Get();
    double imagTicLen = static_cast<GRA_sizeCharacteristic*>(yAxisC->Get("IMAGTICLENGTH"))->GetAsWorld();
    double numHeight = static_cast<GRA_sizeCharacteristic*>(yAxisC->Get("NUMBERSHEIGHT"))->GetAsWorld();
    GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(yAxisC->Get("LABELFONT"))->Get();
    GRA_color *labelColor = static_cast<GRA_colorCharacteristic*>(yAxisC->Get("LABELCOLOR"))->Get();
    double x1, y1, angle=0.0;
    int align = 2; // bottom centre alignment
    double xOrigin, yOrigin;
    yAxis_->GetOrigin( xOrigin, yOrigin );
    //
    // draw the label vertically and centered on the y-axis
    //
    y1 = yOrigin + 0.5*yAxis_->GetLength();
    double numWidth = yAxis_->GetMaxWidth();
    if( imagTicAngle > 180. )
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
    catch ( EGraphicsError &e )
    {
      delete dt;
      throw;
    }
    graphicsOutput->Draw( dt );
  }
}

void GRA_cartesianAxes::AutoScale( std::vector<double> const &x, std::vector<double> const &y )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  ExString autoScale(
   static_cast<GRA_stringCharacteristic*>(generalC->Get("AUTOSCALE"))->Get() );
  GRA_setOfCharacteristics *xCharacteristics = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yCharacteristics = gw->GetYAxisCharacteristics();
  int autoX, autoY;
  if( autoScale == "OFF" )
  {
    autoX = 0;
    autoY = 0;
  }
  else if( autoScale == "ON" )
  {
    autoX = 2;
    autoY = 2;
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDIGITS"))->Set(7);
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDECIMALS"))->Set(-1);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDIGITS"))->Set(7);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDECIMALS"))->Set(-1);
  }
  else if( autoScale == "X" )
  {
    autoX = 2;
    autoY = 0;
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDIGITS"))->Set(7);
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDECIMALS"))->Set(-1);
  }
  else if( autoScale == "Y" )
  {
    autoX = 0;
    autoY = 2;
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDIGITS"))->Set(7);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDECIMALS"))->Set(-1);
  }
  else if( autoScale == "VIRTUAL" )
  {
    autoX = 3;
    autoY = 3;
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDIGITS"))->Set(7);
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDECIMALS"))->Set(-1);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDIGITS"))->Set(7);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDECIMALS"))->Set(-1);
  }
  else if( autoScale == "XVIRTUAL" )
  {
    autoX = 3;
    autoY = 0;
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDIGITS"))->Set(7);
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDECIMALS"))->Set(-1);
  }
  else if( autoScale == "YVIRTUAL" )
  {
    autoX = 0;
    autoY = 3;
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDIGITS"))->Set(7);
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDECIMALS"))->Set(-1);
  }
  else if( autoScale == "COMMENSURATE" )
  {
    double xlwind =
     static_cast<GRA_distanceCharacteristic*>(generalC->Get("XLOWERWINDOW"))->
      GetAsWorld();
    double ylwind =
     static_cast<GRA_distanceCharacteristic*>(generalC->Get("YLOWERWINDOW"))->
      GetAsWorld();
    double xuwind =
     static_cast<GRA_distanceCharacteristic*>(generalC->Get("XUPPERWINDOW"))->
      GetAsWorld();
    double yuwind =
     static_cast<GRA_distanceCharacteristic*>(generalC->Get("YUPPERWINDOW"))->
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
    catch ( runtime_error &e )
    {
      throw EGraphicsError( e.what() );
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
    static_cast<GRA_distanceCharacteristic*>(xCharacteristics->Get("LOWERAXIS"))->SetAsPercent( xlp );
    static_cast<GRA_distanceCharacteristic*>(xCharacteristics->Get("UPPERAXIS"))->SetAsPercent( xup );
    static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("MAX"))->Set( xmax );
    static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("MIN"))->Set( xmin );
    static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("VIRTUALMAX"))->Set( xmax );
    static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("VIRTUALMIN"))->Set( xmin );
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NLINCS"))->Set( nlxinc );
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDIGITS"))->Set( 7 );
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDECIMALS"))->Set( -1 );
    //
    static_cast<GRA_distanceCharacteristic*>(yCharacteristics->Get("LOWERAXIS"))->SetAsPercent( ylp );
    static_cast<GRA_distanceCharacteristic*>(yCharacteristics->Get("UPPERAXIS"))->SetAsPercent( yup );
    static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("MAX"))->Set( ymax );
    static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("MIN"))->Set( ymin );
    static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("VIRTUALMAX"))->Set( ymax );
    static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("VIRTUALMIN"))->Set( ymin );
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NLINCS"))->Set( nlyinc );
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDIGITS"))->Set( 7 );
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDECIMALS"))->Set( -1 );
    //
    return;
  }
  int xnlinc = abs(static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NLINCS"))->Get());
  double xmin = static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("MIN"))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("MAX"))->Get();
  double xmins = static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("VIRTUALMIN"))->Get();
  double xmaxs = static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("VIRTUALMAX"))->Get();
  bool xzeroForce = static_cast<GRA_boolCharacteristic*>(xCharacteristics->Get("ZERO"))->Get();
  double xlogBase = static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("LOGBASE"))->Get();
  bool xlogStyle = static_cast<GRA_boolCharacteristic*>(xCharacteristics->Get("LOGSTYLE"))->Get();
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
  int ynlinc = abs(static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NLINCS"))->Get());
  double ymin = static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("MIN"))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("MAX"))->Get();
  double ymins = static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("VIRTUALMIN"))->Get();
  double ymaxs = static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("VIRTUALMAX"))->Get();
  bool yzeroForce = static_cast<GRA_boolCharacteristic*>(yCharacteristics->Get("ZERO"))->Get();
  double ylogBase = static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("LOGBASE"))->Get();
  bool ylogStyle = static_cast<GRA_boolCharacteristic*>(yCharacteristics->Get("LOGSTYLE"))->Get();
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

  //ExGlobals::WriteOutput(ExString("xminValue=")+ExString(xminValue)+", xmaxValue="+
  //  ExString(xmaxValue)+", yminValue="+ExString(yminValue)+", ymaxValue="+ExString(ymaxValue));

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
        if( x[i]>=xminValue && x[i]<=xmaxValue )
        {
          pointFound = true;
          break;
        }
      }
      if( !pointFound ) // no data values within the x-axis range, cannot autoscale the y-axis
      {
        static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NLINCS"))->Set( 1 );
        static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("MIN"))->Set( 0.0 );
        static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("MAX"))->Set( 1.0 );
        static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("VIRTUALMIN"))->Set( 0.0 );
        static_cast<GRA_doubleCharacteristic*>(yCharacteristics->Get("VIRTUALMAX"))->Set( 1.0 );
        return;
      }
      yminValue = std::numeric_limits<double>::max();
      ymaxValue = -yminValue;
      for( std::size_t i=0; i<npt; ++i )
      {
        if( (x[i]-xminValue)*(xmaxValue-x[i]) >= 0.0 )
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
        if( (y[i]-yminValue)*(ymaxValue-y[i]) >= 0.0 )
        {
          pointFound = true;
          break;
        }
      }
      if( !pointFound ) // no data values within the y-axis range
      {
        static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NLINCS"))->Set( 1 );
        static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("MIN"))->Set( 0.0 );
        static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("MAX"))->Set( 1.0 );
        static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("VIRTUALMIN"))->Set( 0.0 );
        static_cast<GRA_doubleCharacteristic*>(xCharacteristics->Get("VIRTUALMAX"))->Set( 1.0 );
        return;
      }
      xminValue = std::numeric_limits<double>::max();
      xmaxValue = -xminValue;
      for( std::size_t i=0; i<npt; ++i )
      {
        if( (y[i]-yminValue)*(ymaxValue-y[i]) >= 0.0 )
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
      //
      UsefulFunctions::MinMax( y, 0, npt, yminValue, ymaxValue );
      bool yVirtualMinMax = (autoY==3);
      FixupScales( yAxisLinear, yzeroForce, yminValue, ymaxValue, ylogBase,
                   ynlinc, ymins, ymaxs, yinc, yCharacteristics, yVirtualMinMax );
    }
  }
  if( !xAxisLinear && xlogStyle )
    static_cast<GRA_intCharacteristic*>(xCharacteristics->Get("NUMBEROFDIGITS"))->Set(
      NumberOfDigits(xmins,xmaxs,xlogBase) );
  if( !yAxisLinear && ylogStyle )
    static_cast<GRA_intCharacteristic*>(yCharacteristics->Get("NUMBEROFDIGITS"))->Set(
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
    throw EGraphicsError("problem with axis log scales minimum value");
  }
  try
  {
    ResetLogValue( maxValue, logBase );
  }
  catch ( EGraphicsError &e )
  {
    throw EGraphicsError("problem with axis log scales maximum value");
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
  catch ( runtime_error &e )
  {
    throw EGraphicsError( e.what() );
  }
  nlinc = static_cast<int>( (maxs-mins)/inc + 0.5 );
  static_cast<GRA_intCharacteristic*>(characteristics->Get("NLINCS"))->Set( nlinc );
  static_cast<GRA_doubleCharacteristic*>(characteristics->Get("VIRTUALMIN"))->Set( mins );
  static_cast<GRA_doubleCharacteristic*>(characteristics->Get("VIRTUALMAX"))->Set( maxs );
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
  if( value*log(logBase) > log(std::numeric_limits<double>::max()) )
  {
    value = 10.;
    return;
  }
  try
  {
    value = exp(value*log(logBase));
  }
  catch (...)
  {
    throw EGraphicsError(" ");
  }
}

void GRA_cartesianAxes::FixupScales( bool linear, bool zeroForce, double minValue, double maxValue, double logBase,
                                     int &nlinc, double &mins, double &maxs, double &inc,
                                     GRA_setOfCharacteristics *characteristics, bool virtualMinMax )
{
  if( linear ) // axis is linear
  {
    if( minValue == 0.0 )minValue = -0.01;
    else minValue -= 0.01*fabs(minValue);
    if( maxValue == 0.0 )maxValue = 0.01;
    else maxValue += 0.01*fabs(maxValue);
    if( zeroForce )  // force zero to be displayed on a linear axis
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
      catch ( runtime_error &e )
      {
        throw EGraphicsError( e.what() );
      }
      nlinc = static_cast<int>( (maxs-mins)/inc + 0.5 );
      static_cast<GRA_intCharacteristic*>(characteristics->Get("NLINCS"))->Set( nlinc );
    }
    else
    {
      nlinc = std::max(nlinc,2);
      try
      {
        UsefulFunctions::Scale2( mins, maxs, inc, nlinc, minValue, maxValue );
      }
      catch ( runtime_error &e)
      {
        throw EGraphicsError( e.what() );
      }
    }
  }
  else // axis is logarithmic
  {
    if( minValue <= 0.0 )minValue = std::numeric_limits<double>::min();
    mins = log(minValue)/log(logBase); // calculate the power
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
      catch ( runtime_error &e )
      {
        throw EGraphicsError( e.what() );
      }
      nlinc = static_cast<int>( (maxp-minp)/incp + 0.5 );
      static_cast<GRA_intCharacteristic*>(characteristics->Get("NLINCS"))->Set( nlinc );
      mins = static_cast<double>(static_cast<int>(minp));
      maxs = static_cast<double>(static_cast<int>(maxp));
    }
  }
  if( virtualMinMax )
  {
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get("MIN"))->Set( minValue );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get("MAX"))->Set( maxValue );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get("VIRTUALMIN"))->Set( mins );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get("VIRTUALMAX"))->Set( maxs );
  }
  else
  {
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get("MIN"))->Set( mins );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get("MAX"))->Set( maxs );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get("VIRTUALMIN"))->Set( mins );
    static_cast<GRA_doubleCharacteristic*>(characteristics->Get("VIRTUALMAX"))->Set( maxs );
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
