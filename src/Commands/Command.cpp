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
#include "Command.h"
#include "ECommandError.h"
#include "CommandTable.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"

Command::Command( wxChar const *c )
{
  wxString s( c );
  names_.push_back( s );
  stackLine_ = s;
  //
  // this variable allows for interactive TEXT and the STACK command to
  // not be stacked even when STACK is on
  //
  writeStack_ = true;
  CommandTable::GetTable()->AddEntry( this );
}

void Command::AddQualifier( wxChar const *c, bool b )
{
  wxString s( c );
  qualifiers_.insert(entry_type(s,b));
  qualifierEntered_.insert(entry_type(s,false) );
}

void Command::SetUp( ParseLine const *p )
{
  parseLine_ = p;
  stackLine_.erase();
  stackLine_ = names_[0];
}

void Command::SetUp( ParseLine const *p, QualifierMap &qualifiers )
{
  parseLine_ = p;
  stackLine_.erase();
  stackLine_ = names_[0];
  //
  // make a copy of the fixed qualifiers and default bool values
  // so the bool values can be altered
  //
  qualifiers = qualifiers_;
  QualifierMap::iterator qEnd = qualifiers.end();
  //
  wxString const backSlash(1,wxT('\\'));
  std::vector<wxString*> currentQualifiers( p->GetToken(0)->GetQualifiers() );
  std::size_t qsSize = currentQualifiers.size();
  for( std::size_t i=0; i<qsSize; ++i )
  {
    wxString qString( *currentQualifiers[i] );
    bool negate = false;
    if( qString[0] == '-' )
    {
      negate = true;
      qString.erase(0,1);
    }
    else if( qString.substr(0,2) == wxString(wxT("NO")) )
    {
      negate = true;
      qString.erase(0,2);
    }
    bool notFound = true;
    for( QualifierMap::iterator j=qualifiers.begin(); j!=qEnd; ++j )
    {
      wxString q( j->first );
      if( q.find(qString) == 0 )
      {
        j->second = !negate;
        stackLine_ += backSlash + qString;
        notFound = false;
        QualifierMap::iterator qe( qualifierEntered_.find(q) );
        qe->second = true;
        break;
      }
    }
    if( notFound )
      throw ECommandError( names_[0]+wxT(": unknown qualifier: ")+qString );
  }
}

bool Command::IsCommand( wxString const &s ) const
{
  std::size_t nEnd = names_.size();
  for( std::size_t i=0; i<nEnd; ++i )
  {
    if( names_[i].find(s) == 0 )return true;
  }
  return false;
}

void Command::AddToStackLine( wxString const &s )
{ stackLine_ += wxString(wxT(" "))+s; }

void Command::AddToStackLine( double x )
{ stackLine_ << wxT(" ") << x; }

void Command::AddToStackLine( int x )
{ stackLine_ << wxT(" ") << x; }

void Command::AddToStackLine()
{ stackLine_ << wxT(",,"); }

void Command::AddToStackLineNoBlank( wxString const &s )
{ stackLine_ += s; }

void Command::AddToStackLineQualifier( wxString const &s )
{ stackLine_ += wxString(1,wxT('\\')) + s; }

void Command::WriteStackLine()
{ ExGlobals::WriteStack( stackLine_ ); }

void Command::EXStackLine() // only used by EvaluateExpression
{ stackLine_ = parseLine_->GetInputLine(); }

// end of file
