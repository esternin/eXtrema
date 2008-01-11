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
#include <memory>

#include "ParseLine.h"
#include "ESyntaxError.h"
#include "ParseToken.h"

ParseLine::ParseLine( wxString const &line ) : inputLine_(line)
{}

ParseLine::~ParseLine()
{
  ptvIter end = tokens_.end();
  for( ptvIter i=tokens_.begin(); i!=end; ++i )delete *i;
}

ParseLine::ParseLine( ParseLine const &rhs )
{ CopyStuff( rhs ); }

ParseLine &ParseLine::operator=( ParseLine const &rhs )
{
  if( this != &rhs )CopyStuff( rhs );
  return *this;
}

wxString const &ParseLine::GetInputLine() const
{ return inputLine_; }

ParseToken *ParseLine::GetToken( int i ) const
{ return tokens_[i]; }

int ParseLine::GetNumberOfTokens() const
{ return tokens_.size(); }

void ParseLine::CopyStuff( ParseLine const &rhs )
{
  inputLine_ = rhs.inputLine_;
  ptvIter end = tokens_.end();
  for( ptvIter i=tokens_.begin(); i!=end; ++i )delete *i;
  parseTokenVec().swap( tokens_ );
  end = rhs.tokens_.end();
  for( ptvIter i=rhs.tokens_.begin(); i!=end; ++i )
  {
    ParseToken *pt = new ParseToken( **i );
    tokens_.push_back( pt );
  }
}

wxString &ParseLine::GetString( std::size_t i ) const
{
  if( i >= tokens_.size() )throw ESyntaxError( wxT("attempt to access non-existent token") );
  return tokens_[i]->GetString();
}

double ParseLine::GetNumeric( std::size_t i ) const
{
  if( i >= tokens_.size() )throw ESyntaxError( wxT("attempt to access non-existent token") );
  return tokens_[i]->GetNumeric();
}

bool ParseLine::IsEqualSign( std::size_t i ) const
{
  if( i >= tokens_.size() )return false;
  return tokens_[i]->IsaEqualSign();
}

bool ParseLine::IsNull( std::size_t i ) const
{
  if( i >= tokens_.size() )return false;
  return tokens_[i]->IsaNull();
}

bool ParseLine::IsNumeric( std::size_t i ) const
{
  if( i >= tokens_.size() )return false;
  return tokens_[i]->IsaNumeric();
}

bool ParseLine::IsString( std::size_t i ) const
{
  if( i >= tokens_.size() )return false;
  return tokens_[i]->IsaString();
}

void ParseLine::AddToString( wxChar c )
{ tokens_.back()->GetString().append( c ); }

void ParseLine::AddToQualifier( wxChar c )
{ tokens_.back()->GetLastQualifier()->append( c ); }

int ParseLine::NumberOfStrings() const
{
  int n = 0;
  std::size_t size = tokens_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    if( tokens_[i]->IsaString() )++n;
  }
  return n;
}

int ParseLine::NumberOfNumerics() const
{
  int n = 0;
  std::size_t size = tokens_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    if( tokens_[i]->IsaNumeric() )++n;
  }
  return n;
}

int ParseLine::NumberOfNulls() const
{
  int n = 0;
  std::size_t size = tokens_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    if( tokens_[i]->IsaNull() )++n;
  }
  return n;
}

void ParseLine::NumericOrString()
{
  ptvIter tokenEnd = tokens_.end();
  for( ptvIter j=tokens_.begin(); j!=tokenEnd; ++j )
  {
    wxString tokenString( (*j)->GetString() );
    if( !tokenString.empty() && (*j)->IsaNull() )
    {
      double rdum;
      if( tokenString.ToDouble(&rdum) )
      {
        (*j)->SetNumeric( rdum );
      }
      else
      {
        (*j)->SetString();
        if( tokenString.Upper() == wxT("IFF") )(*j)->SetString( wxT("IFF") );
      }
    }
  }
}

void ParseLine::ParseIt()
{
  int const Classes[128] =
  {
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      0,10, 4, 8, 8, 8, 8, 3, 5, 6, 8, 8, 1, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 5, 7, 6, 8, 8,
      2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
      8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 5, 8, 6, 8, 8
  };
  int const StateTable[28][11] =
  {
   // blank  ,   `   '   " {[( }])   \ othr  =   !
      {  0,  2,  5,  8, 11, 14,-18,-12,  3, 21,100}, // 0 delimeter after field
      {  1,  0,  5,  8, 11, 14,-18,-12,  3, 21,100}, // 1 blank following field
      {  0,  2,  5,  8, 11, 14,-18,-12,  3, 21,100}, // 2 comma after blank (+)
      {  1,  0,  5,  8, 11, 15,-18, 19,  4, 21,100}, // 3 start valid char. (+)
      {  1,  0,  5,  8, 11, 15,-18, 19,  4, 21,100}, // 4 continue valid char.
      {  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6}, // 5 opening quote ` (+)
      {  6,  6,  6,  7,  6,  6,  6,  6,  6,  6,  6}, // 6 inside quotes
      {  1,  0,-10,-10,-10,  4,  4, 19,  4,  4,100}, // 7 closing quote
      {  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9}, // 8 opening quote ' (+)
      {  9,  9,  9, 10,  9,  9,  9,  9,  9,  9,  9}, // 9 inside quotes
      {  1,  0,-10,-10,-10,  4,  4, 19,  4,  4,100}, // 10 closing quote
      { 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12}, // 11 opening quote " (+)
      { 12, 12, 12, 12, 13, 12, 12, 12, 12, 12, 12}, // 12 inside quotes
      {  1,  0,-10,-10,-10,  4,  4, 19,  4,  4,100}, // 13 closing quote
      { 16, 16, 16, 16, 16, 15, 17, 16, 16, 16, 16}, // 14 open (  (+)
      { 16, 16, 16, 16, 16, 15, 17, 16, 16, 16, 16}, // 15 open (
      { 16, 16, 16, 16, 16, 15, 17, 16, 16, 16, 16}, // 16 inside ()
      { 16, 16, 16, 16, 16, 15, 17, 16, 16, 16, 16}, // 17 close )
      {  1,  0,-19,-19,-19, 15,-18, 19,  4, 21,100}, // 18 close final )
      {-22,-22,-16,-16,-16, 24,-18,-12, 20,-30,-22}, // 19 start qualifier
      {  1,  0,-16,-16,-16, 24,-18, 19, 20,-30,100}, // 20 inside qual.
      { 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,100}, // 21 equals sign (+)
      { 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23}, // 22 start expression (+)
      { 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23}, // 23 continue expr.
      { 25, 25,-16,-16,-16, 24, 26,-12, 25, 25, 25}, // 24 qual. opening (
      { 25, 25,-16,-16,-16, 24, 26,-12, 25, 25, 25}, // 25 qual. inside ()
      { 25, 25,-16,-16,-16, 24, 26,-12, 25, 25, 25}, // 26 qual. closing )
      {  1,  0,-14,-14,-14, 24,-18, 20, 20, 21,100}  // 27 qual. closing final )
  };
  int len = inputLine_.size();
  std::size_t openBracketCounter = 0;
  wxChar const openingQuote = 96;
  wxChar const singleQuote = 39;
  wxChar const doubleQuote = 34;
  int currentState, newState = 0;
  int i = -1;
  std::size_t errNo = 0;
  bool newToken = true;
  std::string std_input( inputLine_.mb_str(wxConvUTF8) );
  for( ;; )
  {
    switch (newState)
    {
      case 0:
      {
        errNo = 0;
        newToken = true;
        break;
      }
      case 1:                 // blank following field
      {
        errNo = 0;
        newToken = true;
        break;
      }
      case 2:                 // comma after blank
      {
        std::auto_ptr<ParseToken> atoken( new ParseToken );
        ParseToken *token = atoken.release();
        tokens_.push_back( token );
        openBracketCounter = 0;
        errNo = 0;
        newToken = true;
        break;
      }
      case 3:                 // start valid character
      {
        std::auto_ptr<ParseToken> atoken( new ParseToken );
        ParseToken *token = atoken.release();
        tokens_.push_back( token );
        openBracketCounter = 0;
        AddToString( inputLine_[i] );
        break;
      }
      case 4:                 // add character to a string
      {
        AddToString( inputLine_[i] );
        errNo = 0;
        newToken = false;
        break;
      }
      case 5:                 // start quote string with `
      {
        if( newToken )
        {
          std::auto_ptr<ParseToken> atoken( new ParseToken );
          ParseToken *token = atoken.release();
          tokens_.push_back( token );
        }
        errNo = 1;
        AddToString( inputLine_[i] );
        break;
      }
      case 6:                // add character to quote ` ' string
      {
        AddToString( inputLine_[i] );
        break;
      }
      case 7:                // closing quote '
      {
        AddToString( inputLine_[i] );
        errNo = 0;
        break;
      }
      case 8:                 // start quote string with '
      {
        if( newToken )
        {
          std::auto_ptr<ParseToken> atoken( new ParseToken );
          ParseToken *token = atoken.release();
          tokens_.push_back( token );
        }
        errNo = 1;
        AddToString( inputLine_[i] );
        break;
      }
      case 9:                 // add character to quote ' ' string
      {
        AddToString( inputLine_[i] );
        break;
      }
      case 10:                // closing quote '
      {
        AddToString( inputLine_[i] );
        errNo = 0;
        break;
      }
      case 11:                // start quote string with "
      {
        if( newToken )
        {
          std::auto_ptr<ParseToken> atoken( new ParseToken );
          ParseToken *token = atoken.release();
          tokens_.push_back( token );
        }
        errNo = 1;
        AddToString( inputLine_[i] );
        break;
      }
      case 12:                // add character to quote " " string
      {
        AddToString( inputLine_[i] );
        break;
      }
      case 13:                // closing quote "
      {
        AddToString( inputLine_[i] );
        errNo = 0;
        break;
      }
      case 14:               // initial opening bracket
      {
        std::auto_ptr<ParseToken> atoken( new ParseToken );
        ParseToken *token = atoken.release();
        tokens_.push_back( token );
        ++openBracketCounter;
        errNo = 2;
        AddToString( inputLine_[i] );
        break;
      }
      case 15:               // opening bracket
      {
        ++openBracketCounter;
        AddToString( inputLine_[i] );
        break;
      }
      case 16:               // add character inside brackets
      {
        AddToString( inputLine_[i] );
        break;
      }
      case 17:               // closing bracket
      {
        if( --openBracketCounter == 0 )  // final closing bracket
        {
          newState = 18;
          errNo = 0;
        }
        AddToString( inputLine_[i] );
        break;
      }
      case 18:               // final closing bracket
      {
        AddToString( inputLine_[i] );
        break;
      }
      case 19:               // encountered qualifier symbol
      {
        tokens_.back()->MakeNewQualifier();
        errNo = 3;
        break;
      }
      case 20:               // add character to qualifier
      {
        wxString s( inputLine_[i] );
        s.MakeUpper();
        AddToQualifier( s[0] );
        //int tmp = toupper( inputLine_[i] );
        //AddToQualifier( static_cast<char>(tmp) );
        errNo = 0;
        break;
      }
      case 21:               // encountered equal sign
      {
        std::auto_ptr<ParseToken> atoken( new ParseToken );
        ParseToken *token0 = atoken.release();
        tokens_.push_back( token0 );
        if( GetNumberOfTokens() == 1 )
        {
          std::auto_ptr<ParseToken> atoken( new ParseToken );
          ParseToken *token1 = atoken.release();
          tokens_.push_back( token1 );
        }
        //
        // If field 1 is an equal sign, the input line must be an evaluation
        // For consistency, make the first field a null field, putting the
        //  equal sign into the second field
        //
        tokens_.back()->SetType( ParseToken::EQUALSIGN );
        AddToString( '=' );
        errNo = 0;
        break;
      }
      case 22:      // start expression after an equal sign
      {
        std::auto_ptr<ParseToken> atoken( new ParseToken );
        ParseToken *token = atoken.release();
        tokens_.push_back( token );
        tokens_.back()->SetType( ParseToken::STRINGFIELD );
        openBracketCounter = 0;
        errNo = 0;
        if( inputLine_[i] == openingQuote )
        {
          AddToString( inputLine_[i] );
          while ( ++i < len )
          {
            AddToString( inputLine_[i] );
            if( inputLine_[i] == singleQuote )break;
          }
        }
        else if( inputLine_[i] == singleQuote )
        {
          AddToString( inputLine_[i] );
          while ( ++i < len )
          {
            AddToString( inputLine_[i] );
            if( inputLine_[i] == singleQuote )break;
          }
        }
        else if( inputLine_[i] == doubleQuote )
        {
          AddToString( inputLine_[i] );
          while ( ++i < len )
          {
            AddToString( inputLine_[i] );
            if( inputLine_[i] == doubleQuote )break;
          }
        }
        else if( inputLine_[i] == '!' )
        {
          len = i--;
        }
        else
        {
          AddToString( inputLine_[i] );
        }
        break;
      }
      case 23:    // inside expression after an equal sign
      {
        if( inputLine_[i] == openingQuote )
        {
          AddToString( inputLine_[i] );
          while ( ++i < len )
          {
            AddToString( inputLine_[i] );
            if( inputLine_[i] == singleQuote )break;
          }
        }
        else if( inputLine_[i] == singleQuote )
        {
          AddToString( inputLine_[i] );
          while ( ++i < len )
          {
            AddToString( inputLine_[i] );
            if( inputLine_[i] == singleQuote )break;
          }
        }
        else if( inputLine_[i] == doubleQuote )
        {
          AddToString( inputLine_[i] );
          while ( ++i < len )
          {
            AddToString( inputLine_[i] );
            if( inputLine_[i] == doubleQuote )break;
          }
        }
        else if( inputLine_[i] == '!' )
        {
          len = i--;
        }
        else
        {
          AddToString( inputLine_[i] );
        }
        break;
      }
      case 24:    // (qualifier) opening bracket
      {
        ++openBracketCounter;
        errNo = 2;
        AddToQualifier( inputLine_[i] );
        break;
      }
      case 25:    // (qualifier) inside brackets
      {
        wxString s( inputLine_[i] );
        s.MakeUpper();
        AddToQualifier( s[0] );
        //int tmp = toupper( inputLine_[i] );
        //AddToQualifier( static_cast<char>(tmp) );
        break;
      }
      case 26:    // (qualifier) closing bracket
      {
        if( --openBracketCounter == 0 )
        {
          newState = 27;
          errNo = 0;
        }
        AddToQualifier( inputLine_[i] );
        break;
      }
    }
    if( ++i >= len )
    {
      switch ( errNo )
      {
        case 0:
          break;
        case 1:
          throw ESyntaxError( wxT("unbalanced quotes") );
        case 2:
          throw ESyntaxError( wxT("unbalanced brackets") );
        case 3:
          throw ESyntaxError( wxT("null qualifier") );
      }
      break;
    }
    currentState = newState;
    int iascii = toascii( std_input[i] );
    newState = StateTable[currentState][Classes[iascii]];
    if( newState == 100 )
    {
      switch ( errNo )
      {
        case 0:
          break;
        case 1:
          throw ESyntaxError( wxT("unbalanced quotes") );
        case 2:
          throw ESyntaxError( wxT("unbalanced brackets") );
        case 3:
          throw ESyntaxError( wxT("null qualifier") );
      }
      break;
    }
    else if( newState < 0 )
    {
      switch ( newState )
      {
        case -10:
          throw ESyntaxError( wxT("unseperated quote strings") );
        case -11:
          throw ESyntaxError( wxT("opening bracket following quote") );
        case -12:
          throw ESyntaxError( wxT("invalid qualifier") );
        case -13:
          throw ESyntaxError( wxT("closing bracket following quote") );
        case -14:
          throw ESyntaxError( wxT("unbalanced quotes") );
        case -15:
          throw ESyntaxError( wxT("empty quote string") );
        case -16:
          throw ESyntaxError( wxT("quote string in qualifier") );
        case -17:
          throw ESyntaxError( wxT("qualifier on quote string") );
        case -18:
          throw ESyntaxError( wxT("unbalanced brackets") );
        case -19:
          throw ESyntaxError( wxT("unseperated fields") );
        case -20:
          throw ESyntaxError( wxT("equal sign immediately following quote string") );
        case -21:
          throw ESyntaxError( wxT("quote string immediately following valid character") );
        case -22:
          throw ESyntaxError( wxT("expecting qualifier string") );
        case -30:
          throw ESyntaxError( wxT("equal sign inside qualifier") );
      }
      break;
    }
  }
  NumericOrString();
}

// end of file
