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
#ifndef EX_TEXTDATA
#define EX_TEXTDATA

#define VARIABLES_LINKAGE_
/*
#ifdef BUILDING_VARIABLES_DLL_
#define VARIABLES_LINKAGE_ __declspec(dllexport)
#else
#define VARIABLES_LINKAGE_ __declspec(dllimport)
#endif
*/

#include <vector>

#include "ExString.h"
#include "Variable.h"

class NumericData;

class VARIABLES_LINKAGE_ TextData
{
public:
  TextData()
      : numberOfDimensions_(0)
  {}

  TextData( int i )
      : numberOfDimensions_(i)
  {}
  
  virtual ~TextData()
  {}
  
  TextData( TextData const &rhs )
  { CopyStuff( rhs ); }
  
  TextData & operator=( TextData const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }

  void SetNumberOfDimensions( int i )
  { numberOfDimensions_ = i; }
  
  std::size_t GetNumberOfDimensions() const
  { return numberOfDimensions_; }
  
  ExString GetScalarValue() const
  { return scalarValue_; }
  
  void SetScalarValue( ExString const &s )
  { scalarValue_ = s; }

  std::vector<ExString> &GetData()
  { return data_; }

  void AddString( ExString const &s )
  { data_.push_back( s ); }
  
  void SetData( std::vector<ExString> const &d )
  { data_.assign( d.begin(), d.end() ); }

  void SetData( std::size_t, ExString const & );
  
  ExString &GetData( std::size_t i )
  { return data_[i]; }

protected:
  std::vector<ExString> data_;
  ExString scalarValue_;
  int numberOfDimensions_;

  void CopyStuff( TextData const & );
};

#endif
