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

#include <wx/persist/toplevel.h>

#include "StackDialog.h"
#include "ChooseFilePanel.h"
#include "AnalysisWindow.h"
#include "ParseLine.h"
#include "ExGlobals.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( StackDialog, wxFrame )
  EVT_RADIOBOX( ID_onoff, StackDialog::OnOffToggle )
  EVT_BUTTON( wxID_APPLY, StackDialog::OnApply )
  EVT_BUTTON( wxID_CLOSE, StackDialog::OnClose )
  EVT_CLOSE( StackDialog::CloseEventHandler )
END_EVENT_TABLE()

StackDialog::StackDialog( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("record commands to a script"),
              wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      analysisWindow_(parent)
{
  wxPanel* const mainPanel = new wxPanel(this);
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );

  wxPanel *topPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );
  wxString choices[2] = { wxT("ON"), wxT("OFF") };
  onOffRB_ = new wxRadioBox( topPanel, ID_onoff, wxT("Command recording"), wxDefaultPosition, wxDefaultSize,
                             2, choices, 2, wxRA_SPECIFY_COLS );
  topSizer->Add( onOffRB_, wxSizerFlags(0).Border(wxALL,1) );
  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(1).Centre().Border(wxALL,1) );

  chooseFilePanel_ = new ChooseFilePanel( mainPanel, false, wxT("Browse folders"),
                                          wxT("stack file|*.stk|any file|*.*") );
  mainSizer->Add( chooseFilePanel_, wxSizerFlags(1).Expand().Border(wxALL,1) );

  wxPanel *midPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxSize(400,100), wxNO_BORDER );
  wxBoxSizer *midSizer = new wxBoxSizer( wxHORIZONTAL );
  appendCkB_ = new wxCheckBox( midPanel, wxID_ANY, wxT("Append to file") );
  midSizer->Add( appendCkB_, wxSizerFlags(0).Center().Border(wxALL,1) );
  midPanel->SetSizer( midSizer );
  mainSizer->Add( midPanel, wxSizerFlags(1).Expand().Centre().Border(wxALL,1) );
  
  wxPanel *bottomPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxSize(400,100), wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );
  wxButton *applyButton = new wxButton( bottomPanel, wxID_APPLY, wxT("Apply") );
  applyButton->SetToolTip( wxT("click to turn command recording on/off") );
  bottomSizer->Add( applyButton, wxSizerFlags(0).Border(wxALL,10) );
  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );
  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(1).Expand().Centre().Border(wxALL,1) );

  mainPanel->SetSizer( mainSizer );
  
  wxConfigBase *config = wxConfigBase::Get();
  chooseFilePanel_->GetFilenames( config, wxT("/StackDialog") );

  wxPersistentRegisterAndRestore(this, "StackDialog");
  Show( true );

  if( ExGlobals::StackIsOn() )
  {
    onOffRB_->SetSelection( 0 );
    chooseFilePanel_->SetFile( ExGlobals::GetStackFile() );
    chooseFilePanel_->Enable( true );
  }
  else
  {
    onOffRB_->SetSelection( 1 );
    chooseFilePanel_->Enable( false );
  }
}

void StackDialog::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    chooseFilePanel_->SaveFilenames( config, wxT("/StackDialog") );
  }
  //
  // close all child windows
  /*
  wxWindowList children( GetChildren() );
  wxWindowListNode *node = children.GetFirst();
  while( node )
  {
    wxWindow *window = node->GetData();
    window->Close(true);
    node = node->GetNext();
  }
  */
  analysisWindow_->ZeroStackDialog();
  Destroy();
}

void StackDialog::OnOffToggle( wxCommandEvent &WXUNUSED(event) )
{
  Toggle();
}

void StackDialog::Toggle()
{
  chooseFilePanel_->Enable( onOffRB_->GetSelection()==0 );
}

void StackDialog::OnApply( wxCommandEvent &WXUNUSED(event) )
{
  if( onOffRB_->GetSelection() == 1 )
  {
    ExGlobals::SetStackOff();
  }
  else
  {
    wxString fileName( chooseFilePanel_->GetSelection() );
    if( fileName.empty() )
    {
      wxMessageBox( wxT("no file has been chosen"),
                    wxT("Warning"), wxOK|wxICON_INFORMATION, this );
      return;
    }
    if( !ExGlobals::SetStackOn(fileName,appendCkB_->IsChecked()) )
    {
      wxMessageBox( wxString(wxT("could not open "))+fileName,
                    wxT("Warning"), wxOK|wxICON_INFORMATION, this );
      return;
    }
  }
}

void StackDialog::OnClose( wxCommandEvent &WXUNUSED(event) )
{
  Close();
}
  
// end of file
