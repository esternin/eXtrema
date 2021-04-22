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

#include "CMD_clear.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "ExGlobals.h"
#include "NumericVariable.h"
#include "GRA_window.h"

CMD_clear *CMD_clear::cmd_clear_ = 0;

CMD_clear::CMD_clear() : Command( wxT("CLEAR") )
{
  AddQualifier( wxT("REPLOT"), true );
  AddQualifier( wxT("HISTORY"), false );
}

void CMD_clear::Execute( ParseLine const *p )
{
  // Clear the graphics window.
  // If \REPLOT is used, clear the replot buffers as well.
  //
  // CLEAR             clear all GRA_window's
  // CLEAR n           clear GRA_window n
  // CLEAR\-REPLOT     clear all GRA_window's but leave replot buffers alone
  // CLEAR\-REPLOT n   clear GRA_window n but leave replot buffers alone
  // CLEAR\HISTORY     just clear the history buffer
  //
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
  wxString command( Name()+wxT(": ") );
  if( qualifiers[wxT("HISTORY")] )
  {
    ExGlobals::ClearHistory();
    return;
  }
  if( p->GetNumberOfTokens() == 1 ) // no parameters entered
  {
    ExGlobals::ClearGraphicsMonitor();
    if( qualifiers[wxT("REPLOT")] )ExGlobals::ClearWindows();
    return;
  }
  // a window number was entered
  //
  double d;
  try
  {
    NumericVariable::GetScalar( p->GetString(1), wxT("window number"), d );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  int n = static_cast<int>( d );
  if( n < 0 )throw ECommandError( Name()+wxT(": window number < 0") );
  if( n > static_cast<int>(ExGlobals::GetNumberOfWindows()) )
    throw ECommandError( command+wxT("window number > largest defined window number") );
  ExGlobals::ClearGraphicsWindow( qualifiers[wxT("REPLOT")], n );
  AddToStackLine( p->GetString(1) );
}

 // end of file

