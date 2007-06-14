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

#include "wx/wx.h"

#include "GRA_densityPlot.h"

class GRA_colorMap;
class GRA_bitmap;
class GRA_wxWidgets;

class GRA_gradientPlot : public GRA_densityPlot
{
public:
  GRA_gradientPlot( std::vector<double> &, std::vector<double> &, std::vector<double> &,
                    int, double, double, double, double, bool, bool, bool, bool, bool,
                    bool, bool, bool );

  ~GRA_gradientPlot()
  {}

  GRA_gradientPlot( GRA_gradientPlot const &rhs )
  { CopyStuff(rhs); }

  GRA_gradientPlot & operator=( GRA_gradientPlot const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }

  void Draw( GRA_wxWidgets *, wxDC & );

  void FillBitmap( GRA_bitmap * );
  void FillLegendBitmap( GRA_bitmap * );

  GRA_colorMap *GetColorMap() const
  { return colorMap_; }
  
  friend std::ostream &operator<<( std::ostream &, GRA_gradientPlot const & );

private:
  void CopyStuff( GRA_gradientPlot const &rhs )
  {
    GRA_densityPlot::CopyStuff( rhs );
    linear_ = rhs.linear_;
  }

  void DrawBitmap( GRA_wxWidgets *, wxDC & );
  void DrawLegend( GRA_wxWidgets *, wxDC & );

  bool linear_;
  GRA_colorMap *colorMap_; 
};
 
#endif
 
