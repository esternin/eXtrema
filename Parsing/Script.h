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
#ifndef EX_SCRIPT
#define EX_SCRIPT

#include <vector>

#include "ESyntaxError.h"
#include "ExString.h"
#include "ParseToken.h"

class Script
{
public:
  Script( ExString const &name =ExString(" ") )
      : procedureName_(name), paramCount_(0), currentLine_(0),
        doNumber_(-1), echo_(false), scriptOriginatingPause_(false)
  {}
  
  ~Script()
  { DeleteStuff(); }
  
  Script( Script const &rhs )
  { CopyStuff(rhs); }
  
  Script &operator=( Script const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }

  void AddScriptLine( std::size_t, ExString &, ExString & );
  void SetUpDos();
  bool GetNextLine( ExString & );
  void RemakeInputLine( ExString & );
  
  void ProcessGoTo( ExString const & );
  void ProcessIf();
  bool ProcessDo( ExString const & );

  void SetDoData( std::vector<double> const &data )
  {
    doData_[doNumber_].assign( data.begin(), data.end() );
    doCount_[doNumber_] = 0;
  }

  bool ProcessEnddo( ExString &, double & );
  
  void SetEcho( bool e )
  { echo_ = e; }
  
  static char GetCommentCharacter()
  { return Script::commentCharacter_; }
  
  static void SetCommentCharacter( char const c )
  { Script::commentCharacter_ = c; }
  
  static char GetParameterCharacter()
  { return Script::parameterCharacter_; }
  
  static void SetParameterCharacter( char const c )
  { Script::parameterCharacter_ = c; }
  
  void AddParameter()
  {
    ParseToken *p = new ParseToken();
    p->SetNull();
    parameters_.push_back( p );
  }
  
  void AddParameter( ExString const &s )
  {
    ParseToken *p = new ParseToken();
    p->SetString( s );
    parameters_.push_back( p );
  }
  
  void AddParameter( double d )
  {
    ParseToken *p = new ParseToken();
    p->SetNumeric( d );
    parameters_.push_back( p );
  }
  
  void AddLabel( std::size_t n, ExString const &s )
  {
    labelLines_.push_back( n );
    labels_.push_back( s );
  }

  void AddIf( std::size_t n )
  {
    ifLines_.push_back( n );
    endifLines_.push_back( 0 );
    ifStack_.push_back( ifLines_.size()-1 );
  }
  
  void AddEndif( std::size_t n )
  {
    if( ifStack_.empty() )throw ESyntaxError( "ENDIF without a preceeding IF" );
    endifLines_[ifStack_.back()] = n;
    ifStack_.pop_back();
  }

  void AddDo( std::size_t n )
  {
    doLines_.push_back( n );
    enddoLines_.push_back( 0 );
    doStack_.push_back( doLines_.size()-1 );
  }
  
  void AddEnddo( std::size_t n )
  {
    if( doStack_.empty() )throw ESyntaxError( "ENDDO without a preceeding DO" );
    enddoLines_[doStack_.back()] = n;
    doStack_.pop_back();
  }
  
  std::size_t GetNumberOfDos() const
  { return doLines_.size(); }
  
  std::size_t GetEnddoLine( std::size_t n ) const
  { return enddoLines_[n]; }
  
  std::size_t GetDoLine( std::size_t n ) const
  { return doLines_[n]; }

  ExString Filename();
  ExString CurrentLine();

  void Run();

private:
  static char commentCharacter_;
  static char parameterCharacter_;
  bool echo_;
  //
  std::size_t paramCount_;
  //
  ExString procedureName_; // unused for now
  //
  std::vector<ExString> scriptLines_;
  //
  bool scriptOriginatingPause_;
  std::size_t currentLine_;
  int doNumber_;
  std::vector<bool> inADo_;
  std::vector<ExString> doNames_;
  std::vector< std::vector<double> > doData_;
  std::vector<std::size_t> doCount_;
  //
  std::vector<ParseToken*> parameters_;
  //
  std::vector<ExString> labels_;
  std::vector<std::size_t> labelLines_;
  std::vector<std::size_t> ifLines_;
  std::vector<std::size_t> endifLines_;
  std::vector<std::size_t> ifStack_;
  std::vector<std::size_t> doLines_;
  std::vector<std::size_t> enddoLines_;
  std::vector<std::size_t> doStack_;
  //
  void DeleteStuff();
  void CopyStuff( Script const & );
};

#endif
