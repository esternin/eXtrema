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
#ifndef GRA_COLORCONTROL
#define GRA_COLORCONTROL

#include <vector>

#include "ExString.h"
#include "GRA_color.h"
#include "GRA_colorMap.h"

namespace GRA_colorControl
{
  extern GRA_colorMap *current_;
  extern GRA_colorMap *userdefined_;
  extern GRA_colorMap *thermal_, *gray256_, *gray16_, *rainbow_, *redblue16_;
  extern GRA_colorMap *classic8_, *topo256_, *default_;
  extern std::vector<GRA_color*> namedColors_;

  void Initialize();

  void CreateThermalColorMap();
  void CreateGray256ColorMap();
  void CreateGray16ColorMap();
  void CreateRainbowColorMap();
  void CreateRedBlue16ColorMap();
  void CreateClassic8ColorMap();
  void CreateTopo256ColorMap();
  void CreateDefaultColorMap();

  void DeleteStuff();

  ExString GetColorMapName()
  { return current_->GetName(); }

  GRA_colorMap *GetColorMap()
  { return current_; }

  void SetFile( ExString const & );
  void SetColorMap( ExString const & );

  void SetColorMapUser( GRA_colorMap *colorMap )
  {
    delete userdefined_;
    userdefined_ = colorMap;
    current_ = userdefined_;
  }

  void SetColorMap( GRA_colorMap *colorMap )
  { current_ = colorMap; }

  int GetNumberOfNamedColors()
  { return namedColors_.size(); }

  int GetColorCode( ExString const & );
  int GetColorCode( GRA_color * );
  int GetColorCode( int, int, int );

  GRA_color *GetColor( ExString const & );

  GRA_color *GetColor( int i )
  {
    // if i <= 0, color is a named color
    // if i > 0, color is not named
    // no need to check the size of i, since the code number will wrap
    //
    if( i > 0 )return current_->GetColor(i);
    return namedColors_[abs(i)%namedColors_.size()];
  }

  int GetColorMapSize()
  { return current_->GetSize(); }

  void ColorCodeToRGB( int code, int &r, int &g, int &b )
  {
    GRA_color *color = GetColor( code );
    color->GetRGB(r,g,b);
  }

  void CreateNamedColors();
};

#endif
