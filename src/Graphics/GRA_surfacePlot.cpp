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
#include "GRA_surfacePlot.h"

#include "EGraphicsError.h"
#include "GRA_window.h"
#include "GRA_wxWidgets.h"
#include "ExGlobals.h"
#include "wxPLplotstream.h"
                              
GRA_surfacePlot::GRA_surfacePlot( std::vector<double> &x,
                                  std::vector<double> &y,
                                  std::vector<double> &z,
                                  std::size_t nrow, double azimuth, double altitude,
                                  bool colour, bool contours, bool axes, bool sides )
    : GRA_drawableObject(wxT("SURFACEPLOT")),
      nRow_(nrow), azimuth_(azimuth), altitude_(altitude),
      colour_(colour), contours_(contours), axes_(axes), sides_(sides)
{
  x_.assign( x.begin(), x.end() );
  y_.assign( y.begin(), y.end() );
  z_.assign( z.begin(), z.end() );
  nCol_ = z_.size()/nRow_;
}

void GRA_surfacePlot::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{

  //std::cout << "GRA_surfacePlot:Draw  at the top\n";

  double *x, *y;
  if( (x=(double*)malloc(nCol_*sizeof(double))) == 0 )
    throw EGraphicsError( wxT("unable to allocate memory for independant variable") );
  if( (y=(double*)malloc(nRow_*sizeof(double))) == 0 )
  {
    free(x);
    throw EGraphicsError( wxT("unable to allocate memory for dependant variable") );
  }
  double **z;
  if( (z=(double**)malloc(nCol_*sizeof(double*))) == 0 )
  {
    free(x);
    free(y);
    throw EGraphicsError( wxT("unable to allocate memory for matrix data") );
  }
  for( std::size_t i=0; i<nCol_; ++i )
  {
    if( (z[i]=(double*)malloc(nRow_*sizeof(double))) == 0 )
    {
      free(x);
      free(y);
      for( std::size_t j=0; j<i; ++j )free(z[j]);
      free(z);
      throw EGraphicsError( wxT("unable to allocate memory for matrix data") );
    }
  }
  double xmin = x_[0];
  double xmax = xmin;
  for( std::size_t i=0; i<nCol_; ++i )
  {
    x[i] = x_[i];
    if( xmin > x[i] )xmin = x[i];
    if( xmax < x[i] )xmax = x[i];
  }
  double ymin = y_[0];
  double ymax = ymin;
  double zmin = z_[0];
  double zmax = zmin;
  for( std::size_t j=0; j<nRow_; ++j )
  {
    y[j] = y_[j];
    if( ymin > y[j] )ymin = y[j];
    if( ymax < y[j] )ymax = y[j];
  }
  //
  for( std::size_t j=0; j<nCol_; ++j )
  {
    for( std::size_t i=0; i<nRow_; ++i )
    {
      z[j][i] = z_[i+j*nRow_];
      if( zmin > z[j][i] )zmin = z[j][i];
      if( zmax < z[j][i] )zmax = z[j][i];
    }
  }
  int w, h;
  dc.GetSize( &w, &h );
  wxPLplotstream *myStream;
  try
  {
    myStream = new wxPLplotstream( (wxDC*)(&dc), w, h );
  }
  catch ( EGraphicsError &e )
  {
    throw;
  }
  double basex = 3.0;
  double basey = 3.0;
  double height = 3.0;
  //
  double xmin2d = -2.5;
  double xmax2d = 2.5;
  double ymin2d = -2.5;
  double ymax2d = 4.0;
  myStream->env( xmin2d, xmax2d, ymin2d, ymax2d, 0, -2 );
  //
  myStream->col0(1);  // this is black, as set in the wxPLplotstream constructor
  myStream->w3d( basex, basey, height, xmin, xmax, ymin, ymax, zmin, zmax, altitude_, azimuth_ );
  if( axes_ )myStream->box3( "bnstu", "x axis", 0.0, 0,
                             "bnstu", "y axis", 0.0, 0,
                             "bnstu", "z axis", 0.0, 0 );
  //myStream->col0(2); // this could be the colour of the surface, if /COLOUR not used
  if( contours_ )
  {
    int const nlevel = 10;
    double clevel[10];
    double const step = (zmax-zmin)/(nlevel+1.0);
    for( int i=0; i<nlevel; ++i )clevel[i] = zmin + step*(i+1);
    if( colour_ )
      myStream->meshc( x, y, z, (int)nCol_, (int)nRow_, DRAW_LINEXY|MAG_COLOR|BASE_CONT,
                       clevel, nlevel );
    else
      myStream->meshc( x, y, z, (int)nCol_, (int)nRow_, DRAW_LINEXY|BASE_CONT,
                       clevel, nlevel );
  }
  else
  {
    //if( colour_ )
    //  myStream->mesh( x, y, z, (int)nCol_, (int)nRow_, DRAW_LINEXY|MAG_COLOR );
    //else
    //  myStream->mesh( x, y, z, (int)nCol_, (int)nRow_, DRAW_LINEXY );
    if( colour_ )
      myStream->plot3d( x, y, z, (int)nCol_, (int)nRow_, DRAW_LINEXY|MAG_COLOR, sides_ );
    else
      myStream->plot3d( x, y, z, (int)nCol_, (int)nRow_, DRAW_LINEXY, sides_ );
  }
  free( x );
  free( y );
  for( std::size_t j=0; j<nCol_; ++j )free( z[j] );
  free( z );
  delete myStream;
}

void GRA_surfacePlot::CopyStuff( GRA_surfacePlot const &rhs )
{
  nRow_ = rhs.nRow_;
  nCol_ = rhs.nCol_;
  colour_ = rhs.colour_;
  contours_ = rhs.contours_;
  axes_ = rhs.axes_;
  altitude_ = rhs.altitude_;
  azimuth_ = rhs.azimuth_;
  x_.assign( rhs.x_.begin(), rhs.x_.end() );
  y_.assign( rhs.y_.begin(), rhs.y_.end() );
  z_.assign( rhs.z_.begin(), rhs.z_.end() );
}

// end of file
