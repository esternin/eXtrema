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
#include "wx/radiobox.h"

#include "CurvePopup.h"
#include "GraphicsPage.h"
#include "GRA_window.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "ExGlobals.h"
#include "GRA_intCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "BasicColors.h"
#include "EGraphicsError.h"
#include "GRA_cartesianCurve.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( CurvePopup, wxFrame )
  EVT_RADIOBOX( ID_curveType, CurvePopup::OnCurveType )
  EVT_CHECKBOX( ID_connect, CurvePopup::OnConnect )
  EVT_BUTTON( wxID_CLOSE, CurvePopup::OnClose )
  EVT_CLOSE( CurvePopup::CloseEventHandler )
END_EVENT_TABLE()

CurvePopup::CurvePopup( GraphicsPage *parent )
    : wxFrame(parent,wxID_ANY,wxT("Curve Popup"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      page_(parent)
{
  setup_ = true;
  CreateForm();
  //
  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/CurvePopup/UPPERLEFTX"), 950l );
  int uly = config->Read( wxT("/CurvePopup/UPPERLEFTY"), 150l );
  int width = config->Read( wxT("/CurvePopup/WIDTH"), 270l );
  int height = config->Read( wxT("/CurvePopup/HEIGHT"), 420l );
  SetSize( ulx, uly, width, height );
  Show( true );
  //
  Layout();
}

void CurvePopup::CreateForm()
{
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( sizer );

  wxString choices[3];
  choices[0] = wxT("Non-histogram");
  choices[1] = wxT("Histogram without tails");
  choices[2] = wxT("Histogram with tails");
  histogramRB_ =
    new wxRadioBox( this,ID_curveType,wxT("Curve type"),wxDefaultPosition,wxDefaultSize,3,choices );
  histogramRB_->SetToolTip( wxT("set the type of curve") );
  sizer->Add( histogramRB_, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  wxPanel *topMidPanel = new wxPanel( this );
  wxBoxSizer *topMidSizer = new wxBoxSizer( wxHORIZONTAL );
  topMidPanel->SetSizer( topMidSizer );
  
  curveColor_ =
    new ExColorCtrl<CurvePopup>( topMidPanel, wxT("Curve color"), this, &CurvePopup::OnCurveColor );
  curveColor_->SetToolTip( wxT("change the curve color") );
  topMidSizer->Add( curveColor_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  areafillColor_ =
    new ExColorCtrl<CurvePopup>( topMidPanel, wxT("Areafill color"), this, &CurvePopup::OnAreafillColor );
  areafillColor_->SetToolTip( wxT("change the areafill color") );
  topMidSizer->Add( areafillColor_, wxSizerFlags(0).Left().Border(wxALL,5) );

  sizer->Add( topMidPanel, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  wxPanel *topLowerPanel = new wxPanel( this );
  wxBoxSizer *topLowerSizer = new wxBoxSizer( wxHORIZONTAL );
  topLowerPanel->SetSizer( topLowerSizer );
  
  lineWidthSC_ =
    new ExSpinCtrlI<CurvePopup>( topLowerPanel, wxT("Line width"), 1, 10, this, &CurvePopup::OnCurveLineWidth );
  lineWidthSC_->SetToolTip( wxT("set the line width for the curve") );
  topLowerSizer->Add( lineWidthSC_, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  lineTypeSC_ =
    new ExSpinCtrlI<CurvePopup>( topLowerPanel, wxT("Line type"), 1, 10, this, &CurvePopup::OnCurveLineType );
  lineTypeSC_->SetToolTip( wxT("set the line type for the curve") );
  topLowerSizer->Add( lineTypeSC_, wxSizerFlags(0).Center().Border(wxALL,5) );

  sizer->Add( topLowerPanel, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  plotsymbolPanel_ = new wxPanel( this );
  wxStaticBoxSizer *plotsymbolSizer = new wxStaticBoxSizer( wxVERTICAL, plotsymbolPanel_, wxT("Plotsymbol") );
  plotsymbolPanel_->SetSizer( plotsymbolSizer );
  
  wxPanel *psTopPanel = new wxPanel( plotsymbolPanel_ );
  wxBoxSizer *psTopSizer = new wxBoxSizer( wxHORIZONTAL );
  psTopPanel->SetSizer( psTopSizer );
  
  plotsymbolColor_ =
    new ExColorCtrl<CurvePopup>( psTopPanel, wxT("Color"), this, &CurvePopup::OnPlotsymbolColor );
  plotsymbolColor_->SetToolTip( wxT("change the plotsymbol color") );
  psTopSizer->Add( plotsymbolColor_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  plotsymbolCodeSC_ =
    new ExSpinCtrlI<CurvePopup>( psTopPanel, wxT("Code"), 0, 18, this, &CurvePopup::OnPlotsymbolCode );
  plotsymbolCodeSC_->SetToolTip( wxT("change the plotsymbol code") );
  psTopSizer->Add( plotsymbolCodeSC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  connectCB_ = new wxCheckBox( psTopPanel, ID_connect, wxT("Connect") );
  connectCB_->SetToolTip( wxT("toggle connect plotsymbols") );
  psTopSizer->Add( connectCB_, wxSizerFlags(0).Left().Border(wxTOP,15) );
  
  plotsymbolSizer->Add( psTopPanel, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  wxPanel *psBotPanel = new wxPanel( plotsymbolPanel_ );
  wxBoxSizer *psBotSizer = new wxBoxSizer( wxHORIZONTAL );
  psBotPanel->SetSizer( psBotSizer );
  
  plotsymbolSizeSC_ =
    new ExSpinCtrlD<CurvePopup>( psBotPanel, wxT("Size (%)"), this, &CurvePopup::OnPlotsymbolSize );
  plotsymbolSizeSC_->SetToolTip( wxT("set the plotsymbol size (as a percentage of the window)") );
  psBotSizer->Add( plotsymbolSizeSC_, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  plotsymbolAngleSC_ =
    new ExSpinCtrlI<CurvePopup>( psBotPanel, wxT("Angle (degrees)"), 0, 360, this,
                                 &CurvePopup::OnPlotsymbolAngle );
  plotsymbolAngleSC_->SetToolTip( wxT("set the plotsymbol angle (from 0 to 360 degrees)") );
  psBotSizer->Add( plotsymbolAngleSC_, wxSizerFlags(0).Center().Border(wxALL,5) );

  plotsymbolSizer->Add( psBotPanel, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  sizer->Add( plotsymbolPanel_, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  closeBTN_ = new wxButton( this, wxID_CLOSE, wxT("Close") );
  sizer->Add( closeBTN_, wxSizerFlags(0).Center().Border(wxALL,2) );
}

void CurvePopup::Setup( GRA_window *window, GRA_cartesianCurve *curve )
{
  setup_ = true;

  window_ = window;
  curve_ = curve;
  curve_->SetPopup();

  curveChars_ = window_->GetDataCurveCharacteristics();
  
  // this will be a problem if histType is other than 0, 1, or 2
  int histType = curve_->GetHistogramType();
  if( histType == 3 )histType = 1;
  if( histType == 4 )histType = 2;
  histogramRB_->SetSelection( histType );
  plotsymbolPanel_->Enable( histType==0 );

  curveColor_->SetColor( curve_->GetColor() );

  areafillColor_->SetColor( curve_->GetAreaFillColor() );

  lineWidthSC_->SetValue( curve_->GetLineWidth() );
  
  lineTypeSC_->SetValue( curve_->GetLineType() );
  
  plotsymbolCodeSC_->SetValue( abs(curve_->GetPlotsymbolCode()) );
  
  connectCB_->SetValue( curve_->GetPlotsymbolCode()>=0 );
  
  plotsymbolColor_->SetColor( curve_->GetPlotsymbolColor() );
  
  double heightW = curve_->GetPlotsymbolSize();
  double height, dummy;
  window_->WorldToPercent( 0.0, heightW, dummy, height );
  plotsymbolSizeSC_->SetValue( height );
  
  plotsymbolAngleSC_->SetValue( static_cast<int>(curve_->GetPlotsymbolAngle()) );

  setup_ = false;
}

void CurvePopup::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/CurvePopup/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/CurvePopup/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/CurvePopup/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/CurvePopup/HEIGHT"), static_cast<long>(height) );
  }
  if( curve_ )curve_->Disconnect();
  //
  // close all child windows
  /*
  wxWindowList children( GetChildren() );
  wxWindowListNode *node = children.GetFirst();
  while( node )
  {
    wxWindow *window = node->GetData();
    window->Close(true);
    node = node->GetNext();
  }
  */
  Destroy();
  ExGlobals::ZeroCurvePopup();
}

void CurvePopup::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void CurvePopup::Disconnect()
{ curve_ = 0; }

void CurvePopup::OnCurveColor( GRA_color *color )
{
  if( setup_ || !curve_ )return;
  static_cast<GRA_colorCharacteristic*>(curveChars_->Get(wxT("CURVECOLOR")))->Set( color );
  curve_->SetColor( color );
  ReDraw();
}

void CurvePopup::OnAreafillColor( GRA_color *color )
{
  if( setup_ || !curve_ )return;
  if( GRA_colorControl::GetColorCode(color) == 0 )color = 0;
  static_cast<GRA_colorCharacteristic*>(window_->GetGeneralCharacteristics()->
                                        Get(wxT("AREAFILLCOLOR")))->Set( color );
  curve_->SetAreaFillColor( color );
  ReDraw();
}

void CurvePopup::OnPlotsymbolColor( GRA_color *color )
{
  if( setup_ || !curve_ )return;
  static_cast<GRA_colorCharacteristic*>(curveChars_->Get(wxT("PLOTSYMBOLCOLOR")))->Set( color );
  curve_->SetPlotsymbolColor( color );
  ReDraw();
}

void CurvePopup::OnPlotsymbolCode( int code )
{
  if( setup_ || !curve_ )return;
  if( !connectCB_->IsChecked() )code *= -1;
  static_cast<GRA_intCharacteristic*>(curveChars_->Get(wxT("PLOTSYMBOL")))->Set( code );
  curve_->SetPlotsymbolCode( code );
  ReDraw();
}

void CurvePopup::OnCurveLineWidth( int lw )
{
  if( setup_ || !curve_ )return;
  static_cast<GRA_intCharacteristic*>(curveChars_->Get(wxT("CURVELINEWIDTH")))->Set( lw );
  curve_->SetLineWidth( lw );
  ReDraw();
}

void CurvePopup::OnCurveLineType( int lt )
{
  if( setup_ || !curve_ )return;
  static_cast<GRA_intCharacteristic*>(curveChars_->Get(wxT("CURVELINETYPE")))->Set( lt );
  curve_->SetLineType( lt );
  ReDraw();
}

void CurvePopup::OnPlotsymbolSize( double size )
{
  if( setup_ || !curve_ )return;
  static_cast<GRA_sizeCharacteristic*>(curveChars_->Get(wxT("PLOTSYMBOLSIZE")))->SetAsPercent( size );
  curve_->SetPlotsymbolSize(
    static_cast<GRA_sizeCharacteristic*>(curveChars_->Get(wxT("PLOTSYMBOLSIZE")))->GetAsWorld() );
  ReDraw();
}

void CurvePopup::OnPlotsymbolAngle( int angle )
{
  if( setup_ || !curve_ )return;
  static_cast<GRA_angleCharacteristic*>(curveChars_->Get(wxT("PLOTSYMBOLANGLE")))->
      Set( static_cast<double>(angle) );
  curve_->SetPlotsymbolAngle( angle );
  ReDraw();
}

void CurvePopup::OnCurveType( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ || !curve_ )return;
  int histType = histogramRB_->GetSelection();
  static_cast<GRA_intCharacteristic*>(curveChars_->Get(wxT("HISTOGRAMTYPE")))->Set( histType );
  curve_->SetHistogramType( histType );
  plotsymbolPanel_->Enable( histType==0 );
  ReDraw();
}

void CurvePopup::OnConnect( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ || !curve_ )return;
  int code = plotsymbolCodeSC_->GetValue();
  if( !connectCB_->IsChecked() )code *= -1;
  static_cast<GRA_intCharacteristic*>(curveChars_->Get(wxT("PLOTSYMBOL")))->Set( code );
  curve_->SetPlotsymbolCode( code );
  ReDraw();
}

void CurvePopup::ReDraw()
{
  if( setup_ || !curve_ )return;
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
