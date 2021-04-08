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
#include "GRA_setOfCharacteristics.h"
#include "GRA_characteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "ExGlobals.h"

void GRA_setOfCharacteristics::DeleteStuff()
{
  while( !characteristics_.empty() )
  {
     delete characteristics_.back();
     characteristics_.pop_back();
  }
}

void GRA_setOfCharacteristics::CopyStuff( GRA_setOfCharacteristics const &rhs )
{
  cVecIter end = rhs.characteristics_.end();
  for( cVecIter i=rhs.characteristics_.begin(); i!=end; ++i )
  {
    GRA_characteristic *c = 0;
    if( (*i)->IsSize() )
    {
      c = new GRA_sizeCharacteristic(*static_cast<GRA_sizeCharacteristic*>(*i));
    }
    else if( (*i)->IsDistance() )
    {
      c = new GRA_distanceCharacteristic(*static_cast<GRA_distanceCharacteristic*>(*i));
    }
    else if( (*i)->IsInt() )
    {
      c = new GRA_intCharacteristic(*static_cast<GRA_intCharacteristic*>(*i));
    }
    else if( (*i)->IsDouble() )
    {
      c = new GRA_doubleCharacteristic(*static_cast<GRA_doubleCharacteristic*>(*i));
    }
    else if( (*i)->IsBool() )
    {
      c = new GRA_boolCharacteristic(*static_cast<GRA_boolCharacteristic*>(*i));
    }
    else if( (*i)->IsAngle() )
    {
      c = new GRA_angleCharacteristic(*static_cast<GRA_angleCharacteristic*>(*i));
    }
    else if( (*i)->IsFont() )
    {
      c = new GRA_fontCharacteristic(*static_cast<GRA_fontCharacteristic*>(*i));
    }
    else if( (*i)->IsColor() )
    {
      c = new GRA_colorCharacteristic(*static_cast<GRA_colorCharacteristic*>(*i));
    }
    else if( (*i)->IsString() )
    {
      c = new GRA_stringCharacteristic(*static_cast<GRA_stringCharacteristic*>(*i));
    }
    else assert( 0 );
    characteristics_.push_back( c );
  }
}

void GRA_setOfCharacteristics::AddString( wxChar const *name, wxString const &s )
{
  GRA_stringCharacteristic *sc = new GRA_stringCharacteristic(name,s);
  characteristics_.push_back( sc );
}

void GRA_setOfCharacteristics::AddDistance( wxChar const *name, double distance,
                                            bool setAsPercent, double min, double max )
{
  GRA_distanceCharacteristic *dc =
      new GRA_distanceCharacteristic(name,distance,setAsPercent,min,max);
  characteristics_.push_back( dc );
}

void GRA_setOfCharacteristics::AddDistance( wxChar const *name, std::vector<double> d,
                                            bool setAsPercent, double min, double max )
{
  GRA_distanceCharacteristic *dc =
      new GRA_distanceCharacteristic(name,d,setAsPercent,min,max);
  characteristics_.push_back( dc );
}

void GRA_setOfCharacteristics::AddNumber( wxChar const *name, double number )
{
  GRA_doubleCharacteristic *dc = new GRA_doubleCharacteristic(name,number);
  characteristics_.push_back( dc );
}

void GRA_setOfCharacteristics::AddNumber( wxChar const *name, std::vector<double> numbers )
{
  GRA_doubleCharacteristic *dc = new GRA_doubleCharacteristic(name,numbers);
  characteristics_.push_back( dc );
}

void GRA_setOfCharacteristics::AddNumber( wxChar const *name, int number )
{
  GRA_intCharacteristic *ic = new GRA_intCharacteristic(name,number);
  characteristics_.push_back( ic );
}

void GRA_setOfCharacteristics::AddNumber( wxChar const *name, std::vector<int> numbers )
{
  GRA_intCharacteristic *ic = new GRA_intCharacteristic(name,numbers);
  characteristics_.push_back( ic );
}

void GRA_setOfCharacteristics::AddBool( wxChar const *name, bool b )
{
  GRA_boolCharacteristic *bc = new GRA_boolCharacteristic(name,b);
  characteristics_.push_back( bc );
}

void GRA_setOfCharacteristics::AddBool( wxChar const *name, std::vector<bool> bs )
{
  GRA_boolCharacteristic *bc = new GRA_boolCharacteristic(name,bs);
  characteristics_.push_back( bc );
}

void GRA_setOfCharacteristics::AddAngle( wxChar const *name, double angle )
{
  GRA_angleCharacteristic *dc = new GRA_angleCharacteristic(name,angle);
  characteristics_.push_back( dc );
}

void GRA_setOfCharacteristics::AddAngle( wxChar const *name, std::vector<double> angles )
{
  GRA_angleCharacteristic *dc = new GRA_angleCharacteristic(name,angles);
  characteristics_.push_back( dc );
}

void GRA_setOfCharacteristics::AddFont( wxChar const *name, GRA_font *font )
{
  GRA_fontCharacteristic *fc = new GRA_fontCharacteristic(name,font);
  characteristics_.push_back( fc );
}

void GRA_setOfCharacteristics::AddFont( wxChar const *name, std::vector<GRA_font*> fonts )
{
  GRA_fontCharacteristic *fc = new GRA_fontCharacteristic(name,fonts);
  characteristics_.push_back( fc );
}

void GRA_setOfCharacteristics::AddColor( wxChar const *name, GRA_color *color )
{
  GRA_colorCharacteristic *cc = new GRA_colorCharacteristic(name,color);
  characteristics_.push_back( cc );
}

void GRA_setOfCharacteristics::AddColor( wxChar const *name, std::vector<GRA_color*> colors )
{
  GRA_colorCharacteristic *cc = new GRA_colorCharacteristic(name,colors);
  characteristics_.push_back( cc );
}

void GRA_setOfCharacteristics::AddSize( wxChar const *name, double size, bool setAsPercent,
                                        double min, double max )
{
  GRA_sizeCharacteristic *sc = new GRA_sizeCharacteristic(name,size,setAsPercent,min,max);
  characteristics_.push_back( sc );
}

void GRA_setOfCharacteristics::AddSize( wxChar const *name, std::vector<double> s,
                                        bool setAsPercent, double min, double max )
{
  GRA_sizeCharacteristic *sc = new GRA_sizeCharacteristic(name,s,setAsPercent,min,max);
  characteristics_.push_back( sc );
}

GRA_characteristic *GRA_setOfCharacteristics::Get( wxString const &name ) const
{
  cVecIter end = characteristics_.end();
  for( cVecIter i=characteristics_.begin(); i!=end; ++i )
  {
    if( (*i)->GetName() == name )return *i;
  }
  return 0;
}

std::ostream &operator<<( std::ostream &out, GRA_setOfCharacteristics const &sc )
{
  GRA_setOfCharacteristics::cVecIter end = sc.characteristics_.end();
  for( GRA_setOfCharacteristics::cVecIter i=sc.characteristics_.begin(); i!=end; ++i )
   {
    if( (*i)->IsSize() )
      out << *static_cast<GRA_sizeCharacteristic*>(*i);
    else if( (*i)->IsDistance() )
      out << *static_cast<GRA_distanceCharacteristic*>(*i);
    else if( (*i)->IsInt() )
      out << *static_cast<GRA_intCharacteristic*>(*i);
    else if( (*i)->IsDouble() )
      out << *static_cast<GRA_doubleCharacteristic*>(*i);
    else if( (*i)->IsBool() )
      out << *static_cast<GRA_boolCharacteristic*>(*i);
    else if( (*i)->IsAngle() )
      out << *static_cast<GRA_angleCharacteristic*>(*i);
    else if( (*i)->IsFont() )
      out << *static_cast<GRA_fontCharacteristic*>(*i);
    else if( (*i)->IsColor() )
      out << *static_cast<GRA_colorCharacteristic*>(*i);
    else if( (*i)->IsString() )
      out << *static_cast<GRA_stringCharacteristic*>(*i);
  }
  return out;
}

// end of file
