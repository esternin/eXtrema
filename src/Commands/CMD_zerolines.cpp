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

#include "CMD_zerolines.h"
#include "ECommandError.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EGraphicsError.h"
#include "GRA_window.h"

CMD_zerolines *CMD_zerolines::cmd_zerolines_ = 0;

CMD_zerolines::CMD_zerolines() : Command( wxT("ZEROLINES") )
{
  AddQualifier( wxT("HORIZONTAL"), false );
  AddQualifier( wxT("VERTICAL"),   false );
}

void CMD_zerolines::Execute( ParseLine const *p )
{
  // ZEROLINES                         draw both horizontal & vertical
  // ZEROLINES\HORIZONTAL              draw horizontal only
  // ZEROLINES\VERTICAL                draw vertical only
  //
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
  bool horizontal=false, vertical=false;
  if( qualifiers[wxT("HORIZONTAL")] )
  {
    horizontal = true;
    vertical = false;
  }
  if( qualifiers[wxT("VERTICAL")] )
  {
    horizontal = false;
    vertical = true;
  }
  if( !horizontal && !vertical )
  {
    horizontal = true;
    vertical = true;
  }
  try
  {
    ExGlobals::GetGraphWindow()->DrawZerolines( horizontal, vertical );
  }
  catch (EGraphicsError &e)
  {
    throw ECommandError( Name()+wxT(": ")+wxString(e.what(),wxConvUTF8) );
  }
  ExGlobals::RefreshGraphics();
}

// end of file
