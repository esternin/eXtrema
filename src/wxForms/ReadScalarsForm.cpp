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

#include "ReadScalarsForm.h"
#include "AnalysisWindow.h"
#include "ChooseFilePanel.h"
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "TextVariable.h"
#include "ExGlobals.h"
#include "CMD_read.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( ReadScalarsForm, wxFrame )
  EVT_BUTTON( wxID_APPLY, ReadScalarsForm::OnApply )
  EVT_BUTTON( wxID_CLOSE, ReadScalarsForm::OnClose )
  EVT_GRID_CELL_CHANGED( ReadScalarsForm::OnCellChange )
  EVT_CHECKBOX( ID_errorFill, ReadScalarsForm::OnErrorFill )
  EVT_CHECKBOX( ID_format, ReadScalarsForm::OnFormat )
  EVT_CLOSE( ReadScalarsForm::CloseEventHandler )
END_EVENT_TABLE()

ReadScalarsForm::ReadScalarsForm( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Read scalars"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      analysisWindow_(parent)
{
  wxPanel* const mainPanel = new wxPanel(this);

  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->InsertSpacer( 0, 10 );

  topPanel_ = new ChooseFilePanel( mainPanel, true, wxT("Choose a data file for reading"), wxT("any file|*.*") );
  mainSizer->Add( topPanel_, wxSizerFlags(0).Border(wxALL,1) );

  wxPanel *gridPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *gridSizer = new wxBoxSizer( wxHORIZONTAL );

  // create the scalar name/column grid
  // column 0 is "scalar name", column 1 is "column number"
  scalarGrid_ = new wxGrid( gridPanel, wxID_ANY, wxDefaultPosition, wxSize(375,250), wxSUNKEN_BORDER|wxVSCROLL );
  scalarGrid_->CreateGrid( 100, 2 );
  scalarGrid_->SetColFormatNumber( 1 );
  scalarGrid_->SetColLabelValue( 0, wxT("scalar name") );
  scalarGrid_->SetColLabelValue( 1, wxT("column") );
  scalarGrid_->SetColSize( 0, 210 );
  scalarGrid_->SetColSize( 1, 60 );
  gridSizer->Add( scalarGrid_, wxSizerFlags(0).Border(wxALL,2) );
  gridPanel->SetSizer( gridSizer );

  mainSizer->Add( gridPanel, wxSizerFlags(0).Center().Border(wxALL,1) );

  wxPanel *midPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );
  wxBoxSizer *midSizer = new wxBoxSizer( wxVERTICAL );

  closeBefore_ = new wxCheckBox( midPanel, wxID_ANY, wxT("Close file before reading") );
  midSizer->Add( closeBefore_, wxSizerFlags(0).Border(wxALL,2) );
  closeBefore_->SetValue( true );
  
  closeAfter_ = new wxCheckBox( midPanel, wxID_ANY, wxT("Close file after reading") );
  midSizer->Add( closeAfter_, wxSizerFlags(0).Border(wxALL,2) );
  closeAfter_->SetValue( true );
  
  midSizer->Add( new wxStaticLine(midPanel), wxSizerFlags(0).Expand().Border(wxALL,1) );
  
  wxPanel *errorFillPanel = new wxPanel( midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *errorFillSizer = new wxBoxSizer( wxHORIZONTAL );
  
  errorFill_ = new wxCheckBox( errorFillPanel, ID_errorFill, wxT("Error fill") );
  errorFillSizer->Add( errorFill_, wxSizerFlags(0).Border(wxALL,2) );
  
  errorFillTextCtrl_ = new wxTextCtrl( errorFillPanel, wxID_ANY );
  errorFillTextCtrl_->SetValue( wxT("0.0") );
  errorFillSizer->Add( errorFillTextCtrl_, wxSizerFlags(0).Border(wxALL,2) );
  errorFillTextCtrl_->Enable( false );
  
  errorFillPanel->SetSizer( errorFillSizer );
  midSizer->Add( errorFillPanel, wxSizerFlags(0).Border(wxALL,2) );
  
  midSizer->Add( new wxStaticLine(midPanel), wxSizerFlags(0).Expand().Border(wxALL,2) );

  messages_ = new wxCheckBox( midPanel, wxID_ANY, wxT("Display messages") );
  midSizer->Add( messages_, wxSizerFlags(0).Border(wxALL,2) );
  messages_->SetValue( true );
  
  midSizer->Add( new wxStaticLine(midPanel), wxSizerFlags(0).Expand().Border(wxALL,2) );

  wxPanel *lineNumberPanel = new wxPanel( midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxGridSizer *lineNumberSizer = new wxGridSizer( 1, 2, 0, 0 );

  lineNumberText_ = new wxStaticText( lineNumberPanel, wxID_ANY, wxT("Line to read"), wxDefaultPosition,
                                      wxDefaultSize, wxALIGN_RIGHT );
  lineNumberSizer->Add( lineNumberText_, wxSizerFlags(0).Right().Border(wxTOP,10) );
  lineNumberTextCtrl_ = new wxTextCtrl( lineNumberPanel, wxID_ANY );
  lineNumberTextCtrl_->SetValue( wxT("1") );
  lineNumberSizer->Add( lineNumberTextCtrl_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  lineNumberPanel->SetSizer( lineNumberSizer );
  midSizer->Add( lineNumberPanel, wxSizerFlags(0).Border(wxALL,1) );
  
  midSizer->Add( new wxStaticLine(midPanel), wxSizerFlags(0).Expand().Border(wxALL,10) );
  
  format_ = new wxCheckBox( midPanel, ID_format, wxT("Use format") );
  midSizer->Add( format_, wxSizerFlags(0).Center().Border(wxALL,1) );
  
  formatTextCtrl_ = new wxTextCtrl( midPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(175,25) );
  midSizer->Add( formatTextCtrl_, wxSizerFlags(0).Center().Border(wxALL,10) );
  formatTextCtrl_->Enable( false );
  
  midPanel->SetSizer( midSizer );

  mainSizer->Add( midPanel, wxSizerFlags(0).Center().Border(wxALL,1) );
  
  wxPanel *bottomPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );

  wxButton *applyButton = new wxButton( bottomPanel, wxID_APPLY, wxT("Apply") );
  applyButton->SetToolTip( wxT("read data from the file chosen above") );
  bottomSizer->Add( applyButton, wxSizerFlags(0).Border(wxALL,10) );

  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );

  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );

  mainPanel->SetSizer( mainSizer );
  
  wxConfigBase *config = wxConfigBase::Get();
  topPanel_->GetFilenames( config, wxT("/ReadScalarsForm") );

  wxPersistentRegisterAndRestore(this, "ReadScalarsForm");

  Show( true );
}

void ReadScalarsForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    topPanel_->SaveFilenames( config, wxT("/ReadScalarsForm") );
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
  analysisWindow_->ZeroLoadScalars();

  Destroy();
}

void ReadScalarsForm::OnCellChange( wxGridEvent &WXUNUSED(event) )
{
  if( scalarGrid_->GetGridCursorCol() == 0 )
  {
    int row = scalarGrid_->GetGridCursorRow();
    wxString s;
    scalarGrid_->SetCellValue( row, 1, s<<(row+1) );
  }
}

void ReadScalarsForm::OnErrorFill( wxCommandEvent &WXUNUSED(event) )
{ errorFillTextCtrl_->Enable( errorFill_->IsChecked() ); }

void ReadScalarsForm::OnFormat( wxCommandEvent &WXUNUSED(event) )
{ formatTextCtrl_->Enable( format_->IsChecked() ); }

void ReadScalarsForm::OnApply( wxCommandEvent &WXUNUSED(event) )
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
  wxString formatString;
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
  std::vector<wxString> names;
  std::vector<int> columnNumber;
  int nRows = scalarGrid_->GetNumberRows();
  for( int i=0; i<nRows; ++i )
  {
    wxString name( scalarGrid_->GetCellValue(i,0) );
    if( name.empty() )break;
    names.push_back( name );
    wxString colm( scalarGrid_->GetCellValue(i,1) );
    if( colm.empty() || format_->IsChecked() )columnNumber.push_back(i+1);
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
      columnNumber.push_back( static_cast<int>(ltmp) );
    }
  }
  if( names.empty() )
  {
    inStream->close();
    wxMessageBox( wxT("expecting some output vector names"),
                  wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
    return;
  }
  int maxColumnNumber = 0;
  std::size_t numberOfNames = names.size();
  for( std::size_t i=0; i<numberOfNames; ++i )
  {
    if( maxColumnNumber < columnNumber[i] )maxColumnNumber = columnNumber[i];
    TVariableTable::GetTable()->RemoveEntry( names[i] );
    NVariableTable::GetTable()->RemoveEntry( names[i] );
  }
  long ltmp;
  wxString stmp( lineNumberTextCtrl_->GetValue() );
  if( !stmp.ToLong(&ltmp) || ltmp<1L )
  {
    lineNumberTextCtrl_->SetValue( wxT("1") );
    inStream->close();
    wxMessageBox( wxT("line number must be > 0"),
                  wxT("Fatal error: nothing was read"), wxOK|wxICON_INFORMATION, this );
    return;
  }
  int startingLine = static_cast<int>(ltmp);
  
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
  unsigned int recordNumber = 0;
  for( int i=1; i<startingLine; ++i )
  {
    ++recordNumber;
    std::string buffer;
    if( std::getline(*inStream,buffer).fail() )
    {
      inStream->close();
      wxMessageBox( wxT("end of file reached reading initial dummy records"),
                    wxT("Fatal error: no scalars were modified"), wxOK|wxICON_INFORMATION, this );
      return;
    }
  }
  ++recordNumber;
  //
  // finally ready to read some data
  //
  std::vector<double> data( maxColumnNumber );
  if( format_->IsChecked() )
  {
    std::string sc;
    sc.clear();
    if( std::getline(*inStream,sc).fail() || sc.empty() )
    {
      inStream->close();
      wxString c( wxT("record ") ); 
      c << recordNumber;
      wxMessageBox( c+wxT(" could not be read"),
                    wxT("Fatal error: no scalars were modified"),
                    wxOK|wxICON_INFORMATION, this );
      return;
    }
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
    if( result==EOF || result==0 )
    {
      wxString message( wxT("error reading record ") );
      message << recordNumber;
      inStream->close();
      wxMessageBox( message,
                               wxT("Fatal error: no scalars were read"), wxOK|wxICON_INFORMATION, this );
      return;
    }
    else if( result < maxColumnNumber )
    {
      wxString message( wxT("not enough values on record ") );
      message << recordNumber;
      inStream->close();
      wxMessageBox( message,
                    wxT("Fatal error: no scalars were modified"), wxOK|wxICON_INFORMATION, this );
      return;
    }
    //
    // no errors in reading
    //
    for( int k=0; k<maxColumnNumber; ++k )data[k] = vd[k];
  }
  else  // no format entered
  {
    std::string sc;
    sc.clear();
    if( std::getline(*inStream,sc).fail() || sc.empty() )
    {
      wxString message( wxT("record ") );
      message << recordNumber << wxT(" is empty");
      inStream->close();
      wxMessageBox( message,
                    wxT("Fatal error: no scalars were modified"), wxOK|wxICON_INFORMATION, this );
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
    std::vector<double> reals( maxColumnNumber, 0.0 );
    std::vector<int> itype( maxColumnNumber, 0 );
    std::vector<wxString> strings( maxColumnNumber );
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
        if( ++nf == maxColumnNumber-1 )
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
        else
        {
          itype[nf] = 2;
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
      ++nf;
      double d;
      if( strings[nf].ToDouble(&d) )
      {
        itype[nf] = 1;
        reals[nf] = d;
      }
      else
      {
        itype[nf] = 2;
      }
    }
    if( nf > 0 )
    {
      for( std::size_t k=0; k<numberOfNames; ++k )
      {
        if( itype[columnNumber[k]-1] != 1 )
        {
          if( !errorFill_->IsChecked() )
          {
            inStream->close();
            wxMessageBox( wxT("error reading scalars"),
                          wxT("Fatal error: no scalars were modified"), wxOK|wxICON_INFORMATION, this );
            return;
          }
          data[k] = errorFillValue;
          if( messages_->IsChecked() )
          {
            wxChar c[200];
            ::wxSnprintf( c, 200, wxT("error in field %d, record %d, file: %s"),
                          k+1, recordNumber, fileName.c_str() );
            ExGlobals::WarningMessage( c );
          }
        }
        else
          data[k] = reals[columnNumber[k]-1];
      }
    }
  }
  if( closeAfter_->IsChecked() )inStream->close();
  //
  // finished reading, one of: end of file, an error, or not extending vector
  //
  for( std::size_t j=0; j<numberOfNames; ++j )
  {
    NumericVariable *nv = NumericVariable::PutVariable( names[j], data[j], wxT("from gui") );
    nv->SetOrigin( fileName );
    if( messages_->IsChecked() )
      ExGlobals::WriteOutput( wxString(wxT("scalar "))+names[j]+wxT(" has been created") );
  }
}

void ReadScalarsForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

// end of file
