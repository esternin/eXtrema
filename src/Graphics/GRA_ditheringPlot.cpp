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
#include <limits>
#include <cmath>

#include "GRA_ditheringPlot.h"
#include "EGraphicsError.h"
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
#include "GRA_bitmap.h"
#include "UsefulFunctions.h"
#include "ExGlobals.h"

void GRA_ditheringPlot::CopyStuff( GRA_ditheringPlot const &rhs )
{
  GRA_densityPlot::CopyStuff( rhs );
  equallySpaced_ = rhs.equallySpaced_;
  areas_ = rhs.areas_;
  volumes_ = rhs.volumes_;
  lines_ = rhs.lines_;
  std::vector<double>().swap( contourLevels_ );
  contourLevels_.assign( rhs.contourLevels_.begin(), rhs.contourLevels_.end() );
  std::vector<int>().swap( dither_ );
  dither_.assign( rhs.dither_.begin(), rhs.dither_.end() );
}

void GRA_ditheringPlot::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_cartesianAxes *axes =0;
  DrawBitmap( graphicsOutput, dc );
  DrawAxes( graphicsOutput, dc, axes );
  DrawAxes2( graphicsOutput, dc, axes );
}

void GRA_ditheringPlot::DrawBitmap( GRA_wxWidgets *outputType, wxDC &dc )
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
  std::size_t const nLevels = dither_.size()/2;
  if( areas_ )areaVec_.resize( nLevels, 0.0 );
  if( volumes_ )volumeVec_.resize( nLevels, 0.0 );
  //
  contourVec_.resize( nLevels+1, 0.0 );
  contourVec_[0] = alo_;
  if( contourLevels_.empty() )
  {
    for( std::size_t i=1; i<nLevels; ++i )contourVec_[i] = alo_+i*(ahi_-alo_)/nLevels ;
  }
  else
  {
    if( contourLevels_[0] <= amin )
      throw EGraphicsError( wxT("first contour level <= data minimum") );
    if( contourLevels_[nLevels-2] >= amax )
      throw EGraphicsError( wxT("last contour level >= data maximum") );
    for( std::size_t i=2; i<=nLevels; ++i )contourVec_[i-1] = contourLevels_[i-2];
  }
  contourVec_[nLevels] = ahi_;
  //
  if( legend_ )DrawLegend( outputType, dc );
  //
  GRA_bitmap bitmap( xlo_, ylo_, xhi_, yhi_,
                     GRA_colorControl::GetColor(wxT("WHITE")), outputType );
  FillBitmap( &bitmap );
  bitmap.Draw( outputType, dc );
}

void GRA_ditheringPlot::FillBitmap( GRA_bitmap *bitmap )
{
  int w = bitmap->GetWidth();
  int h = bitmap->GetHeight();
  //
  // now, for each print dot location, determine the corresponding
  // effective index of the a(m,n) array and evaluate the
  // appropriate density by 2-d linear (4 pt.) interpolation
  //
  std::size_t const nLevels = dither_.size()/2;
  std::vector<int> iicc( h*w, 0 );
  double totalVolume = 0.0;
  double totalArea = w*h;
  for( int i=0; i<h; ++i )
  {
    double ei = rnlo_ + (rnhi_-rnlo_)*i/(h-1.);
    int il = static_cast<int>(ei);
    if( il < nlo_ )il = nlo_;
    if( il > nhi_-1 )il = nhi_-1;
    int ih = il+1;
    double fi = ei-il;
    for( int j=0; j<w; ++j )
    {
      double ej = rmlo_ + (rmhi_-rmlo_)*j/(w-1.);
      int jl = static_cast<int>(ej);
      if( jl < mlo_ )jl = mlo_;
      if( jl > mhi_-1 )jl = mhi_-1;
      int jh = jl+1;
      double fj = ej-jl;
      //
      // interpolate array to find value at (ei,ej) location
      //
      double fij = (1.0-fi)*(1.0-fj)*z_[il-1+(jl-1)*nrow_] +
                   fi*(1.0-fj)*z_[ih-1+(jl-1)*nrow_] +
                   (1.0-fi)*fj*z_[il-1+(jh-1)*nrow_] +
                   fi*fj*z_[ih-1+(jh-1)*nrow_];
      int icc;
      if( fij <= alo_ )
      {
        fij = ahi_;
        icc = 1;
      }
      else if( fij > ahi_ )
      {
        fij = ahi_;
        icc = nLevels;
      }
      else
      {
        for( std::size_t ii=1; ii<=nLevels; ++ii )
        {
          if( fij > contourVec_[ii-1] )
          {
            if( fij <= contourVec_[ii] )
            {
              icc = ii;
              break;
            }
          }
        }
      }
      if( areas_ )areaVec_[icc-1] += 1.0;
      if( volumes_ )
      {
        totalVolume += fabs(fij);
        volumeVec_[icc-1] += fabs(fij);
      }
      iicc[j+i*w] = icc;
      std::size_t ndx = nLevels-icc+1;
      std::size_t indx = 2*ndx-1;
      std::size_t jndx = 2*ndx;
      int ilv = dither_[indx-1];
      int jlv = dither_[jndx-1];
      bool plot = false;
      if( ilv > 0 )
      {
        if( jlv > 0 )
        {
          if( (i+1)%ilv==0 && (j+1)%jlv==0 )plot = true;
        }
        else if( jlv < 0 )
        {
          if( (i+1)%ilv==0 && (j+1)%(-jlv)!=0 )plot = true;
        }
      }
      else if( ilv < 0 )
      {
        if( jlv > 0 )
        {
          if( (i+1)%(-ilv)!=0 && (j+1)%jlv==0 )plot = true;
        }
        else if( jlv < 0 )
        {
          if( (i+1)%(-ilv)!=0 && (j+1)%(-jlv)!=0 )plot = true;
        }
      }
      if( plot )iicc[j+i*w] *= -1;
    }
  }
  if( areas_ )
  {
    for( std::size_t i=0; i<nLevels; ++i )areaVec_[i] *= 100.0/totalArea;
  }
  if( volumes_ )
  {
    for( std::size_t i=0; i<nLevels; ++i )volumeVec_[i] *= 100.0/totalVolume;
  }
  //
  // draw the dithering pattern
  //
  for( int i=0; i<h; ++i )
  {
    for( int j=0; j<w; ++j )
    {
      if( iicc[j+i*w] < 0 )bitmap->SetPixelColor( j, i, GRA_colorControl::GetColor(wxT("BLACK")) );
    }
  }
  if( lines_ )  // draw the contour lines processed vertically
  {
    int iccold = 0;
    for( int j=0; j<w; ++j )
    {
      for( int i=0; i<h; ++i )
      {
        int icc = abs(iicc[j+i*w]);
        if( icc>0 && icc!=iccold )bitmap->SetPixelColor( j, i, GRA_colorControl::GetColor(wxT("BLACK")) );
        iccold = icc;
      }
    }
    //
    // draw the contour lines processed horizontally
    //
    iccold = 0;
    for( int i=0; i<h; ++i )
    {
      for( int j=0; j<w; ++j )
      {
        int icc = abs(iicc[j+i*w]);
        if( icc>0 && icc!=iccold )bitmap->SetPixelColor( j, i, GRA_colorControl::GetColor(wxT("BLACK")) );
        iccold = icc;
      }
    }
  }
}

void GRA_ditheringPlot::DrawLegend( GRA_wxWidgets *outputType, wxDC &dc )
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
  std::size_t nLevels = dither_.size()/2;
  double yi = (yuaxis-ylaxis)/nLevels;
  //
  if( height > 0.0 )
  {
    for( std::size_t i=0; i<=nLevels; ++i )
    {
      double yp = ylaxis + i*yi;
      wxChar c[50];
      ::wxSnprintf( c, 50, wxT("%9.2le"), contourVec_[i] );
      double angle = 0.0;
      GRA_font *font = static_cast<GRA_fontCharacteristic*>(generalC->Get(wxT("FONT")))->Get();
      wxString stmp(c);
      GRA_drawableText dt( stmp, height, angle, xhi+0.1, yp-height/2., 1,
                           font, GRA_colorControl::GetColor(wxT("BLACK")) );
      dt.Parse();
      dt.Draw( outputType, dc );
    }
  }
  GRA_bitmap bitmap( xlo, ylaxis, xhi, yuaxis,
                     GRA_colorControl::GetColor(wxT("WHITE")), outputType );
  FillLegendBitmap( &bitmap );
  bitmap.SetBorder( GRA_colorControl::GetColor(wxT("BLACK")) );
  bitmap.Draw( outputType, dc );
}

void GRA_ditheringPlot::FillLegendBitmap( GRA_bitmap *bitmap )
{
  int w = bitmap->GetWidth();
  int h = bitmap->GetHeight();
  //
  std::size_t const nLevels = dither_.size()/2;
  std::vector<int> leg( nLevels );
  int sum = 0;
  for( std::size_t i=0; i<nLevels; ++i )
  {
    if( equallySpaced_ )
      leg[i] = static_cast<int>(static_cast<double>(h)/nLevels+0.5);
    else
      leg[i] = static_cast<int>(h*(contourVec_[i+1]-contourVec_[i])/(ahi_-alo_)+0.5);
    sum += leg[i];
  }
  if( sum > h )leg[nLevels-1] -= sum-h;
  if( sum < h )leg[nLevels-1] += h-sum;
  int ny = 0;
  for( std::size_t icc=1; icc<=nLevels; ++icc )
  {
    std::size_t ndx = nLevels-icc+1;
    std::size_t indx = 2*ndx-1;
    std::size_t jndx = 2*ndx;
    int ilv  = dither_[indx-1];
    int jlv  = dither_[jndx-1];
    for( int i=0; i<leg[icc-1]; ++i )
    {
      for( int j=0; j<w; ++j )
      {
        bool plot = false;
        if( ilv > 0 )
        {
          if( jlv > 0 )
          {
            if( (i+1)%ilv==0 && (j+1)%jlv==0 )plot = true;
          }
          else if( ilv < 0 )
          {
            if( (i+1)%ilv==0 && (j+1)%(-jlv)!=0 )plot = true;
          }
        }
        else if( ilv < 0 )
        {
          if( jlv > 0 )
          {
            if( (i+1)%(-ilv)!=0 && (j+1)%jlv==0 )plot = true;
          }
          else if( ilv < 0 )
          {
            if( (i+1)%(-ilv)!=0 && (j+1)%(-jlv)!=0 )plot = true;
          }
        }
        if( plot )bitmap->SetPixelColor( j, ny, GRA_colorControl::GetColor(wxT("BLACK")) );
      }
      if( ++ny == h-1 )break;
    }
    for( int j=0; j<w; ++j )bitmap->SetPixelColor( j, ny, GRA_colorControl::GetColor(wxT("BLACK")) );
  }
}

std::ostream &operator<<( std::ostream &out, GRA_ditheringPlot const &dp )
{
  out << "<ditheringplot nrow=\"" << dp.nrow_ << "\" fmin=\"" << dp.fmin_
      << "\" fmax=\"" << dp.fmax_ << "\" gmin=\"" << dp.gmin_ << "\" gmax=\""
      << dp.gmax_ << "\" xprofile=\"" << dp.xProfile_ << "\" yprofile=\""
      << dp.yProfile_ << "\" border=\"" << dp.border_ << "\" zoom=\""
      << dp.zoom_ << "\" axes=\"" << dp.drawAxes_ << "\" reset=\"" << dp.resetAxes_
      << "\" legend=\"" << dp.legend_ << "\" equalspaced=\"" << dp.equallySpaced_
      << "\" areas=\"" << dp.areas_ << "\" volumes=\"" << dp.volumes_ << "\" lines=\""
      << dp.lines_ << "\">\n";
  int size = dp.dither_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << dp.dither_[i] << ' ';
  out << "</data>\n";
  size = dp.contourLevels_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << dp.contourLevels_[i] << ' ';
  out << "</data>\n";
  size = dp.x_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << dp.x_[i] << ' ';
  out << "</data>\n";
  size = dp.y_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << dp.y_[i] << ' ';
  out << "</data>\n";
  size = dp.z_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << dp.z_[i] << ' ';
  return out << "</data>\n</ditheringplot>\n";
}
// end of file
