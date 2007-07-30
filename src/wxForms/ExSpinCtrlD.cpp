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

#include "ExSpinCtrlD.h"

//BEGIN_EVENT_TABLE( ExSpinCtrlD, wxPanel, T )
//  EVT_SPIN( ID_spin, ExSpinCtrlD<T>::OnSpinChange )
//  EVT_TEXT( ID_text, ExSpinCtrlD<T>::OnTextChange )
//END_EVENT_TABLE()

template<typename T>
ExSpinCtrlD<T>::ExSpinCtrlD( wxPanel *parent, wxString const &label, T *popup, void (T::* memFunc)(double) )
    : wxPanel(parent), popup_(popup), memFunc_(memFunc)
{
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( sizer );
  
  sizer->Add( new wxStaticText(this,wxID_ANY,label), wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *panel2 = new wxPanel( this );
  wxBoxSizer *sizer2 = new wxBoxSizer( wxHORIZONTAL );
  panel2->SetSizer( sizer2 );
  sizer->Add( panel2, wxSizerFlags(0).Center().Border(wxALL,1) );
  
  textCtrl_ = new wxTextCtrl( panel2, ID_text );
  sizer2->Add( textCtrl_, wxSizerFlags(0).Left().Border(wxALL,1) );
  
  spinButton_ = new wxSpinButton( panel2, ID_spin );
  spinButton_->SetRange( 0, 1000 );
  sizer2->Add( spinButton_, wxSizerFlags(0).Left().Border(wxALL,1) );

  Connect( ID_spin, wxID_ANY, wxEVT_SCROLL_THUMBTRACK, 
           (wxObjectEventFunction)
           (wxEventFunction)
           (wxCommandEventFunction)&ExSpinCtrlD<T>::OnSpinChange );

  Connect( ID_text, wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, 
           (wxObjectEventFunction)
           (wxEventFunction)
           (wxCommandEventFunction)&ExSpinCtrlD<T>::OnTextChange );
}

template<typename T>
void ExSpinCtrlD<T>::SetValue( double val )
{
  int itmp = static_cast<int>(val*10+0.5);
  spinButton_->SetValue( itmp );
  textCtrl_->SetValue( wxString()<<itmp/10. );
}

template<typename T>
void ExSpinCtrlD<T>::OnSpinChange( wxSpinEvent &WXUNUSED(event) )
{
  double val = 0.1*spinButton_->GetValue();
  textCtrl_->SetValue( wxString()<<val );
  (popup_->*memFunc_)( val );
}

template<typename T>
void ExSpinCtrlD<T>::OnTextChange( wxCommandEvent &WXUNUSED(event) )
{
  wxString text( textCtrl_->GetValue() );
  if( text.empty() )return;
  double val;
  if( !text.ToDouble(&val) || val<0.0 || val>100.0 )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxT("invalid value entered"), wxT("Error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  (popup_->*memFunc_)( val );
}
