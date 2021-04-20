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

#include "GRA_rectangle.h"
#include "GRA_cross.h"
#include "GRA_crossPlus.h"
#include "GRA_plus.h"
#include "GRA_diamond.h"
#include "GRA_diamondPlus.h"
#include "GRA_asterisk.h"
#include "GRA_triangle.h"
#include "GRA_ellipse.h"
#include "GRA_star5pt.h"
#include "GRA_arrow3.h"

#include "GRA_plotSymbol.h"

int GRA_plotSymbol::numberOfShapes_ = 18;

GRA_plotSymbol::GRA_plotSymbol( int code, double size, double angle, GRA_color *color, int width )
    : GRA_drawableObject(), code_(code), size_(size), angle_(angle), lineWidth_(width),
      x_(0.0), y_(0.0)
{
  color ? color_=color : color_=GRA_colorControl::GetColor("BLACK");
  int c = abs(code_);
  shapeCode_ = (c-1)%numberOfShapes_ + 1;
  connectToPrevious_ = (code_>=0);
  //if( c==0 || c==32 )size_ = 0.0;
}

void GRA_plotSymbol::CopyStuff( GRA_plotSymbol const &rhs )
{
  x_ = rhs.x_;
  y_ = rhs.y_;
  shapeCode_ = rhs.shapeCode_;
  angle_ = rhs.angle_;
  size_ = rhs.size_;
  color_ = rhs.color_;
  lineWidth_ = rhs.lineWidth_;
  connectToPrevious_ = rhs.connectToPrevious_;
}

void GRA_plotSymbol::Draw( GRA_outputType *outputType )
{
  if( shapeCode_==0 || shapeCode_==32 )return;
  switch( shapeCode_ )
  {
    case 1:  // square
    {
      outputType->Draw( &GRA_rectangle(x_,y_,size_,size_,angle_,color_,0,lineWidth_) );
      break;
    }
    case 2:  // cross
    {
      outputType->Draw( &GRA_cross(x_,y_,size_,angle_,color_,lineWidth_) );
      break;
    }
    case 3:  // square with cross
    {
      outputType->Draw( &GRA_crossPlus(x_,y_,size_,angle_,color_,lineWidth_) );
      break;
    }
    case 4:  // plus
    {
      outputType->Draw( &GRA_plus(x_,y_,size_,angle_,color_,lineWidth_) );
      break;
    }
    case 5:  // diamond
    {
      outputType->Draw( &GRA_diamond(x_,y_,size_,angle_,color_,0,lineWidth_) );
      break;
    }
    case 6:  // diamond +
    {
      outputType->Draw( &GRA_diamondPlus(x_,y_,size_,angle_,color_,lineWidth_) );
      break;
    }
    case 7:  // asterisk
    {
      outputType->Draw( &GRA_asterisk(x_,y_,size_,angle_,color_,lineWidth_) );
      break;
    }
    case 8:  // triangle
    {
      outputType->Draw( &GRA_triangle(x_,y_,size_,angle_,color_,0,lineWidth_) );
      break;
    }
    case 9:  // circle
    {
      double radius = size_*0.5;
      outputType->Draw( &GRA_ellipse(x_-radius,y_-radius,x_+radius,y_+radius,true,color_,0,lineWidth_,1) );
      break;
    }
    case 10: // star
    {
      outputType->Draw( &GRA_star5pt(x_,y_,size_,angle_,color_,0,lineWidth_) );
      break;
    }
    case 11: // point
    {
      outputType->PlotPoint( x_, y_ );
      break;
    }
    case 12: // arrow with foot at (x_,y_)
    {
      outputType->Draw( &GRA_arrow3(x_,y_,size_,0.0,angle_,color_,lineWidth_) );
      break;
    }
    case 13: // arrow centred at (x_,y_)
    {
      outputType->Draw( &GRA_arrow3(x_,y_,size_,0.5*size_,angle_,color_,lineWidth_) );
      break;
    }
    case 14:  // filled square
    {
      outputType->Draw( &GRA_rectangle(x_,y_,size_,size_,angle_,color_,color_,lineWidth_) );
      break;
    }
    case 15:  // filled diamond
    {
      outputType->Draw( &GRA_diamond(x_,y_,size_,angle_,color_,color_,lineWidth_) );
      break;
    }
    case 16:  // filled triangle
    {
      outputType->Draw( &GRA_triangle(x_,y_,size_,angle_,color_,color_,lineWidth_) );
      break;
    }
    case 17:  // filled circle
    {
      double radius = size_*0.5;
      outputType->Draw( &GRA_ellipse(x_-radius,y_-radius,x_+radius,y_+radius,true,color_,color_,lineWidth_,1) );
      break;
    }
    case 18:  // filled star
    {
      outputType->Draw( &GRA_star5pt(x_,y_,size_,angle_,color_,color_,lineWidth_) );
      break;
    }
  }
}

std::ostream &operator<<( std::ostream &out, GRA_plotSymbol const &ps )
{
  return out << "<plotsymbol x=\"" << ps.x_ << "\" y=\"" << ps.y_
             << "\" shape=\"" << ps.shapeCode_ << "\" angle=\"" << ps.angle_
             << "\" size=\"" << ps.size_ << "\" color=\"" << GRA_colorControl::GetColorCode(ps.color_)
             << "\" linewidth=\"" << ps.lineWidth_ << "\"/>\n";
}

// end of file
