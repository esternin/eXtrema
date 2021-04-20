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
#ifndef GRA_THREEDFIGURE
#define GRA_THREEDFIGURE

#include <vector>
#include <ostream>

#include "GRA_drawableObject.h"

class GRA_threeDFigure : public GRA_drawableObject
{
public:
  GRA_threeDFigure( std::vector<double> const &, std::size_t, std::size_t,
                    double, double, double, double, bool, bool );

  ~GRA_threeDFigure()
  {}

  GRA_threeDFigure( GRA_threeDFigure const &rhs )
  { CopyStuff(rhs); }

  GRA_threeDFigure &operator=( GRA_threeDFigure const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }

  void GRA_threeDFigure::CopyStuff( GRA_threeDFigure const &rhs )
  {}

  void Draw();

  friend std::ostream &operator<<( std::ostream &, GRA_threeDFigure const & );

private:
  void Plot3D( std::vector<double> &, std::vector<double> &, std::vector<double> &,
               double const, double const,
               std::size_t const, std::size_t const,
               std::vector<int> &, std::vector<double> & );

  void iPlot( int const, int const, int const );

  void Hide( std::vector<double> &, std::vector<double> &, std::vector<double> &,
             double const, double const,
             std::size_t const, std::size_t const,
             std::vector<int> &, double const, double const );

  void Framer( std::vector<int> &, std::vector<double> & );

  std::vector<int> matrix_;
  std::size_t nRow_, nCol_;
  double aMin_, aMax_, aInc_;
  //
  double theta_, phi_;
  double verticalScaleFactor_, pedestalSizeScaleFactor_;
  //
  bool color_;
  bool histogram_;
  //
  double a11, a12, a13, a21, a22, a23, a31, a32, a33;
  std::size_t pipi, nypi, limitx;
  bool storeVertices, indx, indy, indz;
  double yDatanew;
  double xRef_, yRef_, yScale_;
  double xLength_, yLength_;
  //
  bool flat_;
  double aLev_[10];
  double xF_, yF_;
  double xLwind_, yLwind_, xUwind_, yUwind_;
  int xOld_, yOld_, penOld_;
};

#endif



