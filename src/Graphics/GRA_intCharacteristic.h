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
#ifndef GRA_INTCHARACTERISTIC
#define GRA_INTCHARACTERISTIC

#include <vector>
#include <ostream>

#include "GRA_characteristic.h"

class GRA_intCharacteristic : public GRA_characteristic
{
public:
  GRA_intCharacteristic( wxChar const *name, int value )
      : GRA_characteristic(name,INT,false), value_(value), default_(value)
  {}
  
  GRA_intCharacteristic( wxChar const *name, std::vector<int> const &values )
      : GRA_characteristic(name,INT,true), value_(0)
  { values_.assign( values.begin(), values.end() ); }
  
  ~GRA_intCharacteristic()
  {}
  
  GRA_intCharacteristic( GRA_intCharacteristic const &rhs ) : GRA_characteristic(rhs)
  { CopyStuff( rhs ); }
  
  GRA_intCharacteristic &operator=( GRA_intCharacteristic const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }
  
  void  Set( int value )
  {
    value_ = value;
    isAVector_ = false;
  }
  
  void  Set( std::vector<int> const &values )
  {
    values_.assign( values.begin(), values.end() );
    isAVector_ = true;
  }

  int Get() const
  { return value_; }
  
  std::vector<int> &Gets()
  { return values_; }

  friend std::ostream &operator<<( std::ostream &, GRA_intCharacteristic const & );

private:
  void CopyStuff( GRA_intCharacteristic const &rhs );
  //
  int value_, default_;
  std::vector<int> values_;
};

#endif
