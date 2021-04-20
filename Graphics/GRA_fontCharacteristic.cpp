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

#include "GRA_font.h"
#include "GRA_fontControl.h"

#include "GRA_fontCharacteristic.h"

void GRA_fontCharacteristic::Set( int value )
{
  value_ = GRA_fontControl::GetFont(value);
  isAVector_ = false;
}

void GRA_fontCharacteristic::Set( std::vector<int> const &values )
{
  std::vector<GRA_font*>().swap( values_ );
  std::size_t size = values.size();
  for( std::size_t i=0; i<size; ++i )values_.push_back( GRA_fontControl::GetFont(values[i]) );
  isAVector_ = true;
}

std::ostream &operator<<( std::ostream &out, GRA_fontCharacteristic const &c )
{
  if( c.isAVector_ )
  {
    std::size_t size = c.values_.size();
    out << "<characteristic name=\"" << c.name_ << "\""
        << " type=\"FONT\" shape=\"VECTOR\">\n"
        << "<data size=\"" << size << "\">";
    for( std::size_t i=0; i<size-1; ++i )out << GRA_fontControl::GetFontCode(c.values_[i]) << " ";
    out << GRA_fontControl::GetFontCode(c.values_[size-1]) << "</data>\n</characteristic>\n";
  }
  else
  {
    if( c.value_ != c.default_ )
    {
      out << "<characteristic name=\"" << c.name_ << "\""
          << " type=\"FONT\" shape=\"SCALAR\" value=\"" << GRA_fontControl::GetFontCode(c.value_) << "\"/>\n";
    }
  }
  return out;
}

void GRA_fontCharacteristic::CopyStuff( GRA_fontCharacteristic const &rhs )
{
  value_ = rhs.value_;
  default_ = rhs.default_;
  values_.assign( rhs.values_.begin(), rhs.values_.end() );
}

// end of file
