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
#ifndef GRA_COLORCHARACTERISTIC
#define GRA_COLORCHARACTERISTIC

#include <vector>
#include <ostream>

#include "GRA_characteristic.h"

class GRA_color;

class GRA_colorCharacteristic : public GRA_characteristic
{
public:
  GRA_colorCharacteristic( wxChar const *name, GRA_color *value )
      : GRA_characteristic(name,COLOR,false), value_(value), default_(value)
  {}
  
  GRA_colorCharacteristic( wxChar const *name, std::vector<GRA_color*> const &values )
      : GRA_characteristic(name,COLOR,true), value_(0)
  { values_.assign( values.begin(), values.end() ); }
  
  ~GRA_colorCharacteristic()
  {}
  
  GRA_colorCharacteristic( GRA_colorCharacteristic const &rhs ) : GRA_characteristic(rhs)
  { CopyStuff(rhs); }

  GRA_colorCharacteristic &operator=( GRA_colorCharacteristic const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }
  
  void Set( GRA_color *value )
  {
    value_ = value;
    isAVector_ = false;
  }

  void Set( std::vector<GRA_color*> const &values )
  {
    values_.assign( values.begin(), values.end() );
    isAVector_ = true;
  }

  void Set( std::vector<int> const & );

  GRA_color *Get() const
  {
    if( isAVector_ )return values_.front();
    return value_;
  }

  std::vector<GRA_color*> &Gets()
  { return values_; }

  friend std::ostream &operator<<( std::ostream &, GRA_colorCharacteristic const & );

private:
  void CopyStuff( GRA_colorCharacteristic const & );
  //
  GRA_color *value_, *default_;
  std::vector<GRA_color*> values_;
};

#endif
