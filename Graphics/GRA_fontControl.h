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
#ifndef GRA_FONTCONTROL
#define GRA_FONTCONTROL

#include <map>

#include "EGraphicsError.h"
#include "ExString.h"
#include "GRA_font.h"

namespace GRA_fontControl
{
  extern std::vector<GRA_font*> fonts_;
  extern std::map<ExString,ExString> fontNames_;
  extern ExString fontDir_;
  extern std::map<ExString,ExString> psFontNames_;
  
  void Initialize();

  void DeleteStuff();

  ExString GetFontPath( ExString const &name )
  {
    std::map<ExString,ExString>::iterator i = fontNames_.find( name );
    if( i == fontNames_.end() )
      throw EGraphicsError( ExString("unable to find font: ")+name );
    return (*i).second;
  }

  ExString GetPostScriptFontName( ExString const &name )
  {
    std::map<ExString,ExString>::iterator i = psFontNames_.find( name );
    if( i == psFontNames_.end() )
      throw EGraphicsError( ExString("unable to find PostScript font: ")+name );
    return (*i).second;
  }

  int GetFontCode( ExString const & );
  int GetFontCode( GRA_font * );

  GRA_font *GetFont( ExString const & );
  
  GRA_font *GetFont( int i )
  { return fonts_[abs(i)%fonts_.size()]; }
  
  int GetCount()
  { return fonts_.size(); }
};

#endif
