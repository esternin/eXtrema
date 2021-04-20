/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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
#ifndef GRA_POLARAXES
#define GRA_POLARAXES

#include <vector>
#include <ostream>

#include "ExString.h"
#include "GRA_drawableObject.h"

class GRA_window;
class GRA_axis;
class GRA_outputType;
class GRA_setOfCharacteristics;
class GRA_color;
class GRA_font;

class GRA_polarAxes : public GRA_drawableObject
{
public:
  GRA_polarAxes()
    : GRA_drawableObject("POLARAXES"), axis_(0)
  {}

  GRA_polarAxes( std::vector<double> const &, std::vector<double> const &, bool );

  ~GRA_polarAxes()
  { DeleteStuff(); }

  GRA_polarAxes( GRA_polarAxes const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff(rhs); }

  GRA_polarAxes &operator=( GRA_polarAxes const &rhs )
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
  void Draw( GRA_outputType * );

  friend std::ostream &operator<<( std::ostream &, GRA_polarAxes const & );

  void SetAxes( GRA_axis *axis )
  { axis_ = axis; }

  void GetAxes( GRA_axis *&axis ) const
  { axis = axis_; }

private:
  void DeleteStuff();
  void CopyStuff( GRA_polarAxes const & );
  void DrawLabel( GRA_outputType * );
  int NumberOfDigits( double, double );
  void DrawAngle( ExString &, GRA_axis *, double, GRA_color *, GRA_font *, GRA_outputType * );
  //
  GRA_axis *axis_;
  std::vector<GRA_axis*> otherAxes_;
  double r0_, theta0_;
};

#endif
