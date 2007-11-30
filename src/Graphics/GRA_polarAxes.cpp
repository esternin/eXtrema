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
#include <cmath>
#include <stdexcept>
#include <algorithm>

#include "GRA_polarAxes.h"
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
#include "GRA_colorCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"
#include "GRA_color.h"
#include "GRA_font.h"
#include "GRA_polyline.h"
#include "GRA_drawableText.h"

GRA_polarAxes::GRA_polarAxes( std::vector<double> const &r, std::vector<double> const &theta, bool radar )
    : GRA_drawableObject(wxT("POLARAXES")), r0_(r[0]), theta0_(theta[0])
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  //
  GRA_setOfCharacteristics axisC( *polarC );
  //
  gridLineType_ = static_cast<GRA_intCharacteristic*>(generalC->Get(wxT("GRIDLINETYPE")))->Get();
  clockwise_ = static_cast<GRA_boolCharacteristic*>(axisC.Get(wxT("CLOCKWISE")))->Get();
  compassLabels_ = static_cast<GRA_boolCharacteristic*>(axisC.Get(wxT("COMPASSLABELS")))->Get();
  //
  static_cast<GRA_intCharacteristic*>(axisC.Get(wxT("NUMBEROFDIGITS")))->Set(7);
  static_cast<GRA_intCharacteristic*>(axisC.Get(wxT("NUMBEROFDECIMALS")))->Set(-1);

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
  catch ( std::runtime_error const &e )
  {
    throw EGraphicsError( wxString(e.what(),wxConvUTF8) );
  }
  if( vmin != 0.0 )
  {
    vmin = 0.0;
    inc = vmax;
  }
  nlinc = static_cast<int>( (vmax-vmin)/inc + 0.5 );
  static_cast<GRA_intCharacteristic*>(axisC.Get(wxT("NLINCS")))->Set( nlinc );
  double const xorigin = static_cast<GRA_distanceCharacteristic*>(axisC.Get(wxT("XORIGIN")))->GetAsWorld();
  double const yorigin = static_cast<GRA_distanceCharacteristic*>(axisC.Get(wxT("YORIGIN")))->GetAsWorld();
  double const length = static_cast<GRA_sizeCharacteristic*>(axisC.Get(wxT("AXISLENGTH")))->GetAsWorld();
  //
  static_cast<GRA_doubleCharacteristic*>(axisC.Get(wxT("MIN")))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(axisC.Get(wxT("MAX")))->Set( vmax );
  static_cast<GRA_doubleCharacteristic*>(axisC.Get(wxT("VIRTUALMIN")))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(axisC.Get(wxT("VIRTUALMAX")))->Set( vmax );
  static_cast<GRA_doubleCharacteristic*>(polarC->Get(wxT("MAX")))->Set( vmax );
  static_cast<GRA_doubleCharacteristic*>(polarC->Get(wxT("VIRTUALMAX")))->Set( vmax );
  //
  // if radar is true then theta=0 is vertical and up from the origin
  // otherwise, theta=0 is horizontal and to the right from the origin
  //
  if( radar )
  {
    static_cast<GRA_angleCharacteristic*>(axisC.Get(wxT("AXISANGLE")))->Set( 90.0 );
    compassLabels_ = true;
    clockwise_ = true;
  }
  double startAngle = static_cast<GRA_angleCharacteristic*>(axisC.Get(wxT("AXISANGLE")))->Get();
  axis_ = new GRA_axis( xorigin, yorigin, length, &axisC );
  //
  // fix the axis values for conversion to world coordinates
  // so the other axes can be made without confusing the conversion
  //
  double xw, yw;
  gw->PolarToWorld( r0_, theta0_, xw, yw, true );
  //
  int nAxes = static_cast<GRA_intCharacteristic*>(axisC.Get(wxT("NAXES")))->Get();
  static_cast<GRA_boolCharacteristic*>(axisC.Get(wxT("NUMBERSON")))->Set( false );
  for( int i=1; i<nAxes; ++i )
  {
    static_cast<GRA_angleCharacteristic*>(axisC.Get(wxT("AXISANGLE")))->Set( startAngle+i*360.0/nAxes );
    static_cast<GRA_intCharacteristic*>(axisC.Get(wxT("NLINCS")))->Set( nlinc );
    otherAxes_.push_back( new GRA_axis(xorigin,yorigin,length,&axisC) );
  }
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

void GRA_polarAxes::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  // draw the 0 degree radial axis
  //
  axis_->Draw( graphicsOutput, dc );
  DrawLabel( graphicsOutput, dc );
  //
  // draw the other axes
  //
  int nAxes = static_cast<int>(otherAxes_.size());
  for( int i=0; i<nAxes; ++i )otherAxes_[i]->Draw( graphicsOutput, dc );
  //
  // draw the grid lines
  // if nxgrid is zero, no tic coordinates are recorded, so no grid lines will be drawn
  //
  GRA_setOfCharacteristics *axisC = axis_->GetCharacteristics();
  //
  GRA_color *lineColor = static_cast<GRA_colorCharacteristic*>(axisC->Get(wxT("AXISCOLOR")))->Get();
  double xorigin, yorigin;
  axis_->GetOrigin( xorigin, yorigin );
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

  std::size_t ticSize = ticX.size();
  std::vector<double> x, y;
  std::vector<int> pen;
  int ngrid = abs(axis_->GetGrid());
  for( std::size_t i=0; i<ticSize; i+=ngrid )
  {
    double radius = sqrt( (xorigin-ticX[i])*(xorigin-ticX[i]) + (yorigin-ticY[i])*(yorigin-ticY[i]) );
    for( std::size_t j=0; j<101; ++j )
    {
      double angle = j*M_PI/50.;
      x.push_back( xorigin+radius*cos(angle) );
      y.push_back( yorigin+radius*sin(angle) );
      pen.push_back( 2 );
    }
    GRA_polyline p( x, y, pen, 1, gridLineType_, lineColor );
    p.Draw( graphicsOutput, dc );
    x.clear();
    y.clear();
  }
  //
  // draw the angles (or the compass directions) at the ends of the axes
  //
  double height = static_cast<GRA_sizeCharacteristic*>(axisC->Get(wxT("LABELHEIGHT")))->GetAsWorld();
  if( height > 0.0 )
  {
    GRA_color *color = static_cast<GRA_colorCharacteristic*>(axisC->Get(wxT("LABELCOLOR")))->Get();
    GRA_font *font = static_cast<GRA_fontCharacteristic*>(axisC->Get(wxT("LABELFONT")))->Get();
    double xend, yend, angle;
    wxString label;
    if( compassLabels_ && (nAxes==0||nAxes==1||nAxes==3||nAxes==7||nAxes==15) )
    {
      axis_->GetEndPoint( xend, yend );
      DrawAngle( label<<wxT("N"), xend, yend, axisAngle, height, color, font, graphicsOutput, dc );
      label.clear();
      switch (nAxes)
      {
        case 1:  // N, S
        {
          otherAxes_[0]->GetEndPoint( xend, yend );
          angle = otherAxes_[0]->GetAngle();
          DrawAngle( label<<wxT("S"), xend, yend, angle, height, color, font, graphicsOutput, dc );
          label.clear();
          break;
        }
        case 3:  // N, E, S, W
        {
          std::vector<wxString> labels(3);
          labels[0] = wxT("E");
          labels[1] = wxT("S");
          labels[2] = wxT("W");
          for( int i=0; i<3; ++i )
          {
            otherAxes_[i]->GetEndPoint( xend, yend );
            angle = otherAxes_[i]->GetAngle();
            if( clockwise_ )
              DrawAngle( labels[2-i], xend, yend, angle, height, color, font, graphicsOutput, dc );
            else
              DrawAngle( labels[i], xend, yend, angle, height, color, font, graphicsOutput, dc );
          }
          break;
        }
        case 7:  // N, NE, E, SE, S, SW, W, NW
        {
          std::vector<wxString> labels(7);
          labels[0] = wxT("NE");
          labels[1] = wxT("E");
          labels[2] = wxT("SE");
          labels[3] = wxT("S");
          labels[4] = wxT("SW");
          labels[5] = wxT("W");
          labels[6] = wxT("NW");
          for( int i=0; i<7; ++i )
          {
            otherAxes_[i]->GetEndPoint( xend, yend );
            angle = otherAxes_[i]->GetAngle();
            if( clockwise_ )
              DrawAngle( labels[6-i], xend, yend, angle, height, color, font, graphicsOutput, dc );
            else
              DrawAngle( labels[i], xend, yend, angle, height, color, font, graphicsOutput, dc );
          }
          break;
        }
        case 15:
        {
          std::vector<wxString> labels(15);
          labels[0] = wxT("NNE");
          labels[1] = wxT("NE");
          labels[2] = wxT("ENE");
          labels[3] = wxT("E");
          labels[4] = wxT("ESE");
          labels[5] = wxT("SE");
          labels[6] = wxT("SSE");
          labels[7] = wxT("S");
          labels[8] = wxT("SSW");
          labels[9] = wxT("SW");
          labels[10] = wxT("WSW");
          labels[11] = wxT("W");
          labels[12] = wxT("WNW");
          labels[13] = wxT("NW");
          labels[14] = wxT("NNW");
          for( int i=0; i<15; ++i )
          {
            otherAxes_[i]->GetEndPoint( xend, yend );
            angle = otherAxes_[i]->GetAngle();
            if( clockwise_ )
              DrawAngle( labels[14-i], xend, yend, angle, height, color, font, graphicsOutput, dc );
            else
              DrawAngle( labels[i], xend, yend, angle, height, color, font, graphicsOutput, dc );
          }
          break;
        }
      }
    }
    else
    {
      axis_->GetEndPoint( xend, yend );
      DrawAngle( label<<wxT("0<v1%,degree>"), xend, yend, axisAngle, height, color, font,
                 graphicsOutput, dc );
      label.clear();
      for( int i=0; i<nAxes; ++i )
      {
        angle = otherAxes_[i]->GetAngle();
        otherAxes_[i]->GetEndPoint( xend, yend );
        double drawAngle = angle - axisAngle;
        while( drawAngle < 0.0 )drawAngle += 360.0;
        if( clockwise_ )drawAngle = 360.0 - drawAngle;
        DrawAngle( label<<drawAngle<<wxT("<v1%,degree>"), xend, yend, angle, height, color, font,
                   graphicsOutput, dc );
        label.clear();
      }
    }
  }
}

void GRA_polarAxes::DrawAngle( wxString &label, double xend, double yend, double angle,
                               double height, GRA_color *color,
                               GRA_font *font, GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  int align = 4;
  if( 45 < angle && angle < 135 )       align = 2;
  else if( 135 <= angle && angle < 225 )align = 6;
  else if( 225 <= angle && angle < 315 )align = 8;
  //
  double xoff = 0.2*cos(angle*M_PI/180.);
  double yoff = 0.2*sin(angle*M_PI/180.);
  GRA_drawableText dt( label, height, 0.0, xend+xoff, yend+yoff, align, font, color );
  dt.Parse();
  dt.Draw( graphicsOutput, dc );
}

void GRA_polarAxes::DrawLabel( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_setOfCharacteristics *axisC = axis_->GetCharacteristics();
  //
  wxString label;
  bool labelIsOn = static_cast<GRA_boolCharacteristic*>(axisC->Get(wxT("LABELON")))->Get();
  if( labelIsOn )label = static_cast<GRA_stringCharacteristic*>(axisC->Get(wxT("LABEL")))->Get();
  if( label.empty() )labelIsOn = false;
  //
  // if powerAuto_ = 2, calculate the power but do not draw it
  //
  double power = static_cast<GRA_doubleCharacteristic*>(axisC->Get(wxT("POWER")))->Get();
  if( static_cast<GRA_intCharacteristic*>(axisC->Get(wxT("POWERAUTO")))->Get()!=2 &&
      static_cast<GRA_boolCharacteristic*>(axisC->Get(wxT("NUMBERSON")))->Get() && power!=0.0 )
  {
    wxString powLabel(wxT("(x10<^><Z0.5%>"));
    label << power << wxT("<_>)");
    label = label.empty() ? powLabel : label+wxT(" ")+powLabel;
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
    double sizlab = static_cast<GRA_sizeCharacteristic*>(axisC->Get(wxT("LABELHEIGHT")))->GetAsWorld();
    double imagTicAngle = static_cast<GRA_angleCharacteristic*>(axisC->Get(wxT("IMAGTICANGLE")))->Get();
    double imagTicLen = static_cast<GRA_sizeCharacteristic*>(axisC->Get(wxT("IMAGTICLENGTH")))->GetAsWorld();
    double numHeight = static_cast<GRA_sizeCharacteristic*>(axisC->Get(wxT("NUMBERSHEIGHT")))->GetAsWorld();
    GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(axisC->Get(wxT("LABELFONT")))->Get();
    GRA_color *labelColor = static_cast<GRA_colorCharacteristic*>(axisC->Get(wxT("LABELCOLOR")))->Get();
    double xOrigin, yOrigin;
    axis_->GetOrigin( xOrigin, yOrigin );
    double length = axis_->GetLength();
    double axisAngle = axis_->GetAngle();
    double x1, y1, angle;
    int align;
    //
    // draw the label horizontally and centered on the axis
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
    dt.Draw( graphicsOutput, dc );
  }
}

int GRA_polarAxes::NumberOfDigits( double mins, double maxs, double logBase )
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

std::ostream &operator<<( std::ostream &out, GRA_polarAxes const &ca )
{
  return out << "<polaraxes>\n" << *ca.axis_ << "</polaraxes>\n";
}

// end of file
