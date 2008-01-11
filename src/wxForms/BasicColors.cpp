/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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

#include "wx/config.h"

#include "BasicColors.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "ExGlobals.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( BasicColors, wxDialog )
  EVT_CLOSE( BasicColors::CloseEventHandler )
END_EVENT_TABLE()

BasicColors::BasicColors( wxWindow *parent )
    : wxDialog(parent,wxID_ANY,wxT("Basic colors"),wxDefaultPosition,wxDefaultSize,wxCAPTION)
{
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( sizer );
  
  wxPanel *mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  mainPanel->SetSizer( mainSizer );
  sizer->Add( mainPanel, wxSizerFlags(1).Center().Border(wxALL,2) );
  
  for( int j=0; j<4; ++j )
  {
    wxPanel *pnl = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
    wxBoxSizer *szr = new wxBoxSizer( wxHORIZONTAL );
    pnl->SetSizer( szr );
    mainSizer->Add( pnl, wxSizerFlags(1).Expand().Border(wxALL,1) );
    for( int i=j*5; i<(j+1)*5; ++i )
    {
      ColorButton *cb = new ColorButton( pnl, this, -i );
      szr->Add( cb, wxSizerFlags(1).Border(wxALL,1) );
    }
  }
  wxConfigBase *config = wxConfigBase::Get();
  int width = config->Read( wxT("/BasicColors/WIDTH"), 165l );
  int height = config->Read( wxT("/BasicColors/HEIGHT"), 140l );
  SetSize( 0, 0, width, height );
  Show( true );
}

void BasicColors::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/BasicColors/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/BasicColors/HEIGHT"), static_cast<long>(height) );
  }
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

//---------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( ColorButton, wxButton )
  EVT_BUTTON( ID_button, ColorButton::OnClick )
END_EVENT_TABLE()
    
ColorButton::ColorButton( wxPanel *parent, BasicColors *owner, int colorCode ) 
    : wxButton(parent,ID_button,wxT(""),wxDefaultPosition,wxSize(30,30),wxRAISED_BORDER),
      colorCode_(colorCode), owner_(owner)
{
  GRA_color *color = GRA_colorControl::GetColor( colorCode_ );
  SetBackgroundColour( ExGlobals::GetwxColor(color) );
  SetToolTip( color->GetName() );
}

void ColorButton::OnClick( wxCommandEvent &WXUNUSED(event) )
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( colorCode_ );
  owner_->Close();
}
  
  
// end of file
