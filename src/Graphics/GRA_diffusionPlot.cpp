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

#include "GRA_diffusionPlot.h"
#include "EGraphicsError.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_wxWidgets.h"
#include "GRA_bitmap.h"
#include "UsefulFunctions.h"

void GRA_diffusionPlot::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_cartesianAxes *axes =0;
  DrawBitmap( graphicsOutput, dc );
  DrawAxes( graphicsOutput, dc, axes );
  DrawAxes2( graphicsOutput, dc, axes );
}

void GRA_diffusionPlot::DrawBitmap( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  //
  // Determine min and max values in array. The minimum value
  // will correspond to zero (blank) print density, the maximum
  // to solid black print
  //
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
  GRA_bitmap bitmap( xlo_, ylo_, xhi_, yhi_,
                     GRA_colorControl::GetColor(wxT("WHITE")), graphicsOutput );
  FillBitmap( &bitmap );
  bitmap.Draw( graphicsOutput, dc );
}

void GRA_diffusionPlot::FillBitmap( GRA_bitmap *bitmap )
{
  GRA_color *color = GRA_colorControl::GetColor(wxT("BLACK"));
  //
  int w = bitmap->GetWidth();
  int h = bitmap->GetHeight();
  //
  // For each print dot location, determine the corresponding
  // effective index of the array and evaluate the
  // appropriate density by 2-d linear (4 pt.) interpolation.
  //
  std::vector<double> d1( w+4, 0.0 );
  std::vector<double> d2( w+4, 0.0 );
  std::vector<double> d3( w+4, 0.0 );
  //
  for( int i=0; i<h; ++i )
  {
    double ei = rnlo_ + (rnhi_-rnlo_)*i/(h-1.);
    int il = static_cast<int>(ei);
    if( il < nlo_ )il = nlo_;
    if( il > nhi_-1 )il = nhi_-1;
    int ih = il + 1;
    double fi = ei - il;
    for( int j=0; j<w; ++j )
    {
      double ej = rmlo_ + (rmhi_-rmlo_)*j/(w-1.);
      int jl = static_cast<int>(ej);
      if( jl < mlo_ )jl = mlo_;
      if( jl > mhi_-1 )jl = mhi_-1;
      int jh = jl + 1;
      double fj = ej - jl;
      //
      // interpolate array to find value at (ei,ej) location
      //
      double fij = (1.0-fi)*(1.0-fj)*z_[il-1+(jl-1)*nrow_] +
                   fi*(1.0-fj)*z_[ih-1+(jl-1)*nrow_] +
                   (1.0-fi)*fj*z_[il-1+(jh-1)*nrow_] +
                   fi*fj*z_[ih-1+(jh-1)*nrow_];
      if( fij > ahi_ )fij = ahi_;
      double dij = (fij-alo_)/(ahi_-alo_);
      if( dij < 0.0 )dij = 0.0;
      if( dij > 1.0 )dij = 1.0;
      dij += d1[j+2];
      double dout;
      if( dij >= 0.5 )
      {
        try
        {
          bitmap->SetPixelColor( j, i, color );
        }
        catch (EGraphicsError &e)
        {}
        dout = 1.0;
      }
      else
      {
        dout = 0.0;
      }
      double err = dij - dout;
      d1[j+3] += err*7.0/48.0;
      d1[j+4] += err*5.0/48.0;
      d2[j]   += err*3.0/48.0;
      d2[j+1] += err*5.0/48.0;
      d2[j+2] += err*7.0/48.0;
      d2[j+3] += err*5.0/48.0;
      d2[j+4] += err*3.0/48.0;
      d3[j]   += err*1.0/48.0;
      d3[j+1] += err*3.0/48.0;
      d3[j+2] += err*5.0/48.0;
      d3[j+3] += err*3.0/48.0;
      d3[j+4] += err*1.0/48.0;
    }
    for( int j=0; j<w+4; ++j )
    {
      d1[j] = d2[j];
      d2[j] = d3[j];
    }
    d3.assign( w+4, 0.0 );
  }
}

std::ostream &operator<<( std::ostream &out, GRA_diffusionPlot const &dp )
{
  out << "<diffusionplot nrow=\"" << dp.nrow_ << "\" fmin=\"" << dp.fmin_
      << "\" fmax=\"" << dp.fmax_ << "\" gmin=\"" << dp.gmin_ << "\" gmax=\""
      << dp.gmax_ << "\" xprofile=\"" << dp.xProfile_ << "\" yprofile=\""
      << dp.yProfile_ << "\" border=\"" << dp.border_ << "\" zoom=\""
      << dp.zoom_ << "\" axes=\"" << dp.drawAxes_ << "\" reset=\"" << dp.resetAxes_ << "\">\n";
  int size = dp.x_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << dp.x_[i] << " ";
  out << "</data>\n";
  size = dp.y_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << dp.y_[i] << " ";
  out << "</data>\n";
  size = dp.z_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << dp.z_[i] << " ";
  return out << "</data>\n</diffusionplot>\n";
}
// end of file
