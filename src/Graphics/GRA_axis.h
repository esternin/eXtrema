/*
Copyright (C) 2005,...,2007 Joseph L. Chuma, TRIUMF

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
#ifndef GRA_AXIS
#define GRA_AXIS

#include <vector>
#include <fstream>

#include "wx/wx.h"

#include "GRA_drawableObject.h"

class GRA_setOfCharacteristics;
class GRA_wxWidgets;
class GRA_font;
class GRA_polyline;
class GRA_drawableText;

class GRA_axis : public GRA_drawableObject
{
public:
  GRA_axis( double, double, double, GRA_setOfCharacteristics const * );

  ~GRA_axis()
  { DeleteStuff(); }

  GRA_axis( GRA_axis const &rhs ) : GRA_drawableObject(rhs)
  { CopyStuff( rhs ); }

  GRA_axis &operator=( GRA_axis const &rhs )
  {
    if( this != &rhs )
    {
      DeleteStuff();
      type_ = rhs.type_;
      CopyStuff( rhs );
    }
    return *this;
  }

  GRA_setOfCharacteristics *GetCharacteristics()
  { return characteristics_; }

  void GetOrigin( double &xOrigin, double &yOrigin ) const
  { xOrigin=xOrigin_; yOrigin=yOrigin_; }

  double GetMaxWidth() const
  { return maxWidth_; }

  double GetLowerAxis() const;
  double GetUpperAxis() const;
  double GetAngle() const;

  void GetEndPoint( double &x, double &y ) const
  { x=xEnd_; y=yEnd_; }

  bool Inside( double, double );

  void Make();
  void Draw( GRA_wxWidgets *, wxDC & );

  friend std::ostream &operator<<( std::ostream &, GRA_axis const & );

  void SetTicCoordinates( std::vector<double> const &x, std::vector<double> const &y )
  {
    xTicCoordinates_.assign( x.begin(), x.end() );
    yTicCoordinates_.assign( y.begin(), y.end() );
  }

  void GetTicCoordinates( std::vector<double> &x, std::vector<double> &y )
  {
    x.assign( xTicCoordinates_.begin(), xTicCoordinates_.end() );
    y.assign( yTicCoordinates_.begin(), yTicCoordinates_.end() );
  }

  void SetPolylines( std::vector<GRA_polyline*> const &polylines )
  { polylines_.assign( polylines.begin(), polylines.end() ); }

  std::vector<GRA_polyline*> &GetPolylines()
  { return polylines_; }
  
  void SetDrawableText( std::vector<GRA_drawableText*> const &textVec )
  { textVec_.assign( textVec.begin(), textVec.end() ); }

  std::vector<GRA_drawableText*> &GetDrawableText()
  { return textVec_; }
  
  std::size_t GetNPoly()
  { return polylines_.size(); }
  
private:
  void CopyStuff( GRA_axis const & );
  void DeleteStuff();

  void InheritFrom( GRA_axis * );

  void Fnice();
  wxString DoubleToString( double, int, int, int ) const;
  void MakeLinearAxis();
  void MakeLogAxis();
  void MakeATic( double, double, double, double, double, double, double &, double & );
  //
  void NormalizeAngle( double &angle )
  {
    angle -= static_cast<int>(angle/360.)*360.;
    if( angle < 0.0 )angle += 360.;
  }
  //
  GRA_setOfCharacteristics *characteristics_;
  //
  std::vector<GRA_polyline*> polylines_;
  std::vector<GRA_drawableText*> textVec_;
  //
  double xOrigin_, yOrigin_, length_;  // origin coordinates of the axis and its length
  double xEnd_, yEnd_;                 // coordinates of axis end point
  std::vector<double> xTicCoordinates_, yTicCoordinates_;
  double maxWidth_; // max width of axis numbers (world coordinates)
                    // available only after the axis is drawn
};

#endif
