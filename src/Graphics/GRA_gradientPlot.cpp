/*
Copyright (C) 2005 Joseph L. Chuma, TRIUMF

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

#include "GRA_gradientPlot.h"
#include "EGraphicsError.h"
#include "GRA_bitmap.h"
#include "GRA_window.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_color.h"
#include "GRA_colorMap.h"
#include "GRA_colorControl.h"
#include "GRA_drawableText.h"
#include "GRA_cartesianAxes.h"
#include "GRA_wxWidgets.h"
#include "UsefulFunctions.h"
#include "ExGlobals.h"

GRA_gradientPlot::GRA_gradientPlot( std::vector<double> &x, std::vector<double> &y,
                                    std::vector<double> &z, int nrow, double fmin,
                                    double fmax, double gmin, double gmax,
                                    bool xProfile, bool yProfile, bool border, bool zoom,
                                    bool axes, bool reset, bool legend, bool linear )
    : GRA_densityPlot(wxT("GRADIENTPLOT"), x, y, z, nrow, fmin, fmax, gmin, gmax,
                      xProfile, yProfile, border, zoom, axes, reset, legend ),
      linear_(linear)
{
  if( xProfile_ || yProfile_ )MakeProfiles( 0, x_.size(), 0, y_.size() );
  colorMap_ = GRA_colorControl::GetColorMap();
}

void GRA_gradientPlot::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_cartesianAxes *axes =0;
  GRA_colorMap *colorMapSave = GRA_colorControl::GetColorMap();
  GRA_colorControl::SetColorMap( colorMap_ );
  try
  {
    DrawBitmap( graphicsOutput, dc );
    DrawAxes( graphicsOutput, dc, axes );
  }
  catch (EGraphicsError &e)
  {
    GRA_colorControl::SetColorMap( colorMapSave );
    throw;
  }
  DrawAxes2( graphicsOutput, dc, axes );
  GRA_colorControl::SetColorMap( colorMapSave );
}

void GRA_gradientPlot::DrawBitmap( GRA_wxWidgets *outputType, wxDC &dc )
{
  double amin, amax;
  UsefulFunctions::MinMax( z_, nrow_, 0, x_.size(), 0, y_.size(), amin, amax );
  //
  alo_ = amin + fmin_*(amax-amin);
  ahi_ = amin + fmax_*(amax-amin);
  if( ahi_-alo_ < std::numeric_limits<double>::epsilon() )
    throw EGraphicsError( wxT("region is flat") );
  double amean = (amin + amax) / 2.0;
  if( amean == 0.0 )amean = 1.e-10;
  if( fabs((amax-amin)/amean) < 1.0e-5 )
    throw EGraphicsError( wxT("region is flat") );
  //
  SetUp();
  //
  if( legend_ )DrawLegend( outputType, dc );
  //
  GRA_bitmap bitmap( xlo_, ylo_, xhi_, yhi_,
                     GRA_colorControl::GetColor(wxT("WHITE")), outputType );
  FillBitmap( &bitmap );
  bitmap.Draw( outputType, dc );
}

void GRA_gradientPlot::FillBitmap( GRA_bitmap *bitmap )
{
  int w = bitmap->GetWidth();
  int h = bitmap->GetHeight();
  //
  // for each print dot location, determine the corresponding
  // effective index of the a(m,n) array and evaluate the
  // appropriate density by 2-d linear (4 pt.) interpolation
  //
  GRA_colorMap *colorMap = GRA_colorControl::GetColorMap();
  int const nLevels = colorMap->GetSize();
  for( int i=0; i<h; ++i )
  {
    double ei = rnlo_ + (rnhi_-rnlo_)*i/(h-1.);
    int il;
    linear_ ? il=static_cast<int>(ei) : il=static_cast<int>(ei+0.5);
    if( il < nlo_ )il = nlo_;
    if( il > nhi_-1 )il = nhi_-1;
    int ih = il+1;
    double fi = ei-il;
    for( int j=0; j<w; ++j )
    {
      double ej = rmlo_ + (rmhi_-rmlo_)*j/(w-1.);
      int jl;
      linear_ ? jl=static_cast<int>(ej) : jl=static_cast<int>(ej+0.5);
      if( jl < mlo_ )jl = mlo_;
      if( jl > mhi_-1 )jl = mhi_-1;
      int jh = jl+1;
      double fj = ej-jl;
      //
      // interpolate array a to find value at (ei,ej) location
      //
      double fij;
      linear_ ? fij=(1.0-fi)*(1.0-fj)*z_[il-1+(jl-1)*nrow_] +
                     fi*(1.0-fj)*z_[ih-1+(jl-1)*nrow_] +
                     (1.0-fi)*fj*z_[il-1+(jh-1)*nrow_] +
                     fi*fj*z_[ih-1+(jh-1)*nrow_] :
                fij=z_[il-1+(jl-1)*nrow_];
      int index = static_cast<int>((fij-alo_)/(ahi_-alo_)*(nLevels-1)-.0001)%(nLevels-1)+1;
      bitmap->SetPixelColor( j, i, colorMap->GetColor(index) );
    }
  }
  if( border_ )bitmap->SetBorder( GRA_colorControl::GetColor(wxT("BLACK")) );
}

void GRA_gradientPlot::DrawLegend( GRA_wxWidgets *outputType, wxDC &dc )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  double xlwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
  double xuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
  double xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  double ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  double yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  double yticl = static_cast<GRA_sizeCharacteristic*>(yAxisC->Get(wxT("LARGETICLENGTH")))->GetAsWorld();
  double xlo = yProfile_ ? xuaxis+0.1*(xuwind-xlwind)+1.5*yticl : xuaxis+1.5*yticl;
  double xhi = xlo + (xuwind-xlo)/3.0;
  //
  double height = static_cast<GRA_sizeCharacteristic*>(generalC->Get(wxT("LEGENDSIZE")))->GetAsWorld();
  if( height > 0.0 )
  {
    wxChar c[10];
    ::wxSnprintf( c, 10, wxT("%9.2lg"), alo_ );
    double angle = 0.0;
    GRA_font *font = static_cast<GRA_fontCharacteristic*>(generalC->Get(wxT("FONT")))->Get();
    wxString stmp(c);
    GRA_drawableText dt( stmp, height, angle, xhi+0.1, ylaxis-height/2., 1,
                         font, GRA_colorControl::GetColor(wxT("BLACK")) );
    ::wxSnprintf( c, 10, wxT("%9.2lg"), ahi_ );
    stmp = wxString(c);
    GRA_drawableText dt2( stmp, height, angle, xhi+0.1, yuaxis-height/2., 1,
                          font, GRA_colorControl::GetColor(wxT("BLACK")) );
    try
    {
      dt.Parse();
      dt2.Parse();
    }
    catch ( EGraphicsError &e )
    {
      throw;
    }
    dt.Draw( outputType, dc );
    dt2.Draw( outputType, dc );
  }
  GRA_bitmap bitmap( xlo, ylaxis, xhi, yuaxis,
                     GRA_colorControl::GetColor(wxT("WHITE")), outputType );
  FillLegendBitmap( &bitmap );
  bitmap.SetBorder( GRA_colorControl::GetColor(wxT("BLACK")) );
  bitmap.Draw( outputType, dc );
}

void GRA_gradientPlot::FillLegendBitmap( GRA_bitmap *bitmap )
{
  int const w = bitmap->GetWidth();
  int const h = bitmap->GetHeight();
  //
  GRA_colorMap *colorMap = GRA_colorControl::GetColorMap();
  int const nLevels = colorMap->GetSize();
  //
  for( int j=0; j<h; ++j )
  {
    double frac = static_cast<double>(j)/(h-1.0);
    int index = static_cast<int>(1+frac*nLevels);
    for( int i=0; i<w; ++i )bitmap->SetPixelColor( i, j, colorMap->GetColor(index) );
  }
}

std::ostream &operator<<( std::ostream &out, GRA_gradientPlot const &gp )
{
  out << "<gradientplot nrow=\"" << gp.nrow_ << "\" fmin=\"" << gp.fmin_
      << "\" fmax=\"" << gp.fmax_ << "\" gmin=\"" << gp.gmin_ << "\" gmax=\""
      << gp.gmax_ << "\" xprofile=\"" << gp.xProfile_ << "\" yprofile=\""
      << gp.yProfile_ << "\" border=\"" << gp.border_ << "\" zoom=\""
      << gp.zoom_ << "\" axes=\"" << gp.drawAxes_ << "\" reset=\"" << gp.resetAxes_
      << "\" legend=\"" << gp.legend_ << "\" linear=\"" << gp.linear_ << "\">\n";
  int size = gp.x_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << gp.x_[i] << " ";
  out << "</data>\n";
  size = gp.y_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << gp.y_[i] << " ";
  out << "</data>\n";
  size = gp.z_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << gp.z_[i] << " ";
  return out << "</data>\n</gradientplot>\n";
}
// end of file
