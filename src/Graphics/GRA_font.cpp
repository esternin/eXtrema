/*
Copyright (C) 2005,...,2008 Joseph L. Chuma, TRIUMF

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
#include "wx/fontutil.h"

#include "GRA_font.h"

GRA_font::GRA_font( wxString const &name ) 
    : wxfont_(wxFontInfo(12).FaceName(name)),
      name_( name )
{
}

std::ostream &operator<<( std::ostream &out, GRA_font const &font )
{
  return out << "<font info=\"" << font.wxfont_.GetNativeFontInfoDesc().mb_str(wxConvUTF8) << "\"/>\n";
}

// end of file
