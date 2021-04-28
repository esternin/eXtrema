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
#include "wx/filename.h"

#include <wx/persist/toplevel.h>

#include "ReadVectorsForm.h"
#include "ChooseFilePanel.h"
#include "AnalysisWindow.h"
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "TextVariable.h"
#include "EVariableError.h"
#include "ExGlobals.h"
#include "CMD_read.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( ReadVectorsForm, wxFrame )
  EVT_BUTTON( wxID_APPLY, ReadVectorsForm::OnApply )
  EVT_BUTTON( wxID_CLOSE, ReadVectorsForm::OnClose )
  EVT_GRID_CELL_CHANGED( ReadVectorsForm::OnCellChange )
  EVT_CHECKBOX( ID_makeNew, ReadVectorsForm::OnMakeNew )
  EVT_CHECKBOX( ID_stopOnError, ReadVectorsForm::OnErrorStop )
  EVT_CHECKBOX( ID_errorFill, ReadVectorsForm::OnErrorFill )
  EVT_CHECKBOX( ID_lineRange, ReadVectorsForm::OnLineRange )
  EVT_CHECKBOX( ID_format, ReadVectorsForm::OnFormat )
  EVT_CLOSE( ReadVectorsForm::CloseEventHandler )
END_EVENT_TABLE()

ReadVectorsForm::ReadVectorsForm( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Read vectors"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      analysisWindow_(parent)
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->InsertSpacer( 0, 10 );

  topPanel_ = new ChooseFilePanel( this, true, wxT("Choose a data file for reading"), wxT("any file|*.*") );
  mainSizer->Add( topPanel_, wxSizerFlags(0).Border(wxALL,1) );

  wxPanel *gridPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *gridSizer = new wxBoxSizer( wxHORIZONTAL );

  // create the vector name/column grid
  // column 0 is "vector name", column 1 is "column number"
  vectorGrid_ = new wxGrid( gridPanel, wxID_ANY, wxDefaultPosition, wxSize(375,250), wxSUNKEN_BORDER|wxVSCROLL );
  vectorGrid_->CreateGrid( 100, 2 );
  vectorGrid_->SetColFormatNumber( 1 );
  vectorGrid_->SetColLabelValue( 0, wxT("vector name") );
  vectorGrid_->SetColLabelValue( 1, wxT("column") );
  vectorGrid_->SetColSize( 0, 210 );
  vectorGrid_->SetColSize( 1, 60 );
  gridSizer->Add( vectorGrid_, wxSizerFlags(0).Border(wxALL,2) );
  gridPanel->SetSizer( gridSizer );

  mainSizer->Add( gridPanel, wxSizerFlags(0).Center().Border(wxALL,1) );

  wxPanel *midPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *midSizer = new wxBoxSizer( wxHORIZONTAL );

  wxPanel *leftPanel = new wxPanel( midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );
  wxBoxSizer *leftSizer = new wxBoxSizer( wxVERTICAL );

  closeBefore_ = new wxCheckBox( leftPanel, wxID_ANY, wxT("Close file before reading") );
  leftSizer->Add( closeBefore_, wxSizerFlags(0).Border(wxALL,5).Border(wxLEFT|wxRIGHT|wxBOTTOM,2) );
  closeBefore_->SetValue( true );

  closeAfter_ = new wxCheckBox( leftPanel, wxID_ANY, wxT("Close file after reading") );
  leftSizer->Add( closeAfter_, wxSizerFlags(0).Border(wxALL,2) );
  closeAfter_->SetValue( true );

  leftSizer->Add( new wxStaticLine(leftPanel), wxSizerFlags(0).Expand().Border(wxALL,1) );

  makeNew_ = new wxCheckBox( leftPanel, ID_makeNew, wxT("Create new vectors") );
  leftSizer->Add( makeNew_, wxSizerFlags(0).Border(wxALL,2) );

  append_ = new wxCheckBox( leftPanel, wxID_ANY, wxT("Append data") );
  leftSizer->Add( append_, wxSizerFlags(0).Border(wxALL,2) );

  leftSizer->Add( new wxStaticLine(leftPanel), wxSizerFlags(0).Expand().Border(wxALL,2) );

  errorStop_ = new wxCheckBox( leftPanel, ID_stopOnError, wxT("Stop on error") );
  leftSizer->Add( errorStop_, wxSizerFlags(0).Border(wxALL,2) );
  errorStop_->SetValue( true );

  wxPanel *errorFillPanel = new wxPanel( leftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *errorFillSizer = new wxBoxSizer( wxHORIZONTAL );

  errorFill_ = new wxCheckBox( errorFillPanel, ID_errorFill, wxT("Error fill") );
  errorFillSizer->Add( errorFill_, wxSizerFlags(0).Border(wxALL,2) );

  errorFillTextCtrl_ = new wxTextCtrl( errorFillPanel, wxID_ANY );
  errorFillTextCtrl_->SetValue( wxT("0.0") );
  errorFillSizer->Add( errorFillTextCtrl_, wxSizerFlags(0).Border(wxALL,2) );
  errorFillTextCtrl_->Enable( false );

  errorFillPanel->SetSizer( errorFillSizer );
  leftSizer->Add( errorFillPanel, wxSizerFlags(0).Border(wxALL,2) );

  leftSizer->Add( new wxStaticLine(leftPanel), wxSizerFlags(0).Expand().Border(wxALL,2) );

  messages_ = new wxCheckBox( leftPanel, wxID_ANY, wxT("Display messages") );
  leftSizer->Add( messages_, wxSizerFlags(0).Border(wxALL,2) );
  messages_->SetValue( true );

  leftPanel->SetSizer( leftSizer );
  midSizer->Add( leftPanel, wxSizerFlags(0).Border(wxALL,1) );

  wxPanel *rightPanel = new wxPanel( midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );
  wxBoxSizer *rightSizer = new wxBoxSizer( wxVERTICAL );

  lineRange_ = new wxCheckBox( rightPanel, ID_lineRange, wxT("Use line range") );
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
  lastLineTextCtrl_->SetValue( wxT("") );
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
  
  rightSizer->Add( new wxStaticLine(rightPanel), wxSizerFlags(0).Expand().Border(wxALL,10) );

  format_ = new wxCheckBox( rightPanel, ID_format, wxT("Use format") );
  rightSizer->Add( format_, wxSizerFlags(0).Center().Border(wxALL,1) );

  formatTextCtrl_ = new wxTextCtrl( rightPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(175,25) );
  rightSizer->Add( formatTextCtrl_, wxSizerFlags(0).Center().Border(wxALL,10) );
  formatTextCtrl_->Enable( false );

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
  topPanel_->GetFilenames( config, wxT("/ReadVectorsForm") );

  wxPersistentRegisterAndRestore(this, "ReadVectorsForm");
  Show( true );
}

void ReadVectorsForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    topPanel_->SaveFilenames( config, wxT("/ReadVectorsForm") );
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
  analysisWindow_->ZeroLoadVectors();
  Destroy();
}

void ReadVectorsForm::OnCellChange( wxGridEvent &WXUNUSED(event) )
{
  if( vectorGrid_->GetGridCursorCol() == 0 )
  {
    int row = vectorGrid_->GetGridCursorRow();
    wxString s;
    vectorGrid_->SetCellValue( row, 1, s<<(row+1) );
  }
}

void ReadVectorsForm::OnMakeNew( wxCommandEvent &WXUNUSED(event) )
{ append_->Enable( !makeNew_->IsChecked() ); }

void ReadVectorsForm::OnErrorStop( wxCommandEvent &WXUNUSED(event) )
{
  errorFill_->Enable( !errorStop_->IsChecked() );
  errorFillTextCtrl_->Enable( errorFill_->IsEnabled() );
}

void ReadVectorsForm::OnErrorFill( wxCommandEvent &WXUNUSED(event) )
{
  errorFillTextCtrl_->Enable( errorFill_->IsChecked() );
  format_->Enable( !errorFill_->IsEnabled() );
  formatTextCtrl_->Enable( !errorFill_->IsEnabled() );
}

void ReadVectorsForm::OnLineRange( wxCommandEvent &WXUNUSED(event) )
{
  bool const lrc = lineRange_->IsChecked();
  firstLineTextCtrl_->Enable( lrc );
  lastLineTextCtrl_->Enable( lrc );
  incrementTextCtrl_->Enable( lrc );
  firstLineText_->Enable( lrc );
  lastLineText_->Enable( lrc );
  incrementText_->Enable( lrc );
}

void ReadVectorsForm::OnFormat( wxCommandEvent &WXUNUSED(event) )
{ formatTextCtrl_->Enable( format_->IsChecked() ); }

void ReadVectorsForm::OnApply( wxCommandEvent &WXUNUSED(event) )
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
  readCommand->SetReadInFilename( fileName );
  //
  wxFileName wxfn( fileName );
  ExGlobals::SetCurrentPath( wxfn.GetPath() );
  //
  std::vector<wxString> names;
  std::vector<int> columnNumbers;
  int numberOfNames = vectorGrid_->GetNumberRows();
  int maxColumnNumber = 0;
  for( int i=0; i<numberOfNames; ++i )
  {
    wxString name( vectorGrid_->GetCellValue(i,0) );
    if( name.empty() )break;
    //
    // determine if the vector name has a field count prefix
    //
    names.push_back( name );
    wxString colm( vectorGrid_->GetCellValue(i,1) );
    if( colm.empty() || format_->IsChecked() )columnNumbers.push_back(i+1);
    else
    {
      long ltmp;
      if( !colm.ToLong(&ltmp) || ltmp<1L )
      {
        inStream->close();
        wxMessageBox( wxT("column number must be an integer > 0"),
                      wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
        return;
      }
      columnNumbers.push_back( static_cast<int>(ltmp) );
      if( maxColumnNumber < columnNumbers.back() )maxColumnNumber = columnNumbers.back();
    }
  }
  if( names.empty() )
  {
    inStream->close();
    wxMessageBox( wxT("expecting some output vector names"),
                  wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
    return;
  }
  numberOfNames = names.size();
  if( numberOfNames > 100 )
  {
    inStream->close();
    wxMessageBox( wxT("Maximum number of vectors that can be read is 100"),
                  wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
    return;
  }
  std::vector< std::vector<double> > data( maxColumnNumber );
  std::vector<unsigned int> nloc( numberOfNames, 0 );  // number of values to read
  for( std::size_t j=0; j<numberOfNames; ++j )
  {
    TextVariable *tv = TVariableTable::GetTable()->GetVariable( names[j] );
    if( tv ) // text variable exists already
    {
      TVariableTable::GetTable()->RemoveEntry( names[j] );
      continue;
    }
    NumericVariable *nv = NVariableTable::GetTable()->GetVariable( names[j] );
    if( nv ) // variable already exists
    {
      if( makeNew_->IsChecked() || nv->GetData().GetNumberOfDimensions()!=1 )
        NVariableTable::GetTable()->RemoveEntry( nv );
      else
      {
        nloc[j] = nv->GetData().GetDimMag(0);
        if( append_->IsChecked() )
          data[j].assign( nv->GetData().GetData().begin(), nv->GetData().GetData().end() );
      }
    }
  }
  std::vector<bool> fill( numberOfNames, true );
  //
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
      wxMessageBox( wxT("first line must be > 0"),
                    wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
      return;
    }
    startingLine = static_cast<int>(ltmp);
    
    stmp = lastLineTextCtrl_->GetValue();
    if( !stmp.empty() )
    {
      if( !stmp.ToLong(&ltmp) )
      {
        lastLineTextCtrl_->SetValue( wxT("") );
        inStream->close();
        wxMessageBox( stmp+wxT(" is an invalid value for last line"),
                      wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
        return;
      }
      lastLine = static_cast<int>(ltmp);
      if( lastLine < startingLine )
      {
        lastLineTextCtrl_->SetValue( wxT("") );
        inStream->close();
        wxMessageBox( wxT("last line must be >= first line"),
                      wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
        return;
      }
    }
    stmp = incrementTextCtrl_->GetValue();
    if( !stmp.ToLong(&ltmp) || ltmp<1L )
    {
      incrementTextCtrl_->SetValue( wxT("1") );
      inStream->close();
      wxMessageBox( wxT("increment must be > 0"),
                    wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
      return;
    }
    increment = static_cast<int>(ltmp);
  }
  double errorFillValue = readCommand->GetErrorFill();
  if( errorFill_->IsChecked() )
  {
    stmp = errorFillTextCtrl_->GetValue();
    if( !stmp.ToDouble(&errorFillValue) )
    {
      errorFillTextCtrl_->SetValue( wxT("0.0") );
      inStream->close();
      wxMessageBox( stmp+wxT(" is an invalid value for error fill"),
                    wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
      return;
    }
    readCommand->SetErrorFill( errorFillValue );
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
  std::string const blank( ", \t" );
  if( errorFill_->IsChecked() )
  {
    int const Classes[128] =
    {
        3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
    };
    int const StateTable[7][3] =
    {   // blnk  ,   othr
        {   1,   2,   3   },
        {   1,   2,   3   },
        {   4,   6,   3   },
        {   5,   7,   3   },
        {   5,   7,   3   },
        {   1,   2,   3   },
        {   1,   2,   3   }
    };
    for( int j=1;; ++j )
    {
      ++recordNumber;
      std::string sc;
      sc.clear();
      if( std::getline(*inStream,sc).fail() )break;
      if( sc.empty() )continue; // the record is empty, nothing on the line
      std::vector<double> reals( maxColumnNumber, 0.0 );
      std::vector<int> itype( maxColumnNumber, 0 );
      std::vector<wxString> strings( maxColumnNumber );
      int nf = -1;
      unsigned int newState = 1;
      unsigned int currentState = 0;
      bool flag = true;
      std::size_t scLen = sc.size();
      for( std::size_t k=0; k<scLen; ++k )
      {
        currentState = newState;
        int iascii = toascii( sc[k] );
        newState = StateTable[currentState-1][Classes[iascii]-1];
        if( newState == 2 )
        {
          if( nf++ == maxColumnNumber )
          {
            flag = false;
            break;
          }
        }
        else if( newState == 3 )
        {
          strings[nf+1] += wxString(sc.c_str(),wxConvUTF8).at(k);
        }
        else if( newState == 4 || newState == 6 )
        {
          itype[++nf] = 2;
          double d;
          if( std::istringstream( strings[nf].mb_str(wxConvUTF8).release() ) >> d )
          {
            itype[nf] = 1;
            reals[nf] = d;
          }
          if( nf == maxColumnNumber-1 )
          {
            flag = false;
            break;
          }
        }
      }
      if( flag )
      {
        itype[++nf] = 2;
        double d;
        if( std::istringstream( strings[nf].mb_str(wxConvUTF8).release() ) >> d )
        {
          itype[nf] = 1;
          reals[nf] = d;
        }
      }
      bool stopReading = false;
      if( nf >= 0 )
      {
        for( std::size_t k=0; k<numberOfNames; ++k )
        {
          if( fill[k] )
          {
            if( itype[columnNumbers[k]-1] != 1 )
            {
              data[k].push_back( errorFillValue );
              if( messages_->IsChecked() )
              {
                wxString c( wxT("field ") );
                c << k+1 << wxT(", record ") << recordNumber
                  << wxT(" set to Error Fill value, file: ") << fileName;
                ExGlobals::WarningMessage( c );
              }
            }
            else data[k].push_back( reals[columnNumbers[k]-1] );
            if( data[k].size()==nloc[k] && !makeNew_->IsChecked() && !append_->IsChecked() )fill[k] = false;
          }
        }
        if( stopReading )break;
      }
      if( lineRange_->IsChecked() )
      {
        stopReading = false;
        int next = startingLine + increment*j;
        if( next > lastLine )break;
        for( int i=last; i<next; ++i )
        {
          ++recordNumber;
          if( std::getline(*inStream,sc).fail() )
          {
            stopReading = true;
            break;
          }
        }
        if( stopReading )break;
        last = next+1;
      }
      stopReading = true;
      for( std::size_t k=0; k<numberOfNames; ++k )
      {
        if( fill[k] )
        {
          stopReading = false;
          break;
        }
      }
      if( stopReading )break;
    }
  }
  else // not replacing errors with errorFill value
  {
    wxString formatString;
    wxString formatSP( wxT("%lf") );
    wxString formatCM( wxT("%lf") );
    if( format_->IsChecked() )
    {
      formatString = formatTextCtrl_->GetValue();
      if( formatString.empty() )
      {
        wxMessageBox( wxT("expecting a format"),
                      wxT("Fatal error: nothing was read"), wxOK|wxICON_ERROR, this );
        inStream->close();
        return;
      }
    }
    else
    {
      for( std::size_t i=1; i<maxColumnNumber; ++i )
      {
        formatCM += wxT(",%lf");
        formatSP += wxT(" %lf");
      }
    }
    std::string sc;
    for( int j=1;; ++j )
    {
      ++recordNumber;
      sc.clear();
      if( std::getline(*inStream,sc).fail() )break;
      bool readError = false;
      if( sc.empty() )continue; // the record is empty, nothing on the line
      if( !format_->IsChecked() )formatString = sc.find(',')==sc.npos ? formatSP : formatCM;
      double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
      int result = sscanf( sc.c_str(), formatString.mb_str(wxConvUTF8),
                           vd, vd+1, vd+2, vd+3, vd+4, vd+5,
                           vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                           vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                           vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                           vd+31, vd+32, vd+33, vd+34, vd+35, vd+36, vd+37, vd+38,
                           vd+39, vd+40, vd+41, vd+42, vd+43, vd+44, vd+45, vd+46,
                           vd+47, vd+48, vd+49, vd+50, vd+51, vd+52, vd+53, vd+54,
                           vd+55, vd+56, vd+57, vd+58, vd+59, vd+60, vd+61, vd+62,
                           vd+63, vd+64, vd+65, vd+66, vd+67, vd+68, vd+69, vd+70,
                           vd+71, vd+72, vd+73, vd+74, vd+75, vd+76, vd+77, vd+78,
                           vd+79, vd+80, vd+81, vd+81, vd+83, vd+84, vd+85, vd+86,
                           vd+87, vd+88, vd+89, vd+90, vd+91, vd+92, vd+93, vd+94,
                           vd+95, vd+96, vd+97, vd+98, vd+99 );
      if( result == EOF )
      {
        wxString message( wxT("error reading record ") );
        message << recordNumber;
        if( errorStop_->IsChecked() )
        {
          inStream->close();
          wxMessageBox( message,
                        wxT("Fatal error: no vectors were modified"),
                        wxOK|wxICON_INFORMATION, this );
          return;
        }
        if( messages_->IsChecked() )ExGlobals::WarningMessage( message );
      }
      else if( result == 0 )
      {
        wxString message( wxT("error reading record ") );
        message << recordNumber;
        if( errorStop_->IsChecked() )
        {
          inStream->close();
          wxMessageBox( message,
                        wxT("Fatal error: no vectors were modified"),
                        wxOK|wxICON_INFORMATION, this );
          return;
        }
        if( messages_->IsChecked() )ExGlobals::WarningMessage( message );
      }
      else if( result < maxColumnNumber )
      {
        wxString message( wxT("not enough values on record ") );
        message << recordNumber;
        if( errorStop_->IsChecked() )
        {
          inStream->close();
          wxMessageBox( message,
                        wxT("Fatal error: no vectors were modified"),
                        wxOK|wxICON_INFORMATION, this );
          return;
        }
        if( messages_->IsChecked() )ExGlobals::WarningMessage( message );
      }
      else // no errors in reading
      {
        for( std::size_t k=0; k<numberOfNames; ++k )data[k].push_back( vd[columnNumbers[k]-1] );
        if( lineRange_->IsChecked() )
        {
          bool stopReading = false;
          int next = startingLine + increment*j;
          if( lastLine>0 && next>lastLine )break;
          for( int i=last; i<next; ++i )
          {
            ++recordNumber;
            if( std::getline(*inStream,sc).fail() )
            {
              stopReading = true;
              break;
            }
          }
          if( stopReading )break; // stop reading
          last = next+1;
        }
      }
    }
  }
  if( closeAfter_->IsChecked() )inStream->close();
  //
  // finished reading, one of: end of file, an error, or not extending vector
  //
  bool nothingWasRead = true;
  for( std::size_t j=0; j<numberOfNames; ++j )
  {
    if( data[j].size() > 0 )
    {
      try
      {
        NumericVariable *nv = NumericVariable::PutVariable( names[j], data[j], 0, wxT("from gui") );
        nv->SetOrigin( fileName );
      }
      catch ( EVariableError const &e )
      {
        wxMessageBox( wxString(e.what(),wxConvUTF8),
                      wxT("Fatal error"), wxOK|wxICON_INFORMATION, this );
        return;
      }
      nothingWasRead = false;
      if( messages_->IsChecked() )
      {
        wxString c;
        c << data[j].size() - nloc[j];
        if( append_->IsChecked() )
          ExGlobals::WriteOutput( c+wxT(" values have been appended to vector ")+names[j] );
        else
          ExGlobals::WriteOutput( wxString(wxT("vector "))+names[j]+wxT(" has been created with ")+c+wxT(" values") );
      }
    }
  }
  if( nothingWasRead )ExGlobals::WarningMessage( wxT("nothing was read") );
}

void ReadVectorsForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

// end of file
