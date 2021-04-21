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
#include "ParseToken.h"

#include <memory>

ParseToken &ParseToken::operator=( ParseToken const &rhs )
{
  if( this != &rhs )
  {
    DeleteQualifiers();
    CopyStuff( rhs );
  }
  return *this;
}

void ParseToken::DeleteQualifiers()
{
  stringVec::const_iterator end = qualifiers_.end();
  for( stringVec::const_iterator i=qualifiers_.begin(); i!=end; ++i )delete *i;
  stringVec().swap( qualifiers_ );
}

void ParseToken::CopyStuff( ParseToken const &rhs )
{
  stringToken_ = rhs.stringToken_;
  numericToken_ = rhs.numericToken_;
  std::size_t size( rhs.qualifiers_.size() );
  for( std::size_t i=0; i<size; ++i )
  {
    std::unique_ptr<wxString> tmp( new wxString(*rhs.qualifiers_[i]) );
    qualifiers_.push_back( tmp.release() );
  }
  tokenType_ = rhs.tokenType_;
}

bool ParseToken::operator==( ParseToken const &rhs ) const
{
  if( rhs.stringToken_ != stringToken_ )return false;
  if( rhs.numericToken_ != numericToken_ )return false;
  if( rhs.tokenType_ != tokenType_ )return false;
  if( rhs.qualifiers_.size() != qualifiers_.size() )return false;
  std::size_t size( qualifiers_.size() );
  for( std::size_t i=0; i!=size; ++i )
  {
    if( *rhs.qualifiers_[i] != *qualifiers_[i] )return false;
  }
  return true;
}

bool ParseToken::operator<( ParseToken const &rhs ) const
{
  if( tokenType_ == rhs.tokenType_ )
  {
    if( tokenType_ == STRINGFIELD )
      return (stringToken_ < rhs.stringToken_);
    else if( tokenType_ == NUMERICFIELD )
      return (numericToken_ < rhs.numericToken_);
  }
  return false;
}

 // end of file
