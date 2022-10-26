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
#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "Script.h"
#include "ESyntaxError.h"
#include "ParseToken.h"
#include "ExGlobals.h"
#include "ToAscii.h"

wxChar Script::commentCharacter_ = wxT('!');
wxChar Script::parameterCharacter_ = wxT('?');

void Script::Run()
{
  for( ;; )
  {
    wxString commandLine;
    if( ExGlobals::GetRestartingScript() )
    {
      if( scriptOriginatingPause_ )
      {
        ExGlobals::SetRestartingScript( false );
        scriptOriginatingPause_ = false;
        if( GetNextLine(commandLine) )return; // end of script file found
        if( echo_ )ExGlobals::WriteOutput( commandLine );
      }
      else
      {
        ExGlobals::IncrementScript();
        commandLine.clear();
        ExGlobals::SetExecuteCommand( true );
      }
    }
    else if( ExGlobals::GetPausingScript() )
    {
      return;
    }
    else
    {
      if( GetNextLine(commandLine) )return; // end of script file found
      if( echo_ )ExGlobals::WriteOutput( commandLine );
    }
    if( !commandLine.empty() && commandLine!=wxString(wxT(" ")) )
    {
      ExGlobals::ProcessCommand(commandLine);

      // Note that calling PauseScripts() rather unexpectedly results in
      // NotInaScript() returning true, but we do need to continue running this
      // function in this case, if only to set scriptOriginatingPause_ to true.
      if ( ExGlobals::NotInaScript() && !ExGlobals::GetPausingScript() )
      {
        // This can only happen if StopAllScripts() was called from inside the
        // command, in which case this script object itself doesn't exist any
        // longer and we must not use it any more, so bail out immediately.
        return;
      }
    }

    if( ExGlobals::GetExecuteCommand() )
    {
      ExGlobals::SetExecuteCommand( false );
      ExGlobals::RunScript();
    }
    else if( ExGlobals::GetReturnCommand() )
    {
      ExGlobals::SetReturnCommand( false );
      return;
    }
    else if( ExGlobals::GetPausingScript() )
    {
      scriptOriginatingPause_ = true;
      return;
    }
  }
}

wxString Script::Filename()
{
  return parameters_[0]->GetString();
}

wxString Script::CurrentLine()
{
  return scriptLines_[currentLine_-1];
}

void Script::DeleteStuff()
{
  std::vector<ParseToken*>::const_iterator end = parameters_.end();
  for( std::vector<ParseToken*>::const_iterator i=parameters_.begin(); i!=end; ++i )delete *i;
  std::vector<ParseToken*>().swap( parameters_ );
}

void Script::CopyStuff( Script const &rhs )
{
  DeleteStuff();
  std::vector<ParseToken*>::const_iterator end = rhs.parameters_.end();
  for( std::vector<ParseToken*>::const_iterator i=rhs.parameters_.begin(); i!=end; ++i )
  {
    ParseToken *pt = new ParseToken();
    *pt = **i;
  }
  //
  echo_ = rhs.echo_;
  //
  paramCount_ = rhs.paramCount_;
  procedureName_ = rhs.procedureName_; // unused for now
  scriptLines_.assign( rhs.scriptLines_.begin(), rhs.scriptLines_.end() );
  currentLine_ = currentLine_;
  scriptOriginatingPause_ = rhs.scriptOriginatingPause_;
  doNumber_ = rhs.doNumber_;
  inADo_.assign( rhs.inADo_.begin(), rhs.inADo_.end() );
  doNames_.assign( rhs.doNames_.begin(), rhs.doNames_.end() );
  //
  std::vector< std::vector<double> >().swap( doData_ );
  std::size_t size = rhs.doData_.size();
  for( std::size_t i=0; i<size; ++i )doData_.push_back( rhs.doData_[i] );
  //
  doCount_ = rhs.doCount_;
  labels_.assign( rhs.labels_.begin(), rhs.labels_.end() );
  labelLines_.assign( rhs.labelLines_.begin(), rhs.labelLines_.end() );
  ifLines_.assign( rhs.ifLines_.begin(), rhs.ifLines_.end() );
  endifLines_.assign( rhs.endifLines_.begin(), rhs.endifLines_.end() );
  ifStack_.assign( rhs.ifStack_.begin(), rhs.ifStack_.end() );
  doLines_.assign( rhs.doLines_.begin(), rhs.doLines_.end() );
  enddoLines_.assign( rhs.enddoLines_.begin(), rhs.enddoLines_.end() );
  doStack_.assign( rhs.doStack_.begin(), rhs.doStack_.end() );
}

void Script::AddScriptLine( std::size_t numberOfParameters, wxString &fname, wxString &line )
{
  wxChar const openingQuote = 96;
  wxChar const singleQuote = 39;
  wxChar const doubleQuote = 34;
  //
  // strip off comments
  //
  std::size_t lineLength = line.size();
  std::size_t i = 0;
  while( i < lineLength )
  {
    if( line[i] == commentCharacter_ )
    {
      if( i == 0 )return;
      line = line.substr( 0, i );
      std::size_t rTemp = line.find_last_not_of( wxT(' ') ); // trim trailing blanks
      if( rTemp == line.npos )return;
      line = line.substr( 0, rTemp+1 );
      lineLength = line.size();
    }
    else if( line[i] == openingQuote )
    {
      while( ++i < lineLength )
      {
        if( line[i] == singleQuote )break;
      }
    }
    else if( line[i] == singleQuote )
    {
      while( ++i < lineLength )
      {
        if( line[i] == singleQuote )break;
      }
    }
    else if( line[i] == doubleQuote )
    {
      while( ++i < lineLength )
      {
        if( line[i] == doubleQuote )break;
      }
    }
    ++i;
  }
  wxString uLine( line.Upper() ); // convert line to uppercase uLine
  //
  if( uLine[lineLength-1] == wxT(':') ) // found a label, e.g.,  LABEL:
  {
    if( uLine.find(wxT(' ')) != uLine.npos )
      throw ESyntaxError(wxT("blanks are not allowed in a script label"));
    AddLabel( scriptLines_.size(), uLine.substr(0,lineLength-1) );
  }
  else if( uLine.substr(0,3) == wxT("IF ") )
  {
    // only record the ENDIF when in an if block, not a single statement
    //
    if( uLine.substr(lineLength-4,4) == wxT("THEN") )AddIf( scriptLines_.size() );
  }
  else if( uLine.substr(0,5) == wxT("ENDIF") )
  {
    AddEndif( scriptLines_.size() );
  }
  else if( uLine.substr(0,3) == wxT("DO ") )
  {
    AddDo( scriptLines_.size() );
  }
  else if( uLine.substr(0,5) == wxT("ENDDO") )
  {
    AddEnddo( scriptLines_.size() );
  }
  i = 0;
  while( i < lineLength )
  {
    if( line[i] == openingQuote )
    {
      while ( ++i < lineLength )
      {
        if( line[i] == singleQuote )break;
      }
    }
    else if( line[i] == singleQuote )
    {
      while ( ++i < lineLength )
      {
        if( line[i] == singleQuote )break;
      }
    }
    else if( line[i] == doubleQuote )
    {
      while ( ++i < lineLength )
      {
        if( line[i] == doubleQuote )break;
      }
    }
    else if( line[i] == parameterCharacter_ )
    {
      bool missingParameter = false;
      std::size_t ipindex, npl, npn;
      std::string lineTmp( line.mb_str(wxConvUTF8) );
      if( i+1==lineLength || toascii(lineTmp[i+1])<48 || toascii(lineTmp[i+1])>57 )
      {
        ipindex = ++paramCount_;
        if( paramCount_ > numberOfParameters )
        {
          missingParameter = true;
          ++numberOfParameters;
        }
        npl = i;
        npn = 1;
      }
      else            // numbered parameter substitution
      {
        npl = i;
        wxString s;
        s.append( line[++i] );
        for( ;; )
        {
          int ival = i+1<lineLength ? toascii(lineTmp[i+1]) : 0;
          if( ival<48 || ival>57 )break;
          s.append( line[++i] );
        }
        double x;
        std::stringstream ss;
        ss << s.mb_str(wxConvUTF8);
        ss >> x;
        //
        npn = i - npl + 1;
        //
        ipindex = static_cast<int>(x);
        if ( ipindex > numberOfParameters )
        {
          missingParameter = true;
          numberOfParameters = ipindex;
        }
      }
      if ( missingParameter )
      {
        wxChar c[100];
        ::wxSnprintf( c, 100, wxT("parameter %zd missing from file: %s"), ipindex, fname.c_str() );
        throw ESyntaxError( c );
      }
      wxString s1;
      if ( parameters_[ipindex]->IsaNull() )
      {
        s1 = wxT(",,");
      }
      else if ( parameters_[ipindex]->IsaNumeric() )
      {
        wxChar c[50];
        ::wxSnprintf( c, 50, wxT("%g"), parameters_[ipindex]->GetNumeric() );
        s1.assign( c );
      }
      else // must be string type
      {
        s1 = parameters_[ipindex]->GetString();
      }
      line.replace( npl, npn, s1 ); // substitution done here
      i = npl + s1.length();
      lineLength = line.size();
    }
    ++i;
  }
  scriptLines_.push_back( line );
}
 
void Script::SetUpDos()
{
  std::size_t n = doLines_.size();
  inADo_.assign( n, false );
  doNames_.resize( n, wxString(wxT(" ")) );
  doCount_.resize( n, 0 );
  doData_.resize( n );
}

bool Script::GetNextLine( wxString &line )
{
  line.erase();
  bool getLine = true;
  while (getLine)
  {
    getLine = false;
    if( currentLine_ == scriptLines_.size() )
    {
      line.erase();
      return true;
    }
    line = scriptLines_[currentLine_];
    std::vector<std::size_t>::iterator vi =
        find(labelLines_.begin(),labelLines_.end(),currentLine_++);
    if( vi != labelLines_.end() )getLine = true;
  }
  RemakeInputLine(line);
  return false;
}
 
void Script::RemakeInputLine( wxString &line )
{
  if( line.find(parameterCharacter_) == line.npos )return;
  //
  wxString cline;
  wxChar const openingQuote = 96;
  wxChar const singleQuote = 39;
  wxChar const doubleQuote = 34;
  //
  std::size_t parameterCount = 0;
  std::size_t lineLen = line.length();
  std::size_t i = 0;
  while ( i < lineLen )
  {
    if( line[i] == openingQuote )
    {
      cline.append( openingQuote );
      while ( ++i < lineLen )
      {
        cline.append( line[i] );
        if( line[i] == singleQuote )break;
      }
    }
    else if( line[i] == singleQuote )
    {
      cline.append( singleQuote );
      while ( ++i < lineLen )
      {
        cline.append( line[i] );
        if( line[i] == singleQuote )break;
      }
    }
    else if( line[i] == doubleQuote )
    {
      cline.append( doubleQuote );
      while ( ++i < lineLen )
      {
        cline.append( line[i] );
        if( line[i] == doubleQuote )break;
      }
    }
    else if( line[i] == commentCharacter_ )
    {
      return;
    }
    else if( line[i] == parameterCharacter_ )
    {
      int ival = i+1<lineLen ? TryConvertToAscii(line[i+1]) : 0;
      if( ival<48 || ival>57 )
      {
        // sequential parameter substitution
        //
        ParseToken *pt = parameters_[parameterCount];
        if( pt->IsaNumeric() )
        {
          wxChar c[50];
          ::wxSnprintf( c, 50, wxT("%g"), pt->GetNumeric() );
          cline.append( c );
        }
        else if( pt->IsaString() )
        {
          cline += pt->GetString();
        }
        ++parameterCount;
      }
      else // numbered parameter substitution
      {
        wxString s;
        s.append( line[++i] );
        for( ;; )
        {
          ival = i+1<lineLen ? TryConvertToAscii(line[i+1]) : 0;
          if( ival<48 || ival>57 )break;
          s.append( line[++i] );
        }
        double xtemp;
        std::stringstream ss;
        ss << s.mb_str(wxConvUTF8);
        ss >> xtemp;
        //
        parameterCount = static_cast<int>( xtemp );
        ParseToken *pt = parameters_[parameterCount];
        if( pt->IsaNumeric() )
        {
          wxChar c[50];
          ::wxSnprintf( c, 50, wxT("%g"), pt->GetNumeric() );
          cline += wxString( c );
        }
        else if( pt->IsaString() )
        {
          cline += pt->GetString();
        }
      }
    }
    else // character is nothing special
    {
      cline.append( line[i] );
    }
    ++i;
  }
  line = cline;
  return;
}
 
void Script::ProcessGoTo( wxString const &label )
{
  bool found = false;
  std::size_t end = labels_.size();
  for( std::size_t i=0; i<end; ++i )
  {
    if( label == labels_[i] )
    {
      found = true;
      currentLine_ = labelLines_[i]+1;
    }
  }
  if( !found )throw ESyntaxError(wxT("GOTO label not found"));
  std::size_t doSize = doStack_.size();
  for( std::size_t i=0; i<doSize; ++i )
  {
    std::size_t j = doStack_[doSize-1-i];
    if( currentLine_>enddoLines_[j] || currentLine_<doLines_[j] )
    {
      inADo_[j] = false;
      std::vector<double>().swap( doData_[j] );
      doStack_.pop_back();
    }
  }
  doStack_.empty() ? doNumber_=-1 : doNumber_=doStack_.back();
}

void Script::ProcessIf()
{
  std::size_t end = ifLines_.size();
  for( std::size_t jj=0; jj<end; ++jj )
  {
    if( currentLine_-1 == ifLines_[jj] )
    {
      currentLine_ = endifLines_[jj];
      return;
    }
  }
  throw ESyntaxError(wxT("could not find ENDIF"));
}

bool Script::ProcessDo( wxString const &name )
{
  bool foundIt = false;
  std::size_t end = doLines_.size();
  for( std::size_t jj=0; jj<end; ++jj )
  {
    if( currentLine_-1 == doLines_[jj] )
    {
      doNumber_ = jj;
      foundIt = true;
      break;
    }
  }
  if( !foundIt )throw ESyntaxError(wxT("could not find the DO line") );
  if( inADo_[doNumber_] )return false;
  //
  doNames_[doNumber_] = name;
  inADo_[doNumber_] = true;
  doStack_.push_back( doNumber_ );
  return true;
}

bool Script::ProcessEnddo( wxString &name, double &value )
{
  if( ++doCount_[doNumber_] == doData_[doNumber_].size() )
  {
    currentLine_ = enddoLines_[doNumber_]+1;
    inADo_[doNumber_] = false;
    std::vector<double>().swap( doData_[doNumber_] );
    doStack_.pop_back();
    doStack_.empty() ? doNumber_=-1 : doNumber_=doStack_.back();
    return true;
  }
  currentLine_ = doLines_[doNumber_]+1;
  name = doNames_[doNumber_];
  value = doData_[doNumber_][doCount_[doNumber_]];
  return false;
}

void Script::AddEndif( std::size_t n )
{
  if( ifStack_.empty() )throw ESyntaxError( wxT("ENDIF without a preceeding IF") );
  endifLines_[ifStack_.back()] = n;
  ifStack_.pop_back();
}

void Script::AddEnddo( std::size_t n )
{
  if( doStack_.empty() )throw ESyntaxError( wxT("ENDDO without a preceeding DO") );
  enddoLines_[doStack_.back()] = n;
  doStack_.pop_back();
}

void Script::AddParameter()
{
  ParseToken *p = new ParseToken();
  p->SetNull();
  parameters_.push_back( p );
}

void Script::AddParameter( wxString const &s )
{
  ParseToken *p = new ParseToken();
  p->SetString( s );
  parameters_.push_back( p );
}

void Script::AddParameter( double d )
{
  ParseToken *p = new ParseToken();
  p->SetNumeric( d );
  parameters_.push_back( p );
}

// end of file
