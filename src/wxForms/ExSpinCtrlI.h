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
#ifndef EXTREMA_SPINCTRLI
#define EXTREMA_SPINCTRLI

#include "wx/wx.h"
#include "wx/sizer.h"
#include "wx/spinbutt.h"
#include "wx/textctrl.h"

template<typename T>
class ExSpinCtrlI : public wxPanel
{
public:
  ExSpinCtrlI( wxPanel *parent, wxString const &label, int min, int max,
               T *popup, void (T::*memFunc)(int) )
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
    sizer->Add( panel2, 0,wxALIGN_CENTER|wxALL,2 );
    //sizer->Add( panel2, flags.Center().Border(wxALL,2) );
    
    textCtrl_ = new wxTextCtrl( panel2, ID_text );
    sizer2->Add( textCtrl_, 0,wxALIGN_LEFT|wxALL,1 );
    //sizer2->Add( textCtrl_, flags.Left().Border(wxALL,1) );
    
    spinButton_ = new wxSpinButton( panel2, ID_spin );
    spinButton_->SetRange( min, max );
    sizer2->Add( spinButton_, 0,wxALIGN_LEFT|wxALL,1 );
    //sizer2->Add( spinButton_, flags.Left().Border(wxALL,1) );
    
    Connect( ID_spin, wxID_ANY, wxEVT_SCROLL_THUMBTRACK, 
             (wxObjectEventFunction)
             (wxEventFunction)
             (wxCommandEventFunction)&ExSpinCtrlI<T>::OnSpinChange );
    
    Connect( ID_text, wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, 
             (wxObjectEventFunction)
             (wxEventFunction)
             (wxCommandEventFunction)&ExSpinCtrlI<T>::OnTextChange );
  }

  void SetValue( int val )
  {
    spinButton_->SetValue( val );
    textCtrl_->SetValue( wxString()<<val );
  }

  int GetValue() const
  {
    long i;
    textCtrl_->GetValue().ToLong(&i);
    return static_cast<int>(i);
  }

private:
  void OnSpinChange( wxSpinEvent &WXUNUSED(event) )
  {
    int val = spinButton_->GetValue();
    textCtrl_->SetValue( wxString()<<val );
    (popup_->*memFunc_)( val );
  }

  void OnTextChange( wxCommandEvent &WXUNUSED(event) )
  {
    wxString text( textCtrl_->GetValue() );
    if( text.empty() )return;
    long val;
    if( !text.ToLong(&val) || val<spinButton_->GetMin() || val>spinButton_->GetMax() )
    {
      wxMessageBox( wxT("invalid value entered"),
                    wxT("Error"), wxOK|wxICON_ERROR, this );
      return;
    }
    (popup_->*memFunc_)( static_cast<int>(val) );
  }
  
  wxSpinButton *spinButton_;
  wxTextCtrl *textCtrl_;

  T *popup_;
  void (T::* memFunc_)(int);

  enum {
      ID_spin,
      ID_text
  };
  
  //DECLARE_EVENT_TABLE()
};

#endif
