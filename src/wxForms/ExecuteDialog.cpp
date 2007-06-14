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
#include <stdexcept>

#include "wx/config.h"
#include "wx/filename.h"

#include "ExecuteDialog.h"
#include "ChooseFilePanel.h"
#include "AnalysisWindow.h"
#include "ParseLine.h"
#include "CMD_execute.h"
#include "ESyntaxError.h"
#include "ECommandError.h"
#include "ExGlobals.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( ExecuteDialog, wxFrame )
  EVT_BUTTON( wxID_APPLY, ExecuteDialog::OnApply )
  EVT_BUTTON( wxID_CLOSE, ExecuteDialog::OnClose )
  EVT_CLOSE( ExecuteDialog::CloseEventHandler )
END_EVENT_TABLE()

ExecuteDialog::ExecuteDialog( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("execute a script"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      analysisWindow_(parent)
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );

  chooseFilePanel_ = new ChooseFilePanel( this, true, wxT("Choose an extrema script file"),
                                   wxT("script file|*.pcm|stack file|*.stk|any file|*.*") );
  mainSizer->Add( chooseFilePanel_, wxSizerFlags(1).Expand().Border(wxALL,1) );

  wxPanel *midPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize(400,100), wxNO_BORDER );
  wxBoxSizer *midSizer = new wxBoxSizer( wxHORIZONTAL );
  wxStaticText *parameterLabel =
    new wxStaticText( midPanel, wxID_ANY, wxT("Parameter(s): "), wxDefaultPosition, wxDefaultSize );
  midSizer->Add( parameterLabel, wxSizerFlags(0).Right().Border(wxALL,10) );
  parameterTextCtrl_ = new wxTextCtrl( midPanel, wxID_ANY, wxT(""),
                                       wxDefaultPosition, wxSize(350,20) );
  parameterTextCtrl_->SetToolTip( wxT("enter any required parameters") );
  midSizer->Add( parameterTextCtrl_, wxSizerFlags(1).Left().Border(wxALL,10) );
  midPanel->SetSizer( midSizer );
  mainSizer->Add( midPanel, wxSizerFlags(1).Expand().Border(wxALL,1) );
  
  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize(400,100), wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );
  wxButton *applyButton = new wxButton( bottomPanel, wxID_APPLY, wxT("Apply") );
  applyButton->SetToolTip( wxT("execute the chosen file") );
  bottomSizer->Add( applyButton, wxSizerFlags(0).Border(wxALL,10) );
  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );
  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(1).Expand().Centre().Border(wxALL,1) );

  SetSizer( mainSizer );
  
  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/ExecuteDialog/UPPERLEFTX"), 0l );
  int uly = config->Read( wxT("/ExecuteDialog/UPPERLEFTY"), 640l );
  int width = config->Read( wxT("/ExecuteDialog/WIDTH"), 570l );
  int height = config->Read( wxT("/ExecuteDialog/HEIGHT"), 145l );
  SetSize( ulx, uly, width, height );

  chooseFilePanel_->GetFilenames( config, wxT("/ExecuteDialog") );
  
  wxString tmp;
  config->Read( wxT("/ExecuteDialog/PARAMETERS"), &tmp );
  if( !tmp.empty() )
  {
    ExGlobals::RemoveQuotes( tmp );
    parameterTextCtrl_->SetValue( tmp );
  }
  Show( true );
}

void ExecuteDialog::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/ExecuteDialog/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/ExecuteDialog/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/ExecuteDialog/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/ExecuteDialog/HEIGHT"), static_cast<long>(height) );

    chooseFilePanel_->SaveFilenames( config, wxT("/ExecuteDialog") );
    
    if( !parameterTextCtrl_->GetValue().empty() )
      config->Write( wxT("/ExecuteDialog/PARAMETERS"), parameterTextCtrl_->GetValue() );
  }
  //
  // close all child windows
  //
  wxWindowList children( GetChildren() );
  wxWindowListNode *node = children.GetFirst();
  while( node )
  {
    wxWindow *window = node->GetData();
    window->Close(true);
    node = node->GetNext();
  }
  analysisWindow_->ZeroExecuteDialog();
  Destroy();
}

void ExecuteDialog::OnApply( wxCommandEvent &WXUNUSED(event) )
{
  Apply();
}

void ExecuteDialog::Apply()
{
  wxString fileName( chooseFilePanel_->GetSelection() );
  if( fileName.empty() )
  {
    wxMessageDialog *md =
      new wxMessageDialog( this, wxT("no file has been chosen"), wxT("Warning"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return;
  }
  wxFileName wxfn( fileName );
  ExGlobals::SetCurrentPath( wxfn.GetPath() );
  //
  wxString commandLine( wxT("EXECUTE ") );
  commandLine += fileName;
  if( !parameterTextCtrl_->GetValue().empty() )
    commandLine += wxT(' ') + parameterTextCtrl_->GetValue();
  ParseLine p( commandLine );
  try
  {
    p.ParseIt();
  }
  catch( ESyntaxError &e )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxString(e.what(),wxConvUTF8),
                                               wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  try
  {
    CMD_execute::Instance()->Execute( &p );
  }
  catch( ECommandError &e )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxString(e.what(),wxConvUTF8),
                                               wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  if( ExGlobals::StackIsOn() )
  {
    wxString line( wxT("FORM EXECUTE,'") );
    line += fileName;
    wxString p( parameterTextCtrl_->GetValue() );
    if( p.empty() )line += wxT("',,APPLY");
    else line += wxT("','") + p + wxT("',APPLY");
    ExGlobals::WriteStack( line );
  }
  // this must be the top level script since it is run interactively
  //
  try
  {
    ExGlobals::RunScript();
  }
  catch ( std::runtime_error &e )
  {
    ExGlobals::ShowScriptError( e.what() );
    ExGlobals::StopAllScripts();
  }
}

void ExecuteDialog::OnClose( wxCommandEvent &WXUNUSED(event) )
{
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("FORM EXECUTE,CLOSE") );
  Close();
}

void ExecuteDialog::Set( wxString const &filename, wxString const &parameters, wxString const &action )
{
  chooseFilePanel_->SetFile( filename );
  parameterTextCtrl_->SetValue( parameters );
  if( action == wxT("CLOSE") )Close();
  else if( action == wxT("APPLY") )Apply();
}
  
// end of file
