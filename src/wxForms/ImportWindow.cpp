/*
Copyright (C) 2008 Joseph L. Chuma, TRIUMF

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

#include "wx/config.h"
#include "wx/dcbuffer.h"

#include "ImportWindow.h"
#include "NumericVariable.h"
#include "EVariableError.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( ImportWindow, wxFrame )
  EVT_CLOSE( ImportWindow::CloseEventHandler )
  EVT_BUTTON( ID_digitize, ImportWindow::OnDigitize )
  EVT_BUTTON( wxID_CLOSE, ImportWindow::OnClose )
  EVT_PAINT( ImportWindow::OnPaint )
  EVT_LEFT_DOWN( ImportWindow::OnMouseLeftDown )
END_EVENT_TABLE()

ImportWindow::ImportWindow( wxWindow *parent, wxImage &image )
    : wxFrame( parent,wxID_ANY,wxT("Import Window"),
               wxDefaultPosition,wxDefaultSize, wxDEFAULT_FRAME_STYLE )
{
  digitizing_ = false;
  picking_ = false;

  digitizeForm_ = 0;
  bitmap_ = new wxBitmap( image );
  
  //if( !bitmap_->IsOk() )std::cerr << "bitmap not OK\n";
    
  int ulx = 0;
  int uly = 0;
  int width = bitmap_->GetWidth() + 5;
  int height = bitmap_->GetHeight() + 50;
  
  //std::cout << "width=" << width << ", height=" << height << "\n";
  
  SetSize( ulx, uly, width, height );
  
  // for a vertical sizer:
  // proportion = 0    means no vertical expansion
  // proportion > 0    allows for vertical expansion
  // Expand()          allows for horizontal expansion
  
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
  
  wxPanel *panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxRAISED_BORDER|wxTAB_TRAVERSAL );
  wxBoxSizer *panelSizer = new wxBoxSizer( wxHORIZONTAL );
  wxButton *digitizeButton = new wxButton( panel, ID_digitize, wxT("Digitize"),
                                           wxDefaultPosition, wxDefaultSize );
  panelSizer->Add( digitizeButton, wxSizerFlags(0).Border(wxTOP|wxLEFT|wxRIGHT,5) );
  
  wxButton *closeButton = new wxButton( panel, wxID_CLOSE, wxT("Close"),
                                        wxDefaultPosition, wxDefaultSize );
  panelSizer->Add( closeButton, wxSizerFlags(0).Border(wxTOP|wxLEFT|wxRIGHT,5).Right() );
  
  panel->SetSizer( panelSizer );
  
  sizer->Add( panel, wxSizerFlags(0).Expand().Border(wxALL,1) );
  
  window_ = new wxWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE );
  window_->SetBackgroundColour( *wxBLACK );
  
  window_->SetCursor( *wxCROSS_CURSOR );
  window_->SetClientSize( width, height );
  
  sizer->Add( window_, wxSizerFlags(1).Expand().Border(wxALL,1) );
  SetSizer( sizer );
  
  // Show the window.
  // Frames, unlike simple controls, are not shown when created initially.
  Show( true );
}

void ImportWindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
  wxPaintDC dc( window_ );
  dc.Clear();
  dc.DrawBitmap( *bitmap_, 0, 0 );
}

void ImportWindow::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void ImportWindow::OnDigitize( wxCommandEvent &WXUNUSED(event) )
{
  if( digitizeForm_ )digitizeForm_->Raise();
  else
  {
    digitizeForm_ = new DigitizeForm( this );
    digitizeForm_->Show();
  }
}

void ImportWindow::StartDigitizing()
{ digitizing_ = true; }

void ImportWindow::StopDigitizing()
{ digitizing_ = false; }

void ImportWindow::StartPicking()
{ picking_ = true; }

void ImportWindow::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  delete bitmap_;
  Destroy();
}

void ImportWindow::ZeroDigitizeForm()
{ digitizeForm_ = 0; }

void ImportWindow::OnMouseLeftDown( wxMouseEvent &event )
{

  std::cout << "OnMouseLeftDown\n";

  long xl, yl;
  event.GetPosition( &xl, &yl );
  int x = static_cast<int>(xl);
  int y = static_cast<int>(yl);
  if( picking_ )
  {
    digitizeForm_->SetPicked( x, y );
    picking_ = false;
    std::auto_ptr<GetCoordinates> f( new GetCoordinates(this) );
    int width, height;
    f->GetSize( &width, &height );
    f->SetSize( x, y, width, height );
  }
  else if( digitizing_ )
  {
    digitizeForm_->SetDigitized( x, y );
  }
}

void ImportWindow::SetPoints( double x, double y )
{ if( digitizeForm_ )digitizeForm_->SetPoints(x,y); }

BEGIN_EVENT_TABLE( DigitizeForm, wxFrame )
  EVT_CLOSE( DigitizeForm::CloseEventHandler )
  EVT_BUTTON( wxID_CLOSE, DigitizeForm::OnClose )
  EVT_BUTTON( ID_start, DigitizeForm::OnStartStop )
END_EVENT_TABLE()

DigitizeForm::DigitizeForm( ImportWindow *parent )
    : wxFrame((wxWindow*)parent,wxID_ANY,wxT("Digitize Form"),
              wxDefaultPosition,wxDefaultSize, wxDEFAULT_FRAME_STYLE ),
      parent_(parent)
{
  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/DigitizeForm/UPPERLEFTX"), 10l );
  int uly = config->Read( wxT("/DigitizeForm/UPPERLEFTY"), 10l );
  int width = config->Read( wxT("/DigitizeForm/WIDTH"), 400l );
  int height = config->Read( wxT("/DigitizeForm/HEIGHT"), 270l );
  SetSize( ulx, uly, width, height );
  
  wxFont font( GetFont() );
  font.SetFamily( wxFONTFAMILY_TELETYPE );

  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( mainSizer );
  
  wxPanel *xPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE );
  mainSizer->Add( xPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  wxBoxSizer *xSizer = new wxBoxSizer( wxHORIZONTAL );
  xPanel->SetSizer( xSizer );
  xSizer->Add( new wxStaticText(xPanel,wxID_ANY,wxT("X output vector name:")),
               wxSizerFlags(0).Left().Border(wxTOP,2) );
  xTC_ = new wxTextCtrl( xPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(250,25) );
  xTC_->SetToolTip( wxT("enter the name for the independent vector to be created") );
  xSizer->Add( xTC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *yPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE );
  mainSizer->Add( yPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  wxBoxSizer *ySizer = new wxBoxSizer( wxHORIZONTAL );
  yPanel->SetSizer( ySizer );
  ySizer->Add( new wxStaticText(yPanel,wxID_ANY,wxT("Y output vector name:")),
               wxSizerFlags(0).Left().Border(wxTOP,2) );
  yTC_ = new wxTextCtrl( yPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(250,25) );
  yTC_->SetToolTip( wxT("enter the name for the dependent vector to be created") );
  ySizer->Add( yTC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *fpPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE );
  mainSizer->Add( fpPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  wxBoxSizer *fpSizer = new wxBoxSizer( wxHORIZONTAL );
  fpPanel->SetSizer( fpSizer );
  wxStaticText *fpText = new wxStaticText( fpPanel, wxID_ANY, wxT("First point  :") );
  fpText->SetFont( font );
  fpText->SetSize( 200, wxDefaultCoord );
  fpSizer->Add( fpText, wxSizerFlags(0).Left().Border(wxTOP,2) );
  fpST_ = new wxStaticText( fpPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
                            wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
  fpST_->SetFont( font );
  fpSizer->Add( fpST_,wxSizerFlags(0).Left().Border(wxTOP,2) );
  
  wxPanel *spPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE );
  mainSizer->Add( spPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  wxBoxSizer *spSizer = new wxBoxSizer( wxHORIZONTAL );
  spPanel->SetSizer( spSizer );
  wxStaticText *spText = new wxStaticText( spPanel, wxID_ANY, wxT("Second point :") );
  spText->SetFont( font );
  spText->SetSize( 200, wxDefaultCoord );
  spSizer->Add( spText, wxSizerFlags(0).Left().Border(wxTOP,2) );
  spST_ = new wxStaticText( spPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
                            wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
  spST_->SetFont( font );
  spSizer->Add( spST_,wxSizerFlags(0).Left().Border(wxTOP,2) );
  
  wxPanel *tpPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE );
  mainSizer->Add( tpPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  wxBoxSizer *tpSizer = new wxBoxSizer( wxHORIZONTAL );
  tpPanel->SetSizer( tpSizer );
  wxStaticText *tpText = new wxStaticText( tpPanel, wxID_ANY, wxT("Third point  :") );
  tpText->SetFont( font );
  tpText->SetSize( 200, wxDefaultCoord );
  tpSizer->Add( tpText, wxSizerFlags(0).Left().Border(wxTOP,2) );
  tpST_ = new wxStaticText( tpPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
                            wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
  tpST_->SetFont( font );
  tpSizer->Add( tpST_,wxSizerFlags(0).Left().Border(wxTOP,2) );
  
  wxPanel *numPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE );
  mainSizer->Add( numPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  wxBoxSizer *numSizer = new wxBoxSizer( wxHORIZONTAL );
  numPanel->SetSizer( numSizer );
  wxStaticText *numText = new wxStaticText( numPanel, wxID_ANY, wxT("Number of digitized points :") );
  numText->SetFont( font );
  numSizer->Add( numText, wxSizerFlags(0).Left().Border(wxTOP,2) );
  numST_ = new wxStaticText( tpPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
                             wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
  numST_->SetFont( font );
  numSizer->Add( numST_,wxSizerFlags(0).Left().Border(wxTOP,2) );
  
  wxPanel *lastPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE );
  mainSizer->Add( lastPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  wxBoxSizer *lastSizer = new wxBoxSizer( wxHORIZONTAL );
  lastPanel->SetSizer( lastSizer );
  wxStaticText *lastText = new wxStaticText( lastPanel, wxID_ANY, wxT("Last point digitized :") );
  lastText->SetFont( font );
  lastSizer->Add( lastText, wxSizerFlags(0).Left().Border(wxTOP,2) );
  lastST_ = new wxStaticText( tpPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
                              wxALIGN_RIGHT|wxST_NO_AUTORESIZE );
  lastST_->SetFont( font );
  lastSizer->Add( lastST_,wxSizerFlags(0).Left().Border(wxTOP,2) );

  startStopB_ = new wxButton( this, ID_start, wxT("Start digitizing") );
  mainSizer->Add( startStopB_, wxSizerFlags(0).Centre().Border(wxTOP,10) );

  Show( true );

  digitizing_ = false;
  digitizeInfo_ = 0;
}

void DigitizeForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/DigitizeForm/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/DigitizeForm/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/DigitizeForm/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/DigitizeForm/HEIGHT"), static_cast<long>(height) );
  }
  parent_->ZeroDigitizeForm();
  Destroy();
}

void DigitizeForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void DigitizeForm::ZeroDigitizeInfo()
{ digitizeInfo_ = 0; }

void DigitizeForm::OnStartStop( wxCommandEvent &WXUNUSED(event) )
{

  std::cout << "digitizing_ = " << digitizing_ << "\n";

  if( !digitizing_ )
  {
    nPick_ = 0;
    parent_->StartPicking();
    if( digitizeInfo_ )
    {
      digitizeInfo_->Close();
      digitizeInfo_ = 0;
    }
    digitizeInfo_ = new DigitizeInfo(this);
  }
  else
  {
    parent_->StopDigitizing();
    startStopB_->SetLabel( wxT("Start digitizing") );
    digitizing_ = false;
    if( !xTC_->GetLabel().empty() )
    {
      try
      {
        NumericVariable::PutVariable( xTC_->GetLabel(), xd_, 0, wxT("from digitizing") );
      }
      catch ( EVariableError const &e )
      {
        wxMessageDialog *md = new wxMessageDialog( this, wxString(e.what(),wxConvUTF8),
                                                   wxT("Fatal error"), wxOK|wxICON_INFORMATION );
        md->ShowModal();
        return;
      }
    }
    if( !yTC_->GetLabel().empty() )
    {
      try
      {
        NumericVariable::PutVariable( yTC_->GetLabel(), yd_, 0, wxT("from digitizing") );
      }
      catch ( EVariableError const &e )
      {
        wxMessageDialog *md = new wxMessageDialog( this, wxString(e.what(),wxConvUTF8),
                                                   wxT("Fatal error"), wxOK|wxICON_INFORMATION );
        md->ShowModal();
        return;
      }
    }
  }
}

void DigitizeForm::StartPicking()
{ parent_->StartPicking(); }

void DigitizeForm::StartDigitizing()
{
  digitizing_ = true;
  startStopB_->SetLabel( wxT("Stop digitizing") );
  SetupCoordinateTransform();
  parent_->StartDigitizing();
  nDigitized_ = 0;
  numST_->SetLabel( wxString()<<nDigitized_ );
  std::vector<double>().swap( xd_ );
  std::vector<double>().swap( yd_ );
}

void DigitizeForm::AbortDigitizing()
{
  startStopB_->SetLabel( wxT("Start digitizing") );
  std::vector<double>().swap( xd_ );
  std::vector<double>().swap( yd_ );
}

void DigitizeForm::SetPicked( int x, int y )
{
  xImage_[nPick_] = x;
  yImage_[nPick_] = y;
}

void DigitizeForm::SetDigitized( int x, int y )
{
  numST_->SetLabel( wxString()<<(++nDigitized_) );
  double xx = a_ + b_*x + c_*y;
  double yy = d_ + e_*x + f_*y;
  lastST_->SetLabel( wxString()<<wxT("(")<<xx<<wxT(",")<<yy<<wxT(")") );
  xd_.push_back(xx);
  yd_.push_back(yy);
}

void DigitizeForm::SetPoints( double x, double y )
{
  if( nPick_ == 3 )return;
  wxString s = wxString(wxT("x = ")) << x << wxT(", y = ") << y;
  switch ( nPick_ )
  {
    case 0:
      fpST_->SetLabel( s );
      break;
    case 1:
      spST_->SetLabel( s );
      break;
    case 2:
      tpST_->SetLabel( s );
      break;
  }
  xUser_[nPick_] = x;
  yUser_[nPick_] = y;
  ++nPick_;
  digitizeInfo_->Next();
}

void DigitizeForm::SetupCoordinateTransform()
{
  double denom = xImage_[0]*yImage_[1] - xImage_[1]*yImage_[0] -
      xImage_[2]*(yImage_[1]-yImage_[0]) + yImage_[2]*(xImage_[1]-xImage_[0]);
  if( denom == 0.0 )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxT("division by zero in CoordinateTransform"),
                                               wxT("Fatal error"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return;
  }
  a_ = (xImage_[2]*(yImage_[0]*xUser_[1] - yImage_[1]*xUser_[0]) -
        yImage_[2]*(xImage_[0]*xUser_[1] - xImage_[1]*xUser_[0]) +
        xUser_[2]*(xImage_[0]*yImage_[1] - xImage_[1]*yImage_[0]))/denom;
  b_ = (-yImage_[0]*xUser_[1] + yImage_[1]*xUser_[0] +
        yImage_[2]*(xUser_[1]-xUser_[0]) - xUser_[2]*(yImage_[1]-yImage_[0]))/denom;
  c_ = (xImage_[0]*xUser_[1] - xImage_[1]*xUser_[0] -
        xImage_[2]*(xUser_[1]-xUser_[0]) + xUser_[2]*(xImage_[1]-xImage_[0]))/denom;
  d_ = (xImage_[2]*(yImage_[0]*yUser_[1] - yImage_[1]*yUser_[0]) -
        yImage_[2]*(xImage_[0]*yUser_[1] - xImage_[1]*yUser_[0]) +
        yUser_[2]*(xImage_[0]*yImage_[1] - xImage_[1]*yImage_[0]))/denom;
  e_ = (-yImage_[0]*yUser_[1] + yImage_[1]*yUser_[0] +
        yImage_[2]*(yUser_[1]-yUser_[0]) - yUser_[2]*(yImage_[1]-yImage_[0]))/denom;
  f_ = (xImage_[0]*yUser_[1] - xImage_[1]*yUser_[0] -
        xImage_[2]*(yUser_[1]-yUser_[0]) + yUser_[2]*(xImage_[1]-xImage_[0]))/denom;
}

BEGIN_EVENT_TABLE( DigitizeInfo, wxFrame )
  EVT_CLOSE( DigitizeInfo::CloseEventHandler )
  EVT_BUTTON( ID_abort, DigitizeInfo::OnAbort )
END_EVENT_TABLE()

DigitizeInfo::DigitizeInfo( DigitizeForm *parent )
    : wxFrame((wxWindow*)parent,wxID_ANY,wxT("Digitize Info"),
              wxDefaultPosition,wxDefaultSize, wxDEFAULT_FRAME_STYLE ),
      parent_(parent)
{

  std::cout << "DigitizeInfo constructor\n";

  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/DigitizeInfo/UPPERLEFTX"), 10l );
  int uly = config->Read( wxT("/DigitizeInfo/UPPERLEFTY"), 10l );
  int width = config->Read( wxT("/DigitizeInfo/WIDTH"), 500l );
  int height = config->Read( wxT("/DigitizeInfo/HEIGHT"), 150l );
  SetSize( ulx, uly, width, height );
  
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( mainSizer );
  
  wxPanel *infoPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxBORDER_NONE );
  mainSizer->Add( infoPanel, wxSizerFlags(1).Expand().Border(wxALL,5) );
  wxBoxSizer *infoSizer = new wxBoxSizer( wxHORIZONTAL );
  infoPanel->SetSizer( infoSizer );

  infoST_ = new wxStaticText( infoPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
                              wxALIGN_LEFT|wxST_NO_AUTORESIZE );
  infoSizer->Add( infoST_, wxSizerFlags(0).Left().Border(wxTOP,2) );

  abortB_ = new wxButton( this, ID_abort, wxT("Abort") );
  mainSizer->Add( abortB_, wxSizerFlags(0).Centre().Border(wxTOP,10) );

  wxString label( wxT("Click with the left mouse button on a location\n") );
  label += wxT("on the imported drawing where you know the coordinates\n");
  label += wxT("then enter the coordinates on the pop-up form");
  infoST_->SetLabel( label );

  nPoints_ = 0;

  Show( true );

  std::cout << "DigitizeInfo constructor finished\n";

}

void DigitizeInfo::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/DigitizeInfo/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/DigitizeInfo/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/DigitizeInfo/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/DigitizeInfo/HEIGHT"), static_cast<long>(height) );
  }
  parent_->ZeroDigitizeInfo();
  Destroy();
}

void DigitizeInfo::OnAbort( wxCommandEvent &WXUNUSED(event) )
{
  nPoints_ == 3 ? parent_->StartDigitizing() : parent_->AbortDigitizing();
  Close();
}

void DigitizeInfo::Next()
{
  switch (nPoints_++)
  {
    case 0:
      infoST_->SetLabel( wxString() << wxT("Next, click with the left mouse button on another\n")
                         << wxT("location on the imported drawing where you know the coordinates") );
      parent_->StartPicking();
      break;
    case 1:
      infoST_->SetLabel( wxString() << wxT("Finally, click with the left mouse button on a\n")
              << wxT("non-colinear location on the imported drawing where you know the coordinates") );
      parent_->StartPicking();
      break;
    case 2:
      infoST_->SetLabel( wxString() << wxT("Now, close this form and then digitize by clicking\n")
              << wxT("with the left mouse button on the imported drawing") );
      abortB_->SetLabel( wxT("Close") );
      break;
  }
}

BEGIN_EVENT_TABLE( GetCoordinates, wxFrame )
  EVT_CLOSE( GetCoordinates::CloseEventHandler )
  EVT_BUTTON( wxID_OK, GetCoordinates::OnOK )
END_EVENT_TABLE()

GetCoordinates::GetCoordinates( ImportWindow *parent )
    : wxFrame((wxWindow*)parent,wxID_ANY,wxT("Get Coordinates"),
              wxDefaultPosition,wxDefaultSize, wxDEFAULT_FRAME_STYLE ),
      parent_(parent)
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( mainSizer );
  
  wxPanel *xPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE );
  mainSizer->Add( xPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  wxBoxSizer *xSizer = new wxBoxSizer( wxHORIZONTAL );
  xPanel->SetSizer( xSizer );
  xSizer->Add( new wxStaticText(xPanel,wxID_ANY,wxT("X :")), wxSizerFlags(0).Left().Border(wxTOP,2) );
  xTC_ = new wxTextCtrl( xPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(250,25) );
  xTC_->SetToolTip( wxT("enter the x coordinate value") );
  xSizer->Add( xTC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  wxPanel *yPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE );
  mainSizer->Add( yPanel, wxSizerFlags(0).Left().Border(wxALL,5) );
  wxBoxSizer *ySizer = new wxBoxSizer( wxHORIZONTAL );
  yPanel->SetSizer( ySizer );
  ySizer->Add( new wxStaticText(yPanel,wxID_ANY,wxT("Y :")), wxSizerFlags(0).Left().Border(wxTOP,2) );
  yTC_ = new wxTextCtrl( yPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(250,25) );
  yTC_->SetToolTip( wxT("enter the y coordinate value") );
  ySizer->Add( yTC_, wxSizerFlags(0).Left().Border(wxALL,2) );

  wxButton *okB = new wxButton( this, wxID_OK, wxT("OK") );
  mainSizer->Add( okB, wxSizerFlags(0).Centre().Border(wxTOP,10) );

  Show( true );
}

void GetCoordinates::OnOK( wxCommandEvent &WXUNUSED(event) )
{
  double x, y;
  if( !GetValues(x,y) )return;
  parent_->SetPoints( x, y );
  Close();
}

bool GetCoordinates::GetValues( double &x, double &y )
{
  if( xTC_->GetValue().empty() )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxT("Please enter an x-coordinate value"),
                                               wxT("Error"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return false;
  }
  if( yTC_->GetValue().empty() )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxT("Please enter an y-coordinate value"),
                                               wxT("Error"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return false;
  }
  if( !xTC_->GetValue().ToDouble(&x) )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxT("invalid number entered as x-coordinate"),
                                               wxT("Error"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return false;
  }
  if( !yTC_->GetValue().ToDouble(&y) )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxT("invalid number entered as y-coordinate"),
                                               wxT("Error"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return false;
  }
  return true;
}

void GetCoordinates::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{ Destroy(); }

// end of file
