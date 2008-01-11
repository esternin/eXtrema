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

#include "ReadMatrixForm.h"
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
BEGIN_EVENT_TABLE( ReadMatrixForm, wxFrame )
  EVT_BUTTON( wxID_APPLY, ReadMatrixForm::OnApply )
  EVT_BUTTON( wxID_CLOSE, ReadMatrixForm::OnClose )
  EVT_CHECKBOX( ID_errorFill, ReadMatrixForm::OnErrorFill )
  EVT_CHECKBOX( ID_format, ReadMatrixForm::OnFormat )
  EVT_CLOSE( ReadMatrixForm::CloseEventHandler )
END_EVENT_TABLE()

ReadMatrixForm::ReadMatrixForm( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Read matrix"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      analysisWindow_(parent)
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->InsertSpacer( 0, 10 );

  topPanel_ = new ChooseFilePanel( this, true,
                                   wxT("Choose a data file for reading"), wxT("any file|*.*") );
  mainSizer->Add( topPanel_, wxSizerFlags(0).Border(wxALL,1) );

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

  leftSizer->Add( new wxStaticLine(leftPanel), wxSizerFlags(0).Expand().Border(wxALL,1) );

  messages_ = new wxCheckBox( leftPanel, wxID_ANY, wxT("Display messages") );
  leftSizer->Add( messages_, wxSizerFlags(0).Border(wxALL,2) );
  messages_->SetValue( true );

  leftPanel->SetSizer( leftSizer );
  midSizer->Add( leftPanel, wxSizerFlags(0).Border(wxALL,10) );

  wxPanel *rightPanel = new wxPanel( midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );
  wxBoxSizer *rightSizer = new wxBoxSizer( wxVERTICAL );

  wxPanel *midRightPanel = new wxPanel( rightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxGridSizer *midRightSizer = new wxGridSizer( 3, 2, 0, 0 );

  midRightSizer->Add(
    new wxStaticText(midRightPanel,wxID_ANY,wxT("Matrix name"),wxDefaultPosition,wxDefaultSize,wxALIGN_RIGHT ),
    wxSizerFlags(0).Right().Border(wxTOP,10) );
  matrixNameTextCtrl_ = new wxTextCtrl( midRightPanel, wxID_ANY );
  matrixNameTextCtrl_->SetValue( wxT("") );
  midRightSizer->Add( matrixNameTextCtrl_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  midRightSizer->Add(
    new wxStaticText(midRightPanel,wxID_ANY,wxT("Number of rows"),wxDefaultPosition,
                     wxDefaultSize,wxALIGN_RIGHT),
    wxSizerFlags(0).Right().Border(wxTOP,10) );
  rowsTextCtrl_ = new wxTextCtrl( midRightPanel, wxID_ANY );
  rowsTextCtrl_->SetValue( wxT("1") );
  midRightSizer->Add( rowsTextCtrl_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  midRightSizer->Add(
    new wxStaticText(midRightPanel,wxID_ANY,wxT("Number of columns"),wxDefaultPosition,
                     wxDefaultSize,wxALIGN_RIGHT),
    wxSizerFlags(0).Right().Border(wxTOP,10) );
  columnsTextCtrl_ = new wxTextCtrl( midRightPanel, wxID_ANY );
  columnsTextCtrl_->SetValue( wxT("1") );
  midRightSizer->Add( columnsTextCtrl_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  midRightPanel->SetSizer( midRightSizer );
  rightSizer->Add( midRightPanel, wxSizerFlags(0).Border(wxALL,1) );
  
  rightSizer->Add( new wxStaticLine(rightPanel), wxSizerFlags(0).Expand().Border(wxALL,10) );

  wxPanel *lineNumberPanel = new wxPanel( rightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxGridSizer *lineNumberSizer = new wxGridSizer( 1, 2, 0, 0 );

  lineNumberSizer->Add(
    new wxStaticText(lineNumberPanel,wxID_ANY,wxT("Starting line number"),wxDefaultPosition,
                     wxDefaultSize,wxALIGN_RIGHT),
    wxSizerFlags(0).Right().Border(wxTOP,10) );
  lineNumberTextCtrl_ = new wxTextCtrl( lineNumberPanel, wxID_ANY );
  lineNumberTextCtrl_->SetValue( wxT("1") );
  lineNumberSizer->Add( lineNumberTextCtrl_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  lineNumberPanel->SetSizer( lineNumberSizer );
  rightSizer->Add( lineNumberPanel, wxSizerFlags(0).Border(wxALL,1) );

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
  
  wxPanel *bottomPanel = new wxPanel( (wxWindow*)this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                      wxNO_BORDER );
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
  int ulx = config->Read( wxT("/ReadMatrixForm/UPPERLEFTX"), 0l );
  int uly = config->Read( wxT("/ReadMatrixForm/UPPERLEFTY"), 640l );
  int width = config->Read( wxT("/ReadMatrixForm/WIDTH"), 545l );
  int height = config->Read( wxT("/ReadMatrixForm/HEIGHT"), 400l );
  SetSize( ulx, uly, width, height );

  topPanel_->GetFilenames( config, wxT("/ReadMatrixForm") );

  Show( true );
}

void ReadMatrixForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/ReadMatrixForm/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/ReadMatrixForm/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/ReadMatrixForm/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/ReadMatrixForm/HEIGHT"), static_cast<long>(height) );

    topPanel_->SaveFilenames( config, wxT("/ReadMatrixForm") );
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
  analysisWindow_->ZeroLoadMatrix();

  Destroy();
}

void ReadMatrixForm::OnFormat( wxCommandEvent &WXUNUSED(event) )
{ formatTextCtrl_->Enable( format_->IsChecked() ); }

void ReadMatrixForm::OnApply( wxCommandEvent &WXUNUSED(event) )
{
  wxString fileName( topPanel_->GetSelection() );
  if( fileName.empty() )
  {
    wxMessageDialog *md =
      new wxMessageDialog( this, wxT("no file has been chosen"),
                           wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
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
      wxMessageDialog *md = new wxMessageDialog( this, wxString(wxT("Could not open "))+fileName,
                                                 wxT("Fatal error: nothing was read"), wxOK|wxICON_ERROR );
      md->ShowModal();
      return;
    }
  }
  wxString formatString;
  if( format_->IsChecked() )
  {
    formatString = formatTextCtrl_->GetValue();
    if( formatString.empty() )
    {
      wxMessageDialog *md = new wxMessageDialog( this, wxT("expecting a format"),
                                                 wxT("Fatal error: nothing was read"), wxOK|wxICON_ERROR );
      md->ShowModal();
      inStream->close();
      return;
    }
  }
  wxString matrixName( matrixNameTextCtrl_->GetValue() );
  if( matrixName.empty() )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxT("a matrix name must be entered"),
                                               wxT("Fatal error: nothing was read"), wxOK|wxICON_ERROR );
    md->ShowModal();
    inStream->close();
    return;
  }
  wxString stmp( rowsTextCtrl_->GetValue() );
  if( stmp.empty() )
  {
    rowsTextCtrl_->SetValue( wxT("1") );
    inStream->close();
    wxMessageDialog *md = new wxMessageDialog( this, wxT("the number of rows is required"),
                                               wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return;
  }
  long ltmp;
  if( !stmp.ToLong(&ltmp) || ltmp<1L )
  {
    rowsTextCtrl_->SetValue( wxT("1") );
    inStream->close();
    wxMessageDialog *md = new wxMessageDialog( this,
                                               stmp+wxT(" is an invalid value for number of rows"),
                                               wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return;
  }
  int nRow = static_cast<int>(ltmp);
  //
  int nCol;
  bool columnsGiven = true;
  stmp = columnsTextCtrl_->GetValue();
  if( stmp.empty() )columnsGiven = false;
  else
  {
    if( !stmp.ToLong(&ltmp) || ltmp<1L )
    {
      columnsTextCtrl_->SetValue( wxT("1") );
      inStream->close();
      wxMessageDialog *md = new wxMessageDialog( this,
                                                 stmp+wxT(" is an invalid value for number of columns"),
                                                 wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION );
      md->ShowModal();
      return;
    }
    nCol = static_cast<int>(ltmp);
  }
  //
  stmp = lineNumberTextCtrl_->GetValue();
  if( stmp.empty() )
  {
    lineNumberTextCtrl_->SetValue( wxT("1") );
    inStream->close();
    wxMessageDialog *md = new wxMessageDialog( this, wxT("the starting line number is required"),
                                               wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return;
  }
  if( !stmp.ToLong(&ltmp) || ltmp<1L )
  {
    lineNumberTextCtrl_->SetValue( wxT("1") );
    inStream->close();
    wxMessageDialog *md = new wxMessageDialog( this, wxT("line number must be > 0"),
                                               wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return;
  }
  int startingLine = static_cast<int>(ltmp);
  //
  double errorFillValue = readCommand->GetErrorFill();
  if( errorFill_->IsChecked() )
  {
    stmp = errorFillTextCtrl_->GetValue();
    if( !stmp.ToDouble(&errorFillValue) )
    {
      errorFillTextCtrl_->SetValue( wxT("0.0") );
      inStream->close();
      wxMessageDialog *md = new wxMessageDialog( this,
                                                 stmp+wxT(" is an invalid value for error fill"),
                                                 wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION );
      md->ShowModal();
      return;
    }
    readCommand->SetErrorFill( errorFillValue );
  }
  //
  // read in the initial dummy records
  //
  unsigned int recordNumber = 0;
  for( int i=1; i<startingLine; ++i )
  {
    ++recordNumber;
    std::string buffer;
    if( std::getline(*inStream,buffer).fail() )
    {
      inStream->close();
      wxMessageDialog *md = new wxMessageDialog( this, wxT("end of file reached reading initial dummy records"),
                                                 wxT("Fatal error: no matrix was modified"), wxOK|wxICON_INFORMATION );
      md->ShowModal();
      return;
    }
  }
  //
  // finally ready to read some data
  //
  std::vector<double> data;
  if( columnsGiven )
  {
    data.resize( nRow*nCol );
    if( format_->IsChecked() )
    {
      for( int j=0; j<nCol; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( std::getline(*inStream,sc).fail() || sc.empty() )
        {
          inStream->close();
          wxString c( wxT("error reading record ") );
          c << recordNumber;
          wxMessageDialog *md = new wxMessageDialog( this, c,
                                                     wxT("Fatal error: no matrix was modified"),
                                                     wxOK|wxICON_INFORMATION );
          md->ShowModal();
          return;
        }
        double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0};
        int result = sscanf( sc.c_str(), formatString.mb_str(), vd, vd+1, vd+2, vd+3, vd+5,
                             vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                             vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                             vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                             vd+31, vd+32, vd+33, vd+34 );
        if( result==EOF || result==0 )
        {
          inStream->close();
          wxString c( wxT("error reading record ") );
          c << recordNumber;
          wxMessageDialog *md = new wxMessageDialog( this, c,
                                                     wxT("Fatal error: no matrix was modified"),
                                                     wxOK|wxICON_INFORMATION );
          md->ShowModal();
          return;
        }
        else if( result < nRow )
        {
          inStream->close();
          wxString c( wxT("not enough values on record ") );
          c << recordNumber;
          wxMessageDialog *md = new wxMessageDialog( this, c,
                                                     wxT("Fatal error: no matrix was modified"),
                                                     wxOK|wxICON_INFORMATION );
          md->ShowModal();
          return;
        }
        //
        // no errors in reading
        //
        for( int k=0; k<nRow; ++k )data[k+j*nRow] = vd[k];
      }
    }
    else  // no format given
    {
      for( int j=0; j<nCol; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( std::getline(*inStream,sc).fail() || sc.empty() )
        {
          inStream->close();
          wxString c( wxT("error reading record ") );
          c << recordNumber;
          wxMessageDialog *md = new wxMessageDialog( this, c,
                                                     wxT("Fatal error: no matrix was modified"),
                                                     wxOK|wxICON_INFORMATION );
          md->ShowModal();
          return;
        }
        //
        // trim leading and trailing blanks
        //
        std::string const blank( " \t" );
        std::size_t rTemp = sc.find_last_not_of( blank );
        if( rTemp != sc.npos )
        {
          std::size_t lTemp = sc.find_first_not_of( blank );
          if( lTemp != sc.npos )sc = sc.substr( lTemp, rTemp-lTemp+1 );
        }
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
        std::vector<double> reals( nRow, 0.0 );
        std::vector<int> itype( nRow, 0 );
        std::vector<wxString> strings( nRow );
        int nf = -1;
        unsigned int newState = 1;
        unsigned int currentState = 0;
        bool flag = true;
        std::size_t scLen = sc.length();
        for( std::size_t k=0; k<scLen; ++k )
        {
          currentState = newState;
          int iascii = toascii( sc[k] );
          newState = StateTable[currentState-1][Classes[iascii]-1];
          if( newState == 2 )
          {
            if( ++nf == nRow )
            {
              flag = false;
              break;
            }
          }
          else if( newState == 3 )
          {
            strings[nf+1].append( (wxChar)sc[k] );
          }
          else if( newState == 4 || newState == 6 )
          {
            ++nf;
            double d;
            if( strings[nf].ToDouble(&d) )
            {
              itype[nf] = 1;
              reals[nf] = d;
            }
            else itype[nf] = 2;
            if( nf == nRow-1 )
            {
              flag = false;
              break;
            }
          }
        }
        if( flag )
        {
          ++nf;
          double d;
          if( strings[nf].ToDouble(&d) )
          {
            itype[nf] = 1;
            reals[nf] = d;
          }
          else itype[nf] = 2;
        }
        if( nf > 0 )
        {
          for( int k=0; k<nRow; ++k )
          {
            if( itype[k] != 1 )
            {
              data[k+j*nRow] = errorFillValue;
              if( messages_->IsChecked() )
              {
                wxString c( wxT("error in field ") );
                c << k+1 << wxT(", record ") << recordNumber ;
                ExGlobals::WarningMessage( c );
              }
            }
            else data[k+j*nRow] = reals[k];
          }
        }
      }
    }
  }
  else  // number of columns not given
  {
    if( format_->IsChecked() )
    {
      for( int j=0;; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( std::getline(*inStream,sc).fail() || sc.empty() )break;
        double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0};
        int result = sscanf( sc.c_str(), formatString.mb_str(), vd, vd+1, vd+2, vd+3, vd+5,
                             vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                             vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                             vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                             vd+31, vd+32, vd+33, vd+34 );
        if( result == EOF || result == 0 )
        {
          inStream->close();
          wxString c( wxT("error reading record ") );
          c << recordNumber;
          wxMessageDialog *md = new wxMessageDialog( this, c,
                                                     wxT("Fatal error: no matrix was modified"),
                                                     wxOK|wxICON_INFORMATION );
          md->ShowModal();
          return;
        }
        else if( result < nRow )
        {
          inStream->close();
          wxString c( wxT("not enough values on record ") );
          c << recordNumber;
          wxMessageDialog *md = new wxMessageDialog( this, c,
                                                     wxT("Fatal error: no matrix was modified"),
                                                     wxOK|wxICON_INFORMATION );
          md->ShowModal();
          return;
        }
        //
        // no errors in reading
        //
        data.resize( (j+1)*nRow );
        for( int k=0; k<nRow; ++k )data[k+j*nRow] = vd[k];
      }
    }
    else  // no format given
    {
      nCol = 0;
      for( int j=0;; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( std::getline(*inStream,sc).fail() || sc.empty() )break;
        ++nCol;
        //
        // trim leading and trailing blanks
        //
        std::string const blank( " \t" );
        std::size_t rTemp = sc.find_last_not_of( blank );
        if( rTemp != sc.npos )
        {
          std::size_t lTemp = sc.find_first_not_of( blank );
          if( lTemp != sc.npos )sc = sc.substr( lTemp, rTemp-lTemp+1 );
        }
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
        std::vector<double> reals( nRow, 0.0 );
        std::vector<int> itype( nRow, 0 );
        std::vector<wxString> strings( nRow );
        int nf = -1;
        unsigned int newState = 1;
        unsigned int currentState = 0;
        bool flag = true;
        std::size_t scLen = sc.length();
        for( std::size_t k=0; k<scLen; ++k )
        {
          currentState = newState;
          int iascii = toascii( sc[k] );
          newState = StateTable[currentState-1][Classes[iascii]-1];
          if( newState == 2 )
          {
            if( ++nf == nRow )
            {
              flag = false;
              break;
            }
          }
          else if( newState == 3 )
          {
            strings[nf+1].append( wxString(sc.c_str(),wxConvUTF8).at(k) );
          }
          else if( newState == 4 || newState == 6 )
          {
            ++nf;
            double d;
            if( strings[nf].ToDouble(&d) )
            {
              itype[nf] = 1;
              reals[nf] = d;
            }
            else itype[nf] = 2;
            if( nf == nRow-1 )
            {
              flag = false;
              break;
            }
          }
        }
        if( flag )
        {
          ++nf;
          double d;
          if( strings[nf].ToDouble(&d) )
          {
            itype[nf] = 1;
            reals[nf] = d;
          }
          else itype[nf] = 2;
        }
        if( nf > 0 )
        {
          data.resize( (j+1)*nRow );
          for( int k=0; k<nRow; ++k )
          {
            if( itype[k] != 1 )
            {
              wxString c( wxT("error in field ") );
              c << k+1 << wxT(", record ") << recordNumber ;
              if( errorFill_->IsChecked() )
              {
                data[k+j*nRow] = errorFillValue;
                if( messages_->IsChecked() )ExGlobals::WarningMessage( c );
              }
              else
              {
                inStream->close();
                wxString c( wxT("error reading record ") );
                c << recordNumber;
                wxMessageDialog *md = new wxMessageDialog( this, c,
                                                           wxT("Fatal error: no matrix was modified"),
                                                           wxOK|wxICON_INFORMATION );
                md->ShowModal();
                return;
              }
            }
            else data[k+j*nRow] = reals[k];
          }
        }
      }
    }
  }
  if( closeAfter_->IsChecked() )inStream->close();
  //
  // Successful read
  //
  TVariableTable::GetTable()->RemoveEntry( matrixName );
  NVariableTable::GetTable()->RemoveEntry( matrixName );
  NumericVariable *nv = NumericVariable::PutVariable( matrixName, data,
                                                      static_cast<std::size_t>(nRow),
                                                      static_cast<std::size_t>(nCol),
                                                      wxString(wxT("from gui")) );
  nv->SetOrigin( fileName );
  if( messages_->IsChecked() )
  {
    wxString s( wxT("matrix ") );
    ExGlobals::WriteOutput( s << matrixName << wxT(" has been created with ") << nRow
                              << wxT(" rows and ") << nCol << wxT(" columns") );
  }
}

void ReadMatrixForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void ReadMatrixForm::OnErrorFill( wxCommandEvent &WXUNUSED(event) )
{ errorFillTextCtrl_->Enable( errorFill_->IsChecked() ); }

// end of file
