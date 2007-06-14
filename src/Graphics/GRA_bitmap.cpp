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

#include "GRA_bitmap.h"
#include "EGraphicsError.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_wxWidgets.h"
#include "ExGlobals.h"

GRA_bitmap::GRA_bitmap( double xlo, double ylo, double xhi, double yhi,
                        GRA_color *backgroundColor, GRA_wxWidgets *outputType )
  : xLo_(xlo), yLo_(ylo), xHi_(xhi), yHi_(yhi)
{
  // find the distance between pixels in world coordinate units
  //
  double xminW, yminW, xmaxW, ymaxW;
  ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
  double const dxW = fabs(xmaxW-xminW);
  double const dyW = fabs(ymaxW-yminW);
  //
  double xminM, yminM, xmaxM, ymaxM;
  if( outputType )
  {
    int ix1, iy1, ix2, iy2;
    outputType->GetLimits( ix1, iy1, ix2, iy2 );
    xminM = static_cast<double>(ix1);
    yminM = static_cast<double>(iy1);
    xmaxM = static_cast<double>(ix2);
    ymaxM = static_cast<double>(iy2);
  }
  else
  {
    // this section is for the PostScript driver
    xminM = xminW*72;
    yminM = yminW*72;
    xmaxM = xmaxW*72;
    ymaxM = ymaxW*72;
  }
  //
  double const dxM = fabs(xmaxM-xminM);
  double const dyM = fabs(ymaxM-yminM);
  //
  xInc_ = dxW/dxM; // horizontal distance (cm/pixel)
  yInc_ = dyW/dyM; // vertical distance   (cm/pixel)
  //
  ixLo_ = static_cast<int>(xminM + xLo_/xInc_+0.5);  // pixel number for xlo
  iyLo_ = static_cast<int>(yminM + yLo_/yInc_+0.5);  // pixel number for ylo
  ixHi_ = static_cast<int>(xminM + xHi_/xInc_+0.5);  // pixel number for xhi
  iyHi_ = static_cast<int>(yminM + yHi_/yInc_+0.5);  // pixel number for yhi
  //
  height_ = iyHi_ - iyLo_ + 1;
  width_ = ixHi_ - ixLo_ + 1;
  bitmap_.insert( bitmap_.begin(), height_*width_, backgroundColor );
}

void GRA_bitmap::CopyStuff( GRA_bitmap const &rhs )
{
  colorVec().swap( bitmap_ ); // empty out bitmap_
  //
  // now copy rhs.bitmap_ into bitmap_
  //
  bitmap_.assign( rhs.bitmap_.begin(), rhs.bitmap_.end() );
  //
  xLo_ = rhs.xLo_;
  xHi_ = rhs.xHi_;
  yLo_ = rhs.yLo_;
  yHi_ = rhs.yHi_;
  ixLo_ = rhs.ixLo_;
  ixHi_ = rhs.ixHi_;
  iyLo_ = rhs.iyLo_;
  iyHi_ = rhs.iyHi_;
  width_ = rhs.width_;
  height_ = rhs.height_;
  xInc_ = rhs.xInc_;
  yInc_ = rhs.yInc_;
}

void GRA_bitmap::SetBorder( GRA_color *color )
{
  for( int i=0; i<width_; ++i )
  {
    bitmap_[i] = color;
    bitmap_[i+width_*(height_-1)] = color;
  }
  for( int j=0; j<height_; ++j )
  {
    bitmap_[j*width_] = color;
    bitmap_[width_-1+j*width_] = color;
  }
}

void GRA_bitmap::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  int xmin, ymin, xmax, ymax;
  graphicsOutput->GetLimits( xmin, ymin, xmax, ymax );
  for( int x=0; x<width_; ++x )
  {
    for( int y=0; y<height_; ++y )
    {
      wxPen wxpen( dc.GetPen() );
      wxpen.SetColour( ExGlobals::GetwxColor(bitmap_[x+y*width_]) );
      dc.SetPen( wxpen );
      dc.DrawPoint( x+ixLo_, ymax-(y+iyLo_) );
    }
  }
}

// end of file
