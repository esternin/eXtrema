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

#include "ExGlobals.h"
#include "ExString.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "ESyntaxError.h"
#include "EVariableError.h"
#include "Script.h"
#include "NumericVariable.h"

#include "CMD_do.h"

CMD_do CMD_do::cmd_do_;

CMD_do::CMD_do() : Command( "DO" )
{}

void CMD_do::Execute( ParseLine const *p )
{
  // script macro command     DO n = vector
  //
  SetUp( p );
  if( p->GetNumberOfTokens() < 4 )
   throw ECommandError( "DO", "correct syntax: DO i = vector" );
  if( !p->IsString(1) )throw ECommandError("DO","correct syntax: DO i = vector");
  if( !p->IsEqualSign(2) )throw ECommandError("DO","correct syntax: DO i = vector");
  if( !p->IsString(3) )throw ECommandError("DO","correct syntax: DO i = vector");
  Script *script = ExGlobals::GetScript();
  if( !script )throw ECommandError("DO","DO is only valid in a script");
  //
  ExString name( p->GetString(1) );
  bool setupDo;
  try
  {
    setupDo = script->ProcessDo( name.UpperCase() );
  }
  catch (ESyntaxError &e)
  {
    throw ECommandError("DO",e.what());
  }
  if( setupDo )
  {
    int ndim;
    double value;
    std::vector<double> data;
    int dimSizes[3];
    NumericVariable::GetVariable( p->GetString(3), ndim, value, data, dimSizes );
    if( ndim != 1 )throw ECommandError("DO","DO loop range must be a vector");
    script->SetDoData( data );
    try
    {
      NumericVariable::PutVariable( name, data[0], p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( "DO", e.what() );
    }
  }
}

// end of file
