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

#include "wx/wx.h"

class GRA_color;
class GRA_colorMap;

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

  wxString GetColorMapName();
  GRA_colorMap *GetColorMap();
  GRA_colorMap *GetColorMap( wxString const & );

  void SetFile( wxString const & );
  void SetColorMap( wxString const & );

  void SetColorMapUser( GRA_colorMap * );
  void SetColorMap( GRA_colorMap * );
  int GetNumberOfNamedColors();

  int GetColorCode( wxString const & );
  int GetColorCode( GRA_color * );
  int GetColorCode( int, int, int );

  GRA_color *GetColor( wxString const & );
  GRA_color *GetColor( int );

  int GetColorMapSize();

  void ColorCodeToRGB( int, int &, int &, int & );

  void CreateNamedColors();
};

#endif
