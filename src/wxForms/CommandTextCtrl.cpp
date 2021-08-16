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
#include <iostream>
#include <stdexcept>

#include "CommandTextCtrl.h"
#include "AnalysisCommandPanel.h"
#include "ExGlobals.h"

#include "wx/config.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them

BEGIN_EVENT_TABLE( CommandTextCtrl, wxTextCtrl )
  EVT_KEY_DOWN( CommandTextCtrl::OnKeyDown )
END_EVENT_TABLE()

namespace
{

const char* CONFIG_COMMANDS_SECTION = "/Commands/";

} // anonymous namespace

CommandTextCtrl::CommandTextCtrl( AnalysisCommandPanel *parent )
    : wxTextCtrl( (wxWindow*)parent, wxID_ANY, wxT(""), wxDefaultPosition,
                  wxDefaultSize, wxTE_PROCESS_ENTER )
{
  SetToolTip( wxT("enter commands here") );
  commandStringsIndex_ = -1;

  // Restore the previous commands from the config, if any.
  if( auto config = wxConfig::Get() )
  {
    wxConfigPathChanger changePath(config, CONFIG_COMMANDS_SECTION);

    // Get all entries in this section, we don't currently use their names at
    // all.
    wxString name;
    long idx;
    bool cont = config->GetFirstEntry(name, idx);
    while( cont )
    {
      AddCommandString(config->Read(name));

      cont = config->GetNextEntry(name, idx);
    }
  }
}

CommandTextCtrl::~CommandTextCtrl()
{
  // Save the commands to the config.
  if( auto config = wxConfig::Get() )
  {
    // Limit the number of commands we save to prevent config file from growing
    // unboundedly. The choice of 100 is arbitrary and could perhaps be made an
    // option itself.
    const int CONFIG_MAX_COMMANDS = 100;

    // This is less efficient than just saving the new commands, but much
    // simpler and shouldn't be really a problem unless we increase the max
    // commands number by a lot. Until we do, keep things simple and delete
    // everything and than re-create.
    config->DeleteGroup(CONFIG_COMMANDS_SECTION);

    wxConfigPathChanger changePath(config, CONFIG_COMMANDS_SECTION);

    const size_t last = commandStrings_.size();
    const size_t first = last > CONFIG_MAX_COMMANDS ? last - CONFIG_MAX_COMMANDS : 0;
    for( size_t n = first; n < last; ++n )
    {
      config->Write(wxString::Format("%zd", n + 1), commandStrings_[n]);
    }
  }
}

wxString CommandTextCtrl::GetACommand()
{
  if( GetLineLength(0) == 0 ) // nothing was input
  {
    ExGlobals::WriteOutput( wxT(' ') );
    return wxString(); // return empty string
  }
  wxString inputLine( GetLineText(0) );
  ExGlobals::WriteOutput( inputLine );
  //
  // trim leading and trailing blanks
  //
  std::size_t rTemp = inputLine.find_last_not_of( wxT(' ') );
  if( rTemp != inputLine.npos )
  {
    std::size_t lTemp = inputLine.find_first_not_of( wxT(' ') );
    if( lTemp != inputLine.npos )inputLine = inputLine.substr( lTemp, rTemp-lTemp+1 );
  }
  else inputLine = wxT(' ');
  //
  Clear(); // clear the command entry field
  return inputLine;
}

wxString CommandTextCtrl::GetPreviousCommand()
{
  wxString s;
  if( commandStringsIndex_ >= 0 )s = commandStrings_[commandStringsIndex_--];
  return s;
}

wxString CommandTextCtrl::GetNextCommand()
{
  wxString s;
  if( commandStringsIndex_ < static_cast<int>(commandStrings_.size())-1 )
    s = commandStrings_[++commandStringsIndex_];
  return s;
}

void CommandTextCtrl::AddCommandString( wxString const &s )
{
  commandStrings_.push_back( s );
  commandStringsIndex_ = static_cast<int>(commandStrings_.size())-1;
}

void CommandTextCtrl::OnKeyDown( wxKeyEvent &event )
{
  switch ( event.GetKeyCode() )
  {
    case WXK_DOWN:
    {
      wxString s( GetNextCommand() );
      if( s.empty() )Clear();
      else
      {
        SetValue(s);
        SetInsertionPointEnd();
      }
      
      break;
    }
    case WXK_UP:
    {
      wxString s( GetPreviousCommand() );
      if( s.empty() )Clear();
      else
      {
        SetValue(s);
        SetInsertionPointEnd();
      }
      break;
    }
    case WXK_RETURN:
    {
      DoACommand();
      break;
    }
    default:
    {
      event.Skip();
      break;
    }
  }
}

void CommandTextCtrl::DoACommand()
{
  static wxString command;
  wxString inputLine( GetACommand() );
  //
  bool continuing = false;
  int len = inputLine.size();
  if( len > 0 && inputLine[len-1] == ExGlobals::GetContinuationCharacter() )
  {
    if( --len == 0 )
    {
      ExGlobals::WarningMessage( wxT("continuation line is empty") );
      command.clear();
      return;
    }
    inputLine.erase( len, 1 );  // erase continuation character
    continuing = true;
  }
  command += inputLine;
  if( continuing )return; // go back and get the next part of the command line
  if( command.empty() || command==wxString(wxT(' ')) )
  {
    if( ExGlobals::GetPausingScript() )ExGlobals::RestartScripts();
    return;
  }
  ExGlobals::PreParse( command );
  AddCommandString( command );
  try
  {
    ExGlobals::ProcessCommand( command );
  }
  catch ( std::runtime_error &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8),
                  wxT("ERROR"),
                  wxOK|wxICON_ERROR,
                  this );
    command.clear();
    return;
  }
  command.clear();
  if( ExGlobals::GetExecuteCommand() )
  {
    // the script run here must be the top level script
    // since it is run interactively
    //
    try
    {
      ExGlobals::RunScript();
    }
    catch ( std::runtime_error const &e )
    {
      ExGlobals::ShowScriptError( e.what() );
      ExGlobals::StopAllScripts();
    }
  }
}

// end of file
