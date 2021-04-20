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
#ifndef GRA_GRADIENTPLOT
#define GRA_GRADIENTPLOT

#include "GRA_bitmap.h"
#include "GRA_colorMap.h"
#include "GRA_colorControl.h"
#include "GRA_densityPlot.h"

class GRA_outputType;

class GRA_gradientPlot : public GRA_densityPlot
{
public:
  GRA_gradientPlot( std::vector<double> &x, std::vector<double> &y, std::vector<double> &z,
                   int nrow, double fmin, double fmax, double gmin, double gmax,
                   bool xProfile, bool yProfile, bool border, bool zoom, bool axes,
                   bool reset, bool legend, bool linear )
    : GRA_densityPlot("GRADIENTPLOT", x, y, z, nrow, fmin, fmax, gmin, gmax,
                      xProfile, yProfile, border, zoom, axes, reset, legend ),
      linear_(linear)
  {
    if( xProfile_ || yProfile_ )MakeProfiles( 0, x_.size(), 0, y_.size() );
    colorMap_ = GRA_colorControl::GetColorMap();
  }

  ~GRA_gradientPlot()
  {}

  GRA_gradientPlot( GRA_gradientPlot const &rhs )
  { CopyStuff(rhs); }

  GRA_gradientPlot & operator=( GRA_gradientPlot const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }

  void Draw( GRA_outputType * );

  friend std::ostream &operator<<( std::ostream &, GRA_gradientPlot const & );

private:
  void CopyStuff( GRA_gradientPlot const &rhs )
  {
    GRA_densityPlot::CopyStuff( rhs );
    linear_ = rhs.linear_;
  }

  void DrawBitmap( GRA_outputType * );
  void FillBitmap( GRA_bitmap * );
  void DrawLegend( GRA_outputType * );
  void FillLegendBitmap( GRA_bitmap * );

  bool linear_;
  GRA_colorMap *colorMap_; 
};
 
#endif
 
