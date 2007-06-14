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
#ifndef GRA_BOOLCHARACTERISTIC
#define GRA_BOOLCHARACTERISTIC

#include <vector>
#include <ostream>

#include "GRA_characteristic.h"

class GRA_boolCharacteristic : public GRA_characteristic
{
public:
  GRA_boolCharacteristic( wxChar const *name, bool value )
      : GRA_characteristic(name,BOOL,false), value_(value), default_(value)
  {}
  
  GRA_boolCharacteristic( wxChar const *name, std::vector<bool> const &values )
      : GRA_characteristic(name,BOOL,true), value_(false)
  { values_.assign( values.begin(), values.end() ); }
  
  ~GRA_boolCharacteristic()
  {}
  
  GRA_boolCharacteristic( GRA_boolCharacteristic const &rhs ) : GRA_characteristic(rhs)
  { CopyStuff( rhs ); }
  
  GRA_boolCharacteristic &operator=( GRA_boolCharacteristic const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }
  
  void Set( bool value )
  {
    value_ = value;
    isAVector_ = false;
  }

  void Set( std::vector<bool> const &values )
  {
    values_.assign( values.begin(), values.end() );
    isAVector_ = true;
  }

  bool Get() const
  { return value_; }
  
  std::vector<bool> &Gets()
  { return values_; }

  friend std::ostream &operator<<( std::ostream &, GRA_boolCharacteristic const & );

private:
  void CopyStuff( GRA_boolCharacteristic const &rhs );
  //
  bool value_, default_;
  std::vector<bool> values_;
};

#endif
