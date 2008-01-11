/*
Copyright (C) 2005,...,2007 Joseph L. Chuma, TRIUMF

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

#include "GRA_contour.h"
#include "EGraphicsError.h"
#include "GRA_window.h"
#include "GRA_wxWidgets.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_font.h"
#include "GRA_drawableText.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "ExGlobals.h"

GRA_contour::GRA_contour( double value, double cVal, GRA_color *color,
                          std::vector<double> &xData, std::vector<double> &yData,
                          std::vector< std::vector<double> > &zData, bool polar )
  : GRA_drawableObject(wxT("CONTOUR")), contourValue_(value), color_(color), xData_(xData),
    yData_(yData), zData_(zData), polar_(polar)
{
  //  cVal = the value of the number to be placed on the contour
  //         only the integer portion of the number is plotted
  //         cVal=0 --> the first 3 significant digits of
  //         contourValue_ are used
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
  labelSeparation_ = static_cast<GRA_sizeCharacteristic*>(general->Get(wxT("CONTOURLABELSEPARATION")))->GetAsWorld();
  labelSize_ = static_cast<GRA_sizeCharacteristic*>(general->Get(wxT("CONTOURLABELHEIGHT")))->GetAsWorld();
  font_ = static_cast<GRA_fontCharacteristic*>(general->Get(wxT("FONT")))->Get();
  numin_ = false;
  //
  if( labelSeparation_ > 0.0 )
  {
    // initialize for placing numbers on the contours
    //
    if( fabs(cVal) >= 1.0 )
    {
      drawnValue_ = cVal; // the number to be drawn on the contours
      //
      // calculate the number of digits in the number
      //
      int temp = static_cast<int>(log10(fabs(cVal))) + 1;
      if( temp <= 6 )
      {
        cVal > 0.0 ? xSep_ = temp*labelSize_*1.2 :
                     xSep_ = (temp+1)*labelSize_*1.2;
        return;
      }
    }
    //
    // fabs(cVal) is < 1.0  or  number of digits > 6
    // plot the first 3 signifigant digits of the number
    //
    drawnValue_ = 0.0;
    xSep_ = 3.0 * labelSize_;
    if( contourValue_ != 0.0 )
    {
      int temp = static_cast<int>(log10(fabs(contourValue_)));
      temp += 2 - static_cast<int>(temp);
      if( temp < 2 )++temp;
      drawnValue_ = static_cast<int>( pow(10.0,temp)+0.5 );
      if( contourValue_ <= 0.0 )
      {
        drawnValue_ *= -1;
        xSep_ += labelSize_ * 1.2;
      }
    }
  }
}

GRA_contour::~GRA_contour()
{ delete [] array_; }

std::ostream &operator<<( std::ostream &out, GRA_contour const &contour )
{
  out << "<contour color=\"" << GRA_colorControl::GetColorCode(contour.color_)
      << "\">\n";
  int size = contour.xPlot_.size();
  out << "<data size=\"" << size << "\">";
  for( int i=0; i<size; ++i )
    out << contour.xPlot_[i] << " " << contour.yPlot_[i] << " " << contour.connect_[i] << " ";
  out << "</data>\n";
  size = contour.textVec_.size();
  out << "<drawabletexts size=\"" << size << "\">\n";
  std::vector<GRA_drawableText*>::const_iterator tend = contour.textVec_.end();
  for( std::vector<GRA_drawableText*>::const_iterator i=contour.textVec_.begin(); i!=tend; ++i )out << **i;
  return out << "</drawabletexts>\n</contour>\n";
}

void GRA_contour::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  double const xuaxis =
      static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  double const xlaxis =
      static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  double const yuaxis =
      static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  double const ylaxis =
      static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  ExGlobals::SetClippingBoundary( xlaxis, ylaxis, xuaxis, yuaxis );
  wxPen wxpen( dc.GetPen() );
  wxpen.SetColour( ExGlobals::GetwxColor(color_) );
  dc.SetPen( wxpen );
  std::size_t size = xPlot_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    connect_[i] ? graphicsOutput->PenDown( xPlot_[i], yPlot_[i], dc ) :
                  graphicsOutput->PenUp( xPlot_[i], yPlot_[i] );
  }
  size = textVec_.size();
  for( std::size_t i=0; i<size; ++i )textVec_[i]->Draw( graphicsOutput, dc );
  ExGlobals::ResetClippingBoundary();
}

void GRA_contour::Make()
{
  ClearAllSwitches();
  std::size_t nx = xData_.size();
  std::size_t ny = yData_.size();
  //
  // search for a bottom edge contour
  //
  yRef_ = 0;
  yNext_ = 0;
  for( std::size_t i=1; i<nx; ++i )
  {
    if( zData_[0][i] > contourValue_ )
    {
      if( zData_[0][i-1] <= contourValue_ )
      {
        xRef_ = i;
        xNext_ = i-1;
        try
        {
          ContourA();
        }
        catch (EGraphicsError const &e)
        {
          throw;
        }
      }
    }
  }
  //
  // search the right edge
  //
  xNext_ = nx-1;
  xRef_ = nx-1;
  for( std::size_t i=1; i<ny; ++i )
  {
    if( zData_[i][nx-1] > contourValue_ )
    {
      if( zData_[i-1][nx-1] <= contourValue_ )
      {
        yNext_ = i-1;
        yRef_ = i;
        try
        {
          ContourA();
        }
        catch (EGraphicsError const &e)
        {
          throw;
        }
      }
    }
  }
  //
  // search the top edge
  //
  yRef_ = ny - 1;
  yNext_ = ny - 1;
  for( int i = nx-2; i >= 0; --i )
  {
    if( zData_[ny-1][i] > contourValue_ )
    {
      if( zData_[ny-1][i+1] <= contourValue_ )
      {
        xRef_ = i;
        xNext_ = i+1;
        try
        {
          ContourA();
        }
        catch (EGraphicsError const &e)
        {
          throw;
        }
      }
    }
  }
  //
  // search for a left edge contour
  //
  xRef_ = 1;
  xNext_ = 1;
  for( int i = ny-2; i >= 0; --i )
  {
    if( zData_[i][0] > contourValue_ )
    {
      if( zData_[i+1][0] <= contourValue_ )
      {
        yRef_ = i;
        yNext_ = i+1;
        try
        {
          ContourA();
        }
        catch (EGraphicsError const &e)
        {
          throw;
        }
      }
    }
  }
  //
  // search the rest of the array
  //
  for( std::size_t i=1; i<ny-1; ++i )
  {
    for( std::size_t j=1; j<nx-1; ++j )
    {
      if( zData_[i][j] > contourValue_ &&
          zData_[i][j-1] <= contourValue_ &&
          Search(j,i,true) )
      {
        xRef_ = j;
        xNext_ = j-1;
        yRef_ = i;
        yNext_ = i;
        try
        {
          ContourA();
        }
        catch (EGraphicsError const &e)
        {
          throw;
        }
      }
    }
  }
  return;
}

void GRA_contour::ContourA()
{
  int const it[9] = { 0, 1, 1, 0, 9, 0, -1, -1, 0 };
  int const jt[9] = { -1, 0, 0, -1, 9, 1, 0, 0, 1 };
  bool const dtest[9] = { false, true, false, true, false,
                          true, false, true, false };
  //
  // xRef_, yRef_  = reference point
  // xNext_, yNext_ = point we are contouring to
  //
  int irp = xRef_;
  int jrp = yRef_;
  int isp = xNext_;
  int jsp = yNext_;
  pen_ = 3;
  int in, jn;
  for( ;; ) // the only ways out of this loop are the returns in the inner loop
  {
    for( ;; )
    {
      try
      {
        ContourB( xData_[irp], yData_[jrp], zData_[jrp][irp], xData_[isp],
                  yData_[jsp], zData_[jsp][isp] );
      }
      catch (EGraphicsError const &e)
      {
        throw;
      }
      //
      // find the next point to check through a table lookup
      //
      int locate = 3*(jrp-jsp) + irp - isp + 4;
      //
      // in, jn = the new point
      //
      in = isp + it[locate];
      jn = jsp + jt[locate];
      //
      // test for an edge
      //
      if( in >= int(xData_.size()) || in < 0 ||
          jn >= int(yData_.size()) || jn < 0 )return;
      //
      // it may be a diagonal
      //
      if( locate == 5 && !Search(irp,jrp,false) )return;
      if( dtest[locate] )break;
      //
      // determine if it is a contour or switch points
      //
      if( zData_[jn][in] > contourValue_ )
      {
        irp = in;
        jrp = jn;
      }
      else
      {
        isp = in;
        jsp = jn;
      }
    }  // end of inner for loop
    //
    // diagonals get special treatment
    // calculate the height and location of the midpoint
    //
    double vx = ( xData_[irp] + xData_[in] )*0.5;
    double vy = ( yData_[jrp] + yData_[jn] )*0.5;
    int locate = 3*(jrp-jn) + irp - in + 4;
    int inn = in + it[locate];
    int jnn = jn + jt[locate];
    double htm = (zData_[jrp][irp] + zData_[jsp][isp] + zData_[jn][in] + zData_[jnn][inn])/4.0;
    //
    if( htm > contourValue_ ) // midpoint greater than contour
    {
      try
      {
        ContourB( vx, vy, htm, xData_[isp], yData_[jsp],zData_[jsp][isp] );
      }
      catch (EGraphicsError const &e)
      {
        throw;
      }
      if( zData_[jn][in] > contourValue_ )     // a sharp left turn
      {
        irp = in;
        jrp = jn;
      }
      else
      {
        try
        {
          ContourB( vx, vy, htm, xData_[in], yData_[jn], zData_[jn][in] );
        }
        catch (EGraphicsError const &e)
        {
          throw;
        }
        if( zData_[jnn][inn] > contourValue_ ) // continue straight through
        {
          isp = in;
          jsp = jn;
          irp = inn;
          jrp = jnn;
        }
        else                      // wide right turn
        {
          try
          {
            ContourB( vx, vy, htm, xData_[inn], yData_[jnn], zData_[jnn][inn] );
          }
          catch (EGraphicsError const &e)
          {
            throw;
          }
          isp = inn;
          jsp = jnn;
        }
      }
    }
    else                        // midpoint less than contour
    {
      try
      {
        ContourB( xData_[irp], yData_[jrp], zData_[jrp][irp], vx, vy, htm );
      }
      catch (EGraphicsError const &e)
      {
        throw;
      }
      if( zData_[jnn][inn] > contourValue_ )
      {
        try
        {
          ContourB( xData_[inn], yData_[jnn], zData_[jnn][inn], vx, vy, htm );
        }
        catch (EGraphicsError const &e)
        {
          throw;
        }
        if( zData_[jn][in] > contourValue_ )   // wide left turn
        {
          try
          {
            ContourB( xData_[in], yData_[jn], zData_[jn][in], vx, vy, htm );
          }
          catch (EGraphicsError const &e)
          {
            throw;
          }
          irp = in;
          jrp = jn;
        }
        else                      // continue straight through
        {
          irp = inn;
          jrp = jnn;
          isp = in;
          jsp = jn;
        }
      }
      else                        // turnoff sharp right
      {
        isp = inn;
        jsp = jnn;
      }
    }
  }  // end of major for loop
}

void GRA_contour::ContourB( double xrr, double yrr, double hr, double xss, double yss, double hs )
{
  double xr, yr, xs, ys;
  if( polar_ )
  {
    double const degreesToRadians = M_PI/180.;
    double temp = xrr * cos(yrr*degreesToRadians);
    yr = xrr * sin(yrr*degreesToRadians);
    xr = temp;
    temp = xss * cos(yss*degreesToRadians);
    ys = xss * sin(yss*degreesToRadians);
    xs = temp;
  }
  else
  {
    xr = xrr;
    yr = yrr;
    xs = xss;
    ys = yss;
  }
  //
  // linear interpolation for the contour
  //
  double ddx = xr - xs;
  double ddy = yr - ys;
  double frac = ( hr - contourValue_ ) / ( hr - hs );
  double plotx = xr-frac*ddx;
  double ploty = yr-frac*ddy;
  static double stx, sty, fswx, fswy, swx, swy;
  GRA_window *gw = ExGlobals::GetGraphWindow();
  if( labelSeparation_>0.0 && labelSize_>0.0 ) // plot numbers on the contours
  {
    if( pen_ == 3 ) // start a new contour
    {
      // the first number on each contour is saved for checking
      //
      gw->GraphToWorld( plotx, ploty, fswx, fswy );
    }
    else
    {
      double pwx, pwy;
      gw->GraphToWorld( plotx, ploty, pwx, pwy );
      gw->GraphToWorld( stx, sty, swx, swy );
      double distx = pwx - swx;
      double adistx = fabs( distx );
      double disty = pwy - swy;
      double adisty = fabs( disty );
      if( numin_ ) // already spacing for a number
      {
        // is the space big enough
        //
        if( adisty == 0.0 )return;
        double pltwx, pltwy;
        if( adisty >= xSep_ )
        {
          double temp;
          disty<0.0 ? temp=-fabs(xSep_) : temp=fabs(xSep_);
          pltwx = swx + xSep_*distx/adisty;
          pltwy = swy + temp;
        }
        else
        {
          if( adistx < xSep_ )return;
          //
          // plot the number
          //
          double temp;
          distx < 0.0 ? temp = -fabs(xSep_) : temp = fabs(xSep_);
          pltwx = swx + temp;
          pltwy = swy + xSep_*disty/adistx;
        }
        double numwx = (swx+pltwx)*0.5;
        double numwy = (swy+pltwy)*0.5;
        // use world units
        wxString s;
        s << drawnValue_;
        GRA_drawableText *dt = new GRA_drawableText(s,labelSize_,0.0,numwx,numwy,1,font_,color_);
        try
        {
          dt->Parse();
        }
        catch ( EGraphicsError const &e )
        {
          // throw;
        }
        textVec_.push_back( dt );
        pen_ = 2;
        numin_ = false;
        goto DONE;
      }
      //
      // should we start a new number
      // are we sep graph units from the last number plotted
      //
      if( adistx<labelSeparation_ && adisty<labelSeparation_ )goto DONE;
      //
      // or sep graph units from first number plotted for this contour
      //
      if( fabs(pwx-fswx)<labelSeparation_ && fabs(pwy-fswy)<labelSeparation_ )goto DONE;
    }
    //
    // start a new number
    //
    numin_ = true;
    stx = plotx;
    sty = ploty;
    gw->GraphToWorld( stx, sty, swx, swy );
  }
DONE:
  double xw, yw;
  gw->GraphToWorld( plotx, ploty, xw, yw );
  xPlot_.push_back( xw );
  yPlot_.push_back( yw );
  connect_.push_back( pen_==2 );
  pen_ = 2;
  return;
}

bool GRA_contour::Search( int ni, int nj, bool test )
{
  int const mask[32] =
  { 1, 2, 4, 8, 16, 32, 64, 128,
    256, 512, 1024, 2048, 4096, 8192, 16384,
    32768, 65536, 131072, 262144, 524288,
    1048576, 2097152, 4194304, 8388608,
    16777216, 33554432, 67108864, 134217728,
    268435456, 536870912, 1073741824, std::numeric_limits<int>::min() };
  //
  // if test = true then check the flag only
  //          else check the flag and set it to 1
  //
  int pos = nj*xData_.size() + ni+1;
  int word = pos/32;
  int ibit = pos - word*32;
  word = std::min(nArray_,word+1)-1;
  int temp = array_[word] & mask[ibit]; // and bits
  if( temp == 0 )
  {
    if( test )return true;
  }
  else
  {
    return false;
  }
  temp = array_[word] ^ mask[ibit]; // exclusive or
  array_[word] = temp;
  return true;
}

void GRA_contour::ClearAllSwitches()
{
  nArray_ = xData_.size()*yData_.size()/32+1;
  array_ = new int[nArray_];
  for( int i=0; i<nArray_; ++i )array_[i] = 0;
}

// end of file
