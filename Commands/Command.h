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
#ifndef CMD_COMMAND
#define CMD_COMMAND

#include <map>
#include <vector>

#include "ExGlobals.h"
#include "ExString.h"
#include "ParseLine.h"

class Command
{
protected:
  typedef std::map<ExString,bool> QualifierMap;
  typedef QualifierMap::value_type entry_type;
  QualifierMap qualifiers_;
  bool writeStack_;

  Command( char const * );

public:
  virtual ~Command(){}
  bool operator==( Command const & ) const;
  bool operator!=( Command const & ) const;
  //
  virtual void SetInitialize( bool const b ) {} // only used by ExecuteCommand
  virtual void Execute( ParseLine const * ) =0;

  ParseLine const *GetParseLine() const
  { return parseLine_; }

  void AddQualifier( char const *, bool );
  
  void AddName( char const *c )
  { names_.push_back( std::string(c) ); }

  void SetUp( QualifierMap &qualifierEntered )
  { qualifierEntered = qualifierEntered_; }

  void SetUp( ParseLine const * );
  void SetUp( ParseLine const *, QualifierMap & );
  bool IsCommand( ExString const & ) const;

  ExString const &Name() const
  { return names_[0]; }

  ExString const &Name( int i ) const
  { return names_[i]; }

  int NumberOfNames() const
  { return static_cast<int>(names_.size()); }

  void AddToStackLine( ExString const &s )
  { stackLine_ += ExString(" ")+s; }

  void AddToStackLine( double x )
  { stackLine_ += ExString(" ")+ExString(x); }
  
  void AddToStackLine( int x )
  { stackLine_ += ExString(" ")+ExString(x); }

  void AddToStackLine()
  { stackLine_ += ExString(",,"); }

  void AddToStackLineNoBlank( ExString const &s )
  { stackLine_ += s; }

  void AddToStackLineQualifier( ExString const &s )
  { stackLine_ += ExString(1,'\\') + s; }

  ExString &Command::GetStackLine()
  { return stackLine_; }

  void EXStackLine() // only used by EvaluateExpression
  { stackLine_ = parseLine_->GetInputLine(); }

  bool GetWriteStack() const
  { return writeStack_; }

private:
  //
  // cannot call default constructor, copy constructor, or operator=
  //
  Command()
  { exit(1); }
  
  Command( Command const & )
  { exit(1); }

  Command const &operator=( Command const & )
  { exit(1); return *this; }
  //
  ExString stackLine_;
  std::vector<ExString> names_;
  ParseLine const *parseLine_;
  QualifierMap qualifierEntered_;
};

#endif
