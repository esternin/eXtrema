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
#include "wx/slider.h"

#include "LegendPopup.h"
#include "GraphicsPage.h"
#include "GRA_window.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "ExGlobals.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "BasicColors.h"
#include "EGraphicsError.h"
#include "GRA_legend.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( LegendPopup, wxFrame )
  EVT_COMBOBOX( ID_font, LegendPopup::OnTitleFont )
  EVT_CHECKBOX( ID_drawLegend, LegendPopup::OnDrawLegend )
  EVT_CHECKBOX( ID_drawFrame, LegendPopup::OnDrawFrame )
  EVT_CHECKBOX( ID_drawTitle, LegendPopup::OnDrawTitle )
  EVT_TEXT_ENTER( ID_title, LegendPopup::OnTitleEnter )
  EVT_BUTTON( wxID_CLOSE, LegendPopup::OnClose )
  EVT_CLOSE( LegendPopup::CloseEventHandler )
END_EVENT_TABLE()

LegendPopup::LegendPopup( GraphicsPage *parent )
    : wxFrame(parent,wxID_ANY,wxT("Legend Popup"),wxDefaultPosition,wxDefaultSize,
              wxDEFAULT_FRAME_STYLE), page_(parent)
{
  setup_ = true;
  CreateForm();
  //
  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/LegendPopup/UPPERLEFTX"), 950l );
  int uly = config->Read( wxT("/LegendPopup/UPPERLEFTY"), 150l );
  int width = config->Read( wxT("/LegendPopup/WIDTH"), 290l );
  int height = config->Read( wxT("/LegendPopup/HEIGHT"), 570l );
  SetSize( ulx, uly, width, height );
  Show( true );
  //
  Layout();
}

void LegendPopup::CreateForm()
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( mainSizer );

  drawLegendCB_ = new wxCheckBox( this, ID_drawLegend, wxT("Draw legend") );
  drawLegendCB_->SetToolTip( wxT("toggle the graph legend on/off") );
  mainSizer->Add( drawLegendCB_, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  wxPanel *framePanel = new wxPanel( this );
  wxStaticBoxSizer *frameSizer = new wxStaticBoxSizer( wxVERTICAL, framePanel, wxT("Legend frame") );
  framePanel->SetSizer( frameSizer );
  mainSizer->Add( framePanel, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  drawFrameCB_ = new wxCheckBox( framePanel, ID_drawFrame, wxT("Draw legend frame") );
  drawFrameCB_->SetToolTip( wxT("toggle the graph legend frame on/off") );
  frameSizer->Add( drawFrameCB_, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  frameColor_ = new ExColorCtrl<LegendPopup>( framePanel, wxT("Color"), this,
                                              &LegendPopup::OnFrameColor );
  frameColor_->SetToolTip( wxT("change the legend frame color") );
  frameSizer->Add( frameColor_, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  wxPanel *frameMidPanel = new wxPanel( framePanel );
  wxBoxSizer *frameMidSizer = new wxBoxSizer( wxHORIZONTAL );
  frameMidPanel->SetSizer( frameMidSizer );
  frameSizer->Add( frameMidPanel, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  frameMidSizer->Add( new wxStaticText(frameMidPanel,wxID_ANY,wxT("Move")),
                      wxSizerFlags(0).Left().Border(wxTOP,25) );

  moveH_ = new ExSlider<LegendPopup>( frameMidPanel, wxT("Horizontal"), this, &LegendPopup::OnMoveH );
  moveH_->SetToolTip( wxT("shift the legend left or right") );
  frameMidSizer->Add( moveH_, wxSizerFlags(0).Left().Border(wxALL,2) );

  moveV_ = new ExSlider<LegendPopup>( frameMidPanel, wxT("Vertical"), this, &LegendPopup::OnMoveV );
  moveV_->SetToolTip( wxT("shift the legend up or down") );
  frameMidSizer->Add( moveV_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *frameBotPanel = new wxPanel( framePanel );
  wxBoxSizer *frameBotSizer = new wxBoxSizer( wxHORIZONTAL );
  frameBotPanel->SetSizer( frameBotSizer );
  frameSizer->Add( frameBotPanel, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  frameBotSizer->Add( new wxStaticText(frameBotPanel,wxID_ANY,wxT("Expand")),
                      wxSizerFlags(0).Left().Border(wxTOP,25) );
  
  expandH_ = new ExSlider<LegendPopup>( frameBotPanel, wxT("Horizontal"), this, &LegendPopup::OnExpandH );
  expandH_->SetToolTip( wxT("expand the legend horizontally") );
  frameBotSizer->Add( expandH_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  expandV_ = new ExSlider<LegendPopup>( frameBotPanel, wxT("Vertical"), this, &LegendPopup::OnExpandV );
  expandV_->SetToolTip( wxT("expand the legend vertically") );
  frameBotSizer->Add( expandV_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *titlePanel = new wxPanel( this );
  wxStaticBoxSizer *titleSizer = new wxStaticBoxSizer( wxVERTICAL, titlePanel, wxT("Legend title") );
  titlePanel->SetSizer( titleSizer );
  mainSizer->Add( titlePanel, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  drawTitleCB_ = new wxCheckBox( titlePanel, ID_drawTitle, wxT("Draw legend title") );
  drawTitleCB_->SetToolTip( wxT("toggle the graph legend title on/off") );
  titleSizer->Add( drawTitleCB_, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  titleSizer->Add( new wxStaticText(titlePanel,wxID_ANY,wxT("Title")),
                   wxSizerFlags(0).Left().Border(wxTOP,2) );
  titleTC_ = new wxTextCtrl( titlePanel, ID_title, wxT(""), wxDefaultPosition, wxSize(250,25),
                             wxTE_LEFT|wxTE_PROCESS_ENTER );
  titleTC_->SetToolTip( wxT("enter the new title (hit Enter key to accept title)") );
  titleSizer->Add( titleTC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *titleMidPanel = new wxPanel( titlePanel );
  wxBoxSizer *titleMidSizer = new wxBoxSizer( wxHORIZONTAL );
  titleMidPanel->SetSizer( titleMidSizer );
  titleSizer->Add( titleMidPanel, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  titleColor_ = new ExColorCtrl<LegendPopup>( titleMidPanel, wxT("Color"), this,
                                              &LegendPopup::OnTitleColor );
  titleColor_->SetToolTip( wxT("change the legend title color") );
  titleMidSizer->Add( titleColor_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  titleHeightSC_ =
      new ExSpinCtrlD<LegendPopup>( titleMidPanel, wxT("Height (%)"), this,
                                    &LegendPopup::OnTitleHeight );
  titleHeightSC_->SetToolTip( wxT("set the legend title height (as a % of the window)") );
  titleMidSizer->Add( titleHeightSC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  wxPanel *titleBotPanel = new wxPanel( titlePanel );
  wxBoxSizer *titleBotSizer = new wxBoxSizer( wxVERTICAL );
  titleBotPanel->SetSizer( titleBotSizer );
  titleSizer->Add( titleBotPanel, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  titleBotSizer->Add( new wxStaticText(titleBotPanel,wxID_ANY,wxT("Font")),
                      wxSizerFlags(0).Left().Border(wxALL,2) );
  int nf = GRA_fontControl::GetCount();
  wxString choices[nf];
  for( int i=0; i<nf; ++i )choices[i] = GRA_fontControl::GetFont(i)->GetFontName();
  titleFontCB_ = new wxComboBox( titleBotPanel, ID_font, wxT(""), wxDefaultPosition, wxSize(200,25), nf,
                                 choices, wxCB_READONLY );
  titleFontCB_->SetToolTip( wxT("choose the title font") );
  titleBotSizer->Add( titleFontCB_, wxSizerFlags(0).Left().Border(wxALL,2) );

  closeBTN_ = new wxButton( this, wxID_CLOSE, wxT("Close") );
  mainSizer->Add( closeBTN_, wxSizerFlags(0).Center().Border(wxALL,2) );
}

void LegendPopup::Setup( GRA_window *window, GRA_legend *legend )
{
  setup_ = true;
  window_ = window;
  legend_ = legend;
  legend_->SetPopup();
  legChars_ = window_->GetGraphLegendCharacteristics();
  
  drawLegendCB_->SetValue( static_cast<GRA_boolCharacteristic*>(legChars_->Get(wxT("ON")))->Get() );
  
  frameColor_->SetColor( static_cast<GRA_colorCharacteristic*>(legChars_->Get(wxT("FRAMECOLOR")))->Get() );
  
  drawFrameCB_->SetValue( static_cast<GRA_boolCharacteristic*>(legChars_->Get(wxT("FRAMEON")))->Get() );
  
  double xlo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEXLO")))->GetAsPercent();
  double ylo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEYLO")))->GetAsPercent();
  double xhi = static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEXHI")))->GetAsPercent();
  double yhi = static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEYHI")))->GetAsPercent();;
  
  moveH_->SetValue( xlo );
  moveV_->SetValue( ylo );
  expandH_->SetValue( xhi-xlo );
  expandV_->SetValue( yhi-ylo );
  
  drawTitleCB_->SetValue( static_cast<GRA_boolCharacteristic*>(legChars_->Get(wxT("TITLEON")))->Get() );

  titleTC_->SetValue( static_cast<GRA_stringCharacteristic*>(legChars_->Get(wxT("TITLE")))->Get() );
  
  titleColor_->SetColor( static_cast<GRA_colorCharacteristic*>(legChars_->Get(wxT("TITLECOLOR")))->Get() );
  
  titleHeightSC_->SetValue(
    static_cast<GRA_sizeCharacteristic*>(legChars_->Get(wxT("TITLEHEIGHT")))->GetAsPercent() );
  
  titleFontCB_->SetValue(
    static_cast<GRA_fontCharacteristic*>(legChars_->Get(wxT("TITLEFONT")))->Get()->GetFontName() );
  
  setup_ = false;
}

void LegendPopup::OnFrameColor( GRA_color *color )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_colorCharacteristic*>(legChars_->Get(wxT("FRAMECOLOR")))->Set( color );
  legend_->MakeFrame();
  if( drawLegendCB_->IsChecked() && drawFrameCB_->IsChecked() )ReDraw();
}

void LegendPopup::OnTitleColor( GRA_color *color )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_colorCharacteristic*>(legChars_->Get(wxT("TITLECOLOR")))->Set( color );
  legend_->MakeTitle();
  if( drawTitleCB_->IsChecked() && drawLegendCB_->IsChecked() )ReDraw();
}

void LegendPopup::OnTitleHeight( double height )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_sizeCharacteristic*>(legChars_->Get(wxT("TITLEHEIGHT")))->SetAsPercent( height );
  legend_->MakeTitle();
  if( drawTitleCB_->IsChecked() && drawLegendCB_->IsChecked() )ReDraw();
}

void LegendPopup::OnMoveH( double x )
{
  if( setup_ || !legend_ )return;
  double xlo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEXLO")))->GetAsPercent();
  double xhi = static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEXHI")))->GetAsPercent();
  double dx = xhi - xlo;
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEXLO")))->SetAsPercent( x );
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEXHI")))->SetAsPercent( x+dx );
  legend_->Initialize();
  ReDraw();
}

void LegendPopup::OnMoveV( double y )
{
  if( setup_ || !legend_ )return;
  double ylo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEYLO")))->GetAsPercent();
  double yhi = static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEYHI")))->GetAsPercent();
  double dy = yhi - ylo;
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEYLO")))->SetAsPercent( y );
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEYHI")))->SetAsPercent( y+dy );
  legend_->Initialize();
  ReDraw();
}

void LegendPopup::OnExpandH( double dx )
{
  if( setup_ || !legend_ )return;
  double xlo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEXLO")))->GetAsPercent();
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEXHI")))->SetAsPercent( xlo+dx );
  legend_->Initialize();
  ReDraw();
}

void LegendPopup::OnExpandV( double dy )
{
  if( setup_ || !legend_ )return;
  double ylo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEYLO")))->GetAsPercent();
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get(wxT("FRAMEYHI")))->SetAsPercent( ylo+dy );
  legend_->Initialize();
  ReDraw();
}

void LegendPopup::OnTitleEnter( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_stringCharacteristic*>(legChars_->Get(wxT("TITLE")))->Set( titleTC_->GetValue() );
  legend_->MakeTitle();
  if( drawTitleCB_->IsChecked() && drawLegendCB_->IsChecked() )ReDraw();
}

void LegendPopup::OnTitleFont( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_fontCharacteristic*>(legChars_->Get(wxT("TITLEFONT")))->Set(
    GRA_fontControl::GetFont(titleFontCB_->GetValue()) );
  legend_->MakeTitle();
  if( drawTitleCB_->IsChecked() && drawLegendCB_->IsChecked() )ReDraw();
}

void LegendPopup::OnDrawLegend( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_boolCharacteristic*>(legChars_->Get(wxT("ON")))->Set( drawLegendCB_->IsChecked() );
  ReDraw();
}

void LegendPopup::OnDrawFrame( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_boolCharacteristic*>(legChars_->Get(wxT("FRAMEON")))->Set( drawFrameCB_->IsChecked() );
  ReDraw();
}

void LegendPopup::OnDrawTitle( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_boolCharacteristic*>(legChars_->Get(wxT("TITLEON")))->Set( drawTitleCB_->IsChecked() );
  ReDraw();
}

void LegendPopup::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/LegendPopup/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/LegendPopup/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/LegendPopup/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/LegendPopup/HEIGHT"), static_cast<long>(height) );
  }
  if( legend_ )legend_->Disconnect();
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
  ExGlobals::ZeroLegendPopup();
}

void LegendPopup::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void LegendPopup::Disconnect()
{ legend_ = 0; }

void LegendPopup::ReDraw()
{
  if( setup_ || !legend_ )return;
  try
  {
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
