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
#include "AnalysisCommandPanel.h"
#include "CommandTextCtrl.h"
#include "AnalysisWindow.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them

BEGIN_EVENT_TABLE( AnalysisCommandPanel, wxPanel )
  EVT_BUTTON( ID_processCommand, AnalysisCommandPanel::OnProcessCommand )
END_EVENT_TABLE()

AnalysisCommandPanel::AnalysisCommandPanel( AnalysisWindow *parent )
    : wxPanel(parent->GetMainPanel(),wxID_ANY,wxDefaultPosition,wxDefaultSize)
{
  // this panel will expand horizontally but not vertically
  //
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  
  sizer->Add( new wxStaticText((wxWindow*)this,-1,wxT("Command (input field):")),
              wxSizerFlags(0).Border(wxTOP,10) );
  commandTextCtrl_ = new CommandTextCtrl(this);
  sizer->Add( commandTextCtrl_, wxSizerFlags(0).Align(0).Expand().Border(wxALL,1) );
  
  wxButton *processCommandButton =
      new wxButton( (wxWindow*)this, ID_processCommand, wxT("Process Command"), wxDefaultPosition, wxSize(15*GetCharWidth(), -1) );
  processCommandButton->SetToolTip( 
  wxT("click here to enter command or just hit the enter key") );
  sizer->Add( processCommandButton, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  SetSizer( sizer );
}

void AnalysisCommandPanel::OnProcessCommand( wxCommandEvent &WXUNUSED(event) )
{ commandTextCtrl_->DoACommand(); }

void AnalysisCommandPanel::AddCommandString( wxString const &s )
{ commandTextCtrl_->AddCommandString( s ); }

std::vector<wxString> &AnalysisCommandPanel::GetCommandStrings()
{ return commandTextCtrl_->GetCommandStrings(); }

// end of file
