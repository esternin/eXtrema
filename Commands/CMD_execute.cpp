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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "CMD_execute.h"
#include "ExGlobals.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "ESyntaxError.h"
#include "TextVariable.h"
#include "Script.h"

#include "ExecuteForm.h"

CMD_execute CMD_execute::cmd_execute_;

void CMD_execute::Execute( ParseLine const *p )
{
  // Commands will be read from a file instead of from the terminal
  //
  // The order in which things are done:
  //  1) parameters input with the EXECUTE command are stored in PARAM common
  //  2) read a line from the file
  //  3) continuation lines are fixed up, i.e., made into single line
  //  4) the line is stored, along with start and end pointers
  //  5) any ?'s that were not supplied with the EXEC command are now requested
  //
  SetUp( p );
  //
  ExString fname;
  std::ifstream in;
  if( p->GetNumberOfTokens() == 1 )
    throw ECommandError( "EXECUTE", "expecting a script file name" );
  //
  if( p->IsNull(1) || p->IsNumeric(1) || p->IsEqualSign(1) || p->GetString(1).empty() )
    throw ECommandError( "EXECUTE", "expecting a script file name" );
  //
  fname.assign( p->GetString(1) );
  fname.RemoveQuotes();
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, fname );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( "EXECUTE", e.what() );
  }
  in.open( fname.c_str() );
  if( !in.is_open() )
  {
    in.clear();
    // get the file extension, if there was one given
    //
    ExString extension( fname.ExtractFileExt() );
    if( extension.empty() )
    {
      // no extension given, append .pcm to the filename and try again
      // if that fails, try .stk
      //
      ExString fnameTmp( fname+"."+ExGlobals::GetScriptExtension() );
      try
      {
        TryPaths( in, fnameTmp );
      }
      catch ( ECommandError &e )
      {
        fnameTmp = fname+"."+ExGlobals::GetStackExtension();
        try
        {
          TryPaths( in, fnameTmp );
        }
        catch ( ECommandError &e )
        {
          throw;
        }
      }
      fname = fnameTmp;
    }
    else // an extension was given
    {
      try
      {
        TryPaths( in, fname );
      }
      catch ( ECommandError &e )
      {
        throw;
      }
    }
  }
  // do not allow the currently open stack file to be executed
  // this would lead to recursion
  //
  if( ExGlobals::StackIsOn() && ExGlobals::GetStackFile()==fname )
    throw ECommandError( "EXECUTE", "executing an open stack file leads to recursion" );
  //
  AddToStackLine( p->GetString(1) );
  Script *aScript = new Script();
  //
  aScript->AddParameter( fname ); // the first parameter is always the script file name
  //
  // numberOfParameters is the number of parameters that were entered
  // with the execute command and the file name, e.g.
  // @file x y z     has 3 parameters, x, y, and z
  //
  int numberOfParameters = p->GetNumberOfTokens() - 2;
  for( int i=0; i<numberOfParameters; ++i )
  {
    if( p->IsNull(i+2) )           // null parameter
    {
      aScript->AddParameter();
      AddToStackLine( ",," );
    }
    else if( p->IsNumeric(i+2) )   // real parameter
    {
      aScript->AddParameter( p->GetNumeric(i+2) );
      AddToStackLine( ExString(p->GetNumeric(i+2)) );
    }
    else if( p->IsString(i+2) )    // string parameter
    {
      aScript->AddParameter( p->GetString(i+2) );
      AddToStackLine( p->GetString(i+2) );
    }
    else if( p->IsEqualSign(i+2) ) // = sign
    {
      aScript->AddParameter( ExString( "=" ) );
      AddToStackLine( "=" );
    }
  }
  // find out how many parameters there are in the file by reading each
  // line of the file and, if it is not a comment line, then search the
  // line for theParameterCharacter's ( default ? )
  //
  for( int lineNumber=0;; ++lineNumber )
  {
    ExString line;
    if( ReadLine(in,line) )break;  // end of file reached
    try
    {
      aScript->AddScriptLine( numberOfParameters, fname, line );
    }
    catch (ESyntaxError &e)
    {
      delete aScript;
      throw ECommandError( "EXECUTE", e.what() );
    }
  }
  in.close();
  //
  aScript->SetUpDos();
  //
  ExGlobals::SetScript( aScript );
}

void CMD_execute::TryPaths( std::ifstream &in, ExString &fname )
{
  in.open( fname.c_str() );
  if( in.is_open() )return;
  //
  ExString filePath( fname.ExtractFilePath() );
  if( filePath.empty() )
  {
    ExString fname2 = ExGlobals::GetCurrentPath() + fname;
    in.open( fname2.c_str() );
    if( !in.is_open() )
    {
      in.clear();
      ExString fname3 = ExGlobals::GetExecutablePath() + "Scripts/" + fname;
      in.open( fname3.c_str() );
      if( !in.is_open() )
      {
        in.clear();
        ExString s("could not open ");
        if( ExGlobals::GetCurrentPath() != (ExGlobals::GetExecutablePath()+"Scripts/") )
        {
          s += fname2 + ExString(" or ") + fname3;
        }
        else
        {
          s += fname2;
        }
        throw ECommandError( "EXECUTE", s );
      }
      else
      {
        fname = fname3;
      }
    }
    else
    {
      fname = fname2;
    }
  }
  else
  {
    ExString s( "could not open " );
    s += fname;
    throw ECommandError( "EXECUTE", s );
  }
}

bool CMD_execute::ReadLine( std::ifstream &in, ExString &line )
{
  bool readContinuationLine = true;
  std::string line2;
  line.erase();
  char const tab = 9;
  while( readContinuationLine )
  {
    readContinuationLine = false;
    line2.erase();
    if( !std::getline(in,line2) )return true;
    if( line2.empty() )line2 += ' ';
    std::size_t lineLen = line2.length();
    for( std::size_t j=0; j<lineLen; ++j )
      if( line2[j] == tab )line2[j] = ' '; // de-tab the input line
    //
    // trim leading and trailing blanks
    //
    std::size_t rTemp = line2.find_last_not_of( ' ' );
    if( rTemp != line2.npos )
    {
      std::size_t lTemp = line2.find_first_not_of( ' ' );
      if( lTemp != line2.npos )line2 = line2.substr( lTemp, rTemp-lTemp+1 );
    }
    else
    {
      line2 = " ";
    }
    lineLen = line2.length();
    int iend = lineLen-1;
    if( iend >= 0 )
    {
      if( line2[iend] == '-' )
      {
        line2.erase( iend, lineLen-iend );
        line += line2;
        readContinuationLine = true;
      }
    }
  }
  line += line2;
  return false;
}

// end of file
