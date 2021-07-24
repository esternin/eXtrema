/*
Copyright (C) 2010 Joseph L. Chuma

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

#include <wx/persist/toplevel.h>

#include "ThreeDPlotForm.h"
#include "GRA_threeDPlot.h"
#include "VisualizationWindow.h"
#include "NumericVariable.h"
#include "EVariableError.h"
#include "NVariableTable.h"
#include "ExGlobals.h"
#include "GraphicsPage.h"
#include "GRA_window.h"
#include "GRA_intCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_colorControl.h"
#include "GRA_setOfCharacteristics.h"
#include "EGraphicsError.h"
#include "GRA_drawableText.h"
#include "UsefulFunctions.h"
#include "GraphicsPage.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( ThreeDPlotForm, wxFrame )
  EVT_TEXT( ID_angleInc, ThreeDPlotForm::OnAngleIncChange )
  EVT_TEXT( ID_eye2obj, ThreeDPlotForm::OnEye2ObjChange )
  EVT_BUTTON( ID_draw, ThreeDPlotForm::OnDraw )
  EVT_BUTTON( ID_left, ThreeDPlotForm::OnLeft )
  EVT_BUTTON( ID_right, ThreeDPlotForm::OnRight )
  EVT_BUTTON( ID_up, ThreeDPlotForm::OnUp )
  EVT_BUTTON( ID_down, ThreeDPlotForm::OnDown )
  EVT_BUTTON( ID_zoomIn, ThreeDPlotForm::OnZoomIn )
  EVT_BUTTON( ID_zoomOut, ThreeDPlotForm::OnZoomOut )
  EVT_BUTTON( wxID_CLOSE, ThreeDPlotForm::OnClose )
  EVT_CLOSE( ThreeDPlotForm::CloseEventHandler )
END_EVENT_TABLE()

ThreeDPlotForm::ThreeDPlotForm( VisualizationWindow *parent )
  : wxFrame(parent,wxID_ANY,wxT("3D Plot"),wxDefaultPosition,wxSize(-1,-1),
            wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL),
  visualizationWindow_(parent)
{
  CreateForm();
  
  wxPersistentRegisterAndRestore(this, "3DPlotForm");
  Show( true );
  
  UpdateVectors( xVecComboBox_ );
  UpdateVectors( yVecComboBox_ );
  UpdateVectors( zVecComboBox_ );

  wxConfigBase *config = wxConfigBase::Get();
  wxString sTmp;
  config->Read( wxT("/3DPlotForm/X_VECTOR"), &sTmp, wxT("") );
  if( xVecComboBox_->FindString(sTmp) > 0 )xVecComboBox_->SetStringSelection( sTmp );
  sTmp.clear();
  config->Read( wxT("/3DPlotForm/Y_VECTOR"), &sTmp, wxT("") );
  if( yVecComboBox_->FindString(sTmp) > 0 )yVecComboBox_->SetStringSelection( sTmp );
  sTmp.clear();
  config->Read( wxT("/3DPlotForm/Z_VECTOR"), &sTmp, wxT("") );
  if( zVecComboBox_->FindString(sTmp) > 0 )zVecComboBox_->SetStringSelection( sTmp );
  sTmp.clear();

  config->Read( wxT("/3DPlotForm/ANGLE_INC"), &sTmp, wxT("5") );
  angleIncTextCtrl_->ChangeValue( sTmp );
  angleIncTextCtrl_->GetValue().ToDouble( &angleIncrement_ );
  sTmp.clear();
  config->Read( wxT("/3DPlotForm/EYE_TO_OBJECT"), &sTmp, wxT("100") );
  eye2objTextCtrl_->ChangeValue( sTmp );
}

void ThreeDPlotForm::CreateForm()
{
  wxString imageDir = ExGlobals::GetImagePath();

  SetSizeHints( wxDefaultSize, wxDefaultSize );
  
  wxPanel* const mainPanel = new wxPanel(this);
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  
  wxPanel *topPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition,
                                   wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
  
  wxBoxSizer *xVecSizer = new wxBoxSizer( wxHORIZONTAL );
  wxStaticText *xVecText = new wxStaticText( topPanel, wxID_ANY, wxT("X"),
                                             wxDefaultPosition, wxDefaultSize, 0 );
  xVecText->Wrap( -1 );
  xVecSizer->Add( xVecText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
  xVecComboBox_ = new wxComboBox( topPanel, wxID_ANY, wxEmptyString,
                                  wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
  xVecSizer->Add( xVecComboBox_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
  topSizer->Add( xVecSizer, 0, wxALIGN_CENTER_HORIZONTAL, 1 );
  
  wxBoxSizer *yVecSizer = new wxBoxSizer( wxHORIZONTAL );
  wxStaticText *yVecText = new wxStaticText( topPanel, wxID_ANY, wxT("Y"),
                                             wxDefaultPosition, wxDefaultSize, 0 );
  yVecText->Wrap( -1 );
  yVecSizer->Add( yVecText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
  yVecComboBox_ = new wxComboBox( topPanel, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                  wxDefaultSize, 0, NULL, 0 ); 
  yVecSizer->Add( yVecComboBox_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
  topSizer->Add( yVecSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxTOP, 5 );
  
  wxBoxSizer* zVecSizer = new wxBoxSizer( wxHORIZONTAL );
  wxStaticText *zVecText = new wxStaticText( topPanel, wxID_ANY, wxT("Z"),
                                             wxDefaultPosition, wxDefaultSize, 0 );
  zVecText->Wrap( -1 );
  zVecSizer->Add( zVecText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
  zVecComboBox_ = new wxComboBox( topPanel, wxID_ANY, wxEmptyString,
                                  wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
  zVecSizer->Add( zVecComboBox_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
  topSizer->Add( zVecSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxTOP, 5 );
  
  wxButton *drawButton = new wxButton( topPanel, ID_draw, wxT("Draw"), wxDefaultPosition,
                                       wxDefaultSize, 0 );
  topSizer->Add( drawButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10 );
  
  topPanel->SetSizer( topSizer );
  topPanel->Layout();
  topSizer->Fit( topPanel );
  mainSizer->Add( topPanel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
  
  wxPanel *bottomPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition,
                                      wxDefaultSize, wxNO_BORDER&wxTAB_TRAVERSAL );
  wxBoxSizer* bottomSizer = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* rotateSizer = new wxBoxSizer( wxHORIZONTAL );
  wxBitmapButton *leftButton =
      new wxBitmapButton( bottomPanel, ID_left,
                          wxBitmap(imageDir+wxT("/leftarrow.bmp"),wxBITMAP_TYPE_BMP),
                          wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  leftButton->SetToolTip( wxT("rotate left 1 angle increment") );
  rotateSizer->Add( leftButton, 0, wxALL, 5 );
  
  wxBitmapButton *rightButton =
      new wxBitmapButton( bottomPanel, ID_right,
                          wxBitmap(imageDir+wxT("/rightarrow.bmp"),wxBITMAP_TYPE_BMP),
                          wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  rotateSizer->Add( rightButton, 0, wxALL, 5 );
  
  wxBitmapButton *upButton =
      new wxBitmapButton( bottomPanel, ID_up,
                          wxBitmap(imageDir+wxT("/uparrow.bmp"),wxBITMAP_TYPE_BMP),
                          wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  rotateSizer->Add( upButton, 0, wxALL, 5 );
  
  wxBitmapButton *downButton =
      new wxBitmapButton( bottomPanel, ID_down,
                          wxBitmap(imageDir+wxT("/downarrow.bmp"),wxBITMAP_TYPE_BMP),
                          wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  rotateSizer->Add( downButton, 0, wxALL, 5 );
  
  bottomSizer->Add( rotateSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxTOP, 10 );
  
  wxBoxSizer* zoomSizer = new wxBoxSizer( wxHORIZONTAL );
  wxBitmapButton *zoomInButton =
      new wxBitmapButton( bottomPanel, ID_zoomIn,
                          wxBitmap(imageDir+wxT("/zoomin.bmp"),wxBITMAP_TYPE_BMP),
                          wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  zoomInButton->SetToolTip( wxT("click to zoom in") );
  zoomSizer->Add( zoomInButton, 0, wxALL, 5 );
  
  wxBitmapButton *zoomOutButton =
      new wxBitmapButton( bottomPanel, ID_zoomOut,
                          wxBitmap(imageDir+wxT("/zoomout.bmp"),wxBITMAP_TYPE_BMP),
                          wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
  zoomOutButton->SetToolTip( wxT("click to zoom out") );
  zoomSizer->Add( zoomOutButton, 0, wxALL, 5 );
  
  bottomSizer->Add( zoomSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxTOP, 5 );
  
  wxBoxSizer* inputSizer = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* angleIncSizer = new wxBoxSizer( wxHORIZONTAL );
  wxStaticText *angleIncText =
      new wxStaticText( bottomPanel, wxID_ANY,
                        wxT("             Angle increment (degrees)"),
                        wxDefaultPosition, wxDefaultSize, 0 );
  angleIncText->Wrap( -1 );
  angleIncSizer->Add( angleIncText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
  
  angleIncTextCtrl_ =
      new wxTextCtrl( bottomPanel, ID_angleInc, wxEmptyString, wxDefaultPosition,
                      wxDefaultSize, 0 );
  angleIncTextCtrl_->SetToolTip(
    wxT("enter the angle increment for rotating the image (degrees)") );
  angleIncSizer->Add( angleIncTextCtrl_, 0, wxALIGN_LEFT|wxALL, 5 );
  inputSizer->Add( angleIncSizer, 1, 0, 5 );
  
  wxBoxSizer* eye2objSizer = new wxBoxSizer( wxHORIZONTAL );
  wxStaticText *eye2objText =
    new wxStaticText( bottomPanel, wxID_ANY,
                      wxT("Eye to object distance scale factor"),
                      wxDefaultPosition, wxDefaultSize, 0 );
  eye2objText->Wrap( -1 );
  eye2objSizer->Add( eye2objText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
  
  eye2objTextCtrl_ =
      new wxTextCtrl( bottomPanel, ID_eye2obj, wxEmptyString,
                      wxDefaultPosition, wxDefaultSize, 0 );
  eye2objTextCtrl_->SetToolTip( wxT("Enter the eye to object distance scale factor") );
  eye2objSizer->Add( eye2objTextCtrl_, 0, wxALIGN_LEFT|wxALL, 5 );
  inputSizer->Add( eye2objSizer, 1, 0, 5 );
  
  bottomSizer->Add( inputSizer, 1, 0, 5 );
  
  bottomPanel->SetSizer( bottomSizer );
  bottomPanel->Layout();
  bottomSizer->Fit( bottomPanel );
  mainSizer->Add( bottomPanel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
  
  wxButton *closeButton = new wxButton( mainPanel, wxID_CLOSE, wxT("Close"),
                                        wxDefaultPosition, wxDefaultSize, 0 );
  mainSizer->Add( closeButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
  
  mainPanel->SetSizer( mainSizer );
  Layout();
  mainSizer->Fit( this );
}

void ThreeDPlotForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void ThreeDPlotForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    config->Write( wxT("/ThreeDPlotForm/X_VECTOR"), xVecComboBox_->GetStringSelection() );
    config->Write( wxT("/ThreeDPlotForm/Y_VECTOR"), yVecComboBox_->GetStringSelection() );
    config->Write( wxT("/ThreeDPlotForm/Z_VECTOR"), zVecComboBox_->GetStringSelection() );

    config->Write( wxT("/ThreeDPlotForm/ANGLE_INC"), angleIncTextCtrl_->GetValue() );
    config->Write( wxT("/ThreeDPlotForm/EYE_TO_OBJECT"), eye2objTextCtrl_->GetValue() );
  }
  visualizationWindow_->ZeroThreeDPlotForm();
  Destroy();
}

void ThreeDPlotForm::UpdateVectors( wxComboBox *cb )
{
  cb->Clear();
  NVariableTable *nvTable = NVariableTable::GetTable();
  int entries = nvTable->Entries();
  for( int i=0; i<entries; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry(i);
    if( nv->GetData().GetNumberOfDimensions() == 1 )cb->Append( nv->GetName() );
  }
}

void ThreeDPlotForm::OnDraw( wxCommandEvent &WXUNUSED(event) )
{
  std::vector<double> x, y, z;
  try
  {
    GetDataVectors( x, y, z );
  }
  catch( EVariableError const &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8),
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GraphicsDC dc( ExGlobals::GetGraphicsPage() );
  gw->Clear();
  gw->Erase();
  threeDPlot_ = new GRA_threeDPlot( this, x, y, z );
  double e2o;
  eye2objTextCtrl_->GetValue().ToDouble( &e2o );
  threeDPlot_->SetEye2Object( e2o );
  threeDPlot_->Draw( ExGlobals::GetGraphicsOutput(), dc );
  gw->AddDrawableObject( threeDPlot_ );
}

void ThreeDPlotForm::GetDataVectors( std::vector<double> &x,
                                     std::vector<double> &y,
                                     std::vector<double> &z )
{
  wxString xVecName( xVecComboBox_->GetStringSelection() );
  wxString yVecName( yVecComboBox_->GetStringSelection() );
  wxString zVecName( zVecComboBox_->GetStringSelection() );
  NumericVariable::GetVector( xVecName, wxT("x variable"), x );
  NumericVariable::GetVector( yVecName, wxT("y variable"), y );
  NumericVariable::GetVector( zVecName, wxT("z variable"), z );
  std::size_t npt = std::min( std::min(x.size(),y.size()), z.size() );
  x.resize( npt );
  y.resize( npt );
  z.resize( npt );
}

void ThreeDPlotForm::ReDraw()
{
  if( !threeDPlot_ )return;
  GRA_threeDPlot *tmp = new GRA_threeDPlot( *threeDPlot_ );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GraphicsPage *page =  ExGlobals::GetGraphicsPage();
  GraphicsDC dc( page );
  gw->Clear();
  gw->Erase();
  threeDPlot_ = tmp;
  threeDPlot_->Draw( ExGlobals::GetGraphicsOutput(), dc );
  gw->AddDrawableObject( threeDPlot_ );
  page->Refresh();
}

void ThreeDPlotForm::OnAngleIncChange( wxCommandEvent &WXUNUSED(event) )
{
  if( angleIncTextCtrl_->GetValue().IsEmpty() )return;
  double ai;
  if( !angleIncTextCtrl_->GetValue().ToDouble(&ai) )
  {
    wxMessageBox( wxT("invalid value entered for angle incrment"),
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    angleIncTextCtrl_->ChangeValue( wxString() << angleIncrement_ );
    return;
  }
  angleIncrement_ = ai;
}

void ThreeDPlotForm::OnEye2ObjChange( wxCommandEvent &WXUNUSED(event) )
{
  if( eye2objTextCtrl_->GetValue().IsEmpty() )return;
  double e2o;
  if( !eye2objTextCtrl_->GetValue().ToDouble(&e2o) )
  {
    wxMessageBox( wxT("invalid value entered for eye to object distance factor"),
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    eye2objTextCtrl_->ChangeValue( wxString() << eye2obj_ );
    return;
  }
  if( threeDPlot_ )
  {
    threeDPlot_->SetEye2Object( e2o );
    ReDraw();
  }
}

void ThreeDPlotForm::OnLeft( wxCommandEvent &WXUNUSED(event) )
{
  if( threeDPlot_ )
  {
    threeDPlot_->RotateY( -angleIncrement_ );
    ReDraw();
  }
}

void ThreeDPlotForm::OnRight( wxCommandEvent &WXUNUSED(event) )
{
  if( threeDPlot_ )
  {
    threeDPlot_->RotateY( angleIncrement_ );
    ReDraw();
  }
}

void ThreeDPlotForm::OnUp( wxCommandEvent &WXUNUSED(event) )
{
  if( threeDPlot_ )
  {
    threeDPlot_->RotateX( -angleIncrement_ );
    ReDraw();
  }
}

void ThreeDPlotForm::OnDown( wxCommandEvent &WXUNUSED(event) )
{
  if( threeDPlot_ )
  {
    threeDPlot_->RotateX( angleIncrement_ );
    ReDraw();
  }
}

void ThreeDPlotForm::OnZoomIn( wxCommandEvent &WXUNUSED(event) )
{
  if( threeDPlot_ )
  {
    threeDPlot_->ScaleDS( 1.05 );
    ReDraw();
  }
}

void ThreeDPlotForm::OnZoomOut( wxCommandEvent &WXUNUSED(event) )
{
  if( threeDPlot_ )
  {
    threeDPlot_->ScaleDS( 0.95 );
    ReDraw();
  }
}
