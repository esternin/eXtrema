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
#include "wx/stattext.h"

#include <wx/persist/toplevel.h>

#include "ReadTextForm.h"
#include "ChooseFilePanel.h"
#include "AnalysisWindow.h"
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "TextVariable.h"
#include "ExGlobals.h"
#include "CMD_read.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( ReadTextForm, wxFrame )
  EVT_BUTTON( wxID_APPLY, ReadTextForm::OnApply )
  EVT_BUTTON( wxID_CLOSE, ReadTextForm::OnClose )
  EVT_CHECKBOX( ID_lineRange, ReadTextForm::OnLineRange )
  EVT_CLOSE( ReadTextForm::CloseEventHandler )
END_EVENT_TABLE()

ReadTextForm::ReadTextForm( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Read text"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      analysisWindow_(parent)
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->InsertSpacer( 0, 10 );

  topPanel_ = new ChooseFilePanel( this, true,
                                   wxT("Choose a data file for reading"), wxT("any file|*.*") );
  mainSizer->Add( topPanel_, wxSizerFlags(0).Border(wxALL,1) );

  wxPanel *varNamePanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );
  wxGridSizer *varNameSizer = new wxGridSizer( 1, 2, 0, 0 );

  varNameSizer->Add(
    new wxStaticText(varNamePanel,wxID_ANY,wxT("Variable name"),wxDefaultPosition,wxDefaultSize,wxALIGN_RIGHT),
    wxSizerFlags(0).Right().Border(wxTOP,10) );
  varNameTextCtrl_ = new wxTextCtrl( varNamePanel, wxID_ANY );
  varNameTextCtrl_->SetValue( wxT("") );
  varNameSizer->Add( varNameTextCtrl_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  varNamePanel->SetSizer( varNameSizer );
  mainSizer->Add( varNamePanel, wxSizerFlags(0).Center().Border(wxALL,1) );

  wxPanel *midPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *midSizer = new wxBoxSizer( wxHORIZONTAL );

  wxPanel *leftPanel = new wxPanel( midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );
  wxBoxSizer *leftSizer = new wxBoxSizer( wxVERTICAL );

  closeBefore_ = new wxCheckBox( leftPanel, wxID_ANY, wxT("Close file before reading") );
  leftSizer->Add( closeBefore_, wxSizerFlags(0).Border(wxALL,2) );
  closeBefore_->SetValue( true );

  closeAfter_ = new wxCheckBox( leftPanel, wxID_ANY, wxT("Close file after reading") );
  leftSizer->Add( closeAfter_, wxSizerFlags(0).Border(wxALL,2) );
  closeAfter_->SetValue( true );

  leftSizer->Add( new wxStaticLine(leftPanel), wxSizerFlags(0).Expand().Border(wxALL,1) );

  messages_ = new wxCheckBox( leftPanel, wxID_ANY, wxT("Display messages") );
  leftSizer->Add( messages_, wxSizerFlags(0).Border(wxALL,2) );
  messages_->SetValue( true );

  leftPanel->SetSizer( leftSizer );
  midSizer->Add( leftPanel, wxSizerFlags(0).Border(wxALL,1) );

  wxPanel *rightPanel = new wxPanel( midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );
  wxBoxSizer *rightSizer = new wxBoxSizer( wxVERTICAL );

  lineRange_ = new wxCheckBox( rightPanel, ID_lineRange, wxT("Use a line range") );
  rightSizer->Add( lineRange_, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  wxPanel *midRightPanel = new wxPanel( rightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxGridSizer *midRightSizer = new wxGridSizer( 3, 2, 0, 0 );

  firstLineText_ = new wxStaticText( midRightPanel, wxID_ANY, wxT("First line to read"), wxDefaultPosition,
                                     wxDefaultSize, wxALIGN_RIGHT );
  midRightSizer->Add( firstLineText_, wxSizerFlags(0).Right().Border(wxTOP,10) );
  firstLineTextCtrl_ = new wxTextCtrl( midRightPanel, wxID_ANY );
  firstLineTextCtrl_->SetValue( wxT("1") );
  midRightSizer->Add( firstLineTextCtrl_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  lastLineText_ = new wxStaticText( midRightPanel, wxID_ANY, wxT("Last line to read"), wxDefaultPosition,
                                    wxDefaultSize, wxALIGN_RIGHT );
  midRightSizer->Add( lastLineText_, wxSizerFlags(0).Right().Border(wxTOP,10) );
  lastLineTextCtrl_ = new wxTextCtrl( midRightPanel, wxID_ANY );
  lastLineTextCtrl_->SetValue( wxT("1") );
  midRightSizer->Add( lastLineTextCtrl_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  incrementText_ = new wxStaticText( midRightPanel, wxID_ANY, wxT("Increment"), wxDefaultPosition,
                                     wxDefaultSize, wxALIGN_RIGHT );
  midRightSizer->Add( incrementText_, wxSizerFlags(0).Right().Border(wxTOP,10) );
  incrementTextCtrl_ = new wxTextCtrl( midRightPanel, wxID_ANY );
  incrementTextCtrl_->SetValue( wxT("1") );
  midRightSizer->Add( incrementTextCtrl_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  midRightPanel->SetSizer( midRightSizer );
  rightSizer->Add( midRightPanel, wxSizerFlags(0).Border(wxALL,1) );
  firstLineTextCtrl_->Enable( false );
  lastLineTextCtrl_->Enable( false );
  incrementTextCtrl_->Enable( false );
  firstLineText_->Enable( false );
  lastLineText_->Enable( false );
  incrementText_->Enable( false );

  rightPanel->SetSizer( rightSizer );
  midSizer->Add( rightPanel, wxSizerFlags(0).Border(wxALL,10) );
  midPanel->SetSizer( midSizer );

  mainSizer->Add( midPanel, wxSizerFlags(0).Center().Border(wxALL,1) );
  
  wxPanel *bottomPanel = new wxPanel( (wxWindow*)this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );

  wxButton *applyButton = new wxButton( bottomPanel, wxID_APPLY, wxT("Apply") );
  applyButton->SetToolTip( wxT("read data from the file chosen above") );
  bottomSizer->Add( applyButton, wxSizerFlags(0).Border(wxALL,10) );

  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );

  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );

  SetSizer( mainSizer );
  
  wxConfigBase *config = wxConfigBase::Get();
  topPanel_->GetFilenames( config, wxT("/ReadTextForm") );

  wxPersistentRegisterAndRestore(this, "ReadTextForm");
  Show( true );
}

void ReadTextForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    topPanel_->SaveFilenames( config, wxT("/ReadTextForm") );
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
  analysisWindow_->ZeroLoadText();
  Destroy();
}

void ReadTextForm::OnLineRange( wxCommandEvent &WXUNUSED(event) )
{
  if( lineRange_->IsChecked() )
  {
    firstLineTextCtrl_->Enable( true );
    lastLineTextCtrl_->Enable( true );
    incrementTextCtrl_->Enable( true );
    firstLineText_->Enable( true );
    lastLineText_->Enable( true );
    incrementText_->Enable( true );
  }
  else
  {
    firstLineTextCtrl_->Enable( false );
    lastLineTextCtrl_->Enable( false );
    incrementTextCtrl_->Enable( false );
    firstLineText_->Enable( false );
    lastLineText_->Enable( false );
    incrementText_->Enable( false );
  }
}

void ReadTextForm::OnApply( wxCommandEvent &WXUNUSED(event) )
{
  wxString fileName( topPanel_->GetSelection() );
  if( fileName.empty() )
  {
    wxMessageBox( wxT("no file has been chosen"),
                                               wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
    return;
  }
  CMD_read *readCommand = CMD_read::Instance();
  bool newFile = (readCommand->GetFilename() != fileName);
  readCommand->SetReadInFilename( fileName );
  std::ifstream *inStream = readCommand->GetStream();
  if( inStream->is_open() && (closeBefore_->IsChecked() || newFile) )inStream->close();
  if( !inStream->is_open() )
  {
    inStream->clear( std::ios::goodbit );
    inStream->open( fileName.mb_str(wxConvUTF8), std::ios_base::in );
    if( !inStream->is_open() )
    {
      wxMessageBox( wxString(wxT("Could not open "))+fileName,
                    wxT("Fatal error: nothing was read"), wxOK|wxICON_ERROR, this );
      return;
    }
  }
  wxString varName( varNameTextCtrl_->GetValue() );
  if( varName.empty() )
  {
    wxMessageBox( wxT("a text variable name must be entered"),
                  wxT("Fatal error: nothing was read"), wxOK|wxICON_ERROR, this );
    inStream->close();
    return;
  }
  int startingLine = 1;
  int lastLine = -1;
  int increment = 1;
  wxString stmp;
  long ltmp;
  if( lineRange_->IsChecked() )
  {
    stmp = firstLineTextCtrl_->GetValue();
    if( !stmp.ToLong(&ltmp) || ltmp<1L )
    {
      firstLineTextCtrl_->SetValue( wxT("1") );
      inStream->close();
      wxMessageBox( stmp+wxT(" is an invalid value for line range first line"),
                    wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
      return;
    }
    startingLine = static_cast<int>(ltmp);
    
    stmp = lastLineTextCtrl_->GetValue();
    if( !stmp.ToLong(&ltmp) || static_cast<int>(ltmp)<startingLine )
    {
      lastLineTextCtrl_->SetValue( wxT("1") );
      inStream->close();
      wxMessageBox( stmp+wxT(" is an invalid value for line range last line"),
                    wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
      return;
    }
    lastLine = static_cast<int>(ltmp);
    
    stmp = incrementTextCtrl_->GetValue();
    if( !stmp.ToLong(&ltmp) || ltmp<1L )
    {
      incrementTextCtrl_->SetValue( wxT("1") );
      inStream->close();
      wxMessageBox( stmp+wxT(" is an invalid value for line range increment"),
                    wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
      return;
    }
    increment = static_cast<int>(ltmp);
  }
  //
  // read in the initial dummy records
  //
  int last = startingLine+1; // last record read + 1
  //
  // there could only be initial dummy records if a line range was specified
  // and line ranges can only be on files with a record structure
  //
  unsigned int recordNumber = 0;
  for( int i=1; i<startingLine; ++i )
  {
    ++recordNumber;
    std::string buffer;
    if( std::getline(*inStream,buffer).fail() )
    {
      inStream->close();
      wxMessageBox( wxT("end of file reached reading initial dummy records"),
                    wxT("Fatal error: no vectors were modified"),
                    wxOK|wxICON_INFORMATION, this );
      return;
    }
  }
  //
  // finally ready to read some data
  //
  std::vector<wxString> data;
  for( std::size_t j=1;; ++j )
  {
    ++recordNumber;
    std::string sc;
    sc.clear();
    if( std::getline(*inStream,sc).fail() )break;
    if( sc.empty() )sc = " ";
    data.push_back( wxString(sc.c_str(),wxConvUTF8) );
    if( !lineRange_->IsChecked() )break;
    //
    // vector line range
    //
    bool stopReading = false;
    if( j >= (lastLine-startingLine)/increment+1 )break; // stop reading
    int next = startingLine + increment*recordNumber;
    for( int i=last; i<next; ++i )
    {
      ++recordNumber;
      std::string buffer;
      if( std::getline(*inStream,buffer).fail() )
      {
        stopReading = true;
        break;
      }
    }
    if( stopReading )break; // stop reading
    last = next+1;
  }
  if( closeAfter_->IsChecked() )inStream->close();
  //
  // finished reading
  //
  TextVariable *tv = TextVariable::PutVariable( varName, data, wxT("from gui") );
  tv->SetOrigin( fileName );
  if( messages_->IsChecked() )
    ExGlobals::WriteOutput( wxString(wxT("text variable "))+varName+wxT(" has been created") );
}

void ReadTextForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

// end of file
