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
#include "ProjectHeader.h"
#pragma hdrstop

#include <algorithm>

#include "GRA_polarAxes.h"
#include "ExGlobals.h"
#include "EGraphicsError.h"
#include "GRA_axis.h"
#include "GRA_window.h"
#include "GRA_outputType.h"
#include "GRA_setOfCharacteristics.h"
#include "UsefulFunctions.h"
#include "GRA_polyline.h"
#include "GRA_color.h"
#include "GRA_font.h"

GRA_polarAxes::GRA_polarAxes( std::vector<double> const &r, std::vector<double> const &theta, bool radar )
  : GRA_drawableObject("POLARAXES"), r0_(r[0]), theta0_(theta[0])
{
  // for now, radar plots (0 degrees vertical) do not work properly
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
  //
  static_cast<GRA_intCharacteristic*>(polarC->Get("NUMBEROFDIGITS"))->Set(7);
  static_cast<GRA_intCharacteristic*>(polarC->Get("NUMBEROFDECIMALS"))->Set(-1);

  double const minValue = 0.0;
  std::size_t size = r.size();
  double maxValue = 0.0;
  for( std::size_t i=0; i<size; ++i )
  {
    if( maxValue < fabs(r[i]) )maxValue = fabs(r[i]);
  }
  double inc, vmin, vmax;
  int nlinc = 0;
  if( maxValue == 0.0 )maxValue = 0.01;
  else maxValue += 0.01*fabs(maxValue);
  nlinc = 3;
  try
  {
    UsefulFunctions::Scale1( vmin, vmax, inc, nlinc, minValue, maxValue );
  }
  catch ( runtime_error const &e )
  {
    throw EGraphicsError( e.what() );
  }
  if( vmin != 0.0 )
  {
    vmin = 0.0;
    inc = vmax;
  }
  nlinc = static_cast<int>( (vmax-vmin)/inc + 0.5 );
  static_cast<GRA_intCharacteristic*>(polarC->Get("NLINCS"))->Set( nlinc );
  double const xorigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get("XORIGIN"))->GetAsWorld();
  double const yorigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get("YORIGIN"))->GetAsWorld();
  double const length = static_cast<GRA_sizeCharacteristic*>(polarC->Get("AXISLENGTH"))->GetAsWorld();
  //
  static_cast<GRA_doubleCharacteristic*>(polarC->Get("MIN"))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(polarC->Get("MAX"))->Set( vmax );
  static_cast<GRA_doubleCharacteristic*>(polarC->Get("VIRTUALMIN"))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(polarC->Get("VIRTUALMAX"))->Set( vmax );
  //
  // if radar is true then theta=0 is vertical and up from the origin
  // otherwise, theta=0 is horizontal and to the right from the origin
  //
  bool numbersonSave = static_cast<GRA_boolCharacteristic*>(polarC->Get("NUMBERSON"))->Get();
  //double angleSave = static_cast<GRA_angleCharacteristic*>(polarC->Get("AXISANGLE"))->Get();
  //bool compassSave = static_cast<GRA_boolCharacteristic*>(polarC->Get("COMPASSLABELS"))->Get();
  //bool clockwiseSave = static_cast<GRA_boolCharacteristic*>(polarC->Get("CLOCKWISE"))->Get();
  if( radar )
  {
    static_cast<GRA_angleCharacteristic*>(polarC->Get("AXISANGLE"))->Set( 90.0 );
    static_cast<GRA_boolCharacteristic*>(polarC->Get("COMPASSLABELS"))->Set( true );
    static_cast<GRA_boolCharacteristic*>(polarC->Get("CLOCKWISE"))->Set( true );
  }
  double startAngle = static_cast<GRA_angleCharacteristic*>(polarC->Get("AXISANGLE"))->Get();
  axis_ = new GRA_axis( xorigin, yorigin, length, polarC );
  //
  // fix the axis values for conversion to world coordinates
  // so the other axes can be made without confusing the conversion
  //
  double xw, yw;
  gw->PolarToWorld( r0_, theta0_, xw, yw, true );
  //
  int nAxes = static_cast<GRA_intCharacteristic*>(polarC->Get("NAXES"))->Get();
  static_cast<GRA_boolCharacteristic*>(polarC->Get("NUMBERSON"))->Set( false );
  for( int i=1; i<nAxes; ++i )
  {
    static_cast<GRA_angleCharacteristic*>(polarC->Get("AXISANGLE"))->Set( startAngle+i*360.0/nAxes );
    static_cast<GRA_intCharacteristic*>(polarC->Get("NLINCS"))->Set( nlinc );
    otherAxes_.push_back( new GRA_axis(xorigin,yorigin,length,polarC) );
  }
  static_cast<GRA_boolCharacteristic*>(polarC->Get("NUMBERSON"))->Set( numbersonSave );
  static_cast<GRA_angleCharacteristic*>(polarC->Get("AXISANGLE"))->Set( startAngle );
  //static_cast<GRA_boolCharacteristic*>(polarC->Get("COMPASSLABELS"))->Set( compassSave );
  //static_cast<GRA_boolCharacteristic*>(polarC->Get("CLOCKWISE"))->Set( clockwiseSave );
}

void GRA_polarAxes::DeleteStuff()
{
  delete axis_;
  axis_ = 0;
  while( !otherAxes_.empty() )
  {
    delete otherAxes_.back();
    otherAxes_.pop_back();
  }
}

void GRA_polarAxes::CopyStuff( GRA_polarAxes const &rhs )
{
  if( rhs.axis_ )axis_ = new GRA_axis( *rhs.axis_ );
  std::size_t size = rhs.otherAxes_.size();
  for( std::size_t i=0; i<size; ++i )otherAxes_.push_back( new GRA_axis(*rhs.otherAxes_[i]) );
}

void GRA_polarAxes::Make()
{
  axis_->Make();
  std::size_t size = otherAxes_.size();
  for( std::size_t i=0; i<size; ++i )otherAxes_[i]->Make();
}

void GRA_polarAxes::Draw( GRA_outputType *graphicsOutput )
{
  //
  // draw the grid lines
  // if polargrid is zero, no tic coordinates are recorded, so no grid lines will be drawn
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  //
  GRA_color *lineColor = static_cast<GRA_colorCharacteristic*>(polarC->Get("AXISCOLOR"))->Get();
  int lineType = static_cast<GRA_intCharacteristic*>(generalC->Get("GRIDLINETYPE"))->Get();
  double const xorigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get("XORIGIN"))->GetAsWorld();
  double const yorigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get("YORIGIN"))->GetAsWorld();
  //
  // grid circles are drawn at tic marks on the x-axis
  //
  std::vector<double> ticX, ticY;
  axis_->GetTicCoordinates( ticX, ticY );
  double axisAngle = axis_->GetAngle();

  double cosAngle = cos(axisAngle*M_PI/180.0);
  double sinAngle = sin(axisAngle*M_PI/180.0);
  if( fabs(cosAngle) < 0.0001 )cosAngle = 0.0;
  if( fabs(sinAngle) < 0.0001 )sinAngle = 0.0;

  std::size_t size = ticX.size();
  std::vector<double> x, y;
  std::vector<int> pen;
  int ngrid = abs(static_cast<GRA_intCharacteristic*>(polarC->Get("GRID"))->Get());
  for( std::size_t i=0; i<size; i+=ngrid )
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
    GRA_polyline p( x, y, pen, 1, lineType, lineColor );
    p.Draw( graphicsOutput );
    x.clear();
    y.clear();
    pen.clear();
  }
  axis_->Draw( graphicsOutput );
  DrawLabel( graphicsOutput );

  double height = static_cast<GRA_sizeCharacteristic*>(polarC->Get("LABELHEIGHT"))->GetAsWorld();
  GRA_color *color = static_cast<GRA_colorCharacteristic*>(polarC->Get("LABELCOLOR"))->Get();
  GRA_font *font = static_cast<GRA_fontCharacteristic*>(polarC->Get("LABELFONT"))->Get();
  double startAngle = static_cast<GRA_angleCharacteristic*>(polarC->Get("AXISANGLE"))->Get();
  bool clockwise = static_cast<GRA_boolCharacteristic*>(polarC->Get("CLOCKWISE"))->Get();
  bool compassDirections = static_cast<GRA_boolCharacteristic*>(polarC->Get("COMPASSLABELS"))->Get();
  size = otherAxes_.size();
  if( compassDirections && (size==0||size==1||size==3||size==7||size==15) )
  {
    ExString label("N");
    if( height > 0.0 )DrawAngle( label, axis_, height, color, font, graphicsOutput );
    switch (size)
    {
      case 1:  // N, S
      {
        otherAxes_[0]->Draw( graphicsOutput );
        if( height > 0.0 )
        {
          double angle = otherAxes_[0]->GetAngle() - startAngle;
          while( angle < 0.0 )angle += 360.0;
          if( clockwise )angle = 360.0 - angle;
          ExString label("S");
          DrawAngle( label, otherAxes_[0], height, color, font, graphicsOutput );
        }
        break;
      }
      case 3:  // N, E, S, W
      {
        std::vector<ExString> labels(3);
        labels[0] = "E";
        labels[1] = "S";
        labels[2] = "W";
        for( int i=0; i<3; ++i )
        {
          otherAxes_[i]->Draw( graphicsOutput );
          if( height > 0.0 )
          {
            ExString label = clockwise ? labels[2-i] : labels[i];
            DrawAngle( label, otherAxes_[i], height, color, font, graphicsOutput );
          }
        }
        break;
      }
      case 7:  // N, NE, E, SE, S, SW, W, NW
      {
        std::vector<ExString> labels(7);
        labels[0] = "NE";
        labels[1] = "E";
        labels[2] = "SE";
        labels[3] = "S";
        labels[4] = "SW";
        labels[5] = "W";
        labels[6] = "NW";
        for( int i=0; i<7; ++i )
        {
          otherAxes_[i]->Draw( graphicsOutput );
          if( height > 0.0 )
          {
            if( clockwise )
              DrawAngle( labels[6-i], otherAxes_[i], height, color, font, graphicsOutput );
            else
              DrawAngle( labels[i], otherAxes_[i], height, color, font, graphicsOutput );
          }
        }
        break;
      }
      case 15:
      {
        std::vector<ExString> labels(15);
        labels[0] = "NNE";
        labels[1] = "NE";
        labels[2] = "ENE";
        labels[3] = "E";
        labels[4] = "ESE";
        labels[5] = "SE";
        labels[6] = "SSE";
        labels[7] = "S";
        labels[8] = "SSW";
        labels[9] = "SW";
        labels[10] = "WSW";
        labels[11] = "W";
        labels[12] = "WNW";
        labels[13] = "NW";
        labels[14] = "NNW";
        for( int i=0; i<15; ++i )
        {
          otherAxes_[i]->Draw( graphicsOutput );
          if( height > 0.0 )
          {
            if( clockwise )
              DrawAngle( labels[14-i], otherAxes_[i], height, color, font, graphicsOutput );
            else
              DrawAngle( labels[i], otherAxes_[i], height, color, font, graphicsOutput );
          }
        }
        break;
      }
    }
  }
  else
  {
    ExString label("0<^,degree>");
    DrawAngle( label, axis_, height, color, font, graphicsOutput );
    for( std::size_t i=0; i<size; ++i )
    {
      otherAxes_[i]->Draw( graphicsOutput );
      if( height > 0.0 )
      {
        double angle = otherAxes_[i]->GetAngle() - startAngle;
        while( angle < 0.0 )angle += 360.0;
        if( clockwise )angle = 360.0 - angle;
        ExString label(angle);
        label += "<^,degree>";
        DrawAngle( label, otherAxes_[i], height, color, font, graphicsOutput );
      }
    }
  }
}

void GRA_polarAxes::DrawAngle( ExString &label, GRA_axis *axis, double height,
                               GRA_color *color, GRA_font *font,
                               GRA_outputType *graphicsOutput )
{
  double xend, yend;
  axis->GetEndPoint( xend, yend );
  double axisAngle = axis->GetAngle();
  //
  int align = 4;
  if( 45 < axisAngle && axisAngle < 135 )       align = 2;
  else if( 135 <= axisAngle && axisAngle < 225 )align = 6;
  else if( 225 <= axisAngle && axisAngle < 315 )align = 8;
  //
  double xoff = 0.2*cos(axisAngle*M_PI/180.);
  double yoff = 0.2*sin(axisAngle*M_PI/180.);
  GRA_drawableText dt( label, height, 0.0, xend+xoff, yend+yoff, align, font, color );
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

void GRA_polarAxes::DrawLabel( GRA_outputType *graphicsOutput )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
  //
  ExString label;
  bool labelIsOn = static_cast<GRA_boolCharacteristic*>(polarC->Get("LABELON"))->Get();
  if( labelIsOn )label = static_cast<GRA_stringCharacteristic*>(polarC->Get("LABEL"))->Get();
  if( label.empty() )labelIsOn = false;
  //
  // if powerAuto_ = 2, calculate the power but do not draw it
  //
  double power = static_cast<GRA_doubleCharacteristic*>(polarC->Get("POWER"))->Get();
  if( static_cast<GRA_intCharacteristic*>(polarC->Get("POWERAUTO"))->Get()!=2 &&
      static_cast<GRA_boolCharacteristic*>(polarC->Get("NUMBERSON"))->Get() && power!=0.0 )
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
    double sizlab = static_cast<GRA_sizeCharacteristic*>(polarC->Get("LABELHEIGHT"))->GetAsWorld();
    double axisAngle = static_cast<GRA_angleCharacteristic*>(polarC->Get("AXISANGLE"))->Get();
    double imagTicAngle = static_cast<GRA_angleCharacteristic*>(polarC->Get("IMAGTICANGLE"))->Get();
    double imagTicLen = static_cast<GRA_sizeCharacteristic*>(polarC->Get("IMAGTICLENGTH"))->GetAsWorld();
    double numHeight = static_cast<GRA_sizeCharacteristic*>(polarC->Get("NUMBERSHEIGHT"))->GetAsWorld();
    GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(polarC->Get("LABELFONT"))->Get();
    GRA_color *labelColor = static_cast<GRA_colorCharacteristic*>(polarC->Get("LABELCOLOR"))->Get();
    double length = static_cast<GRA_sizeCharacteristic*>(polarC->Get("AXISLENGTH"))->GetAsWorld();
    double xOrigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get("XORIGIN"))->GetAsWorld();
    double yOrigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get("YORIGIN"))->GetAsWorld();
    double x1, y1, angle;
    int align;
    //
    // draw the label horizontally and centered on the x-axis
    if( axisAngle == 0.0 )
    {
      x1 = xOrigin + 0.5*length;
      y1 = yOrigin - 1.05*(imagTicLen+numHeight);
      angle = 0.0;
      align = 8; // 8=top centre
    }
    else
    {
      x1 = xOrigin + 1.1*(imagTicLen+axis_->GetMaxWidth());
      y1 = yOrigin + 0.5*length;
      angle = 270.0;
      align = 2; // bottom centre
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

int GRA_polarAxes::NumberOfDigits( double maxs, double logBase )
{
  int ndig1b = static_cast<int>(log10(logBase))+1;
  int ndig1  = 1 + ndig1b;
  int ndig2p = 1;
  if( fabs(maxs) > 0.0 )ndig2p = static_cast<int>(log10(fabs(maxs)))+1;
  if( maxs < 0.0 )++ndig2p;
  int ndig2 = ndig2p + ndig1b;
  return std::max(ndig1,ndig2);
}

std::ostream &operator<<( std::ostream &out, GRA_polarAxes const &ca )
{
  return out << "<polaraxes>\n" << *ca.axis_  << "</polaraxes>\n";
}

// end of file
