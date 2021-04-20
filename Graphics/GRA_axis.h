/*
Copyright (C) 2005,...,2008 Joseph L. Chuma, TRIUMF

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

#include "ExString.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_drawableObject.h"

class GRA_outputType;
class GRA_font;
class GRA_polyline;
class GRA_drawableText;

class GRA_axis : public GRA_drawableObject
{
public:
  GRA_axis( double xOrigin, double yOrigin, double length, GRA_setOfCharacteristics const *characteristics )
      : GRA_drawableObject("AXIS"), xOrigin_(xOrigin), yOrigin_(yOrigin), length_(length),
        maxWidth_(0.0), characteristics_( new GRA_setOfCharacteristics(*characteristics) )
  {
    nGrid_ = static_cast<GRA_intCharacteristic*>(characteristics_->Get("GRID"))->Get();
  }

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
  double GetAngle() const
  { return static_cast<GRA_angleCharacteristic*>(characteristics_->Get("AXISANGLE"))->Get(); }

  void GetEndPoint( double &x, double &y ) const
  { x=xEnd_; y=yEnd_; }

  void Make();
  void Draw( GRA_outputType * );
  bool Inside( double, double );

  friend std::ostream &operator<<( std::ostream &, GRA_axis const & );

  void GetTicCoordinates( std::vector<double> &ticX, std::vector<double> &ticY )
  {
    ticX.assign( xTicCoordinates_.begin(), xTicCoordinates_.end() );
    ticY.assign( yTicCoordinates_.begin(), yTicCoordinates_.end() );
  }

  void SetTicCoordinates( std::vector<double> const &x, std::vector<double> const &y )
  {
    xTicCoordinates_.assign( x.begin(), x.end() );
    yTicCoordinates_.assign( y.begin(), y.end() );
  }

  void SetPolylines( std::vector<GRA_polyline*> const &polylines )
  { polylines_.assign( polylines.begin(), polylines.end() ); }

  void SetDrawableText( std::vector<GRA_drawableText*> const &textVec )
  { textVec_.assign( textVec.begin(), textVec.end() ); }

  std::vector<GRA_drawableText*> &GetDrawableText()
  { return textVec_; }

  std::size_t GetNPoly()
  { return polylines_.size(); }

  int GetGrid() const
  { return nGrid_; }

  double GetLength() const
  { return length_; }

private:
  void CopyStuff( GRA_axis const & );
  void DeleteStuff();

  void InheritFrom( GRA_axis *inherit )
  {
    delete characteristics_;
    inherit->characteristics_ ? characteristics_ = 0 :
      characteristics_ = new GRA_setOfCharacteristics(*inherit->characteristics_);
  }

  void Fnice();
  ExString DoubleToString( double, int, int, int ) const;
  void MakeLinearAxis();
  void MakeLogAxis();
  void MakeATic( double, double, double, double, double, double, double &, double & );
  //
  void NormalizeAngle( double &angle )
  {
    angle -= static_cast<int>(angle/360.)*360.;
    while( angle < 0.0 )angle += 360.;
  }
  //
  GRA_setOfCharacteristics *characteristics_;
  //
  std::vector<GRA_polyline*> polylines_;
  std::vector<GRA_drawableText*> textVec_;
  //
  double xOrigin_, yOrigin_, length_;  // origin coordinates of the axis and its length
  double xEnd_, yEnd_; // world coordinates of axis end point
  std::vector<double> xTicCoordinates_, yTicCoordinates_;
  double maxWidth_; // max width of axis numbers (world coordinates)
                    // available only after the axis is drawn
  int nGrid_;
};

#endif
