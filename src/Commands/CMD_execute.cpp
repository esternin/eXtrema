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
#include <memory>

#include "CMD_execute.h"
#include "ECommandError.h"
#include "ESyntaxError.h"
#include "ParseLine.h"
#include "Script.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "ExGlobals.h"

CMD_execute *CMD_execute::cmd_execute_ = 0;

CMD_execute::CMD_execute() : Command( wxT("EXECUTE") )
{}

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
  wxString command( Name()+wxT(": ") );
  if( p->GetNumberOfTokens() == 1 )
    throw ECommandError( command+wxT("expecting a script file name") );
  //
  if( p->IsNull(1) || p->IsNumeric(1) || p->IsEqualSign(1) || p->GetString(1).empty() )
    throw ECommandError( command+wxT("expecting a script file name") );
  //
  wxString fname( p->GetString(1) );
  ExGlobals::RemoveQuotes( fname );
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, fname );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  wxString fnameSave( fname );
  //
  inStream_.close();
  inStream_.clear( std::ios::goodbit );
  inStream_.open( fname.mb_str(wxConvUTF8), std::ios::in );
  if( !inStream_.is_open() )
  {
    inStream_.clear( std::ios::goodbit );
    //
    // get the file extension, if there was one given
    //
    wxString extension( ExGlobals::GetFileExtension(fname) );
    if( extension.empty() )
    {
      // no extension given, append .pcm to the filename and try again
      // and if that fails, try the .stk extension
      //
      fname += wxT('.')+ExGlobals::GetScriptExtension();
      try
      {
        TryPaths( fname );
      }
      catch ( ECommandError &e )
      {
        fname = fnameSave+wxT('.')+ExGlobals::GetStackExtension();
        try
        {
          TryPaths( fname );
        }
        catch ( ECommandError &e )
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
      }
    }
    else  // an extension was given
    {
      try
      {
        TryPaths( fname );
      }
      catch ( ECommandError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
    }
  }
  if( !inStream_.is_open() )
    throw ECommandError( command+wxT("unable to open ")+fname );
  //
  // do not allow the currently open stack file to be executed
  // this would lead to recursion
  //
  if( ExGlobals::StackIsOn() && ExGlobals::GetStackFile()==fname )
    throw ECommandError( command+wxT("executing an open stack file leads to recursion") );
  //
  AddToStackLine( p->GetString(1) );
  std::unique_ptr<Script> aScript( new Script() );
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
      AddToStackLine( wxT(",,") );
    }
    else if( p->IsNumeric(i+2) )   // real parameter
    {
      aScript->AddParameter( p->GetNumeric(i+2) );
      wxString s;
      AddToStackLine( s << p->GetNumeric(i+2) );
    }
    else if( p->IsString(i+2) )    // string parameter
    {
      aScript->AddParameter( p->GetString(i+2) );
      AddToStackLine( p->GetString(i+2) );
    }
    else if( p->IsEqualSign(i+2) ) // = sign
    {
      aScript->AddParameter( wxString( wxT('=') ) );
      AddToStackLine( wxT("=") );
    }
  }
  // find out how many parameters there are in the file by reading each
  // line of the file and, if it is not a comment line, then search the
  // line for theParameterCharacter's ( default ? )
  //
  inStream_.seekg( 0, std::ios::beg ); // seek to the beginning of the file
  for( int lineNumber=0;; ++lineNumber )
  {
    wxString line;
    if( ReadLine(line) )break;  // end of file reached
    try
    {
      aScript->AddScriptLine( numberOfParameters, fname, line );
    }
    catch (ESyntaxError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
  inStream_.close();
  inStream_.clear( std::ios::goodbit );
  //
  aScript->SetUpDos();
  //
  ExGlobals::SetScript( aScript.release() );
}

void CMD_execute::TryPaths( wxString &fname )
{
  inStream_.open( fname.mb_str(wxConvUTF8), std::ios::in );
  if( inStream_.is_open() )return;
  inStream_.clear( std::ios::goodbit );
  wxString filePath;
  std::size_t i = fname.find_last_of( wxT('/') );
  if( i != fname.npos )filePath = fname.substr(0,i);
  if( filePath.empty() )
  {
    wxString fname2 = ExGlobals::GetCurrentPath() + wxT("/") + fname;
    inStream_.open( fname2.mb_str(wxConvUTF8), std::ios::in );
    if( !inStream_.is_open() )
    {
      inStream_.clear( std::ios::goodbit );
      wxString fname3 = ExGlobals::GetExecutablePath() + wxT("/Scripts/") + fname;
      inStream_.open( fname3.mb_str(wxConvUTF8), std::ios::in );
      if( !inStream_.is_open() )
      {
        inStream_.clear( std::ios::goodbit );
        wxString s(wxT("could not open "));
        if( ExGlobals::GetCurrentPath() != (ExGlobals::GetExecutablePath()+wxT("/Scripts/")) )
        {
          s += fname2 + wxString(wxT("\n or ")) + fname3;
        }
        else
        {
          s += fname2;
        }
        throw ECommandError( s );
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
  else throw ECommandError( wxT("could not open ")+fname );
}

bool CMD_execute::ReadLine( wxString &line )
{
  bool readContinuationLine = true;
  std::string line2;
  line.erase();
  char const tab = 9;
  while( readContinuationLine )
  {
    readContinuationLine = false;
    line2.clear();
    if( !std::getline(inStream_,line2).good() )return true;
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
        line += wxString(line2.c_str(),wxConvUTF8);
        readContinuationLine = true;
      }
    }
  }
  line += wxString(line2.c_str(),wxConvUTF8);
  return false;
}

// end of file
