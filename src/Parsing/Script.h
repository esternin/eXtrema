/*
Copyright (C) 2005,...,2007 Joseph L. Chuma, TRIUMF

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

#include "wx/wx.h"

class ParseToken;

class Script
{
public:
  Script( wxString const &name =wxString(wxT(" ")) )
      : procedureName_(name), paramCount_(0), currentLine_(0), doNumber_(-1),
        echo_(false), scriptOriginatingPause_(false)
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

  void AddScriptLine( std::size_t, wxString &, wxString & );
  void SetUpDos();
  bool GetNextLine( wxString & );
  void RemakeInputLine( wxString & );
  
  void ProcessGoTo( wxString const & );
  void ProcessIf();
  bool ProcessDo( wxString const & );

  void SetDoData( std::vector<double> const &data )
  {
    doData_[doNumber_].assign( data.begin(), data.end() );
    doCount_[doNumber_] = 0;
  }

  bool ProcessEnddo( wxString &, double & );
  
  void SetEcho( bool e )
  { echo_ = e; }
  
  static wxChar GetCommentCharacter()
  { return Script::commentCharacter_; }
  
  static void SetCommentCharacter( wxChar const c )
  { Script::commentCharacter_ = c; }
  
  static wxChar GetParameterCharacter()
  { return Script::parameterCharacter_; }
  
  static void SetParameterCharacter( wxChar const c )
  { Script::parameterCharacter_ = c; }
  
  void AddParameter();
  void AddParameter( wxString const & );
  void AddParameter( double );
  
  void AddLabel( std::size_t n, wxString const &s )
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
  
  void AddEndif( std::size_t );

  void AddDo( std::size_t n )
  {
    doLines_.push_back( n );
    enddoLines_.push_back( 0 );
    doStack_.push_back( doLines_.size()-1 );
  }
  
  void AddEnddo( std::size_t );
  
  std::size_t GetNumberOfDos() const
  { return doLines_.size(); }
  
  std::size_t GetEnddoLine( std::size_t n ) const
  { return enddoLines_[n]; }
  
  std::size_t GetDoLine( std::size_t n ) const
  { return doLines_[n]; }

  wxString Filename();
  wxString CurrentLine();

  void Run();

private:
  static wxChar commentCharacter_;
  static wxChar parameterCharacter_;
  bool echo_;
  //
  std::size_t paramCount_;
  //
  wxString procedureName_; // unused for now
  //
  std::vector<wxString> scriptLines_;
  //
  bool scriptOriginatingPause_;
  std::size_t currentLine_;
  int doNumber_;
  std::vector<bool> inADo_;
  std::vector<wxString> doNames_;
  std::vector< std::vector<double> > doData_;
  std::vector<std::size_t> doCount_;
  //
  std::vector<ParseToken*> parameters_;
  //
  std::vector<wxString> labels_;
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
