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

#define PARSING_LINKAGE_
/*
#if defined BUILDING_PARSING_DLL_
#define PARSING_LINKAGE_ __declspec(dllexport)
#else
#define PARSING_LINKAGE_ __declspec(dllimport)
#endif
*/

#include <vector>

#include "ExString.h"
#include "ESyntaxError.h"
#include "ParseToken.h"

class PARSING_LINKAGE_ ParseLine
{
public:
  ParseLine( ExString const &line ) : inputLine_(line)
  {}
  
  ~ParseLine()
  {
    ptvIter end = tokens_.end();
    for( ptvIter i=tokens_.begin(); i!=end; ++i )delete *i;
  }
  
  ParseLine( ParseLine const &rhs )
  { CopyStuff( rhs ); }
  
  ParseLine &operator=( ParseLine const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }

  ExString const &GetInputLine() const
  { return inputLine_; }
  
  ParseToken * GetToken( int i ) const
  { return tokens_[i]; }
  
  int GetNumberOfTokens() const
  { return tokens_.size(); }
  
  ExString &GetString( std::size_t i ) const
  {
    if( i >= tokens_.size() )throw ESyntaxError( "attempt to access non-existent token" );
    return tokens_[i]->GetString();
  }
  
  double GetNumeric( std::size_t i ) const
  {
    if( i >= tokens_.size() )throw ESyntaxError( "attempt to access non-existent token" );
    return tokens_[i]->GetNumeric();
  }
  
  bool IsEqualSign( std::size_t i ) const
  {
    if( i >= tokens_.size() )return false;
    return tokens_[i]->IsaEqualSign();
  }
  
  bool IsNull( std::size_t i ) const
  {
    if( i >= tokens_.size() )return false;
    return tokens_[i]->IsaNull();
  }
  
  bool IsNumeric( std::size_t i ) const
  {
    if( i >= tokens_.size() )return false;
    return tokens_[i]->IsaNumeric();
  }
  
  bool IsString( std::size_t i ) const
  {
    if( i >= tokens_.size() )return false;
    return tokens_[i]->IsaString();
  }
  
  int NumberOfStrings() const;
  int NumberOfNumerics() const;
  int NumberOfNulls() const;
  void ParseIt();

private:
  typedef std::vector<ParseToken*> parseTokenVec;
  typedef parseTokenVec::const_iterator ptvIter;

  void CopyStuff( ParseLine const & );
  void NumericOrString();

  void AddToString( char c )
  { tokens_.back()->GetString().append( c ); }

  void AddToQualifier( char c )
  { tokens_.back()->GetLastQualifier()->append( c ); }

  parseTokenVec tokens_;
  ExString inputLine_;
};

#endif
