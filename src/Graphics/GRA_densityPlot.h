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
#ifndef GRA_DENSITYPLOT
#define GRA_DENSITYPLOT

#include <vector>
#include <ostream>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_color;
class GRA_window;
class GRA_wxWidgets;
class GRA_cartesianAxes;

class GRA_densityPlot : public GRA_drawableObject
{
protected:
  enum DensityType { UNKNOWN, BOX, DIFFUSION, DITHERING, GRADIENT };

public:
  GRA_densityPlot() : GRA_drawableObject(wxT("UNKNOWN"))
  { densityType_ = UNKNOWN; }

  GRA_densityPlot( wxChar const *,
                   std::vector<double> &, std::vector<double> &, std::vector<double> &,
                   int =0, double =0.0, double =0.0, double =0.0, double =0.0,
                   bool =false, bool =false, bool =false, bool =false, bool =true,
                   bool =true, bool =false );

  virtual ~GRA_densityPlot()
  {}

  GRA_densityPlot( GRA_densityPlot const &rhs )
  { CopyStuff(rhs); }

  GRA_densityPlot &operator=( GRA_densityPlot const &rhs )
  {
    if( &rhs != this )CopyStuff(rhs);
    return *this;
  }

  void GetLimits( double &xlo, double &ylo, double &xhi, double &yhi ) const
  { xlo=xlo_; ylo=ylo_; xhi=xhi_; yhi=yhi_; }

  bool IsaUnknown() const
  { return (densityType_==UNKNOWN); }

  bool IsaBoxPlot() const
  { return (densityType_==BOX); }

  bool IsaDiffusionPlot() const
  { return (densityType_==DIFFUSION); }

  bool IsaDitheringPlot() const
  { return (densityType_==DITHERING); }

  bool IsaGradientPlot() const
  { return (densityType_==GRADIENT); }

  void DrawAxes( GRA_wxWidgets *, wxDC &, GRA_cartesianAxes *& );
  void DrawAxes2( GRA_wxWidgets *, wxDC &, GRA_cartesianAxes * );

  bool GetDrawAxes() const
  { return drawAxes_; }
  
  void GetDataLimits( double &xmin, double &ymin, double &xmax, double &ymax )
  {
    xmin = dataXmin_;
    ymin = dataYmin_;
    xmax = dataXmax_;
    ymax = dataYmax_;
  }
  
  std::size_t GetNRow() const
  { return nrow_; }

  void GetMNLimits( int &mlo, int &nlo, int &mhi, int &nhi ) const
  {
    mlo = mlo_;
    nlo = nlo_;
    mhi = mhi_;
    nhi = nhi_;
  }

  void GetXYZ( std::vector<double> &x, std::vector<double> &y, std::vector<double> &z ) const
  {
    x = x_;
    y = y_;
    z = z_;
  }

  void GetFminmax( double &fmin, double &fmax ) const
  {
    fmin = fmin_;
    fmax = fmax_;
  }

  bool GetBorder() const
  { return border_; }

  void GetUpperAxis( double &xuaxisp, double &yuaxisp ) const
  {
    xuaxisp = xuaxisp_;
    yuaxisp = yuaxisp_;
  }

  void GetProfiles( bool &x, bool &y ) const
  {
    x = xProfile_;
    y = yProfile_;
  }
  
  void GetProfileVecs( std::vector<double> *&xX, std::vector<double> *&xY,
                       std::vector<double> *&yX, std::vector<double> *&yY )
  {
    xX = &xProfileX_;
    xY = &xProfileY_;
    yX = &yProfileX_;
    yY = &yProfileY_;
  }
  
  bool GetResetAxes() const
  { return resetAxes_; }
  
  void GetXYLimits( double &xlo, double &ylo, double &xhi, double &yhi ) const
  {
    xlo = xlo_;
    ylo = ylo_;
    xhi = xhi_;
    yhi = yhi_;
  }
  
  void SetAlohi( double alo, double ahi )
  {
    alo_ = alo;
    ahi_ = ahi;
  }
  
  bool GetLegend() const
  { return legend_; }
  
  void SetUp();

protected:
  void CopyStuff( GRA_densityPlot const & );
  void MakeProfiles( int, int, int, int );
  void DrawProfiles( GRA_wxWidgets *, wxDC & );

  DensityType densityType_;
  double dataXmin_, dataXmax_, dataYmin_, dataYmax_;
  double alo_, ahi_, fmin_, fmax_, gmin_, gmax_;
  int mlo_, mhi_, nlo_, nhi_;
  double rmlo_, rmhi_, rnlo_, rnhi_;
  std::size_t nrow_;
  std::vector<double> x_, y_, z_, xProfileX_, xProfileY_, yProfileX_, yProfileY_;
  double xlo_, ylo_, xhi_, yhi_, xuaxisp_, yuaxisp_;
  bool border_, xProfile_, yProfile_, zoom_, drawAxes_, resetAxes_, legend_;
};
 
#endif
 
