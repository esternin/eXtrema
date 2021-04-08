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
#include <iostream>

#include "wx/config.h"

#include "ChooseFilePanel.h"
#include "ExGlobals.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( ChooseFilePanel, wxPanel )
  EVT_BUTTON( ID_chooseFile, ChooseFilePanel::OnChooseFile )
  EVT_BUTTON( ID_clearList, ChooseFilePanel::OnClearList )
  EVT_BUTTON( ID_removeFilename, ChooseFilePanel::OnRemoveFilename )
END_EVENT_TABLE()

ChooseFilePanel::ChooseFilePanel( wxWindow *parent, bool read, wxString const &message, wxString const &fileType )
    : wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER ),
      read_(read), message_(message), fileType_(fileType)
{
  wxString imageDir = ExGlobals::GetImagePath();

  wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
  wxButton *chooseFileButton = new wxButton( this, ID_chooseFile, wxT("Browse"), wxDefaultPosition, wxSize(100,25) );
  chooseFileButton->SetToolTip( wxT("click to open a file browser") );
  sizer->Add( chooseFileButton, wxSizerFlags(0).Border(wxALL,5) );
  filenameComboBox_ = new wxComboBox( this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(350,25),
                                      0, 0, wxCB_READONLY );
  sizer->Add( filenameComboBox_, wxSizerFlags(1).Border(wxALL,5) );
  wxBitmapButton *clearListButton =
      new wxBitmapButton( this, ID_clearList, wxBitmap(imageDir+wxT("/executeClearList.GIF"),wxBITMAP_TYPE_GIF) );
  clearListButton->SetToolTip( wxT("remove all files from the list") );
  sizer->Add( clearListButton, wxSizerFlags(0).Border(wxALL,5) );
  wxBitmapButton *removeFilenameButton =
    new wxBitmapButton( this, ID_removeFilename,
                        wxBitmap(imageDir+wxT("/executeRemoveFilename.GIF"),wxBITMAP_TYPE_GIF) );
  removeFilenameButton->SetToolTip( wxT("remove the displayed file from the list") );
  sizer->Add( removeFilenameButton, wxSizerFlags(0).Border(wxALL,5) );
  SetSizer( sizer );
}

void ChooseFilePanel::GetFilenames( wxConfigBase *config, wxString const &iniString )
{
  int count = config->Read( iniString+wxT("/FILECOUNT"), 0l );
  int currentIndex = config->Read( iniString+wxT("/CURRENTINDEX"), 0l );
  for( int i=0; i<count; ++i )
  {
    wxString name;
    if( config->Read((iniString+wxT("/FILE"))<<i,&name) )filenameComboBox_->Append( name );
    else std::cerr << "not ok\n";
  }
  filenameComboBox_->SetSelection( currentIndex );
}

void ChooseFilePanel::SaveFilenames( wxConfigBase *config, wxString const &iniString )
{
  int currentIndex = filenameComboBox_->GetSelection();
  config->Write( iniString+wxT("/CURRENTINDEX"), static_cast<long>(currentIndex) );
  int count = filenameComboBox_->GetCount();
  if( count == 0 )
  {
    int oldCount = config->Read( iniString+wxT("/FILECOUNT"), 0l );
    for( int i=0; i<oldCount; ++i )
      config->DeleteEntry( (iniString+wxT("/FILE"))<<i );
  }
  config->Write( iniString+wxT("/FILECOUNT"), static_cast<long>(count) );
  for( int i=0; i<count; ++i )
  {
    filenameComboBox_->SetSelection( i );
    config->Write( (iniString+wxT("/FILE"))<<i, filenameComboBox_->GetValue() );
  }
}

void ChooseFilePanel::OnChooseFile( wxCommandEvent &WXUNUSED(event) )
{
  wxFileDialog *fd;
  std::ios_base::openmode mode;
  if( read_ )
  {
    fd = new wxFileDialog( this, message_, wxT(""), wxT(""), fileType_,
                           wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
    mode = std::ios_base::in;
  }
  else
  {
    fd = new wxFileDialog( this, message_, wxT(""), wxT(""), fileType_,
                           wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR );
    mode = std::ios_base::out;
  }
  wxString filename;
  if( fd->ShowModal() == wxID_OK )filename = fd->GetPath();
  if( filename.empty() )return;
  std::fstream f( filename.mb_str(wxConvUTF8), mode );
  if( !f.is_open() )
  {
    wxMessageDialog *md =
        new wxMessageDialog( (wxWindow*)this, wxString(wxT("could not open "))+filename,
                             wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  f.close();
  SetFile( filename );
}

void ChooseFilePanel::SetFile( wxString const &filename )
{
  int count = filenameComboBox_->GetCount();
  for( int i=0; i<count; ++i )
  {
    if( filename == filenameComboBox_->GetString(i) ) // filename is already in the list
    {
      filenameComboBox_->SetSelection( i );
      return;
    }
  }
  filenameComboBox_->Append( filename );
  filenameComboBox_->SetSelection( filenameComboBox_->GetCount()-1 );
}

void ChooseFilePanel::OnClearList( wxCommandEvent &WXUNUSED(event) )
{
  filenameComboBox_->Clear();
  filenameComboBox_->SetValue( wxT("") );
}

void ChooseFilePanel::OnRemoveFilename( wxCommandEvent &WXUNUSED(event) )
{ RemoveFilename(); }

void ChooseFilePanel::RemoveFilename()
{
  if( filenameComboBox_->IsListEmpty() )return;
  int i = filenameComboBox_->GetSelection();
  filenameComboBox_->Delete( i );
  i > 0 ? filenameComboBox_->SetSelection(i-1) : filenameComboBox_->SetSelection(0);
  if( filenameComboBox_->IsTextEmpty() )filenameComboBox_->SetValue( wxT("") );
}

wxString ChooseFilePanel::GetSelection()
{ return filenameComboBox_->GetValue(); }

// end of file
