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
#ifndef GRA_BOXPLOT
#define GRA_BOXPLOT

#include <ostream>
#include <vector>

#include "wx/wx.h"

#include "GRA_densityPlot.h"

class GRA_wxWidgets;

class GRA_boxPlot : public GRA_densityPlot
{
public:
  GRA_boxPlot( std::vector<double> &x, std::vector<double> &y, std::vector<double> &z,
               int nrow, double fmin, double fmax, double gmin, double gmax,
               bool xProfile, bool yProfile, bool border,
               bool zoom, bool axes, bool reset, double bscale )
    : GRA_densityPlot(wxT("BOXPLOT"), x, y, z, nrow, fmin, fmax, gmin, gmax,
                      xProfile, yProfile, border, zoom, axes, reset, false),
      bscale_(bscale)
  { SetUp(); }

  ~GRA_boxPlot()
  {}

  GRA_boxPlot( GRA_boxPlot const &rhs )
  { CopyStuff(rhs); }

  GRA_boxPlot &operator=( GRA_boxPlot const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }

  void Make();
  void Draw( GRA_wxWidgets *, wxDC & );

  void GetSides( double &xside, double &yside )
  {
    xside = xside_;
    yside = yside_;
  }

  double GetAmin() const
  { return amin_; }
  
  double GetDa() const
  { return da_; }
  
  friend std::ostream &operator<<( std::ostream &, GRA_boxPlot const & );


private:
  void DrawABox( double, double, double, double, double, double, double,
                 GRA_color *, GRA_color *, GRA_wxWidgets *, wxDC & );
  void CopyStuff( GRA_boxPlot const & );

  double amin_, da_, xside_, yside_, bscale_;
};

#endif

