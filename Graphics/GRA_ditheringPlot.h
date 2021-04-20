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
#ifndef GRA_DITHERINGPLOT
#define GRA_DITHERINGPLOT

#include "GRA_densityPlot.h"

class GRA_outputType;
class GRA_color;
class GRA_bitmap;

class GRA_ditheringPlot : public GRA_densityPlot
{
public:
  GRA_ditheringPlot( std::vector<double> &x, std::vector<double> &y, std::vector<double> &z,
                     int nrow, double fmin, double fmax, double gmin, double gmax,
                     bool xProfile, bool yProfile, bool border,
                     bool zoom, bool axes, bool reset, bool legend,
                     bool equallySpaced, bool areas, bool volumes, bool lines,
                     std::vector<int> &dither, std::vector<double> &contourLevels )
    : GRA_densityPlot("DITHERINGPLOT", x, y, z, nrow, fmin, fmax, gmin, gmax,
                      xProfile, yProfile, border, zoom, axes, reset, legend ),
      equallySpaced_(equallySpaced), areas_(areas), volumes_(volumes), lines_(lines)
  {
    dither_.assign( dither.begin(), dither.end() );
    contourLevels_.assign( contourLevels.begin(), contourLevels.end() );
    if( xProfile_ || yProfile_ )MakeProfiles( 0, x_.size(), 0, y_.size() );
  }

  ~GRA_ditheringPlot()
  {}

  GRA_ditheringPlot( GRA_ditheringPlot const &rhs )
  { CopyStuff(rhs); }

  GRA_ditheringPlot &operator=( GRA_ditheringPlot const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }

  void Draw( GRA_outputType * );

  friend std::ostream &operator<<( std::ostream &, GRA_ditheringPlot const & );

  std::vector<double> &GetAreas()
  { return areaVec_; }

  std::vector<double> &GetVolumes()
  { return volumeVec_; }

  std::vector<double> &GetContours()
  { return contourVec_; }

private:
  void CopyStuff( GRA_ditheringPlot const & );
  void DrawBitmap( GRA_outputType * );
  void FillBitmap( GRA_bitmap * );
  void DrawLegend( GRA_outputType * );
  void FillLegendBitmap( GRA_bitmap * );

  bool equallySpaced_, areas_, volumes_, lines_;
  std::vector<double> areaVec_, volumeVec_, contourVec_;
  std::vector<double> contourLevels_;
  std::vector<int> dither_;
};

#endif

