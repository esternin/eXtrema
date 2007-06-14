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

#include "wx/wx.h"

class GRA_font;
class GRA_color;
class GRA_characteristic;

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

  void AddSize( wxChar const *, double, bool, double, double );
  void AddSize( wxChar const *, std::vector<double>, bool, double, double );

  void AddDistance( wxChar const *, double, bool, double, double );
  void AddDistance( wxChar const *, std::vector<double>, bool, double, double );
  
  void AddNumber( wxChar const *, double );
  void AddNumber( wxChar const *, std::vector<double> );

  void AddNumber( wxChar const *, int );
  void AddNumber( wxChar const *, std::vector<int> );

  void AddBool( wxChar const *, bool );
  void AddBool( wxChar const *, std::vector<bool> );

  void AddAngle( wxChar const *, double );
  void AddAngle( wxChar const *, std::vector<double> );

  void AddFont( wxChar const *, GRA_font * );
  void AddFont( wxChar const *, std::vector<GRA_font*> );
  
  void AddColor( wxChar const *, GRA_color * );
  void AddColor( wxChar const *, std::vector<GRA_color*> );

  void AddString( wxChar const *, wxString const & );
  GRA_characteristic *Get( wxString const & ) const;

  GRA_characteristic *Get( wxChar const * ) const;

  friend std::ostream &operator<<( std::ostream &, GRA_setOfCharacteristics const & );

private:
  void DeleteStuff();
  void CopyStuff( GRA_setOfCharacteristics const & );
  //
  cVec characteristics_;
};

#endif
