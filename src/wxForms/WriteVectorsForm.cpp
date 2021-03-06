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

#include "WriteVectorsForm.h"
#include "AnalysisWindow.h"
#include "ChooseFilePanel.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "CMD_write.h"

BEGIN_EVENT_TABLE( WriteVectorsForm, wxFrame )
  EVT_BUTTON( wxID_APPLY, WriteVectorsForm::OnApply )
  EVT_BUTTON( wxID_CLOSE, WriteVectorsForm::OnClose )
  EVT_BUTTON( wxID_REFRESH, WriteVectorsForm::OnRefresh )
  EVT_CLOSE( WriteVectorsForm::CloseEventHandler )
END_EVENT_TABLE()

WriteVectorsForm::WriteVectorsForm( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Write vectors"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
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
  refreshButton->SetToolTip( wxT("update the list of vectors above") );
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
  topPanel_->GetFilenames( config, wxT("/WriteVectorsForm") );

  wxPersistentRegisterAndRestore(this, "WriteVectorsForm");
  Show( true );
}

void WriteVectorsForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    topPanel_->SaveFilenames( config, wxT("/WriteVectorsForm") );
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
  analysisWindow_->ZeroWriteVectors();
  Destroy();
}

void WriteVectorsForm::OnRefresh( wxCommandEvent &WXUNUSED(event) )
{ FillList(); }

void WriteVectorsForm::FillList()
{
  variableList_->Clear();
  NVariableTable *nvTable = NVariableTable::GetTable();
  int end = nvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry( i );
    if( nv->GetData().GetNumberOfDimensions() == 1 )variableList_->Append( nv->GetName() );
  }
}

void WriteVectorsForm::OnApply( wxCommandEvent &WXUNUSED(event) )
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
  std::vector<wxString> names;
  std::vector< std::vector<double> > data;
  std::vector<double> d;
  std::vector<int> nd1;
  int count = variableList_->GetCount();
  for( int i=0; i<count; ++i )
  {
    if( !variableList_->IsChecked(i) )continue;
    int ndm;
    double value;
    int dimSizes[3];
    std::vector<double>().swap( d );
    wxString name( variableList_->GetString(i) );
    try
    {
      NumericVariable::GetVariable( name, ndm, value, d, dimSizes );
    }
    catch( EVariableError const &e )
    {
      outStream->close();
      wxMessageBox( wxString(e.what(),wxConvUTF8),
                    wxT("Fatal error: nothing was written"), wxOK|wxICON_ERROR, this );
      return;
    }
    names.push_back( name );
    data.push_back( d );
    nd1.push_back( dimSizes[0] );
  }
  if( names.empty() )
  {
    outStream->close();
    wxMessageBox( wxT("no vectors were chosen"),
                  wxT("Warning: nothing was written"), wxOK|wxICON_ERROR, this );
    return;
  }
  std::vector<int> mm( names.size(), 0 );
  for( ;; )
  {
    std::vector<double>().swap( d );
    std::size_t end = names.size();
    for( std::size_t i=0; i<end; ++i )
    {
      if( ++mm[i] <= nd1[i] )d.push_back( data[i][mm[i]-1] );
    }
    if( d.empty() )break;
    end = d.size();
    for( std::size_t i=0; i<end; ++i )(*outStream) << d[i] << " ";
    (*outStream) << "\n";
  }
  outStream->close();
}

void WriteVectorsForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

// end of file
