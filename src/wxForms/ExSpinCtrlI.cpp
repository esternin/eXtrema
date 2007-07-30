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

#include "ExSpinCtrlI.h"

//BEGIN_EVENT_TABLE( ExSpinCtrlI, wxPanel, T )
//  EVT_SPIN( ID_spin, ExSpinCtrlI<T>::OnSpinChange )
//  EVT_TEXT( ID_text, ExSpinCtrlI<T>::OnTextChange )
//END_EVENT_TABLE()

template<typename T>
ExSpinCtrlI<T>::ExSpinCtrlI( wxPanel *parent, wxString const &label, int min, int max,
                             T *popup, void (T::*memFunc)(int) )
    : wxPanel(parent), popup_(popup), memFunc_(memFunc)
{
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( sizer );

  sizer->Add( new wxStaticText(this,wxID_ANY,label), wxSizerFlags(0).Left().Border(wxALL,2) );

  wxPanel *panel2 = new wxPanel( this );
  wxBoxSizer *sizer2 = new wxBoxSizer( wxHORIZONTAL );
  panel2->SetSizer( sizer2 );
  sizer->Add( panel2, wxSizerFlags(0).Center().Border(wxALL,2) );

  textCtrl_ = new wxTextCtrl( panel2, ID_text );
  sizer2->Add( textCtrl_, wxSizerFlags(0).Left().Border(wxALL,1) );
  
  spinButton_ = new wxSpinButton( panel2, ID_spin );
  spinButton_->SetRange( min, max );
  sizer2->Add( spinButton_, wxSizerFlags(0).Left().Border(wxALL,1) );

  Connect( ID_spin, wxID_ANY, wxEVT_SCROLL_THUMBTRACK, 
           (wxObjectEventFunction)
           (wxEventFunction)
           (wxCommandEventFunction)&ExSpinCtrlI<T>::OnSpinChange );

  Connect( ID_text, wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, 
           (wxObjectEventFunction)
           (wxEventFunction)
           (wxCommandEventFunction)&ExSpinCtrlI<T>::OnTextChange );
}

template<typename T>
void ExSpinCtrlI<T>::SetValue( int val )
{
  spinButton_->SetValue( val );
  textCtrl_->SetValue( wxString()<<val );
}

template<typename T>
int ExSpinCtrlI<T>::GetValue() const
{
  int i;
  textCtrl_->GetValue() >> i;
  return i;
}
  
template<typename T>
void ExSpinCtrlI<T>::OnSpinChange( wxSpinEvent &WXUNUSED(event) )
{
  int val = spinButton_->GetValue();
  textCtrl_->SetValue( wxString()<<val );
  (popup_->*memFunc_)( val );
}

template<typename T>
void ExSpinCtrlI<T>::OnTextChange( wxCommandEvent &WXUNUSED(event) )
{
  wxString text( textCtrl_->GetValue() );
  if( text.empty() )return;
  long val;
  if( !text.ToLong(&val) || val<spinButton_->GetMin() || val>spinButton_->GetMax() )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxT("invalid value entered"), wxT("Error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  (popup_->*memFunc_)( static_cast<int>(val) );
}
