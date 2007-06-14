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

#include "wx/config.h"

#include "InquireYNDialog.h"
#include "ExGlobals.h"
#include "CMD_inquire.h"
#include "ECommandError.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( InquireYNDialog, wxDialog )
  EVT_BUTTON( wxID_YES, InquireYNDialog::OnYes )
  EVT_BUTTON( wxID_NO, InquireYNDialog::OnNo )
  EVT_BUTTON( wxID_STOP, InquireYNDialog::OnStopAll )
  EVT_CLOSE( InquireYNDialog::CloseEventHandler )
END_EVENT_TABLE()

InquireYNDialog::InquireYNDialog()
    : wxDialog( (wxWindow*)NULL,wxID_ANY,wxT("inquire"),wxDefaultPosition,wxDefaultSize )
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );

  wxPanel *topPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize(400,100), wxNO_BORDER );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );

  messageCtrl_ = new wxStaticText( topPanel, -1, wxT(" ") );
  topSizer->Add( messageCtrl_, wxSizerFlags(0).Border(wxALL,10) );

  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(1).Expand().Border(wxALL,1) );
  
  wxPanel *bottomPanel = new wxPanel( (wxWindow*)this, wxID_ANY, wxDefaultPosition,
                                      wxSize(400,100), wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );

  wxButton *yesButton = new wxButton( bottomPanel, wxID_YES, wxT("Yes") );
  yesButton->SetToolTip( wxT("click to answer yes to the inquiry") );
  bottomSizer->Add( yesButton, wxSizerFlags(0).Border(wxALL,10) );

  wxButton *noButton = new wxButton( bottomPanel, wxID_NO, wxT("No") );
  noButton->SetToolTip( wxT("click to answer no to the inquiry") );
  bottomSizer->Add( noButton, wxSizerFlags(0).Border(wxALL,10) );

  wxButton *stopAllButton = new wxButton( bottomPanel, wxID_STOP, wxT("Stop all scripts") );
  stopAllButton->SetToolTip( wxT("click to stop all active scripts") );
  bottomSizer->Add( stopAllButton, wxSizerFlags(0).Border(wxALL,10) );

  bottomPanel->SetSizer( bottomSizer );

  mainSizer->Add( bottomPanel, wxSizerFlags(1).Expand().Centre().Border(wxALL,1) );

  SetSizer( mainSizer );
  
  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/InquireYNDialog/UPPERLEFTX"), 0l );
  int uly = config->Read( wxT("/InquireYNDialog/UPPERLEFTY"), 640l );
  int width = config->Read( wxT("/InquireYNDialog/WIDTH"), 570l );
  int height = config->Read( wxT("/InquireYNDialog/HEIGHT"), 145l );
  SetSize( ulx, uly, width, height );
  Show( true );
}

void InquireYNDialog::SetLabel( wxString const &message )
{
  messageCtrl_->SetLabel( message );
}

void InquireYNDialog::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/InquireYNDialog/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/InquireYNDialog/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/InquireYNDialog/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/InquireYNDialog/HEIGHT"), static_cast<long>(height) );
  }
  Destroy();
}

void InquireYNDialog::OnYes( wxCommandEvent &WXUNUSED(event) )
{
  try
  {
    CMD_inquire::Instance()->ProcessYN( wxT("YES") );
  }
  catch ( ECommandError const &e )
  {
    ExGlobals::StopAllScripts();
    wxMessageDialog *md =
        new wxMessageDialog( (wxWindow*)this,
                             wxString(e.what(),wxConvUTF8),
                             wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
  }
  Close();
}

void InquireYNDialog::OnNo( wxCommandEvent &WXUNUSED(event) )
{
  try
  {
    CMD_inquire::Instance()->ProcessYN( wxT("NO") );
  }
  catch ( ECommandError const &e )
  {
    ExGlobals::StopAllScripts();
    wxMessageDialog *md =
        new wxMessageDialog( (wxWindow*)this,
                             wxString(e.what(),wxConvUTF8),
                             wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
  }
  Close();
}

void InquireYNDialog::OnStopAll( wxCommandEvent &WXUNUSED(event) )
{
  ExGlobals::StopAllScripts();
  Close();
}

// end of file
