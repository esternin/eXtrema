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

#include "GRA_colorControl.h"

#include "GRA_simpleText.h"

void GRA_simpleText::CopyStuff( GRA_simpleText const &rhs )
{
  string_ = rhs.string_;
  color_ = rhs.color_;
  height_ = rhs.height_;
  font_ = rhs.font_;
  xShift_ = rhs.xShift_;
  yShift_ = rhs.yShift_;
}

bool GRA_simpleText::operator==( GRA_simpleText const &rhs ) const
{
  if( string_ != rhs.string_ || height_ != rhs.height_ ||
      color_ != rhs.color_  || font_ != rhs.font_ ||
      xShift_ != rhs.xShift_ || yShift_ != rhs.yShift_ )return false;
  return true;
}

void GRA_simpleText::SetBoundary( int x, int y, int width, int height )
{
  xLo_ = x;
  yLo_ = y;
  xHi_ = x+width;
  yHi_ = y+height;
}

void GRA_simpleText::GetBoundary( int &xLo, int &yLo, int &xHi, int &yHi )
{
  xLo = xLo_;
  yLo = yLo_;
  xHi = xHi_;
  yHi = yHi_;
  return;
}

std::ostream &operator<<( std::ostream &out, GRA_simpleText const &st )
{
  return out << "<simpletext height=\"" << st.height_ << "\" xshift=\"" << st.xShift_
             << "\" yshift=\"" << st.yShift_ << "\" color=\""
             << GRA_colorControl::GetColorCode(st.color_)
             << "\" font=\"" << GRA_fontControl::GetFontCode(st.font_) << "\">\n"
             << "<string><![CDATA[" << st.string_ << "]]></string>\n" << "</simpletext>\n";
}

// end of file
