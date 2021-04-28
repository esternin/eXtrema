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
#include <fstream>

#include <wx/persist/toplevel.h>

#include "InquireDialog.h"
#include "ExGlobals.h"
#include "CMD_inquire.h"
#include "ECommandError.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( InquireDialog, wxDialog )
  EVT_BUTTON( wxID_OK, InquireDialog::OnOK )
  EVT_BUTTON( wxID_STOP, InquireDialog::OnStopAll )
  EVT_CLOSE( InquireDialog::CloseEventHandler )
END_EVENT_TABLE()

InquireDialog::InquireDialog()
    : wxDialog( (wxWindow*)NULL,wxID_ANY,wxT("inquire"),wxDefaultPosition,wxDefaultSize )
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );

  wxPanel *topPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize(400,100), wxNO_BORDER );
  wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

  messageCtrl_ = new wxStaticText( topPanel, -1, wxT(" ") );
  topSizer->Add( messageCtrl_, wxSizerFlags(0).Border(wxALL,10) );

  textCtrl_ = new wxTextCtrl( topPanel, -1 );
  topSizer->Add( textCtrl_, wxSizerFlags(0).Expand().Border(wxALL,5) );
  
  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(1).Expand().Border(wxALL,1) );
  
  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize(400,100), wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );

  wxButton *OKButton = new wxButton( bottomPanel, wxID_OK, wxT("OK") );
  OKButton->SetToolTip( wxT("click to accept the inquiry response to the inquiry") );
  bottomSizer->Add( OKButton, wxSizerFlags(0).Border(wxALL,10) );

  wxButton *stopAllButton = new wxButton( bottomPanel, wxID_STOP, wxT("Stop all scripts") );
  stopAllButton->SetToolTip( wxT("click to stop all active scripts") );
  bottomSizer->Add( stopAllButton, wxSizerFlags(0).Border(wxALL,10) );

  bottomPanel->SetSizer( bottomSizer );

  mainSizer->Add( bottomPanel, wxSizerFlags(1).Expand().Centre().Border(wxALL,1) );

  SetSizer( mainSizer );
  
  wxPersistentRegisterAndRestore(this, "InquireDialog");
  Show( true );
}

void InquireDialog::SetLabel( wxString const &message )
{ messageCtrl_->SetLabel( message ); }

void InquireDialog::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  Destroy();
}

void InquireDialog::OnOK( wxCommandEvent &WXUNUSED(event) )
{
  try
  {
    CMD_inquire::Instance()->ProcessString( textCtrl_->GetValue() );
  }
  catch ( ECommandError const &e )
  {
    ExGlobals::StopAllScripts();
    wxMessageBox( wxString(e.what(),wxConvUTF8),
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
  }
  Close();
}

void InquireDialog::OnStopAll( wxCommandEvent &WXUNUSED(event) )
{
  ExGlobals::StopAllScripts();
  Close();
}

// end of file
