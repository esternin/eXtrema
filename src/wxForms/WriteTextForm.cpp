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
#include <sstream>

#include "wx/config.h"
#include "wx/statline.h"

#include <wx/persist/toplevel.h>

#include "WriteTextForm.h"
#include "AnalysisWindow.h"
#include "ChooseFilePanel.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "TVariableTable.h"
#include "CMD_write.h"

BEGIN_EVENT_TABLE( WriteTextForm, wxFrame )
  EVT_BUTTON( wxID_APPLY, WriteTextForm::OnApply )
  EVT_BUTTON( wxID_CLOSE, WriteTextForm::OnClose )
  EVT_BUTTON( wxID_REFRESH, WriteTextForm::OnRefresh )
  EVT_CLOSE( WriteTextForm::CloseEventHandler )
END_EVENT_TABLE()

WriteTextForm::WriteTextForm( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Write text variables"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      analysisWindow_(parent)
{
  wxPanel* const mainPanel = new wxPanel(this);

  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->InsertSpacer( 0, 10 );
  
  topPanel_ = new ChooseFilePanel( mainPanel, false, wxT("Choose a data file for writing"), wxT("any file|*.*") );
  mainSizer->Add( topPanel_, wxSizerFlags(0).Border(wxALL,1) );

  variableList_ = new wxCheckListBox( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                      0, 0, wxLB_MULTIPLE );
  FillList();
  mainSizer->Add( variableList_, wxSizerFlags(1).Expand().Border(wxALL,5) );

  append_ = new wxCheckBox( mainPanel, wxID_ANY, wxT("Append data to the file") );
  mainSizer->Add( append_, wxSizerFlags(0).Border(wxALL,5) );
  append_->SetValue( false );

  wxPanel *bottomPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );

  wxButton *refreshButton = new wxButton( bottomPanel, wxID_REFRESH, wxT("Refresh") );
  refreshButton->SetToolTip( wxT("update the list of text above") );
  bottomSizer->Add( refreshButton, wxSizerFlags(0).Border(wxALL,10) );

  wxButton *applyButton = new wxButton( bottomPanel, wxID_APPLY, wxT("Apply") );
  applyButton->SetToolTip( wxT("write data to the file chosen above") );
  bottomSizer->Add( applyButton, wxSizerFlags(0).Border(wxALL,10) );

  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );

  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );

  mainPanel->SetSizer( mainSizer );
  
  wxConfigBase *config = wxConfigBase::Get();
  topPanel_->GetFilenames( config, wxT("/WriteTextForm") );

  wxPersistentRegisterAndRestore(this, "WriteTextForm");
  Show( true );
}

void WriteTextForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    topPanel_->SaveFilenames( config, wxT("/WriteTextForm") );
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
  analysisWindow_->ZeroWriteText();
  Destroy();
}

void WriteTextForm::OnRefresh( wxCommandEvent &WXUNUSED(event) )
{ FillList(); }

void WriteTextForm::FillList()
{
  variableList_->Clear();
  TVariableTable *tvTable = TVariableTable::GetTable();
  int end = tvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    TextVariable *tv = tvTable->GetEntry( i );
    if( tv->GetData().GetNumberOfDimensions() == 0 )variableList_->Append( tv->GetName() );
    else
    {
      std::size_t len = tv->GetData().GetData().size();
      for( std::size_t j=0; j<len; ++j )
      {
        wxString name( tv->GetName() );
        variableList_->Append( name<<wxT("[")<<(j+1)<<wxT("]") );
      }
    }
  }
}

void WriteTextForm::OnApply( wxCommandEvent &WXUNUSED(event) )
{
  wxString fileName( topPanel_->GetSelection() );
  if( fileName.empty() )
  {
    wxMessageBox( wxT("no file has been chosen"),
                  wxT("Fatal error: nothing was written"), wxOK|wxICON_INFORMATION, this );
    return;
  }
  CMD_write *writeCommand = CMD_write::Instance();
  writeCommand->SetFilename( fileName );
  std::ofstream *outStream = writeCommand->GetStream();
  if( outStream->is_open() )outStream->close();
  outStream->clear( std::ios::goodbit );
  if( append_->IsChecked() )outStream->open( fileName.mb_str(wxConvUTF8), std::ios::out|std::ios::app );
  else                      outStream->open( fileName.mb_str(wxConvUTF8), std::ios::out );
  if( !outStream->is_open() )
  {
    outStream->clear( std::ios::goodbit );
    wxMessageBox( wxString(wxT("Could not open "))+fileName,
                  wxT("Fatal error: nothing was written"), wxOK|wxICON_ERROR, this );
    return;
  }
  bool somethingChecked = false;
  int count = variableList_->GetCount();
  for( int i=0; i<count; ++i )
  {
    if( !variableList_->IsChecked(i) )continue;
    somethingChecked = true;
    wxString line;
    wxString name( variableList_->GetString(i) );
    try
    {
      TextVariable::GetVariable( name, false, line );
    }
    catch( EVariableError const &e )
    {
      outStream->close();
      wxMessageBox( wxString(e.what(),wxConvUTF8),
                    wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
    (*outStream) << line.mb_str(wxConvUTF8) << "\n";
  }
  outStream->close();
  if( !somethingChecked )
  {
    wxMessageBox( wxT("no string variables were chosen"),
                  wxT("Warning: nothing was written"), wxOK|wxICON_ERROR, this );
    return;
  }
}

void WriteTextForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

// end of file
