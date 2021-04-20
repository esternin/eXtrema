/*
Copyright (C) 2005,...,2009 Joseph L. Chuma

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

#include "GRA_outputType.h"

#include "GRA_window.h"
#include "EGraphicsError.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"
#include "GRA_drawableText.h"

void GRA_outputType::SetUpTransformationMatrices( double xmin, double ymin,
                                                  double xmax, double ymax )
{
  xMin_ = xmin;
  yMin_ = ymin;
  xMax_ = xmax;
  yMax_ = ymax;
  //
  double xminW, yminW, xmaxW, ymaxW;
  ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
  //
  //  Calculate the world to outputType transformation
  //
  //            |x|                |xw|
  //            | | = w2oMatrix_ * |  | + w2oShift_
  //            |y|                |yw|
  //
  w2oMatrix_[0][0] = (xmax-xmin)/(xmaxW-xminW);
  w2oMatrix_[0][1] = 0.0;
  w2oMatrix_[1][0] = 0.0;
  w2oMatrix_[1][1] = (ymax-ymin)/(ymaxW-yminW);
  w2oShift_[0] = xmin-xminW*w2oMatrix_[0][0];
  w2oShift_[1] = ymin-yminW*w2oMatrix_[1][1];
  //
  //  Calculate the inverse transformation matrix o2wMatrix_ from
  //  the outputType coordinate system to the world coordinate system:
  //
  //            |xw|                |x|
  //            |  | = o2wMatrix_ * | | + o2wShift_
  //            |yw|                |y|
  //
  //  o2wMatrix_ = inv(w2oMatrix_)
  //  world <-- outputType  =  inv(outputType <-- world)
  //
  double const determinant = w2oMatrix_[0][0]*w2oMatrix_[1][1] -
                             w2oMatrix_[1][0]*w2oMatrix_[0][1];
  o2wMatrix_[0][0] =  w2oMatrix_[1][1]/determinant;
  o2wMatrix_[0][1] = -w2oMatrix_[0][1]/determinant;
  o2wMatrix_[1][0] = -w2oMatrix_[1][0]/determinant;
  o2wMatrix_[1][1] =  w2oMatrix_[0][0]/determinant;
  //
  //  Calculate the inverse translation vector o2wShift_ from the
  //  outputType coordinate system to the world coordinate system
  //
  //  o2wShift_ = -o2wMatrix_ * w2oShift_
  //
  o2wShift_[0] = -o2wMatrix_[0][0]*w2oShift_[0] - o2wMatrix_[0][1]*w2oShift_[1];
  o2wShift_[1] = -o2wMatrix_[1][0]*w2oShift_[0] - o2wMatrix_[1][1]*w2oShift_[1];
}

void GRA_outputType::OutputTypeToWorld( int ix, int iy, double &xw, double &yw ) const
{
  xw = o2wMatrix_[0][0]*ix + o2wMatrix_[0][1]*iy + o2wShift_[0];
  yw = o2wMatrix_[1][0]*ix + o2wMatrix_[1][1]*iy + o2wShift_[1];
}

void GRA_outputType::WorldToOutputType( double xw, double yw, double &xo, double &yo ) const
{
  xo = w2oMatrix_[0][0]*xw + w2oMatrix_[0][1]*yw + w2oShift_[0];
  yo = w2oMatrix_[1][0]*xw + w2oMatrix_[1][1]*yw + w2oShift_[1];
}

void GRA_outputType::Draw()
{
  try
  {
    ExGlobals::DrawGraphWindows( this );
  }
  catch (EGraphicsError &e)
  {
    ExString mess( "GRA_outputType: Draw() error:\n" );
    mess += ExString(e.what());
    throw EGraphicsError( mess );
  }
}

void GRA_outputType::Plot( double x, double y, int pen )
{
  // (x,y) are the world coordinates to which the pen is to be moved either up
  // (pen=3) or down (pen=2). The plot coordinates (x,y) can have any value
  // positive or negative, but are clipped outside the plot windows.
  //
  switch (pen)
  {
    case 3:   // move with pen up to (x,y)
    {
      PenUp( x, y );
      break;
    }
    case 2:   // move with pen down to (x,y)
    {
      PenDown( x, y );
      break;
    }
    case 20:  // plot a point at (x,y)
    {
      PlotPoint( x, y );
      break;
    }
  }
}

void GRA_outputType::PlotPoint( double x, double y )
{
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  if( (x-xmin)*(xmax-x)>=0.0 && (y-ymin)*(ymax-y)>=0.0 )GenerateOutput( x, y, 20 );
  xPrevious_ = x;    // set previous coordinates to new coordinates
  yPrevious_ = y;
  penPrevious_ = 20;
}

void GRA_outputType::PenUp( double x, double y )
{
  // penUp moves with the pen up (not drawing) to the plot coordinates (x,y)
  //
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  if( (x-xmin)*(xmax-x)>=0 && (y-ymin)*(ymax-y)>=0 )GenerateOutput( x, y, 3 );
  xPrevious_ = x;    // set previous coordinates to new coordinates
  yPrevious_ = y;
  penPrevious_ = 3;
}

void GRA_outputType::PenDown( double x, double y )
{
  // PenDown draws a line to the world coordinates (x,y)
  //
  // clip the line segment (xPrevious_,yPrevious_)-->(x,y) within the clipping boundary
  //
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetClippingBoundary( xmin, ymin, xmax, ymax );
  if( x>=xmin && xPrevious_>=xmin && x<=xmax && xPrevious_<=xmax &&
      y>=ymin && yPrevious_>=ymin && y<=ymax && yPrevious_<=ymax )
  {
    // (x,y) and (xPrevious_,yPrevious_) are both inside the clipping boundary
    //
    GenerateOutput( x, y, 2 );
  }
  else
  {
    int ndraw = 1;
    double xdraw[2], ydraw[2];
    xdraw[0] = x;
    ydraw[0] = y;
    UsefulFunctions::WindowClip( xPrevious_, yPrevious_, x, y,
                                 xmin, xmax, ymin, ymax,
                                 xdraw, ydraw, ndraw );
    switch (ndraw)
    {
      case 1:   //(x,y) is outside but (xPrevious_,yPrevious_) is in
      {
        GenerateOutput( xdraw[0], ydraw[0], 2 );
        break;
      }
      case 2:   // (x,y) and (xPrevious_,yPrevious_) are both outside
      case 3:   // (xPrevious_,yPrevious_) is outside but (x,y) is inside
      {
        GenerateOutput( xdraw[0], ydraw[0], 3 );
        GenerateOutput( xdraw[1], ydraw[1], 2 );
        break;
      }
    }
  }
  xPrevious_ = x;    // set previous coordinates to new coordinates
  yPrevious_ = y;
  penPrevious_ = 2;
}

void GRA_outputType::AlignXY( GRA_drawableText *dt, double length,
                              double maxHeight, double &angle,
                              double &xLoc, double &yLoc,
                              double &sinx, double &cosx )
{
  xLoc = dt->GetX();
  yLoc = dt->GetY();
  if( dt->GetGraphUnits() )
  {
    double xg=xLoc, yg=yLoc;
    ExGlobals::GetGraphWindow()->GraphToWorld( xg, yg, xLoc, yLoc );
  }
  angle = dt->GetAngle(); // this is in degrees
  cosx = cos( angle*M_PI/180.0 );
  sinx = sin( angle*M_PI/180.0 );
  double const eps = 0.0001;
  if( fabs(cosx) <= eps )cosx = 0.0;
  if( fabs(sinx) <= eps )sinx = 0.0;
  if( fabs(cosx-1.0) <= eps )cosx = 1.0;
  if( fabs(sinx-1.0) <= eps )sinx = 1.0;
  //
  int alignment = dt->GetAlignment();
  switch (alignment)
  {
    case 1:               // lower left
      break;
    case 2:               // lower center
      xLoc -= 0.5*length*cosx;
      yLoc -= 0.5*length*sinx;
      break;
    case 3:               // lower right
      xLoc -= length*cosx;
      yLoc -= length*sinx;
      break;
    case 4:               // center left
      xLoc += 0.5*maxHeight*sinx;
      yLoc -= 0.5*maxHeight*cosx;
      break;
    case 5:               // center center
      xLoc -= 0.5*length*cosx - 0.5*maxHeight*sinx;
      yLoc -= 0.5*length*sinx + 0.5*maxHeight*cosx;
      break;
    case 6:               // center right
      xLoc -= length*cosx - 0.5*maxHeight*sinx;
      yLoc -= length*sinx + 0.5*maxHeight*cosx;
      break;
    case 7:               // upper left
      xLoc += maxHeight*sinx;
      yLoc -= maxHeight*cosx;
      break;
    case 8:               // upper center
      xLoc -= 0.5*length*cosx - maxHeight*sinx;
      yLoc -= 0.5*length*sinx + maxHeight*cosx;
      break;
    case 9:               // upper right
      xLoc -= length*cosx - maxHeight*sinx;
      yLoc -= length*sinx + maxHeight*cosx;
      break;
    case 10:               // lower left at 90deg
      angle = 90.0;
      cosx = 0.0;
      sinx = 1.0;
      break;
    case 11:              // lower left at 270deg
      angle = 270.0;
      cosx = 0.0;
      sinx = -1.0;
      break;
    case 12:              // lower center at 90deg
      angle = 90.0;
      cosx = 0.0;
      sinx = 1.0;
      yLoc -= 0.5*length;
      break;
  }
}

void GRA_outputType::StartLine( double x, double y )
{
  PenUp( x, y );
  newLine_ = true;
}

void GRA_outputType::ContinueLine( double x, double y )
{
  // This routine draws a line of specified line type from the
  // current location to location (x,y) in world coordinates
  //
  // The line types are in four different styles:
  // 1) Ordinary solid line
  // 2) Double line of specified width
  // 3) Dashed line with specified dash and space lengths
  // 4) Dashed line with two different dash lengths
  //
  // Input:
  //   x,y     point coordinates in world units
  //
  // For each line type type, the appearance of the line is determined
  // by three parameters p1,p2,p3 that are stored in an internal table
  //
  // If p1 = 0, the line is an ordinary solid line
  // If p1 > 0 and p2 = 0, the line is a double line of width p1
  // If p1 > 0 and p2 > 0, the line is a dashed line, with dash
  //  length p1 and space length p2
  // If p1 > 0 and p2 > 0 and p3 > 0, the line is a dashed line,
  //  with first dash length p1, space length p2, and second dash
  //  length p3
  //
  // NOTE:  p1, p2 and p3 should be in world units
  //
  // The user may define his own line types, or he may use the default
  // types established in this routine, which are suitable for the
  // default 640 x 480 coordinate system.  Line types are user-defined
  // by calling DrawLineSet( type, p1, p2, p3 )
  // where type is a line type number between 1 and 10 and p1-p3
  // are the desired parameters.  Alternatively, the default line types
  // can be scaled to fit a user coordinate system by calling
  // DrawLineScale( type, sf ) where sf is the desired scale factor.
  // For example, if the user coordinate system is 10 units in x by
  // 7.5 units in y, the scale factor sf=10/640 would be appropriate
  //
  // This routine always plots from the current location.
  // Thus a call to StartLine must precede a series of calls
  // to ContinueLine.  In order to plot a continuous dashed line through
  // a series of points, ContinueLine "remembers" where it's at between
  // calls.  As long as the current location is the same as that
  // where ContinueLine left off on the previous call, 
  // ContinueLine will continue dashing from where it
  // left off.  Thus the dashing may be interrupted, a symbol or other
  // items plotted, and the dashing can be continued by calling StartLine
  // to the old location before calling ContinueLine.
  //
  if( x==xPrevious_ && y==yPrevious_ )return; // draw nothing if going nowhere
  //
  // get parameters
  //
  double p1 = drawLineTable_[0][currentLineType_-1];
  double p2 = drawLineTable_[1][currentLineType_-1];
  double p3 = drawLineTable_[2][currentLineType_-1];
  //
  if( p1 == 0. ) // draw an ordinary vector
  {
    PenDown( x, y );
  }
  else
  {
    double const deltx = x-xPrevious_;
    double const delty = y-yPrevious_;
    double const distanceToTravel = sqrt(deltx*deltx + delty*delty);
    //
    double sinth = delty/distanceToTravel; // sine of orientation angle
    double costh = deltx/distanceToTravel; // cosine of orientation angle
    //
    if( p2 == 0. ) // double line
    {
      double hwid = p1/2.0;
      double dx = hwid*sinth;
      double dy = hwid*costh;
      double xloc = xPrevious_;
      double yloc = yPrevious_;
      PenUp( xloc-dx, yloc+dy );
      PenDown( x-dx, y+dy );
      PenUp( xloc+dx, yloc-dy );
      PenDown( x+dx, y-dy );
    }
    else           // dashed line
    {
      //static double distanceTraveled = 0.0;
      //static double xOld = 0.0;
      //static double yOld = 0.0;
      //static int op = 0;
      //
      if( p3 == 0. )p3 = p1;
      //
      double dd[4]; // distance increments for operation loop
      dd[0] = p1;
      dd[1] = p2;
      dd[2] = p3;
      dd[3] = p2;
      //
      double const x0 = xPrevious_; // start point
      double const y0 = yPrevious_;
      //
      // continuing previous line?
      //
      double const eps = 0.0001;
      if( !newLine_ && fabs(x0-xOld_)<eps && fabs(y0-yOld_)<eps )
      {
        if( distanceTraveled_ > 0. )goto L210; // continuing dash or space
        goto L200;
      }
      //
      // begin dashing sequence
      //
      op_ = 0;                     // operation number
      distanceTraveled_ = 0.0;     // distance travelled
 L200:                 // operation loop begins
      if( ++op_ == 5 )op_ = 1;
      distanceTraveled_ += dd[op_-1];
 L210:
      double xloc = x0 + distanceTraveled_*costh;
      double yloc = y0 + distanceTraveled_*sinth;
      bool drawTo = (op_==1||op_==3);
      //
      // last operation
      //
      if( distanceTraveled_ >= distanceToTravel )
      {
        xloc = x;
        yloc = y;
        //drawTo = true;
      }
      drawTo ? PenDown(xloc,yloc) : PenUp(xloc,yloc);
      if( distanceTraveled_ < distanceToTravel )goto L200;  // do next operation
    }
    xOld_ = x;  // save end point
    yOld_ = y;
    distanceTraveled_ -= distanceToTravel;  // portion of dash or space to be continued
  }
  newLine_ = false;
}

void GRA_outputType::DrawLineSet( int type, double p1, double p2, double p3 )
{
  // set up line type characteristics for DrawLine
  //
  if( type>10 || type<1 )return;
  drawLineTable_[0][type-1] = p1;
  drawLineTable_[1][type-1] = p2;
  drawLineTable_[2][type-1] = p3;
}

void GRA_outputType::DrawLineGet( int type, double &p1, double &p2, double &p3 )
{
  if( type>10 || type<1 )return;
  p1 = drawLineTable_[0][type-1];
  p2 = drawLineTable_[1][type-1];
  p3 = drawLineTable_[2][type-1];
}

void GRA_outputType::SetDefaultDrawLineTable()
{
  double const defaultTable[3][10] =
  {
    {0.00,0.02,0.30,0.30,0.40,0.40,0.02,0.02,0.06,0.10},    // p1
    {0.00,0.00,0.15,0.15,0.20,0.16,0.12,0.12,0.20,0.30},    // p2
    {0.00,0.00,0.00,0.10,0.00,0.12,0.00,0.08,0.00,0.00}     // p3
  };
  for( std::size_t i=0; i<10; ++i )
  {
    for( std::size_t j=0; j<3; ++j )drawLineTable_[j][i] = defaultTable[j][i];
  }
}

void GRA_outputType::DrawLineScale( int type, double sf )
{
  // scale the line table parameters by factor sf
  // for specified type only
  //
  if( type<1 || type>10 )return;
  for( std::size_t j=0; j<3; ++j )drawLineTable_[j][type-1] *= sf;
}

void GRA_outputType::DrawLineScale( double sf )
{
  // scale all of the line table parameters by factor sf
  //
  for( std::size_t i=0; i<10; ++i )
  {
    for( std::size_t j=0; j<3; ++j )drawLineTable_[j][i] *= sf;
  }
}

// end of file
