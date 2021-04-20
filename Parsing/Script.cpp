/*
Copyright (C) 2005, ..., 2007 Joseph L. Chuma, TRIUMF

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

#include <stdexcept>

#include "Script.h"
#include "ExGlobals.h"
#include "ESyntaxError.h"
#include "ParseToken.h"

char Script::commentCharacter_ = '!';
char Script::parameterCharacter_ = '?';

void Script::Run()
{
  for( ;; )
  {
    Script *script = 0;
    ExString commandLine;
    if( ExGlobals::GetRestartingScript() )
    {
      if( scriptOriginatingPause_ )
      {
        ExGlobals::SetRestartingScript( false );
        scriptOriginatingPause_ = false;
        if( GetNextLine(commandLine) )return; // end of file found
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
      if( GetNextLine(commandLine) )return; // end of file found
      if( echo_ )ExGlobals::WriteOutput( commandLine );
    }
    try
    {
      if( !commandLine.empty() && commandLine!=ExString(" ") )
        ExGlobals::ProcessCommand(commandLine);
    }
    catch( runtime_error &e )
    {
      throw;
    }
    if( ExGlobals::GetExecuteCommand() )
    {
      ExGlobals::SetExecuteCommand( false );
      try
      {
        ExGlobals::RunScript();
      }
      catch ( runtime_error &e )
      {
        throw;
      }
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

ExString Script::Filename()
{
  return parameters_[0]->GetString();
}

ExString Script::CurrentLine()
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

void Script::AddScriptLine( std::size_t numberOfParameters, ExString &fname, ExString &line )
{
  char const openingQuote = 96;
  char const singleQuote = 39;
  char const doubleQuote = 34;
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
      std::size_t rTemp = line.find_last_not_of( ' ' ); // trim trailing blanks
      if( rTemp == line.npos() )return;
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
  ExString uLine( line.UpperCase() ); // convert line to uppercase uLine
  //
  if( uLine[lineLength-1]==':' ) // found a label, e.g.,  LABEL:
  {
    if( uLine.find(' ') != uLine.npos() )throw ESyntaxError("blanks are not allowed in a script label");
    AddLabel( scriptLines_.size(), uLine.substr(0,lineLength-1) );
  }
  else if( uLine.substr(0,3)==ExString("IF ") )
  {
    // only record the ENDIF line when in an if block, not a single statement
    //
    if( uLine.substr(lineLength-4,4) == "THEN" )AddIf( scriptLines_.size() );
  }
  else if( uLine.substr(0,5)==ExString("ENDIF") )
  {
    try
    {
      AddEndif( scriptLines_.size() );
    }
    catch (ESyntaxError &e)
    {
      throw;
    }
  }
  else if( uLine.substr(0,3)==ExString("DO ") )
  {
    AddDo( scriptLines_.size() );
  }
  else if( uLine.substr(0,5)==ExString("ENDDO") )
  {
    try
    {
      AddEnddo( scriptLines_.size() );
    }
    catch (ESyntaxError &e)
    {
      throw;
    }
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
      bool missingParameter;
      std::size_t ipindex, npl, npn;
      if( i+1==lineLength || toascii(line[i+1])<48 || toascii(line[i+1])>57 )
      {
        missingParameter = false;
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
        ExString s;
        s.append( line[++i] );
        for( ;; )
        {
          int ival;
          i+1<lineLength ? ival=toascii(line[i+1]) : ival=0;
          if( ival<48 || ival>57 )break;
          s.append( line[++i] );
        }
        double x;
        std::stringstream ss;
        ss << s.c_str();
        ss >> x;
        //
        npn = i - npl + 1;
        //
        ipindex = static_cast<int>(x);
        missingParameter = false;
        if ( ipindex > numberOfParameters )
        {
          missingParameter = true;
          numberOfParameters = ipindex;
        }
      }
      if ( missingParameter )
      {
        char c[100];
        sprintf( c, "parameter %d missing from file: %s", ipindex, fname.c_str() );
        throw ESyntaxError( c );
      }
      ExString s1;
      if ( parameters_[ipindex]->IsaNull() )
      {
        s1 = ",,";
      }
      else if ( parameters_[ipindex]->IsaNumeric() )
      {
        char c[50];
        sprintf( c, "%g", parameters_[ipindex]->GetNumeric() );
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
  doNames_.resize( n, ExString(" ") );
  doCount_.resize( n, 0 );
  doData_.resize( n );
}

bool Script::GetNextLine( ExString &line )
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
    std::vector<std::size_t>::iterator vi = find(labelLines_.begin(),labelLines_.end(),currentLine_++);
    if( vi != labelLines_.end() )getLine = true;
  }
  RemakeInputLine(line);
  return false;
}
 
void Script::RemakeInputLine( ExString &line )
{
  if( line.find(parameterCharacter_) == line.npos() )return;
  //
  ExString cline;
  char const openingQuote = 96;
  char const singleQuote = 39;
  char const doubleQuote = 34;
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
      int ival;
      i+1<lineLen ? ival=toascii(line[i+1]) : ival=0;
      if( ival<48 || ival>57 )
      {
        // sequential parameter substitution
        //
        ParseToken *pt = parameters_[parameterCount];
        if( pt->IsaNumeric() )
        {
          char c[50];
          sprintf( c, "%g", pt->GetNumeric() );
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
        ExString s;
        s.append( line[++i] );
        for( ;; )
        {
          i+1<lineLen ? ival=toascii(line[i+1]) : ival=0;
          if( ival<48 || ival>57 )break;
          s.append( line[++i] );
        }
        double xtemp;
        std::stringstream ss;
        ss << s.c_str();
        ss >> xtemp;
        //
        parameterCount = static_cast<int>( xtemp );
        ParseToken *pt = parameters_[parameterCount];
        if( pt->IsaNumeric() )
        {
          char c[50];
          sprintf( c, "%g", pt->GetNumeric() );
          cline += ExString( c );
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
 
void Script::ProcessGoTo( ExString const &label )
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
  if( !found )throw ESyntaxError("GOTO label not found");
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
  throw ESyntaxError("could not find ENDIF");
}

bool Script::ProcessDo( ExString const &name )
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
  if( !foundIt )throw ESyntaxError("could not find the DO line" );
  if( inADo_[doNumber_] )return false;
  //
  doNames_[doNumber_] = name;
  inADo_[doNumber_] = true;
  doStack_.push_back( doNumber_ );
  return true;
}

bool Script::ProcessEnddo( ExString &name, double &value )
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

// end of file
