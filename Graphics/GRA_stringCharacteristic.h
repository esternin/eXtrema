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
#ifndef GRA_STRINGCHARACTERISTIC
#define GRA_STRINGCHARACTERISTIC

#include <vector>
#include <ostream>

#include "ExString.h"

#include "GRA_characteristic.h"

class GRA_stringCharacteristic : public GRA_characteristic
{
public:
  GRA_stringCharacteristic( char const *name, ExString const &value )
      : GRA_characteristic(name,STRING,false), value_(value), default_(value)
  {}

  GRA_stringCharacteristic( char const *name, std::vector<ExString> const &values )
      : GRA_characteristic(name,STRING,true)
  { values_.assign( values.begin(), values.end() ); }

  ~GRA_stringCharacteristic()
  {}

  GRA_stringCharacteristic( GRA_stringCharacteristic const &rhs ) : GRA_characteristic(rhs)
  { CopyStuff( rhs ); }

  GRA_stringCharacteristic &operator=( GRA_stringCharacteristic const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }

  void Set( ExString const &value )
  {
    value_ = value;
    isAVector_ = false;
  }

  void Set( std::vector<ExString> const &values )
  {
    values_.assign( values.begin(), values.end() );
    isAVector_ = true;
  }

  ExString Get() const
  { return value_; }

  std::vector<ExString> &Gets()
  { return values_; }

  friend std::ostream &operator<<( std::ostream &, GRA_stringCharacteristic const & );

private:
  void CopyStuff( GRA_stringCharacteristic const & );
  //
  ExString value_, default_;
  std::vector<ExString> values_;
};

#endif
