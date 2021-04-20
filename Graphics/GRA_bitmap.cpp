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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "EGraphicsError.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_outputType.h"
#include "ExGlobals.h"

#include "GRA_bitmap.h"

GRA_bitmap::GRA_bitmap( double xlo, double ylo, double xhi, double yhi,
                        GRA_color *backgroundColor, GRA_outputType *outputType )
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
  /*
  if( outputType->IsPrinter() )
  {
    int xmn, ymn, xmx, ymx;
    ExGlobals::GetMonitorLimits( xmn, ymn, xmx, ymx );
    xminM = static_cast<double>(xmn);
    yminM = static_cast<double>(ymn);
    xmaxM = static_cast<double>(xmx);
    ymaxM = static_cast<double>(ymx);
  }
  else outputType->GetLimits( xminM, yminM, xmaxM, ymaxM );
  */
  outputType->GetLimits( xminM, yminM, xmaxM, ymaxM );
  double const dxM = fabs(xmaxM-xminM);
  double const dyM = fabs(ymaxM-yminM);
  //
  double const xinc = dxW/dxM; // horizontal distance (cm/pixel)
  double const yinc = dyW/dyM; // vertical distance   (cm/pixel)
  //
  ixLo_ = static_cast<int>(xminM + xLo_/xinc+0.5) + 1;  // pixel number for xlo
  iyLo_ = static_cast<int>(yminM + yLo_/yinc+0.5) + 1;  // pixel number for ylo
  ixHi_ = static_cast<int>(xminM + xHi_/xinc+0.5) + 1;  // pixel number for xhi
  iyHi_ = static_cast<int>(yminM + yHi_/yinc+0.5) + 1;  // pixel number for yhi
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

// end of file
