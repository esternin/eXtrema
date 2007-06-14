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
#ifndef GRA_DIFFUSIONPLOT
#define GRA_DIFFUSIONPLOT

#include <ostream>

#include "wx/wx.h"

#include "GRA_densityPlot.h"

class GRA_bitmap;
class GRA_color;
class GRA_wxWidgets;

class GRA_diffusionPlot : public GRA_densityPlot
{
public:
  GRA_diffusionPlot( std::vector<double> &x, std::vector<double> &y, std::vector<double> &z,
                     int nrow, double fmin, double fmax, double gmin, double gmax,
                     bool xProfile, bool yProfile, bool border,
                     bool zoom, bool axes, bool reset )
    : GRA_densityPlot(wxT("DIFFUSIONPLOT"), x, y, z, nrow, fmin, fmax, gmin, gmax,
                      xProfile, yProfile, border, zoom, axes, reset, false )
  {
    if( xProfile_ || yProfile_ )MakeProfiles( 0, x_.size(), 0, y_.size() );
  }

  ~GRA_diffusionPlot()
  {}

  GRA_diffusionPlot( GRA_diffusionPlot const &rhs )
  { CopyStuff(rhs); }

  GRA_diffusionPlot & operator=( GRA_diffusionPlot const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }

  void Draw( GRA_wxWidgets *, wxDC & );
  void FillBitmap( GRA_bitmap * );

  friend std::ostream &operator<<( std::ostream &, GRA_diffusionPlot const & );

private:
  void DrawBitmap( GRA_wxWidgets *, wxDC & );
};
#endif

