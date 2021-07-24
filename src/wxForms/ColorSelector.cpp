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

#include <wx/persist/toplevel.h>

#include "ColorSelector.h"
#include "GRA_color.h"

#include "ParseLine.h"
#include "ESyntaxError.h"
#include "ECommandError.h"
#include "ExGlobals.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( ColorSelector, wxFrame )
  EVT_BUTTON( wxID_OK, ColorSelector::OnOK )
  EVT_BUTTON( wxID_CANCEL, ColorSelector::OnCancel )
  EVT_GRID_SELECT_CELL( ColorSelector::OnSelectCell )
  EVT_RADIOBOX( ID_radio, ColorSelector::OnRadio )
  EVT_CLOSE( ColorSelector::CloseEventHandler )
END_EVENT_TABLE()

ColorSelector::ColorSelector( wxWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("color selector"),
              wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE)
{
  wxPanel* const mainPanel = new wxPanel(this);
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  
  wxPanel *topPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );

  colorMapGrid_ = new wxGrid( topPanel, ID_grid, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
  colorMapGrid_->CreateGrid( 1, 1, wxGrid::wxGridSelectCells );
  topSizer->Add( colorMapGrid, wxSizerFlags(0).Left().Border(wxALL,5) );

  int ncolors = GRA_colorControl::GetNumberOfNamedColors();
  wxString *choices = new wxString[ncolors];
  for( int i=0; i<ncolors; ++i )choices[i] = GRA_colorControl::GetColor(-i)->GetName();

  namedColorsRB_ = new wxRadioBox( topPanel, ID_radio, wxT("Named colors"), wxDefaultPosition,
                                 wxDefaultSize, ncolors, choices, 3, wxRA_SPECIFY_COLS );
  topSizer->Add( namedColorsRB_, wxSizerFlags(0).Right().Border(wxALL,5) );

  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(1).Expand().Border(wxALL,1) );
  
  wxPanel *bottomPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );
  wxButton *okButton = new wxButton( bottomPanel, wxID_OK, wxT("OK") );
  okButton->SetToolTip( wxT("accept the color as chosen above") );
  bottomSizer->Add( okButton, wxSizerFlags(0).Border(wxALL,10) );
  wxButton *cancelButton = new wxButton( bottomPanel, wxID_CANCEL, wxT("Cancel") );
  cancelButton->SetToolTip( wxT("close this form without changing the color") );
  bottomSizer->Add( cancelButton, wxSizerFlags(0).Border(wxALL,10) );
  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(1).Expand().Centre().Border(wxALL,1) );

  mainPanel->SetSizer( mainSizer );
  
  wxPersistentRegisterAndRestore(this, "ColorSelector");

  Show( true );
  
  gridClick_ = false;
  namedColorClick_ = false;
}

void ColorSelector::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
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
  Destroy();
}

void ColorSelector::SetColor( GRA_color *color )
{
  namedColorsRB_->SetSelection( 0 );
  
  int ncolors = GRA_colorControl::GetColorMap()->GetSize();
  int ncols = static_cast<int>( sqrt(static_cast<double>(ncolors))+0.5 );
  int nrows = ncolors/ncols;
  while (ncols*nrows < ncolors) ++nrows;
  
  colorMapGrid_->AppendRows( nrows-1 );
  colorMapGrid_->AppendCols( ncols-1 );
  
  if( color )
  {
    int colorCode = GRA_colorControl::GetColorCode(color);
    if( color->IsNamed() )
    {
      // if the radio box size is 19, black coincides with index 0, not 1
      // but fill color has an extra entry, "no color", so in that case,
      // black coincides with indes 1, which is fine
      //
      if( namedColorsRB_->GetCount() == 19 )++colorCode;
      namedColorsRB_->SetSelection( abs(colorCode) );
    }
    else
    {
      int col = colorCode % ncols;
      int row = (colorCode-col)/ncols;
      colorMapGrid_->SetGridCursor( row, col );
    }
}

void ColorSelector::OnSelectCell( wxGridEvent &WXUNUSED(event) )
{
  gridClick_ = true;
  namedColorClick_ = false;
} 

void ColorSelector::OnRadio( wxCommandEvent &WXUNUSED(event) )
{
  gridClick_ = false;
  namedColorClick_ = true;
}

       
void ColorSelector::OnOK( wxCommandEvent &WXUNUSED(event) )
{
  Apply();
}

void ColorSelector::Apply()
{
  wxString fileName( chooseFilePanel_->GetSelection() );
  if( fileName.empty() )
  {
    wxMessageDialog md( this, wxT("no file has been chosen"),
                           wxT("Warning"), wxOK|wxICON_INFORMATION );
    md.ShowModal();
    return;
  }
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
    wxMessageDialog md( this, wxString(e.what(),wxConvUTF8),
                        wxT("Fatal error"), wxOK|wxICON_ERROR );
    md.ShowModal();
    return;
  }
  try
  {
    CMD_execute::Definition()->Execute( &p );
  }
  catch( ECommandError &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8),
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
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
}

void ColorSelector::OnClose( wxCommandEvent &WXUNUSED(event) )
{
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("FORM EXECUTE,CLOSE") );
  Close();
}

void ColorSelector::Set( wxString const &filename, wxString const &parameters, wxString const &action )
{
  chooseFilePanel_->SetFile( filename );
  parameterTextCtrl_->SetValue( parameters );
  if( action == wxT("CLOSE") )Close();
  else if( action == wxT("APPLY") )Apply();
}
  
// end of file
