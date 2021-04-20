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
#ifndef GRA_CHARACTERISTIC
#define GRA_CHARACTERISTIC

#include <fstream>

#include "ExString.h"

class GRA_characteristic
{
protected:
  enum Type { SIZE, DISTANCE, INT, DOUBLE, BOOL, ANGLE, FONT, COLOR, STRING };

public:
  GRA_characteristic( char const *name, Type type, bool isAVector )
      : name_(name), type_(type), isAVector_(isAVector)
  {}

  GRA_characteristic()
  {}
  
  virtual ~GRA_characteristic()
  {}

  GRA_characteristic( GRA_characteristic const &rhs )
  { CopyStuff( rhs ); }
  
  GRA_characteristic &operator=( GRA_characteristic const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }
  
  bool IsSize() const
  { return (type_ == SIZE); }
  
  bool IsDistance() const
  { return (type_ == DISTANCE); }
  
  bool IsInt() const
  { return (type_ == INT); }
  
  bool IsDouble() const
  { return (type_ == DOUBLE); }

  bool IsBool() const
  { return (type_ == BOOL); }

  bool IsAngle() const
  { return (type_ == ANGLE); }

  bool IsFont() const
  { return (type_ == FONT); }
  
  bool IsColor() const
  { return (type_ == COLOR); }
  
  bool IsString() const
  { return (type_ == STRING); }
  
  ExString GetName() const
  { return name_; }

  bool IsVector() const
  { return isAVector_; }
  
protected:
  void CopyStuff( GRA_characteristic const &rhs )
  {
    name_ = rhs.name_;
    type_ = rhs.type_;
    isAVector_ = rhs.isAVector_;
  }
  //
  ExString name_;
  Type type_;
  bool isAVector_;
};

#endif
