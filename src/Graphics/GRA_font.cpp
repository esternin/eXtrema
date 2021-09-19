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
#include "wx/fontenum.h"

#include "GRA_font.h"

namespace
{

// Return the font info using the available variant of the given portable font
// name which must be one of "SANS", "SERIF" and "MONO".
wxFontInfo GetAvailableVariantOf(wxString const& name)
{
  static wxArrayString const allFacenames = wxFontEnumerator::GetFacenames();

  wxFontInfo info(12);

  if ( allFacenames.Index(name, false /* case-insensitive */) != wxNOT_FOUND )
  {
    // This font is available, just use it directly.
    return info.FaceName(name);
  }

  // Find an available alternative: start by the fonts that we install with
  // Extrema that should be always available.
  wxString nameAlt = "Free" + name;
  if ( allFacenames.Index(nameAlt, false /* case-insensitive */) != wxNOT_FOUND )
    return info.FaceName(nameAlt);

  // If they aren't, just fall back to the corresponding default font of the
  // given family for the current platform.
  wxFontFamily family;
  if ( name == "SANS" )
    family = wxFONTFAMILY_SWISS;
  else if ( name == "SERIF" )
    family = wxFONTFAMILY_ROMAN;
  else if ( name == "MONO" )
    family = wxFONTFAMILY_TELETYPE;
  else
  {
    wxFAIL_MSG("Unknown portable font name \"" + name + "\"");
    return info;
  }

  return info.Family(family);
}

} // anonymous namespace

GRA_font::GRA_font( wxString const &name ) 
    : wxfont_(GetAvailableVariantOf(name)),
      name_( name )
{
}

std::ostream &operator<<( std::ostream &out, GRA_font const &font )
{
  return out << "<font info=\"" << font.wxfont_.GetNativeFontInfoDesc().mb_str(wxConvUTF8) << "\"/>\n";
}

// end of file
