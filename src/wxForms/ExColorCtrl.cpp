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

#include "ExColorCtrl.h"
#include "GRA_color.h"
#include "ExGlobals.h"
#include "BasicColors.h"

//BEGIN_EVENT_TABLE( ExColorCtrl<T>, wxPanel )
//  EVT_BUTTON( ID_button, ExColorCtrl<T>::OnClick )
//END_EVENT_TABLE()

template<typename T>
ExColorCtrl<T>::ExColorCtrl( wxPanel *parent, T *popup, void (T::*memFunc)(GRA_color*) )
    : wxPanel(parent), popup_(popup), memFunc_(memFunc)
{
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( sizer );

  sizer->Add( new wxStaticText(this,wxID_ANY,wxT("Color")), wxSizerFlags(0).Center().Border(wxALL,2) );

  button_ = new wxButton( this, ID_button, wxT(""), wxDefaultPosition, wxSize(25,25), wxRAISED_BORDER );
  sizer->Add( button_, wxSizerFlags(0).Center().Border(wxALL,1) );

  Connect( ID_button, wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED,
           (wxObjectEventFunction)
           (wxEventFunction)
           (wxCommandEventFunction)&ExColorCtrl<T>::OnClick );

}

template<typename T>
void ExColorCtrl<T>::SetColor( GRA_color *color )
{
  button_->SetBackgroundColour( ExGlobals::GetwxColor(color) );
}

template<typename T>
void ExColorCtrl<T>::OnClick( wxCommandEvent &WXUNUSED(event) )
{
  ExGlobals::SetWorkingColorFlag( false );
  BasicColors *bc = new BasicColors( this );
  bc->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *color = ExGlobals::GetWorkingColor();
    (popup_->*memFunc_)( color );
    button_->SetBackgroundColour( ExGlobals::GetwxColor(color) );
  }
}
