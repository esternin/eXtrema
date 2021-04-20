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

#include "CMD_replot.h"
#include "ExGlobals.h"
#include "ExString.h"
#include "GRA_window.h"
#include "EGraphicsError.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "NumericVariable.h"

CMD_replot CMD_replot::cmd_replot_;

CMD_replot::CMD_replot() : Command( "REPLOT" )
{
  AddQualifier( "AXES",       true );
  AddQualifier( "ALLWINDOWS", false );
  AddQualifier( "TEXT",       false );
}

void CMD_replot::Execute( ParseLine const *p )
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
  std::size_t numWindows = ExGlobals::GetNumberOfWindows();
  ParseToken t( *p->GetToken(0) );
  if( qualifiers["ALLWINDOWS"] )
  {
    if( p->GetNumberOfTokens() > 1 )
     throw ECommandError( "REPLOT", "a window number is inconsistent with the \\ALLWINDOWS qualifier" );
    bool nothingToReplot = true;
    for( std::size_t i=0; i<numWindows; ++i )
    {
      if( !ExGlobals::GetGraphWindow(i)->GetDrawableObjects().empty() )
      {
        nothingToReplot = false;
        break;
      }
    }
    if( nothingToReplot )
    {
      ExGlobals::WarningMessage( "REPLOT: there is nothing to replot in any window" );
      return;
    }
    try
    {
      ExGlobals::ReplotAllWindows();
    }
    catch (EGraphicsError &e)
    {
      throw ECommandError( "REPLOT", e.what() );
    }
  }
  else
  {
    int currentWindow = ExGlobals::GetWindowNumber();
    if( p->GetNumberOfTokens() > 1 )
    {
      try
      {
        double d;
        NumericVariable::GetScalar( p->GetString(1), "window number", d );
        currentWindow = static_cast<int>(d);
      }
      catch (EVariableError &e )
      {
        throw ECommandError( "REPLOT", e.what() );
      }
      if( currentWindow < 0 )throw ECommandError( "REPLOT", "window number < 0" );
      if( currentWindow >= static_cast<int>(numWindows) )
       throw ECommandError( "REPLOT",
                            ExString("window ")+ExString(currentWindow)+" is not defined" );
      ExGlobals::SetWindowNumber( currentWindow );
      AddToStackLine( p->GetString(1) );
    }
    if( ExGlobals::GetGraphWindow(currentWindow)->GetDrawableObjects().empty() )
    {
      ExGlobals::WarningMessage(
       ExString("REPLOT: there is nothing to replot in window ")+ExString(currentWindow) );
      return;
    }
    try
    {
      ExGlobals::ReplotCurrentWindow();
    }
    catch (EGraphicsError &e)
    {
      throw ECommandError( "REPLOT", e.what() );
    }
  }
}

// end of file
