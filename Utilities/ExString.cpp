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

#include "ExString.h"

ExString::ExString( double r, int power, int nPos, int nDec )
{
  string_ = " ";
  if( nPos > 0 )
  {
    std::stringstream ss;
    ss << setw(nPos) << setprecision(nDec) << r/pow(10,power);
    ss >> string_;
  }
}
  
bool operator==( ExString const &exstring, char const *cstring )
{ return (exstring.string_ == std::string(cstring)); }

ExString operator+( char const *c, ExString const &rhs )
{ return ExString(std::string(c)+rhs.string_); }

ExString operator+( ExString const &rhs, char const *c )
{ return ExString(rhs.string_+std::string(c)); }

ExString operator+( char const *c, std::string const &rhs )
{ return ExString(std::string(c)+rhs); }

ExString operator+( std::string const &rhs, char const *c )
{ return ExString(rhs+std::string(c)); }

ExString operator+( ExString const &s, std::string const &rhs )
{ return ExString(s.string_+rhs); }

ExString operator+( std::string const &rhs, ExString const &s )
{ return ExString(rhs+s.string_); }

ExString ExString::UpperCase() const
{
  std::size_t n = string_.size();
  char *c = new char[n+1];
  strcpy( c, string_.c_str() );
  for( std::size_t i=0; i<n; ++i )c[i] = char(toupper(c[i]));
  ExString s( c );
  delete [] c;
  return s;
  // std::transform( string_.begin(), string_.end(), string_.begin(), std::toupper );
}

void ExString::ToUpper()
{
  std::size_t n = string_.size();
  char *c = new char[n+1];
  strcpy( c, string_.c_str() );
  for( std::size_t i=0; i<n; ++i )c[i] = char(toupper(c[i]));
  string_ = c;
  delete [] c;
}

ExString ExString::LowerCase() const
{
  std::size_t n = string_.size();
  char *c = new char[n+1];
  strcpy( c, string_.c_str() );
  for( std::size_t i=0; i<n; ++i )c[i] = char(tolower(c[i]));
  ExString s( c );
  delete [] c;
  return s;
}

void ExString::ToLower()
{
  std::size_t n = string_.size();
  char *c = new char[n+1];
  strcpy( c, string_.c_str() );
  for( std::size_t i=0; i<n; ++i )c[i] = char(tolower(c[i]));
  string_ = c;
  delete [] c;
}

ExString ExString::Capitalize() const
{
  std::size_t n = string_.size();
  char *c = new char[n+1];
  strcpy( c, string_.c_str() );
  c[0] = char(toupper(c[0]));
  for( std::size_t i=1; i<n; ++i )c[i] = char(tolower(c[i]));
  ExString s( c );
  delete [] c;
  return s;
}

void ExString::ToCapitalized()
{
  std::size_t n = string_.size();
  char *c = new char[n+1];
  strcpy( c, string_.c_str() );
  c[0] = char(toupper(c[0]));
  for( std::size_t i=1; i<n; ++i )c[i] = char(tolower(c[i]));
  string_ = c;
  delete [] c;
}

ExString ExString::ToggleCase() const
{
  std::size_t n = string_.size();
  char *c = new char[n+1];
  strcpy( c, string_.c_str() );
  for( std::size_t i=0; i<n; ++i )
  {
    int j = static_cast<int>(c[i]);
    if( j>64 && j<91 )
    {
      c[i] = static_cast<char>(j+32);
    }
    else if( j>96 && j<123 )
    {
      c[i] = static_cast<char>(j-32);
    }
  }
  ExString s( c );
  delete [] c;
  return s;
}

void ExString::Toggle()
{
  std::size_t n = string_.size();
  char *c = new char[n+1];
  strcpy( c, string_.c_str() );
  for( std::size_t i=0; i<n; ++i )
  {
    int j = static_cast<int>(c[i]);
    if( j>64 && j<91 )
    {
      c[i] = static_cast<char>(j+32);
    }
    else if( j>96 && j<123 )
    {
      c[i] = static_cast<char>(j-32);
    }
  }
  string_ = c;
  delete [] c;
}

double ExString::ToDouble() const
{
  double result;
  if( std::istringstream(string_) >> result )return result;
  throw runtime_error("Error converting string to double");
}

int ExString::ToInt() const
{
  int result;
  if( std::istringstream(string_) >> result )return result;
  throw runtime_error("Error converting string to int");
}

bool ExString::ToBool() const
{
  bool result;
  if( std::istringstream(string_) >> result )return result;
  throw runtime_error("Error converting string to bool");
}

ExString ExString::IntToHex( int n )
{
  if( n > 255 )throw runtime_error("max value for IntToHex is 255");
  if( n < 0 )throw runtime_error("min value for IntToHex is 0");
  std::string sout("00");
  std::string const alphabet("0123456789ABCDEF");
  if( n >= 16 )
  {
    int i = n%16;
    sout[1] = alphabet[i];
    n = (n-i)/16;
    sout[0] = alphabet[n];
  }
  else
  {
    sout[1] = alphabet[n];
  }
  return ExString(sout);
}

ExString ExString::GetNiceNumber( double d )
{
  int width = 10;
  int precision = 3;
  std::stringstream ss;
  std::string s;
  if( fabs(d)>=1000.0 || fabs(d)<=0.01 )
  {
    ss << setw(width) << setprecision(precision) << scientific << d;
  }
  else
  {
    if( fabs(d) < 10.0 )
    {
      width = 6;
      precision = 3;
    }
    else if( fabs(d) < 100.0 )
    {
      width = 6;
      precision = 2;
    }
    else if( fabs(d) < 1000.0 )
    {
      width = 6;
      precision = 1;
    }
    ss << setw(width) << setprecision(precision) << fixed << d;
  }
  ss >> s;
  if( d >= 0.0 )return ExString(" ") + ExString(s);
  return ExString(s);
}

ExString ExString::GetNiceNumber( double d, int width, int precision )
{
  if( d == 0.0 )return ExString("0.0");
  std::stringstream ss;
  ss << setw(width) << setprecision(precision);
  std::string s;
  if( fabs(d)>=1000.0 || fabs(d)<=0.01 )ss << scientific << d;
  else                                  ss << fixed << d;
  ss >> s;
  if( d >= 0.0 )return ExString(" ") + ExString(s);
  return ExString(s);
}

int ExString::HexToInt() const
{
  std::string const alphabet("0123456789ABCDEF");
  std::size_t i = alphabet.find(string_.substr(0,1),0);
  if( i == alphabet.npos )i = 0;
  std::size_t j = alphabet.find(string_.substr(1,1),0);
  if( j == alphabet.npos )j = 0;
  return i*16+j;
}

ExString ExString::ExtractFileExt() const
{
  std::size_t len = string_.size();
  std::size_t i = string_.find_last_of( '.' );
  if( i == string_.npos )return ExString("");
  else                   return ExString( string_.substr(i,len-i) );
}

ExString ExString::ExtractFileName() const
{
  std::size_t i = string_.find_last_of( '.' );
  if( i == string_.npos )return ExString(string_);
  else                   return ExString( string_.substr(0,i) );
}

ExString ExString::ExtractFilePath() const
{
#ifdef __BORLANDC__
  std::size_t i = string_.find_last_of( '\\' );
#elif
  std::size_t i = string_.find_last_of( '/' );
#endif
  if( i == string_.npos )return ExString("");
  else                   return ExString( string_.substr(0,i) );
}

ExString ExString::Encode() const
{
  ExString result;
  std::size_t length = string_.size();
  for( std::size_t i=0; i<length; ++i )
  {
    switch ( string_.at(i) )
    {
      case '<':
        result += "&lt;";
        break;
      case '>':
        result += "&gt;";
        break;
      case '&':
        result += "&amp;";
        break;
      case '\"':
        result += "&quot;";
        break;
      case '\'':
        result += "&apos;";
        break;
      default:
        result += string_.at(i);
    }
  }
  return result;
}
// end of file
