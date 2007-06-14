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
#include <deque>

#include "wx/config.h"

#include "VarInfoForm.h"
#include "ShowVariablesForm.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "NumericVariable.h"
#include "TextVariable.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( VarInfoForm, wxFrame )
  EVT_BUTTON( ID_refresh, VarInfoForm::OnRefresh )
  EVT_BUTTON( wxID_CLOSE, VarInfoForm::OnClose )
  EVT_CLOSE( VarInfoForm::CloseEventHandler )
END_EVENT_TABLE()

VarInfoForm::VarInfoForm( ShowVariablesForm *parent )
    : wxFrame(parent,wxID_ANY,wxT("variable info"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      showVariablesForm_(parent)
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );

  textCtrl_ = new wxTextCtrl( this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
                              wxTE_MULTILINE|wxTE_READONLY|wxSUNKEN_BORDER );

  mainSizer->Add( textCtrl_, wxSizerFlags(1).Expand().Border(wxALL,1) );
  
  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );
  wxButton *refreshButton = new wxButton( bottomPanel, ID_refresh, wxT("Refresh") );
  refreshButton->SetToolTip( wxT("refresh the variable names") );
  bottomSizer->Add( refreshButton, wxSizerFlags(0).Border(wxALL,10) );
  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );
  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );

  SetSizer( mainSizer );
  
  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/VarInfoForm/UPPERLEFTX"), 0l );
  int uly = config->Read( wxT("/VarInfoForm/UPPERLEFTY"), 640l );
  int width = config->Read( wxT("/VarInfoForm/WIDTH"), 570l );
  int height = config->Read( wxT("/VarInfoForm/HEIGHT"), 145l );
  SetSize( ulx, uly, width, height );

  Show( true );
}

void VarInfoForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/VarInfoForm/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/VarInfoForm/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/VarInfoForm/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/VarInfoForm/HEIGHT"), static_cast<long>(height) );
  }
  //
  // close all child windows
  //
  wxWindowList children( GetChildren() );
  wxWindowListNode *node = children.GetFirst();
  while( node )
  {
    wxWindow *window = node->GetData();
    window->Close(true);
    node = node->GetNext();
  }
  showVariablesForm_->ZeroVarInfoForm();
  Destroy();
}

void VarInfoForm::OnRefresh( wxCommandEvent &WXUNUSED(event) )
{ if( !name_.empty() )DisplayInfo( name_ ); }

void VarInfoForm::DisplayInfo( wxString const &name )
{
  textCtrl_->Clear();
  //
  name_ = name;
  //
  NumericVariable *nv = NVariableTable::GetTable()->GetVariable( name );
  TextVariable *tv = TVariableTable::GetTable()->GetVariable( name );
  if( nv )
  {
    NumericData nd( nv->GetData() );
    wxString s( nv->GetName() );
    wxString type;
    int ndim = nd.GetNumberOfDimensions();
    switch ( ndim )
    {
      case 0:
      {
        textCtrl_->AppendText( nv->GetName()+wxT(" is a scalar\n") );
        if( nd.IsFit() )textCtrl_->AppendText( wxT("allowed to vary in a fit\n") );
        wxString s( wxT("value = ") );
        textCtrl_->AppendText( s << nd.GetScalarValue() << wxT("\n") );
        break;
      }
      case 1:
      {
        textCtrl_->AppendText( nv->GetName()+wxT(" is a vector\n") );
        wxString s( wxT("length = ") );
        textCtrl_->AppendText( s << nd.GetDimMag(0) << wxT("\n") );
        break;
      }
      case 2:
      {
        textCtrl_->AppendText( nv->GetName()+wxT(" is a matrix\n") );
        wxString s( wxT("number of rows = ") );
        textCtrl_->AppendText( s << nd.GetDimMag(0) << wxT(", number of columns = ") << nd.GetDimMag(1) << wxT("\n") );
        break;
      }
      case 3:
      {
        textCtrl_->AppendText( nv->GetName()+wxT(" is a tensor\n") );
        wxString s( wxT("number of rows = ") );
        textCtrl_->AppendText( s << nd.GetDimMag(0) << wxT(", number of columns = ") << nd.GetDimMag(1)
                               << wxT(", number of planes = ") << nd.GetDimMag(2) << wxT("\n") );
        break;
      }
    }
    textCtrl_->AppendText( wxT("History (most recent at the top)\n") );
    textCtrl_->AppendText( wxT("----------------------------------------\n") );
    std::deque<wxString> nvH( nv->GetHistory() );
    std::size_t size = nvH.size();
    for( std::size_t i=0; i<size; ++i )textCtrl_->AppendText( nvH[i]+wxT("\n") );
  }
  else if( tv )
  {
    TextData td( tv->GetData() );
    int ndim = td.GetNumberOfDimensions();
    if( ndim == 0 )
    {
      textCtrl_->AppendText( tv->GetName()+wxT(" is a text scalar\n") );
      wxString s( wxT("number of characters = ") );
      textCtrl_->AppendText( s << td.GetScalarValue().size() << wxT("\n") );
    }
    else
    {
      textCtrl_->AppendText( tv->GetName()+wxT(" is a text vector\n") );
      wxString s( wxT("number of strings = ") );
      textCtrl_->AppendText( s << td.GetData().size() << wxT("\n") );
    }
    textCtrl_->AppendText( wxT("History (most recent at the top)\n") );
    textCtrl_->AppendText( wxT("----------------------------------------\n") );
    std::deque<wxString> tvH( tv->GetHistory() );
    std::size_t size = tvH.size();
    for( std::size_t i=0; i<size; ++i )textCtrl_->AppendText( tvH[i]+wxT("\n") );
  }
  else
    textCtrl_->AppendText( wxT("variable no longer exists\n") );
}

void VarInfoForm::OnClose( wxCommandEvent &event )
{ Close(); }

// end of file
