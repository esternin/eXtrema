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
#ifndef GRA_SURFACEPLOT
#define GRA_SURFACEPLOT

#include <vector>
#include <ostream>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_wxWidgets;

class GRA_surfacePlot : public GRA_drawableObject
{
public:
  GRA_surfacePlot( std::vector<double> &, std::vector<double> &, std::vector<double> &,
                   std::size_t, double, double, bool, bool, bool, bool );

  virtual ~GRA_surfacePlot()
  {}

  GRA_surfacePlot( GRA_surfacePlot const &rhs )
  { CopyStuff(rhs); }

  GRA_surfacePlot &operator=( GRA_surfacePlot const &rhs )
  {
    if( &rhs != this )CopyStuff(rhs);
    return *this;
  }

  void Draw( GRA_wxWidgets *, wxDC & );

protected:
  void CopyStuff( GRA_surfacePlot const & );

  std::size_t nRow_, nCol_;
  std::vector<double> x_, y_, z_;
  bool colour_, contours_, axes_, sides_;
  double azimuth_, altitude_;
};
 
#endif
 
