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
#include <fstream>
 
#include "CMD_save.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "TextVariable.h"

CMD_save *CMD_save::cmd_save_ = 0;

CMD_save::CMD_save() : Command( wxT("SAVE") )
{}

void CMD_save::Execute( ParseLine const *p )
{
  // SAVE filename
  //
  SetUp( p );
  //
  wxString command( Name()+wxT(": ") );
  if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
    throw ECommandError( command+wxT("no filename was entered") );
  wxString filename;
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, filename );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  // get the file extension, if there was one given
  //
  wxString extension;
  std::size_t len = filename.size();
  std::size_t i = filename.find_last_of( wxT('.') );
  if( i != filename.npos )extension = filename.substr(i,len-i);
  //
  if( extension.empty() )filename += wxString(wxT(".xml"));
  std::ofstream ofs( filename.mb_str(), std::ios_base::out );
  AddToStackLine( p->GetString(1) );
  try
  {
    ExGlobals::SaveSession( filename );
  }
  catch (std::runtime_error &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
}

// end of file
