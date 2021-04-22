/*
Copyright (C) 2005, 2006, 2007 Joseph L. Chuma, TRIUMF

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
#include <memory>

#include "wx/wx.h"
#include "wx/filename.h"

#include "CMD_hardcopy.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "EGraphicsError.h"
#include "GRA_wxWidgets.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "NumericVariable.h"
#include "VisualizationWindow.h"

CMD_hardcopy *CMD_hardcopy::cmd_hardcopy_ = 0;

CMD_hardcopy::CMD_hardcopy() : Command( wxT("HARDCOPY") )
{
  AddQualifier( wxT("POSTSCRIPT"), true );
  AddQualifier( wxT("PNG"), false );
  AddQualifier( wxT("JPEG"), false );
  AddQualifier( wxT("WMF"), false );
  AddQualifier( wxT("EMF"), false );
}

void CMD_hardcopy::Execute( ParseLine const *p )
{
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
  wxString command( Name()+wxT(": ") );
  if( !p->IsString(1) )throw ECommandError( command+wxT("filename not entered") );
  wxString filename;
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, filename );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(1) );
  //
  wxFileName wxfn( filename );
  if( wxfn.GetPath().empty() )filename = ExGlobals::GetCurrentPath() + wxfn.GetPathSeparator() + filename;
  //
  wxString extension;
  std::size_t len = filename.size();
  std::size_t i = filename.find_last_of( wxT('.') );
  extension = i==filename.npos ? wxT("") : filename.substr(i,len-i);
  try
  {
    if( qualifiers[wxT("PNG")] || qualifiers[wxT("JPEG")] )
    {
      wxBitmapType type;
      if( qualifiers[wxT("PNG")] )
      {
        if( extension.empty() )filename += wxT(".png");
        type = wxBITMAP_TYPE_PNG;
      }
      else
      {
        if( extension.empty() )filename += wxT(".jpeg");
        type = wxBITMAP_TYPE_JPEG;
      }
      int xminM, yminM, xmaxM, ymaxM;
      ExGlobals::GetMonitorLimits( xminM, yminM, xmaxM, ymaxM );
      int width = xmaxM-xminM;
      int height = ymaxM-yminM;
      if( p->GetNumberOfTokens() == 4 )
      {
        double v1, v2;
        try
        {
          NumericVariable::GetScalar( p->GetString(2), wxT("width"), v1 );
          NumericVariable::GetScalar( p->GetString(3), wxT("height"), v2 );
        }
        catch( EVariableError const &e )
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        width = static_cast<int>(v1+0.5);
        height = static_cast<int>(v2+0.5);
        AddToStackLine( p->GetString(2) );
        AddToStackLine( p->GetString(3) );
        xminM = 0;
        xmaxM = width;
        yminM = 0;
        ymaxM = height;
      }
      try
      {
        ExGlobals::GetVisualizationWindow()->SaveBitmap( xminM, yminM, xmaxM, ymaxM, filename, type );
      }
      catch( EGraphicsError const &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
    }
    else if( qualifiers[wxT("POSTSCRIPT")] || qualifiers[wxT("PSA")] )
    {
      if( extension.empty() )filename += wxT(".ps");
      try
      {
        ExGlobals::GetVisualizationWindow()->SavePS( filename );
      }
      catch( EGraphicsError const &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
    }
    else if( qualifiers[wxT("WMF")] )
    {
      throw ECommandError( command+wxT("windows metafiles are only available under windows") );
    }
    else if( qualifiers[wxT("EMF")] ) // enhanced metafile
    {
      throw ECommandError( command+wxT("windows metafiles are only available under windows") );
    }
  }
  catch( EGraphicsError const &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
}

// end of file

