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
#include "GRA_stringCharacteristic.h"

std::ostream &operator<<( std::ostream &out, GRA_stringCharacteristic const &sc )
{
  if( sc.isAVector_ )
  {
    std::size_t size = sc.values_.size();
    out << "<characteristic name=\"" << sc.name_.mb_str(wxConvUTF8)
        << "\" type=\"STRING\" shape=\"VECTOR\">\n"
        << "<data size=\"" << size << "\">";
    for( std::size_t i=0; i<size; ++i )
      out << GRA_stringCharacteristic::Encode(sc.values_[i]).mb_str(wxConvUTF8) << "\n";
    out << "</data>\n</characteristic>\n";
  }
  else
  {
    if( sc.value_ != sc.default_ )
    {
      out << "<characteristic name=\"" << sc.name_.mb_str(wxConvUTF8)
          << "\" type=\"STRING\" shape=\"SCALAR\" value=\""
          << GRA_stringCharacteristic::Encode(sc.value_).mb_str(wxConvUTF8)
          << "\"/>\n";
    }
  }
  return out;
}

wxString GRA_stringCharacteristic::Encode( wxString const &s )
{
  wxString result;
  std::size_t length = s.size();
  for( std::size_t i=0; i<length; ++i )
  {
    switch ( s[i] )
    {
      case wxT('<'):
        result += wxT("&lt;");
        break;
      case wxT('>'):
        result += wxT("&gt;");
        break;
      case wxT('&'):
        result += wxT("&amp;");
        break;
      case wxT('\"'):
        result += wxT("&quot;");
        break;
      case wxT('\''):
        result += wxT("&apos;");
        break;
      default:
        result += s[i];
    }
  }
  return result;
}

void GRA_stringCharacteristic::CopyStuff( GRA_stringCharacteristic const &rhs )
{
  value_ = rhs.value_;
  default_ = rhs.default_;
  values_.assign( rhs.values_.begin(), rhs.values_.end() );
}

// end of file
