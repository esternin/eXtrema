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
#include "wx/config.h"
#include "wx/statline.h"
#include "wx/stattext.h"

#include "TextPopup.h"
#include "GraphicsPage.h"
#include "GRA_window.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "ExGlobals.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "BasicColors.h"
#include "EGraphicsError.h"
#include "GRA_drawableText.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( TextPopup, wxFrame )
  EVT_COMBOBOX( ID_font, TextPopup::OnFont )
  EVT_TEXT_ENTER( ID_string, TextPopup::OnStringEnter )
  EVT_BUTTON( wxID_CLOSE, TextPopup::OnClose )
  EVT_CLOSE( TextPopup::CloseEventHandler )
END_EVENT_TABLE()

TextPopup::TextPopup( GraphicsPage *parent )
    : wxFrame(parent,wxID_ANY,wxT("Text Popup"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      page_(parent)
{
  setup_ = true;
  CreateForm();
  //
  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/TextPopup/UPPERLEFTX"), 950l );
  int uly = config->Read( wxT("/TextPopup/UPPERLEFTY"), 150l );
  int width = config->Read( wxT("/TextPopup/WIDTH"), 270l );
  int height = config->Read( wxT("/TextPopup/HEIGHT"), 285l );
  SetSize( ulx, uly, width, height );
  Show( true );
  //
  Layout();
}

void TextPopup::CreateForm()
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( mainSizer );
  
  wxPanel *sPanel = new wxPanel( this );
  wxBoxSizer *sSizer = new wxBoxSizer( wxVERTICAL );
  sPanel->SetSizer( sSizer );
  mainSizer->Add( sPanel, wxSizerFlags(0).Center().Border(wxALL,5) );

  sSizer->Add( new wxStaticText(sPanel,wxID_ANY,wxT("String")), wxSizerFlags(0).Left().Border(wxTOP,2) );
  stringTC_ = new wxTextCtrl( sPanel, ID_string, wxT(""), wxDefaultPosition, wxSize(250,25),
                              wxTE_LEFT|wxTE_PROCESS_ENTER );
  stringTC_->SetToolTip( wxT("enter the new text string (hit Enter key to accept string)") );
  sSizer->Add( stringTC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  //
  wxPanel *topPanel = new wxPanel( this );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );
  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(0).Center().Border(wxALL,5) );
  //
  color_ = new ExColorCtrl<TextPopup>( topPanel, wxT("Color"), this, &TextPopup::OnColor );
  color_->SetToolTip( wxT("change the text color") );
  topSizer->Add( color_, wxSizerFlags(0).Left().Border(wxALL,2) );
  //
  heightSC_ =
      new ExSpinCtrlD<TextPopup>( topPanel, wxT("Height (%)"), this, &TextPopup::OnHeight );
  heightSC_->SetToolTip( wxT("set the text height (as a % of the window)") );
  topSizer->Add( heightSC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  //
  angleSC_ =
      new ExSpinCtrlI<TextPopup>( topPanel, wxT("Angle (degrees)"), 0, 360, this, &TextPopup::OnAngle );
  angleSC_->SetToolTip( wxT("set the text angle (0 to 360 degrees)") );
  topSizer->Add( angleSC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *fPanel = new wxPanel( this );
  wxBoxSizer *fSizer = new wxBoxSizer( wxVERTICAL );
  fPanel->SetSizer( fSizer );
  mainSizer->Add( fPanel, wxSizerFlags(0).Center().Border(wxALL,5) );
  //
  fSizer->Add( new wxStaticText(fPanel,wxID_ANY,wxT("Font")), wxSizerFlags(0).Left().Border(wxTOP,2) );
  int nf = GRA_fontControl::GetCount();
  wxString choices[nf];
  for( int i=0; i<nf; ++i )choices[i] = GRA_fontControl::GetFont(i)->GetFontName();
  fontCB_ = new wxComboBox( fPanel, ID_font, wxT(""), wxDefaultPosition, wxSize(200,25), nf,
                            choices, wxCB_READONLY );
  fontCB_->SetToolTip( wxT("choose the text font") );
  fSizer->Add( fontCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *botPanel = new wxPanel( this );
  wxBoxSizer *botSizer = new wxBoxSizer( wxHORIZONTAL );
  botPanel->SetSizer( botSizer );
  mainSizer->Add( botPanel, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  xLocSC_ =
      new ExSpinCtrlD<TextPopup>( botPanel, wxT("X-location (%)"), this, &TextPopup::OnXLocation );
  xLocSC_->SetToolTip( wxT("set the text x location (as a % of the window)") );
  botSizer->Add( xLocSC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  //
  yLocSC_ =
      new ExSpinCtrlD<TextPopup>( botPanel, wxT("Y-location (%)"), this, &TextPopup::OnYLocation );
  yLocSC_->SetToolTip( wxT("set the text y location (as a % of the window)") );
  botSizer->Add( yLocSC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  //
  closeBTN_ = new wxButton( this, wxID_CLOSE, wxT("Close") );
  mainSizer->Add( closeBTN_, wxSizerFlags(0).Center().Border(wxALL,2) );
}

void TextPopup::Setup( GRA_window *window, GRA_drawableText *dt )
{
  setup_ = true;
  window_ = window;
  drawableText_ = dt;
  textChars_ = window_->GetTextCharacteristics();
  
  stringTC_->SetValue( drawableText_->GetString() );
  
  fontCB_->SetValue( drawableText_->GetFont()->GetFontName() );
  
  color_->SetColor( drawableText_->GetColor() );

  double heightW = drawableText_->GetHeight();
  double height, dummy;
  window_->WorldToPercent( 0.0, heightW, dummy, height );
  heightSC_->SetValue( height );
  
  angleSC_->SetValue( static_cast<int>(drawableText_->GetAngle()) );
  
  double xlocW = drawableText_->GetX();
  double ylocW = drawableText_->GetY();
  double xloc, yloc;
  window_->WorldToPercent( xlocW, ylocW, xloc, yloc );
  xLocSC_->SetValue( xloc );
  yLocSC_->SetValue( yloc );
  
  setup_ = false;
}

void TextPopup::OnColor( GRA_color *color )
{
  if( setup_ )return;
  static_cast<GRA_colorCharacteristic*>(textChars_->Get(wxT("COLOR")))->Set( color );
  drawableText_->SetColor( color );
  ReDraw();
}

void TextPopup::OnHeight( double height )
{
  if( setup_ )return;
  static_cast<GRA_sizeCharacteristic*>(textChars_->Get(wxT("HEIGHT")))->SetAsPercent( height );
  drawableText_->SetHeight(
    static_cast<GRA_sizeCharacteristic*>(textChars_->Get(wxT("HEIGHT")))->GetAsWorld() );
  ReDraw();
}

void TextPopup::OnAngle( int angle )
{
  if( setup_ )return;
  static_cast<GRA_angleCharacteristic*>(textChars_->Get(wxT("ANGLE")))->Set( angle );
  drawableText_->SetAngle( angle );
  ReDraw();
}

void TextPopup::OnXLocation( double loc )
{
  if( setup_ )return;
  static_cast<GRA_distanceCharacteristic*>(textChars_->Get(wxT("XLOCATION")))->SetAsPercent( loc );
  drawableText_->SetX(
    static_cast<GRA_distanceCharacteristic*>(textChars_->Get(wxT("XLOCATION")))->GetAsWorld() );
  ReDraw();
}

void TextPopup::OnYLocation( double loc )
{
  if( setup_ )return;
  static_cast<GRA_distanceCharacteristic*>(textChars_->Get(wxT("YLOCATION")))->SetAsPercent( loc );
  drawableText_->SetY(
    static_cast<GRA_distanceCharacteristic*>(textChars_->Get(wxT("YLOCATION")))->GetAsWorld() );
  ReDraw();
}

void TextPopup::OnStringEnter( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  drawableText_->SetString( stringTC_->GetValue() );
  ReDraw();
}

void TextPopup::OnFont( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_fontCharacteristic*>(textChars_->Get(wxT("FONT")))->Set(
    GRA_fontControl::GetFont(fontCB_->GetValue()));
  drawableText_->SetFont( static_cast<GRA_fontCharacteristic*>(textChars_->Get(wxT("FONT")))->Get() );
  ReDraw();
}

void TextPopup::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/TextPopup/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/TextPopup/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/TextPopup/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/TextPopup/HEIGHT"), static_cast<long>(height) );
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
  Destroy();
  ExGlobals::ZeroTextPopup();
}

void TextPopup::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void TextPopup::ReDraw()
{
  if( setup_ )return;
  try
  {
    drawableText_->Parse();
    page_->SetGraphWindow( window_ );
    page_->ReplotCurrentWindow( true );
  }
  catch ( EGraphicsError const &e )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxString(e.what(),wxConvUTF8),
                                               wxT("Error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
}

// end of file
