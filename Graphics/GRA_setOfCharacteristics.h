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
#ifndef GRA_SETOFCHARACTERISTICS
#define GRA_SETOFCHARACTERISTICS

#include <vector>
#include <ostream>

#include "ExString.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_stringCharacteristic.h"

class GRA_font;
class GRA_color;

class GRA_setOfCharacteristics
{
private:
  typedef std::vector<GRA_characteristic*> cVec;
  typedef cVec::const_iterator cVecIter;

public:
  GRA_setOfCharacteristics()
  {}
  
  virtual ~GRA_setOfCharacteristics()
  { DeleteStuff(); }

  GRA_setOfCharacteristics( GRA_setOfCharacteristics const &rhs )
  { CopyStuff( rhs ); }
  
  GRA_setOfCharacteristics &operator=( GRA_setOfCharacteristics const &rhs )
  {
    if( this != &rhs )
    {
      DeleteStuff();
      CopyStuff( rhs );
    }
    return *this;
  }

  void Clear()
  { DeleteStuff(); }

  void AddSize( char const *name, double size, bool setAsPercent, double min, double max )
  {
    GRA_sizeCharacteristic *sc = new GRA_sizeCharacteristic(name,size,setAsPercent,min,max);
    characteristics_.push_back( sc );
  }

  void AddSize( char const *name, std::vector<double> s, bool setAsPercent, double min, double max )
  {
    GRA_sizeCharacteristic *sc = new GRA_sizeCharacteristic(name,s,setAsPercent,min,max);
    characteristics_.push_back( sc );
  }

  void AddDistance( char const *name, double distance, bool setAsPercent, double min, double max )
  {
    GRA_distanceCharacteristic *dc =
        new GRA_distanceCharacteristic(name,distance,setAsPercent,min,max);
    characteristics_.push_back( dc );
  }
  
  void AddDistance( char const *name, std::vector<double> d, bool setAsPercent, double min, double max )
  {
    GRA_distanceCharacteristic *dc = new GRA_distanceCharacteristic(name,d,setAsPercent,min,max);
    characteristics_.push_back( dc );
  }
  
  void AddNumber( char const *name, double number )
  {
    GRA_doubleCharacteristic *dc = new GRA_doubleCharacteristic(name,number);
    characteristics_.push_back( dc );
  }
  
  void AddNumber( char const *name, std::vector<double> numbers )
  {
    GRA_doubleCharacteristic *dc = new GRA_doubleCharacteristic(name,numbers);
    characteristics_.push_back( dc );
  }

  void AddNumber( char const *name, int number )
  {
    GRA_intCharacteristic *ic = new GRA_intCharacteristic(name,number);
    characteristics_.push_back( ic );
  }

  void AddNumber( char const *name, std::vector<int> numbers )
  {
    GRA_intCharacteristic *ic = new GRA_intCharacteristic(name,numbers);
    characteristics_.push_back( ic );
  }

  void AddBool( char const *name, bool b )
  {
    GRA_boolCharacteristic *bc = new GRA_boolCharacteristic(name,b);
    characteristics_.push_back( bc );
  }

  void AddBool( char const *name, std::vector<bool> bs )
  {
    GRA_boolCharacteristic *bc = new GRA_boolCharacteristic(name,bs);
    characteristics_.push_back( bc );
  }

  void AddAngle( char const *name, double angle )
  {
    GRA_angleCharacteristic *dc = new GRA_angleCharacteristic(name,angle);
    characteristics_.push_back( dc );
  }

  void AddAngle( char const *name, std::vector<double> angles )
  {
    GRA_angleCharacteristic *dc = new GRA_angleCharacteristic(name,angles);
    characteristics_.push_back( dc );
  }

  void AddFont( char const *name, GRA_font *font )
  {
    GRA_fontCharacteristic *fc = new GRA_fontCharacteristic(name,font);
    characteristics_.push_back( fc );
  }

  void AddFont( char const *name, std::vector<GRA_font*> fonts )
  {
    GRA_fontCharacteristic *fc = new GRA_fontCharacteristic(name,fonts);
    characteristics_.push_back( fc );
  }
  
  void AddColor( char const *name, GRA_color *color )
  {
    GRA_colorCharacteristic *cc = new GRA_colorCharacteristic(name,color);
    characteristics_.push_back( cc );
  }
  
  void AddColor( char const *name, std::vector<GRA_color*> colors )
  {
    GRA_colorCharacteristic *cc = new GRA_colorCharacteristic(name,colors);
    characteristics_.push_back( cc );
  }

  void AddString( char const *name, ExString const &s )
  {
    GRA_stringCharacteristic *sc = new GRA_stringCharacteristic(name,s);
    characteristics_.push_back( sc );
  }

  GRA_characteristic *Get( ExString const &name ) const
  { return Get( name.c_str() ); }

  GRA_characteristic *Get( char const * ) const;

  friend std::ostream &operator<<( std::ostream &, GRA_setOfCharacteristics const & );

private:
  void DeleteStuff();
  void CopyStuff( GRA_setOfCharacteristics const & );
  //
  cVec characteristics_;
};

#endif
