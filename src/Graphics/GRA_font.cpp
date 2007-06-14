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
#include "wx/fontutil.h"

#include "GRA_font.h"

GRA_font::GRA_font( wxString const &name ) 
    : name_( name.Upper() )
{
  if( name_ == wxT("DECORATIVE") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("DECORATIVEBOLD") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("DECORATIVEBOLDITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("DECORATIVEITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_DECORATIVE, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("ROMAN") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("ROMANBOLD") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("ROMANBOLDITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("ROMANITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("SCRIPT") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("SCRIPTBOLD") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("SCRIPTBOLDITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("SCRIPTITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_SCRIPT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("SWISS") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("SWISSBOLD") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("SWISSBOLDITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("SWISSITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("MODERN") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("MODERNBOLD") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("MODERNBOLDITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_MODERN, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("MODERNITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_MODERN, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("TELETYPE") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("TELETYPEBOLD") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("TELETYPEBOLDITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("TELETYPEITALIC") )
  {
    wxfont_ = wxFont( 12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("ARIAL") )
  {
    //neither of the following works to set the font
    //wxfont_ = wxFont( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, name );
    //wxfont_.SetFaceName( name );
    //
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("ARIAL") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("ARIALBLACK") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("ARIAL BLACK") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("ARIALBOLD") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("ARIAL") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("ARIALBOLDITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("ARIAL") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("ARIALITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("ARIAL") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("BLACKCHANCERY") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("BLACKCHANCERY") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("COMICSANSMS") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("COMIC SANS MS") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("COMICSANSMSBOLD") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("COMIC SANS MS") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("COURIERNEW") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("COURIER NEW") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("COURIERNEWBOLD") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("COURIER NEW") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("COURIERNEWBOLDITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("COURIER NEW") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("COURIERNEWITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("COURIER NEW") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("GEORGIA") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("GEORGIA") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("GEORGIABOLD") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("GEORGIA") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("GEORGIABOLDITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("GEORGIA") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("GEORGIAITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("GEORGIA") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("IMPACT") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("IMPACT") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("MONOTYPE") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("MONOTYPE.COM") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("SYMBOL") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("SYMBOL") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("TIMESNEWROMAN") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("TIMES NEW ROMAN") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("TIMESNEWROMANBOLD") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("TIMES NEW ROMAN") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("TIMESNEWROMANBOLDITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("TIMES NEW ROMAN") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("TIMESNEWROMANITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("TIMES NEW ROMAN") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("TREBUCHETMS") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("TREBUCHET MS") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("TREBUCHETMSBOLD") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("TREBUCHET MS") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("TREBUCHETMSBOLDITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("TREBUCHET MS") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("TREBUCHETMSITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("TREBUCHET MS") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("VERDANA") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("VERDANA") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("VERDANABOLD") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("VERDANA") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("VERDANABOLDITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("VERDANA") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_BOLD );
  }
  else if( name_ == wxT("VERDANAITALIC") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("VERDANA") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_ITALIC );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("WEBDINGS") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("WEBDINGS") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else if( name_ == wxT("WINGDINGS") )
  {
    wxNativeFontInfo nfi;
    nfi.FromString( wxT("WINGDINGS") );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  else
  {
    wxNativeFontInfo nfi;
    nfi.FromString( name_ );
    wxfont_ = wxFont( nfi );
    wxfont_.SetFamily( wxFONTFAMILY_DEFAULT );
    wxfont_.SetStyle( wxFONTSTYLE_NORMAL );
    wxfont_.SetWeight( wxFONTWEIGHT_NORMAL );
  }
  wxfont_.SetPointSize( 12 );  // 12 seems to fit the most number of fonts (using xfontsel)
}

std::ostream &operator<<( std::ostream &out, GRA_font const &font )
{
  return out << wxT("<font info=\"") << font.wxfont_.GetNativeFontInfoDesc().c_str() << wxT("\"/>\n");
}

// end of file
