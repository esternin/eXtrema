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
#include "wx/wx.h"

#include "CMD_if.h"
#include "ECommandError.h"
#include "ESyntaxError.h"
#include "ParseLine.h"
#include "Script.h"
#include "NumericVariable.h"
#include "ExGlobals.h"

CMD_if *CMD_if::cmd_if_ = 0;

CMD_if::CMD_if() : Command( wxT("IF") )
{}

void CMD_if::Execute( ParseLine const *p )
{
  SetUp( p );
  wxString command( Name()+wxT(": ") );
  //
  // script command     IF expression THEN command
  //                 or
  //                    IF expression THEN
  //                       command
  //                       ...
  //                       command
  //                    ENDIF
  //
  if( !p->IsString(1) )throw ECommandError( command+wxT("invalid IF syntax, expecting an expression") );
  if( !p->IsString(2) )throw ECommandError( command+wxT("invalid IF syntax, expecting THEN") );
  //
  Script *script = ExGlobals::GetScript();
  if( !script )throw ECommandError( command+wxT("only valid in a script") );
  //
  wxString s1( p->GetString(1) );
  wxString s2( p->GetString(2) );
  int n = p->GetNumberOfTokens();
  //
  wxString then( s2.Upper() );
  if( then != wxT("THEN") )throw ECommandError( command+wxT("invalid syntax, expecting THEN") );
  int ndim;
  double value;
  std::vector<double> data;
  int dimSizes[3];
  NumericVariable::GetVariable( s1, ndim, value, data, dimSizes );
  if( ndim > 0 )throw ECommandError( command+wxT("IF expression must have scalar result") );
  if( n == 3 ) // there must be an ENDIF
  {
    if( value != 1.0 )
    {
      try
      {
        script->ProcessIf();
      }
      catch( ESyntaxError const &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
    }
  }
  else // no ENDIF; some command follows THEN
  {
    if( value == 1.0 )
    {
      wxString commandLine( p->GetInputLine() );
      wxString tmp( commandLine.Upper() );
      int i = tmp.find(wxT("THEN"));
      commandLine = commandLine.substr(i+5,tmp.size()-i-4);
      try
      {
        ExGlobals::ProcessCommand(commandLine);
      }
      catch( std::runtime_error const &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( ExGlobals::GetReturnCommand() )ExGlobals::PrepareToStopScript();
      if( ExGlobals::GetExecuteCommand() )ExGlobals::PrepareToExecuteScript();
      if( ExGlobals::GetPausingScript() )ExGlobals::PrepareToPauseScript();
    }
  }
}

// end of file
