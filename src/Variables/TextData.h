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

#include <vector>

#include "wx/wx.h"

class TextData
{
public:
  TextData();
  TextData( int );
  virtual ~TextData();
  TextData( TextData const & );
  TextData &operator=( TextData const & );

  void SetNumberOfDimensions( int i )
  { numberOfDimensions_ = i; }
  
  std::size_t GetNumberOfDimensions() const
  { return numberOfDimensions_; }

  wxString GetScalarValue() const;
  void SetScalarValue( wxString const & );
  std::vector<wxString> &GetData();
  void AddString( wxString const & );
  void SetData( std::vector<wxString> const & );
  void SetData( std::size_t, wxString const & );
  wxString &GetData( std::size_t );

protected:
  std::vector<wxString> data_;
  wxString scalarValue_;
  int numberOfDimensions_;

  void CopyStuff( TextData const & );
};

#endif
