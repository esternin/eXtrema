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
#ifndef GRA_DOUBLECHARACTERISTIC
#define GRA_DOUBLECHARACTERISTIC

#include <vector>
#include <ostream>

#include "GRA_characteristic.h"

class GRA_doubleCharacteristic : public GRA_characteristic
{
public:
  GRA_doubleCharacteristic( wxChar const *name, double value )
      : GRA_characteristic(name,DOUBLE,false), value_(value), default_(value)
  {}

  GRA_doubleCharacteristic( wxChar const *name, std::vector<double> const &values )
      : GRA_characteristic(name,DOUBLE,true), value_(0.0)
  { values_.assign( values.begin(), values.end() ); }
  
  ~GRA_doubleCharacteristic()
  {}
  
  GRA_doubleCharacteristic( GRA_doubleCharacteristic const &rhs ) : GRA_characteristic(rhs)
  { CopyStuff( rhs ); }
  
  GRA_doubleCharacteristic &operator=( GRA_doubleCharacteristic const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }
  
  void Set( double value )
  {
    value_ = value;
    isAVector_ = false;
  }
  
  void Set( std::vector<double> const &values )
  {
    values_.assign( values.begin(), values.end() );
    isAVector_ = true;
  }

  double Get() const
  { return value_; }
  
  std::vector<double> &Gets()
  { return values_; }

  friend std::ostream &operator<<( std::ostream &, GRA_doubleCharacteristic const & );

private:
  void CopyStuff( GRA_doubleCharacteristic const &rhs );
  //
  double value_, default_;
  std::vector<double> values_;
};

#endif
