/*
Copyright (C) 2010 Joseph L. Chuma

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
#ifndef GRA_SHAPE
#define GRA_SHAPE

#include <fstream>

#include "ExString.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"

#include "GRA_drawableObject.h"

class GRA_shape : public GRA_drawableObject
{
public:
  GRA_shape( GRA_color *lineColor, GRA_color *fillColor,
             int lineWidth, int lineType, char const *type )
    : GRA_drawableObject(type), lineWidth_(lineWidth), lineType_(lineType),
      lineColor_(lineColor), fillColor_(fillColor)
  {}
  
  virtual ~GRA_shape()
  {}
  
  GRA_shape( GRA_shape const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff(rhs); }
  
  GRA_shape &operator=( GRA_shape const &rhs )
  {
    if( &rhs != this )
    {
      type_ = rhs.type_;
      CopyStuff(rhs);
    }
    return *this;
  }
  
  void SetLineColor( GRA_color *c )
  { lineColor_ = c; }
  
  void SetFillColor( GRA_color *c )
  { fillColor_ = c; }
  
  GRA_color *GetLineColor() const
  { return lineColor_; }
  
  GRA_color *GetFillColor() const
  { return fillColor_; }

  void SetLineWidth( int i )
  { lineWidth_ = i; }

  int GetLineWidth() const
  { return lineWidth_; }

  void SetLineType( int i )
  { lineType_ = i; }

  int GetLineType() const
  { return lineType_; }

  void GetCenter( double &x, double &y )
  { x=xc_; y=yc_; }
  
  void GetLimits( double &xmin, double &ymin, double &xmax, double &ymax ) const
  { xmin=xmin_; ymin=ymin_; xmax=xmax_; ymax=ymax_; }

protected:
  double RotX( double x, double y, double sinang, double cosang ) const
  { return x*cosang - y*sinang; }
  
  double RotY( double x, double y, double sinang, double cosang ) const
  { return y*cosang + x*sinang; }

  void CopyStuff( GRA_shape const &rhs )
  {
    xc_ = rhs.xc_;
    yc_ = rhs.yc_;
    xmin_ = rhs.xmin_;
    xmax_ = rhs.xmax_;
    ymin_ = rhs.ymin_;
    ymax_ = rhs.ymax_;
    lineColor_ = rhs.lineColor_;
    fillColor_ = rhs.fillColor_;
    lineWidth_ = rhs.lineWidth_;
    lineType_ = rhs.lineType_;
  }

  double xc_, yc_;
  double xmin_, ymin_, xmax_, ymax_;
  GRA_color *lineColor_, *fillColor_;
  int lineWidth_, lineType_;
};

#endif

