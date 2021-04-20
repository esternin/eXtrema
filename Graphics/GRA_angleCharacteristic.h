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
#ifndef GRA_ANGLECHARACTERISTIC
#define GRA_ANGLECHARACTERISTIC

#include <vector>
#include <ostream>

#include "GRA_characteristic.h"

class GRA_angleCharacteristic : public GRA_characteristic
{
public:
  GRA_angleCharacteristic( char const *name, double angle )
      : GRA_characteristic(name,ANGLE,false), default_(angle)
  { Set(angle); }

  GRA_angleCharacteristic( char const *name, std::vector<double> const &angles )
      : GRA_characteristic(name,ANGLE,true), value_(0.0)
  { Set(angles); }
  
  ~GRA_angleCharacteristic()
  {}

  GRA_angleCharacteristic( GRA_angleCharacteristic const &rhs ) : GRA_characteristic(rhs)
  { CopyStuff( rhs ); }

  GRA_angleCharacteristic &operator=( GRA_angleCharacteristic const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }

  void Set( double angle )
  {
    value_ = angle - static_cast<int>(angle/360.)*360.;
    if( value_ < 0.0 )value_ += 360.0;
    isAVector_ = false;
  }

  void Set( std::vector<double> const &angles )
  {
    std::vector<double>().swap( values_ ); // empty the values_ vector
    std::vector<double>::const_iterator end = angles.end();
    for( std::vector<double>::const_iterator i=angles.begin(); i!=end; ++i )
    {
      double angle = (*i) - static_cast<int>((*i)/360.)*360.;
      if( angle < 0.0 )angle += 360.0;
      values_.push_back( angle );
    }
    isAVector_ = true;
  }

  double Get() const
  { return value_; }

  std::vector<double> &Gets()
  { return values_; }

  friend std::ostream &operator<<( std::ostream &, GRA_angleCharacteristic const & );

private:
  void CopyStuff( GRA_angleCharacteristic const &rhs );
  //
  double default_, value_;
  std::vector<double> values_;
};

#endif
