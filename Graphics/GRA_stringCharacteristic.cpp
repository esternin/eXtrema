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

#include "GRA_stringCharacteristic.h"

std::ostream &operator<<( std::ostream &out, GRA_stringCharacteristic const &sc )
{
  if( sc.isAVector_ )
  {
    std::size_t size = sc.values_.size();
    out << "<characteristic name=\"" << sc.name_ << "\""
        << " type=\"STRING\" shape=\"VECTOR\">\n"
        << "<data size=\"" << size << "\">\n";
    for( std::size_t i=0; i<size; ++i )out << sc.values_[i].Encode() << "\n";
    out << "</data>\n</characteristic>\n";
  }
  else
  {
    if( sc.value_ != sc.default_ )
    {
      out << "<characteristic name=\"" << sc.name_ << "\""
          << " type=\"STRING\" shape=\"SCALAR\" value=\""
          << sc.value_.Encode() << "\"/>\n";
    }
  }
  return out;
}

void GRA_stringCharacteristic::CopyStuff( GRA_stringCharacteristic const &rhs )
{
  value_ = rhs.value_;
  default_ = rhs.default_;
  values_.assign( rhs.values_.begin(), rhs.values_.end() );
}

// end of file
