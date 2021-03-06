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
#include "TextData.h"
#include "NumericData.h"

TextData::TextData()
    : numberOfDimensions_(0)
{}

TextData::TextData( int i )
    : numberOfDimensions_(i)
{}

TextData::~TextData()
{}

TextData::TextData( TextData const &rhs )
{ CopyStuff( rhs ); }

TextData & TextData::operator=( TextData const &rhs )
{
  if( this != &rhs )CopyStuff( rhs );
  return *this;
}

void TextData::CopyStuff( TextData const &rhs )
{
  data_.assign( rhs.data_.begin(), rhs.data_.end() );
  scalarValue_ = rhs.scalarValue_;
  numberOfDimensions_ = rhs.numberOfDimensions_;
}

void TextData::SetData( std::size_t i, wxString const &s )
{
  if( i < data_.size() )
  {
    data_[i] = s;
  }
  else
  {
    for( std::size_t j=data_.size(); j<i; ++j )data_.push_back( wxString() );
    data_.push_back( s );
  }
}

wxString TextData::GetScalarValue() const
{ return scalarValue_; }

void TextData::SetScalarValue( wxString const &s )
{ scalarValue_ = s; }

std::vector<wxString> &TextData::GetData()
{ return data_; }

void TextData::AddString( wxString const &s )
{ data_.push_back( s ); }

void TextData::SetData( std::vector<wxString> const &d )
{ data_.assign( d.begin(), d.end() ); }
  
wxString &TextData::GetData( std::size_t i )
{ return data_[i]; }

// end of file
