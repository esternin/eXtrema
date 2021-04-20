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
#ifndef EX_EXSTRING
#define EX_EXSTRING

#define UTILITIES_LINKAGE_
/*
#ifdef BUILDING_UTILITIES_DLL_
#define UTILITIES_LINKAGE_ __declspec(dllexport)
#else
#define UTILITIES_LINKAGE_ __declspec(dllimport)
#endif
*/

#include <fstream>
#include <sstream>
#include <string>

class UTILITIES_LINKAGE_ ExString
{
public:
  ExString()
  {}
  
  ExString( std::string const &s )
  { string_ = s; }
  
  ExString( char const *c )
  { string_ = std::string(c); }
  
  ExString( char c )
  {
    std::string s(1,c);
    string_ = s;
  }
  
  ExString( std::size_t n, char c )
  {
    std::string s(n,c);
    string_ = s;
  }

  ExString( double d )
  {
    std::stringstream ss;
    ss << d;
    ss >> string_;
  }
  
  ExString( int i )
  {
    std::stringstream ss;
    ss << i;
    ss >> string_;
  }
  
  ExString( std::size_t i )
  {
    std::stringstream ss;
    ss << i;
    ss >> string_;
  }
  
  ExString( double, int, int, int );
  
  ExString( ExString const &rhs )
  { string_ = rhs.string_; }
  
  ExString & operator=( ExString const &rhs )
  {
    if( this != &rhs )string_ = rhs.string_;
    return *this;
  }
  
  bool operator==( ExString const &rhs ) const
  { return (string_ == rhs.string_); }
  
  bool operator!=( ExString const &rhs ) const
  { return (string_ != rhs.string_); }
  
  char & operator[]( std::size_t i )
  { return string_[i]; }
  
  friend bool operator==( ExString const &, char const * );

  bool operator<( ExString const &rhs ) const
  { return (string_ < rhs.string_); }
  
  bool operator>( ExString const &rhs ) const
  { return (string_ > rhs.string_); }
  
  bool operator<=( ExString const &rhs ) const
  { return (string_ <= rhs.string_); }
  
  bool operator>=( ExString const &rhs ) const
  { return (string_ >= rhs.string_); }
  
  friend ExString operator+( char const *, ExString const & );
  friend ExString operator+( ExString const &, char const * );
  friend ExString operator+( char const *, std::string const & );
  friend ExString operator+( std::string const &, char const * );
  friend ExString operator+( ExString const &, std::string const & );
  friend ExString operator+( std::string const &, ExString const & );

  ExString operator+( ExString const &rhs ) const
  { return ExString(string_+rhs.string_); }

  ExString operator+( std::string const &rhs ) const
  { return ExString(string_+rhs); }

  ExString &operator+=( ExString const &rhs )
  { string_ += rhs.string_; return *this; }

  ExString &operator+=( std::string const &rhs )
  { string_ += rhs; return *this; }

  ExString &operator+=( char const *rhs )
  { string_ += std::string(rhs); return *this; }

  char &at( std::size_t i )
  { return string_.at(i); }
        
  char const *c_str() const
  { return string_.c_str(); }

  std::size_t npos() const
  { return string_.npos; }
  
  std::size_t length() const
  { return string_.length(); }
  
  std::size_t size() const
  { return string_.size(); }
  
  bool empty() const
  { return string_.empty(); }
  
  ExString &erase( std::size_t pos=0, int n=-1 )
  {
    if( n == -1 )n=string_.npos;
    string_.erase( pos, n );
    return *this;
  }
  
  void clear()
  { string_.erase( 0, string_.npos ); }
  
  ExString &append( ExString const &s, std::size_t pos, std::size_t n )
  {
    string_.append( s.string_, pos, n );
    return *this;
  }
  
  ExString &append( ExString const &s )
  {
    string_.append( s.string_ );
    return *this;
  }
  
  ExString &append( char const *c, std::size_t n )
  {
    string_.append( c, n );
    return *this;
  }
  
  ExString &append( char const *c )
  {
    string_.append( c );
    return *this;
  }
  
  ExString &append( char c )
  {
    string_.append( 1, c );
    return *this;
  }
  
  ExString &append( std::size_t n, char c )
  {
    string_.append( n, c );
    return *this;
  }

  ExString &assign( ExString const &s )
  {
    string_.assign(s.string_);
    return *this;
  }
  
  ExString &assign( ExString const &s, std::size_t pos, std::size_t n )
  {
    string_.assign(s.string_,pos,n);
    return *this;
  }
  
  ExString &assign( char const *s, std::size_t n )
  {
    string_.assign(s,n);
    return *this;
  }
  
  ExString &assign( char const *s )
  {
    string_.assign(s);
    return *this;
  }
  
  ExString &assign( std::size_t n, char c )
  {
    string_.assign(n,c);
    return *this;
  }
  
  std::size_t find( ExString const &s, std::size_t pos=0 ) const
  { return string_.find( s.string_, pos ); }
  
  std::size_t find( char const *c, std::size_t pos, std::size_t n ) const
  { return string_.find( c, pos, n ); }
  
  std::size_t find( char const *c, std::size_t pos=0 ) const
  { return string_.find( c, pos ); }
  
  std::size_t find( char c, std::size_t pos=0 ) const
  { return string_.find( c, pos ); }
  
  std::size_t find_first_not_of( ExString const &s, std::size_t pos=0 ) const
  { return string_.find_first_not_of( s.string_, pos ); }
  
  std::size_t find_first_not_of( char const *c, std::size_t pos=0 ) const
  { return string_.find_first_not_of( c, pos ); }
  
  std::size_t find_first_not_of( char const *c, std::size_t pos, std::size_t n ) const
  { return string_.find_first_not_of( c, pos, n ); }
  
  std::size_t find_first_not_of( char c, std::size_t pos=0 ) const
  { return string_.find_first_not_of( c, pos ); }
  
  std::size_t find_first_of( ExString &s, std::size_t pos=0 ) const
  { return string_.find_first_of( s.string_, pos ); }
  
  std::size_t find_first_of( char const *c, std::size_t pos=0 ) const
  { return string_.find_first_of( c, pos ); }

  std::size_t find_first_of( char const *c, std::size_t pos, std::size_t n ) const
  { return string_.find_first_of( c, pos, n ); }
  
  std::size_t find_first_of( char c, std::size_t pos=0 ) const
  { return string_.find_first_of( c, pos ); }
  
  std::size_t find_last_not_of( ExString const &s ) const
  { return string_.find_last_not_of( s.string_, string_.npos ); }

  std::size_t find_last_not_of( ExString const &s, std::size_t pos ) const
  { return string_.find_last_not_of( s.string_, pos ); }

  std::size_t find_last_not_of( char const *c ) const
  { return string_.find_last_not_of( c, string_.npos ); }

  std::size_t find_last_not_of( char const *c, std::size_t pos ) const
  { return string_.find_last_not_of( c, pos ); }

  std::size_t find_last_not_of( char const *c, std::size_t pos, std::size_t n ) const
  { return string_.find_last_not_of( c, pos, n ); }

  std::size_t find_last_not_of( char c ) const
  { return string_.find_last_not_of( c, string_.npos ); }

  std::size_t find_last_not_of( char c, std::size_t pos ) const
  { return string_.find_last_not_of( c, pos ); }

  std::size_t find_last_of( ExString const &s ) const
  { return string_.find_last_of( s.string_, string_.npos ); }

  std::size_t find_last_of( ExString const &s, std::size_t pos ) const
  { return string_.find_last_of( s.string_, pos ); }

  std::size_t find_last_of( char const *c ) const
  { return string_.find_last_of( c, string_.npos ); }

  std::size_t find_last_of( char const *c, std::size_t pos ) const
  { return string_.find_last_of( c, pos ); }

  std::size_t find_last_of( char const *c, std::size_t pos, std::size_t n ) const
  { return string_.find_last_of( c, pos, n ); }

  std::size_t find_last_of( char c ) const
  { return string_.find_last_of( c, string_.npos ); }

  std::size_t find_last_of( char c, std::size_t pos ) const
  { return string_.find_last_of( c, pos ); }

  ExString &insert( std::size_t pos, ExString const &s )
  {
    string_.insert( pos, s.string_ );
    return *this;
  }

  ExString &insert( std::size_t pos, ExString const &s, std::size_t pos2=0, std::size_t n=-1 )
  {
    if( n == -1 )n=string_.npos;
    string_.insert( pos, s.string_, pos2, n );
    return *this;
  }

  ExString &insert( std::size_t pos, char const *c, std::size_t n )
  {
    string_.insert( pos, c, n );
    return *this;
  }
  
  ExString &insert( std::size_t pos, char const *c )
  {
    string_.insert( pos, c );
    return *this;
  }
  
  ExString &insert( std::size_t pos, std::size_t n, char c )
  {
    string_.insert( pos, n, c );
    return *this;
  }
  
  ExString &replace( std::size_t pos, std::size_t n, ExString const &s )
  {
    string_.replace( pos, n, s.string_ );
    return *this;
  }
  
  ExString &replace( std::size_t pos1, std::size_t n1, ExString const &s,
                     std::size_t pos2, std::size_t n2 )
  {
    string_.replace( pos1, n1, s.string_, pos2, n2 );
    return *this;
  }
  
  ExString &replace( std::size_t pos, std::size_t n1, char const *c, std::size_t n2 )
  {
    string_.replace( pos, n1, c, n2 );
    return *this;
  }
  
  ExString &replace( std::size_t pos, std::size_t n, char const *c )
  {
    string_.replace( pos, n, c );
    return *this;
  }
  
  ExString &replace( std::size_t pos, std::size_t n1, std::size_t n2, char c )
  {
    string_.replace( pos, n1, n2, c );
    return *this;
  }
  
  void resize( std::size_t n, char c )
  { string_.resize( n, c ); }
  
  void resize( std::size_t n )
  { string_.resize( n ); }
  
  ExString substr( std::size_t pos=0, std::size_t n=-1 ) const
  {
    if( n == -1 )n=string_.npos;
    return ExString( string_.substr(pos,n) );
  }

  ExString LowerCase() const;
  ExString UpperCase() const;
  ExString ToggleCase() const;
  ExString Capitalize() const;
  void ToLower();
  void ToUpper();
  void Toggle();
  void ToCapitalized();
  //
  double ToDouble() const;
  int ToInt() const;
  bool ToBool() const;
  int HexToInt() const;
  //
  // static member functions do not have a this pointer so can be called
  // without reference to a class object
  //
  static ExString IntToHex( int );
  static ExString GetNiceNumber( double );
  static ExString GetNiceNumber( double, int, int );

  void RemoveBlanks()
  {
    std::size_t i;
    while ( (i=string_.find(' ')) != string_.npos )
     string_.erase( string_.begin()+i );
  }

  void RemoveQuotes()
  {
    if( string_.length()>=2 &&
       (string_[0]=='`' || string_[0]=='\'' || string_[0]=='"') )
      string_.assign( string_.substr(1,string_.length()-2) );
  }

  void LeftJustify()
  {
    std::size_t i = string_.find_first_not_of( ' ', 0 );
    if( i > 0 )string_.erase( 0, i );
  }
  
  ExString ExtractFileExt() const;
  ExString ExtractFilePath() const;
  ExString ExtractFileName() const;

  void Dump( std::ofstream &out ) const
  { if( string_.size() > 0 )out << "<![CDATA[" << string_.c_str() << "]]>\n"; }

  friend std::ostream &operator<<( std::ostream &out, ExString const &s )
  { return out << s.string_; }

  ExString Encode() const;
private:
  std::string string_;
};
#endif

