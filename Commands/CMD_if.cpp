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

#include "CMD_if.h"
#include "ExGlobals.h"
#include "ExString.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "ESyntaxError.h"
#include "Script.h"
#include "NumericVariable.h"

CMD_if CMD_if::cmd_if_;

void CMD_if::Execute( ParseLine const *p )
{
  SetUp( p );
  //
  // script command     IF expression THEN command
  //                 or
  //                    IF expression THEN
  //                       command
  //                       ...
  //                       command
  //                    ENDIF
  //
  if( !p->IsString(1) )throw ECommandError("IF","invalid IF syntax, expecting an expression");
  if( !p->IsString(2) )throw ECommandError("IF","invalid IF syntax, expecting THEN");
  //
  Script *script = ExGlobals::GetScript();
  if( !script )throw ECommandError("IF","IF is only valid in a script");
  //
  ExString s1( p->GetString(1) );
  ExString s2( p->GetString(2) );
  int n = p->GetNumberOfTokens();
  //
  ExString then( s2.UpperCase() );
  if( then != "THEN" )throw ECommandError("IF","invalid syntax, expecting THEN");
  int ndim;
  double value;
  std::vector<double> data;
  int dimSizes[3];
  NumericVariable::GetVariable( s1, ndim, value, data, dimSizes );
  if( ndim > 0 )throw ECommandError("IF","IF expression must have scalar result");
  if( n == 3 ) // there must be an ENDIF
  {
    if( value != 1.0 )
    {
      try
      {
        script->ProcessIf(); // skip ahead to the endif
      }
      catch (ESyntaxError &e)
      {
        throw ECommandError("IF",e.what());
      }
    }
  }
  else // no ENDIF; some command follows THEN
  {
    if( value == 1.0 )
    {
      ExString commandLine( p->GetInputLine() );
      ExString tmp( commandLine.UpperCase() );
      int i = tmp.find("THEN");
      commandLine = commandLine.substr(i+5,tmp.size()-i-4);
      try
      {
        ExGlobals::ProcessCommand(commandLine);
      }
      catch (runtime_error &e)
      {
        throw ECommandError("IF",e.what());
      }
      if( ExGlobals::GetReturnCommand() )ExGlobals::PrepareToStopScript();
      if( ExGlobals::GetExecuteCommand() )ExGlobals::PrepareToExecuteScript();
      if( ExGlobals::GetPausingScript() )ExGlobals::PrepareToPauseScript();
    }
  }
}

// end of file
