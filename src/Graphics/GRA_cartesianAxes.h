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
#ifndef GRA_CARTESIANAXES
#define GRA_CARTESIANAXES

#include <vector>
#include <ostream>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_window;
class GRA_axis;
class GRA_wxWidgets;
class GRA_setOfCharacteristics;

class GRA_cartesianAxes : public GRA_drawableObject
{
public:
  GRA_cartesianAxes()
    : GRA_drawableObject(wxT("CARTESIANAXES")), xAxis_(0), yAxis_(0),
      boxBottomAxis_(0), boxTopAxis_(0), boxRightAxis_(0), boxLeftAxis_(0)
  {}

  GRA_cartesianAxes( std::vector<double> const &, std::vector<double> const &, bool, bool );

  ~GRA_cartesianAxes()
  { DeleteStuff(); }

  GRA_cartesianAxes( GRA_cartesianAxes const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff( rhs ); }

  GRA_cartesianAxes &operator=( GRA_cartesianAxes const &rhs )
  {
    if( this != &rhs )
    {
      DeleteStuff();
      type_ = rhs.type_;
      CopyStuff( rhs );
    }
    return *this;
  }

  void Make();
  void Draw( GRA_wxWidgets *, wxDC & );

  friend std::ostream &operator<<( std::ostream &, GRA_cartesianAxes const & );

  void SetAxes( GRA_axis *x, GRA_axis *y,
                GRA_axis *bottom, GRA_axis *top, GRA_axis *left, GRA_axis *right )
  {
    xAxis_ = x;
    yAxis_ = y;
    boxBottomAxis_ = bottom;
    boxTopAxis_ = top;
    boxLeftAxis_ = left;
    boxRightAxis_ = right;
  }

  void GetAxes( GRA_axis *&, GRA_axis *&, GRA_axis *&, GRA_axis *&, GRA_axis *&, GRA_axis *& );
  
private:
  void DeleteStuff();
  void CopyStuff( GRA_cartesianAxes const & );
  void AutoScale( std::vector<double> const &, std::vector<double> const & );
  void GetActualValues( double &, double &, double );
  void ResetLogValue( double &, double );
  void FixupScales( bool, bool, double, double, double, int &, double &, double &, double &,
                    GRA_setOfCharacteristics *, bool );
  int NumberOfDigits( double, double, double );
  void SetNumberOfIncrements( int &, double, double, GRA_setOfCharacteristics * );
  //
  bool xOnTop_, yOnRight_;
  GRA_axis *xAxis_, *yAxis_;
  GRA_axis *boxBottomAxis_, *boxTopAxis_, *boxLeftAxis_, *boxRightAxis_;
};

#endif
