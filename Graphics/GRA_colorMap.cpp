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

#include "EGraphicsError.h"
#include "GRA_colorMap.h"

void GRA_colorMap::DeleteStuff()
{
  while( !colors_.empty() )
  {
    delete colors_.back();
    colors_.pop_back();
  }
}

int GRA_colorMap::GetCode( GRA_color *color ) const
{
  std::size_t size = colors_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    if( colors_[i] == color )return static_cast<int>(i+1);
  }
  throw EGraphicsError( "invalid color pointer" );
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
  if( cmap.name_ == "USERDEFINED" )
  {
    int cmapSize = cmap.colors_.size();
    out << "<colormap name=\"USERDEFINED\" size=\"" << cmapSize << "\">\n";
    for( int i=0; i<cmapSize; ++i )out << *cmap.colors_[i];
    out << "</colormap>\n";
  }
  else
  {
    out << "<colormap name=\"" << cmap.name_ << "\"/>\n";
  }
  return out;
}

// end of file
