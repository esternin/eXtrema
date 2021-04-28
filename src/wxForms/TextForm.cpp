/*
Copyright (C) 2005 Joseph L. Chuma, TRIUMF

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
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include "wx/config.h"
#include "wx/statline.h"
#include "wx/stattext.h"
#include "wx/filedlg.h"

#include <wx/persist/toplevel.h>

#include "TextForm.h"
#include "VisualizationWindow.h"
#include "NumericVariable.h"
#include "EVariableError.h"
#include "NVariableTable.h"
#include "ExGlobals.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "GRA_window.h"
#include "GRA_intCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_colorControl.h"
#include "GRA_setOfCharacteristics.h"
#include "EGraphicsError.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "GRA_drawableText.h"
#include "FontChooser.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( TextForm, wxFrame )
  EVT_CHECKBOX( ID_interactive, TextForm::OnInteractive )
  EVT_RADIOBUTTON( ID_ul, TextForm::OnUL )
  EVT_RADIOBUTTON( ID_uc, TextForm::OnUC )
  EVT_RADIOBUTTON( ID_ur, TextForm::OnUR )
  EVT_RADIOBUTTON( ID_cl, TextForm::OnCL )
  EVT_RADIOBUTTON( ID_cc, TextForm::OnCC )
  EVT_RADIOBUTTON( ID_cr, TextForm::OnCR )
  EVT_RADIOBUTTON( ID_ll, TextForm::OnLL )
  EVT_RADIOBUTTON( ID_lc, TextForm::OnLC )
  EVT_RADIOBUTTON( ID_lr, TextForm::OnLR )
  EVT_BUTTON( ID_font, TextForm::OnFont )
  EVT_BUTTON( ID_erase, TextForm::OnErase )
  EVT_BUTTON( wxID_APPLY, TextForm::OnDraw )
  EVT_BUTTON( wxID_CLOSE, TextForm::OnClose )
  EVT_CLOSE( TextForm::CloseEventHandler )
END_EVENT_TABLE()

int TextForm::gwToRadio_[] = { 6,7,8, 3,4,5, 0,1,2 };

TextForm::TextForm( VisualizationWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Text Form"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      visualizationWindow_(parent)
{
  CreateForm();

  FillOutForm();

  wxConfigBase *config = wxConfigBase::Get();
  wxString sTmp;
  config->Read( wxT("/DrawTextForm/STRING"), &sTmp, wxT("") );
  stringTC_->SetValue( sTmp );
  sTmp.clear();
  config->Read( wxT("/DrawTextForm/XLOC"), &sTmp, wxT("") );
  locxTC_->SetValue( sTmp );
  sTmp.clear();
  config->Read( wxT("/DrawTextForm/YLOC"), &sTmp, wxT("") );
  locyTC_->SetValue( sTmp );
  bool bTmp = true;
  config->Read( wxT("/DrawTextForm/INTERACTIVE"), &bTmp, true );
  interactiveCkB_->SetValue( bTmp );
  SetupInteractive();
  int i = config->Read( wxT("/DrawTextForm/ALIGNMENT"), 1l );
  alignmentRB_[gwToRadio_[i-1]]->SetValue( true );

  Layout();

  wxPersistentRegisterAndRestore(this, "DrawTextForm");
  Show( true );

  fromSet_ = false;
}

void TextForm::CreateForm()
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->AddSpacer( 10 );
  
  wxPanel *topPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );

  topSizer->Add( new wxStaticText(topPanel,wxID_ANY,wxT("Text string to draw: ")), wxSizerFlags(0).Right().Border(wxTOP,5) );
  stringTC_ = new wxTextCtrl( topPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(300,25) );
  topSizer->Add( stringTC_, wxSizerFlags(1).Left().Border(wxALL,2) );
  
  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(0).Center().Border(wxALL,2) );

  wxPanel *nextPanel = new wxPanel( this, wxID_ANY );
  wxBoxSizer *nextSizer = new wxBoxSizer( wxHORIZONTAL);

  interactiveCkB_ = new wxCheckBox( nextPanel, ID_interactive, wxT("Interactively position string   ") );
  nextSizer->Add( interactiveCkB_, wxSizerFlags(0).Left().Border(wxTOP,25) );

  wxPanel *locPanel = new wxPanel( nextPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *locSizer = new wxBoxSizer( wxHORIZONTAL );
  
  wxPanel *locxPanel = new wxPanel( locPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *locxSizer = new wxBoxSizer( wxVERTICAL );
  locxST_ = new wxStaticText( locxPanel, wxID_ANY, wxT("%x location") );
  locxSizer->Add( locxST_, wxSizerFlags(0).Center().Border(wxALL,1) );
  locxTC_ = new wxTextCtrl( locxPanel, wxID_ANY );
  locxSizer->Add( locxTC_, wxSizerFlags(0).Center().Border(wxALL,1) );
  locxPanel->SetSizer( locxSizer );
  locSizer->Add( locxPanel, wxSizerFlags(0).Left().Border(wxALL,1) );
  
  wxPanel *locyPanel = new wxPanel( locPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *locySizer = new wxBoxSizer( wxVERTICAL );
  locyST_ = new wxStaticText( locyPanel, wxID_ANY, wxT("%y location") );
  locySizer->Add( locyST_, wxSizerFlags(0).Center().Border(wxALL,1) );
  locyTC_ = new wxTextCtrl( locyPanel, wxID_ANY );
  locySizer->Add( locyTC_, wxSizerFlags(0).Center().Border(wxALL,1) );
  locyPanel->SetSizer( locySizer );
  locSizer->Add( locyPanel, wxSizerFlags(0).Left().Border(wxALL,1) );
  
  locPanel->SetSizer( locSizer );
  nextSizer->Add( locPanel, wxSizerFlags(0).Left().Border(wxALL,1) );

  nextPanel->SetSizer( nextSizer );
  mainSizer->Add( nextPanel, wxSizerFlags(0).Center().Border(wxALL,2) );

  wxPanel *alignPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
  wxGridSizer *alignSizer = new wxGridSizer( 4, 3, 2, 2 );

  alignSizer->Add( new wxStaticText(alignPanel,wxID_ANY,wxT("Alignment")), wxSizerFlags(0).Left().Border(wxALL,5) );
  alignSizer->Add( new wxStaticText(alignPanel,wxID_ANY,wxT("")), wxSizerFlags(0).Left().Border(wxALL,5) );
  alignSizer->Add( new wxStaticText(alignPanel,wxID_ANY,wxT("")), wxSizerFlags(0).Left().Border(wxALL,5) );
  alignmentRB_[0] = new wxRadioButton( alignPanel, ID_ul, wxT("upper left"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
  alignSizer->Add( alignmentRB_[0], wxSizerFlags(0).Left().Border(wxALL,5) );
  alignmentRB_[1] = new wxRadioButton( alignPanel, ID_uc, wxT("upper centre") );
  alignSizer->Add( alignmentRB_[1], wxSizerFlags(0).Left().Border(wxALL,5) );
  alignmentRB_[2] = new wxRadioButton( alignPanel, ID_ur, wxT("upper right") );
  alignSizer->Add( alignmentRB_[2], wxSizerFlags(0).Left().Border(wxALL,5) );
  alignmentRB_[3] = new wxRadioButton( alignPanel, ID_cl, wxT("centre left") );
  alignSizer->Add( alignmentRB_[3], wxSizerFlags(0).Left().Border(wxALL,5) );
  alignmentRB_[4] = new wxRadioButton( alignPanel, ID_cc, wxT("centre centre") );
  alignSizer->Add( alignmentRB_[4], wxSizerFlags(0).Left().Border(wxALL,5) );
  alignmentRB_[5] = new wxRadioButton( alignPanel, ID_cr, wxT("centre right") );
  alignSizer->Add( alignmentRB_[5], wxSizerFlags(0).Left().Border(wxALL,5) );
  alignmentRB_[6] = new wxRadioButton( alignPanel, ID_ll, wxT("lower left") );
  alignSizer->Add( alignmentRB_[6], wxSizerFlags(0).Left().Border(wxALL,5) );
  alignmentRB_[7] = new wxRadioButton( alignPanel, ID_lc, wxT("lower centre") );
  alignSizer->Add( alignmentRB_[7], wxSizerFlags(0).Left().Border(wxALL,5) );
  alignmentRB_[8] = new wxRadioButton( alignPanel, ID_lr, wxT("lower right") );
  alignSizer->Add( alignmentRB_[8], wxSizerFlags(0).Left().Border(wxALL,5) );

  alignPanel->SetSizer( alignSizer );
  mainSizer->Add( alignPanel, wxSizerFlags(0).Center().Border(wxALL,2) );

  wxButton *fontButton = new wxButton( this, ID_font, wxT("Font") );
  fontButton->SetToolTip( wxT("click to change the font") );
  mainSizer->Add( fontButton, wxSizerFlags(0).Center().Border(wxALL,10) );
  
  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );
  
  wxButton *eraseButton = new wxButton( bottomPanel, ID_erase, wxT("Erase") );
  eraseButton->SetToolTip( wxT("erase the last drawn string") );
  bottomSizer->Add( eraseButton, wxSizerFlags(0).Border(wxALL,10) );
  
  wxButton *drawButton = new wxButton( bottomPanel, wxID_APPLY, wxT("Draw") );
  drawButton->SetToolTip( wxT("draw the string") );
  bottomSizer->Add( drawButton, wxSizerFlags(0).Border(wxALL,10) );
  
  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );
  
  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );
  
  SetSizer( mainSizer );
}

void TextForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    config->Write( wxT("/DrawTextForm/STRING"), stringTC_->GetValue() );
    config->Write( wxT("/DrawTextForm/XLOC"), locxTC_->GetValue() );
    config->Write( wxT("/DrawTextForm/YLOC"), locyTC_->GetValue() );
    config->Write( wxT("/DrawTextForm/INTERACTIVE"), interactiveCkB_->IsChecked() );
    for( int i=0; i<9; ++i )
    {
      if( alignmentRB_[i]->GetValue() )
      {
        config->Write( wxT("/DrawTextForm/ALIGNMENT"), static_cast<long>(gwToRadio_[i]+1) );
        break;
      }
    }
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
  visualizationWindow_->ZeroTextForm();

  Destroy();
}

void TextForm::FillOutForm()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *textC = gw->GetTextCharacteristics();
  //
  locxTC_->SetValue( wxString()<<static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->GetAsPercent() );
  locyTC_->SetValue( wxString()<<static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->GetAsPercent() );
  //
  alignmentRB_[gwToRadio_[static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Get()-1]]->SetValue( true );
  //
  interactiveCkB_->SetValue( static_cast<GRA_boolCharacteristic*>(textC->Get(wxT("INTERACTIVE")))->Get() );
}

void TextForm::SetupInteractive()
{
  if( interactiveCkB_->IsChecked() )
  {
    locxST_->Enable( false );
    locxTC_->Enable( false );
    locyST_->Enable( false );
    locyTC_->Enable( false );
  }
  else
  {
    locxST_->Enable( true );
    locxTC_->Enable( true );
    locyST_->Enable( true );
    locyTC_->Enable( true );
  }
}

void TextForm::OnDraw( wxCommandEvent &event )
{
  Draw();
}

void TextForm::Draw()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *textC = gw->GetTextCharacteristics();
  //
  int alignment;
  for( int i=0; i<9; ++i )
  {
    if( alignmentRB_[i]->GetValue() )
    {
      alignment = i;
      static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( gwToRadio_[i]+1 );
      break;
    }
  }
  bool interactive = interactiveCkB_->IsChecked();
  if( !interactive )
  {
    double xloc, yloc;
    if( !locxTC_->GetValue().ToDouble(&xloc) )
    {
      wxMessageDialog *md =
          new wxMessageDialog( this, wxT("invalid value entered for %x location"),
                               wxT("Fatal error"), wxOK|wxICON_ERROR );
      md->ShowModal();
      return;
    }
    if( !locyTC_->GetValue().ToDouble(&yloc) )
    {
      wxMessageDialog *md =
          new wxMessageDialog( this, wxT("invalid value entered for %y location"),
                               wxT("Fatal error"), wxOK|wxICON_ERROR );
      md->ShowModal();
      return;
    }
    static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsPercent( xloc );
    static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( yloc );
  }
  if( !fromSet_ || !interactive )
  {
    GRA_drawableText *dt = new GRA_drawableText( stringTC_->GetValue() );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      delete dt;
      wxMessageDialog *md =
          new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR );
      md->ShowModal();
      return;
    }
    if( interactive )visualizationWindow_->SetInteractiveText( dt );
    else
    {
      gw->AddDrawableObject( dt );
      dt->SetX( static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->GetAsWorld() );
      dt->SetY( static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->GetAsWorld() );
      wxWindow *wx = ExGlobals::GetwxWindow();
      wxClientDC dc( wx );
      wx->PrepareDC( dc );
      dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
      ExGlobals::RefreshGraphics();
    }
  }
  if( ExGlobals::StackIsOn() )
  {
    wxString line( wxT("FORM TEXT,'") );
    int psc = interactive ? 1 : 0;
    line += stringTC_->GetValue() + wxT("',") + (wxString()<<psc) + wxT(",") + locxTC_->GetValue() +
        wxT(",") + locyTC_->GetValue() + wxT(",") + (wxString()<<alignment) + wxT(",DRAW");
    ExGlobals::WriteStack( line );
  }
}

void TextForm::OnUL( wxCommandEvent &WXUNUSED(event) )
{ alignmentRB_[0]->SetValue( true ); }

void TextForm::OnUC( wxCommandEvent &WXUNUSED(event) )
{ alignmentRB_[1]->SetValue( true ); }

void TextForm::OnUR( wxCommandEvent &WXUNUSED(event) )
{ alignmentRB_[2]->SetValue( true ); }

void TextForm::OnCL( wxCommandEvent &WXUNUSED(event) )
{ alignmentRB_[3]->SetValue( true ); }

void TextForm::OnCC( wxCommandEvent &WXUNUSED(event) )
{ alignmentRB_[4]->SetValue( true ); }

void TextForm::OnCR( wxCommandEvent &WXUNUSED(event) )
{ alignmentRB_[5]->SetValue( true ); }

void TextForm::OnLL( wxCommandEvent &WXUNUSED(event) )
{ alignmentRB_[6]->SetValue( true ); }

void TextForm::OnLC( wxCommandEvent &WXUNUSED(event) )
{ alignmentRB_[7]->SetValue( true ); }

void TextForm::OnLR( wxCommandEvent &WXUNUSED(event) )
{ alignmentRB_[8]->SetValue( true ); }
  
void TextForm::OnFont( wxCommandEvent &WXUNUSED(event) )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *textC = gw->GetTextCharacteristics();
  //
  ExGlobals::SetWorkingFontFlag( false );
  //
  FontChooser fc( this );
  fc.SetFont( GRA_fontControl::GetFontCode(static_cast<GRA_fontCharacteristic*>(textC->Get(wxT("FONT")))->Get()),
              GRA_colorControl::GetColorCode(static_cast<GRA_colorCharacteristic*>(textC->Get(wxT("COLOR")))->Get()),
              static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->GetAsPercent(),
              static_cast<GRA_angleCharacteristic*>(textC->Get(wxT("ANGLE")))->Get() );
  fc.ShowModal();
  //
  if( ExGlobals::GetWorkingFontFlag() )
  {
    static_cast<GRA_fontCharacteristic*>(textC->Get(wxT("FONT")))->Set( ExGlobals::GetWorkingFont() );
    static_cast<GRA_colorCharacteristic*>(textC->Get(wxT("COLOR")))->Set( ExGlobals::GetWorkingFontColor() );
    static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->SetAsPercent( ExGlobals::GetWorkingFontHeight() );
    static_cast<GRA_angleCharacteristic*>(textC->Get(wxT("ANGLE")))->Set( ExGlobals::GetWorkingFontAngle() );
  }
}

void TextForm::OnInteractive( wxCommandEvent &WXUNUSED(event) )
{ SetupInteractive(); }

void TextForm::OnErase( wxCommandEvent &WXUNUSED(event) )
{
  Erase();
}

void TextForm::Erase()
{
  ExGlobals::GetGraphWindow()->RemoveLastTextString();
  ExGlobals::GetwxWindow()->Refresh();
  ExGlobals::GetwxWindow()->Update();
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("FORM TEXT,ERASE") );
}

void TextForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("FORM TEXT,CLOSE") );
  Close();
}

void TextForm::Set( wxString const &text, bool psc, double xloc, double yloc,
                    int alignment, wxString const &action )
{
  stringTC_->SetValue( text );
  interactiveCkB_->SetValue( psc );
  locxTC_->SetValue( wxString() << xloc );
  locyTC_->SetValue( wxString() << yloc );
  alignmentRB_[gwToRadio_[alignment-1]]->SetValue( true );
  if( action == wxT("CLOSE") )
  {
    if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("FORM TEXT,CLOSE") );
    Close();
  }
  else if( action == wxT("ERASE") )
  {
    Erase();
  }
  else if( action == wxT("DRAW" ) )
  {
    fromSet_ = true;
    Draw();
    fromSet_ = false;
  }
}

// end of file
