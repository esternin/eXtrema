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
#include <stdexcept>

#include "AnalysisSpeedButtonPanel.h"
#include "AnalysisWindow.h"
#include "ExGlobals.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them

BEGIN_EVENT_TABLE( AnalysisSpeedButtonPanel, wxPanel )
  EVT_BUTTON( ID_execute, AnalysisSpeedButtonPanel::OnExecute )
  EVT_BUTTON( ID_show, AnalysisSpeedButtonPanel::OnShowVariables )
  EVT_BUTTON( ID_fit, AnalysisSpeedButtonPanel::OnFit )
  EVT_BUTTON( ID_generate, AnalysisSpeedButtonPanel::OnGenerateVector )
  EVT_BUTTON( ID_constants, AnalysisSpeedButtonPanel::OnConstants )
  EVT_BUTTON( ID_saveSession, AnalysisSpeedButtonPanel::OnSaveSession )
  EVT_BUTTON( ID_restoreSession, AnalysisSpeedButtonPanel::OnRestoreSession )
  EVT_BUTTON( ID_clearMessages, AnalysisSpeedButtonPanel::OnClearMessages )
  EVT_BUTTON( ID_stackToggle, AnalysisSpeedButtonPanel::OnStackToggle )
  EVT_BUTTON( wxID_HELP, AnalysisSpeedButtonPanel::OnHelp )
  EVT_BUTTON( wxID_EXIT, AnalysisSpeedButtonPanel::OnExit )
END_EVENT_TABLE()

AnalysisSpeedButtonPanel::AnalysisSpeedButtonPanel( AnalysisWindow *parent )
    : wxPanel(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxRAISED_BORDER|wxTAB_TRAVERSAL),
      analysisWindow_(parent)
{
  wxString imageDir = ExGlobals::GetImagePath();
  wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );

  wxBitmapButton *executeButton =
      new wxBitmapButton( this, ID_execute, wxBitmap(imageDir+wxT("/execute.GIF"),wxBITMAP_TYPE_GIF) );
  executeButton->SetToolTip( wxT("execute a command script") );
  sizer->Add( executeButton, wxSizerFlags(0).Border(wxALL,2) );

  wxBitmapButton *showButton =
      new wxBitmapButton( this, ID_show, wxBitmap(imageDir+wxT("/show.GIF"),wxBITMAP_TYPE_GIF) );
  showButton->SetToolTip( wxT("show existing variables") );
  sizer->Add( showButton, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxRIGHT,2) );

  wxBitmapButton *fitButton =
      new wxBitmapButton( this, ID_fit, wxBitmap(imageDir+wxT("/fit.GIF"),wxBITMAP_TYPE_GIF) );
  fitButton->SetToolTip( wxT("fit a function to data") );
  sizer->Add( fitButton, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxRIGHT,2) );

  wxBitmapButton *generateButton =
    new wxBitmapButton( this, ID_generate, wxBitmap(imageDir+wxT("/generate.GIF"),wxBITMAP_TYPE_GIF) );
  generateButton->SetToolTip( wxT("generate a vector") );
  sizer->Add( generateButton, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxRIGHT,2) );

  wxBitmapButton *constantsButton =
    new wxBitmapButton( this, ID_constants, wxBitmap(imageDir+wxT("/constants.GIF"),wxBITMAP_TYPE_GIF) );
  constantsButton->SetToolTip( wxT("define mathematical and physical constants") );
  sizer->Add( constantsButton, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxRIGHT,2) );

  sizer->InsertStretchSpacer( 5, 1 );

  wxBitmapButton *stackToggleButton =
      new wxBitmapButton( this, ID_stackToggle, wxBitmap(imageDir+wxT("/stack.GIF"),wxBITMAP_TYPE_GIF) );
  stackToggleButton->SetToolTip( wxT("toggle recording of commands on/off") );
  sizer->Add( stackToggleButton, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxLEFT,2) );

  wxBitmapButton *saveSessionButton =
    new wxBitmapButton( this, ID_saveSession, wxBitmap(imageDir+wxT("/save.GIF"),wxBITMAP_TYPE_GIF) );
  saveSessionButton->SetToolTip( wxT("save the current session to a file") );
  sizer->Add( saveSessionButton, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxLEFT,2) );

  wxBitmapButton *restoreSessionButton =
    new wxBitmapButton( this, ID_restoreSession,
                        wxBitmap(imageDir+wxT("/restore.GIF"),wxBITMAP_TYPE_GIF) );
  restoreSessionButton->SetToolTip( wxT("restore a previously saved session") );
  sizer->Add( restoreSessionButton, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxLEFT,2) );

  wxBitmapButton *clearMessagesButton =
      new wxBitmapButton( this, ID_clearMessages,
                          wxBitmap(imageDir+wxT("/clear.GIF"),wxBITMAP_TYPE_GIF) );
  clearMessagesButton->SetToolTip( wxT("clear the message window") );
  sizer->Add( clearMessagesButton, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxLEFT,2) );
  
  wxBitmapButton *helpButton =
      new wxBitmapButton( this, wxID_HELP,
                          wxBitmap(imageDir+wxT("/help.GIF"),wxBITMAP_TYPE_GIF) );
  helpButton->SetToolTip( wxT("help contents") );
  sizer->Add( helpButton, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxLEFT,2) );
  
  wxBitmapButton *exitButton =
      new wxBitmapButton( this, wxID_EXIT,
                          wxBitmap(imageDir+wxT("/exit.GIF"),wxBITMAP_TYPE_GIF) );
  exitButton->SetToolTip( wxT("quit the program (with confirmation)") );
  sizer->Add( exitButton, wxSizerFlags(0).Border(wxALL,2) );

  SetSizer( sizer );
}

void AnalysisSpeedButtonPanel::OnConstants( wxCommandEvent &WXUNUSED(event) )
{
  ExGlobals::DefineConstants();
  wxMessageBox( wxT("Many mathematical and physical constants are now available"), wxT("info"), wxOK );
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("DEFINE\\CONSTANTS") );
}

void AnalysisSpeedButtonPanel::OnClearMessages( wxCommandEvent &WXUNUSED(event) )
{
  analysisWindow_->ClearOutput();
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("CLEAR\\HISTORY") );
}

void AnalysisSpeedButtonPanel::OnExit( wxCommandEvent &WXUNUSED(event) )
{
  wxMessageDialog md( (wxWindow*)this,
                           wxT("Do you really want to quit?"),
                           wxT("Confirm quit"),
                           wxYES_NO|wxICON_QUESTION );
  if( md.ShowModal() == wxID_YES )
  {
    if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("QUIT") );
    GetParent()->Close(true);
  }
}

void AnalysisSpeedButtonPanel::OnShowVariables( wxCommandEvent &event )
{ analysisWindow_->OnShowVariables( event ); }

void AnalysisSpeedButtonPanel::OnExecute( wxCommandEvent &event )
{ analysisWindow_->OnExecute( event ); }

void AnalysisSpeedButtonPanel::OnGenerateVector( wxCommandEvent &event )
{ analysisWindow_->OnGenerateVector( event ); }

void AnalysisSpeedButtonPanel::OnFit( wxCommandEvent &event )
{ analysisWindow_->OnFit( event ); }

void AnalysisSpeedButtonPanel::OnHelp( wxCommandEvent &WXUNUSED(event) )
{
  ExGlobals::StartHelp();
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("HELP") );
}

void AnalysisSpeedButtonPanel::OnStackToggle( wxCommandEvent &event )
{ analysisWindow_->OnStackToggle( event ); }

void AnalysisSpeedButtonPanel::OnSaveSession( wxCommandEvent &WXUNUSED(event) )
{
  wxFileDialog fd( this, wxT("Save session"), wxT(""), wxT(""),
                         wxT("xml files (*.xml)|*.xml"),
                         wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR );
  wxString filename;
  if( fd.ShowModal() == wxID_OK )filename = fd.GetPath();
  if( filename.empty() )return;
  if( filename.find_last_of(wxT('.')) == filename.npos )filename += wxT(".xml");
  std::ofstream f( filename.mb_str(wxConvUTF8), std::ios_base::out );
  if( !f.is_open() )
  {
    wxMessageBox( wxString()<<wxT("could not open ")<<filename,
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  f.close();
  try
  {
    ExGlobals::SaveSession( filename );
  }
  catch ( std::runtime_error const &e )
  {
    wxMessageBox( wxString::FromUTF8(e.what()),
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
}

void AnalysisSpeedButtonPanel::OnRestoreSession( wxCommandEvent &WXUNUSED(event) )
{
  wxFileDialog fd( this, wxT("Save session"), wxT(""), wxT(""),
                         wxT("xml files (*.xml)|*.xml"),
                         wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  wxString filename;
  if( fd.ShowModal() == wxID_OK )filename = fd.GetPath();
  if( filename.empty() )return;
  std::ofstream f( filename.mb_str(wxConvUTF8), std::ios_base::in );
  if( !f.is_open() )
  {
    wxMessageBox( wxString()<<wxT("could not open ")<<filename,
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  f.close();
  try
  {
    ExGlobals::RestoreSession( filename );
  }
  catch ( std::runtime_error const &e )
  {

    std::cout << "error: |" << e.what() << "|\n";

    wxMessageBox( wxString(e.what(),wxConvUTF8),
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
}

// end of file
