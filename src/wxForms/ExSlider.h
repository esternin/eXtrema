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
#ifndef EXTREMA_SLIDER
#define EXTREMA_SLIDER

#include "wx/wx.h"
#include "wx/slider.h"

template<typename T>
class ExSlider : public wxPanel
{
public:
  ExSlider( wxPanel *parent, wxString const &label, T *popup, void (T::*memFunc)(double) )
      : wxPanel(parent), popup_(popup), memFunc_(memFunc)
  {
    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    SetSizer( sizer );
    
    sizer->Add( new wxStaticText(this,wxID_ANY,label), wxSizerFlags(0).Center().Border(wxALL,2) );
    
    slider_ = new wxSlider( this, ID_slider, 0, 0, 100, wxDefaultPosition, wxSize(100,wxDefaultCoord),
                            wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_BOTTOM );
    slider_->SetLineSize( 1 );
    slider_->SetPageSize( 10 );
    sizer->Add( slider_, wxSizerFlags(0).Center().Border(wxALL,2) );
    
    value_ = new wxStaticText( this, wxID_ANY, wxT("") );
    sizer->Add( value_, wxSizerFlags(0).Center().Border(wxALL,2) );
    
    Connect( ID_slider, wxID_ANY, wxEVT_SCROLL_CHANGED,
             (wxObjectEventFunction)
             (wxEventFunction)
             (wxCommandEventFunction)&ExSlider<T>::OnSliderChange );
    //wxEVT_SCROLL_LINEUP|wxEVT_SCROLL_LINEDOWN|wxEVT_SCROLL_PAGEUP|wxEVT_SCROLL_PAGEDOWN|wxEVT_SCROLL_THUMBTRACK, 
  }
 
  void SetValue( double val )
  {
    int ival = static_cast<int>(val+0.5);
    slider_->SetValue( ival );
    value_->SetLabel( wxString()<<ival<<wxT("%") );
  }

private:
  void OnSliderChange( wxScrollEvent &WXUNUSED(event) )
  {
    int val = slider_->GetValue();
    value_->SetLabel( wxString()<<val<<wxT("%") );
    (popup_->*memFunc_)( static_cast<double>(val) );
  }

  wxSlider *slider_;
  wxStaticText *value_;

  T *popup_;
  void (T::* memFunc_)(double);

  enum {
      ID_slider
  };
};

#endif
