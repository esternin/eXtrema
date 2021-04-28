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

#include "wx/statline.h"
#include "wx/stattext.h"

#include <wx/persist/toplevel.h>

#include "ShowVariablesForm.h"
#include "AnalysisWindow.h"
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "TextVariable.h"
#include "VarInfoForm.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( ShowVariablesForm, wxFrame )
  EVT_BUTTON( wxID_REFRESH, ShowVariablesForm::OnRefresh )
  EVT_BUTTON( wxID_CLOSE, ShowVariablesForm::OnClose )
  EVT_CLOSE( ShowVariablesForm::CloseEventHandler )
  EVT_GRID_SELECT_CELL( ShowVariablesForm::OnSelectCell )
END_EVENT_TABLE()

ShowVariablesForm::ShowVariablesForm( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Show variables"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      analysisWindow_(parent)
{
  varInfoForm_ = 0;

  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  
  wxPanel *gridPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize(550,350), wxNO_BORDER );
  wxBoxSizer *gridSizer = new wxBoxSizer( wxHORIZONTAL );
  //
  // create the variable grid
  varGrid_ = new wxGrid( gridPanel, wxID_ANY, wxDefaultPosition, wxSize(540,340), wxSUNKEN_BORDER|wxVSCROLL );
  varGrid_->CreateGrid( 50, 4 );
  varGrid_->SetColSize( 0, 140 );
  varGrid_->SetColSize( 1, 100 );
  varGrid_->SetColSize( 2, 100 );
  varGrid_->SetColSize( 3, 100 );
  varGrid_->SetColLabelValue( 0, wxT("Scalars") );
  varGrid_->SetColLabelValue( 1, wxT("Vectors") );
  varGrid_->SetColLabelValue( 2, wxT("Matrices") );
  varGrid_->SetColLabelValue( 3, wxT("Strings") );
  wxGridCellAttr *attr = new wxGridCellAttr();
  attr->SetReadOnly(true);
  varGrid_->SetColAttr( 0, attr );
  attr = new wxGridCellAttr();
  attr->SetReadOnly(true);
  varGrid_->SetColAttr( 1, attr );
  attr = new wxGridCellAttr();
  attr->SetReadOnly(true);
  varGrid_->SetColAttr( 2, attr );
  attr = new wxGridCellAttr();
  attr->SetReadOnly(true);
  varGrid_->SetColAttr( 3, attr );
  gridSizer->Add( varGrid_, wxSizerFlags(1).Expand().Border(wxALL,1) );

  mainSizer->Add( gridPanel, wxSizerFlags(1).Expand().Border(wxALL,2) );

  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );
  
  wxButton *refreshButton = new wxButton( bottomPanel, wxID_REFRESH, wxT("Refresh") );
  refreshButton->SetToolTip( wxT("refresh the display of current variables") );
  bottomSizer->Add( refreshButton, wxSizerFlags(0).Border(wxALL,10) );
  
  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );
  
  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );

  SetSizer( mainSizer );

  wxPersistentRegisterAndRestore(this, "ShowVariablesForm");
  Show( true );

  FillGrid();
}

void ShowVariablesForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
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
  analysisWindow_->ZeroShowVariables();
  Destroy();
}

void ShowVariablesForm::OnRefresh( wxCommandEvent &WXUNUSED(event) )
{ FillGrid(); }

void ShowVariablesForm::FillGrid()
{
  if( varGrid_->GetNumberRows() > 0 )
  {
    varGrid_->ClearGrid();
    varGrid_->DeleteRows( 0, varGrid_->GetNumberRows() );
  }
  NVariableTable *nvTable = NVariableTable::GetTable();
  std::vector<wxString> scalarNames, vectorNames, matrixNames, stringNames;
  int end = nvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry( i );
    wxString name( nv->GetName() );
    switch ( nv->GetData().GetNumberOfDimensions() )
    {
      case 0:
        scalarNames.push_back( name );
        break;
      case 1:
        vectorNames.push_back( name );
        break;
      case 2:
        matrixNames.push_back( name );
        break;
    }
  }
  TVariableTable *tvTable = TVariableTable::GetTable();
  end = tvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    TextVariable *tv = tvTable->GetEntry( i );
    stringNames.push_back( tv->GetName() );
  }
  std::size_t nRows = std::max( scalarNames.size(),
                                std::max(vectorNames.size(),std::max(matrixNames.size(),stringNames.size())) );
  if( nRows > 0 )
  {
    varGrid_->InsertRows( 0, nRows );
    for( std::size_t i=0; i<nRows; ++i )
    {
      if( i < scalarNames.size() )varGrid_->SetCellValue( i, 0, scalarNames[i] );
      if( i < vectorNames.size() )varGrid_->SetCellValue( i, 1, vectorNames[i] );
      if( i < matrixNames.size() )varGrid_->SetCellValue( i, 2, matrixNames[i] );
      if( i < stringNames.size() )varGrid_->SetCellValue( i, 3, stringNames[i] );
    }
  }
}

void ShowVariablesForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void ShowVariablesForm::ZeroVarInfoForm()
{ varInfoForm_ = 0; }

void ShowVariablesForm::OnSelectCell( wxGridEvent &e )
{
  int row = e.GetRow();
  int column = e.GetCol();
  //
  e.Skip();
  //
  wxString name( varGrid_->GetCellValue(row,column) );
  if( !name.empty() )
  {
    if( !varInfoForm_ )varInfoForm_ = new VarInfoForm( this );
    varInfoForm_->DisplayInfo( name );
  }
}

// end of file
