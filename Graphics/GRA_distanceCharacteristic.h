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
#ifndef GRA_DISTANCECHARACTERISTIC
#define GRA_DISTANCECHARACTERISTIC

#include <vector>
#include <ostream>

#include "GRA_characteristic.h"

class GRA_distanceCharacteristic : public GRA_characteristic
{
public:
  GRA_distanceCharacteristic( char const *name, double value, bool percent, double min, double max )
      : GRA_characteristic(name,DISTANCE,false),
        min_(min), max_(max), value_(value), setAsPercent_(percent), default_(value)
  {}

  GRA_distanceCharacteristic( char const *name, std::vector<double> const &values, bool percent,
                              double min, double max )
      : GRA_characteristic(name,DISTANCE,true),
        min_(min), max_(max), value_(0.0), setAsPercent_(percent)
  { values_.assign( values.begin(), values.end() ); }

  ~GRA_distanceCharacteristic()
  {}
  
  GRA_distanceCharacteristic( GRA_distanceCharacteristic const &rhs ) : GRA_characteristic(rhs)
  { CopyStuff( rhs ); }
  
  GRA_distanceCharacteristic &operator=( GRA_distanceCharacteristic const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }
  
  double GetMin() const
  { return min_; }
  
  double GetMax() const
  { return max_; }
  
  void SetAsPercent( double value )
  {
    value_ = value;
    setAsPercent_ = true;
    isAVector_ = false;
  }
  
  void SetAsPercent( std::vector<double> const &values )
  {
    values_.assign( values.begin(), values.end() );
    setAsPercent_ = true;
    isAVector_ = true;
  }
  
  void SetAsWorld( double value )
  {
    value_ = value;
    setAsPercent_ = false;
    isAVector_ = false;
  }
  
  void SetAsWorld( std::vector<double> const &values )
  {
    values_.assign( values.begin(), values.end() );
    setAsPercent_ = false;
    isAVector_ = true;
  }

  void Set( double value, bool setAsPercent )
  {
    value_ = value;
    setAsPercent_ = setAsPercent;
    isAVector_ = false;
  }

  void Set( std::vector<double> const &values, bool setAsPercent )
  {
    values_.assign( values.begin(), values.end() );
    setAsPercent_ = setAsPercent;
    isAVector_ = true;
  }

  double Get( bool percent ) const
  {
    if( percent )return GetAsPercent();
    else         return GetAsWorld();
  }

  std::vector<double> Gets( bool percent ) const
  {
    if( percent )return GetAsPercents();
    else         return GetAsWorlds();
  }

  double GetAsPercent() const;
  std::vector<double> GetAsPercents() const;
  double GetAsWorld() const;
  std::vector<double> GetAsWorlds() const;

  friend std::ostream &operator<<( std::ostream &, GRA_distanceCharacteristic const & );

private:
  void CopyStuff( GRA_distanceCharacteristic const & );
  //
  double min_, max_;  // dimension size
  double value_, default_;
  std::vector<double> values_;
  bool setAsPercent_;
};

#endif
