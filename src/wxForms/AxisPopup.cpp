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

#include "AxisPopup.h"
#include "GraphicsPage.h"
#include "GRA_window.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "ExGlobals.h"
#include "GRA_intCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "BasicColors.h"
#include "EGraphicsError.h"

#include <wx/persist/toplevel.h>

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( AxisPopup, wxFrame )
  EVT_CHECKBOX( ID_drawAxis, AxisPopup::OnDrawAxis )
  EVT_CHECKBOX( ID_drawGraphBox, AxisPopup::OnDrawGraphBox )
  EVT_RADIOBOX( ID_gridLines, AxisPopup::OnGridLines )
  EVT_RADIOBOX( ID_logBase, AxisPopup::OnLogBase )
  EVT_CHECKBOX( ID_logStyle, AxisPopup::OnLogStyle )
  EVT_CHECKBOX( ID_drawTics, AxisPopup::OnDrawTics )
  EVT_RADIOBOX( ID_ticDirection, AxisPopup::OnTicDirection )
  EVT_CHECKBOX( ID_drawNumbers, AxisPopup::OnDrawNumbers )
  EVT_CHECKBOX( ID_drawLabel, AxisPopup::OnDrawLabel )
  EVT_COMBOBOX( ID_numbersFont, AxisPopup::OnNumbersFont )
  EVT_TEXT( ID_label, AxisPopup::OnLabel )
  EVT_TEXT_ENTER( ID_label2, AxisPopup::OnLabelEnter )
  EVT_COMBOBOX( ID_labelFont, AxisPopup::OnLabelFont )
  EVT_BUTTON( ID_applyScales, AxisPopup::OnApplyScales )
  EVT_BUTTON( wxID_CLOSE, AxisPopup::OnClose )
  EVT_CLOSE( AxisPopup::CloseEventHandler )
END_EVENT_TABLE()

AxisPopup::AxisPopup( GraphicsPage *parent )
    : wxFrame(parent,wxID_ANY,wxT("Axis Popup"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      page_(parent)
{
  setup_ = true;
  CreateForm();
  //
  wxConfigBase *config = wxConfigBase::Get();
  int page = config->Read( wxT("/AxisPopup/PAGE"), 0l );
#if wxMINOR_VERSION < 8
  notebook_->SetSelection( std::max(0,std::min(4,page)) );
#else
  notebook_->ChangeSelection( std::max(0,std::min(4,page)) );
#endif

  wxPersistentRegisterAndRestore(this, "AxisPopup");

  Layout();

  Show( true );
}

void AxisPopup::CreateForm()
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( mainSizer );

  notebook_ = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP );
  mainSizer->Add( notebook_, wxSizerFlags(0).Left().Border(wxALL,5) );
  //
  MakeAxisPanel();
  MakeNumbersPanel();
  MakeLabelPanel();
  MakeLocationPanel();
  MakeAxisScalesPanel();
  //
  closeBTN_ = new wxButton( this, wxID_CLOSE, wxT("Close") );
  mainSizer->Add( closeBTN_, wxSizerFlags(0).Center().Border(wxALL,2) );
}

void AxisPopup::MakeAxisPanel()
{
  wxPanel *panel = new wxPanel( notebook_ );
  notebook_->AddPage( panel, wxT("Axis"), true );
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  panel->SetSizer( sizer );

  wxPanel *topPanel = new wxPanel( panel );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );
  topPanel->SetSizer( topSizer );
  sizer->Add( topPanel, wxSizerFlags(0).Left().Border(wxALL,2) );

  wxPanel *topLeftPanel = new wxPanel( topPanel );
  wxBoxSizer *topLeftSizer = new wxBoxSizer( wxVERTICAL );
  topLeftPanel->SetSizer( topLeftSizer );
  topSizer->Add( topLeftPanel, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  drawAxisCB_ = new wxCheckBox( topLeftPanel, ID_drawAxis, wxT("Draw axis") );
  drawAxisCB_->SetToolTip( wxT("toggle display of axis") );
  topLeftSizer->Add( drawAxisCB_, wxSizerFlags(0).Left().Border(wxALL,2) );

  drawGraphBoxCB_ = new wxCheckBox( topLeftPanel, ID_drawGraphBox, wxT("Draw graph box") );
  drawGraphBoxCB_->SetToolTip( wxT("toggle display of the graph box") );
  topLeftSizer->Add( drawGraphBoxCB_, wxSizerFlags(0).Left().Border(wxALL,2) );

  wxPanel *tlBotPanel = new wxPanel( topLeftPanel );
  wxBoxSizer *tlbSizer = new wxBoxSizer( wxHORIZONTAL );
  tlBotPanel->SetSizer( tlbSizer );
  topLeftSizer->Add( tlBotPanel, wxSizerFlags(0).Left().Border(wxALL,2) );

  axisColor_ =
    new ExColorCtrl<AxisPopup>( tlBotPanel, wxT("Axis color"), this, &AxisPopup::OnAxisColor );
  axisColor_->SetToolTip( wxT("change the axis color") );
  tlbSizer->Add( axisColor_, wxSizerFlags(0).Left().Border(wxALL,2) );

  lineWidthSC_ =
    new ExSpinCtrlI<AxisPopup>( tlBotPanel, wxT("Line width"), 1, 10, this, &AxisPopup::OnLineWidth );
  lineWidthSC_->SetToolTip( wxT("set the line width for the axis") );
  tlbSizer->Add( lineWidthSC_, wxSizerFlags(0).Left().Border(wxALL,2) );

  wxPanel *topRightPanel = new wxPanel( topPanel );
  wxBoxSizer *topRightSizer = new wxBoxSizer( wxVERTICAL );
  topRightPanel->SetSizer( topRightSizer );
  topSizer->Add( topRightPanel, wxSizerFlags(0).Right().Border(wxALL,2) );
  
  wxString choices[3];
  choices[0] = wxT("None");
  choices[1] = wxT("At major tics");
  choices[2] = wxT("At all tics");
  gridLinesRB_ =
    new wxRadioBox( topRightPanel,ID_gridLines,wxT("Grid lines"),wxDefaultPosition,wxDefaultSize,3,choices );
  gridLinesRB_->SetToolTip( wxT("draw axis grid lines") );
  topRightSizer->Add( gridLinesRB_, wxSizerFlags(0).Center().Border(wxALL,2) );

  lineTypeSC_ =
    new ExSpinCtrlI<AxisPopup>( topRightPanel, wxT("Grid line type"), 1, 10, this, &AxisPopup::OnLineType );
  lineTypeSC_->SetToolTip( wxT("set the line type for the grid lines") );
  topRightSizer->Add( lineTypeSC_, wxSizerFlags(0).Center().Border(wxALL,2) );

  drawTicsCB_ = new wxCheckBox( panel, ID_drawTics, wxT("Draw tics") );
  drawTicsCB_->SetToolTip( wxT("toggle display of tic marks") );
  sizer->Add( drawTicsCB_, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  wxPanel *midPanel = new wxPanel( panel );
  wxBoxSizer *midSizer = new wxBoxSizer( wxHORIZONTAL );
  midPanel->SetSizer( midSizer );
  sizer->Add( midPanel, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  choices[0] = wxT("Inside");
  choices[1] = wxT("Outside");
  choices[2] = wxT("Both");
  ticDirectionRB_ = new wxRadioBox( midPanel, ID_ticDirection, wxT("Tics"), wxDefaultPosition, wxDefaultSize, 3, choices );
  ticDirectionRB_->SetToolTip( wxT("choose tic mark direction") );
  midSizer->Add( ticDirectionRB_, wxSizerFlags(0).Left().Border(wxALL,5) );

  minorIncsSC_ =
    new ExSpinCtrlI<AxisPopup>( midPanel, wxT("# minor incs"), 0, 50, this, &AxisPopup::OnMinorIncs );
  minorIncsSC_->SetToolTip( wxT("set the number of minor divisions (between major tics)") );
  midSizer->Add( minorIncsSC_, wxSizerFlags(0).Right().Border(wxALL,5) );
  
  wxPanel *botPanel = new wxPanel( panel );
  wxBoxSizer *botSizer = new wxBoxSizer( wxHORIZONTAL );
  botPanel->SetSizer( botSizer );
  sizer->Add( botPanel, wxSizerFlags(0).Center().Border(wxALL,2) );

  majorTicLenSC_ =
    new ExSpinCtrlD<AxisPopup>( botPanel, wxT("Major tic length (%)"), this, &AxisPopup::OnMajorTicLen );
  majorTicLenSC_->SetToolTip( wxT("set the length of the major tics (as a % of the window)") );
  botSizer->Add( majorTicLenSC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  minorTicLenSC_ =
    new ExSpinCtrlD<AxisPopup>( botPanel, wxT("Minor tic length (%)"), this, &AxisPopup::OnMinorTicLen );
  majorTicLenSC_->SetToolTip( wxT("set the length of the minor tics (as a % of the window)") );
  botSizer->Add( minorTicLenSC_, wxSizerFlags(0).Left().Border(wxALL,2) );
}

void AxisPopup::MakeNumbersPanel()
{
  wxPanel *panel = new wxPanel( notebook_ );
  notebook_->AddPage( panel, wxT("Numbering"), true );
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  panel->SetSizer( sizer );
  
  drawNumbersCB_ = new wxCheckBox( panel, ID_drawNumbers, wxT("Draw axis values") );
  drawNumbersCB_->SetToolTip( wxT("toggle display of the axis numbering") );
  sizer->Add( drawNumbersCB_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  sizer->Add( new wxStaticText(panel,wxID_ANY,wxT("Font ")), wxSizerFlags(0).Left().Border(wxTOP,5) );
  int nf = GRA_fontControl::GetCount();
  wxString choices[nf];
  for( int i=0; i<nf; ++i )choices[i] = GRA_fontControl::GetFont(i)->GetFontName();
  nFontCB_ = new wxComboBox( panel, ID_numbersFont, wxT(""), wxDefaultPosition, wxSize(200,25), nf, choices, wxCB_READONLY );
  nFontCB_->SetToolTip( wxT("choose the axis numbers font") );
  sizer->Add( nFontCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *botPanel = new wxPanel( panel );
  wxBoxSizer *botSizer = new wxBoxSizer( wxHORIZONTAL );
  botPanel->SetSizer( botSizer );
  sizer->Add( botPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  numbersColor_ =
    new ExColorCtrl<AxisPopup>( botPanel, wxT("Color"), this, &AxisPopup::OnNumbersColor );
  numbersColor_->SetToolTip( wxT("click to set the color for the axis numbering") );
  botSizer->Add( numbersColor_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  numbersHeightSC_ =
    new ExSpinCtrlD<AxisPopup>( botPanel, wxT("Height (%)"), this, &AxisPopup::OnNumbersHeight );
  numbersHeightSC_->SetToolTip( wxT("set the height for the axis numbering (as a % of the window)") );
  botSizer->Add( numbersHeightSC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  numbersAngleSC_ =
    new ExSpinCtrlI<AxisPopup>( botPanel, wxT("Angle (degrees)"), 0, 360, this, &AxisPopup::OnNumbersAngle );
  numbersAngleSC_->SetToolTip( wxT("set the angle for the axis numbering (0 to 360 degrees)") );
  botSizer->Add( numbersAngleSC_, wxSizerFlags(0).Left().Border(wxALL,5) );

  logStyleCB_ = new wxCheckBox( panel, ID_logStyle, wxT("Exponential style numbers") );
  logStyleCB_->SetToolTip( wxT("toggle decimal or exponential style numbers (logarithmic axis only)") );
  sizer->Add( logStyleCB_, wxSizerFlags(0).Center().Border(wxALL,5) );
}

void AxisPopup::MakeLabelPanel()
{
  wxPanel *panel = new wxPanel( notebook_ );
  notebook_->AddPage( panel, wxT("Label"), true );
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  panel->SetSizer( sizer );
  
  wxPanel *topPanel = new wxPanel( panel );
  wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
  topPanel->SetSizer( topSizer );
  sizer->Add( topPanel, wxSizerFlags(0).Left().Border(wxALL,5) );

  drawLabelCB_ = new wxCheckBox( topPanel, ID_drawLabel, wxT("Draw label") );
  drawLabelCB_->SetToolTip( wxT("toggle display of the axis label") );
  topSizer->Add( drawLabelCB_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  topSizer->Add( new wxStaticText(topPanel,wxID_ANY,wxT("Axis label")), wxSizerFlags(0).Left().Border(wxTOP,5) );
  labelTC_ = new wxTextCtrl( topPanel, ID_label, wxT(""), wxDefaultPosition, wxSize(200,25), wxTE_LEFT|wxTE_PROCESS_ENTER );
  labelTC_->SetToolTip( wxT("enter the axis label (hit Enter key to accept label)") );
  topSizer->Add( labelTC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  topSizer->Add( new wxStaticText(topPanel,wxID_ANY,wxT("Font ")), wxSizerFlags(0).Left().Border(wxTOP,5) );
  int nf = GRA_fontControl::GetCount();
  wxString choices[nf];
  for( int i=0; i<nf; ++i )choices[i] = GRA_fontControl::GetFont(i)->GetFontName();
  lFontCB_ = new wxComboBox( topPanel, ID_labelFont, wxT(""), wxDefaultPosition, wxSize(200,25), nf, choices, wxCB_READONLY );
  lFontCB_->SetToolTip( wxT("choose the axis label font") );
  topSizer->Add( lFontCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *botPanel = new wxPanel( panel );
  wxBoxSizer *botSizer = new wxBoxSizer( wxHORIZONTAL );
  botPanel->SetSizer( botSizer );
  sizer->Add( botPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  labelColor_ =
    new ExColorCtrl<AxisPopup>( botPanel, wxT("Color"), this, &AxisPopup::OnLabelColor );
  labelColor_->SetToolTip( wxT("click to set the color for the axis label") );
  botSizer->Add( labelColor_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  labelHeightSC_ =
    new ExSpinCtrlD<AxisPopup>( botPanel, wxT("Height (%)"), this, &AxisPopup::OnLabelHeight );
  labelHeightSC_->SetToolTip( wxT("set the axis label height (as a % of the window)") );
  botSizer->Add( labelHeightSC_, wxSizerFlags(0).Left().Border(wxALL,5) );
}

void AxisPopup::MakeLocationPanel()
{
  wxPanel *panel = new wxPanel( notebook_ );
  notebook_->AddPage( panel, wxT("Location"), true );
  wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
  panel->SetSizer( sizer );
  
  lowAxisSC_ =
    new ExSpinCtrlD<AxisPopup>( panel, wxT("Lower axis (%)"), this, &AxisPopup::OnLowAxis );
  lowAxisSC_->SetToolTip( wxT("set the location of the lower axis (as a % of the window)") );
  sizer->Add( lowAxisSC_, wxSizerFlags(0).Left().Border(wxALL,10) );
  
  upAxisSC_ =
    new ExSpinCtrlD<AxisPopup>( panel, wxT("Upper axis (%)"), this, &AxisPopup::OnUpAxis );
  upAxisSC_->SetToolTip( wxT("set the location of the upper axis (as a % of the window)") );
  sizer->Add( upAxisSC_, wxSizerFlags(0).Left().Border(wxALL,10) );
}

void AxisPopup::MakeAxisScalesPanel()
{
  wxPanel *panel = new wxPanel( notebook_ );
  notebook_->AddPage( panel, wxT("Scales"), true );
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  panel->SetSizer( sizer );
  
  wxPanel *topPanel = new wxPanel( panel );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );
  topPanel->SetSizer( topSizer );
  sizer->Add( topPanel, wxSizerFlags(0).Left().Border(wxALL,2) );

  topSizer->Add( new wxStaticText(topPanel,wxID_ANY,wxT("Min")), wxSizerFlags(0).Left().Border(wxALL,2) );
  minTC_ = new wxTextCtrl( topPanel, wxID_ANY );
  minTC_->SetToolTip( wxT("set the axis minimum value") );
  topSizer->Add( minTC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  topSizer->Add( new wxStaticText(topPanel,wxID_ANY,wxT("Max")), wxSizerFlags(0).Left().Border(wxALL,2) );
  maxTC_ = new wxTextCtrl( topPanel, wxID_ANY );
  maxTC_->SetToolTip( wxT("set the axis maximum value") );
  topSizer->Add( maxTC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  wxString choices[4];
  choices[0] = wxT("Linear axis");
  choices[1] = wxT("Log base 2");
  choices[2] = wxT("Log base e");
  choices[3] = wxT("Log base 10");
  logBaseRB_ = new wxRadioBox( panel, ID_logBase, wxT(""), wxDefaultPosition, wxDefaultSize, 4, choices );
  logBaseRB_->SetToolTip( wxT("choose linear or logarithmic axis") );
  sizer->Add( logBaseRB_, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  applyScalesBTN_ = new wxButton( panel, ID_applyScales, wxT("Apply scales") );
  sizer->Add( applyScalesBTN_, wxSizerFlags(0).Center().Border(wxALL,10) );
}

void AxisPopup::Setup( GRA_window *window, char const which )
{
  setup_ = true;

  window_ = window;
  which_ = which;
  axisChars_ = which_=='X' ? window_->GetXAxisCharacteristics() : window_->GetYAxisCharacteristics();
  //
  // set up the axis panel
  //
  drawAxisCB_->SetValue( static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("AXISON")))->Get() );
  drawGraphBoxCB_->SetValue(
   static_cast<GRA_boolCharacteristic*>(window_->GetGeneralCharacteristics()->Get(wxT("GRAPHBOX")))->Get() );
  axisColor_->SetColor( static_cast<GRA_colorCharacteristic*>(axisChars_->Get(wxT("AXISCOLOR")))->Get() );
  lineWidthSC_->SetValue( static_cast<GRA_intCharacteristic*>(axisChars_->Get(wxT("LINEWIDTH")))->Get() );
  
  int grid = static_cast<GRA_intCharacteristic*>(axisChars_->Get(wxT("GRID")))->Get();
  if( grid == 0 )gridLinesRB_->SetSelection(0);
  else if( grid > 0 )gridLinesRB_->SetSelection(1);
  else gridLinesRB_->SetSelection(2);
  
  lineTypeSC_->SetValue(
    static_cast<GRA_intCharacteristic*>(window_->GetGeneralCharacteristics()->Get(wxT("GRIDLINETYPE")))->Get() );
  drawTicsCB_->SetValue( static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("TICSON")))->Get() );

  double ticAngle = static_cast<GRA_angleCharacteristic*>(axisChars_->Get(wxT("TICANGLE")))->Get();
  if( static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("TICSBOTHSIDES")))->Get() )
    ticDirectionRB_->SetSelection( 2 );
  else
  {
    if( which_ == 'X' )ticDirectionRB_->SetSelection( ticAngle>=0.0&&ticAngle<=180.0 ? 0 : 1 );
    else               ticDirectionRB_->SetSelection( ticAngle>=0.0&&ticAngle<=180.0 ? 1 : 0 );
  }
  //
  majorTicLenSC_->SetValue( static_cast<GRA_sizeCharacteristic*>(axisChars_->Get(wxT("LARGETICLENGTH")))->GetAsPercent() );
  minorIncsSC_->SetValue( static_cast<GRA_intCharacteristic*>(axisChars_->Get(wxT("NSINCS")))->Get() );
  minorTicLenSC_->SetValue( static_cast<GRA_sizeCharacteristic*>(axisChars_->Get(wxT("SMALLTICLENGTH")))->GetAsPercent() );
  //
  // set up the numbering panel
  //
  drawNumbersCB_->SetValue( static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("NUMBERSON")))->Get() );
  nFontCB_->SetValue( static_cast<GRA_fontCharacteristic*>(axisChars_->Get(wxT("NUMBERSFONT")))->Get()->GetFontName() );
  numbersColor_->SetColor( static_cast<GRA_colorCharacteristic*>(axisChars_->Get(wxT("NUMBERSCOLOR")))->Get() );
  numbersHeightSC_->SetValue( static_cast<GRA_sizeCharacteristic*>(axisChars_->Get(wxT("NUMBERSHEIGHT")))->GetAsPercent() );
  numbersAngleSC_->SetValue(
   static_cast<int>(static_cast<GRA_angleCharacteristic*>(axisChars_->Get(wxT("NUMBERSANGLE")))->Get()) );
  logStyleCB_->SetValue( static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("LOGSTYLE")))->Get() );
  double base = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("LOGBASE")))->Get();
  logStyleCB_->Enable( base>1.0 );
  //
  // set up the label panel
  //
  drawLabelCB_->SetValue( static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("LABELON")))->Get() );
  labelTC_->SetValue( static_cast<GRA_stringCharacteristic*>(axisChars_->Get(wxT("LABEL")))->Get() );
  lFontCB_->SetValue( static_cast<GRA_fontCharacteristic*>(axisChars_->Get(wxT("LABELFONT")))->Get()->GetFontName() );
  labelColor_->SetColor( static_cast<GRA_colorCharacteristic*>(axisChars_->Get(wxT("LABELCOLOR")))->Get() );
  labelHeightSC_->SetValue( static_cast<GRA_sizeCharacteristic*>(axisChars_->Get(wxT("LABELHEIGHT")))->GetAsPercent() );
  //
  // set up the location panel
  //
  lowAxisSC_->SetValue( static_cast<GRA_distanceCharacteristic*>(axisChars_->Get(wxT("LOWERAXIS")))->GetAsPercent() );
  upAxisSC_->SetValue( static_cast<GRA_distanceCharacteristic*>(axisChars_->Get(wxT("UPPERAXIS")))->GetAsPercent() );
  //
  // set up the axis scales panel
  //
  minTC_->SetValue( wxString()<<static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("MIN")))->Get() );
  maxTC_->SetValue( wxString()<<static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("MAX")))->Get() );
  //
  // this will have problems if the log base is other than 2, e, or 10
  int idx = 0;
  if( fabs(base-2.0) < 0.01 )idx = 1;
  else if( fabs(base-exp(1.0)) < 0.01 )idx = 2;
  else if( fabs(base-10.0) < 0.01 )idx = 3;
  logBaseRB_->SetSelection( idx );
  
  setup_ = false;
}

void AxisPopup::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    config->Write( wxT("/AxisPopup/PAGE"), static_cast<long>(notebook_->GetSelection()) );
  }
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
  ExGlobals::ZeroAxisPopup();
}

void AxisPopup::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void AxisPopup::OnMajorTicLen( double length )
{
  if( setup_ )return;
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get(wxT("LARGETICLENGTH")))->SetAsPercent( length );
  ReDraw();
}

void AxisPopup::OnMinorTicLen( double length )
{
  if( setup_ )return;
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get(wxT("SMALLTICLENGTH")))->SetAsPercent( length );
  ReDraw();
}

void AxisPopup::OnNumbersHeight( double height )
{
  if( setup_ )return;
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get(wxT("NUMBERSHEIGHT")))->SetAsPercent( height );
  ReDraw();
}

void AxisPopup::OnLabelHeight( double height )
{
  if( setup_ )return;
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get(wxT("LABELHEIGHT")))->SetAsPercent( height );
  ReDraw();
}

void AxisPopup::OnLowAxis( double val )
{
  if( setup_ )return;
  static_cast<GRA_distanceCharacteristic*>(axisChars_->Get(wxT("LOWERAXIS")))->SetAsPercent( val );
  ReDraw();
}

void AxisPopup::OnUpAxis( double val )
{
  if( setup_ )return;
  static_cast<GRA_distanceCharacteristic*>(axisChars_->Get(wxT("UPPERAXIS")))->SetAsPercent( val );
  ReDraw();
}

void AxisPopup::OnLineWidth( int val )
{
  if( setup_ )return;
  static_cast<GRA_intCharacteristic*>(axisChars_->Get(wxT("LINEWIDTH")))->Set( val );
  ReDraw();
}

void AxisPopup::OnLineType( int val )
{
  if( setup_ )return;
  static_cast<GRA_intCharacteristic*>(window_->GetGeneralCharacteristics()->
                                      Get(wxT("GRIDLINETYPE")))->Set( val );
  ReDraw();
}

void AxisPopup::OnMinorIncs( int val )
{
  if( setup_ )return;
  static_cast<GRA_intCharacteristic*>(axisChars_->Get(wxT("NSINCS")))->Set( val );
  ReDraw();
}

void AxisPopup::OnNumbersAngle( int val )
{
  if( setup_ )return;
  static_cast<GRA_angleCharacteristic*>(axisChars_->Get(wxT("NUMBERSANGLE")))->Set( static_cast<double>(val) );
  ReDraw();
}

void AxisPopup::OnAxisColor( GRA_color *color )
{
  if( setup_ )return;
  static_cast<GRA_colorCharacteristic*>(axisChars_->Get(wxT("AXISCOLOR")))->Set( color );
  ReDraw();
}

void AxisPopup::OnNumbersColor( GRA_color *color )
{
  if( setup_ )return;
  static_cast<GRA_colorCharacteristic*>(axisChars_->Get(wxT("NUMBERSCOLOR")))->Set( color );
  ReDraw();
}

void AxisPopup::OnLabelColor( GRA_color *color )
{
  if( setup_ )return;
  static_cast<GRA_colorCharacteristic*>(axisChars_->Get(wxT("LABELCOLOR")))->Set( color );
  ReDraw();
}

void AxisPopup::OnDrawAxis( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("AXISON")))->Set( drawAxisCB_->IsChecked() );
  ReDraw();
}

void AxisPopup::OnDrawGraphBox( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(window_->GetGeneralCharacteristics()->
                                       Get(wxT("GRAPHBOX")))->Set( drawGraphBoxCB_->IsChecked() );
  ReDraw();
}

void AxisPopup::OnGridLines( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  switch ( gridLinesRB_->GetSelection() )
  {
    case 0:
      static_cast<GRA_intCharacteristic*>(axisChars_->Get(wxT("GRID")))->Set( 0 );
      break;
    case 1:
      static_cast<GRA_intCharacteristic*>(axisChars_->Get(wxT("GRID")))->Set( 1 );
      break;
    case 2:
      static_cast<GRA_intCharacteristic*>(axisChars_->Get(wxT("GRID")))->Set( -1 );
      break;
  }
  ReDraw();
}

void AxisPopup::OnDrawTics( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("TICSON")))->Set( drawTicsCB_->IsChecked() );
  ReDraw();
}

void AxisPopup::OnTicDirection( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  switch ( ticDirectionRB_->GetSelection() )
  {
    case 0:
      if( which_ == 'X' )static_cast<GRA_angleCharacteristic*>(axisChars_->Get(wxT("TICANGLE")))->Set(  90.0 );
      else               static_cast<GRA_angleCharacteristic*>(axisChars_->Get(wxT("TICANGLE")))->Set( 270.0 );
      static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("TICSBOTHSIDES")))->Set( false );
      break;
    case 1:
      if( which_ == 'X' )static_cast<GRA_angleCharacteristic*>(axisChars_->Get(wxT("TICANGLE")))->Set( 270.0 );
      else               static_cast<GRA_angleCharacteristic*>(axisChars_->Get(wxT("TICANGLE")))->Set(  90.0 );
      static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("TICSBOTHSIDES")))->Set( false );
      break;
    case 2:
      static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("TICSBOTHSIDES")))->Set( true );
      break;
  }
  ReDraw();
}

void AxisPopup::OnDrawNumbers( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("NUMBERSON")))->Set( drawNumbersCB_->IsChecked() );
  ReDraw();
}

void AxisPopup::OnDrawLabel( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("LABELON")))->Set( drawLabelCB_->IsChecked() );
  ReDraw();
}

void AxisPopup::OnNumbersFont( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_fontCharacteristic*>(axisChars_->Get(wxT("NUMBERSFONT")))->
      Set(GRA_fontControl::GetFont(nFontCB_->GetValue()));
  ReDraw();
}

void AxisPopup::OnLabel( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_stringCharacteristic*>(axisChars_->Get(wxT("LABEL")))->Set( labelTC_->GetValue() );
}

void AxisPopup::OnLabelEnter( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_stringCharacteristic*>(axisChars_->Get(wxT("LABEL")))->Set( labelTC_->GetValue() );
  if( drawLabelCB_->IsChecked() )ReDraw();
}

void AxisPopup::OnLabelFont( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_fontCharacteristic*>(axisChars_->Get(wxT("LABELFONT")))->
      Set(GRA_fontControl::GetFont(lFontCB_->GetValue()));
  if( drawLabelCB_->IsChecked() )ReDraw();
}

void AxisPopup::OnApplyScales( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  double min;
  if( !minTC_->GetValue().ToDouble(&min) )
  {
    wxMessageBox( wxT("invalid value entered for axis minimum"),
                  wxT("Error"), wxOK|wxICON_ERROR, this );
    return;
  }
  double max;
  if( !maxTC_->GetValue().ToDouble(&max) )
  {
    wxMessageBox( wxT("invalid value entered for axis maximum"),
                  wxT("Error"), wxOK|wxICON_ERROR, this );
    return;
  }
  GRA_setOfCharacteristics *general = window_->GetGeneralCharacteristics();
  GRA_stringCharacteristic *autoscale = static_cast<GRA_stringCharacteristic*>(general->Get(wxT("AUTOSCALE")));
  GRA_doubleCharacteristic *axisMin = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("MIN")));
  GRA_doubleCharacteristic *axisMax = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("MAX")));
  GRA_doubleCharacteristic *axisVMin = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("VIRTUALMIN")));
  GRA_doubleCharacteristic *axisVMax = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("VIRTUALMAX")));
  GRA_intCharacteristic *axisNlincs = static_cast<GRA_intCharacteristic*>(axisChars_->Get(wxT("NLINCS")));
  autoscale->Set(wxT("OFF"));
  axisMin->Set( min );
  axisMax->Set( max );
  axisVMin->Set( min );
  axisVMax->Set( max );
  axisNlincs->Set( 0 );
  ReDraw();
}

void AxisPopup::OnLogStyle( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(axisChars_->Get(wxT("LOGSTYLE")))->Set( logStyleCB_->IsChecked() );
  ReDraw();
}

void AxisPopup::OnLogBase( wxCommandEvent &WXUNUSED(event) )
{
  if( setup_ )return;
  double currentBase = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("LOGBASE")))->Get();
  double currentMin = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("MIN")))->Get();
  double currentMax = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("MAX")))->Get();
  double axisMin = currentMin;
  double axisMax = currentMax;
  if( currentBase > 1.0 )
  {
    axisMin = pow(currentBase,currentMin);
    axisMax = pow(currentBase,currentMax);
  }
  int idx = logBaseRB_->GetSelection();
  if( (idx>0) && (axisMin<0.0 || axisMax<0.0) )
  {
    wxMessageBox( wxT("logarithmic axis not possible with negative values"),
                  wxT("Error"), wxOK|wxICON_ERROR, this );
    logBaseRB_->SetSelection( 0 );
    return;
  }
  switch (idx)
  {
    case 0:  // linear axis
      static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("LOGBASE")))->Set( 1.0 );
      minTC_->SetValue( wxString()<<axisMin );
      maxTC_->SetValue( wxString()<<axisMax );
      logStyleCB_->Enable( false );
      break;
    case 1:  // logarithmic base 2
      static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("LOGBASE")))->Set( 2.0 );
      minTC_->SetValue( axisMin==0.0 ? wxString()<<(-1) : wxString()<<log(axisMin)/log(2.0)  );
      maxTC_->SetValue( wxString()<<log(axisMax)/log(2.0) );
      logStyleCB_->Enable( true );
      break;
    case 2:  // logarithmic base e
      static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("LOGBASE")))->Set( exp(1.0) );
      minTC_->SetValue( axisMin==0.0 ? wxString()<<(-1) : wxString()<<log(axisMin)  );
      maxTC_->SetValue( wxString()<<log(axisMax) );
      logStyleCB_->Enable( true );
      break;
    case 3:  // logarithmic base 10
      static_cast<GRA_doubleCharacteristic*>(axisChars_->Get(wxT("LOGBASE")))->Set( 10.0 );
      minTC_->SetValue( axisMin==0.0 ? wxString()<<(-1) : wxString()<<log(axisMin)/log(10.0)  );
      maxTC_->SetValue( wxString()<<log(axisMax)/log(10.0) );
      logStyleCB_->Enable( true );
      break;
  }
}

void AxisPopup::ReDraw()
{
  if( setup_ )return;
  try
  {
    page_->SetGraphWindow( window_ );
    page_->ReplotCurrentWindow( true );
  }
  catch ( EGraphicsError const &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8),
                  wxT("Error"), wxOK|wxICON_ERROR, this );
    return;
  }
}

// end of file
