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
#include <cmath>

#include "GRA_axis.h"
#include "EGraphicsError.h"
#include "GRA_drawableText.h"
#include "GRA_wxWidgets.h"
#include "GRA_colorControl.h"
#include "GRA_color.h"
#include "GRA_font.h"
#include "GRA_polyline.h"
#include "GRA_characteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_window.h"
#include "UsefulFunctions.h"
#include "ExGlobals.h"

GRA_axis::GRA_axis( double xOrigin, double yOrigin, double length,
                    GRA_setOfCharacteristics const *characteristics )
    : GRA_drawableObject(wxT("AXIS")), xOrigin_(xOrigin), yOrigin_(yOrigin), length_(length),
      characteristics_( new GRA_setOfCharacteristics(*characteristics) )
{
  nGrid_ = static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("GRID")))->Get();
}

void GRA_axis::CopyStuff( GRA_axis const &rhs )
{
  if( rhs.characteristics_ )characteristics_ = new GRA_setOfCharacteristics(*rhs.characteristics_);
  //
  std::vector<GRA_polyline*>::const_iterator pEnd = rhs.polylines_.end();
  for( std::vector<GRA_polyline*>::const_iterator i=rhs.polylines_.begin(); i!=pEnd; ++i )
  { polylines_.push_back( new GRA_polyline(**i) ); }
  //
  std::vector<GRA_drawableText*>::const_iterator tEnd = rhs.textVec_.end();
  for( std::vector<GRA_drawableText*>::const_iterator i=textVec_.begin(); i!=tEnd; ++i )
    textVec_.push_back( new GRA_drawableText(**i) );
  //
  xTicCoordinates_.assign( rhs.xTicCoordinates_.begin(), rhs.xTicCoordinates_.end() );
  yTicCoordinates_.assign( rhs.yTicCoordinates_.begin(), rhs.yTicCoordinates_.end() );
  xOrigin_ = rhs.xOrigin_;
  yOrigin_ = rhs.yOrigin_;
  length_ = rhs.length_;
}

void GRA_axis::DeleteStuff()
{
  delete characteristics_;
  characteristics_ = 0;
  while( !polylines_.empty() )
  {
    delete polylines_.back();
    polylines_.pop_back();
  }
  while( !textVec_.empty() )
  {
    delete textVec_.back();
    textVec_.pop_back();
  }
}

void GRA_axis::InheritFrom( GRA_axis *inherit )
{
  delete characteristics_;
  inherit->characteristics_ ? characteristics_ = 0 :
      characteristics_ = new GRA_setOfCharacteristics(*inherit->characteristics_);
}

bool GRA_axis::Inside( double xloc, double yloc )
{
  double const degToRad = M_PI/180.;
  double const eps = 0.0001;
  double axisAngle =
    static_cast<GRA_angleCharacteristic*>(characteristics_->Get(wxT("AXISANGLE")))->Get();
  NormalizeAngle( axisAngle );
  double cosA = cos(axisAngle*degToRad);
  double sinA = sin(axisAngle*degToRad);
  if( fabs(cosA) <= eps )cosA = 0.0;
  if( fabs(sinA) <= eps )sinA = 0.0;
  //
  double const uAxis =
    static_cast<GRA_distanceCharacteristic*>(characteristics_->Get(wxT("UPPERAXIS")))->GetAsWorld();
  double const lAxis =
    static_cast<GRA_distanceCharacteristic*>(characteristics_->Get(wxT("LOWERAXIS")))->GetAsWorld();
  double const dx = uAxis - lAxis;
  //
  double const fudge = 0.05;
  double xlo = xOrigin_ - fudge*cosA - 2*fudge*sinA;
  double ylo = yOrigin_ - 2*fudge*cosA - fudge*sinA;
  double xup = xOrigin_ + (dx+fudge)*cosA + 2*fudge*sinA;
  double yup = yOrigin_ + 2*fudge*cosA + (dx+fudge)*sinA;
  return (xup-xloc)*(xloc-xlo)>0.0 && (yup-yloc)*(yloc-ylo)>0.0;
}
  
void GRA_axis::Make()
{
  if( static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("AXISON")))->Get() )
  {
    try
    {
      static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("LOGBASE")))->Get()<=1.0 ?
          MakeLinearAxis() : MakeLogAxis();
    }
    catch (EGraphicsError const &e)
    {
      throw;
    }
  }
}

void GRA_axis::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  std::vector<GRA_polyline*>::const_iterator pEnd = polylines_.end();
  for( std::vector<GRA_polyline*>::const_iterator i=polylines_.begin(); i!=pEnd; ++i )
    (*i)->Draw( graphicsOutput, dc );
  std::vector<GRA_drawableText*>::const_iterator tEnd = textVec_.end();
  for( std::vector<GRA_drawableText*>::const_iterator i=textVec_.begin(); i!=tEnd; ++i )
  {
    (*i)->Draw( graphicsOutput, dc );
    double width = (*i)->GetWidth();
    if( maxWidth_ < width )maxWidth_ = width;
  }
}

void GRA_axis::MakeLinearAxis()
{
  // Plot a general linear axis with equally spaced tic marks.
  // The axis both starts and ends with a large tic mark, i.e.,
  // it is subdivided into a whole number of equally spaced
  // large tic marks which are in turn subdivided into a whole
  // number of small tic marks.
  //
  double const degToRad = M_PI/180.;
  double const eps = 0.0001;
  //
  double const axisMax =
    static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("MAX")))->Get();
  double const axisMin =
    static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("MIN")))->Get();
  if( fabs(axisMax-axisMin) < std::numeric_limits<double>::epsilon() )return;
  //
  double const virtualMax =
    static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("VIRTUALMAX")))->Get();
  double const virtualMin =
    static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("VIRTUALMIN")))->Get();
  //
  int const powerAuto =
    static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("POWERAUTO")))->Get();
  double base =
    static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("LOGBASE")))->Get();
  if( powerAuto!=0 && base<=1.0 )Fnice();
  //
  // angle of the axis in degrees relative to the horizontal
  //
  double axisAngle =
    static_cast<GRA_angleCharacteristic*>(characteristics_->Get(wxT("AXISANGLE")))->Get();
  NormalizeAngle( axisAngle );
  double cosAxisAngle = cos(axisAngle*degToRad);
  double sinAxisAngle = sin(axisAngle*degToRad);
  if( fabs(cosAxisAngle) <= eps )cosAxisAngle = 0.0;
  if( fabs(sinAxisAngle) <= eps )sinAxisAngle = 0.0;
  //
  // angle of the axis numbers in degrees, w.r.t. the horizontal
  // The virtual pointers are vectors whose starting points are on the axis
  // at the large tic mark locations and whose end points determine the positions
  // of the axis number labels
  //
  double numberAngle =
    static_cast<GRA_angleCharacteristic*>(characteristics_->Get(wxT("NUMBERSANGLE")))->Get();
  NormalizeAngle( numberAngle );
  numberAngle -= axisAngle;
  double coslab = cos(numberAngle*degToRad);
  double sinlab = sin(numberAngle*degToRad);
  if( fabs(coslab) < eps )coslab = 0.0;
  if( fabs(sinlab) < eps )sinlab = 0.0;
  //
  bool tics =
    static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("TICSON")))->Get();
  bool ticsBothSides =
    static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("TICSBOTHSIDES")))->Get();
  //
  double ticAngle =
    static_cast<GRA_angleCharacteristic*>(characteristics_->Get(wxT("TICANGLE")))->Get();
  NormalizeAngle( ticAngle );
  double costic = cos(ticAngle*degToRad);
  double sintic = sin(ticAngle*degToRad);
  if( fabs(costic) <= eps )costic = 0.0;
  if( fabs(sintic) <= eps )sintic = 0.0;
  //
  // length of the virtual pointer (tic mark), which points to the location where the axis
  // number will be centered on its perimeter at the large tic mark locations
  //
  double imagTicAngle =
    static_cast<GRA_angleCharacteristic*>(characteristics_->Get(wxT("IMAGTICANGLE")))->Get();
  NormalizeAngle( imagTicAngle );
  double imagTicLength =
    static_cast<GRA_sizeCharacteristic*>(characteristics_->Get(wxT("IMAGTICLENGTH")))->GetAsWorld();
  double costn = cos(imagTicAngle*degToRad);
  double sintn = sin(imagTicAngle*degToRad);
  if( fabs(costn) < eps )costn = 0.0;
  if( fabs(sintn) < eps )sintn = 0.0;
  //
  // The number of large tic marks plotted on the axis which
  // delineate the large increments will be exactly numberOfLargeIncrements+1
  //
  int numberOfLargeIncrements =
    static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("NLINCS")))->Get();
  numberOfLargeIncrements = std::max(1,numberOfLargeIncrements);
  double largeTicLength =
    static_cast<GRA_sizeCharacteristic*>(characteristics_->Get(wxT("LARGETICLENGTH")))->GetAsWorld();
  //
  // number of small increments into which each of the large increments will be
  // subdivided. The number of small tic marks plotted between each large
  // increment will be exactly numberOfSmallIncrements-1
  //
  int numberOfSmallIncrements =
    static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("NSINCS")))->Get();
  numberOfSmallIncrements = std::max(1,numberOfSmallIncrements);
  double smallTicLength =
    static_cast<GRA_sizeCharacteristic*>(characteristics_->Get(wxT("SMALLTICLENGTH")))->GetAsWorld();
  //
  // (xticl,yticl) are the coordinates of the end of the virtual pointer (tic mark)
  // which points to the location where the axis number will be centered on its perimeter
  //
  double xticl = costn*imagTicLength;
  double yticl = sintn*imagTicLength;
  if( !tics )
  {
    largeTicLength = 0.0;
    smallTicLength = 0.0;
  }
  //
  bool numbers = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("NUMBERSON")))->Get();
  //
  // width in characters of the axis number format
  // Maximum value is 20. If <= 0 then no axis numbers will be plotted
  //
  int numberOfDigits =
    static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("NUMBEROFDIGITS")))->Get();
  numberOfDigits = std::min(20,std::max(numberOfDigits,0));
  //
  // number of decimal places in the axis number format
  // if numberOfDecimals < 0 then the decimal point is suppressed
  //
  int numberOfDecimals =
    static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("NUMBEROFDECIMALS")))->Get();
  //
  // height of the axis number labels, if numberHeight <= 0, no numbers will be plotted
  //
  double numberHeight =
    static_cast<GRA_sizeCharacteristic*>(characteristics_->Get(wxT("NUMBERSHEIGHT")))->GetAsWorld();
  if( numberHeight <= 0.0 )numbers = false;
  //
  // If "X" is an axis number value at a large tic mark location then the number
  // plotted will have value "C" with format %numberOfDigits.numberOfDecimals f)
  // where "X" = "C" * 10^power
  //
  // ticSpacing = the spacing between the small tic marks of the axis
  //
  double xinc = (virtualMax-virtualMin)/numberOfLargeIncrements/numberOfSmallIncrements;
  double slen = length_*(1+(axisMin-virtualMin + virtualMax-axisMax)/(axisMax-axisMin));
  double ticSpacing = slen/numberOfLargeIncrements/numberOfSmallIncrements;
  //
  int const lineWidth =
    static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("LINEWIDTH")))->Get();
  int const lineType = 1;
  GRA_color *color =
    static_cast<GRA_colorCharacteristic*>(characteristics_->Get(wxT("AXISCOLOR")))->Get();
  //
  bool dropFirstNumber = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("DROPFIRSTNUMBER")))->Get();
  bool dropLastNumber = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("DROPLASTNUMBER")))->Get();
  double axisModulo = static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("MOD")))->Get();
  double offSet = static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("OFFSET")))->Get();
  double power = static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("POWER")))->Get();
  bool leadingZeros = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("LEADINGZEROS")))->Get();
  GRA_font *numbersFont = static_cast<GRA_fontCharacteristic*>(characteristics_->Get(wxT("NUMBERSFONT")))->Get();
  GRA_color *numbersColor = static_cast<GRA_colorCharacteristic*>(characteristics_->Get(wxT("NUMBERSCOLOR")))->Get();
  //
  // draw the axis line
  //
  GRA_polyline *tmp = new GRA_polyline(xOrigin_,yOrigin_,lineWidth,lineType,color);
  xEnd_ = xOrigin_+length_*cosAxisAngle;
  yEnd_ = yOrigin_+length_*sinAxisAngle;
  tmp->Add( xEnd_, yEnd_, 2 );
  polylines_.push_back( tmp );
  //
  double x0 = xOrigin_ - (axisMin-virtualMin)/(axisMax-axisMin)*length_*cosAxisAngle;
  double y0 = yOrigin_ - (axisMin-virtualMin)/(axisMax-axisMin)*length_*sinAxisAngle;
  //
  int alignment;
  if( fabs(axisAngle) <= 45.0 )
  {
    alignment = imagTicAngle>180.0 ? 8 : 2; // 8=top centre, 2=bottom centre
  }
  else
  {
    alignment = imagTicAngle>180.0 ? 4 : 6; // 4=centre left, 6=centre right
  }
  //
  int ntics = numberOfLargeIncrements*numberOfSmallIncrements + 1;
  int i = 1;
  while( virtualMin-i*xinc >= axisMin )
  {
    double xs = x0 - cosAxisAngle*i*ticSpacing;
    double ys = y0 + sinAxisAngle*i*ticSpacing;
    int modinc = static_cast<int>(fmod(i++,numberOfSmallIncrements));
    double ticlen = modinc==0 ? largeTicLength : smallTicLength;
    if( modinc == 0 )
    {
      if( nGrid_ != 0 )
      {
        xTicCoordinates_.push_back(xs);
        yTicCoordinates_.push_back(ys);
      }
    }
    else
    {
      if( nGrid_ < 0 )
      {
        xTicCoordinates_.push_back(xs);
        yTicCoordinates_.push_back(ys);
      }
    }
    if( !tics )continue;
    //
    // (xt,yt) are the unrotated coordinates of the end of the
    // tic mark relative to the location "stic"
    //
    double xt = costic*ticlen;
    double yt = sintic*ticlen;
    //
    // (xts,yts) are rotated screen coordinates of end of the tic mark
    //
    double xts = xs + cosAxisAngle*xt - sinAxisAngle*yt;
    double yts = ys + sinAxisAngle*xt + cosAxisAngle*yt;
    //
    // Plot the tic mark by moving the pen from the tic mark location
    // (xs,ys) to the end of the tic mark (xts,yts) with the pen down
    //
    if( ticsBothSides )
    {
      // Plot the tic mark symmetrically on both sides of the
      // axis as a straight line segment crossing the axis at
      // an angle of ticAngle degrees
      //
      double xtsn = xs - cosAxisAngle*xt + sinAxisAngle*yt;
      double ytsn = ys - sinAxisAngle*xt - cosAxisAngle*yt;
      //
      GRA_polyline *tmp = new GRA_polyline(xts,yts,lineWidth,lineType,color);
      tmp->Add( xtsn, ytsn, 2 );
      polylines_.push_back( tmp );
    }
    else
    {
      GRA_polyline *tmp = new GRA_polyline(xs,ys,lineWidth,lineType,color);
      tmp->Add( xts, yts, 2 );
      polylines_.push_back( tmp );
    }
  }
  for( int i=0; i<ntics; ++i )
  {
    double xdum = virtualMin + i*xinc;
    if( i == ntics-1 )xdum = virtualMax;
    double xm1 = axisMax;
    if( xm1 > axisMin )xm1 = axisMin;
    double xm2 = axisMin;
    if( xm2 < axisMax )xm2 = axisMax;
    if( xdum < xm1 || xdum > xm2 )continue;
    double stic = i*ticSpacing;
    double xs = x0 + cosAxisAngle*stic;
    double ys = y0 + sinAxisAngle*stic;
    //
    // Determine whether the tic length ticlen is long or short
    //
    int modinc = static_cast<int>(fmod(i,numberOfSmallIncrements));
    double ticlen = modinc==0 ? largeTicLength : smallTicLength;
    if( modinc == 0 )
    {
      if( nGrid_ != 0 )
      {
        xTicCoordinates_.push_back(xs);
        yTicCoordinates_.push_back(ys);
      }
    }
    else
    {
      if( nGrid_ < 0 )
      {
        xTicCoordinates_.push_back(xs);
        yTicCoordinates_.push_back(ys);
      }
    }
    if( tics )
    {
      // (xt,yt) are the unrotated coordinates of the end of the
      // tic mark relative to the location "stic"
      //
      double xt = costic*ticlen;
      double yt = sintic*ticlen;
      //
      // (xts,yts) are rotated screen coordinates of end of the tic mark
      //
      double xts = xs + cosAxisAngle*xt - sinAxisAngle*yt;
      double yts = ys + sinAxisAngle*xt + cosAxisAngle*yt;
      //
      // Plot the tic mark by moving the pen from the tic mark location
      // (xs,ys) to the end of the tic mark (xts,yts) with the pen down
      //
      if( ticsBothSides )
      {
        // Plot the tic mark symmetrically on both sides of the
        // axis as a straight line segment crossing the axis at
        // an angle of ticAngle degrees
        //
        double xtsn = xs - cosAxisAngle*xt + sinAxisAngle*yt;
        double ytsn = ys - sinAxisAngle*xt - cosAxisAngle*yt;
        //
        GRA_polyline *tmp = new GRA_polyline(xts,yts,lineWidth,lineType,color);
        tmp->Add( xtsn, ytsn, 2 );
        polylines_.push_back( tmp );
      }
      else
      {
        GRA_polyline *tmp = new GRA_polyline(xs,ys,lineWidth,lineType,color);
        tmp->Add( xts, yts, 2 );
        polylines_.push_back( tmp );
      }
    }
    // Check to see if a number is to be plotted
    //
    if( modinc==0 && numberOfDigits!=0 && numbers &&
        (!dropFirstNumber || i!=0) && (!dropLastNumber || i!=ntics-1) )
    {
      // A number is to be plotted
      // Store the characters for the real number in String number
      // nlabch is the number of non-blank characters in number
      //
      double xdum2 = xdum;
      if( axisModulo < 0.0 )
      {
        xdum2 = fmod(xdum2,fabs(axisModulo));
      }
      else if( axisModulo > 0.0 )
      {
        xdum2 = fmod(xdum2,axisModulo);
        if( xdum2 < 0.0 )xdum2 = xdum2 + axisModulo;
      }
      xdum2 += offSet;
      //
      wxString number = DoubleToString( xdum2, static_cast<int>(power), numberOfDigits, numberOfDecimals );
      //
      // left justify
      //
      std::size_t i = number.find_first_not_of( wxT(' '), 0 );
      if( i > 0 )number.erase( 0, i );
      //
      std::size_t nSize = number.size();
      if( leadingZeros>0 && number.at(0)==wxT('.') )
      {
        number.resize( nSize+1 );
        for( std::size_t j=0; j<nSize; ++j )number.at(nSize-j) = number.at(nSize-j-1);
        number.at(0) = wxT('0');
      }
      if( number.at(0)==wxT('-') && number.at(1)==wxT('.') )
      {
        number.resize( nSize+1 );
        for( std::size_t j=0; j<nSize-1; ++j )number.at(nSize-j) = number.at(nSize-j-1);
        number.at(1) = wxT('0');
      }
      // Plot the number at an angle relative to the horizontal of numberAngle+axisAngle degrees
      //
      GRA_drawableText *tb = new GRA_drawableText( number, numberHeight, numberAngle+axisAngle,
                                                   xs+cosAxisAngle*xticl-sinAxisAngle*yticl,
                                                   ys+sinAxisAngle*xticl+cosAxisAngle*yticl,
                                                   alignment, numbersFont, numbersColor );
      try
      {
        tb->Parse();
      }
      catch ( EGraphicsError &e )
      {
        throw;
      }
      textVec_.push_back( tb );
    }
  }
  i = 1;
  while( virtualMax+i*xinc <= axisMax )
  {
    double xs = x0+cosAxisAngle*(ntics-1+i)*ticSpacing;
    double ys = y0+sinAxisAngle*i*ticSpacing;
    int modinc = static_cast<int>(fmod(i++,numberOfSmallIncrements));
    double ticlen = modinc==0 ? largeTicLength : smallTicLength;
    if( modinc == 0 )
    {
      if( nGrid_ != 0 )
      {
        xTicCoordinates_.push_back(xs);
        yTicCoordinates_.push_back(ys);
      }
    }
    else
    {
      if( nGrid_ < 0 )
      {
        xTicCoordinates_.push_back(xs);
        yTicCoordinates_.push_back(ys);
      }
    }
    if( !tics )continue;
    //
    // (xt,yt) are the unrotated coordinates of the end of the
    // tic mark relative to the location "stic"
    //
    double xt = costic*ticlen;
    double yt = sintic*ticlen;
    //
    // (xts,yts) are rotated coordinates of end of the tic mark
    //
    double xts = xs + cosAxisAngle*xt - sinAxisAngle*yt;
    double yts = ys + sinAxisAngle*xt + cosAxisAngle*yt;
    //
    // Plot the tic mark by moving the pen from the tic mark location
    // (xs,ys) to the end of the tic mark (xts,yts) with the pen down
    //
    if( ticsBothSides )
    {
      // Plot the tic mark symmetrically on both sides of the
      // axis as a straight line segment crossing the axis at
      // an angle of ticAngle degrees
      //
      double xtsn = xs - cosAxisAngle*xt + sinAxisAngle*yt;
      double ytsn = ys - sinAxisAngle*xt - cosAxisAngle*yt;
      //
      GRA_polyline *tmp = new GRA_polyline(xts,yts,lineWidth,lineType,color);
      tmp->Add( xtsn, ytsn, 2 );
      polylines_.push_back( tmp );
    }
    else
    {
      GRA_polyline *tmp = new GRA_polyline(xs,ys,lineWidth,lineType,color);
      tmp->Add( xts, yts, 2 );
      polylines_.push_back( tmp );
    }
  }
}

void GRA_axis::MakeLogAxis()
{
  // Plot a general logarithmic axis with tic marks
  //
  double const degToRad = M_PI/180.;
  double const eps = 0.0001;
  //
  double const axisMax =
    static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("MAX")))->Get();
  double const axisMin =
    static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("MIN")))->Get();
  //
  if( fabs(axisMax-axisMin)<std::numeric_limits<double>::epsilon() )return;
  //
  double const base = static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("LOGBASE")))->Get();
  int const intBase = static_cast<int>(base);
  double const logBase = log(base);
  double const actualMin = exp(axisMin*logBase);
  double const actualMax = exp(axisMax*logBase);
  //
  // if logStyle_ = false, write the axis labels as decimal numbers
  // instead of as powers, e.g., 0.1  1  10   100   1000  instead of
  //
  //                             -1    0    2    3
  //                           10    10   10   10
  //
  bool logStyle = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("LOGSTYLE")))->Get();
  if( !logStyle && fabs(base-exp(1.0))<0.02 )logStyle = true;
  //
  // axisAngle = angle of the axis in degrees relative to the horizontal
  //
  double axisAngle = static_cast<GRA_angleCharacteristic*>(characteristics_->Get(wxT("AXISANGLE")))->Get();
  NormalizeAngle( axisAngle );
  double cosAxisAngle = cos(axisAngle*degToRad);
  double sinAxisAngle = sin(axisAngle*degToRad);
  if( fabs(cosAxisAngle) <= eps )cosAxisAngle = 0.0;
  if( fabs(sinAxisAngle) <= eps )sinAxisAngle = 0.0;
  //
  int const lineWidth = static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("LINEWIDTH")))->Get();
  int const lineType = 1;
  GRA_color *color = static_cast<GRA_colorCharacteristic*>(characteristics_->Get(wxT("AXISCOLOR")))->Get();
  //
  double numberAngle = static_cast<GRA_angleCharacteristic*>(characteristics_->Get(wxT("NUMBERSANGLE")))->Get();
  NormalizeAngle( numberAngle );
  numberAngle -= axisAngle; // angle of the axis numbers in degrees, w.r.t. the horizontal
  //
  double ticAngle = static_cast<GRA_angleCharacteristic*>(characteristics_->Get(wxT("TICANGLE")))->Get();
  NormalizeAngle( ticAngle );
  double cosTicAngle = cos(ticAngle*degToRad);
  double sinTicAngle = sin(ticAngle*degToRad);
  if( fabs(cosTicAngle) <= eps )cosTicAngle = 0.0;
  if( fabs(sinTicAngle) <= eps )sinTicAngle = 0.0;
  //
  // nlinc = number of large increments into which the axis is
  // to be subdivided. The number of large tic marks plotted on the axis which
  // delineate the large increments will be exactly nlinc+1
  //
  int numberOfLargeIncrements =
    static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("NLINCS")))->Get();
  std::size_t nlinc = std::max(1,numberOfLargeIncrements);
  double largeTicLength =
    static_cast<GRA_sizeCharacteristic*>(characteristics_->Get(wxT("LARGETICLENGTH")))->GetAsWorld();
  double smallTicLength =
    static_cast<GRA_sizeCharacteristic*>(characteristics_->Get(wxT("SMALLTICLENGTH")))->GetAsWorld();
  //
  bool tics = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("TICSON")))->Get();
  if( !tics )
  {
    largeTicLength = 0.0;
    smallTicLength = 0.0;
  }
  bool ticsBothSides = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("TICSBOTHSIDES")))->Get();
  double imagTicLength =
    static_cast<GRA_sizeCharacteristic*>(characteristics_->Get(wxT("IMAGTICLENGTH")))->GetAsWorld();
  //
  // length of the virtual pointer, which points to the location where the axis
  // number will be centered on its perimeter at the large tic mark locations
  //
  double imagTicAngle = static_cast<GRA_angleCharacteristic*>(characteristics_->Get(wxT("IMAGTICANGLE")))->Get();
  NormalizeAngle( imagTicAngle );
  double cosImagTicAngle = cos(imagTicAngle*degToRad);
  double sinImagTicAngle = sin(imagTicAngle*degToRad);
  if( fabs(cosImagTicAngle) < eps )cosImagTicAngle = 0.0;
  if( fabs(sinImagTicAngle) < eps )sinImagTicAngle = 0.0;
  //
  // (xticl,yticl) are the coordinates of the end of the virtual pointer which points
  // to the location where the axis number will be centered on its perimeter
  //
  double xticl = cosImagTicAngle*imagTicLength;
  double yticl = sinImagTicAngle*imagTicLength;
  if( logStyle && axisAngle==0.0 ) // move exponential style numbers on x-axis further away
  {
    xticl *= 2.0;
    yticl *= 2.0;
  }
  //
  bool numbers = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("NUMBERSON")))->Get();
  double numberHeight =
    static_cast<GRA_sizeCharacteristic*>(characteristics_->Get(wxT("NUMBERSHEIGHT")))->GetAsWorld();
  if( numberHeight <= 0.0 )numbers = false;
  //
  bool dropFirstNumber = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("DROPFIRSTNUMBER")))->Get();
  bool dropLastNumber = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("DROPLASTNUMBER")))->Get();
  GRA_font *numbersFont = static_cast<GRA_fontCharacteristic*>(characteristics_->Get(wxT("NUMBERSFONT")))->Get();
  GRA_color *numbersColor = static_cast<GRA_colorCharacteristic*>(characteristics_->Get(wxT("NUMBERSCOLOR")))->Get();
  //
  double const gapLength = length_/(axisMax-axisMin);
  //
  // draw the axis line
  //
  GRA_polyline *tmp = new GRA_polyline(xOrigin_,yOrigin_,lineWidth,lineType,color);
  xEnd_ = xOrigin_+length_*cosAxisAngle;
  yEnd_ = yOrigin_+length_*sinAxisAngle;
  tmp->Add( xEnd_, yEnd_, 2 );
  polylines_.push_back( tmp );
  //
  double xs, ys;
  int numberOfSmallTics = intBase-2;
  std::vector<double> fractionOfGap( numberOfSmallTics );
  for( int i=0; i<numberOfSmallTics; ++i )fractionOfGap[i] = log(i+2)/logBase;
  //
  // find first major tic mark <= axis minimum
  //
  int powMin = axisMin>=0.0 ? static_cast<int>(axisMin) : static_cast<int>(axisMin-0.9999);
  double const actualPowMin = exp(powMin*logBase);
  //
  // convert the base into the string label with nsig1 characters
  // put the up code (<^>) at the end of the string
  // this signifies that an exponent is to follow and therefore
  // to be shifted up as well as reduced in size
  //
  bool leadingZeros =
   static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("LEADINGZEROS")))->Get();
  wxString label;
  fabs(base-exp(1.0)) < 0.02 ? label=wxT('e') : label<<intBase;
  int alignment;
  if( fabs(axisAngle) <= 45.0 )
    alignment = imagTicAngle>180.0 ? 8 : 2; // 8=top centre, 2=bottom centre
  else
    alignment = imagTicAngle>180.0 ? 4 : 6; // 4=centre left, 6=centre right
  //
  // number small tic marks when no major tics will be visible
  //
  bool numberSmallTics = numbers &&
                         ((static_cast<int>(axisMax)-static_cast<int>(axisMin))==0);
  if( powMin < axisMin )
  {
    double const vminLoc = (axisMin-powMin)*gapLength;
    for( int i=0; i<numberOfSmallTics; ++i )
    {
      double actualValue = actualPowMin*(i+2);
      if( actualMin<=actualValue && actualValue<=actualMax )
      {
        double stic = fractionOfGap[i]*gapLength - vminLoc; // distance from origin to tic mark
        MakeATic( stic, cosAxisAngle, sinAxisAngle, cosTicAngle, sinTicAngle, smallTicLength, xs, ys );
        if( nGrid_ < 0 )
        {
          xTicCoordinates_.push_back(xs);
          yTicCoordinates_.push_back(ys);
        }
        if( numberSmallTics )
        {
          wxString lab2;
          if( actualValue>9999999.0 || actualValue<0.0000001 ) // too many digits, use log style
          {
            wxChar s[25];
            ::wxSnprintf( s, 25, wxT("%024u"), i+2 );
            std::size_t idc1;
            for( idc1=0; idc1<wxStrlen(s); ++idc1 )
            {
              if( s[idc1] != wxT('0') )break;
            }
            for( std::size_t i=idc1; i<wxStrlen(s); ++i )lab2 += wxString( s[i] );
            lab2 << wxT("*") << label << wxT("<^>") << powMin;
          }
          else
          {
            wxChar s[25];
            if( actualValue >= 1.0 )::wxSnprintf( s, 25, wxT("%024u"),
                                                  static_cast<int>(actualValue+0.5) );
            else                    ::wxSnprintf( s, 25, wxT("%024.7g"), actualValue );
            std::size_t idc1;
            for( idc1=0; idc1<wxStrlen(s); ++idc1 )
            {
              if( s[idc1] != wxT('0') )break;
            }
            if( actualValue<1.0 && leadingZeros )--idc1;
            for( std::size_t i=idc1; i<wxStrlen(s); ++i )lab2 += wxString( s[i] );
          }
          GRA_drawableText *tb = new GRA_drawableText( lab2, numberHeight, numberAngle+axisAngle,
                                                       xs+cosAxisAngle*xticl-sinAxisAngle*yticl,
                                                       ys+sinAxisAngle*xticl+cosAxisAngle*yticl,
                                                       alignment, numbersFont, numbersColor );
          try
          {
            tb->Parse();
          }
          catch ( EGraphicsError &e )
          {
            throw;
          }
          textVec_.push_back( tb );
        }
      }
    }
  }
  //
  // find first major tic mark >= axis minimum
  // and last major tic mark <= axis maximum
  //
  powMin = axisMin>=0.0 ? static_cast<int>(axisMin+0.9999) : static_cast<int>(axisMin);
  int powMax = axisMax>=0.0 ? static_cast<int>(axisMax) : static_cast<int>(axisMax-0.9999);
  //
  double const offset = (powMin-axisMin)*gapLength;
  for( int ipow=powMin; ipow<=powMax; ++ipow )
  {
    // draw large (labeled) tic mark
    //
    double const ltic = offset + (ipow-powMin)*gapLength; // distance from lAxis to large tic mark
    MakeATic( ltic, cosAxisAngle, sinAxisAngle, cosTicAngle, sinTicAngle, largeTicLength, xs, ys );
    if( nGrid_ != 0 )
    {
      xTicCoordinates_.push_back(xs);
      yTicCoordinates_.push_back(ys);
    }
    // plot number labels at the large tic marks
    //
    if( numbers )
    {
      // a label is to be plotted
      // ipow is the power of the label being plotted
      // append the characters of the power to the base + <^>
      //
      std::size_t idc1;
      wxString lab2;
      lab2.clear();
      if( logStyle )
      {
        lab2 << label << wxT("<^>") << ipow;
        idc1 = 1;
      }
      else // draw decimal style numbers, e.g., 0.001, 0.01
      {
        if( ipow*logBase > log(std::numeric_limits<double>::max()) )
          throw EGraphicsError(wxT("problem with log scales"));
        double xout = pow(base,static_cast<double>(ipow));
        if( xout>9999999.0 || xout<0.0000001 ) // too many digits, use log style
        {
          lab2 << label << wxT("<^>") << ipow;
          idc1 = 1;
        }
        else
        {
          wxChar s[25];
          ::wxSnprintf( s, 25, wxT("%024.7f"), xout );
          for( idc1=0; idc1<wxStrlen(s); ++idc1 )
          {
            if( s[idc1] != wxT('0') )break;
          }
          int idc2;
          for( idc2=wxStrlen(s)-1; idc2>=0; --idc2 )
          {
            if( s[idc2] != wxT('0') )break;
          }
          if( s[idc1] == '.' )--idc1;
          if( s[idc2] == '.' )--idc2;
          for( int i=idc1; i<=idc2; ++i )lab2 += wxString( s[i] );
        }
      }
      // plot the label at an angle relative to the horizontal of angle degrees
      //
      GRA_drawableText *tb = new GRA_drawableText( lab2, numberHeight, numberAngle+axisAngle,
                                                   xs+cosAxisAngle*xticl-sinAxisAngle*yticl,
                                                   ys+sinAxisAngle*xticl+cosAxisAngle*yticl,
                                                   alignment, numbersFont, numbersColor );
      try
      {
        tb->Parse();
      }
      catch ( EGraphicsError &e )
      {
        throw;
      }
      textVec_.push_back( tb );
    }
    double const largeTicValue = exp(ipow*logBase);
    for( int j=0; j<numberOfSmallTics; ++j )
    {
      double actualValue = (j+2)*largeTicValue;
      if( actualValue <= actualMax )
      {
        double stic = fractionOfGap[j]*gapLength + ltic; // distance from lAxis to small tic mark
        MakeATic( stic, cosAxisAngle, sinAxisAngle, cosTicAngle, sinTicAngle, smallTicLength, xs, ys );
        if( nGrid_ < 0 )
        {
          xTicCoordinates_.push_back(xs);
          yTicCoordinates_.push_back(ys);
        }
      }
    }
  }
}

void GRA_axis::MakeATic( double stic,
                         double costh, double sinth,
                         double costic, double sintic,
                         double ticLength, double &xs, double &ys )
{
  xs = xOrigin_ + costh*stic;
  ys = yOrigin_ + sinth*stic;
  //
  polylines_.back()->Add( xs, ys, 2 );
  //
  // (xt,yt) are the unrotated coordinates of the end of the tic mark
  // relative to the location stic
  //
  double xt = costic*ticLength;
  double yt = sintic*ticLength;
  //
  // (xts,yts) are the rotated screen coordinates of the end of the tic mark
  //
  double xts = xs + costh*xt - sinth*yt;
  double yts = ys + sinth*xt + costh*yt;
  //
  // plot the tic mark by moving the pen from the tic mark location
  // (xs,ys) to the end of the tic mark (xts,yts) with the pen down
  //
  polylines_.back()->Add( xts, yts, 2 );
  //
  bool ticsBothSides = static_cast<GRA_boolCharacteristic*>(characteristics_->Get(wxT("TICSBOTHSIDES")))->Get();
  if( ticsBothSides )
  {
    // plot the tic mark symmetrically on both sides of the axis as a
    // straight line segment crossing the axis at an angle of ticAngle degrees
    //
    double xtsn = xs - costh*xt + sinth*yt;
    double ytsn = ys - sinth*xt - costh*yt;
    //
    polylines_.back()->Add( xtsn, ytsn, 2 );
  }
  polylines_.back()->Add( xs, ys, 2 );
}

void GRA_axis::Fnice()
{
  // This method calculates exactly the number of digits required to display "XMIN" and
  // "XMAX" such that they can be displayed in f-format, and such that they include enough
  // decimal places so that the effect of the first two (one if it has only one) significant
  // digits of "XINC" can be observed.
  //          - If NDEC < 0, the "." is suppressed
  //               although NWID still includes a position for it.
  //          - If NWID < 0 then FNICE will try to fit XMIN and
  //               XMAX into ABS(NWID) --without loosing the
  //               significance from XINC-- by returning a factor
  //               (10**IPOW) by which XMIN and XMAX should be multiplied.
  //          - Primarily used for labelling axis on plots nicely.
  //
  // XMIN & XMAX   -- ends of range
  // XINC          -- increment
  // NWID          -- < 0 --> fit in XMIN & XMAX into ABS(NWID)by returning IPOW
  //                  width of field includes "." (& "-" if needed)
  // NDEC          -- < 0  --> if inc is an integer then supress "."
  //                  >= 0 --> include "."
  //                  #no. of digs after "."
  // IPOW          -- 10^IPOW is a multiplication factor used to fit XMIN & XMAX into -NWID.
  // Note 1 - NWID ALWAYS INCLUDES the "."
  // Note 2 - the numbers are rounded off before the calculations
  //          are carried out so that there are always enough digits included.
  // Example -
  //       fnice(-12.0135,5.,.15,nwid,ndec,ipow)
  //   returns
  //       nwid=6, ndec=2, ipow=0
  // Examples (cont) -
  //
  //                input                        |       output
  //   xmin     xmax     xinc     nwid     ndec  |  nwid  ndec  ipow
  //  -12.0135   5.        .15      0        0   |    6     2     0
  //    9.9      9.99      .01      0        0   |    4     2     0
  //    1.0      9.9      1.0       0        0   |    3     0     0
  //    1.0      9.9      1.0       0       -1   |    3    -1     0
  //    2.5    122.5     12.5       0        0   |    4     0     0
  //  100.1    100.6       .1      -5        0   |    5     1     0
  //  100.1    100.6       .1      -1        0   |    5     1     0
  // 1000.    5000.     100.       -5        0   |    5     0     0
  // 1000.    5000.     100.       -3        0   |    3    -1     2
  // 1000.    5000.     100.       -4        0   |    3    -1     2
  //     .1234    .1239    .00015  -3        0   |    6     5     0
  //     .115     .145     .015    -3        0   |    4     3     0
  //     .0045    .0090    .0015   -3        0   |    3    -1    -4
  //
  double virtualMax = static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("VIRTUALMAX")))->Get();
  double virtualMin = static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("VIRTUALMIN")))->Get();
  double vmin = fabs(virtualMin);
  double vmax = fabs(virtualMax);
  int numberOfLargeIncrements = static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("NLINCS")))->Get();
  double inc = fabs((virtualMax-virtualMin)/numberOfLargeIncrements);
  if( inc == 0.0 )inc = 1.0;
  //
  int numberOfDigits = static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("NUMBEROFDIGITS")))->Get();
  numberOfDigits *= -1;
  int nwidor = numberOfDigits;      // original
  int numberOfDecimals = static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("NUMBEROFDECIMALS")))->Get();
  int ndecor = numberOfDecimals;    // original
  numberOfDecimals = 0;
  double power = 0.0;
  //
  // shift the decimal place left or right until we get;
  // 10 <= inc < 100, and keep track of the number of shifts;
  // so that we know how many decimal places we need;
  //
  // section for shifting decimal right for everything.
  //
  while( inc < 10.0 )
  {
    vmin *= 10.0;
    vmax *= 10.0;
    inc *= 10.0;
    ++numberOfDecimals;
  }
  // section for shifting decimal left for everything
  //
  while( inc >= 100.0 )
  {
    vmin /= 10.0;
    vmax /= 10.0;
    inc /= 10.0;
    --numberOfDecimals;
  }
  inc = floor(inc+0.5);
  double eps = 0.0001;
  if( fabs(fmod(inc,10.0))<eps && fabs(fmod(vmin,10.0))<eps && fabs(fmod(vmax,10.0))<eps )
  {
    vmin /= 10.0;
    vmax /= 10.0;
    inc /= 10.0;
    --numberOfDecimals;
  }
  //
  // determine number of significant digits of virtualMin
  //
  vmin = floor(vmin);
  int nsigmin = 1;
  if( vmin >= 1.0 )nsigmin = static_cast<int>(log10(vmin)) + 1;
  //
  // determine nwidmin (numberOfDigits for virtualMin) and include a position for the decimal '.'
  //
  int nwidmin = 0;
  if( numberOfDecimals > nsigmin )
    nwidmin = numberOfDecimals+1;
  else if ( numberOfDecimals <= 0 )
    nwidmin = nsigmin-numberOfDecimals+1;
  else
    nwidmin = nsigmin+1;
  if( virtualMin < 0.0 )++nwidmin; // include '-'
  //
  // determine number of significant digits of virtualMax ;
  //
  vmax = ceil(vmax);
  int nsigmax = 0;
  if( vmax < 1.0 )
    nsigmax = 1;
  else
    nsigmax = static_cast<int>(log10(vmax))+1;
  //
  // determine nwidmax (numberOfDigits for virtualMax) and include a position for the decimal '.'
  //
  int nwidmax = 0;
  if( numberOfDecimals > nsigmax )nwidmax = numberOfDecimals+1;
  else if( numberOfDecimals <= 0 )nwidmax = nsigmax-numberOfDecimals+1;
  else                            nwidmax = nsigmax+1;
  if( virtualMax < 0.0 )++nwidmax; // include '-'
  //
  // numberOfDigits is the larger of the nwidmin and nwidmax
  //
  nwidmin<nwidmax ? numberOfDigits=nwidmax : numberOfDigits=nwidmin;
  if( ndecor<0 && numberOfDecimals<=0 )--numberOfDigits;
  if( nwidor>=0  || numberOfDigits<=-nwidor )
  {
    if( numberOfDecimals <= 0 )
    {
      numberOfDecimals = 0;
      if( ndecor < 0 )numberOfDecimals = -1;
    }
  }
  else
  {
    // if we returned above, then there was nothing to check for
    // problems fitting numberOfDigits, but if we continue on
    // to here, we find that we must use power_ to fit numberOfDigits.  we also
    // check to make sure that the new numberOfDigits is really an improvement
    // over the old numberOfDigits
    //
    if( virtualMin < 0.0 )++nsigmin;  // include '-'
    if( virtualMax < 0.0 )++nsigmax;  // include '-'
    int nwidtemp = std::max(nsigmin,nsigmax);
    //
    if( numberOfDigits <= nwidtemp ) // not improved
    {
      if( numberOfDecimals <= 0 )
      {
        numberOfDecimals = 0;
        if( ndecor < 0 )numberOfDecimals = -1;
      }
    }
    else                             // improvement
    {
      numberOfDigits = nwidtemp;
      power = static_cast<double>(-numberOfDecimals);
      numberOfDecimals = -1;
    }
  }
  static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("NUMBEROFDIGITS")))->Set( numberOfDigits );
  static_cast<GRA_intCharacteristic*>(characteristics_->Get(wxT("NUMBEROFDECIMALS")))->Set( numberOfDecimals );
  static_cast<GRA_doubleCharacteristic*>(characteristics_->Get(wxT("POWER")))->Set( power );
}

wxString GRA_axis::DoubleToString( double r, int power, int nPos, int nDec ) const
{
  // DoubleToString converts the double r into a string s, with format
  // f(nPos).(nDec) such that: r = s*pow(10,power), and the string s is
  // right-justified in the field width of nPos characters.
  //
  // convert the scaled number r/pow(10,power) into s
  //
  // if the number of decimal places, nDec < 0, then the decimal point
  // in the character representation of r will be suppressed, i.e., it
  // is assumed to be on the far right of the nPos characters of s.
  //
  if( nPos <= 0 )return wxString(wxT(" "));
  wxChar digit[] = wxT("0123456789");
  wxChar minus=wxT('-'), dec=wxT('.'), blank=wxT(' '), star=wxT('*');
  //
  // determine nint, the number of digits to the left of the
  // decimal point, and ndec2, the number of decimal places,
  // allowed by the format specification and the sign of r.
  // ndig=ndec2+nint = number of digits allowed by the format.
  // if nint < 0 or ndig <= 0 then an format overflow
  // has occured, hence fill s with "*"s
  //
  int nDec1 = std::max(nDec,-1);
  int nDec2 = std::max(nDec, 0);
  int nint = nPos-nDec1-1;
  if( r < 0.0 )--nint;
  if( nint<0 || nint+nDec2<=0 )return wxString(nPos,star);
  //
  // if flag = false, determine power so that r/pow(10,power) will fit
  // exactly in the format specification (even when the number is rounded)
  //
  double powdec = pow(10.0,nDec2);
  double ra = fabs(r);
  //
  // scale ra = |r| down by pow(10,power)
  //
  // exp10(r,power) = r/pow(10.0,power)
  // exp10 is used instead of "pow" because |power| may be too large
  // example: if r=1.e-70 and power = -100 then pow(10.0,power) is
  //          undefined (underflow) but r/pow(10.0,power) =
  //          1.e-70/pow(10,-100) = 1.e30 is defined
  //
  ra = UsefulFunctions::exp10(ra,power);
  //
  // extract the required number of digits from ra,
  // and convert them to characters
  //
  wxString s( nPos, blank );
  double realn = powdec*ra + 0.5;
  for( int i=0; i<nPos; ++i )
  {
    if( i!=nDec2 || nDec1<0 )
    {
      int index = static_cast<int>(fmod(realn,10.0));
      if( index<0 || index>9 )index = 0;
      s.at(nPos-i-1) = digit[index];
      realn /= 10.0;
    }
    else
    {
      s.at(nPos-i-1) = dec;
    }
  }
  // if realn >= 1 then realn has overflowed the format, because there are
  // still non-zero leading digits which do not fit into the nPos positions of s
  //
  if( realn >= 1.0 )return wxString(nPos,star);
  //
  // determine the first non-zero character
  //
  std::size_t i = s.find_first_not_of( digit[0], 0 );
  if( i == s.npos )i = nPos-1;
  s.replace( 0, i, i, blank );
  //
  if( r >= 0.0 )
  {
    // if the decimal point is the first non-zero character,
    // then put a leading zero in front if there is room
    //
    if( s.at(i) == wxT('.') && i > 0 )s.at(i-1) = digit[0];
  }
  else     // r < 0, so put a minus sign
  {
    if( i == 0 )
    {
      s.insert( static_cast<std::size_t>(0), nPos, star );
    }
    else
    {
      if( s.at(i) == wxT('.') )  // first non-zero character is a '.'
      {
        if( i == 1 )
        {
          s.at(0) = minus;
        }
        else
        {
          s.at(i-1) = digit[0];
          s.at(i-2) = minus;
        }
      }
      else
      {
        s.at(i-1) = minus;
      }
    }
  }
  //
  // strip off leading blanks, i.e.,left justify
  //
  i = s.find_first_not_of( wxT(' '), 0 );
  if( i > 0 )s.erase( 0, i );
  //
  return s;
}

double GRA_axis::GetLowerAxis() const
{ return static_cast<GRA_distanceCharacteristic*>(characteristics_->Get(wxT("LOWERAXIS")))->GetAsWorld(); }

double GRA_axis::GetUpperAxis() const
{ return static_cast<GRA_distanceCharacteristic*>(characteristics_->Get(wxT("UPPERAXIS")))->GetAsWorld(); }

double GRA_axis::GetAngle() const
{ return static_cast<GRA_angleCharacteristic*>(characteristics_->Get(wxT("AXISANGLE")))->Get(); }

std::ostream &operator<<( std::ostream &out, GRA_axis const &axis )
{
  out << "<axis xorigin=\"" << axis.xOrigin_ << "\" yorigin=\"" << axis.yOrigin_
      << "\" length=\"" << axis.length_ << "\">\n";
  std::size_t size = axis.xTicCoordinates_.size();
  out << "<ticcoordinates size=\"" << size << "\">";
  for( std::size_t i=0; i<size; ++i )
    out << axis.xTicCoordinates_[i] << " " << axis.yTicCoordinates_[i] << " ";
  out << "</ticcoordinates>\n";
  out << "<axisc>\n" << *axis.characteristics_ << "</axisc>\n";
  size = axis.polylines_.size();
  out << "<polylines size=\"" << size << "\">\n";
  std::vector<GRA_polyline*>::const_iterator pend = axis.polylines_.end();
  for( std::vector<GRA_polyline*>::const_iterator i=axis.polylines_.begin(); i!=pend; ++i )out << **i;
  out << "</polylines>\n";
  size = axis.textVec_.size();
  out << "<drawabletexts size=\"" << size << "\">\n";
  std::vector<GRA_drawableText*>::const_iterator tend = axis.textVec_.end();
  for( std::vector<GRA_drawableText*>::const_iterator i=axis.textVec_.begin(); i!=tend; ++i )out << **i;
  return out << "</drawabletexts>\n</axis>\n";
}

// end of file
