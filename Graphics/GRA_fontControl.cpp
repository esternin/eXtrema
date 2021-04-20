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

#include "GRA_fontControl.h"
#include "EGraphicsError.h"

namespace GRA_fontControl
{
  std::vector<GRA_font*> fonts_;
  std::map<ExString,ExString> fontNames_;
  std::map<ExString,ExString> psFontNames_;
  ExString fontDir_;

void Initialize()
{
  fonts_.push_back( new GRA_font("ARIAL") );
  fonts_.push_back( new GRA_font("ARIAL BLACK") );
  fonts_.push_back( new GRA_font("ARIAL NARROW") );
  //fonts_.push_back( new GRA_font("BATANG") );
  fonts_.push_back( new GRA_font("BOOK ANTIQUA") );
  fonts_.push_back( new GRA_font("BOOKMAN OLD STYLE") );
  fonts_.push_back( new GRA_font("CENTURY") );
  fonts_.push_back( new GRA_font("COURIER NEW") );
  fonts_.push_back( new GRA_font("GARAMOND") );
  fonts_.push_back( new GRA_font("SYMBOL") );
  fonts_.push_back( new GRA_font("TIMES NEW ROMAN") );
  fonts_.push_back( new GRA_font("WINGDINGS") );
  //
  char tmp[MAX_PATH];
  unsigned int j = GetWindowsDirectory( tmp, MAX_PATH );
  if( !j )throw EGraphicsError(
                 "unable to get windows directory to find font files" );
  fontDir_ = ExString(tmp)+"/Fonts/";
  //
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("ARIAL"),fontDir_+"ARIAL.TTF") );
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("ARIAL BLACK"),fontDir_+"ARIBLK.TTF") );
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("ARIAL NARROW"),fontDir_+"ARIALN.TTF") );
  //fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("BATANG"),fontDir_+"BATANG.TTF") );
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("BOOK ANTIQUA"),fontDir_+"BKANT.TTF") );
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("BOOKMAN OLD STYLE"),fontDir_+"BOOKOS.TTF") );
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("CENTURY"),fontDir_+"CENTURY.TTF") );
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("COURIER NEW"),fontDir_+"COUR.TTF") );
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("GARAMOND"),fontDir_+"GARA.TTF") );
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("SYMBOL"),fontDir_+"SYMBOL.TTF") );
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("TIMES NEW ROMAN"),fontDir_+"TIMES.TTF") );
  fontNames_.insert( std::map<ExString,ExString>::value_type(ExString("WINGDINGS"),fontDir_+"WINGDING.TTF") );
  //
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("ARIAL"),ExString("ArialMT")) );
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("ARIAL BLACK"),ExString("Arial-Black")) );
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("ARIAL NARROW"),ExString("ArialNarrow")) );
  //psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("BATANG"),ExString("Batang")) );
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("BOOK ANTIQUA"),ExString("BookAntiqua")) );
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("BOOKMAN OLD STYLE"),ExString("BookmanOldStyle")) );
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("CENTURY"),ExString("Century")) );
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("COURIER NEW"),ExString("CourierNewPSMT")) );
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("GARAMOND"),ExString("Garamond")) );
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("SYMBOL"),ExString("SymbolMT")) );
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("TIMES NEW ROMAN"),ExString("TimesNewRomanPSMT")) );
  psFontNames_.insert( std::map<ExString,ExString>::value_type(ExString("WINGDINGS"),ExString("Wingdings-Regular")) );
}

void DeleteStuff()
{
  while( !fonts_.empty() )
  {
    delete fonts_.back();
    fonts_.pop_back();
  }
}

int GetFontCode( ExString const &name )
{
  std::size_t size = fonts_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    if( name.UpperCase() == fonts_[i]->GetName() )return static_cast<int>(i);
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
    if( fonts_[i] == font )
    {
      code = static_cast<int>(i);
      break;
    }
  }
  return code;
}

GRA_font *GetFont( ExString const &name )
{
  std::size_t size = fonts_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    if( name.UpperCase() == fonts_[i]->GetName() )return fonts_[i];
  }
  return fonts_[0];
}
} // end of namespace

// end of file
