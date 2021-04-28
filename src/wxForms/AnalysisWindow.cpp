/*
Copyright (C) 2005,...,2010 Joseph L. Chuma

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
#include "wx/config.h"

#include "AnalysisWindow.h"
#include "AnalysisSpeedButtonPanel.h"
#include "AnalysisMessagePanel.h"
#include "AnalysisCommandPanel.h"
#include "VisualizationWindow.h"
#include "ExecuteDialog.h"
#include "ReadVectorsForm.h"
#include "ReadScalarsForm.h"
#include "ReadMatrixForm.h"
#include "ReadTextForm.h"
#include "WriteVectorsForm.h"
#include "WriteScalarsForm.h"
#include "WriteMatrixForm.h"
#include "WriteTextForm.h"
#include "ShowVariablesForm.h"
#include "GenerateVectorForm.h"
#include "FitForm.h"
#include "StackDialog.h"
#include "ExGlobals.h"

#include <wx/persist/toplevel.h>

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
// it is important for the id corresponding to the "About" command to have
// this standard value as otherwise it won't be handled properly under Mac
// (where it is special and put into the "Apple" menu)
//
BEGIN_EVENT_TABLE( AnalysisWindow, wxFrame )
  EVT_MENU( wxID_EXIT, AnalysisWindow::OnQuit )
  EVT_MENU( wxID_ABOUT, AnalysisWindow::OnAbout )
  EVT_MENU( wxID_HELP, AnalysisWindow::OnHelp )
  EVT_MENU( ID_execute, AnalysisWindow::OnExecute )
  EVT_MENU( ID_loadVectors, AnalysisWindow::OnLoadVectors )
  EVT_MENU( ID_loadScalars, AnalysisWindow::OnLoadScalars )
  EVT_MENU( ID_loadMatrix, AnalysisWindow::OnLoadMatrix )
  EVT_MENU( ID_loadText, AnalysisWindow::OnLoadText )
  EVT_MENU( ID_writeVectors, AnalysisWindow::OnWriteVectors )
  EVT_MENU( ID_writeScalars, AnalysisWindow::OnWriteScalars )
  EVT_MENU( ID_writeMatrix, AnalysisWindow::OnWriteMatrix )
  EVT_MENU( ID_writeText, AnalysisWindow::OnWriteText )
  EVT_CLOSE( AnalysisWindow::CloseEventHandler )
END_EVENT_TABLE()

AnalysisWindow::AnalysisWindow( extrema *extrema )
    : wxFrame( (wxWindow*)NULL,wxID_ANY,wxT("Analysis Window"),
               wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      extrema_(extrema)
{
  executeDialog_ = 0;
  showVariablesForm_ = 0;
  readVectorsForm_ = 0;
  readScalarsForm_ = 0;
  readMatrixForm_ = 0;
  readTextForm_ = 0;
  writeVectorsForm_ = 0;
  writeScalarsForm_ = 0;
  writeMatrixForm_ = 0;
  writeTextForm_ = 0;
  fitForm_ = 0;
  stackDialog_ = 0;
  generateVectorForm_ = 0;

  wxMenu *fileMenu = new wxMenu();
  fileMenu->Append( ID_execute, wxT("&Execute"), wxT("execute an extrema script") );
  fileMenu->AppendSeparator();
  wxMenu *loadDataMenu = new wxMenu();
  loadDataMenu->Append( ID_loadVectors, wxT("Vectors"), wxT("read vector data from a file") );
  loadDataMenu->Append( ID_loadScalars, wxT("Scalars"), wxT("read scalar data from a file") );
  loadDataMenu->Append( ID_loadMatrix, wxT("Matrix"), wxT("read matrix data from a file") );
  loadDataMenu->Append( ID_loadText, wxT("Text"), wxT("read character data from a file") );
  fileMenu->Append( wxID_ANY, wxT("Read data"), loadDataMenu, wxT("read data from a file") );

  fileMenu->AppendSeparator();
  wxMenu *writeDataMenu = new wxMenu();
  writeDataMenu->Append( ID_writeVectors, wxT("Vectors"), wxT("write vector data to a file") );
  writeDataMenu->Append( ID_writeScalars, wxT("Scalars"), wxT("write scalar data to a file") );
  writeDataMenu->Append( ID_writeMatrix, wxT("Matrices"), wxT("write matrix data to a file") );
  writeDataMenu->Append( ID_writeText, wxT("Text variables"), wxT("write string data to a file") );
  fileMenu->Append( wxID_ANY, wxT("Write data"), writeDataMenu, wxT("write data to a file") );
  
  fileMenu->AppendSeparator();
  fileMenu->Append( wxID_EXIT, wxT("E&xit"), wxT("Quit this program") );
  
  wxMenu *helpMenu = new wxMenu();
  helpMenu->Append( wxID_HELP, wxT("&Help contents"), wxT("Open the help facility") );
  helpMenu->Append( wxID_ABOUT, wxT("&About"), wxT("Show the about dialog") );
  
  wxMenuBar *menuBar = new wxMenuBar();
  menuBar->Append( fileMenu, wxT("&File") );
  menuBar->Append( helpMenu, wxT("&Help") );
  
  SetMenuBar( menuBar );
  //
  // for a vertical sizer:
  // proportion = 0    means no vertical expansion
  // proportion > 0    allows for vertical expansion
  // Expand()          allows for horizontal expansion
  //
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  sizer->Add( new AnalysisSpeedButtonPanel(this), wxSizerFlags(0).Expand().Border(wxALL,1) );
  messagePanel_ = new AnalysisMessagePanel(this);
  sizer->Add( messagePanel_, wxSizerFlags(1).Expand().Border(wxALL,1) );
  commandPanel_ = new AnalysisCommandPanel(this);
  sizer->Add( commandPanel_, wxSizerFlags(0).Expand().Border(wxALL,1) );
  SetSizer( sizer );

  CreateStatusBar( 1 );
  SetStatusText( wxT("Welcome to extrema!") );

  wxPersistentRegisterAndRestore(this, "AnalysisWindow");

  // Show the main window.
  // Frames, unlike simple controls, are not shown when created initially.
  Show( true );
  ExGlobals::SetAnalysisWindow( this );

  // create the main visualization window, with the analysis window as its parent
  VisualizationWindow *visualizationWindow = new VisualizationWindow( (wxWindow*)this );
  if( !visualizationWindow )Close(true);
  ExGlobals::SetVisualizationWindow( visualizationWindow );
}

void AnalysisWindow::WriteOutput( wxString const &s )
{ messagePanel_->WriteOutput( s ); }

void AnalysisWindow::ClearOutput()
{ messagePanel_->ClearOutput(); }

void AnalysisWindow::AddCommandString( wxString const &s )
{ commandPanel_->AddCommandString( s ); }

std::vector<wxString> &AnalysisWindow::GetCommandStrings()
{ return commandPanel_->GetCommandStrings(); }

void AnalysisWindow::OnQuit( wxCommandEvent &WXUNUSED(event) )
{ extrema_->QuitApp(); }

void AnalysisWindow::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  ExGlobals::DeleteStuff();
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
  Destroy();
}

void AnalysisWindow::OnHelp( wxCommandEvent &WXUNUSED(event) )
{ ExGlobals::StartHelp(); }

void AnalysisWindow::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
  wxDialog dg( this, wxID_ANY, wxT("Extrema"), wxDefaultPosition, wxDefaultSize );
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  wxTextCtrl *textCtrl = new wxTextCtrl( &dg, wxID_ANY, wxT(""),
                                         wxDefaultPosition, wxDefaultSize,
                                         wxTE_MULTILINE|wxTE_READONLY );
  wxTextAttr ta( *wxRED, wxNullColour, wxNullFont, wxTEXT_ALIGNMENT_CENTRE );
  textCtrl->SetDefaultStyle( ta );
  textCtrl->AppendText( wxT(" \n") );
  textCtrl->AppendText( wxT("EXTREMA\n") );
  textCtrl->AppendText( wxT(" \n") );
  wxTextAttr ta2( *wxBLACK, wxNullColour, wxNullFont, wxTEXT_ALIGNMENT_CENTRE );
  textCtrl->SetDefaultStyle( ta2 );
  textCtrl->AppendText( wxString()<<wxT("version ")<<wxT(VERSION)<<wxT("\n") );
  textCtrl->AppendText( wxT(" \n") );
  textCtrl->AppendText( wxT("Copyright 2005-10 by Joseph L. Chuma\n") );
  textCtrl->AppendText( wxT("Copyright 2012-21 by Edward Sternin\n") );

  textCtrl->AppendText( wxT(" \n") );
  textCtrl->AppendText( wxT("Extrema comes with ABSOLUTELY NO WARRANTY\n") );
  textCtrl->AppendText( wxT("This is free software, distributed under the\n") );
  textCtrl->AppendText( wxT("GNU General Public License, version 2 or greater,\n") );
  textCtrl->AppendText( wxT("as published by the FSF.\n") );
  sizer->Add( textCtrl, wxSizerFlags(1).Align(0).Expand().Border(wxALL,1) );
  
  wxButton *ok = new wxButton( &dg, wxID_OK, wxT("OK") );
  sizer->Add( ok, wxSizerFlags(0).Centre().Border(wxALL,2) );

  dg.SetSizer( sizer );

  dg.ShowModal();
}

void AnalysisWindow::ZeroExecuteDialog()
{ executeDialog_ = 0; }

ExecuteDialog *AnalysisWindow::GetExecuteDialog()
{ return executeDialog_; }

void AnalysisWindow::SetExecuteDialog( ExecuteDialog *ed )
{ executeDialog_ = ed; }

void AnalysisWindow::ZeroShowVariables()
{ showVariablesForm_ = 0; }

void AnalysisWindow::ZeroLoadVectors()
{ readVectorsForm_ = 0; }

void AnalysisWindow::ZeroLoadScalars()
{ readScalarsForm_ = 0; }

void AnalysisWindow::ZeroLoadMatrix()
{ readMatrixForm_ = 0; }

void AnalysisWindow::ZeroLoadText()
{ readTextForm_ = 0; }

void AnalysisWindow::ZeroWriteVectors()
{ writeVectorsForm_ = 0; }

void AnalysisWindow::ZeroWriteScalars()
{ writeScalarsForm_ = 0; }

void AnalysisWindow::ZeroWriteMatrix()
{ writeMatrixForm_ = 0; }

void AnalysisWindow::ZeroWriteText()
{ writeTextForm_ = 0; }

void AnalysisWindow::ZeroGenerateVector()
{ generateVectorForm_ = 0; }

void AnalysisWindow::ZeroFit()
{ fitForm_ = 0; }

void AnalysisWindow::ZeroStackDialog()
{ stackDialog_ = 0; }

FitForm *AnalysisWindow::GetFitForm()
{ return fitForm_; }

void AnalysisWindow::SetFitForm( FitForm *fitForm )
{ fitForm_ = fitForm; }

GenerateVectorForm *AnalysisWindow::GetGenerateVectorForm()
{ return generateVectorForm_; }

void AnalysisWindow::SetGenerateVectorForm( GenerateVectorForm *form )
{ generateVectorForm_ = form; }

void AnalysisWindow::OnExecute( wxCommandEvent &WXUNUSED(event) )
{
  if( executeDialog_ )executeDialog_->Raise();
  else
  {
    executeDialog_ = new ExecuteDialog( this );
    executeDialog_->Show();
  }
}

void AnalysisWindow::OnShowVariables( wxCommandEvent &WXUNUSED(event) )
{
  if( showVariablesForm_ )showVariablesForm_->Raise();
  else
  {
    showVariablesForm_ = new ShowVariablesForm( this );
    showVariablesForm_->Show();
  }
}

void AnalysisWindow::OnLoadVectors( wxCommandEvent &WXUNUSED(event) )
{
  if( readVectorsForm_ )readVectorsForm_->Raise();
  else
  {
    readVectorsForm_ = new ReadVectorsForm( this );
    readVectorsForm_->Show();
  }
}

void AnalysisWindow::OnLoadScalars( wxCommandEvent &WXUNUSED(event) )
{
  if( readScalarsForm_ )readScalarsForm_->Raise();
  else
  {
    readScalarsForm_ = new ReadScalarsForm( this );
    readScalarsForm_->Show();
  }
}

void AnalysisWindow::OnLoadMatrix( wxCommandEvent &WXUNUSED(event) )
{
  if( readMatrixForm_ )readMatrixForm_->Raise();
  else
  {
    readMatrixForm_ = new ReadMatrixForm( this );
    readMatrixForm_->Show();
  }
}

void AnalysisWindow::OnLoadText( wxCommandEvent &WXUNUSED(event) )
{
  if( readTextForm_ )readTextForm_->Raise();
  else
  {
    readTextForm_ = new ReadTextForm( this );
    readTextForm_->Show();
  }
}

void AnalysisWindow::OnWriteVectors( wxCommandEvent &WXUNUSED(event) )
{
  if( writeVectorsForm_ )writeVectorsForm_->Raise();
  else
  {
    writeVectorsForm_ = new WriteVectorsForm( this );
    writeVectorsForm_->Show();
  }
}

void AnalysisWindow::OnWriteScalars( wxCommandEvent &WXUNUSED(event) )
{
  if( writeScalarsForm_ )writeScalarsForm_->Raise();
  else
  {
    writeScalarsForm_ = new WriteScalarsForm( this );
    writeScalarsForm_->Show();
  }
}

void AnalysisWindow::OnWriteMatrix( wxCommandEvent &WXUNUSED(event) )
{
  if( writeMatrixForm_ )writeMatrixForm_->Raise();
  else
  {
    writeMatrixForm_ = new WriteMatrixForm( this );
    writeMatrixForm_->Show();
  }
}

void AnalysisWindow::OnWriteText( wxCommandEvent &WXUNUSED(event) )
{
  if( writeTextForm_ )writeTextForm_->Raise();
  else
  {
    writeTextForm_ = new WriteTextForm( this );
    writeTextForm_->Show();
  }
}

void AnalysisWindow::OnGenerateVector( wxCommandEvent &WXUNUSED(event) )
{
  if( generateVectorForm_ )generateVectorForm_->Raise();
  else
  {
    generateVectorForm_ = new GenerateVectorForm( this );
    generateVectorForm_->Show();
  }
}

void AnalysisWindow::OnFit( wxCommandEvent &WXUNUSED(event) )
{
  if( fitForm_ )fitForm_->Raise();
  else
  {
    fitForm_ = new FitForm( this );
    fitForm_->Show();
  }
}

void AnalysisWindow::OnStackToggle( wxCommandEvent &WXUNUSED(event) )
{
  if( stackDialog_ )stackDialog_->Raise();
  else
  {
    stackDialog_ = new StackDialog( this );
    stackDialog_->Show();
  }
}

std::ostream &operator<<( std::ostream &out, AnalysisWindow const *aw )
{
  int ulx, uly;
  aw->GetPosition( &ulx, &uly );
  int width, height;
  aw->GetSize( &width, &height );
  out << "<analysiswindow top=\"" << uly << "\" left=\"" << ulx
      << "\" height=\"" << height << "\" width=\"" << width << "\">\n";
  out << "<string><![CDATA[" << aw->messagePanel_->GetValue().mb_str(wxConvUTF8)
      << "]]></string>\n";
  std::vector<wxString> strings( aw->commandPanel_->GetCommandStrings() );
  int size = strings.size();
  out << "<commands size=\"" << size << "\">\n";
  for( int i=0; i<size; ++i )
    out << "<string>" << strings[i].mb_str(wxConvUTF8) << "</string>\n";
  out << "</commands>\n";
  out << "</analysiswindow>\n";
  return out;
}

// end of file
