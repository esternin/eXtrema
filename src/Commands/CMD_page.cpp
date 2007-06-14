/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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

#include "CMD_page.h"

#include "ExGlobals.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "GRA_window.h"
#include "NumericVariable.h"
#include "VisualizationWindow.h"

CMD_page *CMD_page::cmd_page_ = 0;

CMD_page::CMD_page() : Command( wxT("PAGE") )
{
  AddQualifier( wxT("MESSAGES"), true );
  AddQualifier( wxT("INHERIT"), false );
}

void CMD_page::Execute( ParseLine const *p )
{
  // Make or select a graphics page
  //
  // PAGE n
  // PAGE\INHERIT target inherit_from
  //
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch ( ECommandError const &e )
  {
    throw;
  }
  wxString command( Name()+wxT(": ") );
  //
  // PAGE n
  // PAGE\INHERIT n inherit_from
  //
  VisualizationWindow *visWindow = ExGlobals::GetVisualizationWindow();
  int nPages = visWindow->GetNumberOfPages();
  double d;
  try
  {
    NumericVariable::GetScalar( p->GetString(1), wxT("page number"), d );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  int n = static_cast<int>( d );
  if( n < 0 )throw ECommandError( command+wxT("window number < 0") );
  AddToStackLine( p->GetString(1) );
  //
  int inherit = 0;
  if( qualifiers[wxT("INHERIT")] )
  {
    if( p->GetNumberOfTokens() < 3 )
      throw ECommandError( command+wxT("expecting target page number and page number to inherit from") );
    try
    {
      NumericVariable::GetScalar( p->GetString(2), wxT("page number to inherit from"), d );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    inherit = static_cast<int>( d );
    if( inherit < 0 )throw ECommandError( command+wxT("page number to inherit from < 0") );
    if( inherit > nPages )
      throw ECommandError( command+wxT("page number to inherit from > largest defined page number") );
    AddToStackLine( p->GetString(2) );
  }
  n > nPages ? visWindow->NewPage(n) : visWindow->SetPage(n);
  if( qualifiers[wxT("INHERIT")] )visWindow->InheritPage( inherit );
}

// end of file
