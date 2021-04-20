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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ExGlobals.h"
#include "GRA_setOfCharacteristics.h"

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

GRA_characteristic *GRA_setOfCharacteristics::Get( char const *name ) const
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
