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
#include "GRA_colorMap.h"
#include "GRA_color.h"
#include "EGraphicsError.h"

void GRA_colorMap::DeleteStuff()
{
  while( !colors_.empty() )
  {
    delete colors_.back();
    colors_.pop_back();
  }
}
  
GRA_color *GRA_colorMap::GetColor( int i ) const
{
  GRA_color *color = i>0 ? colors_[(i-1)%colors_.size()] : 0;
  return color;
}

int GRA_colorMap::GetCode( GRA_color *color ) const
{
  std::size_t size = colors_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    if( colors_[i] == color )return static_cast<int>(i+1);
  }
  return -1; // entered an invalid color pointer, return black
}

int GRA_colorMap::GetCode( int r, int g, int b ) const
{
  std::size_t size = colors_.size();
  GRA_color color(r,g,b);
  for( std::size_t i=0; i<size; ++i )
  {
    if( *colors_[i] == color )return static_cast<int>(i+1);
  }
  return 0; // r, g, b values not found in color map
}

std::ostream &operator<<( std::ostream &out, GRA_colorMap const &cmap )
{
  if( cmap.name_ == wxT("USERDEFINED") )
  {
    int cmapSize = cmap.colors_.size();
    out << "<colormap name=\"USERDEFINED\" size=\"" << cmapSize << "\">\n";
    for( int i=0; i<cmapSize; ++i )out << *cmap.colors_[i];
    out << "</colormap>\n";
  }
  else
  {
    out << "<colormap name=\"" << cmap.name_.mb_str(wxConvUTF8) << "\"/>\n";
  }
  return out;
}

// end of file
