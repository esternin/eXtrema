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
#ifndef EXTREMA_SPINCTRLD
#define EXTREMA_SPINCTRLD

#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/spinbutt.h"
#include "wx/textctrl.h"

template<typename T>
class ExSpinCtrlD : public wxPanel
{
public:
  ExSpinCtrlD( wxPanel *parent, wxString const &label, T *popup, void (T::*memFunc)(double) )
      : wxPanel(parent), popup_(popup), memFunc_(memFunc)
  {
    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    SetSizer( sizer );
    sizer->Add( new wxStaticText(this,wxID_ANY,label), 0,wxALIGN_LEFT|wxALL,2 );
    //wxSizerFlags flags(0);
    //sizer->Add( new wxStaticText(this,wxID_ANY,label), flags.Left().Border(wxALL,2) );
    
    wxPanel *panel2 = new wxPanel( this );
    wxBoxSizer *sizer2 = new wxBoxSizer( wxHORIZONTAL );
    panel2->SetSizer( sizer2 );
    sizer->Add( panel2, 0,wxALIGN_CENTER|wxALL,1 );
    //sizer->Add( panel2, flags.Center().Border(wxALL,1) );
    
    textCtrl_ = new wxTextCtrl( panel2, ID_text );
    sizer2->Add( textCtrl_, 0,wxALIGN_LEFT|wxALL,1 );
    //sizer2->Add( textCtrl_, flags.Left().Border(wxALL,1) );
    
    spinButton_ = new wxSpinButton( panel2, ID_spin );
    spinButton_->SetRange( 0, 1000 );
    sizer2->Add( spinButton_, 0,wxALIGN_LEFT|wxALL,1 );
    //sizer2->Add( spinButton_, flags.Left().Border(wxALL,1) );
    
    Connect( ID_spin, wxID_ANY, wxEVT_SCROLL_THUMBTRACK, 
             (wxObjectEventFunction)
             (wxEventFunction)
             (wxCommandEventFunction)&ExSpinCtrlD<T>::OnSpinChange );
    
    Connect( ID_text, wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, 
             (wxObjectEventFunction)
             (wxEventFunction)
             (wxCommandEventFunction)&ExSpinCtrlD<T>::OnTextChange );
  }
 
  void SetValue( double val )
  {
    int itmp = static_cast<int>(val*10+0.5);
    spinButton_->SetValue( itmp );
    textCtrl_->SetValue( wxString()<<itmp/10. );
  }

private:
  void OnSpinChange( wxSpinEvent &WXUNUSED(event) )
  {
    double val = 0.1*spinButton_->GetValue();
    textCtrl_->SetValue( wxString()<<val );
    (popup_->*memFunc_)( val );
  }
  
  void OnTextChange( wxCommandEvent &WXUNUSED(event) )
  {
    wxString text( textCtrl_->GetValue() );
    if( text.empty() )return;
    double val;
    if( !text.ToDouble(&val) || val<0.0 || val>100.0 )
    {
      wxMessageBox( wxT("invalid value entered"),
                    wxT("Error"), wxOK|wxICON_ERROR, this );
      return;
    }
    (popup_->*memFunc_)( val );
  }

  wxSpinButton *spinButton_;
  wxTextCtrl *textCtrl_;

  T *popup_;
  void (T::* memFunc_)(double);

  enum {
      ID_spin,
      ID_text
  };
  
  //DECLARE_EVENT_TABLE()
};

#endif
