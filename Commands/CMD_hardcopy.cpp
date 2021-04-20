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

#include <memory>

#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "EGraphicsError.h"
#include "TextVariable.h"
#include "ExGlobals.h"

#include "GRA_postscript.h"
#include "GRA_gd.h"
#include "GRA_borlandMetafile.h"

#include "CMD_hardcopy.h"

CMD_hardcopy CMD_hardcopy::cmd_hardcopy_;

CMD_hardcopy::CMD_hardcopy() : Command( "HARDCOPY" )
{
  AddQualifier( "POSTSCRIPT", true );
  AddQualifier( "PNG", false );
  AddQualifier( "WMF", false );
  AddQualifier( "EMF", false );
}

void CMD_hardcopy::Execute( ParseLine const *p )
{
  // HARDCOPY filename                  (assumes PostScript)
  // HARDCOPY\POSTSCRIPT filename
  // HARDCOPY\PNG filename
  // HARDCOPY\PNG filename width height
  // HARDCOPY\WMF filename
  // HARDCOPY\EMF filename
  //
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  if( !p->IsString(1) )throw ECommandError( "HARDCOPY", "filename not entered" );
  ExString filename;
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, filename );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( "HARDCOPY", e.what() );
  }
  AddToStackLine( p->GetString(1) );
  //
  if( filename.ExtractFilePath().empty() )filename = ExGlobals::GetCurrentPath() + filename;
  //
  try
  {
    if( qualifiers["PNG"] )
    {
      int width =0, height =0;
      if( p->GetNumberOfTokens() == 4 )
      {
        double v1, v2;
        try
        {
          NumericVariable::GetScalar( p->GetString(2), "png width", v1 );
          NumericVariable::GetScalar( p->GetString(3), "png height", v2 );
        }
        catch( EVariableError &e )
        {
          throw ECommandError( "HARDCOPY", e.what() );
        }
        width = static_cast<int>(v1+0.5);
        height = static_cast<int>(v2+0.5);
        AddToStackLine( p->GetString(2) );
        AddToStackLine( p->GetString(3) );
      }
      GRA_gd png( filename, width, height );
      png.Draw();
    }
    else if( qualifiers["WMF"] )
    {
      std::auto_ptr<GRA_borlandMetafile> bm( new GRA_borlandMetafile(false,filename) );
      bm->DrawAndSave();
    }
    else if( qualifiers["EMF"] ) // enhanced metafile
    {
      std::auto_ptr<GRA_borlandMetafile> bm( new GRA_borlandMetafile(true,filename) );
      bm->DrawAndSave();
    }
    else if( qualifiers["POSTSCRIPT"] )
    {
      std::auto_ptr<GRA_postscript> ps( new GRA_postscript(filename.c_str()) );
      ps->Draw();
    }
  }
  catch (EGraphicsError &e)
  {
    throw ECommandError( "HARDOCOPY", e.what() );
  }
}

// end of file

