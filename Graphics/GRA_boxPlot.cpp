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

#include "ExGlobals.h"
#include "GRA_window.h"
#include "UsefulFunctions.h"
#include "GRA_rectangle.h"
#include "EGraphicsError.h"
#include "GRA_cartesianCurve.h"
#include "GRA_cartesianAxes.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_colorCharacteristic.h"

#include "GRA_boxPlot.h"

void GRA_boxPlot::CopyStuff( GRA_boxPlot const &rhs )
{
  GRA_densityPlot::CopyStuff( rhs );
  amin_ = rhs.amin_;
  da_ = rhs.da_;
  xside_ = rhs.xside_;
  yside_ = rhs.yside_;
  bscale_ = rhs.bscale_;
}

void GRA_boxPlot::Make()
{
  int nx = x_.size();
  int ny = y_.size();
  int i1, i2, j1, j2;
  if( zoom_ )
  {
    j1 = nlo_;
    j2 = nhi_;
    i1 = mlo_;
    i2 = mhi_;
  }
  else
  {
    j1 = 1;
    j2 = ny;
    i1 = 1;
    i2 = nx;
  }
  //
  // Determine min and max values in array. The minimum value will
  // correspond to zero box size, the maximum to the largest box size.
  //
  // use linear density scale
  //
  double amax;
  if( nrow_ > 0 ) // a matrix and not x,y,z data
  {
    UsefulFunctions::MinMax( z_, nrow_, i1-1, i2, j1-1, j2, amin_, amax );
  }
  else                      // x,y,z data and not a matrix
  {
    UsefulFunctions::MinMax( z_, i1-1, i2, amin_, amax );
  }
  if( fmin_ < 0.0 )fmin_ = 0.0;
  amax += 0.05*(amax - amin_);
  //
  std::vector<double> xt( x_ ), yt( y_ );
  UsefulFunctions::QuickSort( xt, true ); // sort increasing
  double dx = xt[nx-1] - xt[0];
  UsefulFunctions::QuickSort( yt, true ); // sort increasing
  double dy = yt[ny-1] - yt[0];
  //
  double xdum = xt[0] - 1.0;
  int nxs = 0;
  for( int i=0; i<nx; ++i )
  {
    if( xt[i]-xdum > 0.001*dx )
    {
      ++nxs;
      xdum = xt[i];
    }
  }
  double ydum = yt[0] - 1.0;
  int nys = 0;
  for( int j=0; j<ny; ++j )
  {
    if( yt[j]-ydum > 0.001*dy )
    {
      ++nys;
      ydum = yt[j];
    }
  }
  //
  xside_ = (xhi_-xlo_)/std::max(1,nxs)*bscale_;
  yside_ = (yhi_-ylo_)/std::max(1,nys)*bscale_;
  //
  da_ = amax - amin_;
  double amaxx = amin_ + gmax_*da_;
  amin_ += gmin_*da_;
  amax = amaxx;
  da_ = amax==amin_ ? 1.0 : amax-amin_;
  //
  if( nrow_>0 && (xProfile_||yProfile_) )MakeProfiles( i1, i2, j1, j2 );
}

void GRA_boxPlot::Draw( GRA_outputType *graphicsOutput )
{
  GRA_cartesianAxes *axes =0;
  try
  {
    DrawAxes( graphicsOutput, axes );
  }
  catch (EGraphicsError &e)
  {
    throw;
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *dataC = gw->GetDataCurveCharacteristics();
  double xtmp, ytmp;
  gw->GraphToWorld( 0.0, 0.0, xtmp, ytmp, true );
  double xmin = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MIN"))->Get();
  double xmax = static_cast<GRA_doubleCharacteristic*>(xAxisC->Get("MAX"))->Get();
  double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MIN"))->Get();
  double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get("MAX"))->Get();
  GRA_color *fillColor = static_cast<GRA_colorCharacteristic*>(generalC->Get("AREAFILLCOLOR"))->Get();
  GRA_color *lineColor = static_cast<GRA_colorCharacteristic*>(dataC->Get("CURVECOLOR"))->Get();
  if( nrow_ > 0 ) // using the matrix and not x y z data
  {
    for( int i=mlo_-1; i<mhi_; ++i )
    {
      for( int j=nlo_-1; j<nhi_; ++j )
        DrawABox( x_[i], y_[j], z_[j+i*nrow_], xmin, xmax, ymin, ymax,
                  lineColor, fillColor, graphicsOutput );
    }
  }
  else               // using x y z data and not the matrix
  {
    int ny = y_.size();
    for( int j=0; j<ny; ++j )
    {
      if( (x_[j]-xmin)*(xmax-x_[j])>=0.0 && (y_[j]-ymin)*(ymax-y_[j])>=0.0 )
        DrawABox( x_[j], y_[j], z_[j], xmin, xmax, ymin, ymax,
                  lineColor, fillColor, graphicsOutput );
    }
  }
  DrawAxes2( graphicsOutput, axes );
}

void GRA_boxPlot::DrawABox( double xi, double yj, double z,
                            double xmin, double xmax, double ymin, double ymax,
                            GRA_color *lineColor, GRA_color *fillColor,
                            GRA_outputType *graphicsOutput )
{
  if( xi>xmax || xi<xmin || yj>ymax || yj<ymin )return;
  //
  double const frac = (z-amin_)/da_;
  if( frac<fmin_ || frac>fmax_ )return;
  //
  double x, y;
  ExGlobals::GetGraphWindow()->GraphToWorld( xi, yj, x, y );
  //
  std::auto_ptr<GRA_rectangle> r(
   new GRA_rectangle(x,y,xside_*frac,yside_*frac,0.0,lineColor,fillColor,1) );
  graphicsOutput->Draw( r.get() );
}

std::ostream &operator<<( std::ostream &out, GRA_boxPlot const &bp )
{
  out << "<boxplot nrow=\"" << bp.nrow_ << "\" fmin=\"" << bp.fmin_
      << "\" fmax=\"" << bp.fmax_ << "\" gmin=\"" << bp.gmin_ << "\" gmax=\""
      << bp.gmax_ << "\" xprofile=\"" << bp.xProfile_ << "\" yprofile=\""
      << bp.yProfile_ << "\" border=\"" << bp.border_ << "\" zoom=\""
      << bp.zoom_ << "\" axes=\"" << bp.drawAxes_ << "\" reset=\"" << bp.resetAxes_
      << "\" bscale=\"" << bp.bscale_ << "\">\n";
  int size = bp.x_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << bp.x_[i] << " ";
  out << "</data>\n";
  size = bp.y_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << bp.y_[i] << " ";
  out << "</data>\n";
  size = bp.z_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )out << bp.z_[i] << " ";
  return out << "</data>\n</boxplot>\n";
}
// end of file
