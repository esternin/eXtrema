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
#include "EGraphicsError.h"

namespace GRA_fontControl
{
  std::vector<GRA_font*> fonts_;
  std::map<wxString,wxString> psFontNames_;

  void Initialize()
  {
    fonts_.push_back( new GRA_font(wxT("ARIAL")) );
    fonts_.push_back( new GRA_font(wxT("ARIAL BLACK")) );
    fonts_.push_back( new GRA_font(wxT("ARIAL NARROW")) );
    fonts_.push_back( new GRA_font(wxT("BOOK ANTIQUA")) );
    fonts_.push_back( new GRA_font(wxT("BOOKMAN OLD STYLE")) );
    fonts_.push_back( new GRA_font(wxT("CENTURY")) );
    fonts_.push_back( new GRA_font(wxT("COURIER NEW")) );
    fonts_.push_back( new GRA_font(wxT("GEORGIA")) );
    fonts_.push_back( new GRA_font(wxT("SYMBOL")) );
    fonts_.push_back( new GRA_font(wxT("TIMES NEW ROMAN")) );
    fonts_.push_back( new GRA_font(wxT("VERDANA")) );
    fonts_.push_back( new GRA_font(wxT("ZAPFDINGBATS")) );

    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("ARIAL")),wxString(wxT("ArialMT"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("ARIAL BLACK")),wxString(wxT("Arial-Black"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("ARIAL NARROW")),wxString(wxT("ArialNarrow"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("BOOK ANTIQUA")),wxString(wxT("BookAntiqua"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("BOOKMAN OLD STYLE")),wxString(wxT("BookmanOldStyle"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("CENTURY")),wxString(wxT("Century"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("COURIER NEW")),wxString(wxT("CourierNewPSMT"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("GEORGIA")),wxString(wxT("Georgia"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("SYMBOL")),wxString(wxT("Symbol"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("TIMES NEW ROMAN")),wxString(wxT("TimesNewRomanPSMT"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("VERDANA")),wxString(wxT("Verdana"))) );
    psFontNames_.insert( std::map<wxString,wxString>::value_type(wxString(wxT("ZAPFDINGBATS")),wxString(wxT("ZapfDingbats"))) );
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

  wxString GetPostScriptFontName( wxString const &name )
  {
    std::map<wxString,wxString>::iterator i = psFontNames_.find( name );
    if( i == psFontNames_.end() )
      throw EGraphicsError( wxString()<<wxT("unable to find PostScript font: ")<<name );
    return (*i).second;
  }
  
} // end of GRA_fontControl namespace

// end of file
