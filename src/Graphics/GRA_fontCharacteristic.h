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
#ifndef GRA_FONTCHARACTERISTIC
#define GRA_FONTCHARACTERISTIC

#include <vector>
#include <ostream>

#include "GRA_characteristic.h"

class GRA_font;

class GRA_fontCharacteristic : public GRA_characteristic
{
public:
  GRA_fontCharacteristic( wxChar const *name, GRA_font *value )
    : GRA_characteristic(name,FONT,false), value_(value), default_(value)
  {}

  GRA_fontCharacteristic( wxChar const *name, std::vector<GRA_font*> const &values )
    : GRA_characteristic(name,FONT,true), value_(0)
  { values_.assign( values.begin(), values.end() ); }

  ~GRA_fontCharacteristic()
  {}

  GRA_fontCharacteristic( GRA_fontCharacteristic const &rhs ) : GRA_characteristic(rhs)
  { CopyStuff(rhs); }

  GRA_fontCharacteristic &operator=( GRA_fontCharacteristic const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }

  void Set( GRA_font *value )
  {
    value_ = value;
    isAVector_ = false;
  }

  void Set( std::vector<GRA_font*> const &values )
  {
    values_.assign( values.begin(), values.end() );
    isAVector_ = true;
  }

  void Set( int );
  void Set( std::vector<int> const & );

  GRA_font *Get() const
  {
    if( isAVector_ )return values_.front();
    return value_;
  }

  std::vector<GRA_font*> &Gets()
  { return values_; }

  friend std::ostream &operator<<( std::ostream &, GRA_fontCharacteristic const & );

private:
  void CopyStuff( GRA_fontCharacteristic const & );
  //
  GRA_font *value_, *default_;
  std::vector<GRA_font*> values_;
};

#endif
