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

#include "CMD_text.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "TextVariable.h"
#include "EGraphicsError.h"
#include "GRA_window.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_drawableText.h"
#include "GRA_wxWidgets.h"
#include "VisualizationWindow.h"

CMD_text *CMD_text::cmd_text_ = 0;

CMD_text::CMD_text() : Command( wxT("TEXT") )
{
  AddQualifier( wxT("ERASE"), false );
  AddQualifier( wxT("GRAPHUNITS"), false );
}

void CMD_text::Execute( ParseLine const *p )
{
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  ParseToken t( *p->GetToken(0) );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  if( qualifiers[wxT("ERASE")] ) // delete the last drawn text string
  {
    if( p->GetNumberOfTokens() > 1 )ExGlobals::WarningMessage( wxT("TEXT\\ERASE: parameters will be ignored") );
    gw->RemoveLastTextString();
  }
  else
  {
    if( p->GetNumberOfTokens()<2 || !p->IsString(1) )throw ECommandError( Name()+wxT(": expecting text string"));
    wxString text;
    TextVariable::GetVariable( p->GetString(1), false, text );
    AddToStackLine( p->GetString(1) );
    if( text.empty() )return;
    GRA_drawableText *dt = new GRA_drawableText( text, qualifiers[wxT("GRAPHUNITS")] );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      throw ECommandError( Name()+wxT(": ")+wxString(e.what(),wxConvUTF8) );
    }
    if( static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get(wxT("INTERACTIVE")))->Get() )
    {
      writeStack_ = false;
      ExGlobals::GetVisualizationWindow()->SetInteractiveText( dt );
      ExGlobals::PauseScripts();
    }
    else
    {
      wxClientDC dc( ExGlobals::GetwxWindow() );
      dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
      gw->AddDrawableObject( dt );
    }
  }
  ExGlobals::RefreshGraphics();
}

// end of file
