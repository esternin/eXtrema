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

#include <dos.h>

#include "CMD_text.h"
#include "ExGlobals.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "GRA_window.h"
#include "ECommandError.h"
#include "EGraphicsError.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "MainGraphicsWindow.h"
#include "TextVariable.h"
#include "GRA_drawableText.h"

CMD_text CMD_text::cmd_text_;

CMD_text::CMD_text() : Command( "TEXT" )
{
  AddQualifier( "ERASE", false );
  AddQualifier( "GRAPHUNITS", false );
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
  if( qualifiers["ERASE"] ) // delete the last drawn text string
  {
    if( p->GetNumberOfTokens() > 1 )ExGlobals::WarningMessage( "TEXT\\ERASE: parameters will be ignored" );
    gw->RemoveLastTextString();
  }
  else
  {
    if( p->GetNumberOfTokens()<2 || !p->IsString(1) )throw ECommandError("TEXT","expecting text string");
    ExString text;
    TextVariable::GetVariable( p->GetString(1), false, text );
    AddToStackLine( p->GetString(1) );
    if( text.empty() )return;
    GRA_drawableText *dt = new GRA_drawableText( text, qualifiers["GRAPHUNITS"] );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      delete dt;
      throw ECommandError( "TEXT", e.what() );
    }
    if( static_cast<GRA_boolCharacteristic*>(gw->GetTextCharacteristics()->Get("INTERACTIVE"))->Get() )
    {
      writeStack_ = false;
      MainGraphicsForm->SetTextPlacementMode( dt );
      ExGlobals::PauseScripts();
      PostMessage( MainGraphicsForm->Handle, WM_EXITSIZEMOVE, 1,2 );
    }
    else
    {
      ExGlobals::GetScreenOutput()->Draw( dt );
      gw->AddDrawableObject( dt );
    }
  }
}

// end of file
