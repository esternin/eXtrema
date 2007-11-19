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
    : GRA_drawableObject(wxT("POLARAXES")), radar_(radar), r0_(r[0]), theta0_(theta[0])
{
  // for now, radar plots (0 degrees vertical) do not work properly
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
  //
  static_cast<GRA_intCharacteristic*>(polarC->Get(wxT("NUMBEROFDIGITS")))->Set(7);
  static_cast<GRA_intCharacteristic*>(polarC->Get(wxT("NUMBEROFDECIMALS")))->Set(-1);

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
  static_cast<GRA_intCharacteristic*>(polarC->Get(wxT("NLINCS")))->Set( nlinc );
  double const xorigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get(wxT("XORIGIN")))->GetAsWorld();
  double const yorigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get(wxT("YORIGIN")))->GetAsWorld();
  double const length = static_cast<GRA_sizeCharacteristic*>(polarC->Get(wxT("AXISLENGTH")))->GetAsWorld();
  //
  static_cast<GRA_doubleCharacteristic*>(polarC->Get(wxT("MIN")))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(polarC->Get(wxT("MAX")))->Set( vmax );
  static_cast<GRA_doubleCharacteristic*>(polarC->Get(wxT("VIRTUALMIN")))->Set( 0.0 );
  static_cast<GRA_doubleCharacteristic*>(polarC->Get(wxT("VIRTUALMAX")))->Set( vmax );
  bool numbersOn = static_cast<GRA_boolCharacteristic*>(polarC->Get(wxT("NUMBERSON")))->Get();
  //
  // if radar_ is true then theta=0 is vertical and up from the origin
  // otherwise, theta=0 is horizontal and to the right from the origin
  //
  double angle = radar_ ? 90.0 : 0.0;
  static_cast<GRA_angleCharacteristic*>(polarC->Get(wxT("AXISANGLE")))->Set( angle );
  axis_ = new GRA_axis( xorigin, yorigin, length, polarC );
  //
  // fix the axis values for conversion to world coordinates
  // so the other axes can be made without confusing the conversion
  //
  double xw, yw;
  gw->PolarToWorld( r0_, theta0_, xw, yw, true );
  //
  int nAxes = static_cast<GRA_intCharacteristic*>(polarC->Get(wxT("NAXES")))->Get();
  static_cast<GRA_boolCharacteristic*>(polarC->Get(wxT("NUMBERSON")))->Set( false );
  for( int i=1; i<nAxes; ++i )
  {
    static_cast<GRA_angleCharacteristic*>(polarC->Get(wxT("AXISANGLE")))->Set( angle+i*360.0/nAxes );
    static_cast<GRA_intCharacteristic*>(polarC->Get(wxT("NLINCS")))->Set( nlinc );
    otherAxes_.push_back( new GRA_axis(xorigin,yorigin,length,polarC) );
  }
  static_cast<GRA_boolCharacteristic*>(polarC->Get(wxT("NUMBERSON")))->Set( numbersOn );
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
  radar_ = rhs.radar_;
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
  // draw the grid lines
  // if nxgrid is zero, no tic coordinates are recorded, so no grid lines will be drawn
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  //
  GRA_color *lineColor = static_cast<GRA_colorCharacteristic*>(polarC->Get(wxT("AXISCOLOR")))->Get();
  int lineType = static_cast<GRA_intCharacteristic*>(generalC->Get(wxT("GRIDLINETYPE")))->Get();
  double const xorigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get(wxT("XORIGIN")))->GetAsWorld();
  double const yorigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get(wxT("YORIGIN")))->GetAsWorld();
  //
  // grid circles are drawn at tic marks on the x-axis
  //
  std::vector<double> ticX, ticY;
  axis_->GetTicCoordinates( ticX, ticY );
  double axisAngle = axis_->GetAngle();
  std::size_t size = ticX.size();
  std::vector<double> x, y;
  std::vector<int> pen;
  int ngrid = abs(static_cast<GRA_intCharacteristic*>(polarC->Get(wxT("GRID")))->Get());
  for( std::size_t i=0; i<size; i+=ngrid )
  {
    double radius = axisAngle==0.0 ? ticX[i]-xorigin : ticY[i]-yorigin;
    for( std::size_t j=0; j<101; ++j )
    {
      double angle = j*M_PI/50.;
      x.push_back( xorigin+radius*cos(angle) );
      y.push_back( yorigin+radius*sin(angle) );
      pen.push_back( 2 );
    }
    GRA_polyline p( x, y, pen, 1, lineType, lineColor );
    p.Draw( graphicsOutput, dc );
    x.clear();
    y.clear();
  }
  axis_->Draw( graphicsOutput, dc );
  DrawLabel( graphicsOutput, dc );
  double height = static_cast<GRA_sizeCharacteristic*>(polarC->Get(wxT("NUMBERSHEIGHT")))->GetAsWorld();
  GRA_color *color = static_cast<GRA_colorCharacteristic*>(polarC->Get(wxT("LABELCOLOR")))->Get();
  GRA_font *font = static_cast<GRA_fontCharacteristic*>(polarC->Get(wxT("LABELFONT")))->Get();
  try
  {
    wxString label;
    DrawAngle( label<<wxT("0<v1%,degree>"), axis_, height, color, font, graphicsOutput, dc );
  }
  catch ( EGraphicsError const &e )
  {
    throw;
  }
  size = otherAxes_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    otherAxes_[i]->Draw( graphicsOutput, dc );
    double angle = otherAxes_[i]->GetAngle();
    if( radar_ )angle -= 90.0;
    if( angle < 0.0 )angle += 360.0;
    try
    {
      wxString label;
      DrawAngle( label<<angle<<wxT("<v1%,degree>"), otherAxes_[i], height, color, font, graphicsOutput, dc );
    }
    catch ( EGraphicsError const &e )
    {
      throw;
    }
  }
}

void GRA_polarAxes::DrawAngle( wxString &label, GRA_axis *axis, double height, GRA_color *color,
                               GRA_font *font, GRA_wxWidgets *graphicsOutput, wxDC &dc )
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
  dt.Draw( graphicsOutput, dc );
}

void GRA_polarAxes::DrawLabel( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
  //
  wxString label;
  bool labelIsOn = static_cast<GRA_boolCharacteristic*>(polarC->Get(wxT("LABELON")))->Get();
  if( labelIsOn )label = static_cast<GRA_stringCharacteristic*>(polarC->Get(wxT("LABEL")))->Get();
  if( label.empty() )labelIsOn = false;
  //
  // if powerAuto_ = 2, calculate the power but do not draw it
  //
  double power = static_cast<GRA_doubleCharacteristic*>(polarC->Get(wxT("POWER")))->Get();
  if( static_cast<GRA_intCharacteristic*>(polarC->Get(wxT("POWERAUTO")))->Get()!=2 &&
      static_cast<GRA_boolCharacteristic*>(polarC->Get(wxT("NUMBERSON")))->Get() && power!=0.0 )
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
    double sizlab = static_cast<GRA_sizeCharacteristic*>(polarC->Get(wxT("LABELHEIGHT")))->GetAsWorld();
    double axisAngle = static_cast<GRA_angleCharacteristic*>(polarC->Get(wxT("AXISANGLE")))->Get();
    double imagTicAngle = static_cast<GRA_angleCharacteristic*>(polarC->Get(wxT("IMAGTICANGLE")))->Get();
    double imagTicLen = static_cast<GRA_sizeCharacteristic*>(polarC->Get(wxT("IMAGTICLENGTH")))->GetAsWorld();
    double numHeight = static_cast<GRA_sizeCharacteristic*>(polarC->Get(wxT("NUMBERSHEIGHT")))->GetAsWorld();
    GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(polarC->Get(wxT("LABELFONT")))->Get();
    GRA_color *labelColor = static_cast<GRA_colorCharacteristic*>(polarC->Get(wxT("LABELCOLOR")))->Get();
    double length = static_cast<GRA_sizeCharacteristic*>(polarC->Get(wxT("AXISLENGTH")))->GetAsWorld();
    double xOrigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get(wxT("XORIGIN")))->GetAsWorld();
    double yOrigin = static_cast<GRA_distanceCharacteristic*>(polarC->Get(wxT("YORIGIN")))->GetAsWorld();
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
