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
#include "GRA_colorCharacteristic.h"
#include "GRA_colorControl.h"

std::ostream &operator<<( std::ostream &out, GRA_colorCharacteristic const &c )
{
  if( c.isAVector_ )
  {
    std::size_t size = c.values_.size();
    out << "<characteristic name=\"" << c.name_.mb_str(wxConvUTF8) << "\""
        << " type=\"COLOR\" shape=\"VECTOR\">\n"
        << "<data size=\"" << size << "\">";
    for( std::size_t i=0; i<size-1; ++i )
      out << GRA_colorControl::GetColorCode(c.values_[i]) << " ";
    out << GRA_colorControl::GetColorCode(c.values_[size-1]) << "</data>\n</characteristic>\n";
  }
  else
  {
    if( c.value_ != c.default_ )
    {
      out << "<characteristic name=\"" << c.name_.mb_str(wxConvUTF8) << "\""
          << " type=\"COLOR\" shape=\"SCALAR\" value=\""
          << GRA_colorControl::GetColorCode(c.value_) << "\"/>\n";
    }
  }
  return out;
}

void GRA_colorCharacteristic::Set( std::vector<int> const &values )
{
  std::vector<GRA_color*>().swap( values_ );
  std::size_t size = values.size();
  for( std::size_t i=0; i<size; ++i )values_.push_back( GRA_colorControl::GetColor(values[i]) );
  isAVector_ = true;
}

void GRA_colorCharacteristic::CopyStuff( GRA_colorCharacteristic const &rhs )
{
  value_ = rhs.value_;
  default_ = rhs.default_;
  values_.assign( rhs.values_.begin(), rhs.values_.end() );
}

// end of file
