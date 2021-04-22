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

#include "CMD_display.h"
#include "ECommandError.h"
#include "ParseLine.h"
#include "EExpressionError.h"
#include "Expression.h"

CMD_display *CMD_display::cmd_display_ = 0;

CMD_display::CMD_display() : Command( wxT("DISPLAY") )
{
  AddQualifier( wxT("CLEAR"), false );
}

void CMD_display::Execute( ParseLine const *p )
{
  //
  // script macro command     DISPLAY string
  //
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
  wxString command( Name()+wxT(": ") );
  if( p->GetNumberOfTokens()==1 || !p->IsString(1) )
   throw ECommandError( command+wxT("correct syntax: DISPLAY string") );
  //
  // evaluate the expression
  //
  Expression expr( p->GetString(1) );
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( !expr.IsCharacter() )throw ECommandError( command+wxT("correct syntax: DISPLAY string") );
  //SetUpDisplayForm( expr.GetFinalString(), qualifiers[wxT("CLEAR")] );
}

// end of file
