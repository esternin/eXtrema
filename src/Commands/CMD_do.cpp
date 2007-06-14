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
#include "wx/wx.h"

#include "CMD_do.h"
#include "ECommandError.h"
#include "ExGlobals.h"
#include "Script.h"
#include "ESyntaxError.h"
#include "ParseLine.h"
#include "EVariableError.h"
#include "NumericVariable.h"

CMD_do *CMD_do::cmd_do_ = 0;

CMD_do::CMD_do() : Command( wxT("DO") )
{}

void CMD_do::Execute( ParseLine const *p )
{
  // script macro command     DO n = vector
  //
  SetUp( p );
  wxString command( Name()+wxT(": ") );
  if( !p->IsString(1) )throw ECommandError( command+wxT("correct syntax: DO i = vector") );
  if( !p->IsEqualSign(2) )throw ECommandError( command+wxT("correct syntax: DO i = vector") );
  if( !p->IsString(3) )throw ECommandError( command+wxT("correct syntax: DO i = vector") );
  Script *script = ExGlobals::GetScript();
  if( !script )throw ECommandError( command+wxT("DO is only valid in a script") );
  //
  wxString name( p->GetString(1) );
  bool setupDo;
  try
  {
    setupDo = script->ProcessDo( name.Upper() );
  }
  catch (ESyntaxError &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( setupDo )
  {
    int ndim;
    double value;
    std::vector<double> data;
    int dimSizes[3];
    NumericVariable::GetVariable( p->GetString(3), ndim, value, data, dimSizes );
    if( ndim != 1 )throw ECommandError( command+wxT("DO loop range must be a vector") );
    script->SetDoData( data );
    try
    {
      NumericVariable::PutVariable( name, data[0], p->GetInputLine() );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
}

// end of file
