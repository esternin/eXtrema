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

#include "wx/statline.h"
#include "wx/stattext.h"

#include <wx/persist/toplevel.h>

#include "PeakFindForm.h"
#include "VisualizationWindow.h"
#include "GRA_arrow3.h"
#include "GRA_cartesianCurve.h"
#include "GRA_window.h"
#include "NumericVariable.h"
#include "EVariableError.h"
#include "ExGlobals.h"
#include "GraphicsPage.h"
#include "UsefulFunctions.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_colorControl.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( PeakFindForm, wxFrame )
  EVT_BUTTON( ID_save, PeakFindForm::OnSave )
  EVT_BUTTON( ID_left, PeakFindForm::OnLeft )
  EVT_BUTTON( ID_right, PeakFindForm::OnRight )
  EVT_BUTTON( ID_initialize, PeakFindForm::OnInitialize )
  EVT_BUTTON( wxID_CLOSE, PeakFindForm::OnClose )
  EVT_CLOSE( PeakFindForm::CloseEventHandler )
END_EVENT_TABLE()

    //    : wxFrame(parent,wxID_ANY,wxT("Peak Finding"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),

PeakFindForm::PeakFindForm( VisualizationWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Peak Finding")), visualizationWindow_(parent)
{
  wxString imageDir = ExGlobals::GetImagePath();

  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );

  mainSizer->InsertSpacer( 0, 2 );

  wxPanel *topPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
  
  wxPanel *topTopPanel = new wxPanel( topPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topTopSizer = new wxBoxSizer( wxHORIZONTAL );
  
  wxBitmapButton *leftButton = new wxBitmapButton( topTopPanel, ID_left,
                                                   wxBitmap(imageDir+wxT("/leftArrow.GIF"),wxBITMAP_TYPE_GIF) );
  leftButton->SetToolTip( wxT("scan to the left for peaks") );
  topTopSizer->Add( leftButton, wxSizerFlags(0).Border(wxTOP,15) );

  topTopSizer->InsertSpacer( 1, 10 );
  
  wxBitmapButton *rightButton = new wxBitmapButton( topTopPanel, ID_right,
                                                    wxBitmap(imageDir+wxT("/rightArrow.GIF"),wxBITMAP_TYPE_GIF) );
  rightButton->SetToolTip( wxT("scan to the right for peaks") );
  topTopSizer->Add( rightButton, wxSizerFlags(0).Border(wxTOP,15) );

  topTopSizer->InsertSpacer( 3, 10 );

  wxString maxMinArray[] = { wxT("Maxima"), wxT("Minima") };
  wxArrayString maxMin( 2, maxMinArray );
  maxMinRB_ = new wxRadioBox( topTopPanel, wxID_ANY, wxT("Search for"), wxDefaultPosition, wxDefaultSize,
                              maxMin, 2, wxRA_SPECIFY_COLS );
  maxMinRB_->SetSelection( 0 );
  topTopSizer->Add( maxMinRB_, wxSizerFlags(0).Border(wxALL,5) );

  topTopPanel->SetSizer( topTopSizer );
  topSizer->Add( topTopPanel, wxSizerFlags(0).Border(wxALL,10) );

  wxPanel *midTopPanel = new wxPanel( topPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *midTopSizer = new wxBoxSizer( wxHORIZONTAL );

  wxPanel *midTopLeftPanel = new wxPanel( midTopPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *midTopLeftSizer = new wxBoxSizer( wxVERTICAL );
  
  wxPanel *pxPanel = new wxPanel( midTopLeftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *pxSizer = new wxBoxSizer( wxHORIZONTAL );

  pxSizer->Add(
    new wxStaticText(pxPanel,wxID_ANY,wxT("Peak x-coordinate"),wxDefaultPosition,wxDefaultSize,wxALIGN_RIGHT ),
    wxSizerFlags(0).Right().Border(wxTOP,10) );
  pxTC_ = new wxTextCtrl( pxPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
  pxTC_->SetValue( wxT("") );
  pxSizer->Add( pxTC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  pxPanel->SetSizer( pxSizer );
  midTopLeftSizer->Add( pxPanel, wxSizerFlags(0).Center().Border(wxALL,2) );

  wxPanel *pyPanel = new wxPanel( midTopLeftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *pySizer = new wxBoxSizer( wxHORIZONTAL );

  pySizer->Add(
    new wxStaticText(pyPanel,wxID_ANY,wxT("Peak y-coordinate"),wxDefaultPosition,wxDefaultSize,wxALIGN_RIGHT ),
    wxSizerFlags(0).Right().Border(wxTOP,10) );
  pyTC_ = new wxTextCtrl( pyPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
  pyTC_->SetValue( wxT("") );
  pySizer->Add( pyTC_, wxSizerFlags(0).Left().Border(wxALL,5) );

  pyPanel->SetSizer( pySizer );
  midTopLeftSizer->Add( pyPanel, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  midTopLeftPanel->SetSizer( midTopLeftSizer );
  midTopSizer->Add( midTopLeftPanel, wxSizerFlags(0).Left().Border(wxALL,2) );

  wxButton *saveButton = new wxButton( midTopPanel, ID_save, wxT("Save"), wxDefaultPosition, wxSize(50,60) );
  saveButton->SetToolTip( wxT("save the currently chosen peak's coordinates to vectors PEAK$X and PEAK$Y") );
  midTopSizer->Add( saveButton, wxSizerFlags(0).Border(wxALL,10) );
  
  midTopPanel->SetSizer( midTopSizer );
  topSizer->Add( midTopPanel, wxSizerFlags(0).Centre().Border(wxALL,2) );

  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(0).Center().Border(wxALL,2) );

  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );
  
  wxButton *initializeButton = new wxButton( bottomPanel, ID_initialize, wxT("Initialize") );
  initializeButton->SetToolTip( wxT("start again with the latest curve drawn") );
  bottomSizer->Add( initializeButton, wxSizerFlags(0).Border(wxALL,10) );
  
  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form and create PEAK$X and PEAK$Y if non-empty") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );

  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );

  SetSizer( mainSizer );

  wxPersistentRegisterAndRestore(this, "PeakFindForm");
  Show( true );

  Initialize();
}

void PeakFindForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  EraseLastArrow();

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
  visualizationWindow_->ZeroPeakFindForm();

  Destroy();
}

void PeakFindForm::OnInitialize( wxCommandEvent &WXUNUSED(event) )
{
  EraseLastArrow();
  xData_.clear();
  yData_.clear();
  Initialize();
}

void PeakFindForm::Initialize()
{
  first_ = true;
  currentArrow_ = 0;
  GRA_window *gw = ExGlobals::GetGraphWindow();
  std::vector<GRA_drawableObject*> objs( gw->GetDrawableObjects() );
  curve_ = 0;
  int size = objs.size();
  for( int i=size-1; i>=0; --i )
  {
    if( objs[i]->IsaCartesianCurve() )
    {
      curve_ = static_cast<GRA_cartesianCurve*>(objs[i]);
      break;
    }
  }
  if( curve_ )
  {
    xData_.assign( curve_->GetXData().begin(), curve_->GetXData().end() );
    yData_.assign( curve_->GetYData().begin(), curve_->GetYData().end() );
    npts_ = xData_.size();
    //
    GRA_setOfCharacteristics *xaxis = gw->GetXAxisCharacteristics();
    xLogBase_ = static_cast<GRA_doubleCharacteristic*>(xaxis->Get(wxT("LOGBASE")))->Get();
    xMin_ = static_cast<GRA_doubleCharacteristic*>(xaxis->Get(wxT("MIN")))->Get();
    xMax_ = static_cast<GRA_doubleCharacteristic*>(xaxis->Get(wxT("MAX")))->Get();
    GRA_setOfCharacteristics *yaxis = gw->GetYAxisCharacteristics();
    double ylaxis = static_cast<GRA_distanceCharacteristic*>(yaxis->Get(wxT("LOWERAXIS")))->GetAsWorld();
    double yuaxis = static_cast<GRA_distanceCharacteristic*>(yaxis->Get(wxT("UPPERAXIS")))->GetAsWorld();
    yLogBase_ = static_cast<GRA_doubleCharacteristic*>(yaxis->Get(wxT("LOGBASE")))->Get();
    yMin_ = static_cast<GRA_doubleCharacteristic*>(yaxis->Get(wxT("MIN")))->Get();
    yMax_ = static_cast<GRA_doubleCharacteristic*>(yaxis->Get(wxT("MAX")))->Get();
    arrowLength_ = 0.05*(yuaxis-ylaxis);
    yRange_ = fabs( yMax_-yMin_ );
  }
}

void PeakFindForm::EraseLastArrow()
{
  if( currentArrow_ )
  {
    GraphicsDC dc( ExGlobals::GetGraphicsPage() );
    currentArrow_->SetLineColor( GRA_colorControl::GetColor(wxT("WHITE")) );
    currentArrow_->Draw( ExGlobals::GetGraphicsOutput(), dc );
    delete currentArrow_;
    currentArrow_ = 0;
  }
}

void PeakFindForm::DrawArrow()
{
  if( currentArrow_ )
  {
    GraphicsDC dc( ExGlobals::GetGraphicsPage() );
    currentArrow_->SetLineColor( GRA_colorControl::GetColor(wxT("RED")) );
    currentArrow_->Draw( ExGlobals::GetGraphicsOutput(), dc );
  }
}

void PeakFindForm::OnSave( wxCommandEvent &WXUNUSED(event) )
{
  xVec_.push_back( xLogBase_>1.0 ? pow(xLogBase_,xCurrent_) : xCurrent_ );
  yVec_.push_back( yLogBase_>1.0 ? pow(yLogBase_,yCurrent_) : yCurrent_ );
}

void PeakFindForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ 
  if( !xVec_.empty() )
  {
    try
    {
      wxString name;
      name = wxT("PEAK$X");
      NumericVariable::PutVariable( name, xVec_, 0, wxT("from PEAK FIND gui") );
      name = wxT("PEAK$Y");
      NumericVariable::PutVariable( name, yVec_, 0, wxT("from PEAK FIND gui") );
    }
    catch ( EVariableError &e )
    {
      wxMessageBox( wxString(e.what(),wxConvUTF8),
                    wxT("Fatal error"), wxOK|wxICON_INFORMATION, this );
      return;
    }  
  }
  
  Close();
}
  
void PeakFindForm::OnLeft( wxCommandEvent &WXUNUSED(event) )
{
  if( !curve_ )
  {
    wxMessageBox( wxT("There is no curve drawn in the current window"),
                  wxT("Fatal error"), wxOK|wxICON_INFORMATION, this );
    return;
  }
  EraseLastArrow();
  if( first_ )
  {
    xCurrent_ = xMin_;
    yCurrent_ = yMin_;
    first_ = false;
  }
  double xLo = xCurrent_ - 0.002*(xMax_-xMin_);
  double xHi = xCurrent_ + 0.002*(xMax_-xMin_);
  if( xLogBase_ > 1.0 )
  {
    xHi = pow(xLogBase_,xHi);
    xLo = pow(xLogBase_,xLo);
  }
  int searchType = maxMinRB_->GetSelection()==0 ? 1 : -1;
  //
  // Find the next peak (or minimum) to the left.  Assume that the plotted
  // x-coordinates are monotonically increasing.
  //
  // The following section codes the "tolerance" peak-finding algorithm.
  // Basically, this finds the maximum (or minimum) value of the curve
  // between consecutive crossings of the line Y=YTOL, which is set to
  // a y-value that is sufficiently large (or small) to avoid finding
  // extraneous peaks.
  //
  int nj =0, icr =0, dcr =0;
  bool peakFound = false;
  for( int j=npts_-1; j>=0; --j )
  {
    if( xData_[j] <= xHi )
    {
      if( ++nj > 1 )
      {
        double s = UsefulFunctions::Sign( 1.0001, yData_[j]-yData_[j+1] );
        s > 0 ? icr=j+1 : dcr=j+1; // s>0 -> increasing y, s<0 -> decreasing values
      }
    }
    // Check whether an extremum has been located.  This is done by finding an increasing
    // crossing of the yTolerance value eventually followed by a decreasing crossing for
    // a maximum, or vice-versa for a minimum.  The desired extremum lies between these
    // two intervals, and is found by selecting the extreme value, maximum or minimum,
    // over this region. icr and dcr refer to the j-indices of increasing and decreasing
    // crossing respectively, and are initialized to zero.
    //
    if( dcr*icr == 0 )continue;
    //
    // The y=yTolerance crossings bounding the extremum have been found.
    // Check whether these are consistent with the type of extremum
    // (maximum or minimum) desired.
    //
    if( UsefulFunctions::Sign(1,icr-dcr) != searchType )continue;
    //
    // Loop over the points between these crossings and locate the extreme value
    //
    if( searchType == 1 ) // maximum sought
    {
      double ycmax = yData_[icr-1];
      jPeak_ = icr;
      for( int k=icr; k>=dcr+1; --k )
      {
        if( yData_[k-1] > ycmax )
        {
          ycmax = yData_[k-1];
          jPeak_ = k;
        }
      }
      peakFound = true;
      break;
    }
    else  // minimum sought
    {
      double ycmin = yData_[dcr-1];
      jPeak_ = dcr;
      for( int k=dcr; k>=icr+1; --k )
      {
        if( yData_[k-1] < ycmin )
        {
          ycmin = yData_[k-1];
          jPeak_ = k;
        }
      }
      peakFound = true;
      break;
    }
  }
  if( !peakFound )
  {
    wxMessageBox( wxT("no peak found"), wxT("Info"), wxOK|wxICON_INFORMATION, this );
    return;
  }
  // Found a peak at index jPeak_-1
  //
  xCurrent_ = xData_[jPeak_-1];
  yCurrent_ = yData_[jPeak_-1];
  //
  ShowValues();
}
  
void PeakFindForm::OnRight( wxCommandEvent &WXUNUSED(event) )
{
  if( !curve_ )
  {
    wxMessageBox( wxT("There is no curve drawn in the current window"),
                  wxT("Fatal error"), wxOK|wxICON_INFORMATION, this );
    return;
  }
  EraseLastArrow();
  if( first_ )
  {
    xCurrent_ = xMin_;
    yCurrent_ = yMin_;
    first_ = false;
  }
  double xLo = xCurrent_ - 0.002*(xMax_-xMin_);
  double xHi = xCurrent_ + 0.002*(xMax_-xMin_);
  if( xLogBase_ > 1.0 )
  {
    xHi = pow(xLogBase_,xHi);
    xLo = pow(xLogBase_,xLo);
  }
  int searchType = maxMinRB_->GetSelection()==0 ? 1 : -1;
  //
  // Find the next peak (or minimum) to the left.  Assume that the plotted
  // x-coordinates are monotonically increasing.
  //
  // The following section codes the "tolerance" peak-finding algorithm.
  // Basically, this finds the maximum (or minimum) value of the curve
  // between consecutive crossings of the line Y=YTOL, which is set to
  // a y-value that is sufficiently large (or small) to avoid finding
  // extraneous peaks.
  //
  int nj =0, icr =0, dcr =0;
  bool peakFound = false;
  for( int j=0; j<npts_; ++j )
  {
    if( xData_[j] >= xLo )
    {
      if( ++nj > 1 )
      {
        double s = UsefulFunctions::Sign( 1.0001, yData_[j]-yData_[j-1] );
        s > 0 ? icr=j+1 : dcr=j+1; // s>0 -> increasing y, s<0 -> decreasing values
      }
    }
    // Check whether an extremum has been located.  This is done by finding an increasing
    // crossing of the yTolerance value eventually followed by a decreasing crossing for
    // a maximum, or vice-versa for a minimum.  The desired extremum lies between these
    // two intervals, and is found by selecting the extreme value, maximum or minimum,
    // over this region. icr and dcr refer to the j-indices of increasing and decreasing
    // crossing respectively, and are initialized to zero.
    //
    if( dcr*icr == 0 )continue;
    //
    // The y=yTolerance crossings bounding the extremum have been found.
    // Check whether these are consistent with the type of extremum
    // (maximum or minimum) desired.
    //
    if( UsefulFunctions::Sign(1,dcr-icr) != searchType )continue;
    //
    // Loop over the points between these crossings and locate the extreme value
    //
    if( searchType == 1 ) // maximum sought
    {
      double ycmax = yData_[icr-1];
      jPeak_ = icr;
      for( int k=icr; k<=dcr-1; ++k )
      {
        if( yData_[k-1] > ycmax )
        {
          ycmax = yData_[k-1];
          jPeak_ = k;
        }
      }
      peakFound = true;
      break;
    }
    else  // minimum sought
    {
      double ycmin = yData_[dcr-1];
      jPeak_ = dcr;
      for( int k=dcr; k<=icr-1; ++k )
      {
        if( yData_[k-1] < ycmin )
        {
          ycmin = yData_[k-1];
          jPeak_ = k;
        }
      }
      peakFound = true;
      break;
    }
  }
  if( !peakFound )
  {
    wxMessageBox( wxT("no peak found"), wxT("Info"), wxOK|wxICON_INFORMATION, this );
    return;
  }
  // Found a peak at index jPeak_-1
  //
  xCurrent_ = xData_[jPeak_-1];
  yCurrent_ = yData_[jPeak_-1];
  //
  ShowValues();
}

void PeakFindForm::ShowValues()
{
  pxTC_->SetValue( wxString()<<xCurrent_ );
  pyTC_->SetValue( wxString()<<yCurrent_ );
  //
  double xw, yw;
  ExGlobals::GetGraphWindow()->GraphToWorld( xCurrent_, yCurrent_, xw, yw, true );
  if( xLogBase_ > 1.0 )xCurrent_ = log(xCurrent_)/log(xLogBase_);
  if( yLogBase_ > 1.0 )yCurrent_ = log(yCurrent_)/log(yLogBase_);
  if( xMin_<=xCurrent_ && xCurrent_<=xMax_ )
  {
    EraseLastArrow();
    if( maxMinRB_->GetSelection() == 0 ) // maxima
      currentArrow_ = new GRA_arrow3(xw,yw,xw,yw+arrowLength_,false);
    else
      currentArrow_ = new GRA_arrow3(xw,yw,xw,yw-arrowLength_,false);
    DrawArrow();
  }
  else
  {
    wxMessageBox( wxT("extremum found outside plot range"),
                  wxT("Info"), wxOK|wxICON_INFORMATION, this );
    return;
  }
}

// end of file
