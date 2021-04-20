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
#include "EGraphicsError.h"
#include "GRA_setOfCharacteristics.h"

#include "GRA_threeDFigure.h"

GRA_threeDFigure::GRA_threeDFigure( std::vector<double> const &m,
                                    std::size_t nRow, std::size_t nCol,
                                    double theta, double phi,
                                    double vsf, double pdsf,
                                    bool color, bool histogram )
    : GRA_drawableObject("THREEDFIGURE"), nRow_(nRow), nCol_(nCol), phi_(phi),
      verticalScaleFactor_(vsf), pedestalSizeScaleFactor_(pdsf),
      color_(color), histogram_(histogram)
{
  theta_ = -1.0*fabs(theta); // theta should always be negative
  double absmax = -1.0;
  aMin_ = m[0];
  aMax_ = aMin_;
  for( std::size_t i=0; i<nCol_*nRow_; ++i )
  {
    if( absmax < fabs(m[i]) )absmax = fabs(m[i]);
    if( aMin_ > m[i] )aMin_ = m[i];
    if( aMax_ < m[i] )aMax_ = m[i];
  }
  //
  // scale the data so the max value of the histogram is 32767
  //
  double abm = absmax + fabs(aMin_);
  double scale = 1.0;
  if( abm != 0.0 )scale = 32766.0/abm;
  for( std::size_t i=0; i<nRow_*nCol_; ++i )
    matrix_.push_back( static_cast<int>(m[i]*scale) );
  aMin_ *= scale;
  aMax_ *= scale;
  //
  double const deg2rad = M_PI/180.0;
  //
  // calculate elements of rotation matrix
  //
  double sphi = sin(deg2rad*phi_);
  double cphi = cos(deg2rad*phi_);
  double stheta = sin(deg2rad*theta_);
  double ctheta = cos(deg2rad*theta_);
  a11 = cphi;
  a12 = 0.0;
  a13 = -sphi;
  a21 = stheta*sphi;
  a22 = ctheta;
  a23 = stheta*cphi;
  a31 = ctheta*sphi;
  a32 = -stheta;
  a33 = ctheta*cphi;
  //
  // set plotter parameters
  //
  pipi = 50;
  nypi = 11*pipi;
  //
  // adjust dimensions
  //
  yLength_ = 40.0/2.54;
  xLength_ = yLength_*1023.0/780.0;
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  xLwind_ = static_cast<GRA_distanceCharacteristic*>(generalC->Get("XLOWERWINDOW"))->GetAsWorld();
  xUwind_ = static_cast<GRA_distanceCharacteristic*>(generalC->Get("XUPPERWINDOW"))->GetAsWorld();
  yLwind_ = static_cast<GRA_distanceCharacteristic*>(generalC->Get("YLOWERWINDOW"))->GetAsWorld();
  yUwind_ = static_cast<GRA_distanceCharacteristic*>(generalC->Get("YUPPERWINDOW"))->GetAsWorld();
  xF_ = (xUwind_-xLwind_)/xLength_;
  yF_ = (yUwind_-yLwind_)/yLength_;
}

std::ostream &operator<<( std::ostream &out, GRA_threeDFigure const &tdf )
{ return out; }

void GRA_threeDFigure::Draw()
{
  // display a 2-dimensional histogram in the form
  // of a 3-dimensional figure which can be rotated in space
  //
  // the following display options are available:
  //   1. plotting in "line" or "bar-graph" mode
  //
  // description of parameters:
  //   matrix_              - 2-dim. array containing histogram
  //   nRow_                - limit of 1st subscript of array
  //   nCol_                - limit of 2nd subscript of array
  //   verticalScaleFactor_ - expansion factor along y-axis
  //                            (max. value of histogram is assumed to be 32767)
  //   theta_               - rotaion angle about x-axis
  //   phi_                 - rotation angle about y-axis
  //   histogram_           - plot mode
  //                             0 - "line" i.e. histogram is represented by points joined 
  //                                 with straight lines
  //                             1 - "bar-graph" i.e. hist. is represented by rectangular bars
  //
  //
  // set values which determine order in which lines are to be plotted
  //
  int idx = 1;
  int idz = 1;
  if(  a31*nCol_ > 0.0 )idx = -1;
  if( -a33*nRow_ > 0.0 )idz = -1;
  int jdx = -(idx-1)/2;
  int jdz = -(idz-1)/2;
  int jxs = (nCol_+1)*jdx;
  int jzs = (nRow_+1)*jdz;
  //
  // determine plot scaling factors and offsets
  //
  double const xLen = 14.0;
  double fact = xLen/sqrt(static_cast<double>(nCol_*nCol_+nRow_*nRow_));
  double t1 = a11*fact*nCol_;
  double t2 = -a13*fact*nRow_;
  double tt = sqrt(fabs(yLength_*yLength_-xLen*xLen));
  xRef_ = (xLength_-t1-t2)/2.0 + 1.33333;
  yRef_ = (xLen-a21*fact*nCol_-a22*tt+a23*fact*nRow_)/2.0+1.33333;
  yScale_ = (tt-pedestalSizeScaleFactor_)/32767.0*verticalScaleFactor_;
  //
  // determine the min and max of array for color
  //
  flat_ = true;
  GRA_window *gw = ExGlobals::GetGraphWindow();
  int penColorSave = gw->GetColor();
  if( color_ )
  {
    aMin_ = aMin_*yScale_ + pedestalSizeScaleFactor_;
    aMax_ = aMax_*yScale_ + pedestalSizeScaleFactor_;
    if( aMin_ != aMax_ )
    {
      flat_ = false;
      double atemp = 0.0001*(aMax_-aMin_);
      aMin_ -= atemp;
      aMax_ += atemp;
      aInc_ = (aMax_-aMin_)/10.0;
      for( std::size_t i=0; i<10; ++i )aLev_[i] = aMin_ + i*aInc_;
    }
  }
  std::size_t nData;
  histogram_ ? nData = 4*(max(nCol_,nRow_)+1)+1 : nData = max(nCol_,nRow_);
  std::vector<double> xData( nData, 0.0 );
  std::vector<double> yData( nData, 0.0 );
  std::vector<double> zData( nData, 0.0 );
  //
  std::size_t const nMask = static_cast<int>(2*xLength_*pipi);
  std::vector<int> mask( nMask, 0 );
  //
  std::vector<double> vertex( 16, 0.0 );
  //
  zData[0] = -fact*nRow_*jdz;
  int jx, jz = jzs;
  std::size_t nLine = 0;
  std::size_t nPoints = 0;
  //
  // simulate line drawn across bottom of page
  //
  storeVertices = true;
  indx = true;
  indy = true;
  indz = false;
  if( histogram_ )
  {
    Plot3D( xData, yData, zData, 1.0, 0.0, nLine, nPoints, mask, vertex );
  }
  //
  // draw lines parallel to x-axis
  //
  for( std::size_t iz=0; iz<nRow_; ++iz )
  {
    ++nLine;
    jx = jxs;
    jz += idz;
    double yLast = pedestalSizeScaleFactor_;
    nPoints = 0;
    for( std::size_t ix=0; ix<nCol_; ++ix )
    {
      ++nPoints;
      jx += idx;
      xData[nPoints-1] = fact*(jx-1+jdx);
      if( histogram_ )
      {
        yData[nPoints-1] = yLast;
        ++nPoints;
        xData[nPoints-1] = xData[nPoints-2];
      }
      yData[nPoints-1] = matrix_[jz+(jx-1)*nRow_-1]*yScale_+pedestalSizeScaleFactor_;
      yLast = yData[nPoints-1];
    }
    if( histogram_ )
    {
      ++nPoints;
      xData[nPoints-1] = fact*(jx+idx-1+jdx);
      yData[nPoints-1] = yLast;
      ++nPoints;
      xData[nPoints-1] = xData[nPoints-2];
      yData[nPoints-1] = pedestalSizeScaleFactor_;
    }
    Plot3D( xData, yData, zData, 1.0, 0.0, nLine, nPoints, mask, vertex );
    if( histogram_ )
    {
      double ff = -fact*idz;
      //
      // hide portions of next line to be plotted
      //
      Hide( xData, yData, zData, 1.0, 0.0, nLine, nPoints, mask, 0.0, ff );
      ++nLine;
      zData[0] = -fact*(jz-jdz);
      Plot3D( xData, yData, zData, 1.0, 0.0, nLine, nPoints, mask, vertex );
    }
    else
    {
      zData[0] = -fact*(jz-jdz);
    }
  }
  //
  // draw lines parallel to z-axis
  //
  indx = false;
  indy = true;
  indz = true;
  zData[0] = fact*nCol_*jdx;
  jx = jxs;
  nLine = 0;
  for( std::size_t ix=0; ix<nCol_; ++ix )
  {
    ++nLine;
    jx += idx;
    jz = jzs;
    double yLast = pedestalSizeScaleFactor_;
    nPoints = 0;
    for( std::size_t iz=0; iz<nRow_; ++iz )
    {
      ++nPoints;
      jz += idz;
      xData[nPoints-1] = -fact*(jz-1+jdz);
      if( histogram_ )
      {
        yData[nPoints-1] = yLast;
        ++nPoints;
        xData[nPoints-1] = xData[nPoints-2];
      }
      yData[nPoints-1] = matrix_[jz+(jx-1)*nRow_-1]*yScale_+pedestalSizeScaleFactor_;
      yLast = yData[nPoints-1];
    }
    if( histogram_ )
    {
      ++nPoints;
      xData[nPoints-1] = -fact*(jz+idz-1+jdz);
      yData[nPoints-1] = yLast;
      ++nPoints;
      xData[nPoints-1] = xData[nPoints-2];
      yData[nPoints-1] = pedestalSizeScaleFactor_;
    }
    Plot3D( zData, yData, xData, 0.0, 1.0, nLine, nPoints, mask, vertex );
    if( histogram_ )
    {
      double ff = fact*idx;
      //
      // hide portions of next line to be plotted
      //
      Hide( zData, yData, xData, 0.0, 1.0, nLine, nPoints, mask, ff, 0.0 );
      ++nLine;
      zData[0] = fact*(jx-jdx);
      Plot3D( zData, yData, xData, 0.0, 1.0, nLine, nPoints, mask, vertex );
    }
    else
    {
      zData[0] = fact*(jx-jdx);
    }
  }
  flat_ = true;
  GRA_lowLevel::GetPtr()->SetColor( penColorSave );
  //Framer( mask, vertex ); // draw a pedestal
}                                                                

void GRA_threeDFigure::Plot3D( std::vector<double> &xData, std::vector<double> &yData, std::vector<double> &zData,
                               double const xScale, double const zScale,
                               std::size_t const nLine, std::size_t const nPoints,
                               std::vector<int> &mask, std::vector<double> &vertex )
{
  // This routine will accept 3-dimensional data in various forms as input, rotate it in
  // 3-space to any angle, and plot the projection of the resulting figure onto the xy plane.
  // Linear interpolation is used between data points.  Those lines of a figure which should
  // be hidden by a previous line are masked. The masking technique used by this routine is based
  // on two premises -
  //  lines in the foreground (positive z direction) are plotted before lines in the background.
  //  a line or portion of a line is masked (hidden) if it lies within the region bounded by
  //   previously plotted lines.
  // Each call to Plot3D causes one line of a figure to be plotted.
  // Two parameters of the plotter are set on the initial call for each figure -
  //  pipi is the number of plotter increments per inch.
  //  nypi is the number of increments available across the width of the page (y-direction).
  // When a new figure is initiated, the plotter origin is set at the bottom of the paper by
  // plot3d and should not be moved until the figure is completed.
  // Input parameters -
  //   ivxyz is a four digit decimal integer which is used to select various input/output options.
  //         These digits, in decreasing order of magnitude, will be referred to as v, x, y, and z.
  //   If v != 0, the vertices of the current figure and their projection onto the y=0 plane, will
  //   be stored in a 16 entry real array, vertex, and will be updated as each line is plotted.
  //   These coordinates are in inches and relative to the current plotter origin.  The x y pairs
  //   are ordered so that the first pair corresponds to the first point of the figure, the second
  //   pair corresponds to the last point of the first line, and the following pairs are ordered in
  //   a circular fashion.  The pairs on the y=0 plane of the figure, then follow in the same order.
  //   If v=0, the vertex parameter is ignored, but should not be deleted.
  //   If x=0, the x-components of this line are assumed to be equally spaced, and are computed by
  //   x[i] = xData+i*xScale where xData is the initial value in inches and xScale is the spacing
  //   between points in inches.  if x != 0, the x-components of this line are read from an array
  //   and modified by x[i]=xData[i]*xScale where xScale is a scale factor. The same relations hold
  //   for the y-components, that is, if y=0  y[i]=yData+i*yScale_ and if y != 0
  //   y[i]=yData[i]*yScale_.  If z=0, the z-components of this line are all assumed to be equal,
  //   and are computed by z[i]=zData+nLine*zScale  where nLine is some integer associated
  //   with this line.  If z != 0, again we have z[i]=zData[i]*zScale when (nLine) is equal to one,
  //   it indicates the beginning of a new figure.  A call to Plot3D with nLine equal to zero before
  //   initiating a new figure simulates a line drawn at the bottom of the page.  Therefore only
  //   those portions of a line lying above all previuos lines will be plotted. All other parameters
  //   are ignored on such a call.
  //  nPoints is the number of points on this line, and may be altered from line to line.
  //  phi_ and theta_ are the two angles (in degrees) used to specify the desired 3-dimensional
  //   rotation.  The following two definitions of these rotations are equivalent
  //   - in terms of rotations of axes, the initial system of axes, xyz, is rotated by an angle phi_
  //     counterclockwise about the y-axis, and the resultant system is labelled the tuv axes. The
  //     tuv axes are then rotated by an angle theta_ counterclockwise about the t-axis, and this
  //     final system is labelled the pqr axes.  The plotted figure is the projection of the
  //     original figure onto the pq-plane.
  //   - in terms of rotation of coordinates, the figure is first rotated by an angle theta_
  //     clockwise about the x-axis. The resultant figure is then rotated by an angle phi_ clockwise
  //     about its y-axis.  The plotted figure is the projection of this final figure onto the
  //     xy-plane.
  //  WARNING. some rotations will alter the foreground/background relationships between the lines,
  //           and thus the order in which they should be plotted.
  //  xRef_ and yRef_ are the coordinates, in inches, relative to the plotter origin, to be used as
  //   the origin of the figure.
  //  xLength_ is the length, in inches, to which the plot is restricted. Any point which exceeds
  //   this limit, or the limits of the paper in the y-direction NYPI, will be set to that limit.
  //  mask is an integer array of 2*xLength_*PIPI entries which is used to
  //   store the mask.  The contents of this array should not be altered
  //   during the plotting of any given figure.
  // All parameters except mask and vertex are returned unchanged.
  // Between any two calls for the same figure, any parameter can be
  // meaningfully changed except xLength_, mask, and vertex.
  //
  int low, high, mlow, mhigh, oldhi, oldlow, loc, locold, jx, jy;
  //
  // Initialization procedure for a new figure
  // Test for special mask modifying call
  //
  static int i, inci, init;
  double xx, yy, xxx, yyy;
  if( nLine == 0 ) // option to modify the masking technique to be used on the
  {                // following figure so as to plot only above all previous lines
    init = 0;
    return;
  }
  else if( nLine == 1 ) // initialization is required
  {
    //
    // compute length of plot page in increments
    //
    limitx = static_cast<int>(xLength_*pipi + 0.5);
    //
    // initialize masking array over the length of the plot page
    //
    mask.insert( mask.begin(), 2*limitx, init );
    init = -1;
    //
    // set the necessary indicators for the first line of a new figure
    //
    inci = -1;
    i = 0;
  }
  //    
  // processing procedures
  // set flag to move through the data arrays in the opposite direction
  //
  inci = -inci;
  //    
  // set indicator to the first point to be processed
  //
  if( i != 0 )i = nPoints + 1;
  //    
  // loop to process each point in the data arrays
  //
  int iyref;
  static int incx;
  for( std::size_t k=0; k<nPoints; ++k )
  {
    // data calculation
    //
    i += inci;
    double x, y, z;
    indx ? x = xData[i-1]*xScale : x = xData[0] + (i-1)*xScale;
    indy ? y = yData[i-1]*yScale_ : y = yData[0] + (i-1)*yScale_;
    indz ? z = zData[i-1]*zScale : z = zData[0] + (nLine-1)*zScale;
    yDatanew = yData[i-1];
    //
    // data rotation
    //
    xxx = a11*x + a13*z + xRef_;
    xx = xxx;
    int ix = static_cast<int>(xx*pipi + 0.5);
    yyy = a21*x + a23*z + yRef_;
    yy = yyy + a22*y;
    int iy = static_cast<int>(yy*pipi + 0.5);
    if( k == 0 )
    {
      //
      // (loc) is the position of the previous point with respect to the mask
      //     +1  above the mask
      //      0  within the limits of the mask
      //     -1  below the mask
      // procedure for initial point of each line
      // locate initial point with respect to the mask then update the mask
      //
      low = 2*ix;
      high = low - 1;
      mlow = mask[low-1];
      mhigh = mask[high-1];
      if( mhigh < iy )
      {
        mask[high-1] = iy;
        if( mlow == -1 )mask[low-1] = iy;
        locold = 1;
      }
      else if( mhigh == iy )
      {
        locold = 1;
      }
      else
      {
        if( mlow < iy )
        {
          locold = 0;
        }
        else if( mlow == iy )
        {
          locold = -1;
        }
        else if( mlow > iy )
        {
          mask[low-1] = iy;
          locold = -1;
        }
      }
      //
      // move the raised pen to this initial point
      //
      iPlot( ix, iy, 0 );
      jx = ix;
      jy = iy;
      iyref = iy;
      if( locold == 0 )penOld_ = 3;
      if( storeVertices )
      {
        vertex[inci+5] = xx;
        vertex[inci+6] = yy;
        vertex[inci+13] = xxx;
        vertex[inci+14] = yyy;
        if( nLine == 1 )
        {
          vertex[0] = xx;
          vertex[1] = yy;
          vertex[8] = xxx;
          vertex[9] = yyy;
        }
      }
      continue;
    }
    //
    // special case where change in x coordinate is zero
    // a special provision is made at this point so that a line
    // will not mask itself as long as the x coordinate remains constant
    //
    double yj, yinc;
    if( ix == jx )
    {
      jy = iy;
    }
    else
    {
      //
      // compute constants for linear interpolation
      //
      yinc = static_cast<double>(iy-jy)/fabs(static_cast<double>(ix-jx));
      incx = (ix-jx)/abs(ix-jx);
      yj = static_cast<double>(jy);
      //      
      // perform linear interpolation at each incremental step on the x axis
      //
      jx += incx;
      yj += yinc;
      jy = static_cast<int>(yj + 0.5);
      //
      // locate the current point with respect to the mask at that
      // point then plot the increment as a function of the
      // location of the previous point with respect to its mask
      //
      low = 2*jx;
      high = low - 1;
      mlow = mask[low-1];
      mhigh = mask[high-1];
    }
    L10:
    if( mhigh <= jy )
    {
      //
      // the current point is above the mask
      //
      loc = 1;
      if( locold < 0 )
      {
        //      
        // plot from below the mask to above the mask
        //
        if( mlow < iyref )
        {
          //      
          // plot from within the mask to above the mask
          //
          if( mhigh < iyref )
          {
            if( mhigh != -1 )
            {
              oldhi = high - 2*incx;
              mask[oldhi-1] <= jy ? iPlot( jx-incx, mask[oldhi-1], 3 ) : iPlot( jx, jy, 3 );
            }
          }
          else
          {
            iPlot( jx, mhigh, 3 );
          }
        }
        else
        {
          iPlot( jx, mlow, 2 );
          iPlot( jx, mhigh, 3 );
        }
        //
        // plot from above the mask to above the mask
        //
        mask[high-1] = jy;
        if( mlow == -1 )mask[low-1] = jy;
        iPlot( jx, jy, 2 );
      }
      else if( locold == 0 )
      {
        //      
        // plot from within the mask to above the mask
        //
        if( mhigh < iyref )
        {
          if( mhigh != -1 )
          {
            oldhi = high - 2*incx;
            mask[oldhi-1] <= jy ? iPlot( jx-incx, mask[oldhi-1], 3 ) : iPlot( jx, jy, 3 );
          }
        }
        else
        {
          iPlot( jx, mhigh, 3 );
        }
      }
      //
      // plot from above the mask to above the mask
      //
      mask[high-1] = jy;
      if( mlow == -1 )mask[low-1] = jy;
      iPlot( jx, jy, 2 );
    }
    else
    {
      if( mlow < jy )
      {
        //      
        // the current point is within the mask
        //
        loc = 0;
        if( locold < 0 )
        {
          //
          // plot from below the mask to within the mask
          //
          if( mlow >= iyref )iPlot( jx, mlow, 2 );
          //
          // plot from within the mask to within the mask
          //
          iPlot( jx, jy, 3 );
        }
        else if( locold == 0 )
        {
          //
          // plot from within the mask to within the mask
          //
          iPlot( jx, jy, 3 );
        }
        else
        {
          //      
          // plot from above the mask to within the mask
          //
          if( mhigh <= iyref )iPlot( jx, mhigh, 2 );
          //
          // plot from within the mask to within the mask
          //
          iPlot( jx, jy, 3 );
        }
      }
      else // the current point is below the mask
      {
        loc = -1;
        if( locold == 0 ) // plot from within the mask to below the mask
        {
          if( mlow <= iyref )
          {
            iPlot( jx, mlow, 3 );
          }
          else
          {
            oldlow = low - 2*incx;
            mask[oldlow-1] < jy ? iPlot( jx, jy, 3 ) : iPlot( jx-incx, mask[oldlow-1], 3 );
          }
        }
        else // plot from above the mask to below the mask
        {
          if( mhigh <= iyref )
          {
            iPlot( jx, mhigh, 2 );
            iPlot( jx, mlow, 3 );
          }
          else // plot from within the mask to below the mask
          {
            if( mlow <= iyref )
            {
              iPlot( jx, mlow, 3 );
            }
            else
            {
              oldlow = low - 2*incx;
              mask[oldlow-1] < jy ? iPlot( jx, jy, 3 ) : iPlot( jx-incx, mask[oldlow-1], 3 );
            }
          }
        }
      }
      mask[low-1] = jy;
      iPlot( jx, jy, 2 );
      iyref = jy;
    }
    locold = loc;
    if( jx != ix )
    {
      jx += incx;
      yj += yinc;
      jy = static_cast<int>(yj + 0.5);
      //
      // locate the current point with respect to the mask at that
      // point then plot the increment as a function of the
      // location of the previous point with respect to its mask
      //
      low = 2*jx;
      high = low - 1;
      mlow = mask[low-1];
      mhigh = mask[high-1];
      goto L10;
    }
    int isv = penOld_;
    iPlot( jx, jy, -1 );
    penOld_ = isv;
  }
  iPlot( jx, jy, 0 ); // raise pen
  //
  if( storeVertices )
  {
    vertex[-inci+5] = xx;
    vertex[-inci+6] = yy;
    vertex[-inci+13] = xxx;
    vertex[-inci+14] = yyy;
    if( nLine == 1 )
    {
      vertex[2] = xx;
      vertex[3] = yy;
      vertex[10] = xxx;
      vertex[11] = yyy;
    }
  }
  --i;
  return;
}

void GRA_threeDFigure::Hide( std::vector<double> &xData, std::vector<double> &yData, std::vector<double> &zData,
                             double const xScale, double const zScale,
                             std::size_t const nLine, std::size_t const nPoints,
                             std::vector<int> &mask, double const dx, double const dz )
{
  for( std::size_t k=1; k<=nPoints+1; ++k ) // loop to process each point in the data arrays
  {
    std::size_t i = 2*k-1;
    double x, y, z;
    indx ? x = xData[i-1]*xScale : x = xData[0]+(i-1)*xScale;
    indy ? y = yData[i-1]*yScale_ : y = yData[0]+(i-1)*yScale_;
    indz ? z = zData[i-1]*zScale : z = zData[0]+(nLine-1)*zScale;
    //
    // data rotation
    //
    double xx = a11*x + a13*z + xRef_;
    int ix = static_cast<int>(xx*pipi + 0.5);
    double yy = a21*x + a23*z + yRef_ + a22*y;
    int iy = static_cast<int>(yy*pipi + 0.5);
    //
    // restrict figure to plot page
    //
    if( ix <= 0 )ix = 1;
    if( ix > static_cast<int>(limitx) )ix = limitx;
    if( iy < 10 )iy = 10;
    if( iy > static_cast<int>(nypi) )iy = nypi;
    x += dx;
    z += dz;
    xx = a11*x + a13*z + xRef_;
    int jx = static_cast<int>(xx*pipi + 0.5);
    yy = a21*x + a23*z + yRef_ + a22*y;
    int jy = static_cast<int>(yy*pipi + 0.5);
    //
    // restrict figure to plot page
    //
    if( jx <= 0 )jx = 1;
    if( jx > static_cast<int>(limitx) )jx = limitx;
    if( jy < 10 )jy = 10;
    if( jy > static_cast<int>(nypi) )jy = nypi;
    int low = 2*ix;
    int high = low - 1;
    int mlow = mask[low-1];
    int mhigh = mask[high-1];
    //
    // special case where change in x coordinate is zero
    // a special provision is made at this point so that a line
    // will not mask itself as long as the x coordinate remains constant
    //
    int incx;
    double yj, yinc;
    if( ix != jx )
    {
      // compute constants for linear interpolation
      yinc = static_cast<double>(iy-jy)/fabs(static_cast<double>(ix-jx));
      incx = (ix-jx)/abs(ix-jx);
      yj = static_cast<double>(jy);
      //
      // locate the current point with respect to the mask at that
      // point then plot the increment as a function of the
      // location of the previous point with respect to its mask
      //
      low = 2*jx;
      high = low - 1;
      mlow = mask[low-1];
      mhigh = mask[high-1];
    }
    else
    {
      jy = iy;
    }
    //
    // perform linear interpolation at each incremental step on the x axis
    //
    for( ;; )
    {
      if( mhigh <= jy )
      {
        mask[high-1] = jy;
        if( mlow == -1 )mask[low-1] = jy;
      }
      else
      {
        if( mlow >= jy )mask[low-1] = jy;
      }
      if( jx == ix )break;
      jx += incx;
      yj += yinc;
      jy = static_cast<int>(yj + 0.5);
      low = 2*jx;
      high = low - 1;
      mlow = mask[low-1];
      mhigh = mask[high-1];
    }
  }
  return;
}

void GRA_threeDFigure::iPlot( int const x, int const y, int const pen )
{
  int const colorArray[] = {-1,-19,-5,-18,-14,-15,-9,-11,-13,-2};
  static double yData1, yData2;
  static int xSave, ySave;
  if( pen == 0 )
  {
    yData1 = yDatanew;
    yData2 = yDatanew;
    if( color_ )
    {
      int color;
      flat_ ? color=1 : color=max(1,min(10,static_cast<int>((yData2-aMin_)/aInc_)));
      GRA_lowLevel::GetPtr()->SetColor( colorArray[color-1] );
    }
    xSave = x;
    ySave = y;
    GRA_lowLevel::GetPtr()->Plot( xF_*x/pipi+xLwind_,yF_*y/pipi+yLwind_,3 );
    xOld_ = x;
    yOld_ = y;
    penOld_ = 2;
  }
  else
  {
    if( !color_ || flat_ )
    {
      if( pen != penOld_ )
      {
        GRA_lowLevel::GetPtr()->Plot( xF_*xOld_/pipi+xLwind_,yF_*yOld_/pipi+yLwind_,penOld_ );
      }
      penOld_ = pen;
      xOld_ = x;
      yOld_ = y;
      return;
    }
    if( pen == penOld_ )
    {
      yData2 = yDatanew;
      xOld_ = x;
      yOld_ = y;
      return;
    }
    int lend = max(1,min(10,static_cast<int>((yData2-aMin_)/aInc_)+1));
    int lstr = max(1,min(10,static_cast<int>((yData1-aMin_)/aInc_)+1));
    for( ;; )
    {
      GRA_lowLevel::GetPtr()->SetColor( colorArray[lstr-1] );
      if( lend == lstr )
      {
        GRA_lowLevel::GetPtr()->Plot( xF_*xOld_/pipi+xLwind_,yF_*yOld_/pipi+yLwind_,penOld_ );
        xSave = xOld_;
        ySave = yOld_;
        if( penOld_ == 2 )yData1 = yData2;
        yData2 = yDatanew;
        break;
      }
      else if( lend > lstr )
      {
        ++lstr;
        double atemp = (aLev_[lstr-1]-yData1)/(yData2-yData1);
        double xx = (atemp*(xOld_-xSave)+xSave)/pipi;
        double yy = (atemp*(yOld_-ySave)+ySave)/pipi;
        GRA_lowLevel::GetPtr()->Plot( xF_*xx+xLwind_,yF_*yy+yLwind_,penOld_ );
      }
      else if( lend < lstr )
      {
        double atemp = (aLev_[lstr-1]-yData1)/(yData2-yData1);
        --lstr;
        double xx = (atemp*(xOld_-xSave)+xSave)/pipi;
        double yy = (atemp*(yOld_-ySave)+ySave)/pipi;
        GRA_lowLevel::GetPtr()->Plot( xF_*xx+xLwind_,yF_*yy+yLwind_, penOld_ );
      }
    }
    penOld_ = pen;
    xOld_  = x;
    yOld_  = y;
  }
}

void GRA_threeDFigure::Framer( std::vector<int> &mask, std::vector<double> &vertex )
{
  std::vector<double> xData( 7 );
  std::vector<double> yData( 7 );
  std::vector<double> zData( 1, 0.0 );
  //
  xData[0] = vertex[4];
  xData[1] = vertex[12];
  xData[2] = vertex[10];
  xData[3] = xData[1];
  xData[4] = vertex[14];
  xData[5] = xData[1];
  xData[6] = xData[0];
  //
  yData[0] = vertex[5];
  yData[1] = vertex[13];
  yData[2] = vertex[11];
  yData[3] = yData[1];
  yData[4] = vertex[15];
  yData[5] = yData[1];
  yData[6] = yData[0];
  //
  storeVertices = false;
  indx = true;
  indy = true;
  indz = false;
  yScale_ = 1.0;
  std::size_t nLine = 2;
  std::size_t nPoints = 7;
  phi_ = 0.0;
  theta_ = 0.0;
  xRef_ = 0.0;
  yRef_ = 0.0;
  xLength_ = 0.0;
  Plot3D( xData, yData, zData, 1.0, 0.0, nLine, nPoints, mask, vertex );
  //
  // the remaining vertices are drawn now
  //
  GRA_lowLevel *llg = GRA_lowLevel::GetPtr();
  llg->Plot( xF_*vertex[6]/pipi+xLwind_, yF_*vertex[7]/pipi+yLwind_, 3 );
  llg->Plot( xF_*vertex[14]/pipi+xLwind_, yF_*vertex[15]/pipi+yLwind_, 2 );
  llg->Plot( xF_*vertex[8]/pipi+xLwind_, yF_*vertex[9]/pipi+yLwind_, 2 );
  llg->Plot( xF_*vertex[10]/pipi+xLwind_, yF_*vertex[11]/pipi+yLwind_, 2 );
  llg->Plot( xF_*vertex[2]/pipi+xLwind_, yF_*vertex[3]/pipi+yLwind_, 2 );
  llg->Plot( xF_*vertex[0]/pipi+xLwind_, yF_*vertex[1]/pipi+yLwind_, 3 );
  llg->Plot( xF_*vertex[8]/pipi+xLwind_, yF_*vertex[9]/pipi+yLwind_, 2 );
}

// end of file

