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
#include "wx/gdicmn.h"

#include "HintForm.h"

HintForm::HintForm()
    : wxFrame((wxWindow*)NULL,wxID_ANY,wxT(""),wxDefaultPosition,wxDefaultSize,
              wxSYSTEM_MENU|wxRESIZE_BORDER|wxCAPTION&~(wxMINIMIZE_BOX|wxMAXIMIZE_BOX))
{
  int screenWidth, screenHeight;
  ::wxDisplaySize( &screenWidth, &screenHeight );
  wxConfigBase *config = wxConfigBase::Get();
  int width = config->Read( wxT("/HintForm/WIDTH"), 320l );
  int height = config->Read( wxT("/HintForm/HEIGHT"), 60l );
  int ulx = config->Read( wxT("/HintForm/UPPERLEFTX"), static_cast<long>((screenWidth-width)/2) );
  int uly = config->Read( wxT("/HintForm/UPPERLEFTY"), static_cast<long>((screenHeight-height)/2) );
  SetSize( ulx, uly, width, height );
  //
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  textLabel_ = new wxStaticText( this, wxID_ANY, wxT("") );
  mainSizer->Add( textLabel_, wxSizerFlags(1).Centre().Border(wxALL,20) );
  SetSizer( mainSizer );
  SetBackgroundColour( wxColour(255,250,205) );
  Layout();
}

void HintForm::ShowHint( std::vector<wxString> const &lines )
{
  wxString text;
  for( std::size_t i=0; i<lines.size()-1; ++i )text += lines[i] + wxT("\n");
  text += lines.back();
  textLabel_->SetLabel( text );
  Layout();
  Show( true );
  Raise();
}

void HintForm::HideHint()
{
  Show( false );
}

HintForm::~HintForm()
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/HintForm/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/HintForm/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/HintForm/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/HintForm/HEIGHT"), static_cast<long>(height) );
  }
}

// end of file
