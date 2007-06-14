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
#ifndef EX_PARSELINE
#define EX_PARSELINE

#include <vector>

#include "wx/wx.h"

class ParseToken;

class ParseLine
{
public:
  ParseLine( wxString const & );
  ~ParseLine();
  ParseLine( ParseLine const & );
  ParseLine &operator=( ParseLine const & );

  wxString const &GetInputLine() const;
  
  ParseToken *GetToken( int ) const;
  int GetNumberOfTokens() const;
  
  wxString &GetString( std::size_t ) const;
  double GetNumeric( std::size_t ) const;
  
  bool IsEqualSign( std::size_t ) const;
  bool IsNull( std::size_t ) const;
  bool IsNumeric( std::size_t ) const;
  bool IsString( std::size_t ) const;
  
  int NumberOfStrings() const;
  int NumberOfNumerics() const;
  int NumberOfNulls() const;
  void ParseIt();

private:
  typedef std::vector<ParseToken*> parseTokenVec;
  typedef parseTokenVec::const_iterator ptvIter;

  void CopyStuff( ParseLine const & );
  void NumericOrString();

  void AddToString( wxChar );
  void AddToQualifier( wxChar );

  parseTokenVec tokens_;
  wxString inputLine_;
};

#endif
