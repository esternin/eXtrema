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
#include "GRA_fontControl.h"
#include "GRA_font.h"

namespace GRA_fontControl
{
  std::vector<GRA_font*> fonts_;

  void Initialize()
  {
    fonts_.push_back( new GRA_font(wxT("DECORATIVE")) );
    fonts_.push_back( new GRA_font(wxT("DECORATIVEBOLD")) );
    fonts_.push_back( new GRA_font(wxT("DECORATIVEBOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("DECORATIVEITALIC")) );
    fonts_.push_back( new GRA_font(wxT("ROMAN")) );
    fonts_.push_back( new GRA_font(wxT("ROMANBOLD")) );
    fonts_.push_back( new GRA_font(wxT("ROMANBOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("ROMANITALIC")) );
    fonts_.push_back( new GRA_font(wxT("SCRIPT")) );
    fonts_.push_back( new GRA_font(wxT("SCRIPTBOLD")) );
    fonts_.push_back( new GRA_font(wxT("SCRIPTBOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("SCRIPTITALIC")) );
    fonts_.push_back( new GRA_font(wxT("SWISS")) );
    fonts_.push_back( new GRA_font(wxT("SWISSBOLD")) );
    fonts_.push_back( new GRA_font(wxT("SWISSBOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("SWISSITALIC")) );
    fonts_.push_back( new GRA_font(wxT("MODERN")) );
    fonts_.push_back( new GRA_font(wxT("MODERNBOLD")) );
    fonts_.push_back( new GRA_font(wxT("MODERNBOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("MODERNITALIC")) );
    fonts_.push_back( new GRA_font(wxT("TELETYPE")) );
    fonts_.push_back( new GRA_font(wxT("TELETYPEBOLD")) );
    fonts_.push_back( new GRA_font(wxT("TELETYPEBOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("TELETYPEITALIC")) );
    fonts_.push_back( new GRA_font(wxT("ARIAL")) );
    fonts_.push_back( new GRA_font(wxT("ARIALBLACK")) );
    fonts_.push_back( new GRA_font(wxT("ARIALBOLD")) );
    fonts_.push_back( new GRA_font(wxT("ARIALBOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("ARIALITALIC")) );
    fonts_.push_back( new GRA_font(wxT("BLACKCHANCERY")) );
    fonts_.push_back( new GRA_font(wxT("COMICSANSMS")) );
    fonts_.push_back( new GRA_font(wxT("COMICSANSMSBOLD")) );
    fonts_.push_back( new GRA_font(wxT("COURIERNEW")) );
    fonts_.push_back( new GRA_font(wxT("COURIERNEWBOLD")) );
    fonts_.push_back( new GRA_font(wxT("COURIERNEWBOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("COURIERNEWITALIC")) );
    fonts_.push_back( new GRA_font(wxT("GEORGIA")) );
    fonts_.push_back( new GRA_font(wxT("GEORGIABOLD")) );
    fonts_.push_back( new GRA_font(wxT("GEORGIABOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("GEORGIAITALIC")) );
    fonts_.push_back( new GRA_font(wxT("IMPACT")) );
    fonts_.push_back( new GRA_font(wxT("MONOTYPE")) );
    fonts_.push_back( new GRA_font(wxT("SYMBOL")) );
    fonts_.push_back( new GRA_font(wxT("TIMESNEWROMAN")) );
    fonts_.push_back( new GRA_font(wxT("TIMESNEWROMANBOLD")) );
    fonts_.push_back( new GRA_font(wxT("TIMESNEWROMANBOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("TIMESNEWROMANITALIC")) );
    fonts_.push_back( new GRA_font(wxT("TREBUCHETMS")) );
    fonts_.push_back( new GRA_font(wxT("TREBUCHETMSBOLD")) );
    fonts_.push_back( new GRA_font(wxT("TREBUCHETMSBOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("TREBUCHETMSITALIC")) );
    fonts_.push_back( new GRA_font(wxT("VERDANA")) );
    fonts_.push_back( new GRA_font(wxT("VERDANABOLD")) );
    fonts_.push_back( new GRA_font(wxT("VERDANABOLDITALIC")) );
    fonts_.push_back( new GRA_font(wxT("VERDANAITALIC")) );
    fonts_.push_back( new GRA_font(wxT("WEBDINGS")) );
    fonts_.push_back( new GRA_font(wxT("WINGDINGS")) );
  }

  void DeleteStuff()
  {
    while( !fonts_.empty() )
    {
      delete fonts_.back();
      fonts_.pop_back();
    }
  }

  int GetFontCode( wxString const &nameIn )
  {
    std::size_t size = fonts_.size();
    wxString name( nameIn.Upper() );
    for( std::size_t i=0; i<size; ++i )
    {
      if( name == fonts_[i]->GetFontName() )return static_cast<int>(i);
    }
    return 0;
  }

  int GetFontCode( GRA_font *font )
  {
    int code = 0;
    if( !font )return code;
    std::size_t size = fonts_.size();
    for( std::size_t i=0; i<size; ++i )
    {
      if( *fonts_[i] == *font )
      {
        code = static_cast<int>(i);
        break;
      }
    }
    return code;
  }

  GRA_font *GetFont( wxString const &nameIn )
  { return fonts_[ GetFontCode(nameIn) ]; }

  GRA_font *GetFont( int i )
  { return fonts_[abs(i)%fonts_.size()]; }

  int GetCount()
  { return fonts_.size(); }
  
} // end of GRA_fontControl namespace

// end of file
