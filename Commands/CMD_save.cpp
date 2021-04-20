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

#include <fstream>
 
#include "ExGlobals.h"
#include "ExString.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "TextVariable.h"

#include "CMD_save.h"

CMD_save CMD_save::cmd_save_;

void CMD_save::Execute( ParseLine const *p )
{
  // SAVE filename
  //
  SetUp( p );
  //
  if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
    throw ECommandError( "SAVE", "no filename was entered" );
  ExString filename;
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, filename );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "SAVE", e.what() );
  }
  ExString extension( filename.ExtractFileExt() );
  if( extension.empty() )filename += ExString(".xml");
  std::ofstream ofs( filename.c_str(), ios_base::out );
  AddToStackLine( p->GetString(1) );
  try
  {
    ExGlobals::SaveSession( filename );
  }
  catch (runtime_error &rte)
  {
    throw ECommandError( "SAVE", rte.what() );
  }
}

// end of file
